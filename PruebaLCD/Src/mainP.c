/*
 * mainP.c
 *
 *  Created on: 1/11/2022
 *      Author: German
 *
 *      Solución del Parcial Taller V
 */
#include <stm32f4xx.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "BasicTimer.h"
#include "I2CDriver.h"
#include "I2CLCD.h"
#include "PwmDriver.h"
#include "math.h"
#include "RTCxDriver.h"

/* Definición de variables para operación matematicas */

#define MAX_X    16000
#define MIN_Y   -16000
#define MIN_X   -16000
#define MIN_Z   -16000

int16_t radio = 0;
int16_t  angulo = 0;

int16_t AccelX = 0;
int16_t AccelY = 0;
int16_t AccelZ = 0;

/* Definición de variables */
GPIO_Handler_t handlerBlinkyPin     = {0};

GPIO_Handler_t handlerTx     		= {0};
GPIO_Handler_t handlerRx	 		= {0};

GPIO_Handler_t handlerI2cSDA 		= {0};
GPIO_Handler_t handlerI2cSCL		= {0};

I2C_Handler_t handlerAcelerometro   = {0};
I2C_HandlerLCD_t handlerLCD         = {0};

USART_Handler_t handlerUsart1    = {0};

BasicTimer_Handler_t handlerBlinkyTimer = {0};

/* Definición de variables del proyecto */
uint8_t handlerLed  = 0;
uint8_t rxData      =  0;

uint8_t i2cBuffer = 0;

/* Definición para RTC */

uint8_t  *ptrData;

RTC_handler_t handlerRTC = {0};

uint8_t  Seg ;
uint8_t  Min ;
uint8_t  Hor;
uint8_t  Sem ;
uint8_t  Day;
uint8_t  Mes;
uint8_t  Ano;

/* Para mensajes */
char DataRTC[64] = "Wipi";
char DataRTCDate[64] = "Wipi";
char bufferData[64] = "esto es una pequeña prueba";
char welcomer[64]   = "Bienvenido";

/* Variables del I2C LCD */

GPIO_Handler_t handlerLCDcSDA   = {0};
GPIO_Handler_t handlerLCDcSCL   = {0};

/* Variables del PWM */

PWM_Handler_t  handlerPWMB       = {0};
PWM_Handler_t  handlerPWMR       = {0};
PWM_Handler_t  handlerPWMG       = {0};

BasicTimer_Handler_t  handlerTimer = {0};
GPIO_Handler_t        handlerBlue = {0};
GPIO_Handler_t        handlerGreen = {0};
GPIO_Handler_t        handlerRed = {0};



/* Definiciones para funcionamiento del acelerometro */
#define ACCEL_ADDRESS 		0b1101001;
#define ACCEL_XOUT_H 		59
#define ACCEL_XOUT_L		60
#define ACCEL_YOUT_H		61
#define ACCEL_YOUT_L		62
#define ACCEL_ZOUT_H		63
#define ACCEL_ZOUT_L		64

#define PWR_MGMT_l			107
#define WHO_AM_I			175

/* Deficiniones para la LCD */

#define LCD_ADDRESS     0b0100111

//ddd
/* Definición de las cabezeras de las funciones */
void initSystem (void);


int main(void)
{

	LCD_Init(&handlerLCD); // Pasa todo el proceso interno de la LCD


    LCD_setCursor(&handlerLCD, 0, 0);

	LCD_sendSTR(&handlerLCD, "Bienvenido");

	/* Ciclo principal del programa */

	while(1){

		/* Se escribe el valor de la variable estadoBlinky en el handlerBlinkyPin */
		GPIO_WritePin(&handlerBlinkyPin,handlerLed );


		if(rxData != '\0'){
			writeChar(&handlerUsart1, rxData);

			if(rxData == 'd'){
				i2cBuffer = i2c_readSingleRegister(&handlerAcelerometro, WHO_AM_I);
				sprintf(bufferData, "dataRead = 0x%2x  \n", (unsigned int) i2cBuffer);
				writeMsg(&handlerUsart1, bufferData);
				rxData = '\0';

			}


		  }
	}
}

