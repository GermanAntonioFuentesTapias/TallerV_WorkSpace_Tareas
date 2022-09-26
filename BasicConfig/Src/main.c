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
#include "USARTxDriver.h"
//uint16_t data = 0;
//uint16_t *ptrEjemplo;
//uint32_t ValorPuntero = 0;

int dato = 0;

BasicTimer_Handler_t  handlerTimerX ={0};
GPIO_Handler_t       handlerBlinkyLed = {0};
GPIO_Handler_t       handlerTx = {0};
USART_Handler_t     handlerUsar = {0};

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

// Estte el blinky pai
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
//Listo main pa como asi? R  donde ?
	 //    Iniciando variables
	 	handlerTx.pGPIOx= GPIOB;
	 //
	 	handlerTx.GPIO_PinConfig.GPIO_PinNumber =        PIN_6;
	 	handlerTx.GPIO_PinConfig.GPIO_PinMode   =        GPIO_MODE_ALTFN; // ah mk jajajaj, no olvidar colocar funcion alternativa ome hueva
	 	handlerTx.GPIO_PinConfig.GPIO_PinOPType  =       GPIO_OTYPE_PUSHPULL;
	 	handlerTx.GPIO_PinConfig.GPIO_PinPuPdControl =   GPIO_PUPDR_NOTHING;
	 	handlerTx.GPIO_PinConfig.GPIO_PinSpeed =         GPIO_OSPEED_MEDIUM;
	 	handlerTx.GPIO_PinConfig.GPIO_PinAltFunMode =    AF7;

//Listo sigamos viendo pai, mera hueva ome, R

	 	GPIO_Config(&handlerTx);
	 //	GPIO_WritePin(&handlerBlinkyLed, SET);
// magia

	 	// Configuración Usart

	 	handlerUsar.ptrUSARTx = USART1 ;
//Yo entiendo ome

	 	handlerUsar.USART_Config.USART_mode =       USART_MODE_TX;
	    handlerUsar.USART_Config.USART_baudrate =   USART_BAUDRATE_19200;
	    handlerUsar.USART_Config.USART_datasize =   USART_DATASIZE_8BIT;
	    handlerUsar.USART_Config.USART_parity   =   USART_PARITY_NONE;
	    handlerUsar.USART_Config.USART_stopbits  =  USART_STOPBIT_1;



// Cargo de configguracion  , como?

	    USART_Config(&handlerUsar);



//
	while(1){



    writeChar(&handlerUsar,'l');



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


