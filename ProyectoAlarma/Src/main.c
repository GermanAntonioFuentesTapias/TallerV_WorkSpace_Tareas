/*
 * main.c
 *
 *  Created on: 1/11/2022
 *      Author: German Antonio Fuentes Tapias
 *
 *      @ gafuentest@unal.edu.co
 *
 *     Elaboración proyecto
 *
 *      Paridad Even
 *      Usart1 para comunicación serial
 *      Velocidad 19200
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "stm32f4xx.h"
#include "BasicTimer.h"
#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "LCDDriver.h"
#include "I2CDriver.h"
#include "ExtiDriver.h"
#include  "KeyPad.h"

GPIO_Handler_t    handlerBlinkyPin   = {0};

//Timer encargado del Blinky
BasicTimer_Handler_t  handlerBlinkyTimer = {0};

GPIO_Handler_t handlerTx     	    	= {0};
GPIO_Handler_t handlerRx	 	     	= {0};
USART_Handler_t        handlerUsart       = {0}; // Handler para el Usart
uint8_t rxData      =  0;
char DataRTC[64] = "Wipi";
uint8_t handlerLed                      = 0;
uint8_t Code                            = 0;
uint8_t Bandera                         = 0;
uint8_t valor                           = 0;
GPIO_Handler_t	handlerI2CSDA			= {0};
GPIO_Handler_t	handlerI2CSCL 			= {0};
GPIO_Handler_t  handlerSensor           = {0};
GPIO_Handler_t  handlerRojo             = {0};
I2C_Handler_t	handlerLCD 				= {0};
EXTI_Config_t   handlerExtiSensor           = {0}; // Configuración para el boton externo
char dataLCD[64] = {0};
char bufferData[64]    =  {0};


#define LCD_ADRESS		0b0100111

//Definición de las cabeceras del main
void initSystem (void);

void Alarma(uint8_t Opciones);

int main(void){
 	initSystem();


	LCD_Init(&handlerLCD);
	delay_10();
	LCD_ClearScreen(&handlerLCD);
	delay_10();
//	sprintf(dataLCD, "Alarmas S,A.        Bienvenidos");
	LCD_setCursor(&handlerLCD,0,0);
	LCD_sendSTR(&handlerLCD,"____________________");
	LCD_setCursor(&handlerLCD,1,4);
	LCD_sendSTR(&handlerLCD,"Alarmas S,A.");
	LCD_setCursor(&handlerLCD,2,4);
	LCD_sendSTR(&handlerLCD,"Bienvenidos");
	LCD_setCursor(&handlerLCD,3,0);
	LCD_sendSTR(&handlerLCD,"____________________");

	startCounterTimer(&handlerBlinkyTimer);

while (1){

	// Si hay interrupción , la bandera Code se activa

	      if(Code){
//	      if (GPIO_ReadPin(&handlerSensor) ==  SET){
//            if(!Bandera){
	    	LCD_ClearScreen(&handlerLCD);
			LCD_setCursor(&handlerLCD,0,1);
			LCD_sendSTR(&handlerLCD,"Detecto movimiento");
			LCD_setCursor(&handlerLCD,2,7);
			LCD_sendSTR(&handlerLCD,"Alerta!");
			LCD_setCursor(&handlerLCD,3,0);
			LCD_sendSTR(&handlerLCD,"Por favor desactive");

          GPIO_WritePin(&handlerRojo, SET);

		// Se baja la bandera para otra interrupción
		Code = 0;


            }

	      if((GPIO_ReadPin(&handlerSensor) == 0)){

	    	  GPIO_WritePin(&handlerRojo, RESET);


//	    	  Alarma(0);

	      			      		}


//	      if(Bandera == 16){
//
//
//	      			LCD_ClearScreen(&handlerLCD);
//	      			LCD_setCursor(&handlerLCD,0,1);
//	      			LCD_sendSTR(&handlerLCD,"Por favor reinicie ");
//	      			LCD_setCursor(&handlerLCD,2,6);
//	      			LCD_sendSTR(&handlerLCD,"Movimiento");
//
//	      			GPIO_WritePin(&handlerRojo, RESET);
//
//	      		}


//	      else{
//
//			 GPIO_WritePin(&handlerRojo, RESET);
////
//			LCD_ClearScreen(&handlerLCD);
//			LCD_setCursor(&handlerLCD,0,1);
//			LCD_sendSTR(&handlerLCD,"Por favor reinicie ");
//			LCD_setCursor(&handlerLCD,2,6);
//			LCD_sendSTR(&handlerLCD,"Movimiento");
//
//		}
//
//			 if(Code ==0) {
//				LCD_ClearScreen(&handlerLCD);
//				LCD_setCursor(&handlerLCD,0,0);
//				LCD_sendSTR(&handlerLCD,"Detecto movimiento");
//				LCD_setCursor(&handlerLCD,2,7);
//				LCD_sendSTR(&handlerLCD,"Alerta");
//
//
//
//			 }


	      /* Comunicacion serial */

	      if (rxData != '\0'){
	      			writeChar(&handlerUsart, rxData);

	      			if (rxData == 'd'){



	      				LCD_ClearScreen(&handlerLCD);

	      				LCD_setCursor(&handlerLCD,0,1);
	      				LCD_sendSTR(&handlerLCD,"Correcion para de  prueba mas adelante");

//	      				sprintf (bufferData, "Vamos %u \n", (unsigned int) Bandera);
	      				writeMsg(&handlerUsart, "Mazato \n");
	      				rxData = '\0';
	      			}

	      			else if (rxData == 'u'){

	      				// Lanzamos un nuevo ciclo de adquisiciones

	      				sprintf (bufferData, "dutty = %u \n", (unsigned int) Bandera);
	      				writeMsg(&handlerUsart, bufferData);

	      			}
	      		}




		}

	    }


