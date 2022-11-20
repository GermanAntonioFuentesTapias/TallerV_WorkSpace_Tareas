/*
 * main3.c
 *
 *  Created on: 18/11/2022
 *      Author: German
 */


#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "PwmDriver.h"
#include "CaptureFrecDriver.h"

// Definición de variables
GPIO_Handler_t 				handlerBlinkyPin	  = {0};
GPIO_Handler_t				handlerTxPin		  = {0};
GPIO_Handler_t				handlerRxPin		  = {0};
uint8_t handlerLed                      = 0;

// Timer encargado del Blinky
BasicTimer_Handler_t 		handlerBlinkyTimer 	  = {0};
BasicTimer_Handler_t        handlerTimerPWM       = {0};

GPIO_Handler_t 				handlerPinPWMChannel 	  = {0};
PWM_Handler_t				handlerSignalPWM  	  = {0};

GPIO_Handler_t				handlerPinCapturaFreq = {0};
uint8_t 					rawPeriod 			  =  0;

// Comunicación RS-232 con el PC, ya habilitada en la board Núcleo
USART_Handler_t 			handlerUSART2		 = {0};
uint8_t						rxData  			 =  0;
char 						bufferData[64]		 = "Prueba";
int16_t                     period []= {0};
//int16_t                     period = 0;

Capture_Handler_t			handlerCaptureFreq   = {0};

uint16_t 					duttyValue			 = 5000;

/* Para el barrido */

uint8_t j = 0;
uint16_t lectura= 0;
//bool adcIsComplete 	 = false;

uint8_t FinishCap = 0;

/* Definición de prototipos de funciones */
void initSystem (void);


/*
 *  Función principal del programa
 */

int main(void){

	// Inicializamos todos los elementos del sistema
	initSystem();

	writeChar(&handlerUSART2, ' ');

	/* Loop forever */
	while(1){

		// Hacemos un "eco" con el valor que nos llega por el serial
		if (rxData != '\0'){
			writeChar(&handlerUSART2, rxData);

			if (rxData == 'd'){
				duttyValue -= 100;
				// Lanzamos un nuevo ciclo de adquisiciones
				updateDuttyCycle(&handlerSignalPWM, duttyValue);
				sprintf (bufferData, "dutty = %u \n", (unsigned int) duttyValue);
				writeMsg(&handlerUSART2, bufferData);
				rxData = '\0';
			}

			else if (rxData == 'u'){
				duttyValue += 100;
				// Lanzamos un nuevo ciclo de adquisiciones
				updateDuttyCycle(&handlerSignalPWM, duttyValue);
				sprintf (bufferData, "dutty = %u \n", (unsigned int) duttyValue);
				writeMsg(&handlerUSART2, bufferData);
				rxData = '\0';
			}

//			else if (rxData == 'c'){
//				rawPeriod = getPeriodFreq(&handlerCaptureFreq) ;
//				sprintf (bufferData, "rawPeriod = %u \n", (unsigned int) rawPeriod);
//				writeMsg(&handlerUSART2, bufferData);
//				rxData = '\0';
//			}
//

			else if (rxData == 'c'){
				getInter(&handlerCaptureFreq);
			}

			else if(FinishCap){

				startCounterTimer(&handlerTimerPWM);
				rawPeriod =period[1] - period[0] ;
//				rawPeriod =  getInter(&handlerCaptureFreq) ;
				sprintf (bufferData, "rawPeriod = %u \n", (unsigned int) rawPeriod);
				writeMsg(&handlerUSART2, bufferData);
				rxData = '\0';
			}
		}
	}

	//return 0;
}

