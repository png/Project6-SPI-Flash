#include "spi.h"

void InitializeSPI()
{	
    SPI_SCK_DDR |= SPI_SCK_BIT;
}

void SPISendByte(unsigned char SendValue)
{
    TURN_OFF_SCLK;
    unsigned int bits = sizeof(char)*8;
    unsigned char copyVal = SendValue;
    for(int i = bits-1; i >= 0; i--){
        if((copyVal & ( 1 << i )) >> i){ // value is 1
            TURN_ON_MOSI;
        }
        else{ //value is 0
            TURN_OFF_MOSI;
            //
        }
        //toggle sclk on and off
       TOGGLE_SCLK;
       TOGGLE_SCLK;
       //TURN_OFF_MOSI;
    }
    TURN_OFF_MOSI;

}

unsigned char SPIReceiveByte()
{
	unsigned char ReceiveValue = 0;

	return ReceiveValue;
}

