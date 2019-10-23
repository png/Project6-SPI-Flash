#include <msp430.h>
#include <stdio.h>
#include "spi.h"
#include "serial_flash.h"
#include "timerA.h"

// Used with the LFSR function
#define DIVISOR_POLYNOMIAL 0xABCD
#define INITIAL_LFSR_STATE 0x0001

#define BUFFER_SIZE 2

#define LFSR_TYPE unsigned int

LFSR_TYPE LFSR(LFSR_TYPE CurrentState, LFSR_TYPE Divisor);

// Local prototypes
void ConfigureClockModule();

// Use for read/write flash memory tests.
#define TRUE 1
#define FALSE 0

int main(void)
{
	SerialFlash U2, U3;
	//volatile unsigned int ID_U2, ID_U3;
	//volatile unsigned char STATUS_U2, STATUS_U3;

	// Stop the watchdog timer, and configure the clock module.
	WDTCTL = WDTPW + WDTHOLD;
    ConfigureClockModule();

    // Initialize port pins.
    InitializeSPI();
    InitializeSerialFlashPortPins();

    // Initialize serial flash components
    InitializeSerialFlashComponent(&U2, (char *) &CHIP_ENABLE_U2_PORT, CHIP_ENABLE_U2_BIT);
    InitializeSerialFlashComponent(&U3, (char *) &CHIP_ENABLE_U3_PORT, CHIP_ENABLE_U3_BIT);
    //STATUS_U3 = ReadFlashMemoryStatusRegister(&U3);

	while (TRUE) {
//	    SPISendByte(READ_ID);
//	    //ID_U3 = ReadFlashMemoryID(&U3);
//	    //ID_U2 = ReadFlashMemoryID(&U2);
//	    //printf("U3 Stat before: %c\n",STATUS_U3);
//	    //WriteFlashMemoryStatusRegister(0x08, &U3);
//	    //STATUS_U3 = ReadFlashMemoryStatusRegister(&U3);
//	    //ByteProgramFlashMemory((unsigned long)0x0080, 0xFF, &U3);
//
//	    STATUS_U3 = ReadFlashMemoryStatusRegister(&U3);
//	    /*
//	     * void ReadFlashMemory(unsigned long StartAddress, unsigned char* DataValuesArray,
//            unsigned int NumberOfDataValues, SerialFlash *Component, unsigned char ReadMode)
//	     */
//	    ReadFlashMemory((unsigned long)0x0064, READ_U3, 1, &U3, READ);
//
//	    ByteProgramFlashMemory((unsigned long)0x0064, 0b01010101, &U3);
//
//	    ReadFlashMemory((unsigned long)0x0064, READ_U3, 1, &U3, READ);
//
//	    __nop();
//
//		TimeDelay(250,1000);

	    SetBlockProtection(NONE, &U3);
	    ChipEraseFlashMemory(&U3);

	    SetBlockProtection(NONE, &U2);
	    /*
	     * Block erase first half, sector the second
	     */
	    SectorBlockEraseFlashMemory(0x0, &U2, BLOCK_ERASE);
	    for (unsigned long i = FLASH_BLOCK_SIZE+1; i < FLASH_MEMORY_SIZE; i+=FLASH_SECTOR_SIZE){ //guaranteed to be even, so we can do this
	        SectorBlockEraseFlashMemory(i, &U2, SECTOR_ERASE);
	    }
	    unsigned int ret = 0;
	    volatile unsigned int checksum_RET_U2 = 0xFFFF;
	    unsigned char readBuffer[BUFFER_SIZE];
	    for (unsigned long i = 0; i < FLASH_MEMORY_SIZE; i+=BUFFER_SIZE){
	        ReadFlashMemory(i, readBuffer, BUFFER_SIZE, &U2, READ);
	        ret = (readBuffer[0] + (readBuffer[1] << 8));
	        checksum_RET_U2 ^= ret;
	    }
	    __nop();

	    /*
	     * LFSR Time
	     */
	    LFSR_TYPE LFSRState = INITIAL_LFSR_STATE;
	    for (unsigned long i = 0; i < FLASH_MEMORY_SIZE; i+=BUFFER_SIZE){ //guaranteed to be even, so we can do this
	        ByteProgramFlashMemory(i, (unsigned char)LFSRState, &U3);
	        ByteProgramFlashMemory(i+1, (unsigned char)(LFSRState>>8), &U3);
	        AAIProgramFlashMemory(i, (unsigned char*)&LFSRState, BUFFER_SIZE, &U2);
//	        ByteProgramFlashMemory(i, (unsigned char)LFSRState, &U2);
//          ByteProgramFlashMemory(i+1, (unsigned char)(LFSRState>>8), &U2);
	        LFSRState = LFSR(LFSRState, DIVISOR_POLYNOMIAL);
	    }

	    __nop();

	    LFSRState = INITIAL_LFSR_STATE;
	    volatile unsigned int checksum_LFSR = 0xFFFF;
	    volatile unsigned int checksum_RET_U3 = 0xFFFF;
	    for (unsigned long i = 0; i < BUFFER_SIZE; i++) readBuffer[i] = (char) 0;
        for (unsigned long i = 0; i < FLASH_MEMORY_SIZE; i+=BUFFER_SIZE){
            checksum_LFSR ^= LFSRState;

            ReadFlashMemory(i, readBuffer, BUFFER_SIZE, &U3, READ);
            ret = (readBuffer[0] + (readBuffer[1] << 8));
            checksum_RET_U3 ^= ret;

            ReadFlashMemory(i, readBuffer, BUFFER_SIZE, &U2, READ);
            ret = (readBuffer[0] + (readBuffer[1] << 8));
            checksum_RET_U2 ^= ret;

            LFSRState = LFSR(LFSRState, DIVISOR_POLYNOMIAL);
        }
        __nop();

	}
	return 0;
}

