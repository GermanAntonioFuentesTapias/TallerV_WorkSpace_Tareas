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
#include "PwmDriver.h"
#include "ExtiDriver.h"
#include  "KeyPad.h"

GPIO_Handler_t    handlerBlinkyPin   = {0};


//Timer encargado del Blinky
BasicTimer_Handler_t  handlerBlinkyTimer = {0};
BasicTimer_Handler_t  handlerReset    = {0};

GPIO_Handler_t handlerTx     	    	= {0};
GPIO_Handler_t handlerRx	 	     	= {0};
USART_Handler_t        handlerUsart       = {0}; // Handler para el Usart
uint8_t rxData      =  0;
char DataRTC[64] = "Wipi";

/* Banderas interrupciones */
// Interrupciones importantes
//uint8_t stop                            = 1;
uint8_t movimiento_puerta               = 0;
//uint8_t activacion                      = 0;
//uint8_t inicial                         = 1;
//uint8_t opcion4                         = 0;
//uint8_t opcion41                         = 0;

uint8_t Confi1                          = 0;
uint8_t Confi2                          = 0;
uint8_t Confi3                          = 0;
uint8_t Confi4                          = 0;

uint8_t Confi11                         = 0;
uint8_t Confi22                         = 0;
uint8_t Confi33                         = 0;
uint8_t Confi44                         = 0;

uint8_t activarA                        = 0;
uint8_t activarS                        = 0;
uint8_t devolverse                      = 0;
uint8_t desactivar                      = 0;

// Otras interrupciones
uint8_t reset                           = 0;
uint8_t handlerLed                      = 0;
uint8_t Code                            = 0;
uint8_t Fila1                           = 0;
uint8_t Fila2                           = 0;
uint8_t Fila3                           = 0;
uint8_t Fila4                           = 0;
uint8_t Bandera                         = 0;
uint8_t valor                           = 0;
uint8_t Puerta                          = 0;
uint16_t duttyValue                      = 15000;

/* Para Muestro */

uint8_t  Columna1                        = {1};
uint8_t  Columna2                        = {1};
uint8_t  Columna3                        = {1};
uint8_t  Columna4                        = {1};


GPIO_Handler_t	handlerI2CSDA			= {0};
GPIO_Handler_t	handlerI2CSCL 			= {0};
GPIO_Handler_t  handlerSensor           = {0};
GPIO_Handler_t  handlerRojo             = {0};
GPIO_Handler_t  handlerPuerta           = {0};
GPIO_Handler_t  handlerBuzzer           = {0};

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
EXTI_Config_t   handlerF1Ex             = {0};
EXTI_Config_t   handlerF2Ex             = {0};
EXTI_Config_t   handlerF3Ex             = {0};
EXTI_Config_t   handlerF4Ex             = {0};
PWM_Handler_t   handlerPWM              = {0};
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
//int Filas[];
//int Columnas[];

#define LCD_ADRESS		0b0100111

//Definición de las cabeceras del main
void initSystem (void);
//Para el KeyPad
void ConfigKeyPad(void);
// Alarma
void Alarma(void);

//Para las multiples opciones del KeyPad
//void read_keypad (void);

void read_keypad(uint8_t casos);

void Muestreo(void);

//Se utiliza para llamar a la funciones de cada caso
// de manera inmediata

void config_general(void);

// Casos para despliegues de entrada
void config_inicial(uint8_t entrada);
// despliegue general
void menu(void);

// Encargado de lo que activa y desactiva
void menu_activacion(void);

// Encargado de configuraciones

void menu_configuracion(void);

// Encargado de parametros

void menu_parametros(void);

// Encargado de fecha y hora

void fechahora(void);


