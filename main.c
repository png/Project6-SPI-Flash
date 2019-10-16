#include <msp430.h>
#include <stdio.h>
#include "spi.h"
#include "serial_flash.h"
#include "timerA.h"

// Local prototypes
void ConfigureClockModule();

// Use for read/write flash memory tests.
#define TRUE 1
#define FALSE 0

int main(void)
{
	SerialFlash U2, U3;
	volatile unsigned int ID_U2, ID_U3;
	volatile unsigned char STATUS_U2, STATUS_U3;

	// Stop the watchdog timer, and configure the clock module.
	WDTCTL = WDTPW + WDTHOLD;
    ConfigureClockModule();

    // Initialize port pins.
    InitializeSPI();
    InitializeSerialFlashPortPins();

    // Initialize serial flash components
    InitializeSerialFlashComponent(&U2, (char *) &CHIP_ENABLE_U2_PORT, CHIP_ENABLE_U2_BIT);
    InitializeSerialFlashComponent(&U3, (char *) &CHIP_ENABLE_U3_PORT, CHIP_ENABLE_U3_BIT);

    unsigned char READ_U3[1] = {0x00};

	while (TRUE) {
	    SPISendByte(READ_ID);
	    //ID_U3 = ReadFlashMemoryID(&U3);
	    //ID_U2 = ReadFlashMemoryID(&U2);

        STATUS_U3 = ReadFlashMemoryStatusRegister(&U3);
	    //printf("U3 Stat before: %c\n",STATUS_U3);
	    //WriteFlashMemoryStatusRegister(0x08, &U3);
	    //STATUS_U3 = ReadFlashMemoryStatusRegister(&U3);
	    ChipEraseFlashMemory(&U3);
	    ByteProgramFlashMemory((unsigned long)0x112233, 0xFF, &U3);
	    /*
	     * void ReadFlashMemory(unsigned long StartAddress, unsigned char* DataValuesArray,
            unsigned int NumberOfDataValues, SerialFlash *Component, unsigned char ReadMode)
	     */
	    ReadFlashMemory((unsigned long)0x112233, READ_U3, 1, &U3, READ);

		TimeDelay(250,1000);
	}
	return 0;
}

void ConfigureClockModule()
{
	// Configure Digitally Controlled Oscillator (DCO) using factory calibrations
	DCOCTL  = CALDCO_16MHZ;
	BCSCTL1 = CALBC1_16MHZ;
}


