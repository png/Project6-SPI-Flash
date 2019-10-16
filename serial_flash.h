#ifndef SPI_SERIAL_FLASH_H_
#define SPI_SERIAL_FLASH_H_

#include <msp430.h>
#include "spi.h"

typedef struct SerialFlash_Structure {
    char *PortRegister;
    unsigned char BitMask;
} SerialFlash;

#define ENABLE_SERIAL_FLASH *(Component->PortRegister) &= ~Component->BitMask
#define DISABLE_SERIAL_FLASH *(Component->PortRegister) |= Component->BitMask

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Chip Enable for SPI Serial Flash Memory
 * on GPIO board, component U3
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * GPIO      :  P1.4
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
//#define FLASH_MEMORY_U3 3
#define CHIP_ENABLE_U3_BIT				BIT4
#define CHIP_ENABLE_U3_PORT				P1OUT
#define CHIP_ENABLE_U3_DDR				P1DIR

#define CHIP_U3_OUT CHIP_ENABLE_U3_DDR |= CHIP_ENABLE_U3_BIT

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Chip Enable for SPI Serial Flash Memory
 * on GPIO board, component U2
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * GPIO      :  P2.0
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
//#define FLASH_MEMORY_U2 2
#define CHIP_ENABLE_U2_BIT				BIT0
#define CHIP_ENABLE_U2_PORT				P2OUT
#define CHIP_ENABLE_U2_DDR				P2DIR

#define CHIP_U2_OUT CHIP_ENABLE_U2_DDR |= CHIP_ENABLE_U2_BIT

#define FLASH_MEMORY_SIZE 0x10000	// 512 Kbits = 64K bytes
#define FLASH_SECTOR_SIZE 0x1000	// 4 Kbytes
#define FLASH_BLOCK_SIZE 0x8000		// 32 Kbytes
#define TSCE 100 					// chip-erase time limit (milliseconds)
#define TBP 20						// byte-program time (microseconds)

// Device Operation Instructions (see Table 6 in data sheet)
#define READ                0x03
#define HIGH_SPEED_READ     0x0b
#define SECTOR_ERASE        0x20
#define BLOCK_ERASE         0x52
#define CHIP_ERASE          0x60
#define BYTE_PROGRAM        0x02
#define AAI_PROGRAM         0xAF    // Auto Address Increment Program
#define RDSR                0x05    // Read Status Register
#define EWSR                0x50    // Enable-Write-Status-Register
#define WRSR                0x01    // Write-Status-Register
#define WREN                0x06    // Write-Enable
#define WRDI                0x04    // Write-Disable
#define READ_ID             0x90

//Bits of the SPI status  register
#define SR_BUSY                BIT0        // active high, is system busy writing
#define SR_WRITE_ENABLED       BIT1        // active high, is writing allowed
#define SR_BP0                 BIT2        // Block protection first bit
#define SR_BP1                 BIT3        // Block protection second bit
#define SR_AAI                 BIT6        // active high - use AAI programming mode
#define SR_BPL                 BIT7        // controls whether or not BP0 and BP1 bits are read/write (low) or not (high)

// Block protection levels
#define NONE 					0
#define QUARTER_0xC000_0xFFFF 	1
#define HALF_0x8000_0xFFFF 		2
#define FULL 					3

#define HOLD_U3_BIT     BIT2
#define HOLD_U3_PORT    P2OUT
#define HOLD_U3_DIR     P2DIR

#define ENABLE_HOLD_U3 HOLD_U3_PORT &= ~HOLD_U3_BIT
#define DISABLE_HOLD_U3 HOLD_U3_PORT |= HOLD_U3_BIT
#define HOLD_U3_OUT HOLD_U3_DIR |= HOLD_U3_BIT

#define HOLD_U2_BIT     BIT3
#define HOLD_U2_PORT    P2OUT
#define HOLD_U2_DIR     P2DIR

