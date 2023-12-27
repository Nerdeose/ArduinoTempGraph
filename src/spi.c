//
// Created by remco on 5-11-23.
//

#include "spi.h"

void SPI_MasterInit(void)
{
    // Set MOSI and SCK output, others are input by default
    DDR_SPI = (1<<DD_MOSI) | (1<<DD_SCK) | (1<<CS_PIN);
    // Enable SPI, Master, set clock rate fck/16
    SPCR= (1<<SPE) | (1<<MSTR) | (1<<SPR0);
    CS_PORT = (1 << CS_PIN);
}

void SPI_MasterTransmit(char data)
{
    // Start transmission
    SPDR = data;
    // Wait for transmission to complete
    while (!(SPSR & (1<<SPIF)));
}

void SPI_SlaveInit(void)
{
    // Set MISO to output, others are input by default
    DDR_SPI = (1<<DD_MISO);
    // Enable SPI, Master is disabled by default, clock is set by master
    SPCR = (1<<SPE);
}

char SPI_SlaveReceive(void)
{
    // Wait for reception complete
    while(!(SPSR & (1<<SPIF)));
    // Return data register
    return SPDR;
}

void SPI_ChipSelectStart(void)
{
    CS_PORT = (0 << CS_PIN);
}

void SPI_ChipSelectStop(void)
{
    CS_PORT = (1 << CS_PIN);
}