//
//}




void initSystem(void){

	//Configurando el Timer2 para que funcione con el Blinky
		handlerBlinkyTimer.ptrTIMx                            = TIM2;
		handlerBlinkyTimer.TIMx_Config.TIMx_mode              = BTIMER_MODE_UP;
		handlerBlinkyTimer.TIMx_Config.TIMx_speed             = BTIMER_SPEED_100us;
		handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable   = 1;
		handlerBlinkyTimer.TIMx_Config.TIMx_period            = 2500;
		BasicTimer_Config(&handlerBlinkyTimer);

	handlerBlinkyPin.pGPIOx                              = GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber       = PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_FAST;
	GPIO_Config(&handlerBlinkyPin);


	/* Configurando los pines sobre los que funciona el I2C1 */
	handlerI2CSCL.pGPIOx								= GPIOB;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_OPENDRAIN;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinAltFunMode		= AF4;

	GPIO_Config(&handlerI2CSCL);



	/* Configurando los pines sobre los que funciona el I2C1 */
	handlerI2CSDA.pGPIOx								= GPIOB;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinNumber			= PIN_7;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_OPENDRAIN;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinAltFunMode		= AF4;

	GPIO_Config(&handlerI2CSDA);

	handlerLCD.ptrI2Cx		= I2C1;
	handlerLCD.modeI2C		= I2C_MODE_SM;
	handlerLCD.slaveAddress	= LCD_ADRESS;
	//handlerLCD.dataI2C      = dataLCD;
	i2c_config(&handlerLCD);


	/* Configuración de entradas sensor */


	handlerSensor.pGPIOx                             = GPIOC;
	handlerSensor.GPIO_PinConfig.GPIO_PinNumber      = PIN_7;
	handlerSensor.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_IN;
	handlerSensor.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
	handlerSensor.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	handlerSensor.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_FAST;

	GPIO_Config(&handlerSensor);

	/* Configuración led a prender */

	handlerRojo.pGPIOx                              = GPIOC;
	handlerRojo.GPIO_PinConfig.GPIO_PinNumber       = PIN_2;
	handlerRojo.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerRojo.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerRojo.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerRojo.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_FAST;

	GPIO_Config(&handlerRojo);


	/* Configuraciones Exti */

	handlerExtiSensor.pGPIOHandler  = &handlerSensor ;
	handlerExtiSensor.edgeType      = EXTERNAL_INTERRUPT_RISING_EDGE; // Flanco de bajada

	/* Cargando configuración */

	extInt_Config(&handlerExtiSensor);


	handlerTx.pGPIOx                       			= GPIOA;
		handlerTx.GPIO_PinConfig.GPIO_PinNumber			= PIN_2;				// Pin TX USART2
		handlerTx.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
		handlerTx.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
		handlerTx.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
		handlerTx.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
		handlerTx.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;					// Función alternativa USART2

		/* Se carga la configuración */
		GPIO_Config(&handlerTx);

		/* Handler para el PIN A3 */
		handlerRx.pGPIOx                       			= GPIOA;
		handlerRx.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;				// Pin RX USART2
		handlerRx.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
		handlerRx.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
		handlerRx.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
		handlerRx.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
		handlerRx.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;					// Función alternativa USART2

		/* Se carga la configuración */
		GPIO_Config(&handlerRx);

		/* Inicialización de la configuración para el USART2*/

		handlerUsart.ptrUSARTx = USART2 ;
		// Configuración del usuario para el pin -->> Paridad activa en modo Even

		handlerUsart.USART_Config.USART_mode          =  USART_MODE_RXTX;       // En modo de transmisión
		handlerUsart.USART_Config.USART_baudrate      =  USART_BAUDRATE_115200;
		handlerUsart.USART_Config.USART_datasize      =  USART_DATASIZE_9BIT;
		handlerUsart.USART_Config.USART_parity        =  USART_PARITY_EVEN;
		handlerUsart.USART_Config.USART_stopbits      =  USART_STOPBIT_1;
		handlerUsart.USART_Config.USART_IntRx         =  USART_RX_INTERRUPT_ENABLE;
		handlerUsart.USART_Config.USART_IntTx         =  USART_TX_INTERRUP_DISABLE;

		/*Cargando la configuración del Usart para la comunicación serial */
		USART_Config(&handlerUsart);

}


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

			Bandera ++;
				 if(Bandera > 16){
					 Bandera = 0;
				 }


}

void USART2Rx_CallBack(void){

	rxData = getRxData();
}



void callback_extInt7(void){

	Code = 1; // Bandera que da inicio al envio del mensaje mediante comunicación serial

}

void Alarma(uint8_t Opciones){

	switch(Opciones){

	case 0: {

		 GPIO_WritePin(&handlerRojo, RESET);

		LCD_ClearScreen(&handlerLCD);

		LCD_setCursor(&handlerLCD,0,1);
		LCD_sendSTR(&handlerLCD,"Por favor reinicie ");
		LCD_setCursor(&handlerLCD,2,6);
		LCD_sendSTR(&handlerLCD,"Movimiento");

		break;

}

	default:{


		LCD_ClearScreen(&handlerLCD);

		LCD_setCursor(&handlerLCD,0,1);
		LCD_sendSTR(&handlerLCD,"Error en el ");
		LCD_setCursor(&handlerLCD,2,6);
		LCD_sendSTR(&handlerLCD,"Sistema");

		break;

	}

}

}