#define ENABLE_HOLD_U2 HOLD_U2_PORT &= ~HOLD_U2_BIT
#define DISABLE_HOLD_U2 HOLD_U2_PORT |= HOLD_U2_BIT
#define HOLD_U2_OUT HOLD_U2_DIR |= HOLD_U2_BIT


#define WRITE_PROTECT_BIT   BIT1
#define WRITE_PROTECT_PORT  P2OUT
#define WRITE_PROTECT_DIR   P2DIR

#define ENABLE_WRITE_PROTECT WRITE_PROTECT_PORT &= ~WRITE_PROTECT_BIT
#define DISABLE_WRITE_PROTECT WRITE_PROTECT_PORT |= WRITE_PROTECT_BIT
#define WRITE_PROTECT_OUT WRITE_PROTECT_DIR |= WRITE_PROTECT_BIT

// Function Protoypes

/*
 * This function initializes the port pins associated with the serial flash components.
 */
void InitializeSerialFlashPortPins();

/*
 * This function will initialize a serial flash component to an associated port register
 * and bit mask (that represents the chip enable for the device).
 */
void InitializeSerialFlashComponent(SerialFlash *Component, char *PortRegister, unsigned char BitMask);

/*
 * This function reads the ID of the flash memory component, Component.
 *
 * Return Value: The ID read from the flash memory component.
 */
unsigned int ReadFlashMemoryID(SerialFlash *Component);

/*
 * This function reads from the status register of the flash memory component, Component.
 *
 * Return Value: The content of the status register.
 */
unsigned char ReadFlashMemoryStatusRegister(SerialFlash *Component);

/*
 * This function writes the value of WriteValue to the status register of the flash memory
 * component, Component.
 */
void WriteFlashMemoryStatusRegister(unsigned char WriteValue,SerialFlash *Component);

/*
 * This function reads from flash memory component Component, beginning at StartAddress,
 * and reads NumberOfDataValues, storing them in DataValuesArray.
 *
 * The input parameter, ReadMode, is used to select between READ mode and HIGH_SPEED_READ mode
 * (both of which corresponds to device commands - see Device Operation Instructions table above).
 */
void ReadFlashMemory(unsigned long StartAddress, unsigned char* DataValuesArray,
 unsigned int NumberOfDataValues, SerialFlash *Component, unsigned char ReadMode);

/*
 * This function sets the block protection of flash memory component, Component, to
 * the value of ProtectionLevel, which can have 1 of 4 values: NONE, QUARTER_0xC000_0xFFFF,
 * HALF_0x8000_0xFFFF, and FULL (see Block protection level table above).
 */
void SetBlockProtection(unsigned char ProtectionLevel, SerialFlash *Component);

/*
 * This function writes the byte, WriteValue, to the flash memory component, Component,
 * at memory location MemoryAddress.
 */
void ByteProgramFlashMemory(unsigned long MemoryAddress, unsigned char WriteValue, SerialFlash *Component);

/*
 * This function writes to flash memory component Component, beginning at StartAddress,
 * and writes NumberOfDataValues, stored in DataValuesArray.
 */
void AAIProgramFlashMemory(unsigned long StartAddress, unsigned char* DataValuesArray,
 unsigned int NumberOfDataValues, SerialFlash *Component);

/*
 * This function will erase flash memory component Component.
 */
void ChipEraseFlashMemory(SerialFlash *Component);

/*
 * This function will erase either a sector or block of flash memory in component Component, starting
 * at address StartAddress.
 * 
 * Note EraseMode = SECTOR_ERASE performs a sector erase, and EraseMode = BLOCK_ERASE performs a block erase
 * (both of which corresponds to device commands - see Device Operation Instructions table above).
 */
void SectorBlockEraseFlashMemory(unsigned long StartAddress, SerialFlash *Component, unsigned char EraseMode);

/*
 * This function is used to determine if flash memory component Component is busy (that is,
 * whether or not the flash memory has completed the current command).
 *
 * Return Value: The return value is the SR_BUSY bit contained in the status register within the flash
 * memory component (see table above that lists bitmasks for bits of status register).
 */
unsigned char FlashMemoryBusy(SerialFlash *Component);

#endif
