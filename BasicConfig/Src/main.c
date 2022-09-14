/**
 ******************************************************************************
 * @file           : main.c
 * @author         : German Antonio Fuentes
 * @brief          : Configuración inicial basica de un proyecto
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
//uint16_t data = 0;
//uint16_t *ptrEjemplo;
//uint32_t ValorPuntero = 0;

BasicTimer_Handler_t  handlerTimerX ={0};
GPIO_Handler_t       handlerBlinkyLed = {0};

uint8_t blinky = 0;
int main(void){
//    Iniciando variables
	handlerBlinkyLed.pGPIOx= GPIOA;
//
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinNumber =        PIN_5;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinMode   =        GPIO_MODE_OUT;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinOPType  =       GPIO_OTYPE_PUSHPULL;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinPuPdControl =   GPIO_PUPDR_NOTHING;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinSpeed =         GPIO_OSPEED_MEDIUM;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinAltFunMode =    AF0;


	GPIO_Config(&handlerBlinkyLed);
//	GPIO_WritePin(&handlerBlinkyLed, SET);

	 handlerTimerX.ptrTIMx = TIM4;
	 handlerTimerX.TIMx_Config.TIMx_mode = BTIMER_MODE_UP; // CUENTA HACIA ARRIBA
	 handlerTimerX.TIMx_Config.TIMx_speed = BTIMER_SPEED_100us; // La velocidad
	 handlerTimerX.TIMx_Config.TIMx_period = 2500;
//	 handlerTimer2.TIMx_Config.TIMx_interruptEnable = 0;
	 BasicTimer_Config(&handlerTimerX);

//	data= 57;
//
//	// Cambiando el valor de data en +32
//	data +=32;
//
//	// A que el puntero apunte
//
//	ptrEjemplo = &data;
//
//	ValorPuntero = (uint32_t) ptrEjemplo;
//
//    *ptrEjemplo +=2;
//
//    // Incrementamos posición de memoria
//
//    ptrEjemplo++;
//
//    *ptrEjemplo= 0xAC;
//
//
//
//
	while(1){

   }

	return 0;

// Call Back del timer
}
void BasicTimer4_CallBack(void){

	blinky = !blinky;

	if(blinky){
		GPIO_WritePin(&handlerBlinkyLed, SET);
	}else{
		GPIO_WritePin(&handlerBlinkyLed, RESET);
	}
}

void BasicTimer3_CallBack(void){

	blinky = !blinky;

	if(blinky){
		GPIO_WritePin(&handlerBlinkyLed, SET);
	}else{
		GPIO_WritePin(&handlerBlinkyLed, RESET);
	}
}


