#include "spi.h"

void InitializeSPI()
{	
    SPI_SCK_DDR |= SPI_SCK_BIT;
    USCIB0_MOSI_DDR |= USCIB0_MOSI_BIT;
    USCIB0_MISO_DDR &= ~USCIB0_MISO_BIT;
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
    TURN_OFF_SCLK;
	unsigned char ReceiveValue = 0;
	for(int i = 0; i < 8; i++){
	    //Read output.
	    //volatile int test = USCIB0_MISO_PORT & USCIB0_MISO_BIT;
	    ReceiveValue = ReceiveValue << 1;
	    ReceiveValue |= (USCIB0_MISO_PORT & USCIB0_MISO_BIT) >> (USCIB0_MISO_PIN);
	    TOGGLE_SCLK;
	    TOGGLE_SCLK;
	}

	return ReceiveValue;
}

