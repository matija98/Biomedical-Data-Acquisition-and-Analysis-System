#ifndef USART_H
#define USART_H
#include <stm32f4xx.h> // common stuff
#include <stm32f4xx_rcc.h> // reset anc clocking
#include <stm32f4xx_gpio.h> // gpio control
#include <stm32f4xx_usart.h> // USART
#define BUFSIZE1 1024
#define BUFSIZE6 64
#define BAUDRATE 115200
void USART1_Init(void); // init USART1 peripheral
void USART6_Init(void);
int USART1_Dequeue(char* c); // pop character from receive FIFO
void USART1_SendChar(char c); // blocking send character
void USART6_SendChar(char c);
#endif
