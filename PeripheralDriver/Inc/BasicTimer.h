/*
 * BasicTimer.h
 *
 *  Created on: Apr 18, 2022
 *      Author: German Fuentes
 */

#ifndef INC_BASICTIMER_H_
#define INC_BASICTIMER_H_

#include "stm32f4xx.h"

#define BTIMER_MODE_UP		0
#define BTIMER_MODE_DOWN	1

#define BTIMER_SPEED_10us	160
#define BTIMER_SPEED_100us	1600
#define BTIMER_SPEED_1ms	16000


#define BTIMER_SPEED_10us_100	1000
#define BTIMER_SPEED_100us_100	10000
#define BTIMER_SPEED_1ms_100	100000


/* Estructura que contiene la configuración mínima necesaria para el manejo del Timer.*/
typedef struct
{
	uint8_t		TIMx_mode; 		// Up or dowm
	uint32_t	TIMx_speed;		// A qué velocidad se incrementa el Timer
	uint32_t	TIMx_period;	// Valor en ms del periodo del Timer
	uint8_t		TIMx_interruptEnable;	// Activa o desactiva el modo interrupción del timer.
}BasicTimer_Config_t;

/* Handler para el Timer*/
typedef struct
{
	TIM_TypeDef			*ptrTIMx;
	BasicTimer_Config_t	TIMx_Config;   // Estructura que apunta a los archivos o registros del periferico
}BasicTimer_Handler_t;

void BasicTimer_Config(BasicTimer_Handler_t *ptrBTimerHandler);
void BasicTimer2_CallBack(void); /* Esta función debe ser sobre-escrita en el main para que el sistema funcione*/
void BasicTimer3_CallBack(void);
void BasicTimer4_CallBack(void);
void BasicTimer5_CallBack(void);




void startCounterTimer(BasicTimer_Handler_t *ptrTimerConfig);
void StopCounterTimer(BasicTimer_Handler_t *ptrTimerConfig);
//void GPIOxTooglePin (GPIO_Handler_t *pPinHandler)

#endif /* INC_BASICTIMER_H_ */
