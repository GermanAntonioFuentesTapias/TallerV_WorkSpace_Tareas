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

/* Definición de variables */
GPIO_Handler_t handlerBlinkyPin     = {0};

GPIO_Handler_t handlerTx     		= {0};
GPIO_Handler_t handlerRx	 		= {0};

GPIO_Handler_t handlerI2cSDA 		= {0};
GPIO_Handler_t handlerI2cSCL		= {0};

I2C_Handler_t handlerAcelerometro   = {0};

USART_Handler_t handlerUsart2    = {0};

BasicTimer_Handler_t handlerBlinkyTimer = {0};

/* Definición de variables del proyecto */
uint8_t handlerLed  = 0;
uint8_t rxData       =  0;

uint8_t i2cBuffer = 0;

//char bufferData[64] =  {0};
//char greetingMsg[] = "Dale Medellin\n\r";

char bufferData[64] = "esto es una pequeña prueba";

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

/* Definición de las cabezeras de las funciones */
void initSystem (void);

int main(void)
{
   /* Llamamos a la función initSystem para que se ejecute */
	initSystem();


	/* Ciclo principal del programa */
	while(1){

		/* Se escribe el valor de la variable estadoBlinky en el handlerBlinkyPin */
		GPIO_WritePin(&handlerBlinkyPin,handlerLed );


		if(rxData != '\0'){
			writeChar(&handlerUsart2, rxData);

			if(rxData == 'd'){
				i2cBuffer = i2c_readSingleRegister(&handlerAcelerometro, WHO_AM_I);
				sprintf(bufferData, "dataRead = 0x%2x  \n", (unsigned int) i2cBuffer);
				writeMsg(&handlerUsart2, bufferData);
				rxData = '\0';

			}

			else if (rxData == 'p'){
				i2cBuffer = i2c_readSingleRegister(&handlerAcelerometro, PWR_MGMT_l);
				sprintf(bufferData, "dataRead = 0x%2x \n", (unsigned int) i2cBuffer);
				writeMsg(&handlerUsart2, bufferData);
				rxData = '\0';
			}

			else if(rxData == 'r'){

				i2c_writeSingleRegister(&handlerAcelerometro, PWR_MGMT_l, 0x00);
				rxData = '\0';

			}

			else if(rxData == 'x'){
				uint8_t AccelX_low = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_XOUT_L);
				uint8_t AccelX_high = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_XOUT_H);
				int16_t AccelX = AccelX_high << 8 | AccelX_low;
				sprintf(bufferData, "AccelX = %d \n", (int) AccelX);
				writeMsg(&handlerUsart2, bufferData);
				rxData = '\0';
			}

			else if (rxData == 'y'){
				uint8_t AccelY_low = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_YOUT_L);
				uint8_t AccelY_high = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_YOUT_H);
				int16_t AccelY = AccelY_high << 8 | AccelY_low;
				sprintf(bufferData, "AccelY = %d \n", (int) AccelY);
				writeMsg(&handlerUsart2, bufferData);
				rxData = '\0';

			}

			else if (rxData == 'z'){
				uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_ZOUT_L);
				uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_ZOUT_H);
				int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
				sprintf(bufferData, "AccelZ = %d \n", (int) AccelZ);
				writeMsg(&handlerUsart2, bufferData);
				rxData = '\0';
			}

			else{
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

	/* Handler para el PIN A2 */
	handlerTx.pGPIOx                       			= GPIOA;
	handlerTx.GPIO_PinConfig.GPIO_PinNumber			= PIN_2;				// Pin TX USART2
	handlerTx.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerTx.GPIO_PinConfig.GPIO_PinOPType		    = GPIO_OTYPE_PUSHPULL;
	handlerTx.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerTx.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerTx.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;					// Función alternativa USART1

	/* Se carga la configuración */
	GPIO_Config(&handlerTx);

	/* Handler para el PIN A3 */
	handlerRx.pGPIOx                       			= GPIOA;
	handlerRx.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;				// Pin TX USART2
	handlerRx.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerRx.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerRx.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerRx.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerRx.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;					// Función alternativa USART1

	/* Se carga la configuración */
	GPIO_Config(&handlerRx);

	/* Handler para el USART1 */
	handlerUsart2.ptrUSARTx								= USART2;
	handlerUsart2.USART_Config.USART_baudrate			= USART_BAUDRATE_19200;
	handlerUsart2.USART_Config.USART_datasize			= USART_DATASIZE_9BIT;
	handlerUsart2.USART_Config.USART_mode				= USART_MODE_RXTX;
	handlerUsart2.USART_Config.USART_parity				= USART_PARITY_NONE;
	handlerUsart2.USART_Config.USART_stopbits			= USART_STOPBIT_1;
	handlerUsart2.USART_Config.USART_IntRx				= USART_RX_INTERRUPT_ENABLE;



	/* Se carga la configuración */
	USART_Config(&handlerUsart2);

	/* Handler para el Timer2 */
	handlerBlinkyTimer.ptrTIMx 								= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed				= BTIMER_SPEED_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period 				= 2500; 			// Periodo de 250ms

	/* Se carga la configuración */
	BasicTimer_Config(&handlerBlinkyTimer);

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

	/* Handler para el acelerometro */
	handlerAcelerometro.ptrI2Cx				= I2C1;
	handlerAcelerometro.modeI2C				= I2C_MODE_FM;
	handlerAcelerometro.slaveAddress		= ACCEL_ADDRESS;

	i2c_config(&handlerAcelerometro);

}

void USART2Rx_CallBack(void){

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