int main(void){
  	initSystem();
 	ConfigKeyPad();




 	void parseCommands (char *ptrBufferReception);


	LCD_Init(&handlerLCD);
	delay_10();
	LCD_ClearScreen(&handlerLCD);
	delay_10();
//	sprintf(dataLCD, "Alarmas S,A.        Bienvenidos");
	LCD_setCursor(&handlerLCD,2,0);
	LCD_sendSTR(&handlerLCD,"____________________");
	LCD_setCursor(&handlerLCD,0,0);
	LCD_sendSTR(&handlerLCD,"Alarmas German S.A.S");
	LCD_setCursor(&handlerLCD,1,4);
	LCD_sendSTR(&handlerLCD,"Bienvenidos    ");
	LCD_setCursor(&handlerLCD,3,0);
	LCD_sendSTR(&handlerLCD,"Menu con las Letras");

	startCounterTimer(&handlerBlinkyTimer);
	startCounterTimer(&handlerReset);

while (1){


//	    if((reset) && (GPIO_ReadPin(&handlerRojo) == 0)){
//
//	    	Alarma();
//
//	    	reset = 0;
//
//	    }


	    config_general();



        if(((Puerta) ||  (Code)) && ((movimiento_puerta) == 1) ) {
//	      if (GPIO_ReadPin(&handlerSensor) ==  SET){
//            if(!Bandera){
        	updateFrequency(&handlerPWM, 18000);
        	updateDuttyCycle(&handlerPWM, 15000);
        	startPwmSignal(&handlerPWM);
        	enableOutput(&handlerPWM);

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

//	      if(Bandera){
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
//	      			Bandera = 0;
//
//	      		}
//
//	      if((GPIO_ReadPin(&handlerC1) == 1)){
//
//
//	    	  GPIO_WritePin(&handlerRojo, SET);
//	      }




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

//	      if ( stringComplete){
//
//	      			parseCommands(bufferReception);
//
//	      			stringComplete = false;
//	      		}




		}

	    }

void initSystem(void){

	//Configurando el Timer2 para que funcione con el Blinky
		handlerBlinkyTimer.ptrTIMx                            = TIM2;
		handlerBlinkyTimer.TIMx_Config.TIMx_mode              = BTIMER_MODE_UP;
		handlerBlinkyTimer.TIMx_Config.TIMx_speed             = BTIMER_SPEED_100us;
		handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable   = 1;
		handlerBlinkyTimer.TIMx_Config.TIMx_period            = 2500;
		BasicTimer_Config(&handlerBlinkyTimer);

		//Configurando el Timer4 para que funcione para el muestreo

		//MUESTREO

		handlerReset .ptrTIMx                            = TIM4;
		handlerReset .TIMx_Config.TIMx_mode              = BTIMER_MODE_UP;
		handlerReset .TIMx_Config.TIMx_speed             = BTIMER_SPEED_1ms;
		handlerReset .TIMx_Config.TIMx_interruptEnable   = 1;
		handlerReset .TIMx_Config.TIMx_period            = 5000;
		BasicTimer_Config(&handlerReset);

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
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_HIGH;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinAltFunMode		= AF4;

	GPIO_Config(&handlerI2CSCL);



	/* Configurando los pines sobre los que funciona el I2C1 */
	handlerI2CSDA.pGPIOx								= GPIOB;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinNumber			= PIN_7;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_OPENDRAIN;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_HIGH;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinAltFunMode		= AF4;

	GPIO_Config(&handlerI2CSDA);

	handlerLCD.ptrI2Cx		= I2C1;
	handlerLCD.modeI2C		= I2C_MODE_SM;
	handlerLCD.slaveAddress	= LCD_ADRESS;

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

    /* Para PWM del Buzzer */

	handlerBuzzer.pGPIOx                             = GPIOC;
	handlerBuzzer.GPIO_PinConfig.GPIO_PinNumber      = PIN_6;
	handlerBuzzer.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_ALTFN;
	handlerBuzzer.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
	handlerBuzzer.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerBuzzer.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_FAST;
	handlerBuzzer.GPIO_PinConfig.GPIO_PinAltFunMode  = AF2;


	GPIO_Config(&handlerBuzzer);

	// Handler para el PWM

	handlerPWM.ptrTIMx                               = TIM3;
	handlerPWM.config.channel                        = PWM_CHANNEL_1;
	handlerPWM.config.duttyCicle                     = duttyValue;
    handlerPWM.config.periodo                        = 18000;
    handlerPWM.config.prescaler                      = 16;

    pwm_Config(&handlerPWM);

	/* Configuraciones Exti */

	// Sensor

	handlerExtiSensor.pGPIOHandler  = &handlerSensor ;
	handlerExtiSensor.edgeType      = EXTERNAL_INTERRUPT_RISING_EDGE; // Flanco de bajada


	// Puerta

	handlerExtiPuerta.pGPIOHandler  = &handlerPuerta;
	handlerExtiPuerta.edgeType      = EXTERNAL_INTERRUPT_RISING_EDGE;


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
		handlerF1.GPIO_PinConfig.GPIO_PinMode          =  GPIO_MODE_OUT;
		handlerF1.GPIO_PinConfig.GPIO_PinOPType        =  GPIO_OTYPE_PUSHPULL;
		handlerF1.GPIO_PinConfig.GPIO_PinPuPdControl   =  GPIO_PUPDR_NOTHING;
		handlerF1.GPIO_PinConfig.GPIO_PinSpeed         =  GPIO_OSPEED_FAST;

		handlerF2.pGPIOx                               =  GPIOB;
		handlerF2.GPIO_PinConfig.GPIO_PinNumber        =  PIN_9;
		handlerF2.GPIO_PinConfig.GPIO_PinMode          =  GPIO_MODE_OUT;
		handlerF2.GPIO_PinConfig.GPIO_PinOPType        =  GPIO_OTYPE_PUSHPULL;
		handlerF2.GPIO_PinConfig.GPIO_PinPuPdControl   =  GPIO_PUPDR_NOTHING;
		handlerF2.GPIO_PinConfig.GPIO_PinSpeed         =  GPIO_OSPEED_FAST;

		handlerF3.pGPIOx                               =  GPIOC;
		handlerF3.GPIO_PinConfig.GPIO_PinNumber        =  PIN_9;
		handlerF3.GPIO_PinConfig.GPIO_PinMode          =  GPIO_MODE_OUT;
		handlerF3.GPIO_PinConfig.GPIO_PinOPType        =  GPIO_OTYPE_PUSHPULL;
		handlerF3.GPIO_PinConfig.GPIO_PinPuPdControl   =  GPIO_PUPDR_NOTHING;
		handlerF3.GPIO_PinConfig.GPIO_PinSpeed         =  GPIO_OSPEED_FAST;

		handlerF4.pGPIOx                               =  GPIOC;
		handlerF4.GPIO_PinConfig.GPIO_PinNumber        =  PIN_8;
		handlerF4.GPIO_PinConfig.GPIO_PinMode          =  GPIO_MODE_OUT;
		handlerF4.GPIO_PinConfig.GPIO_PinOPType        =  GPIO_OTYPE_PUSHPULL;
		handlerF4.GPIO_PinConfig.GPIO_PinPuPdControl   =  GPIO_PUPDR_NOTHING;
		handlerF4.GPIO_PinConfig.GPIO_PinSpeed         =  GPIO_OSPEED_FAST;

		handlerC1.pGPIOx                               =  GPIOA;
		handlerC1.GPIO_PinConfig.GPIO_PinNumber        =  PIN_12;
		handlerC1.GPIO_PinConfig.GPIO_PinMode          =  GPIO_MODE_IN;
		handlerC1.GPIO_PinConfig.GPIO_PinOPType        =  GPIO_OTYPE_PUSHPULL;
		handlerC1.GPIO_PinConfig.GPIO_PinPuPdControl   =  GPIO_PUPDR_NOTHING;
		handlerC1.GPIO_PinConfig.GPIO_PinSpeed         =  GPIO_OSPEED_FAST;


		handlerC2.pGPIOx                               =  GPIOA;
		handlerC2.GPIO_PinConfig.GPIO_PinNumber        =  PIN_11;
		handlerC2.GPIO_PinConfig.GPIO_PinMode          =  GPIO_MODE_IN;
		handlerC2.GPIO_PinConfig.GPIO_PinOPType        =  GPIO_OTYPE_PUSHPULL;
		handlerC2.GPIO_PinConfig.GPIO_PinPuPdControl   =  GPIO_PUPDR_NOTHING;
		handlerC2.GPIO_PinConfig.GPIO_PinSpeed         =  GPIO_OSPEED_FAST;

		handlerC3.pGPIOx                               =  GPIOB;
		handlerC3.GPIO_PinConfig.GPIO_PinNumber        =  PIN_14;
		handlerC3.GPIO_PinConfig.GPIO_PinMode          =  GPIO_MODE_IN;
		handlerC3.GPIO_PinConfig.GPIO_PinOPType        =  GPIO_OTYPE_PUSHPULL;
		handlerC3.GPIO_PinConfig.GPIO_PinPuPdControl   =  GPIO_PUPDR_NOTHING;
		handlerC3.GPIO_PinConfig.GPIO_PinSpeed         =  GPIO_OSPEED_FAST;

		handlerC4.pGPIOx                               =  GPIOB;
		handlerC4.GPIO_PinConfig.GPIO_PinNumber        =  PIN_13;
		handlerC4.GPIO_PinConfig.GPIO_PinMode          =  GPIO_MODE_IN;
		handlerC4.GPIO_PinConfig.GPIO_PinOPType        =  GPIO_OTYPE_PUSHPULL;
		handlerC4.GPIO_PinConfig.GPIO_PinPuPdControl   =  GPIO_PUPDR_NOTHING;
		handlerC4.GPIO_PinConfig.GPIO_PinSpeed         =  GPIO_OSPEED_FAST;


	     /* Configuración de handlers */

		GPIO_Config(&handlerF1);
		GPIO_Config(&handlerF2);
		GPIO_Config(&handlerF3);
		GPIO_Config(&handlerF4);
		GPIO_Config(&handlerC1);
		GPIO_Config(&handlerC2);
		GPIO_Config(&handlerC3);
		GPIO_Config(&handlerC4);

		//Lecturas Exti

		handlerF1Ex.pGPIOHandler       = &handlerC1;
		handlerF1Ex.edgeType           = EXTERNAL_INTERRUPT_RISING_EDGE;

		handlerF2Ex.pGPIOHandler       = &handlerC2;
		handlerF2Ex.edgeType           = EXTERNAL_INTERRUPT_RISING_EDGE;

		handlerF3Ex.pGPIOHandler       = &handlerC3;
		handlerF3Ex.edgeType           = EXTERNAL_INTERRUPT_RISING_EDGE;

		handlerF4Ex.pGPIOHandler       = &handlerC4;
		handlerF4Ex.edgeType           = EXTERNAL_INTERRUPT_RISING_EDGE;


		/* Cargando configuración */


		extInt_Config(&handlerF1Ex);
		extInt_Config(&handlerF2Ex);
		extInt_Config(&handlerF3Ex);
		extInt_Config(&handlerF4Ex);



}

void BasicTimer2_CallBack(void){

	handlerLed = !handlerLed; // Activación de Blinky

	Muestreo();

}

void BasicTimer4_CallBack(void){

  reset = 1;

}

void USART2Rx_CallBack(void){

	rxData = getRxData();
}

void callback_extInt7(void){


	Code = 1; // Bandera que da inicio a alerta


}

void callback_extInt11(void){

	Fila2 = 1; // Bandera que da inicio a alerta de Key

}

void callback_extInt12(void){

	Fila1 = 1;
}

void callback_extInt13(void){

	Fila4 = 1;
}

void callback_extInt14(void){

	Fila3 = 1;
}

void callback_extInt1(void){

	Puerta = 1;
}


void Alarma(void){

	LCD_ClearScreen(&handlerLCD);
	delay_10();

	LCD_setCursor(&handlerLCD,0,0);
	LCD_sendSTR(&handlerLCD,"____________________");
	LCD_setCursor(&handlerLCD,1,1);
	LCD_sendSTR(&handlerLCD,"Alarmas German S.A");
	LCD_setCursor(&handlerLCD,2,4);
	LCD_sendSTR(&handlerLCD,"Bienvenidos");
	LCD_setCursor(&handlerLCD,3,0);
	LCD_sendSTR(&handlerLCD,"Menu Letras");

}

void Muestreo(void){


	if(Columna1){

		GPIO_WritePin(&handlerF1, SET );
	}else{

		GPIO_WritePin(&handlerF1, RESET );
	}

	if(Columna2){

		GPIO_WritePin(&handlerF2, SET );
	}else{

		GPIO_WritePin(&handlerF2, RESET );
	}

	if(Columna3){

		GPIO_WritePin(&handlerF3, SET );
	}else{

		GPIO_WritePin(&handlerF3, RESET );
	}

	if(Columna4){

		GPIO_WritePin(&handlerF4, SET );
	}else{

		GPIO_WritePin(&handlerF4, RESET );
	}

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

//void read_keypad(uint8_t casos){
//
//	switch(casos){
//
//	/* Columna 1 */
//
//		case 11:{
//
//			if(Fila1){
//
//                /* Fila 1- Columna 1 */
//		    	GPIO_WritePin(&handlerF1, SET);
//		    	GPIO_WritePin(&handlerF2, RESET);
//		    	GPIO_WritePin(&handlerF3, RESET);
//		    	GPIO_WritePin(&handlerF4, RESET);
//
//		    	if((GPIO_ReadPin(&handlerF1) == 1) && GPIO_ReadPin(&handlerC1) == 1){
//
//		    		LCD_ClearScreen(&handlerLCD);
//					LCD_setCursor(&handlerLCD,0,1);
//					LCD_sendSTR(&handlerLCD,"Letra = 1 ");
//					LCD_setCursor(&handlerLCD,2,6);
//					LCD_sendSTR(&handlerLCD,"Presionada");
//
//					Fila1 = 0;
//		    	}
//
//		    	break;
// 	     }
//		}
//
//		    	 /* Fila 2 - Columna 1 */
//		case 21:{
//
//			if(Fila1){
//		    	GPIO_WritePin(&handlerF1, RESET);
//				GPIO_WritePin(&handlerF2, SET);
//				GPIO_WritePin(&handlerF3, RESET);
//				GPIO_WritePin(&handlerF4, RESET);
//
//				 if((GPIO_ReadPin(&handlerF2) == 1) && GPIO_ReadPin(&handlerC1) == 1){
//
//				LCD_ClearScreen(&handlerLCD);
//				LCD_setCursor(&handlerLCD,0,1);
//				LCD_sendSTR(&handlerLCD,"Letra = 4 ");
//				LCD_setCursor(&handlerLCD,2,6);
//				LCD_sendSTR(&handlerLCD,"Presionada");
//
//				Fila1 = 0;
//		    		  	    	}
//
//				 break;
//		}
//
//		}
//
//		case 31:{
//
//				 /* Fila 3 - Columna 1 */
//
//			if(Fila1){
//
//				GPIO_WritePin(&handlerF1, RESET);
//				GPIO_WritePin(&handlerF2, RESET);
//				GPIO_WritePin(&handlerF3, SET);
//				GPIO_WritePin(&handlerF4, RESET);
//
//				 if((GPIO_ReadPin(&handlerF3) == 1) && GPIO_ReadPin(&handlerC1) == 1){
//
//				LCD_ClearScreen(&handlerLCD);
//				LCD_setCursor(&handlerLCD,0,1);
//				LCD_sendSTR(&handlerLCD,"Letra = 7 ");
//				LCD_setCursor(&handlerLCD,2,6);
//				LCD_sendSTR(&handlerLCD,"Presionada");
//
//				Fila1 = 0;
//								}
//
//				 break;
//
//		     }
//
//		}
//
//		case 41:{
//
//				 /* Fila 4- Columna 1 */
//
//			if(Fila1){
//
//				GPIO_WritePin(&handlerF1, RESET);
//				GPIO_WritePin(&handlerF2, RESET);
//				GPIO_WritePin(&handlerF3, RESET);
//				GPIO_WritePin(&handlerF4, SET);
//
//				 if((GPIO_ReadPin(&handlerF4) == 1) && GPIO_ReadPin(&handlerC1) == 1){
//
//				LCD_ClearScreen(&handlerLCD);
//				LCD_setCursor(&handlerLCD,0,1);
//				LCD_sendSTR(&handlerLCD,"Letra = * ");
//				LCD_setCursor(&handlerLCD,2,6);
//				LCD_sendSTR(&handlerLCD,"Presionada");
//
//				Fila1 = 0;
//								}
//				 break;
//		}
//		}
//
//
//
//	       // Me cierra Columna 1
//
//                /* Opciones para siguiente columna */
//		   case 12:{
//
//
//				 if(Fila2){
//
//		    /* Fila 1 - Columna 2 */
//
//			GPIO_WritePin(&handlerF1, SET);
//			GPIO_WritePin(&handlerF2, RESET);
//			GPIO_WritePin(&handlerF3, RESET);
//			GPIO_WritePin(&handlerF4, RESET);
//
//			if((GPIO_ReadPin(&handlerF1) == 1) && GPIO_ReadPin(&handlerC2) == 1){
//
//			LCD_ClearScreen(&handlerLCD);
//			LCD_setCursor(&handlerLCD,0,1);
//			LCD_sendSTR(&handlerLCD,"Letra = 2 ");
//			LCD_setCursor(&handlerLCD,2,6);
//			LCD_sendSTR(&handlerLCD,"Presionada");
//
//				Fila2 = 0;
//			}
//
//			break;
//
//				 }
//
//		   }
//
//			/* Fila 2 - Columna  2*/
//		   case 22:{
//
//
//		   if(Fila2){
//
//
//			GPIO_WritePin(&handlerF1, RESET);
//			GPIO_WritePin(&handlerF2, SET);
//			GPIO_WritePin(&handlerF3, RESET);
//			GPIO_WritePin(&handlerF4, RESET);
//
//			if((GPIO_ReadPin(&handlerF2) == 1) && GPIO_ReadPin(&handlerC2) == 1){
//
//			LCD_ClearScreen(&handlerLCD);
//			LCD_setCursor(&handlerLCD,0,1);
//			LCD_sendSTR(&handlerLCD,"Letra = 5 ");
//			LCD_setCursor(&handlerLCD,2,6);
//			LCD_sendSTR(&handlerLCD,"Presionada");
//
//			Fila2 = 0;
//			}
//
//	       break;
//
//		   }
//		   } // Cierra el case
//
//			 /* Fila 3- Columna 2 */
//		   case 32:{
//
//			 if(Fila2){
//
//		    GPIO_WritePin(&handlerF1, RESET);
//			GPIO_WritePin(&handlerF2, RESET);
//			GPIO_WritePin(&handlerF3, SET);
//			GPIO_WritePin(&handlerF4, RESET);
//
//			 if((GPIO_ReadPin(&handlerF3) == 1) && GPIO_ReadPin(&handlerC2) == 1){
//
//			LCD_ClearScreen(&handlerLCD);
//			LCD_setCursor(&handlerLCD,0,1);
//			LCD_sendSTR(&handlerLCD,"Letra = 8 ");
//			LCD_setCursor(&handlerLCD,2,6);
//			LCD_sendSTR(&handlerLCD,"Presionada");
//
//			Fila2 = 0;
//							}
//
//			 break;
//
//		   }
//
//	       } // Cierra el case
//
//
//			 /* Fila 4- Columna 2 */
//
//		   case 42:{
//
//			   if(Fila2){
//
//			GPIO_WritePin(&handlerF1, RESET);
//			GPIO_WritePin(&handlerF2, RESET);
//			GPIO_WritePin(&handlerF3, RESET);
//			GPIO_WritePin(&handlerF4, SET);
//
//			 if((GPIO_ReadPin(&handlerF4) == 1) && GPIO_ReadPin(&handlerC2) == 1){
//
//			LCD_ClearScreen(&handlerLCD);
//			LCD_setCursor(&handlerLCD,0,1);
//			LCD_sendSTR(&handlerLCD,"Letra = 0 ");
//			LCD_setCursor(&handlerLCD,2,6);
//			LCD_sendSTR(&handlerLCD,"Presionada");
//
//			Fila2 = 0;
//							}
//
//             break;
//
//		     }
//			 }// Cierre columna 2
//
//			 /* Columna 3 */
//
//		   case 13: {
//
//
//			 if(Fila3){
//
//			/* Fila 1 - Columna 3 */
//
//			GPIO_WritePin(&handlerF1, SET);
//			GPIO_WritePin(&handlerF2, RESET);
//			GPIO_WritePin(&handlerF3, RESET);
//			GPIO_WritePin(&handlerF4, RESET);
//
//			if((GPIO_ReadPin(&handlerF1) == 1) && GPIO_ReadPin(&handlerC3) == 1){
//
//			LCD_ClearScreen(&handlerLCD);
//			LCD_setCursor(&handlerLCD,0,1);
//			LCD_sendSTR(&handlerLCD,"Letra = 3 ");
//			LCD_setCursor(&handlerLCD,2,6);
//			LCD_sendSTR(&handlerLCD,"Presionada");
//
//				Fila3 = 0;
//			}
//
//			break;
//
//			 }
//
//		   }
//
//			/* Fila 2 - Columna  3*/
//
//		   case 23:{
//
//			 if(Fila3){
//
//			GPIO_WritePin(&handlerF1, RESET);
//			GPIO_WritePin(&handlerF2, SET);
//			GPIO_WritePin(&handlerF3, RESET);
//			GPIO_WritePin(&handlerF4, RESET);
//
//			if((GPIO_ReadPin(&handlerF2) == 1) && GPIO_ReadPin(&handlerC3) == 1){
//
//			LCD_ClearScreen(&handlerLCD);
//			LCD_setCursor(&handlerLCD,0,1);
//			LCD_sendSTR(&handlerLCD,"Letra = 6 ");
//			LCD_setCursor(&handlerLCD,2,6);
//			LCD_sendSTR(&handlerLCD,"Presionada");
//
//			Fila3 = 0;
//			}
//
//			break;
//
//			 }
//
//		   }
//
//			 /* Fila 3- Columna 3 */
//
//		   case 33:{
//
//			if(Fila3){
//
//			GPIO_WritePin(&handlerF1, RESET);
//			GPIO_WritePin(&handlerF2, RESET);
//			GPIO_WritePin(&handlerF3, SET);
//			GPIO_WritePin(&handlerF4, RESET);
//
//			 if((GPIO_ReadPin(&handlerF3) == 1) && GPIO_ReadPin(&handlerC3) == 1){
//
//			LCD_ClearScreen(&handlerLCD);
//			LCD_setCursor(&handlerLCD,0,1);
//			LCD_sendSTR(&handlerLCD,"Letra = 9 ");
//			LCD_setCursor(&handlerLCD,2,6);
//			LCD_sendSTR(&handlerLCD,"Presionada");
//
//			Fila3 = 0;
//							}
//
//			 break;
//
//			}
//
//		   }
//
//
//			 /* Fila 4- Columna 3 */
//
//		   case 43:{
//
//		    if(Fila3){
//
//			GPIO_WritePin(&handlerF1, RESET);
//			GPIO_WritePin(&handlerF2, RESET);
//			GPIO_WritePin(&handlerF3, RESET);
//			GPIO_WritePin(&handlerF4, SET);
//
//			 if((GPIO_ReadPin(&handlerF4) == 1) && GPIO_ReadPin(&handlerC3) == 1){
//
//			LCD_ClearScreen(&handlerLCD);
//			LCD_setCursor(&handlerLCD,0,1);
//			LCD_sendSTR(&handlerLCD,"Letra = # ");
//			LCD_setCursor(&handlerLCD,2,6);
//			LCD_sendSTR(&handlerLCD,"Presionada");
//
//			Fila3 = 0;
//							}
//
//			 break;
//
//		    }
//
//			 }
//
//			 /* Nueva columna  4*/
//
//		   case 14:{
//
//
//			 if(Fila4){
//
//			/* Fila 1 - Columna 4*/
//
//			GPIO_WritePin(&handlerF1, SET);
//			GPIO_WritePin(&handlerF2, RESET);
//			GPIO_WritePin(&handlerF3, RESET);
//			GPIO_WritePin(&handlerF4, RESET);
//
//			if((GPIO_ReadPin(&handlerF1) == 1) && GPIO_ReadPin(&handlerC4) == 1){
//
//			LCD_ClearScreen(&handlerLCD);
//			LCD_setCursor(&handlerLCD,0,1);
//			LCD_sendSTR(&handlerLCD,"Letra = A ");
//			LCD_setCursor(&handlerLCD,2,6);
//			LCD_sendSTR(&handlerLCD,"Presionada");
//
//				Fila4 = 0;
//			}
//
//			break;
//
//			 }
//
//		   }
//
//			/* Fila 2 - Columna  4*/
//
//		   case 24:{
//
//			if(Fila4){
//
//
//			GPIO_WritePin(&handlerF1, RESET);
//			GPIO_WritePin(&handlerF2, SET);
//			GPIO_WritePin(&handlerF3, RESET);
//			GPIO_WritePin(&handlerF4, RESET);
//
//			if((GPIO_ReadPin(&handlerF2) == 1) && GPIO_ReadPin(&handlerC4) == 1){
//
//			LCD_ClearScreen(&handlerLCD);
//			LCD_setCursor(&handlerLCD,0,1);
//			LCD_sendSTR(&handlerLCD,"Letra = B ");
//			LCD_setCursor(&handlerLCD,2,6);
//			LCD_sendSTR(&handlerLCD,"Presionada");
//
//			Fila4 = 0;
//			}
//
//			break;
//
//			}
//
//		   }
//
//			 /* Fila 3- Columna 4 */
//
//		   case 34:{
//
//
//			GPIO_WritePin(&handlerF1, RESET);
//			GPIO_WritePin(&handlerF2, RESET);
//			GPIO_WritePin(&handlerF3, SET);
//			GPIO_WritePin(&handlerF4, RESET);
//
//			 if((GPIO_ReadPin(&handlerF3) == 1) && GPIO_ReadPin(&handlerC4) == 1){
//
//			LCD_ClearScreen(&handlerLCD);
//			LCD_setCursor(&handlerLCD,0,1);
//			LCD_sendSTR(&handlerLCD,"Letra = C ");
//			LCD_setCursor(&handlerLCD,2,6);
//			LCD_sendSTR(&handlerLCD,"Presionada");
//
//			Fila4 = 0;
//							}
//
//			 break;
//
//		   }
//
//
//			 /* Fila 4- Columna 4 */
//
//		   case 44: {
//
//			if(Fila4){
//
//			GPIO_WritePin(&handlerF1, RESET);
//			GPIO_WritePin(&handlerF2, RESET);
//			GPIO_WritePin(&handlerF3, RESET);
//			GPIO_WritePin(&handlerF4, SET);
//
//			 if((GPIO_ReadPin(&handlerF4) == 1) && GPIO_ReadPin(&handlerC4) == 1){
//
//			LCD_ClearScreen(&handlerLCD);
//			LCD_setCursor(&handlerLCD,0,1);
//			LCD_sendSTR(&handlerLCD,"Letra = D ");
//			LCD_setCursor(&handlerLCD,2,6);
//			LCD_sendSTR(&handlerLCD,"Presionada");
//
//			Fila4 = 0;
//							}
//
//		    break;
//			}
//		 } // Cierre Col 4
//
//		   default:
//
//			   __NOP();
//
//	  }
//    }

void config_general(void){

     // El stop es para que no se devuelva la función sin querer
//    if((Fila1) && (stop)) {

	if((Fila1)){
    config_inicial(11);
    config_inicial(21);
    config_inicial(31);
    config_inicial(41);

//	read_keypad(11);
//	read_keypad(21);
//	read_keypad(31);
//	read_keypad(41);

	Fila1=0;
    }

//    if((Fila2) && (stop)){
    if((Fila2)){

    config_inicial(12);
	config_inicial(22);
	config_inicial(32);
	config_inicial(42);
//
//	read_keypad(12);
//	read_keypad(22);
//	read_keypad(32);
//	read_keypad(42);

	Fila2=0;

    }

//    if((Fila3) && (stop)){

    if((Fila3)){

    	config_inicial(13);
		config_inicial(23);
		config_inicial(33);
		config_inicial(43);

//	read_keypad(13);
//	read_keypad(23);
//	read_keypad(33);
//	read_keypad(43);

	Fila3=0;

    }

//    if((Fila4) && (stop)){

    	if((Fila4)){

    config_inicial(14);
	config_inicial(24);
	config_inicial(34);
	config_inicial(44);

//	read_keypad(14);
//	read_keypad(24);
//	read_keypad(34);
//	read_keypad(44);

	Fila4=0;

    }


}

void config_inicial(uint8_t entrada){



	switch(entrada){

		/* Columna 1 */

			case 11:{

				if(Fila1){

				/* Fila 1- Columna 1 */
				GPIO_WritePin(&handlerF1, SET);
				GPIO_WritePin(&handlerF2, RESET);
				GPIO_WritePin(&handlerF3, RESET);
				GPIO_WritePin(&handlerF4, RESET);

				if((GPIO_ReadPin(&handlerF1) == 1) && GPIO_ReadPin(&handlerC1) == 1){

				LCD_ClearScreen(&handlerLCD);
				LCD_setCursor(&handlerLCD,0,0);
				LCD_sendSTR(&handlerLCD,"____________________");
				LCD_setCursor(&handlerLCD,1,1);
				LCD_sendSTR(&handlerLCD,"Alarmas German S.A");
				LCD_setCursor(&handlerLCD,2,4);
				LCD_sendSTR(&handlerLCD,"Bienvenidos");
				LCD_setCursor(&handlerLCD,3,0);
				LCD_sendSTR(&handlerLCD,"____Menu  Letras____");


					Fila1 = 0;
				}

			    	break;
	 	     }
			}

			    	 /* Fila 2 - Columna 1 */
			case 21:{

				if(Fila1){
				GPIO_WritePin(&handlerF1, RESET);
				GPIO_WritePin(&handlerF2, SET);
				GPIO_WritePin(&handlerF3, RESET);
				GPIO_WritePin(&handlerF4, RESET);

				 if((GPIO_ReadPin(&handlerF2) == 1) && GPIO_ReadPin(&handlerC1) == 1){

				 LCD_ClearScreen(&handlerLCD);
				LCD_setCursor(&handlerLCD,0,0);
				LCD_sendSTR(&handlerLCD,"____________________");
				LCD_setCursor(&handlerLCD,1,1);
				LCD_sendSTR(&handlerLCD,"Alarmas German S.A");
				LCD_setCursor(&handlerLCD,2,4);
				LCD_sendSTR(&handlerLCD,"Bienvenidos");
				LCD_setCursor(&handlerLCD,3,0);
				LCD_sendSTR(&handlerLCD,"____Menu  Letras____");

				Fila1 = 0;
							}

			 break;
			}

			}

			case 31:{

					 /* Fila 3 - Columna 1 */

				if(Fila1){

				GPIO_WritePin(&handlerF1, RESET);
				GPIO_WritePin(&handlerF2, RESET);
				GPIO_WritePin(&handlerF3, SET);
				GPIO_WritePin(&handlerF4, RESET);

				 if((GPIO_ReadPin(&handlerF3) == 1) && GPIO_ReadPin(&handlerC1) == 1){

				 LCD_ClearScreen(&handlerLCD);
				LCD_setCursor(&handlerLCD,0,0);
				LCD_sendSTR(&handlerLCD,"____________________");
				LCD_setCursor(&handlerLCD,1,1);
				LCD_sendSTR(&handlerLCD,"Alarmas German S.A");
				LCD_setCursor(&handlerLCD,2,4);
				LCD_sendSTR(&handlerLCD,"Bienvenidos");
				LCD_setCursor(&handlerLCD,3,0);
				LCD_sendSTR(&handlerLCD,"____Menu  Letras____");


				Fila1 = 0;
								}

				 break;

			 }

			}

			case 41:{

					 /* Fila 4- Columna 1 */

				if(Fila1){

				GPIO_WritePin(&handlerF1, RESET);
				GPIO_WritePin(&handlerF2, RESET);
				GPIO_WritePin(&handlerF3, RESET);
				GPIO_WritePin(&handlerF4, SET);

				 if((GPIO_ReadPin(&handlerF4) == 1) && GPIO_ReadPin(&handlerC1) == 1){

					LCD_ClearScreen(&handlerLCD);
					LCD_setCursor(&handlerLCD,0,0);
					LCD_sendSTR(&handlerLCD,"____________________");
					LCD_setCursor(&handlerLCD,1,1);
					LCD_sendSTR(&handlerLCD,"Alarmas German S.A");
					LCD_setCursor(&handlerLCD,2,4);
					LCD_sendSTR(&handlerLCD,"Bienvenidos");
					LCD_setCursor(&handlerLCD,3,0);
					LCD_sendSTR(&handlerLCD,"____Menu  Letras____");

				Fila1 = 0;
								}
					 break;
			}
			}

		       // Me cierra Columna 1

	                /* Opciones para siguiente columna */
			   case 12:{


					 if(Fila2){

			    /* Fila 1 - Columna 2 */

				GPIO_WritePin(&handlerF1, SET);
				GPIO_WritePin(&handlerF2, RESET);
				GPIO_WritePin(&handlerF3, RESET);
				GPIO_WritePin(&handlerF4, RESET);

				if((GPIO_ReadPin(&handlerF1) == 1) && GPIO_ReadPin(&handlerC2) == 1){

				LCD_ClearScreen(&handlerLCD);
				LCD_setCursor(&handlerLCD,0,0);
				LCD_sendSTR(&handlerLCD,"____________________");
				LCD_setCursor(&handlerLCD,1,1);
				LCD_sendSTR(&handlerLCD,"Alarmas German S.A");
				LCD_setCursor(&handlerLCD,2,4);
				LCD_sendSTR(&handlerLCD,"Bienvenidos");
				LCD_setCursor(&handlerLCD,3,0);
				LCD_sendSTR(&handlerLCD,"____Menu  Letras____");


					Fila2 = 0;
				}

				break;

					 }

			   }

				/* Fila 2 - Columna  2*/
			   case 22:{


			   if(Fila2){


				GPIO_WritePin(&handlerF1, RESET);
				GPIO_WritePin(&handlerF2, SET);
				GPIO_WritePin(&handlerF3, RESET);
				GPIO_WritePin(&handlerF4, RESET);

				if((GPIO_ReadPin(&handlerF2) == 1) && GPIO_ReadPin(&handlerC2) == 1){

				LCD_ClearScreen(&handlerLCD);
				LCD_setCursor(&handlerLCD,0,0);
				LCD_sendSTR(&handlerLCD,"____________________");
				LCD_setCursor(&handlerLCD,1,1);
				LCD_sendSTR(&handlerLCD,"Alarmas German S.A");
				LCD_setCursor(&handlerLCD,2,4);
				LCD_sendSTR(&handlerLCD,"Bienvenidos");
				LCD_setCursor(&handlerLCD,3,0);
				LCD_sendSTR(&handlerLCD,"____Menu  Letras____");


				Fila2 = 0;
				}

		       break;

			   }
			   } // Cierra el case

				 /* Fila 3- Columna 2 */
			   case 32:{

				 if(Fila2){

			    GPIO_WritePin(&handlerF1, RESET);
				GPIO_WritePin(&handlerF2, RESET);
				GPIO_WritePin(&handlerF3, SET);
				GPIO_WritePin(&handlerF4, RESET);

				 if((GPIO_ReadPin(&handlerF3) == 1) && GPIO_ReadPin(&handlerC2) == 1){

				 LCD_ClearScreen(&handlerLCD);
				LCD_setCursor(&handlerLCD,0,0);
				LCD_sendSTR(&handlerLCD,"____________________");
				LCD_setCursor(&handlerLCD,1,1);
				LCD_sendSTR(&handlerLCD,"Alarmas German S.A");
				LCD_setCursor(&handlerLCD,2,4);
				LCD_sendSTR(&handlerLCD,"Bienvenidos");
				LCD_setCursor(&handlerLCD,3,0);
				LCD_sendSTR(&handlerLCD,"____Menu  Letras____");


				Fila2 = 0;
								}

				 break;

			   }

		       }


				 /* Fila 4- Columna 2 */

			   case 42:{

				   if(Fila2){

				GPIO_WritePin(&handlerF1, RESET);
				GPIO_WritePin(&handlerF2, RESET);
				GPIO_WritePin(&handlerF3, RESET);
				GPIO_WritePin(&handlerF4, SET);

				 if((GPIO_ReadPin(&handlerF4) == 1) && GPIO_ReadPin(&handlerC2) == 1){

			// Menu

					LCD_ClearScreen(&handlerLCD);
					LCD_setCursor(&handlerLCD,0,0);
					LCD_sendSTR(&handlerLCD,"____________________");
					LCD_setCursor(&handlerLCD,1,1);
					LCD_sendSTR(&handlerLCD,"Alarmas German S.A");
					LCD_setCursor(&handlerLCD,2,4);
					LCD_sendSTR(&handlerLCD,"Bienvenidos");
					LCD_setCursor(&handlerLCD,3,0);
					LCD_sendSTR(&handlerLCD,"____Menu  Letras____");


				Fila2 = 0;
								}

	             break;

			     }
				 }// Cierre columna 2

				 /* Columna 3 */

			   case 13: {


				 if(Fila3){

				/* Fila 1 - Columna 3 */

				GPIO_WritePin(&handlerF1, SET);
				GPIO_WritePin(&handlerF2, RESET);
				GPIO_WritePin(&handlerF3, RESET);
				GPIO_WritePin(&handlerF4, RESET);

				if((GPIO_ReadPin(&handlerF1) == 1) && GPIO_ReadPin(&handlerC3) == 1){

					LCD_ClearScreen(&handlerLCD);
					LCD_setCursor(&handlerLCD,0,0);
					LCD_sendSTR(&handlerLCD,"____________________");
					LCD_setCursor(&handlerLCD,1,1);
					LCD_sendSTR(&handlerLCD,"Alarmas German S.A");
					LCD_setCursor(&handlerLCD,2,4);
					LCD_sendSTR(&handlerLCD,"Bienvenidos");
					LCD_setCursor(&handlerLCD,3,0);
					LCD_sendSTR(&handlerLCD,"____Menu  Letras____");


					Fila3 = 0;
				}

				break;

				 }

			   }

				/* Fila 2 - Columna  3*/

			   case 23:{

				 if(Fila3){

				GPIO_WritePin(&handlerF1, RESET);
				GPIO_WritePin(&handlerF2, SET);
				GPIO_WritePin(&handlerF3, RESET);
				GPIO_WritePin(&handlerF4, RESET);

				if((GPIO_ReadPin(&handlerF2) == 1) && GPIO_ReadPin(&handlerC3) == 1){

					LCD_ClearScreen(&handlerLCD);
					LCD_setCursor(&handlerLCD,0,0);
					LCD_sendSTR(&handlerLCD,"____________________");
					LCD_setCursor(&handlerLCD,1,1);
					LCD_sendSTR(&handlerLCD,"Alarmas German S.A");
					LCD_setCursor(&handlerLCD,2,4);
					LCD_sendSTR(&handlerLCD,"Bienvenidos");
					LCD_setCursor(&handlerLCD,3,0);
					LCD_sendSTR(&handlerLCD,"____Menu  Letras____");

				Fila3 = 0;
				}

				break;

				 }

			   }

				 /* Fila 3- Columna 3 */

			   case 33:{

				if(Fila3){

				GPIO_WritePin(&handlerF1, RESET);
				GPIO_WritePin(&handlerF2, RESET);
				GPIO_WritePin(&handlerF3, SET);
				GPIO_WritePin(&handlerF4, RESET);

				 if((GPIO_ReadPin(&handlerF3) == 1) && GPIO_ReadPin(&handlerC3) == 1){

					LCD_ClearScreen(&handlerLCD);
					LCD_setCursor(&handlerLCD,0,0);
					LCD_sendSTR(&handlerLCD,"____________________");
					LCD_setCursor(&handlerLCD,1,1);
					LCD_sendSTR(&handlerLCD,"Alarmas German S.A");
					LCD_setCursor(&handlerLCD,2,4);
					LCD_sendSTR(&handlerLCD,"Bienvenidos");
					LCD_setCursor(&handlerLCD,3,0);
					LCD_sendSTR(&handlerLCD,"____Menu  Letras____");


				Fila3 = 0;
								}

				 break;

				}

			   }


				 /* Fila 4- Columna 3 */

			   case 43:{

			    if(Fila3){

				GPIO_WritePin(&handlerF1, RESET);
				GPIO_WritePin(&handlerF2, RESET);
				GPIO_WritePin(&handlerF3, RESET);
				GPIO_WritePin(&handlerF4, SET);

				 if((GPIO_ReadPin(&handlerF4) == 1) && GPIO_ReadPin(&handlerC3) == 1){

					LCD_ClearScreen(&handlerLCD);
					LCD_setCursor(&handlerLCD,0,0);
					LCD_sendSTR(&handlerLCD,"____________________");
					LCD_setCursor(&handlerLCD,1,1);
					LCD_sendSTR(&handlerLCD,"Alarmas German S.A");
					LCD_setCursor(&handlerLCD,2,4);
					LCD_sendSTR(&handlerLCD,"Bienvenidos");
					LCD_setCursor(&handlerLCD,3,0);
					LCD_sendSTR(&handlerLCD,"____Menu  Letras____");

				Fila3 = 0;
								}

				 break;

			    }

				 }

				 /* Nueva columna  4*/

			   case 14:{


				 if(Fila4){

				/* Fila 1 - Columna 4*/

				GPIO_WritePin(&handlerF1, SET);
				GPIO_WritePin(&handlerF2, RESET);
				GPIO_WritePin(&handlerF3, RESET);
				GPIO_WritePin(&handlerF4, RESET);

				if((GPIO_ReadPin(&handlerF1) == 1) && GPIO_ReadPin(&handlerC4) == 1){



				 Confi1 = 1;
				 Confi2 = 1;
				 Confi3 = 1;
				 Confi4 = 1;


				 LCD_ClearScreen(&handlerLCD);
				LCD_setCursor(&handlerLCD,0,0);
				LCD_sendSTR(&handlerLCD,"A| Configuracion");
				LCD_setCursor(&handlerLCD,1,0);
				LCD_sendSTR(&handlerLCD,"B| Parametros");
				LCD_setCursor(&handlerLCD,2,0);
				LCD_sendSTR(&handlerLCD,"C| Activacion");
				LCD_setCursor(&handlerLCD,3,0);
				LCD_sendSTR(&handlerLCD,"D| Fecha y hora");

//
//			menu();
//
//					opcion4 = 1;


					Fila4 = 0;
				}


				 }

				 if((Fila4) && ((Confi1) == 1)){

					GPIO_WritePin(&handlerF1, SET);
					GPIO_WritePin(&handlerF2, RESET);
					GPIO_WritePin(&handlerF3, RESET);
					GPIO_WritePin(&handlerF4, RESET);

					 if((GPIO_ReadPin(&handlerF1) == 1) && GPIO_ReadPin(&handlerC4) == 1){


							LCD_ClearScreen(&handlerLCD);
							LCD_setCursor(&handlerLCD,0,0);
							LCD_sendSTR(&handlerLCD,"A| Activar alarmas");
							LCD_setCursor(&handlerLCD,1,0);
							LCD_sendSTR(&handlerLCD,"B| Activar sensor");
							LCD_setCursor(&handlerLCD,2,0);
							LCD_sendSTR(&handlerLCD,"C| Desactivar");
							LCD_setCursor(&handlerLCD,3,0);
							LCD_sendSTR(&handlerLCD,"D| Devolverse");


						 activarA = 1;
						 activarS = 1;
						 desactivar=1;
						 devolverse=1;

						 Fila4 = 0;

						 Confi1 = 0;
						 Confi2 = 0;
						 Confi3 = 0;
						 Confi4 = 0;




					 }


				 } // Cierre condicional de  Confi


				 if((Fila4) && ( (activarA)== 1)){


					 activarA = 0;
					 activarS = 0;
					 desactivar=0;
					 devolverse=0;

					 movimiento_puerta = 1;

					 Fila4= 0;


				 }



				 break;

			   }

				/* Fila 2 - Columna  4*/

			   case 24:{

				if(Fila4){


				GPIO_WritePin(&handlerF1, RESET);
				GPIO_WritePin(&handlerF2, SET);
				GPIO_WritePin(&handlerF3, RESET);
				GPIO_WritePin(&handlerF4, RESET);

				if((GPIO_ReadPin(&handlerF2) == 1) && GPIO_ReadPin(&handlerC4) == 1){

					 Confi1 = 1;
					 Confi2 = 1;
					 Confi3 = 1;
					 Confi4 = 1;

					LCD_ClearScreen(&handlerLCD);
					LCD_setCursor(&handlerLCD,0,0);
					LCD_sendSTR(&handlerLCD,"A| Configuracion");
					LCD_setCursor(&handlerLCD,1,0);
					LCD_sendSTR(&handlerLCD,"B| Parametros");
					LCD_setCursor(&handlerLCD,2,0);
					LCD_sendSTR(&handlerLCD,"C| Activacion");
					LCD_setCursor(&handlerLCD,3,0);
					LCD_sendSTR(&handlerLCD,"D| Fecha y hora");

//					menu();
//
//					opcion4 = 1;
//
				Fila4 = 0;
				}

				break;

				}

			   }

				 /* Fila 3- Columna 4 */

			   case 34:{


				GPIO_WritePin(&handlerF1, RESET);
				GPIO_WritePin(&handlerF2, RESET);
				GPIO_WritePin(&handlerF3, SET);
				GPIO_WritePin(&handlerF4, RESET);

				 if((GPIO_ReadPin(&handlerF3) == 1) && GPIO_ReadPin(&handlerC4) == 1){

					 LCD_ClearScreen(&handlerLCD);
					LCD_setCursor(&handlerLCD,0,0);
					LCD_sendSTR(&handlerLCD,"A| Configuracion");
					LCD_setCursor(&handlerLCD,1,0);
					LCD_sendSTR(&handlerLCD,"B| Parametros");
					LCD_setCursor(&handlerLCD,2,0);
					LCD_sendSTR(&handlerLCD,"C| Activacion");
					LCD_setCursor(&handlerLCD,3,0);
					LCD_sendSTR(&handlerLCD,"D| Fecha y hora");


					 Confi1 = 1;
					 Confi2 = 1;
					 Confi3 = 1;
					 Confi4 = 1;


//					 stop = 0;
//
//					 menu();
//
//					 menu_activacion();
//					 menu_parametros();
//					 opcion4 = 1;


				Fila4 = 0;
								}

				 break;

			   }


				 /* Fila 4- Columna 4 */

			   case 44: {

				if(Fila4){

				GPIO_WritePin(&handlerF1, RESET);
				GPIO_WritePin(&handlerF2, RESET);
				GPIO_WritePin(&handlerF3, RESET);
				GPIO_WritePin(&handlerF4, SET);

				 if((GPIO_ReadPin(&handlerF4) == 1) && GPIO_ReadPin(&handlerC4) == 1){


					 Confi1 = 1;
					 Confi2 = 1;
					 Confi3 = 1;
					 Confi4 = 1;

					LCD_ClearScreen(&handlerLCD);
					LCD_setCursor(&handlerLCD,0,0);
					LCD_sendSTR(&handlerLCD,"A| Configuracion");
					LCD_setCursor(&handlerLCD,1,0);
					LCD_sendSTR(&handlerLCD,"B| Parametros");
					LCD_setCursor(&handlerLCD,2,0);
					LCD_sendSTR(&handlerLCD,"C| Activacion");
					LCD_setCursor(&handlerLCD,3,0);
					LCD_sendSTR(&handlerLCD,"D| Fecha y hora");

//
//					opcion4 = 1;




				Fila4 = 0;
								}

			    break;
				}
			 } // Cierre Col 4

			   default:

				   __NOP();

		  }


}


//void parseCommands(char *ptrBufferReception){
//
//	sscanf(ptrBufferReception, " %s %u %u %s" , cmd , &firstParameter , &secondParameter ,userMsg);
//
//	if(strcmp(cmd , "help") == 0){
//
//		writeMsg(&handlerUsart, "Help Menu CMDs: \n");
//		writeMsg(&handlerUsart, "1) Millos = Colores del grande \n ");
//		writeMsg(&handlerUsart, "2) Nacional= Color verde \n");
//
//
//	}
//
//	else if(strcmp(cmd ,"Millos") == 0){
//
////		   writeMsg(&handlerUsart1, "Color purpura \n \r");
//
//		Bandera = 1;
//
//
//		}
//
//	else if(strcmp(cmd ,"Nacional") == 0){
//
//		updateFrequency(&handlerPWM, 0);
//		  updateDuttyCycle(&handlerPWM, RESET);
//	       stopPwmSignal(&handlerPWM);
//
//
//}
//
//}

//void menu(void){
//
//    /* Se viene de Configuracion General */
//
// // Aqui empieza la magia a revisar, al dar click al menu
//	LCD_ClearScreen(&handlerLCD);
//	LCD_setCursor(&handlerLCD,0,0);
//	LCD_sendSTR(&handlerLCD,"A| Configuracion");
//	LCD_setCursor(&handlerLCD,1,0);
//	LCD_sendSTR(&handlerLCD,"B| Parametros");
//	LCD_setCursor(&handlerLCD,2,0);
//	LCD_sendSTR(&handlerLCD,"C| Activacion");
//	LCD_setCursor(&handlerLCD,3,0);
//	LCD_sendSTR(&handlerLCD,"D| Fecha y hora");
//
////	stop = 0; // Para que no se devuelva
//	inicial = 0;
//
//
//
//                // Esta ya solo deja al menu
//			    if((Fila4) && opcion4 == 1 ){
//
//			    	/* SE PRESIONO LA LETRA A */
//
//				GPIO_WritePin(&handlerF1, SET);
//				GPIO_WritePin(&handlerF2, RESET);
//				GPIO_WritePin(&handlerF3, RESET);
//				GPIO_WritePin(&handlerF4, RESET);
//
//				 if((GPIO_ReadPin(&handlerF1) == 1) && GPIO_ReadPin(&handlerC4) == 1){
//
////				menu_configuracion();
//
//				opcion41 = 1;
//
//				Fila4 = 0;
//
//
//
//				 }
//
//				 /* SE PRESIONO LA LETRA B */
//				GPIO_WritePin(&handlerF1, RESET);
//				GPIO_WritePin(&handlerF2, SET);
//				GPIO_WritePin(&handlerF3, RESET);
//				GPIO_WritePin(&handlerF4, RESET);
//
//				// Me manda  a parametros
//				 if((GPIO_ReadPin(&handlerF2) == 1) && GPIO_ReadPin(&handlerC4) == 1){
//
//				menu_parametros();
//
//				Fila4 = 0;
//
//				 }
//
//				 /* SE PRESIONO LA LETRA C */
//
//				// Opcion C
//
//
//				GPIO_WritePin(&handlerF1, RESET);
//				GPIO_WritePin(&handlerF2, RESET);
//				GPIO_WritePin(&handlerF3, SET);
//				GPIO_WritePin(&handlerF4, RESET);
//
//				 if((GPIO_ReadPin(&handlerF3) == 1) && GPIO_ReadPin(&handlerC4) == 1){
//
//			   // Me manda a menu de activacion
//
//				// Se activa otra vez el menu
//
//
//				menu_activacion(); // Nos dirigimos a activación
//
//				activacion = 1;
//
//				Fila4 = 0;
//								}
//
//
//				 /* SE PRESIONO LA LETRA D */
//
//				GPIO_WritePin(&handlerF1, RESET);
//				GPIO_WritePin(&handlerF2, RESET);
//				GPIO_WritePin(&handlerF3, RESET);
//				GPIO_WritePin(&handlerF4, SET);
//
//				 if((GPIO_ReadPin(&handlerF4) == 1) && GPIO_ReadPin(&handlerC4) == 1){
//
//				// Me manda a configuración de fecha y hora
//
//				fechahora();
//
//				Fila4 = 0;
//
//			    }
//
//				 }
//
//                }
//
//
//
//void menu_activacion(void){
//
//
//	/* Se despliega un nuevo menu dentro de este que se tiene que cambiar */
//
//	LCD_ClearScreen(&handlerLCD);
//	LCD_setCursor(&handlerLCD,0,0);
//	LCD_sendSTR(&handlerLCD,"A| Activar alarmas");
//	LCD_setCursor(&handlerLCD,1,0);
//	LCD_sendSTR(&handlerLCD,"B| Activar sensor");
//	LCD_setCursor(&handlerLCD,2,0);
//	LCD_sendSTR(&handlerLCD,"C| Desactivar");
//	LCD_setCursor(&handlerLCD,3,0);
//	LCD_sendSTR(&handlerLCD,"D| Devolverse");
//
////	activacion = 1;
//
//	 if((Fila4) && (activacion == 1)){
//
//		/* Fila 1 - Columna 4*/
//
//		 // Configuracion para el despliegue del menu de activacion
//
//		 /* SE PRESIONO LA LETRA A */
//
//		GPIO_WritePin(&handlerF1, SET);
//		GPIO_WritePin(&handlerF2, RESET);
//		GPIO_WritePin(&handlerF3, RESET);
//		GPIO_WritePin(&handlerF4, RESET);
//
//		// Caso activar sensores ambos
//
//		if((GPIO_ReadPin(&handlerF1) == 1) && GPIO_ReadPin(&handlerC4) == 1){
//
//
//		LCD_ClearScreen(&handlerLCD);
//		LCD_setCursor(&handlerLCD,0,0);
//		LCD_sendSTR(&handlerLCD,"Se activo la alarma");
//		LCD_setCursor(&handlerLCD,1,0);
//		LCD_sendSTR(&handlerLCD,"Correctamente");
//		LCD_setCursor(&handlerLCD,2,0);
//		LCD_sendSTR(&handlerLCD,"Pulsa cualquier");
//		LCD_setCursor(&handlerLCD,3,0);
//		LCD_sendSTR(&handlerLCD,"letra para volver");
//
//		/* Bajar todas las banderas involucradas */
//        stop = 1;
//        activacion = 0;
//		movimiento_puerta = 1;
//
//		inicial = 1;
//
//		Fila4 = 0;
//
//		}
//
//		/* SE PRESIONO LA LETRA B */
//
//		GPIO_WritePin(&handlerF1, RESET);
//		GPIO_WritePin(&handlerF2, SET);
//		GPIO_WritePin(&handlerF3, RESET);
//		GPIO_WritePin(&handlerF4, RESET);
//
//		// Caso activar solo de movimiento
//
//		if((GPIO_ReadPin(&handlerF2) == 1) && GPIO_ReadPin(&handlerC4) == 1){
//
//
//		// Otra bandera sola para esa opción
//
//
//
//		Fila4 = 0;
//		}
//
//		/* SE PRESIONO LA LETRA C */
//
//
//		GPIO_WritePin(&handlerF1, RESET);
//		GPIO_WritePin(&handlerF2, RESET);
//		GPIO_WritePin(&handlerF3, SET);
//		GPIO_WritePin(&handlerF4, RESET);
//
//
//		// Caso de desactivar alarma ya configurada
//
//		if((GPIO_ReadPin(&handlerF3) == 1) && GPIO_ReadPin(&handlerC4) == 1){
//
//
//			// Parametros de clave largos :O
//
//
//
//			Fila4 = 0;
//		}
//
//		/* SE PRESIONO LA LETRA D */
//
//
//		GPIO_WritePin(&handlerF1, RESET);
//		GPIO_WritePin(&handlerF2, RESET);
//		GPIO_WritePin(&handlerF3, RESET);
//		GPIO_WritePin(&handlerF4, SET);
//
//		// Devolverse al menu
//
//		if((GPIO_ReadPin(&handlerF4) == 1) && GPIO_ReadPin(&handlerC4) == 1){
//
//
//			Fila4 = 0;
//		}
//
//		 }
//
//        }
//
//void menu_configuracion(void){
//
//
//	LCD_ClearScreen(&handlerLCD);
//	LCD_setCursor(&handlerLCD,0,0);
//	LCD_sendSTR(&handlerLCD,"A| Activar alarmas");
//	LCD_setCursor(&handlerLCD,1,0);
//	LCD_sendSTR(&handlerLCD,"B| Activar sensor");
//	LCD_setCursor(&handlerLCD,2,0);
//	LCD_sendSTR(&handlerLCD,"C| Desactivar");
//	LCD_setCursor(&handlerLCD,3,0);
//	LCD_sendSTR(&handlerLCD,"D| Devolverse");
//
//
//	/* SE PRESIONO LA LETRA C */
//
//	if((Fila4) && (opcion41 == 1)){
//			GPIO_WritePin(&handlerF1, SET);
//			GPIO_WritePin(&handlerF2, RESET);
//			GPIO_WritePin(&handlerF3, RESET);
//			GPIO_WritePin(&handlerF4, RESET);
//
//
//			// Caso de desactivar alarma ya configurada
//
//			if((GPIO_ReadPin(&handlerF1) == 1) && GPIO_ReadPin(&handlerC4) == 1){
//
//
//				// Parametros de clave largos :O
//
//				movimiento_puerta = 1;
//
//
//
//				Fila4 = 0;
//			}
//
//	}
//}
//
//void menu_parametros(void){
//
//	LCD_ClearScreen(&handlerLCD);
//	LCD_setCursor(&handlerLCD,0,1);
//	LCD_sendSTR(&handlerLCD,"Zona de parametros");
//	LCD_setCursor(&handlerLCD,1,3);
//	LCD_sendSTR(&handlerLCD,"en esta podras");
//	LCD_setCursor(&handlerLCD,2,1);
//	LCD_sendSTR(&handlerLCD,"cambiar el sonido del");
//	LCD_setCursor(&handlerLCD,3,6);
//	LCD_sendSTR(&handlerLCD,"buzzer. Return D");
//
//
//}
//
//void fechahora(void){
//
//	LCD_ClearScreen(&handlerLCD);
//	LCD_setCursor(&handlerLCD,0,1);
//	LCD_sendSTR(&handlerLCD,"Zona de cambio tanto");
//	LCD_setCursor(&handlerLCD,1,3);
//	LCD_sendSTR(&handlerLCD,"de la fecha como la");
//	LCD_setCursor(&handlerLCD,2,1);
//	LCD_sendSTR(&handlerLCD,"hora. Hora Push = A");
//	LCD_setCursor(&handlerLCD,3,6);
//	LCD_sendSTR(&handlerLCD,",Fecha:Push=B.Return D");
//
//
//}

//void KeyPad_Confi(uint8_t opciones){
//
//	switch(opciones){
//
//
//	case 11:{
//
//		GPIO_WritePin(&handlerF1, SET);
//		GPIO_WritePin(&handlerF2, RESET);
//		GPIO_WritePin(&handlerF3, RESET);
//		GPIO_WritePin(&handlerF4, RESET);
//
//		break;
//
//
//	}
//
//	case 12:{
//
//
//		GPIO_WritePin(&handlerF1, RESET);
//		GPIO_WritePin(&handlerF2, SET);
//		GPIO_WritePin(&handlerF3, RESET);
//		GPIO_WritePin(&handlerF4, RESET);
//
//		break;
//
//
//	}
//
//
//	case 13:{
//
//		GPIO_WritePin(&handlerF1, RESET);
//		GPIO_WritePin(&handlerF2, RESET);
//		GPIO_WritePin(&handlerF3, SET);
//		GPIO_WritePin(&handlerF4, RESET);
//
//		break;
//	}
//
//	case 14:{
//
//	GPIO_WritePin(&handlerF1, RESET);
//	GPIO_WritePin(&handlerF2, RESET);
//	GPIO_WritePin(&handlerF3, RESET);
//	GPIO_WritePin(&handlerF4, SET);
//
//	break;
//
//	}
//
//	default:{
//
//		GPIO_WritePin(&handlerF1, SET);
//		GPIO_WritePin(&handlerF2, SET);
//		GPIO_WritePin(&handlerF3, SET);
//		GPIO_WritePin(&handlerF4, SET);
//
//		break;
//	}
//
//	}
//}