void initSystem (void){

	/* Handler para el PIN A5 = Led de estado */
	handlerBlinkyPin.pGPIOx									= GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber 			= PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType		    = GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;

	/* Se carga la configuración */
	GPIO_Config(&handlerBlinkyPin);

	/* Handler para el PIN B6 */
	handlerTx.pGPIOx                       			= GPIOB;
	handlerTx.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;				// Pin TX USART1 NO
	handlerTx.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerTx.GPIO_PinConfig.GPIO_PinOPType		    = GPIO_OTYPE_PUSHPULL;
	handlerTx.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerTx.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerTx.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;					// Función alternativa USART1

	/* Se carga la configuración */
	GPIO_Config(&handlerTx);

	/* Handler para el PIN B7 */
	handlerRx.pGPIOx                       			= GPIOB;
	handlerRx.GPIO_PinConfig.GPIO_PinNumber			= PIN_7;				// Pin RX USART1
	handlerRx.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerRx.GPIO_PinConfig.GPIO_PinOPType		    = GPIO_OTYPE_PUSHPULL;
	handlerRx.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerRx.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerRx.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;					// Función alternativa USART1

	/* Se carga la configuración */
	GPIO_Config(&handlerRx);

	/* Handler para el USART1 */
	handlerUsart1.ptrUSARTx								= USART1;
	handlerUsart1.USART_Config.USART_baudrate			= USART_BAUDRATE_19200;
	handlerUsart1.USART_Config.USART_datasize			= USART_DATASIZE_9BIT;
	handlerUsart1.USART_Config.USART_mode				= USART_MODE_RXTX;
	handlerUsart1.USART_Config.USART_parity				= USART_PARITY_EVEN;
	handlerUsart1.USART_Config.USART_stopbits			= USART_STOPBIT_1;
	handlerUsart1.USART_Config.USART_IntRx				= USART_RX_INTERRUPT_ENABLE;
	handlerUsart1.USART_Config.USART_IntTx              = USART_TX_INTERRUP_DISABLE;


	/* Se carga la configuración */
	USART_Config(&handlerUsart1);

	/* Handler para el Timer2 */
	handlerBlinkyTimer.ptrTIMx 								= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed				= BTIMER_SPEED_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period 				= 2500; 			// Periodo de 250ms

	/* Se carga la configuración */
		BasicTimer_Config(&handlerBlinkyTimer);
	/* Handler para el Timer3 */

	/* Aqui se encargara de lo de la LDC */

	//Definimos puertos

	handlerLCDcSCL.pGPIOx                               = GPIOB;
	handlerLCDcSCL.GPIO_PinConfig.GPIO_PinNumber        = PIN_10;
	handlerLCDcSCL.GPIO_PinConfig.GPIO_PinMode          = GPIO_MODE_ALTFN;
	handlerLCDcSCL.GPIO_PinConfig.GPIO_PinOPType        = GPIO_OTYPE_OPENDRAIN;
	handlerLCDcSCL.GPIO_PinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;
	handlerLCDcSCL.GPIO_PinConfig.GPIO_PinSpeed         = GPIO_OSPEED_FAST;
	handlerLCDcSCL.GPIO_PinConfig.GPIO_PinAltFunMode    = AF4;

	/* Se carga a la configuración */
	GPIO_Config(&handlerLCDcSCL);

	handlerLCDcSDA.pGPIOx                               = GPIOB;
	handlerLCDcSDA.GPIO_PinConfig.GPIO_PinNumber        = PIN_11;
	handlerLCDcSDA.GPIO_PinConfig.GPIO_PinMode          = GPIO_MODE_ALTFN;
	handlerLCDcSDA.GPIO_PinConfig.GPIO_PinOPType        = GPIO_OTYPE_OPENDRAIN;
	handlerLCDcSDA.GPIO_PinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_PULLUP;
	handlerLCDcSDA.GPIO_PinConfig.GPIO_PinSpeed         = GPIO_OSPEED_FAST;
	handlerLCDcSDA.GPIO_PinConfig.GPIO_PinAltFunMode    = AF4;

	/* Se carga la configuración */

	GPIO_Config(&handlerLCDcSDA);

	/* Se carga la configuración I2C */

	/* Handler para el acelerometro */
	handlerLCD.ptrI2Cx				= I2C2;
	handlerLCD.modeI2C				= I2C_MODE_FM;
	handlerLCD.slaveAddress		    = LCD_ADDRESS  ;

	/* Cargo la configuración de LCD */


	I2C_ConfigLCD(&handlerLCD);


	/* Handler para el I2CSCL */
	handlerI2cSCL.pGPIOx								= GPIOB;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinNumber			= PIN_8;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinOPType		    = GPIO_OTYPE_OPENDRAIN;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinAltFunMode		= AF4;

	/* Se carga la configuración */
	GPIO_Config(&handlerI2cSCL);

	/* Handler para el I2CSDA */
	handlerI2cSDA.pGPIOx								= GPIOB;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinNumber			= PIN_9;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_OPENDRAIN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinAltFunMode		= AF4;

	/* Se carga la configuración */
	GPIO_Config(&handlerI2cSDA);



}

void USART1Rx_CallBack(void){

rxData = getRxData();

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


}
