#include <msp430.h>
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

	// Stop the watchdog timer, and configure the clock module.
	WDTCTL = WDTPW + WDTHOLD;
    ConfigureClockModule();

    // Initialize port pins.
    InitializeSPI();
    InitializeSerialFlashPortPins();

    // Initialize serial flash components
    InitializeSerialFlashComponent(&U2, (char *) &CHIP_ENABLE_U2_PORT, CHIP_ENABLE_U2_BIT);
    InitializeSerialFlashComponent(&U3, (char *) &CHIP_ENABLE_U3_PORT, CHIP_ENABLE_U3_BIT);

	while (TRUE) {
	    SPISendByte(READ_ID);
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


