/*
 * RTC.h
 *
 *  Created on: 4/11/2022
 *      Author: German
 */

#ifndef RTC_H_
#define RTC_H_

#include "stm32f4xx.h"

#define  RTC_MODE_1    0
#define  RTC_MODE_0    1

/* Estructura que contiene la configuración mínima necesaria para el manejo del Timer.*/
typedef struct
{
	uint8_t		TIMx_mode; 		// Up or dowm
	uint32_t	TIMx_speed;		// A qué velocidad se incrementa el Timer
	uint32_t	TIMx_period;	// Valor en ms del periodo del Timer
	uint8_t		TIMx_interruptEnable;	// Activa o desactiva el modo interrupción del timer.
}RTC_Config_t;

/* Handler para el Timer*/

typedef struct
{
	TIM_TypeDef			*ptrTIMx; // Timer asociado al RTC
	RTC_Config_t	    TIMx_Config;   // Estructura que apunta a los archivos o registros del periferico
}rtcTimer_Handler_t;

void RTC_Config(RTC_Config_t *ptrRTCHandler);

#endif /* RTC_H_ */
