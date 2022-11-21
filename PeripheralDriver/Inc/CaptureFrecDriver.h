/*
 * CaptureFrecDriver.h
 *
 *  Created on: 18/11/2022
 *      Author: German
 */

#ifndef CAPTUREFRECDRIVER_H_
#define CAPTUREFRECDRIVER_H_

#include "stm32f4xx.h"

#define CAPTURE_CHANNEL_1	0
#define CAPTURE_CHANNEL_2	1
#define CAPTURE_CHANNEL_3	2
#define CAPTURE_CHANNEL_4	3

#define CAPTURE_RISING_EDGE		0
#define CAPTURE_FALLING_EDGE	1
#define CAPTURE_BOTH_EDGE		2


#define CAPTURE_PRESCALER_1_1	0
#define CAPTURE_PRESCALER_2_1	1
#define CAPTURE_PRESCALER_4_1	2
#define CAPTURE_PRESCALER_8_1	3


#define CAPTURE_TIMER_SPEED_1us		16
#define CAPTURE_TIMER_SPEED_10us	160
#define CAPTURE_TIMER_SPEED_100us	1600
#define CAPTURE_TIMER_SPEED_1ms		16000


/**/
typedef struct
{
	uint8_t		channel;			// Canal relacionado con el TIMER
	uint8_t		edgeSignal;			// Selección del tipo de flanco que se desea capturar
	uint16_t 	timerSpeed;			// A qué velocidad se incrementa el Timer
	uint8_t		prescalerCapture; 	// 4 posibles prescaler para captura

}Capture_Config_t;


typedef struct
{
	TIM_TypeDef		*ptrTIMx;	// Timer al que está asociada la captura
	Capture_Config_t	config;		// Configuración inicial de la captura

}Capture_Handler_t;

/* Prototipos de las funciones */

void capture_Config (Capture_Handler_t *ptrCaptureHandler);
uint32_t getPeriodFreq (Capture_Handler_t *ptrCaptureHandler);

void capturefrecuencia2(void);
void capturefrecuencia3(void);
void capturefrecuencia4(void);
void capturefrecuencia5(void);

uint32_t getPeriodFrec(Capture_Handler_t  *ptrCaptureHandler, uint32_t Captura1, uint32_t Captura2 );

void CleanCapture(Capture_Handler_t *ptrCaptureHandler);

//uint32_t  getInter (Capture_Handler_t *ptrCaptureHandler);
//
//uint16_t getData(void);

#endif /* CAPTUREFRECDRIVER_H_ */