void ConfigureClockModule()
{
	// Configure Digitally Controlled Oscillator (DCO) using factory calibrations
	DCOCTL  = CALDCO_16MHZ;
	BCSCTL1 = CALBC1_16MHZ;
}

LFSR_TYPE LFSR(LFSR_TYPE CurrentState, LFSR_TYPE Divisor)
{
    volatile LFSR_TYPE dividend;
    volatile LFSR_TYPE lsb;

    /* In general, the polynomial exponents increase from left to right, whereas
     * the bit numbering increases from right to left:
     *
     * Bit:  1 1 1 1 1 1
     *       5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
     *
     *       b b b b b b b b b b b b b b b b, b = 0,1
     *
     * Poly: 0 1 2 3 4 5 6 7 8 9 1 1 1 1 1 1
     *                           0 1 2 3 4 5
     *
     * Thus, for the polynomial, 1 + x^11 + x^13 + x^14 + x^16:
     * Bit:  1 1 1 1 1 1
     *       5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
     *
     *       1 0 0 0 0 0 0 0 0 0 0 1 0 1 1 0
     *
     * Poly: 0 1 2 3 4 5 6 7 8 9 1 1 1 1 1 1
     *                           0 1 2 3 4 5
     */
    dividend = CurrentState;

    /* Get the value of the Least Significant Bit (LSB). Then, right-shift the dividend.
     *
     * The LSB represents the coefficient of x^(n-1) in the polynomial, and
     * right-shifting corresponds to multiplying the polynomial by x.  So, these
     * two operations essentially determine if the coefficient for x^n equals 1.
     */
    lsb = dividend & 1;
    dividend >>= 1;

    /* If the LSB == 1, then perform polynomial division using the XOR
     * operation (which corresponds to bitwise addition).
     *
     * The LSB equal to 1 corresponds to the coefficient for x^n equal to 1,
     * which means that the Divisor should be added to perform polynomial division.
     *
     * For example, if the dividend = x^15 (that is, 0x0001), then right-shifting
     * (that is, multiplying by x) produces dividend = x^16.  So, perform polynomial
     * division:
     *                                 1
     *                                -----
     * 1 + x^11 + x^13 + x^14 + x^16 | x^16
     *                              +  x^16 + 1 + x^11 + x^13 + x^14
     *                                 -----------------------------
     *                                 1 + x^11 + x^13 + x^14
     */
    dividend ^= (-lsb) & Divisor;

    return dividend;
}


