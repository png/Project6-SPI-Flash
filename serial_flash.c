#include "serial_flash.h"

void InitializeSerialFlashPortPins()
{
    HOLD_U3_OUT;
    HOLD_U2_OUT;
    CHIP_U3_OUT;
    CHIP_U2_OUT;

    //Set write protect
    WRITE_PROTECT_OUT;
    DISABLE_WRITE_PROTECT;
    //Set hold
    DISABLE_HOLD_U2;
    DISABLE_HOLD_U3;

    //CHIP_ENABLE_U2_PORT |= CHIP_ENABLE_U2_BIT;

    //CHIP_ENABLE_U3_PORT |= CHIP_ENABLE_U3_BIT;

}

void InitializeSerialFlashComponent(SerialFlash *Component, char *PortRegister, unsigned char BitMask)
{
    Component->PortRegister = PortRegister;
    Component->BitMask = BitMask;

    // Disable flash memory device.
    DISABLE_SERIAL_FLASH;
}

unsigned int ReadFlashMemoryID(SerialFlash *Component)
{
   unsigned int ReturnValue = 0;

   unsigned char ID[2] = {0x00, 0x00};
   ENABLE_SERIAL_FLASH;

   SPISendByte(READ_ID); // Device Command
   //Address
   SPISendByte(0x00);
   SPISendByte(0x00);
   SPISendByte(0x00);

   //Read mfr and device IDs.
   ID[0] = SPIReceiveByte(); //mfr -- 0xBF
   ID[1] = SPIReceiveByte(); //device -- 0x48

   DISABLE_SERIAL_FLASH;

   ReturnValue = ((unsigned int) ID[0] << 8) + (unsigned int) ID[1];

   return ReturnValue;
}

unsigned char ReadFlashMemoryStatusRegister(SerialFlash *Component)
{
	//unsigned char RegisterValue = 0;

	unsigned char STATUS = 0x00;
    ENABLE_SERIAL_FLASH;

    SPISendByte(RDSR); // Device Command
    //Address
    SPISendByte(0x00);
    SPISendByte(0x00);
    SPISendByte(0x00);

    //Read mfr and device IDs.
    STATUS = SPIReceiveByte(); //status

    DISABLE_SERIAL_FLASH;

	return STATUS;
}

void WriteFlashMemoryStatusRegister(unsigned char WriteValue,SerialFlash *Component)
{
    TURN_OFF_SCLK;
    ENABLE_SERIAL_FLASH;

    SPISendByte(EWSR); // Device Command -- Enable write
    DISABLE_SERIAL_FLASH;
    TOGGLE_SCLK;

    TOGGLE_SCLK;
    ENABLE_SERIAL_FLASH;
    SPISendByte(WRSR); // Switch to write
    SPISendByte(WriteValue); // Give value.

    DISABLE_SERIAL_FLASH;
    TURN_ON_SCLK;

}
/*
 * This function reads from flash memory component Component, beginning at StartAddress,
 * and reads NumberOfDataValues, storing them in DataValuesArray.
 *
 * The input parameter, ReadMode, is used to select between READ mode and HIGH_SPEED_READ mode
 * (both of which corresponds to device commands - see Device Operation Instructions table above).
 */
union LongToChar{
    unsigned long address;
    unsigned char bytes[4];
};

void ReadFlashMemory(unsigned long StartAddress, unsigned char* DataValuesArray,
unsigned int NumberOfDataValues, SerialFlash *Component, unsigned char ReadMode)
{
    ENABLE_SERIAL_FLASH;

    union LongToChar addr;
    addr.address = StartAddress;
    SPISendByte(ReadMode);
//    volatile unsigned char a = addr.bytes[2];
//    volatile unsigned char b = addr.bytes[1];
//    volatile unsigned char c = addr.bytes[0];
    SPISendByte((unsigned char)addr.bytes[2]);
    SPISendByte((unsigned char)addr.bytes[1]);
    SPISendByte((unsigned char)addr.bytes[0]);
    //Ignore addr[0] -- if little endian, contains MSB, which is 00

    if(ReadMode == HIGH_SPEED_READ){
        SPISendByte(0x00); //A nop for hsr
    }

    TURN_OFF_SCLK;

    for(unsigned int i = 0; i < NumberOfDataValues; i++){
        unsigned char c = SPIReceiveByte();
        DataValuesArray[i] = c;
    }

    DISABLE_SERIAL_FLASH;
}

