//
// Created by remco on 26-12-23.
//
#include <avr/io.h>
#include <stdbool.h>

#ifndef ARDUINOTEMPGRAPH_MCP23S17_H
#define ARDUINOTEMPGRAPH_MCP23S17_H

#define SEQOP 5

void IO_Init(void);
void IO_Write(uint8_t address, uint8_t data);

#endif //ARDUINOTEMPGRAPH_MCP23S17_H
