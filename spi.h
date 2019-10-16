#ifndef USCI_SPI_H
#define USCI_SPI_H

#include <msp430.h>

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * USCI B0 Master-In-Slave-Out (MISO)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * GPIO      :  P1.6
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define USCIB0_MISO_PIN                 6
#define USCIB0_MISO_BIT					BIT6
#define USCIB0_MISO_PORT				P1IN
#define USCIB0_MISO_DDR					P1DIR

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * USCI B0 Master-Out-Slave-In (MOSI)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * GPIO      :  P1.7
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define USCIB0_MOSI_BIT					BIT7
#define USCIB0_MOSI_PORT				P1OUT
#define USCIB0_MOSI_DDR 				P1DIR

#define TOGGLE_MOSI                 USCIB0_MOSI_PORT ^= USCIB0_MOSI_BIT
#define TURN_ON_MOSI                USCIB0_MOSI_PORT |= USCIB0_MOSI_BIT
#define TURN_OFF_MOSI               USCIB0_MOSI_PORT  &= ~USCIB0_MOSI_BIT

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Slave Clock for GPIO Flash Memory Board
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * GPIO      :  P1.5
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define SPI_SCK_BIT					BIT5
#define SPI_SCK_PORT				P1OUT // SPI Slave Clock output
#define SPI_SCK_DDR					P1DIR // SPI Slave Clock direction

#define TOGGLE_SCLK                 SPI_SCK_PORT ^= SPI_SCK_BIT
#define TURN_OFF_SCLK               SPI_SCK_PORT  &= ~SPI_SCK_BIT
#define TURN_ON_SCLK                SPI_SCK_PORT  |= SPI_SCK_BIT

/*
 * This function initializes all hardware and port pins to support SPI.
 */
void InitializeSPI();

/*
 * This function sends the byte, SendValue, using SPI.
 */
void SPISendByte(unsigned char SendValue);

/*
 * This function receives a byte using SPI.
 *
 * Return Value: The byte that is received over SPI.
 */
unsigned char SPIReceiveByte();

#endif
