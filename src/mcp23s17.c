//
// Created by remco on 26-12-23.
//

#include "spi.h"
#include "mcp23s17.h"

#include <util/delay.h>
#include "spi.h"

void IO_Init(void)
{
    // Disable sequantial writing
    IO_Write(0x05, (1 << SEQOP));

    // Set both banks to output
    IO_Write(0x00, 0);
    IO_Write(0x01, 0);
}

void IO_Write(uint8_t address, uint8_t data)
{
    SPI_ChipSelectStart();
    uint8_t opcode = 0x40;
    SPI_MasterTransmit(opcode);
    SPI_MasterTransmit(address);
    SPI_MasterTransmit(data);
    SPI_ChipSelectStop();
}
