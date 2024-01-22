//
// Created by remco on 26-12-23.
//



#include <avr/io.h>
#include <stdlib.h>
#include <avr/delay.h>

#define DDR_SPI DDRB
#define DD_MOSI PB3
#define DD_MISO PB4
#define DD_SCK PB5

#define FCPU 16000000
#define BAUD 9600
#define CLOCK (FCPU/16/BAUD-1)

#define MIN_TEMP 15
#define MAX_TEMP 25
#define NR_OF_COLUMNS 8
#define NR_OF_ROWS 8

void SPI_MasterTransmit(char data)
{
    // Start transmission
    SPDR = data;
    // Wait for transmission to complete
    while (!(SPSR & (1<<SPIF)));
}

unsigned char SPI_SlaveReceive(void)
{
    SPDR = 0xFF;
    // Wait for reception complete
    while(!(SPSR & (1<<SPIF)));
    // Return data register
    return SPDR;
}

void USART_Transmit(char *data)
{
    while (*data)
    {
        while(!(UCSR0A & (1<<UDRE0)));
        UDR0 = (*data++);
    }
}

void Temp_Init() {
    PORTB = (1 << PB1);
    SPI_MasterTransmit(0x80);
    SPI_MasterTransmit(0xE8);
    PORTB = (0 << PB1);
}

void Temp_ToSerial(float temp) {
    char buffer[8 * sizeof(temp) + 1];
    dtostrf(temp, 8, 4, buffer);
    USART_Transmit(buffer);
    USART_Transmit("\n\r");
}

void USART_Init(void) {
    UBRR0H = (unsigned char)(CLOCK >> 8);
    UBRR0L = (unsigned char)CLOCK;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
}

void SPI_Init(void) {// Set MOSI and SCK output, others are input by default
    DDR_SPI = (1 << DD_MOSI) | (1 << DD_SCK) | (1 << PB2) | (1 << PB1);
    // Enable SPI, Master, set clock rate fck/16
    SPCR = (1 << SPE) | (0 << DORD) | (1 << MSTR) | (1<<CPOL) | (1 << CPHA) | (1 << SPR1) | (1 << SPR0);
}

void IO_Write(unsigned char address, unsigned char data)
{
    PORTB = (0<<PB2);
    SPI_MasterTransmit(0x40); //opcode
    SPI_MasterTransmit(address);
    SPI_MasterTransmit(data);
    PORTB = (1<<PB2);
    _delay_us(50);
}

int main(void)
{
    USART_Init();
    SPI_Init();


    PORTB = (0<<PB2);
    _delay_ms(1);
    PORTB = (1<<PB2);
    _delay_ms(1);
    PORTB = (1<<PB1);
    _delay_ms(1);
    PORTB = (0<<PB1);
    _delay_ms(1);

    Temp_Init();


    // Write 1 to all possible IOCON registers to put it into a known state of 0.
    IO_Write(0x06, 0);
    IO_Write(0x16, 0);
    IO_Write(0x0B, 0);
    IO_Write(0x0C, 0);


    //Set IOCON to joint banks and disabled sequential writing other settings are default 1.
    IO_Write(0x0B, 0b10100000);


    // Set both banks to output
    IO_Write(0x01, 0);
    IO_Write(0x11, 0);

    IO_Write(0x0A, 0xFF);
    uint8_t rowLight = 0;
    for (int i = 1; i < NR_OF_ROWS ; ++i) {
        IO_Write(0x1A, rowLight);
        rowLight = (rowLight << 2) + 1;
        _delay_ms(21);
    }
    _delay_ms(501);

    float measurements[NR_OF_COLUMNS] = {0};

    while (1) {
        IO_Write(0x09, 1);
        IO_Write(0x19, 1);
        _delay_ms(1000);
        IO_Write(0x09, 0);
        IO_Write(0x19, 0);
        _delay_ms(500);

        // Place this in 5-second timer interrupt routine
        char tempLSB, tempMSB;
        float temp;
        PORTB = (1 << PB1);
        SPI_MasterTransmit(0x01);
        tempLSB = SPI_SlaveReceive();
        tempMSB = SPI_SlaveReceive();
        PORTB = (0 << PB1);

        if (tempMSB & 0x80) {
            tempMSB = ~tempMSB + 1;
            tempLSB = ~tempLSB + 1;
            tempLSB >>= 4;
            temp = -((float)tempMSB) + (((float)tempLSB) / 16.0);
        } else {
            tempLSB >>= 4;
            temp = -((float)tempMSB) + (((float)tempLSB) / 16.0);
        }
        Temp_ToSerial(temp);

        for (int i = NR_OF_COLUMNS-2; i >= 0; ++i) {
           measurements[i] = measurements[i+1];
        }

        measurements[NR_OF_COLUMNS-1] = temp;

        uint8_t convertedMeasurements[NR_OF_COLUMNS] = {0};

        for (int i = 0; i < NR_OF_COLUMNS; ++i) {
            convertedMeasurements[i] = roundf(measurements[i] * ((MAX_TEMP - MIN_TEMP) / NR_OF_ROWS));
        }

        for (int i = 0; i < NR_OF_COLUMNS; ++i) {
            IO_Write(0x09, (1 << i));
            IO_Write(0x19, (1 << convertedMeasurements[i]));
            _delay_ms(10);
            IO_Write(0x09, (0 << i));
            IO_Write(0x19, (0 << convertedMeasurements[i]));
        }






    }
}