#include "serial_flash.h"

void InitializeSerialFlashPortPins()
{
    USCIB0_MOSI_DDR |= USCIB0_MOSI_BIT;
}

void InitializeSerialFlashComponent(SerialFlash *Component, char *PortRegister, unsigned char BitMask)
{
    Component->PortRegister = PortRegister;
    Component->BitMask = BitMask;

    // Disable flash memory device.
}

unsigned int ReadFlashMemoryID(SerialFlash *Component)
{
   unsigned int ReturnValue = 0;

   return ReturnValue;
}

unsigned char ReadFlashMemoryStatusRegister(SerialFlash *Component)
{
	unsigned char RegisterValue = 0;

	return RegisterValue;
}

void WriteFlashMemoryStatusRegister(unsigned char WriteValue,SerialFlash *Component)
{

}

void ReadFlashMemory(unsigned long StartAddress, unsigned char* DataValuesArray,
unsigned int NumberOfDataValues, SerialFlash *Component, unsigned char ReadMode)
{

}

void ByteProgramFlashMemory(unsigned long MemoryAddress, unsigned char WriteValue, SerialFlash *Component)
{

}

void AAIProgramFlashMemory(unsigned long StartAddress, unsigned char* DataValuesArray,
unsigned int NumberOfDataValues, SerialFlash *Component)
{

}

void ChipEraseFlashMemory(SerialFlash *Component)
{

}

void SectorBlockEraseFlashMemory(unsigned long StartAddress,SerialFlash *Component, unsigned char EraseMode)
{


}

void SetBlockProtection(unsigned char ProtectionLevel,SerialFlash *Component)
{

}

unsigned char FlashMemoryBusy(SerialFlash *Component)
{
    unsigned char Busy = 0;

    return Busy;
}
