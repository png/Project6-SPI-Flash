#include "timerA.h"

void TimeDelay(unsigned int delay, unsigned int resolution)
{
	unsigned int i;

	// Use Timer A1 Control Register

	// Stop the timer and clear register
	TA1CTL = (MC_0 | TACLR);

	// Update the value of Compare-Capture Register 0.
	TA1CCR0 = resolution-1;

	// Restart the timer.
	TA1CTL = (TASSEL_2 | ID_0 | MC_1);

	for (i = 0; i < delay; i++) {

		// Wait for the timer to overflow
		while (!(TA1CTL & TAIFG));

		// Reset the TAIFG.
		TA1CTL &= ~TAIFG;
	}

	// Stop the timer and clear register
	TA1CTL = (MC_0 | TACLR);
}
