#ifndef MAIN_H
#define MAIN_H
#include <stm32f4xx.h> // common stuff
#include <stm32f4xx_gpio.h> // gpio control
#include <stm32f4xx_rcc.h> // reset anc clocking
#include <usart.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_l3gd20.h"
#include "arm_math.h"

void Delay(__IO uint32_t nTime);
void TimingDelay_Decrement(void);
#endif
