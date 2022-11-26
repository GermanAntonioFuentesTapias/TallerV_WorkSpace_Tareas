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
uint8_t Puerta                          = 0;
GPIO_Handler_t	handlerI2CSDA			= {0};
GPIO_Handler_t	handlerI2CSCL 			= {0};
GPIO_Handler_t  handlerSensor           = {0};
GPIO_Handler_t  handlerRojo             = {0};
GPIO_Handler_t  handlerPuerta           = {0};

/* Configuración Pines Key Pad */

GPIO_Handler_t  handlerF1               = {0};
GPIO_Handler_t  handlerF2               = {0};
GPIO_Handler_t  handlerF3               = {0};
GPIO_Handler_t  handlerF4               = {0};
GPIO_Handler_t  handlerC1               = {0};
GPIO_Handler_t  handlerC2               = {0};
GPIO_Handler_t  handlerC3               = {0};
GPIO_Handler_t  handlerC4               = {0};

/**********************************************/

I2C_Handler_t	handlerLCD 				= {0};
EXTI_Config_t   handlerExtiSensor       = {0}; // Configuración para el boton externo
EXTI_Config_t   handlerExtiPuerta       = {0};
char dataLCD[64] = {0};
char bufferData[64]    =  {0};

/* Funciones para comandos */


uint8_t counterReception = 0;
char bufferReception[64] = {0};
char cmd[64];
char userMsg[64];
char data[64] = {0} ;
bool stringComplete = false;
bool makeUpdateLCD  = false;
unsigned int firstParameter;
unsigned int secondParameter;


#define LCD_ADRESS		0b0100111

//Definición de las cabeceras del main
void initSystem (void);
//Para el KeyPad
void ConfigKeyPad(void);
// Alarma
void Alarma(uint8_t Opciones);