/*
 * This function writes the byte, WriteValue, to the flash memory component, Component,
 * at memory location MemoryAddress.
 */
void ByteProgramFlashMemory(unsigned long MemoryAddress, unsigned char WriteValue, SerialFlash *Component)
{
    //Just to enable write.
    ENABLE_SERIAL_FLASH;
    SPISendByte(WREN);
    DISABLE_SERIAL_FLASH;

    TURN_OFF_SCLK;

    ENABLE_SERIAL_FLASH;

    union LongToChar addr;
    addr.address = MemoryAddress;
    SPISendByte(BYTE_PROGRAM);

    SPISendByte((unsigned char)addr.bytes[2]);
    SPISendByte((unsigned char)addr.bytes[1]);
    SPISendByte((unsigned char)addr.bytes[0]);

//    volatile unsigned char a = addr.bytes[2];
//    volatile unsigned char b = addr.bytes[1];
//    volatile unsigned char c = addr.bytes[0];

    SPISendByte(WriteValue);

    DISABLE_SERIAL_FLASH;

    ENABLE_SERIAL_FLASH;
    SPISendByte(WRDI);
    DISABLE_SERIAL_FLASH;
}

void AAIProgramFlashMemory(unsigned long StartAddress, unsigned char* DataValuesArray,
unsigned int NumberOfDataValues, SerialFlash *Component)
{
//    ENABLE_SERIAL_FLASH;
//    union LongToChar addr;
//    addr.address = StartAddress;
//    SPISendByte(AAI_PROGRAM);
//
//    SPISendByte((unsigned char)addr.bytes[2]);
//    SPISendByte((unsigned char)addr.bytes[1]);
//    SPISendByte((unsigned char)addr.bytes[0]);
//
//    DISABLE_SERIAL_FLASH;
}

void ChipEraseFlashMemory(SerialFlash *Component)
{
    ENABLE_SERIAL_FLASH;
    SetBlockProtection(NONE, Component);
    SPISendByte(CHIP_ERASE);
    char busy;
    do{
        busy = FlashMemoryBusy(Component);
    }while(busy > 0);
    DISABLE_SERIAL_FLASH;
}

void SectorBlockEraseFlashMemory(unsigned long StartAddress,SerialFlash *Component, unsigned char EraseMode)
{
    ENABLE_SERIAL_FLASH;
    //union LongToChar addr;
    //addr.address = StartAddress;
    SPISendByte(BYTE_PROGRAM);

    DISABLE_SERIAL_FLASH;
}
/*
 * This function sets the block protection of flash memory component, Component, to
 * the value of ProtectionLevel, which can have 1 of 4 values: NONE, QUARTER_0xC000_0xFFFF,
 * HALF_0x8000_0xFFFF, and FULL (see Block protection level table above).
 */

void SetBlockProtection(unsigned char ProtectionLevel,SerialFlash *Component)
{
    switch(ProtectionLevel){
         case NONE:
             WriteFlashMemoryStatusRegister(0x00, Component);
             break;
         case QUARTER_0xC000_0xFFFF:
             WriteFlashMemoryStatusRegister(0x04, Component);
             break;
         case HALF_0x8000_0xFFFF:
             WriteFlashMemoryStatusRegister(0x08, Component);
             break;
         case FULL:
             WriteFlashMemoryStatusRegister(0x0c, Component);
             break;
         default:
             break;
    }
}

/*
 * This function is used to determine if flash memory component Component is busy (that is,
 * whether or not the flash memory has completed the current command).
 *
 * Return Value: The return value is the SR_BUSY bit contained in the status register within the flash
 * memory component (see table above that lists bitmasks for bits of status register).
 */
unsigned char FlashMemoryBusy(SerialFlash *Component)
{
    unsigned char Busy = 0;
    Busy = ReadFlashMemoryStatusRegister(Component) & SR_BUSY;

    return Busy;
}
