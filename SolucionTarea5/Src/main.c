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

uint8_t blinkySimple                      = 0; //Asignación a el blinky de led de estado
uint8_t Bandera                           = 0;
uint8_t contador                          = 0;
uint8_t rxData                            =  0;
BasicTimer_Handler_t   handlerTimer2      = {0}; // Handler para el timer 2
USART_Handler_t        Usart2             = {0};
GPIO_Handler_t handlerTx     	    	  = {0};
GPIO_Handler_t handlerRx	 	     	  = {0};
char bufferData[64] =  {0};
char greetingMsg[] = "Millos el mas grande \n";

void InitSystem(void);

int main(void){
	ResetClock();
	InitSystem();


while (1){

  // Mensaje a enviar con bandera

	startCounterTimer(&handlerTimer2);

	if (rxData != '\0'){
				//Imprimimos el caracter recibido
				writeChar(&Usart2, rxData);

				if (rxData == 'm'){
					//Presentamos un mensaje
					writeMsg(&Usart2 , greetingMsg);
				}



				rxData = '\0';
			}

	if(Bandera == 4){

		contador ++;
		sprintf(bufferData, "Contador= %d \n", (int) contador);
		writeMsg(&Usart2, bufferData);
		Bandera = 0;
	}

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
	  Bandera ++;
	 if(Bandera > 4){
		 Bandera = 0;
	 }

    }




void USART2Rx_CallBack(void){

	rxData = getRxData();
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

			 Usart2.ptrUSARTx  = USART2;
			 Usart2.USART_Config.USART_baudrate   = USART_BAUDRATE_115200_Extra;
			 Usart2.USART_Config.USART_datasize   = USART_DATASIZE_9BIT;
			 Usart2.USART_Config.USART_mode       = USART_MODE_RXTX;
			 Usart2.USART_Config.USART_parity     = USART_PARITY_NONE;
			 Usart2.USART_Config.USART_stopbits   = USART_STOPBIT_1;
			 Usart2.USART_Config.USART_IntTx      = USART_TX_INTERRUP_DISABLE;
			 Usart2.USART_Config.USART_IntRx      = USART_RX_INTERRUPT_ENABLE;

			 USART_Config(&Usart2);

			 // Los pines

			 /* Handler para el PIN A2 para transmisión */
			 	handlerTx.pGPIOx                       			= GPIOA;
			 	handlerTx.GPIO_PinConfig.GPIO_PinNumber			= PIN_2;				// Pin TX USART2
			 	handlerTx.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
			 	handlerTx.GPIO_PinConfig.GPIO_PinOPType		    = GPIO_OTYPE_PUSHPULL;
			 	handlerTx.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
			 	handlerTx.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
			 	handlerTx.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;

			 	/* Se carga la configuración */
			 	GPIO_Config(&handlerTx);

			 	/* Handler para el PIN A3  para recepción */
			 	handlerRx.pGPIOx                       			= GPIOA;
			 	handlerRx.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;				// Pin RX USART2
			 	handlerRx.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
			 	handlerRx.GPIO_PinConfig.GPIO_PinOPType		    = GPIO_OTYPE_PUSHPULL;
			 	handlerRx.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
			 	handlerRx.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
			 	handlerRx.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;					// Función alternativa USART2

			 	/* Se carga la configuración */
			 	GPIO_Config(&handlerRx);




}
