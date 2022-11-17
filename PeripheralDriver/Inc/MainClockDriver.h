/*
 * MainClockDriver.h
 *
 *  Created on: 14/11/2022
 *      Author: German
 */
#include <stdio.h>

#include "stm32f4xx.h"

#ifndef MAINCLOCKDRIVER_H_
#define MAINCLOCKDRIVER_H_


//#define USART_BAUDRATE_115200_Extra  3

#define PLL_M 	8
#define PLL_N 	100
#define PLL_P 	2



void ResetClock(void);


#endif /* MAINCLOCKDRIVER_H_ */