int main(void){
 	initSystem();
 	void parseCommands (char *ptrBufferReception);


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

	      if((Puerta) ||  (Code)){
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

		Puerta = 0;


            }

	      if((GPIO_ReadPin(&handlerSensor) == 0)){

	    	  GPIO_WritePin(&handlerRojo, RESET);


//	    	  Alarma(0);

	      			      		}


	      if(Bandera){


	      			LCD_ClearScreen(&handlerLCD);
	      			LCD_setCursor(&handlerLCD,0,1);
	      			LCD_sendSTR(&handlerLCD,"Por favor reinicie ");
	      			LCD_setCursor(&handlerLCD,2,6);
	      			LCD_sendSTR(&handlerLCD,"Movimiento");

	      			GPIO_WritePin(&handlerRojo, RESET);

	      			Bandera = 0;

	      		}




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


			if(rxData != '\0'){


			bufferReception[counterReception] = rxData;
			counterReception++;
				 if (rxData == '@'){
					 stringComplete = true;

					 // Agrego linea

					 bufferReception[counterReception] = '\0';

					 counterReception = 0;
				 }

				 rxData = '\0';

		   }

	      if ( stringComplete){

	      			parseCommands(bufferReception);

	      			stringComplete = false;
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

	/* Configuración para Pin externo */

	handlerPuerta.pGPIOx                             = GPIOB;
	handlerPuerta.GPIO_PinConfig.GPIO_PinNumber      = PIN_1;
	handlerPuerta.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_IN;
	handlerPuerta.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
	handlerPuerta.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	handlerPuerta.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_FAST;

	GPIO_Config(&handlerPuerta);




	/* Configuraciones Exti */

	// Sensor

	handlerExtiSensor.pGPIOHandler  = &handlerSensor ;
	handlerExtiSensor.edgeType      = EXTERNAL_INTERRUPT_RISING_EDGE; // Flanco de bajada


	// Puerta

	handlerExtiPuerta.pGPIOHandler  = &handlerPuerta;
	handlerExtiPuerta.edgeType      = EXTERNAL_INTERRUPT_RISING_EDGE;

	/* Cargando configuración */

	extInt_Config(&handlerExtiSensor);
	extInt_Config(&handlerExtiPuerta);


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

/* Configuración del Key Pad */

void ConfigKeyPad(void){

	handlerF1.pGPIOx                               =  GPIOB;
    handlerF1.GPIO_PinConfig.GPIO_PinNumber        =  PIN_8;
	handlerF1.GPIO_PinConfig.GPIO_PinMode          =  GPIO_MODE_IN;
	handlerF1.GPIO_PinConfig.GPIO_PinOPType        =  GPIO_OTYPE_PUSHPULL;
	handlerF1.GPIO_PinConfig.GPIO_PinPuPdControl   =  GPIO_PUPDR_PULLDOWN;
	handlerF1.GPIO_PinConfig.GPIO_PinSpeed         =  GPIO_OSPEED_FAST;

	handlerF2.pGPIOx                               =  GPIOB;
	handlerF2.GPIO_PinConfig.GPIO_PinNumber        =  PIN_9;
	handlerF2.GPIO_PinConfig.GPIO_PinMode          =  GPIO_MODE_IN;
	handlerF2.GPIO_PinConfig.GPIO_PinOPType        =  GPIO_OTYPE_PUSHPULL;
	handlerF2.GPIO_PinConfig.GPIO_PinPuPdControl   =  GPIO_PUPDR_PULLDOWN;
	handlerF2.GPIO_PinConfig.GPIO_PinSpeed         =  GPIO_OSPEED_FAST;

	handlerF3.pGPIOx                               =  GPIOC;
	handlerF3.GPIO_PinConfig.GPIO_PinNumber        =  PIN_9;
	handlerF3.GPIO_PinConfig.GPIO_PinMode          =  GPIO_MODE_IN;
	handlerF3.GPIO_PinConfig.GPIO_PinOPType        =  GPIO_OTYPE_PUSHPULL;
	handlerF3.GPIO_PinConfig.GPIO_PinPuPdControl   =  GPIO_PUPDR_PULLDOWN;
	handlerF3.GPIO_PinConfig.GPIO_PinSpeed         =  GPIO_OSPEED_FAST;

	handlerF4.pGPIOx                               =  GPIOC;
	handlerF4.GPIO_PinConfig.GPIO_PinNumber        =  PIN_8;
	handlerF4.GPIO_PinConfig.GPIO_PinMode          =  GPIO_MODE_IN;
	handlerF4.GPIO_PinConfig.GPIO_PinOPType        =  GPIO_OTYPE_PUSHPULL;
	handlerF4.GPIO_PinConfig.GPIO_PinPuPdControl   =  GPIO_PUPDR_PULLDOWN;
	handlerF4.GPIO_PinConfig.GPIO_PinSpeed         =  GPIO_OSPEED_FAST;

	handlerC1.pGPIOx                               =  GPIOA;
	handlerC1.GPIO_PinConfig.GPIO_PinNumber        =  PIN_12;
	handlerC1.GPIO_PinConfig.GPIO_PinMode          =  GPIO_MODE_OUT;
	handlerC1.GPIO_PinConfig.GPIO_PinOPType        =  GPIO_OTYPE_PUSHPULL;
	handlerC1.GPIO_PinConfig.GPIO_PinPuPdControl   =  GPIO_PUPDR_NOTHING;
	handlerC1.GPIO_PinConfig.GPIO_PinSpeed         =  GPIO_OSPEED_FAST;


	handlerC2.pGPIOx                               =  GPIOA;
	handlerC2.GPIO_PinConfig.GPIO_PinNumber        =  PIN_11;
	handlerC2.GPIO_PinConfig.GPIO_PinMode          =  GPIO_MODE_OUT;
	handlerC2.GPIO_PinConfig.GPIO_PinOPType        =  GPIO_OTYPE_PUSHPULL;
	handlerC2.GPIO_PinConfig.GPIO_PinPuPdControl   =  GPIO_PUPDR_NOTHING;
	handlerC2.GPIO_PinConfig.GPIO_PinSpeed         =  GPIO_OSPEED_FAST;

	handlerC3.pGPIOx                               =  GPIOB;
	handlerC3.GPIO_PinConfig.GPIO_PinNumber        =  PIN_12;
	handlerC3.GPIO_PinConfig.GPIO_PinMode          =  GPIO_MODE_OUT;
	handlerC3.GPIO_PinConfig.GPIO_PinOPType        =  GPIO_OTYPE_PUSHPULL;
	handlerC3.GPIO_PinConfig.GPIO_PinPuPdControl   =  GPIO_PUPDR_NOTHING;
	handlerC3.GPIO_PinConfig.GPIO_PinSpeed         =  GPIO_OSPEED_FAST;

	handlerC4.pGPIOx                               =  GPIOB;
	handlerC4.GPIO_PinConfig.GPIO_PinNumber        =  PIN_11;
	handlerC4.GPIO_PinConfig.GPIO_PinMode          =  GPIO_MODE_OUT;
	handlerC4.GPIO_PinConfig.GPIO_PinOPType        =  GPIO_OTYPE_PUSHPULL;
	handlerC4.GPIO_PinConfig.GPIO_PinPuPdControl   =  GPIO_PUPDR_NOTHING;
	handlerC4.GPIO_PinConfig.GPIO_PinSpeed         =  GPIO_OSPEED_FAST;



	GPIO_Config(&handlerF1);
	GPIO_Config(&handlerF2);
	GPIO_Config(&handlerF3);
	GPIO_Config(&handlerF4);
	GPIO_Config(&handlerC1);
	GPIO_Config(&handlerC2);
	GPIO_Config(&handlerC3);
	GPIO_Config(&handlerC4);

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

//			Bandera ++;
//				 if(Bandera > 16){
//					 Bandera = 0;
//				 }


}

void USART2Rx_CallBack(void){

	rxData = getRxData();
}



void callback_extInt7(void){

	Code = 1; // Bandera que da inicio a alerta

}

void callback_extInt1(void){

	Puerta = 1;
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


char read_keypad (void){

		GPIO_WritePin(&handlerF1, RESET);//Pull the R1 low
		GPIO_WritePin(&handlerF2, SET);  // Pull the R2 High
		GPIO_WritePin(&handlerF3, SET);
		GPIO_WritePin(&handlerF4, SET);

		if(!(GPIO_ReadPin(&handlerC1))){
			while(!(GPIO_ReadPin(&handlerC1)));
			return '1';

		}

		if(!(GPIO_ReadPin(&handlerC2))){
					while(!(GPIO_ReadPin(&handlerC2)));
					return '2';

				}


		if(!(GPIO_ReadPin(&handlerC3))){
					while(!(GPIO_ReadPin(&handlerC3)));
					return '3';

				}

		if(!(GPIO_ReadPin(&handlerC4))){
					while(!(GPIO_ReadPin(&handlerC4)));
					return 'A';

				}
 // Para fila 2


		GPIO_WritePin(&handlerF1, SET);//Pull the R1 low
		GPIO_WritePin(&handlerF2, RESET);  // Pull the R2 High
		GPIO_WritePin(&handlerF3, SET);
		GPIO_WritePin(&handlerF4, SET);

		if(!(GPIO_ReadPin(&handlerC1))){
					while(!(GPIO_ReadPin(&handlerC1)));
					return '4';

				}

				if(!(GPIO_ReadPin(&handlerC2))){
							while(!(GPIO_ReadPin(&handlerC2)));
							return '5';

						}


				if(!(GPIO_ReadPin(&handlerC3))){
							while(!(GPIO_ReadPin(&handlerC3)));
							return '6';

						}

				if(!(GPIO_ReadPin(&handlerC4))){
							while(!(GPIO_ReadPin(&handlerC4)));
							return 'B';

						}

				// Para fila 3


		GPIO_WritePin(&handlerF1, SET);//Pull the R1 low
		GPIO_WritePin(&handlerF2, SET);  // Pull the R2 High
		GPIO_WritePin(&handlerF3, RESET);
		GPIO_WritePin(&handlerF4, SET);

		if(!(GPIO_ReadPin(&handlerC1))){
					while(!(GPIO_ReadPin(&handlerC1)));
					return '7';

		}

		if(!(GPIO_ReadPin(&handlerC2))){
					while(!(GPIO_ReadPin(&handlerC2)));
					return '8';

				}


		if(!(GPIO_ReadPin(&handlerC3))){
					while(!(GPIO_ReadPin(&handlerC3)));
					return '9';

				}

		if(!(GPIO_ReadPin(&handlerC4))){
					while(!(GPIO_ReadPin(&handlerC4)));
					return 'C';

				}
		// Para fila 4


GPIO_WritePin(&handlerF1, SET);//Pull the R1 low
GPIO_WritePin(&handlerF2, SET);  // Pull the R2 High
GPIO_WritePin(&handlerF3, SET);
GPIO_WritePin(&handlerF4, RESET);

if(!(GPIO_ReadPin(&handlerC1))){
		while(!(GPIO_ReadPin(&handlerC1)));
		return '*';

	}

	if(!(GPIO_ReadPin(&handlerC2))){
				while(!(GPIO_ReadPin(&handlerC2)));
				return '0';

			}


	if(!(GPIO_ReadPin(&handlerC3))){
				while(!(GPIO_ReadPin(&handlerC3)));
				return '#';

			}

	if(!(GPIO_ReadPin(&handlerC4))){
				while(!(GPIO_ReadPin(&handlerC4)));
				return 'D';

	}

}


void parseCommands(char *ptrBufferReception){

	sscanf(ptrBufferReception, " %s %u %u %s" , cmd , &firstParameter , &secondParameter ,userMsg);

	if(strcmp(cmd , "help") == 0){

		writeMsg(&handlerUsart, "Help Menu CMDs: \n");
		writeMsg(&handlerUsart, "1) Millos = Colores del grande \n ");
		writeMsg(&handlerUsart, "2) Nacional= Color verde \n");


	}

	else if(strcmp(cmd ,"Millos") == 0){

//		   writeMsg(&handlerUsart1, "Color purpura \n \r");

		Bandera = 1;


		}

}
