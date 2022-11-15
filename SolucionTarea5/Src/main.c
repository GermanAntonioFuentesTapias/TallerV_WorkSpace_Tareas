/*
 * main.c
 *
 *  Created on: Nov 15, 2022
 *      Author: German
 *
 *      Tarea V
 *
 *      - Modificar la señal principal del MCU, de forma que sea la máxima que el equipo tolera: 100MHz.
        - Modificar adecuadamente los valores de configuración del USART, de forma que se tenga una comunicación estable a 115200 bps utilizando el USART2.
        - Modificar adecuadamente los valores de configuración de un timer, de forma que el led de estado funcione correctamente a 250ms.
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "MainClockDriver.h"

GPIO_Handler_t         blinkySimplePin    = {0}; // Handler para el blinkySimple

uint8_t blinkySimple       = 0; //Asignación a el blinky de led de estado
BasicTimer_Handler_t   handlerTimer2      = {0}; // Handler para el timer 2

void InitSystem(void);

int main(void){

	ResetClock();
	InitSystem();


while (1){

//	 startCounterTimer(&handlerTimer2);

	// Timer

	// Usart

	// Configurar Pin




}

}


void BasicTimer2_CallBack(void){

    //Nuestro CallBack ocurre a la velocidad asignada en la configuración del usuario,
	//En nuestro caso 250 ms

	blinkySimple = !blinkySimple; // Activación de Blinky

	if(blinkySimple){

		GPIO_WritePin(&blinkySimplePin, SET); // Activa

	}else{

		GPIO_WritePin(&blinkySimplePin, RESET); // Desactiva
		// Y para qué teniamos la funcion toogle?
	}
    }

void InitSystem(void){

	blinkySimplePin.pGPIOx= GPIOA; // Selección del GPIOA

			/*Configuración del Led de Blinky*/
			blinkySimplePin.GPIO_PinConfig.GPIO_PinNumber           = PIN_5;
			blinkySimplePin.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_OUT;
			blinkySimplePin.GPIO_PinConfig.GPIO_PinOPType           = GPIO_OTYPE_PUSHPULL;
			blinkySimplePin.GPIO_PinConfig.GPIO_PinPuPdControl      = GPIO_PUPDR_NOTHING;
			blinkySimplePin.GPIO_PinConfig.GPIO_PinSpeed            = GPIO_OSPEED_MEDIUM;
			blinkySimplePin.GPIO_PinConfig.GPIO_PinAltFunMode       = AF0;

		     // Cargando la configuración del Led de Blinky
			 GPIO_Config(&blinkySimplePin);



			 handlerTimer2.ptrTIMx = TIM2;
			 handlerTimer2.TIMx_Config.TIMx_mode                    = BTIMER_MODE_UP;     // Cuenta hacia arriba
			 handlerTimer2.TIMx_Config.TIMx_speed                   = 50000; // La velocidad
			 handlerTimer2.TIMx_Config.TIMx_period                  = 500;               //250ms
			 handlerTimer2.TIMx_Config.TIMx_interruptEnable         = 1;                  // Activando la configuración, aunque todavia


			 BasicTimer_Config(&handlerTimer2);


}