void initSystem (void){

	// Configurando el pin para el Led Blinky
	handlerBlinkyPin.pGPIOx 										= GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber					= PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType					= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl				= GPIO_PUPDR_NOTHING;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_MEDIUM;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinAltFunMode				= AF0;

	GPIO_Config(&handlerBlinkyPin);
	GPIO_WritePin(&handlerBlinkyPin, SET);

	// Configurando los pines sobre los que funciona el USART2 (TX)
	handlerTxPin.pGPIOx												= GPIOA;
	handlerTxPin.GPIO_PinConfig.GPIO_PinNumber						= PIN_2;
	handlerTxPin.GPIO_PinConfig.GPIO_PinMode						= GPIO_MODE_ALTFN;
	handlerTxPin.GPIO_PinConfig.GPIO_PinOPType						= GPIO_OTYPE_PUSHPULL;
	handlerTxPin.GPIO_PinConfig.GPIO_PinPuPdControl					= GPIO_PUPDR_NOTHING;
	handlerTxPin.GPIO_PinConfig.GPIO_PinSpeed						= GPIO_OSPEED_FAST;
	handlerTxPin.GPIO_PinConfig.GPIO_PinAltFunMode					= AF7;

	GPIO_Config(&handlerTxPin);

	// Configurando los pines sobre los que funciona el USART2 (RX)
	handlerRxPin.pGPIOx												= GPIOA;
	handlerRxPin.GPIO_PinConfig.GPIO_PinNumber						= PIN_3;
	handlerRxPin.GPIO_PinConfig.GPIO_PinMode						= GPIO_MODE_ALTFN;
	handlerRxPin.GPIO_PinConfig.GPIO_PinOPType						= GPIO_OTYPE_PUSHPULL;
	handlerRxPin.GPIO_PinConfig.GPIO_PinPuPdControl					= GPIO_PUPDR_NOTHING;
	handlerRxPin.GPIO_PinConfig.GPIO_PinSpeed						= GPIO_OSPEED_FAST;
	handlerRxPin.GPIO_PinConfig.GPIO_PinAltFunMode					= AF7;

	GPIO_Config(&handlerRxPin);

	// Configurando la comunicación serial (Cable verde es TX, Cable Blando es RX)
	handlerUSART2.ptrUSARTx											= USART2;
	handlerUSART2.USART_Config.USART_mode							= USART_MODE_RXTX;
	handlerUSART2.USART_Config.USART_baudrate						= USART_BAUDRATE_115200;
	handlerUSART2.USART_Config.USART_datasize               		= USART_DATASIZE_9BIT;
	handlerUSART2.USART_Config.USART_parity							= USART_PARITY_EVEN;
	handlerUSART2.USART_Config.USART_stopbits						= USART_STOPBIT_1;
	handlerUSART2.USART_Config.USART_IntRx				            = USART_RX_INTERRUPT_ENABLE;
	handlerUSART2.USART_Config.USART_IntTx				         	= USART_TX_INTERRUP_DISABLE;

	USART_Config(&handlerUSART2);

	// Configurando el Timer2 para que funcione con el blinky
	handlerBlinkyTimer.ptrTIMx 										= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode 						= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed 						= BTIMER_SPEED_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period 						= 2500;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable 			= 1;  //

	BasicTimer_Config(&handlerBlinkyTimer);

	handlerTimerPWM.ptrTIMx                                          = TIM4;
	handlerTimerPWM.TIMx_Config.TIMx_mode                            = BTIMER_MODE_UP;
	handlerTimerPWM.TIMx_Config.TIMx_speed                           = BTIMER_SPEED_1ms;
	handlerTimerPWM.TIMx_Config.TIMx_period                          = 200;
	handlerTimerPWM.TIMx_Config.TIMx_interruptEnable                 = 1;

	BasicTimer_Config(&handlerTimerPWM);


	// Activamos el TIM2
	startCounterTimer(&handlerBlinkyTimer);


	/* Configuramos el PWM */
	handlerPinPWMChannel.pGPIOx										= GPIOC;
	handlerPinPWMChannel.GPIO_PinConfig.GPIO_PinNumber				= PIN_7;
	handlerPinPWMChannel.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinCapturaFreq.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerPinPWMChannel.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerPinPWMChannel.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_FAST;
	handlerPinPWMChannel.GPIO_PinConfig.GPIO_PinAltFunMode			= AF2;

	GPIO_Config(&handlerPinPWMChannel);


	// Handler para el PWM
	handlerSignalPWM.ptrTIMx											= TIM3;
	handlerSignalPWM.config.channel										= PWM_CHANNEL_2;
	handlerSignalPWM.config.duttyCicle									= duttyValue;
	handlerSignalPWM.config.periodo										= 20000;
	handlerSignalPWM.config.prescaler									= 16;

	pwm_Config(&handlerSignalPWM);
	enableOutput(&handlerSignalPWM);

	startPwmSignal(&handlerSignalPWM);

	/* Configuramos el pin para la captura de la señal */
	handlerPinCapturaFreq.pGPIOx										= GPIOB;
	handlerPinCapturaFreq.GPIO_PinConfig.GPIO_PinNumber					= PIN_8;
	handlerPinCapturaFreq.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_ALTFN;
	handlerPinCapturaFreq.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerPinCapturaFreq.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_FAST;
	handlerPinCapturaFreq.GPIO_PinConfig.GPIO_PinAltFunMode				= AF2;

	GPIO_Config(&handlerPinCapturaFreq);

	handlerCaptureFreq.ptrTIMx											= TIM4;
	handlerCaptureFreq.config.channel									= CAPTURE_CHANNEL_3;
	handlerCaptureFreq.config.edgeSignal								= CAPTURE_FALLING_EDGE;
	handlerCaptureFreq.config.prescalerCapture							= CAPTURE_PRESCALER_1_1;
	handlerCaptureFreq.config.timerSpeed								= CAPTURE_TIMER_SPEED_100us;

	capture_Config(&handlerCaptureFreq);
}

/*
 * Callback del Timer 2, hacemos blinky
 */

void BasicTimer2_CallBack(void){
	handlerLed = !handlerLed; // Activación de Blinky
		//
				if(handlerLed){
		//
					// Activa
					GPIO_WritePin(&handlerBlinkyPin,  SET);
		//
				}else{
		//
					GPIO_WritePin(&handlerBlinkyPin, RESET); // Desactiva

	}
}

/*
 *  Callback relacionado con la recepción del USART2
 *  El puerto es leído en el ISR (para bajar la bandera de la interrupción)
 *  El caracter que se lee es devuelto por la función getRxData
 */

void USART2Rx_CallBack (void){
	// Leemos el valor del registro DR, donde se almacena el dato que llega.
	// Esto además debe bajar la bandera de la interrupción
	rxData = getRxData();
}

void capturefrecuencia4(void){

	// Se toma la representación para en canal 3 timer 4

	lectura = getData();

	j ++;

	if(j == 1){

		period[0] = lectura;
	}

	else if( j == 2){

		period[1] = lectura;

		FinishCap = 1;

		j= 0;
	}


}
