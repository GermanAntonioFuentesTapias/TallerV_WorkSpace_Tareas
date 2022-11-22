/*
 * main3.c
 *
 *  Created on: 18/11/2022
 *      Author: German Antonio Fuentes Tapias
 *
 *      Correo = gafuentest@unal.edu.co
 *
 *       Solución tarea 6 captura de frecuencias a traves de timer
 *
 *
 *       Paridad Even
 *       Usart2 para comunicación serial
 *      Velocidad 115200
 *
 * ***************************************************************************************************************
 *       Se tomo el codigo CaptureFrecDriver del docente y se elimino la función en la cual se tenia el While
 *
 *       Al querer realizar por interrupción fue necesario usar las interrupciones de cada canal que trae los timer en
 *       TIMx_DIER, generar 3 funciones nuevas ( 2 actuales modificadas) y una de limpiar el contador
 *
 *       Asi que el paso 1: fue crear o habilitar para cada canal TIM_CCER_CC1E; el modulo de captura
 *       con su respectiva interrupción TIM_DIER_CC1IE;
 *       El paso 2: fue entonces crear las funciones en el .h y los respectivos call back (llamados capturefrecuencia)
 *       El paso 3: Meter esos capture en  el .c y empezar armar el cuerpo de las funciones
 *       El paso 4: Colocar la activación y desactivación de las interrupciones globales como la habilitación de TIM_CR1_CEN;
 *       El paso 5: Dentro de la funcion Start leer que canal es y con base a esto apuntar a CCR# dependiendo de cual sea,
 *                  ya según despues de capturado el valor en CCR# bajar su respectiva bandera  &= ~TIM_SR_CC#OF; y retornar
 *                  el evento  capture/compare register
 *       El paso 6: Con la debida configuración de lo anterior puede tener registro de captura mas no la información requerida para
 *                 saber el periodo, asi que entra la configuración de la siguiente función de leer los datos o interpretar lo ocurrido,
 *                 Asi que en este main se toman Cap1 y Cap2 en los cuales se condicionan que la primera tomara el valor de Cap1 dado por
 *                 el paso 5, la bandera que da la posibilidad de Cap2 se habilita, se obtiene el evento dado por esa función y se almacena
 *                 en Cap 2, asi que tenemos los dos valores, es ahi donde entra la función de esta paso, toma Cap1 y Cap2, los procesa
 *                 matematicamente que en nuestro caso cambian segun el prescaler establecido, se restan entre si y se operan por la velocidad
 *                 segun sea el caso (us a ms) y es asi como se obtiene el valor de dato listo como el periodo generado por el PWM
 *
 *          paso 7: Desabilitar el timer4 aqui de la configuración de la forma anterior del driver y una correpción que se puede decir operativa
 *                             con la impresión y listo. Se tiene el valor del periodo.
 *
 *         Caso a mejor: A mayores periodos dados por el PWM se tiene un aumento de 1 unidad respecto al valor verdadero, pero se puede afirmar
 *                       que para valores como el inicial (20) se tiene una aproximación cercana.
 *

 *
 *
 *
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

GPIO_Handler_t 				handlerPinPWMChannel  = {0};
PWM_Handler_t				handlerSignalPWM  	  = {0};

GPIO_Handler_t				handlerPinCapturaFreq = {0};
uint8_t 					rawPeriod 			  =  0;

// Comunicación RS-232 con el PC, ya habilitada en la board Núcleo
USART_Handler_t 			handlerUSART2		 = {0};
uint8_t						rxData  			 =  0;
char 						bufferData[64]		 = "Prueba";


Capture_Handler_t			handlerCaptureFreq   = {0};

uint8_t BanderaRegistro 			  = 0;
uint8_t BanderaCaptura				  = 0;
uint8_t Counter 					  = 0;
uint32_t Captura1 		              = 0;
uint32_t Captura2	                  = 0;
uint32_t Dato						  = 0;

uint16_t 					duttyValue			 = 5000;


/* Definición de prototipos de funciones */
void initSystem (void);


/*
 *  Función principal del programa
 */

int main(void){

	// Inicializamos todos los elementos del sistema
	initSystem();


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
		}

//			else if (rxData == 'c'){
//				rawPeriod = getPeriodFreq(&handlerCaptureFreq) ;
//				sprintf (bufferData, "rawPeriod = %u \n", (unsigned int) rawPeriod);
//				writeMsg(&handlerUSART2, bufferData);
//				rxData = '\0';
//			}
//

			 if (BanderaCaptura){
				if(Counter){

					Captura2 = StartPeriod(&handlerCaptureFreq);

					Dato = Period_Frecuen_Get(&handlerCaptureFreq, Captura1, Captura2);

					sprintf(bufferData, "\n\r El Periodo es de = %u", (unsigned int) Dato -1);

					writeMsg(&handlerUSART2, bufferData);

					Counter = 0;
                 }



			else {

				CleanCapture(&handlerCaptureFreq);

				Captura1 = StartPeriod(&handlerCaptureFreq);

				Counter = 1;
			}
				BanderaCaptura = 0;
		}
	}

	return 0;
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

	// Cargando la configuración
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

	// Cargando la configuración
	GPIO_Config(&handlerTxPin);

	// Configurando los pines sobre los que funciona el USART2 (RX)
	handlerRxPin.pGPIOx												= GPIOA;
	handlerRxPin.GPIO_PinConfig.GPIO_PinNumber						= PIN_3;
	handlerRxPin.GPIO_PinConfig.GPIO_PinMode						= GPIO_MODE_ALTFN;
	handlerRxPin.GPIO_PinConfig.GPIO_PinOPType						= GPIO_OTYPE_PUSHPULL;
	handlerRxPin.GPIO_PinConfig.GPIO_PinPuPdControl					= GPIO_PUPDR_NOTHING;
	handlerRxPin.GPIO_PinConfig.GPIO_PinSpeed						= GPIO_OSPEED_FAST;
	handlerRxPin.GPIO_PinConfig.GPIO_PinAltFunMode					= AF7;

	// Cargando la configuración
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

	// Cargando la configuración
	BasicTimer_Config(&handlerBlinkyTimer);

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
	handlerSignalPWM.config.periodo										= 18000;
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
	handlerCaptureFreq.config.prescalerCapture							= CAPTURE_PRESCALER_4_1;
	handlerCaptureFreq.config.timerSpeed								= CAPTURE_TIMER_SPEED_1ms ;

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

void capturefrecuencia3(void){

	// Se toma la representación para en canal 3 timer 4

 BanderaCaptura  =1;

}

