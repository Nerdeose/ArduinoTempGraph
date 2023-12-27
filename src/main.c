//
// Created by remco on 26-12-23.
//

#include <util/delay.h>

#include "spi.h"
#include "mcp23s17.h"
#include "main.h"

int main(void)
{
    SPI_MasterInit();
    IO_Init();
    _delay_ms(500);

    while(1)
    {
        IO_Write(0x13, 0x01);
        _delay_ms(1000);
        IO_Write(0x13, 0x00);
        _delay_ms(1000);
    }

    return 0;
}