//
// Created by remco on 5-11-23.
//

#include <avr/io.h>

#ifndef ARDUINOTEMPGRAPH_SPI_H
#define ARDUINOTEMPGRAPH_SPI_H
#define DDR_SPI DDRB
#define DD_MOSI PB3
#define DD_MISO PB4
#define DD_SCK PB5
#define CS_PORT PORTB
#define CS_PIN  PB2

void SPI_MasterInit(void);
void SPI_MasterTransmit(char data);
void SPI_SlaveInit(void);
char SPI_SlaveReceive(void);
void SPI_ChipSelectStart(void);
void SPI_ChipSelectStop(void);

#endif //ARDUINOTEMPGRAPH_SPI_H
