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
LCD_Handler_t handlerLCD         = {0};


USART_Handler_t handlerUsart1    = {0};

BasicTimer_Handler_t handlerBlinkyTimer = {0};

/* Definición de variables del proyecto */
uint8_t handlerLed  = 0;
uint8_t rxData      =  0;
uint8_t rxDataCMD   = 0;
uint8_t Bandera     = 0;
uint8_t BanderaRGB  = 0;
uint8_t BanderaRGB2 = 0;

BasicTimer_Handler_t handlerCommands = {0};
BasicTimer_Handler_t handlerRGB      = {0};

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
char bufferDataY[64] = " Valor Y";
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

#define LCD_ADDRESS     0b100111
//#define LCD_ADDRESS     0b0100111
/* Funciones para comandos */


uint8_t counterReception = 0;
char bufferReception[64] = {0};
char cmd[64];
char userMsg[64];
bool stringComplete = false;
bool makeUpdateLCD  = false;
unsigned int firstParameter;
unsigned int secondParameter;


/****************************************************************/




/* Definición de las cabezeras de las funciones */
void initSystem (void);
void parseCommands (char *ptrBufferReception);

void Welcome(void);

int main(void)
{
   /* Llamamos a la función initSystem para que se ejecute */

	initSystem();
	startPwmSignal(&handlerPWMR);
	startPwmSignal(&handlerPWMB);
	startPwmSignal(&handlerPWMG);


	LCD_Clear(&handlerLCD, LCD_ADDRESS);
	LCD_Init(&handlerLCD, LCD_ADDRESS);

	Welcome();
//	LCD_Init(&handlerLCD); // Pasa todo el proceso interno de la LCD
//
//
//    LCD_setCursor(&handlerLCD, 0, 0);
//
//	LCD_sendSTR(&handlerLCD, "Bienvenido");

	/* Ciclo principal del programa */

	while(1){

		/* Se escribe el valor de la variable estadoBlinky en el handlerBlinkyPin */
		GPIO_WritePin(&handlerBlinkyPin,handlerLed );

		startCounterTimer(&handlerBlinkyTimer);


		ptrData =leer_datos();

		   Seg = ptrData[0];
		   Min = ptrData[1];
		   Hor = ptrData[2];
		   Sem = ptrData[3];
		   Day = ptrData[4];
		   Mes = ptrData[5];
		   Ano = ptrData[6];

       if (Bandera){
    	   i2cBuffer = i2c_readSingleRegister(&handlerAcelerometro, WHO_AM_I);
    	   		i2cBuffer = i2c_readSingleRegister(&handlerAcelerometro, PWR_MGMT_l);
    	   		i2c_writeSingleRegister(&handlerAcelerometro, PWR_MGMT_l, 0x00);
    	   		uint8_t AccelX_low = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_XOUT_L);
    	   		uint8_t AccelX_high = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_XOUT_H);
    	   		AccelX = AccelX_high << 8 | AccelX_low;

    	   		uint8_t AccelY_low = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_YOUT_L);
    	   		uint8_t AccelY_high = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_YOUT_H);
    	   		AccelY = AccelY_high << 8 | AccelY_low;
    	   		sprintf(bufferDataY, "AccelY = %d \n \r", (int) AccelY);
    	   		sprintf(bufferData, "AccelX = %d \n", (int) AccelX);
    	   		writeMsg(&handlerUsart1, bufferData);
    	   		writeMsg(&handlerUsart1, bufferDataY);
    	   		rxDataCMD = '\0';

    	   		Bandera = 0;

       }

//       if (BanderaRGB2) {
//
//
//    		sprintf(DataRTC, " La hora es = %uh: %um: %us: \n", (unsigned int) Hor, (unsigned int) Min, (unsigned int) Seg );
//    		writeMsg(&handlerUsart1, DataRTC);
//    		rxDataCMD = '\0';
//
//    		BanderaRGB2 = 0;
//       }

       if (BanderaRGB){

    	   i2cBuffer = i2c_readSingleRegister(&handlerAcelerometro, WHO_AM_I);
			i2cBuffer = i2c_readSingleRegister(&handlerAcelerometro, PWR_MGMT_l);
			i2c_writeSingleRegister(&handlerAcelerometro, PWR_MGMT_l, 0x00);
			uint8_t AccelX_low = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_XOUT_L);
			uint8_t AccelX_high = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_XOUT_H);
			AccelX = AccelX_high << 8 | AccelX_low;
			uint8_t AccelY_low = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_YOUT_L);
			uint8_t AccelY_high = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_YOUT_H);
			AccelY = AccelY_high << 8 | AccelY_low;

			 if(AccelY > 0 && AccelX > 0){

			  angulo = atan(AccelY/AccelX)*(180/(acos(-1)));

			  if( angulo < 30){
			  updateDuttyCycle(&handlerPWMB, 100);
			  updateDuttyCycle(&handlerPWMR, 50);
			  updateDuttyCycle(&handlerPWMG, 0);

			  } else if (angulo >= 30 && angulo < 60){
				  updateDuttyCycle(&handlerPWMB, 50);
				  updateDuttyCycle(&handlerPWMR, 50);
				  updateDuttyCycle(&handlerPWMG, 0);


			  } else if ( angulo >= 60 && angulo < 90) {
				  updateDuttyCycle(&handlerPWMB, 100);
				  updateDuttyCycle(&handlerPWMR, 100);
				  updateDuttyCycle(&handlerPWMG, 0 );


			  } else{
				  __NOP();
			  }

			  }else if( AccelX < 0 && AccelY > 0){
				  angulo = atan(AccelY/(-AccelX))*(180/(acos(-1)));
				  angulo = 180 - angulo;

				  if (angulo >= 90 && angulo < 120){

					  updateDuttyCycle(&handlerPWMB, 100);
					  updateDuttyCycle(&handlerPWMR,100);
					  updateDuttyCycle(&handlerPWMG, 0);

					  } else if (angulo >= 120 && angulo < 150){
						  updateDuttyCycle(&handlerPWMB, 0);
						  updateDuttyCycle(&handlerPWMR, 0);
						  updateDuttyCycle(&handlerPWMG, 100);


					  } else if ( angulo >= 150 && angulo < 180) {
						  updateDuttyCycle(&handlerPWMB, 100);
						  updateDuttyCycle(&handlerPWMR, 0);
						  updateDuttyCycle(&handlerPWMG, 100 );


									  } else{
										  __NOP();
									  }

			  } else if( AccelX <0 && AccelY < 0){
				  angulo = atan(-AccelY/(-AccelX))*(180/(acos(-1)));
				  angulo = 180 + angulo;

				  if(angulo >= 180 && angulo <210){

			  updateDuttyCycle(&handlerPWMB, 100);
				  updateDuttyCycle(&handlerPWMR, 0);
				  updateDuttyCycle(&handlerPWMG, 100);

				  } else if (angulo >= 210 && angulo < 240){
					  updateDuttyCycle(&handlerPWMB, 100);
					  updateDuttyCycle(&handlerPWMR, 0);
					  updateDuttyCycle(&handlerPWMG, 100);


				  } else if ( angulo >= 240 && angulo < 270) {
					  updateDuttyCycle(&handlerPWMB, 100);
					  updateDuttyCycle(&handlerPWMR, 300);
					  updateDuttyCycle(&handlerPWMG, 0 );


				  } else{
									  __NOP();
								  }



			  } else if ( AccelX > 0 && AccelY <= 0){
				  angulo = atan(-AccelY/(AccelX))*(180/(acos(-1)));
				  angulo = 360 - angulo;

				  if( angulo >= 270 && angulo < 300){

				  updateDuttyCycle(&handlerPWMB, 10000);
				  updateDuttyCycle(&handlerPWMR, 0);
				  updateDuttyCycle(&handlerPWMG, 0);

				  } else if (angulo >= 300 && angulo < 330){
					  updateDuttyCycle(&handlerPWMB, 10000);
					  updateDuttyCycle(&handlerPWMR, 10000);
					  updateDuttyCycle(&handlerPWMG, 0);


				  } else if ( angulo >= 330 && angulo <= 360) {
					  updateDuttyCycle(&handlerPWMB, 0);
					  updateDuttyCycle(&handlerPWMR, 10000);
					  updateDuttyCycle(&handlerPWMG, 0 );

				  }

			  } else if (AccelX == 0 || AccelY == 0){
				  angulo = 0;
				  updateDuttyCycle(&handlerPWMR, 10000);
				  updateDuttyCycle(&handlerPWMB, 10000);
				  updateDuttyCycle(&handlerPWMG, 10000);
			  }

    	   			sprintf(welcomer, "Angulo = %d \n", (int) angulo);
    	   			writeMsg(&handlerUsart1, welcomer);
    	   			rxDataCMD = '\0';

    	   BanderaRGB = 0;
       }
//		if(rxData != '\0'){
//		writeChar(&handlerUsart1, rxData);
//		bufferReception[counterReception] = rxData;
//
//			if(rxData == 'd'){
//				i2cBuffer = i2c_readSingleRegister(&handlerAcelerometro, WHO_AM_I);
//				sprintf(bufferData, "dataRead = 0x%2x  \n", (unsigned int) i2cBuffer);
//				writeMsg(&handlerUsart1, bufferData);
//				rxData = '\0';
//
//			}
//			else if (rxData == 'm'){
//
//				  ptrData =leer_datos();
//
//				   Seg = ptrData[0];
//				   Min = ptrData[1];
//				   Hor = ptrData[2];
//				   Sem = ptrData[3];
//				   Day = ptrData[4];
//				   Mes = ptrData[5];
//				   Ano = ptrData[6];
//
//				sprintf(DataRTC, " La hora es = %uh: %um: %us: \n", (unsigned int) Hor, (unsigned int) Min, (unsigned int) Seg );
//				writeMsg(&handlerUsart1, DataRTC);
//				rxData = '\0';
//			}
//
//			else if (rxData == 'f'){
//				sprintf(DataRTCDate, " La fecha es = %u/ %u/ %u/  \n", (unsigned int) Day , (unsigned int) Mes, (unsigned int) Ano);
//				writeMsg(&handlerUsart1, DataRTCDate);
//				rxData = '\0';
//
//			}
//
//			else if (rxData == 'p'){
//				i2cBuffer = i2c_readSingleRegister(&handlerAcelerometro, PWR_MGMT_l);
//				sprintf(bufferData, "dataRead = 0x%2x \n", (unsigned int) i2cBuffer);
//				writeMsg(&handlerUsart1, bufferData);
//				rxData = '\0';
//			}
//
//			else if(rxData == 'r'){
//
//				i2c_writeSingleRegister(&handlerAcelerometro, PWR_MGMT_l, 0x00);
//				rxData = '\0';
//
//			}
//
//			else if(rxData == 'x'){
//
//				uint8_t AccelX_low = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_XOUT_L);
//				uint8_t AccelX_high = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_XOUT_H);
//				AccelX = AccelX_high << 8 | AccelX_low;
//				sprintf(bufferData, "AccelX = %d \n", (int) AccelX);
//				writeMsg(&handlerUsart1, bufferData);
//				rxData = '\0';
//			}
//
//			else if (rxData == 'y'){
//				uint8_t AccelY_low = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_YOUT_L);
//				uint8_t AccelY_high = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_YOUT_H);
//				AccelY = AccelY_high << 8 | AccelY_low;
//				sprintf(bufferData, "AccelY = %d \n", (int) AccelY);
//				writeMsg(&handlerUsart1, bufferData);
//				rxData = '\0';
//
//			}
//
//			else if (rxData == 'z'){
//				uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_ZOUT_L);
//				uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_ZOUT_H);
//			    AccelZ = AccelZ_high << 8 | AccelZ_low;
//				sprintf(bufferData, "AccelZ = %d \n", (int) AccelZ);
//				writeMsg(&handlerUsart1, bufferData);
//				rxData = '\0';
//			}
//			else{
//				rxData = '\0';
//			}
//
//               if(AccelX != 0){
//                  if(AccelY > 0 && AccelX > 0){
//				  angulo = atan(AccelY/AccelX)*(180/(acos(-1)));
//
//				  if( angulo < 30){
//				  updateDuttyCycle(&handlerPWMB, 10000);
//				  updateDuttyCycle(&handlerPWMR, 5000);
//				  updateDuttyCycle(&handlerPWMG, 0);
//
//				  } else if (angulo >= 30 && angulo < 60){
//					  updateDuttyCycle(&handlerPWMB, 5000);
//					  updateDuttyCycle(&handlerPWMR, 5000);
//					  updateDuttyCycle(&handlerPWMG, 0);
//
//
//				  } else if ( angulo >= 60 && angulo < 90) {
//					  updateDuttyCycle(&handlerPWMB, 3000);
//					  updateDuttyCycle(&handlerPWMR, 3000);
//					  updateDuttyCycle(&handlerPWMG, 0 );
//
//
//				  } else{
//					  __NOP();
//				  }
//
//                  }else if( AccelX < 0 && AccelY > 0){
//                	  angulo = atan(AccelY/(-AccelX))*(180/(acos(-1)));
//                	  angulo = 180 - angulo;
//
//                	  if (angulo >= 90 && angulo < 120){
//
//                		  updateDuttyCycle(&handlerPWMB, 5000);
//						  updateDuttyCycle(&handlerPWMR, 5000);
//						  updateDuttyCycle(&handlerPWMG, 0);
//
//						  } else if (angulo >= 120 && angulo < 150){
//							  updateDuttyCycle(&handlerPWMB, 0);
//							  updateDuttyCycle(&handlerPWMR, 0);
//							  updateDuttyCycle(&handlerPWMG, 10000);
//
//
//						  } else if ( angulo >= 150 && angulo < 180) {
//							  updateDuttyCycle(&handlerPWMB, 5000);
//							  updateDuttyCycle(&handlerPWMR, 0);
//							  updateDuttyCycle(&handlerPWMG, 10000 );
//
//
//                		  				  } else{
//                		  					  __NOP();
//                		  				  }
//
//                  } else if( AccelX <0 && AccelY < 0){
//                	  angulo = atan(-AccelY/(-AccelX))*(180/(acos(-1)));
//                	  angulo = 180 + angulo;
//
//                	  if(angulo >= 180 && angulo <210){
//
//				  updateDuttyCycle(&handlerPWMB, 10000);
//					  updateDuttyCycle(&handlerPWMR, 0);
//					  updateDuttyCycle(&handlerPWMG, 5000);
//
//					  } else if (angulo >= 210 && angulo < 240){
//						  updateDuttyCycle(&handlerPWMB, 10000);
//						  updateDuttyCycle(&handlerPWMR, 0);
//						  updateDuttyCycle(&handlerPWMG, 1000);
//
//
//					  } else if ( angulo >= 240 && angulo < 270) {
//						  updateDuttyCycle(&handlerPWMB, 10000);
//						  updateDuttyCycle(&handlerPWMR, 3000);
//						  updateDuttyCycle(&handlerPWMG, 0 );
//
//
//					  } else{
//                	  					  __NOP();
//                	  				  }
//
//
//
//                  } else if ( AccelX > 0 && AccelY <= 0){
//                	  angulo = atan(-AccelY/(AccelX))*(180/(acos(-1)));
//                	  angulo = 360 - angulo;
//
//                	  if( angulo >= 270 && angulo < 300){
//
//					  updateDuttyCycle(&handlerPWMB, 10000);
//					  updateDuttyCycle(&handlerPWMR, 0);
//					  updateDuttyCycle(&handlerPWMG, 0);
//
//					  } else if (angulo >= 300 && angulo < 330){
//						  updateDuttyCycle(&handlerPWMB, 10000);
//						  updateDuttyCycle(&handlerPWMR, 10000);
//						  updateDuttyCycle(&handlerPWMG, 0);
//
//
//					  } else if ( angulo >= 330 && angulo <= 360) {
//						  updateDuttyCycle(&handlerPWMB, 0);
//						  updateDuttyCycle(&handlerPWMR, 10000);
//						  updateDuttyCycle(&handlerPWMG, 0 );
//
//					  }
//
//                  } else if (AccelX == 0 || AccelY == 0){
//                	  angulo = 0;
//                	  updateDuttyCycle(&handlerPWMR, 10000);
//					  updateDuttyCycle(&handlerPWMB, 10000);
//					  updateDuttyCycle(&handlerPWMG, 10000);
//                  }
//
//			sprintf(welcomer, "Angulo = %d \n", (int) angulo);
//			writeMsg(&handlerUsart1, welcomer);
//			rxData = '\0';
//               }
//
//		  }

		if(rxDataCMD != '\0'){


		bufferReception[counterReception] = rxDataCMD;
				counterReception++;
				     if (rxDataCMD == '@'){
				    	 stringComplete = true;

				    	 // Agrego linea

				    	 bufferReception[counterReception] = '\0';

				    	 counterReception = 0;
				     }

				     rxDataCMD = '\0';

	}

		// Hacemos un análisis de la cadena de datos obtenida

		if ( stringComplete){

			parseCommands(bufferReception);

			stringComplete = false;
		}

//		if(makeUpdateLCD){
//
//			updateLCD();
//			makeUpdateLCD = false;
//		}
//
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



		handlerCommands.ptrTIMx                                = TIM4;
		handlerCommands.TIMx_Config.TIMx_mode                  = BTIMER_MODE_UP;
		handlerCommands.TIMx_Config.TIMx_speed                 = BTIMER_SPEED_1ms;
		handlerCommands.TIMx_Config.TIMx_period                = 1000;


	    BasicTimer_Config(&handlerCommands);

	    handlerRGB.ptrTIMx                                    = TIM5;
	    handlerRGB.TIMx_Config.TIMx_mode                      = BTIMER_MODE_UP;
	    handlerRGB.TIMx_Config.TIMx_speed                     = BTIMER_SPEED_100us;
	    handlerRGB.TIMx_Config.TIMx_period                    = 10000;

	    BasicTimer_Config(&handlerRGB);


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
	handlerLCD.ptrLCDx              = I2C2;
	handlerLCD.modeI2CLCD           = I2C_MODE_FM;
	handlerLCD.slaveAddressLCD      = LCD_ADDRESS;

	/* Cargo la configuración de LCD */


	LCD_Config(&handlerLCD);


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

   /* Configuraciones PWM GENERAL */

	// Color Azul

	handlerBlue.pGPIOx                                = GPIOC;
	handlerBlue.GPIO_PinConfig.GPIO_PinNumber         = PIN_7;
	handlerBlue.GPIO_PinConfig.GPIO_PinAltFunMode     = AF2;
	handlerBlue.GPIO_PinConfig.GPIO_PinMode           = GPIO_MODE_ALTFN;
	handlerBlue.GPIO_PinConfig.GPIO_PinOPType         = GPIO_OTYPE_PUSHPULL;
	handlerBlue.GPIO_PinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_NOTHING;
	handlerBlue.GPIO_PinConfig.GPIO_PinSpeed          = GPIO_OSPEED_FAST;

	/* Se carga la configuración */

	GPIO_Config(&handlerBlue);

	// Color Rojo

	handlerRed.pGPIOx                                = GPIOC;
	handlerRed.GPIO_PinConfig.GPIO_PinNumber         = PIN_6;
	handlerRed.GPIO_PinConfig.GPIO_PinAltFunMode     = AF2;
	handlerRed.GPIO_PinConfig.GPIO_PinMode           = GPIO_MODE_ALTFN;
	handlerRed.GPIO_PinConfig.GPIO_PinOPType         = GPIO_OTYPE_PUSHPULL;
	handlerRed.GPIO_PinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_NOTHING;
	handlerRed.GPIO_PinConfig.GPIO_PinSpeed          = GPIO_OSPEED_FAST;

	/* Se carga la configuración */

	GPIO_Config(&handlerRed);

		// Color Verde

	handlerGreen.pGPIOx                                = GPIOC;
	handlerGreen.GPIO_PinConfig.GPIO_PinNumber         = PIN_8;
	handlerGreen.GPIO_PinConfig.GPIO_PinAltFunMode     = AF2;
	handlerGreen.GPIO_PinConfig.GPIO_PinMode           = GPIO_MODE_ALTFN;
	handlerGreen.GPIO_PinConfig.GPIO_PinOPType         = GPIO_OTYPE_PUSHPULL;
	handlerGreen.GPIO_PinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_NOTHING;
	handlerGreen.GPIO_PinConfig.GPIO_PinSpeed          = GPIO_OSPEED_FAST;

	/* Se carga la configuración */

	GPIO_Config(&handlerGreen);


	/* Funciones para config PWM */


	handlerPWMB.ptrTIMx                                   = TIM3;
	handlerPWMB.config.channel                            = PWM_CHANNEL_2;
	handlerPWMB.config.duttyCicle                         = 0;
	handlerPWMB.config.periodo                            = 120;
	handlerPWMB.config.prescaler                          = BTIMER_SPEED_100us;

    /* Se carga la configuración */
	pwm_Config(&handlerPWMB);

     handlerPWMG.ptrTIMx                                   = TIM3;
     handlerPWMG.config.channel                            = PWM_CHANNEL_3;
	 handlerPWMG.config.duttyCicle                         = 1000;
	 handlerPWMG.config.periodo                            = 120;
	 handlerPWMG.config.prescaler                          = BTIMER_SPEED_100us;

	 pwm_Config(&handlerPWMG);

	 handlerPWMR.ptrTIMx                                   = TIM3;
	 handlerPWMR.config.channel                            = PWM_CHANNEL_1;
	 handlerPWMR.config.duttyCicle                         = 0;
	 handlerPWMR.config.periodo                            = 120;
	 handlerPWMR.config.prescaler                          = BTIMER_SPEED_100us;

	 pwm_Config(&handlerPWMR);

	 /* Despliegue de handler PWM */

	 enableOutput(&handlerPWMR);
	 enableOutput(&handlerPWMG);
	 enableOutput(&handlerPWMB);


	 /* Configuración RTC */

	 handlerRTC.DateTypeDef.RTC_Date     = 05;
	 handlerRTC.DateTypeDef.RTC_Month    = 11;
	 handlerRTC.DateTypeDef.RTC_WeekDay  = Satur;
	 handlerRTC.DateTypeDef.RTC_Year     = 22;
	 handlerRTC.TimeTypeDef.RTC_H12      = 12;
	 handlerRTC.TimeTypeDef.RTC_Hours    = 23;
	 handlerRTC.TimeTypeDef.RTC_Minutes  = 55;
	 handlerRTC.TimeTypeDef.RTC_Seconds  = 44;

	 RTC_Config(&handlerRTC);





	/* Configuración  de RGB */


}

void USART1Rx_CallBack(void){

rxData = getRxData();

rxDataCMD = getRxDataCMD();
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


void BasicTimer4_CallBack(void){

	Bandera = 1;

	BanderaRGB = 1;





}

void BasicTimer5_CallBack(void){

	BanderaRGB2 = 1;

}




void parseCommands(char *ptrBufferReception){

	sscanf(ptrBufferReception, " %s %u %u %s" , cmd , &firstParameter , &secondParameter ,userMsg);

	if(strcmp(cmd , "help") == 0){

		writeMsg(&handlerUsart1, "Help Menu CMDs: \n");
		writeMsg(&handlerUsart1, "1) Millos = Colores del grande ");
		writeMsg(&handlerUsart1, "2) Dutty = #A #B --- dummy cmd, #A and B son uint32_t \n");
		writeMsg(&handlerUsart1, "3) Hora = en Hora - Minuto - Segundos \n");
		writeMsg(&handlerUsart1, "4) Fecha=  en Dia / Mes / Año \n" );
		writeMsg(&handlerUsart1, "5) Accel = Se inicializa el acelerometro \n ");
		writeMsg(&handlerUsart1, "6) StopA = Se detiene el acelerometro \n");
		writeMsg(&handlerUsart1, "7) RBG = Se activa RGB  y cambia con acelerometro de manera tranquila \n)");
		writeMsg(&handlerUsart1, "8) StopR= Se detiene RGB  \n");
		writeMsg(&handlerUsart1, "9) GO = Se activa RGB con colores programdos \n");
		writeMsg(&handlerUsart1, "10) Se detiene RGB Variante \n ");



	} else if (strcmp(cmd , "Millos" ) == 0) {

//		startCounterTimer(&handlerRGB);
		updateDuttyCycle(&handlerPWMB, 10000);
//		pwm_Config(&handlerPWMB);



	}

	else if(strcmp(cmd ,"Hora") == 0) {


	sprintf(DataRTC, " La hora es = %uh: %um: %us: \n", (unsigned int) Hor, (unsigned int) Min, (unsigned int) Seg );
	writeMsg(&handlerUsart1, DataRTC);
	rxDataCMD = '\0';




	}

	else if(strcmp(cmd , "Fecha") == 0) {

	sprintf(DataRTCDate, " La fecha es = %u/ %u/ %u \n", (unsigned int) Day , (unsigned int) Mes, (unsigned int) Ano);
	writeMsg(&handlerUsart1, DataRTCDate);
	rxDataCMD = '\0';


	}

	else if(strcmp(cmd, "Accel") == 0) {


        startCounterTimer(&handlerCommands);


	}

	else if(strcmp(cmd , "StopA") == 0) {

       StopCounterTimer(&handlerCommands);


	}

	else if(strcmp(cmd ,"RBG") == 0){

		startCounterTimer(&handlerRGB);

	}

	else if(strcmp(cmd, "StopR") == 0){

		StopCounterTimer(&handlerRGB);
	}


	else{

		writeMsg(&handlerUsart1, " Wrong CMD");
	}


}

//		if(rxData != '\0'){
//		writeChar(&handlerUsart1, rxData);
//		bufferReception[counterReception] = rxData;
//
//			if(rxData == 'd'){
//				i2cBuffer = i2c_readSingleRegister(&handlerAcelerometro, WHO_AM_I);
//				sprintf(bufferData, "dataRead = 0x%2x  \n", (unsigned int) i2cBuffer);
//				writeMsg(&handlerUsart1, bufferData);
//				rxData = '\0';
//
//			}
//			else if (rxData == 'm'){
//
//				  ptrData =leer_datos();
//
//				   Seg = ptrData[0];
//				   Min = ptrData[1];
//				   Hor = ptrData[2];
//				   Sem = ptrData[3];
//				   Day = ptrData[4];
//				   Mes = ptrData[5];
//				   Ano = ptrData[6];
//
//				sprintf(DataRTC, " La hora es = %uh: %um: %us: \n", (unsigned int) Hor, (unsigned int) Min, (unsigned int) Seg );
//				writeMsg(&handlerUsart1, DataRTC);
//				rxData = '\0';
//			}
//
//			else if (rxData == 'f'){
//				sprintf(DataRTCDate, " La fecha es = %u/ %u/ %u/  \n", (unsigned int) Day , (unsigned int) Mes, (unsigned int) Ano);
//				writeMsg(&handlerUsart1, DataRTCDate);
//				rxData = '\0';
//
//			}
//
//			else if (rxData == 'p'){
//				i2cBuffer = i2c_readSingleRegister(&handlerAcelerometro, PWR_MGMT_l);
//				sprintf(bufferData, "dataRead = 0x%2x \n", (unsigned int) i2cBuffer);
//				writeMsg(&handlerUsart1, bufferData);
//				rxData = '\0';
//			}
//
//			else if(rxData == 'r'){
//
//				i2c_writeSingleRegister(&handlerAcelerometro, PWR_MGMT_l, 0x00);
//				rxData = '\0';
//
//			}
//
//			else if(rxData == 'x'){
//
//				uint8_t AccelX_low = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_XOUT_L);
//				uint8_t AccelX_high = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_XOUT_H);
//				AccelX = AccelX_high << 8 | AccelX_low;
//				sprintf(bufferData, "AccelX = %d \n", (int) AccelX);
//				writeMsg(&handlerUsart1, bufferData);
//				rxData = '\0';
//			}
//
//			else if (rxData == 'y'){
//				uint8_t AccelY_low = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_YOUT_L);
//				uint8_t AccelY_high = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_YOUT_H);
//				AccelY = AccelY_high << 8 | AccelY_low;
//				sprintf(bufferData, "AccelY = %d \n", (int) AccelY);
//				writeMsg(&handlerUsart1, bufferData);
//				rxData = '\0';
//
//			}
//
//			else if (rxData == 'z'){
//				uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_ZOUT_L);
//				uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_ZOUT_H);
//			    AccelZ = AccelZ_high << 8 | AccelZ_low;
//				sprintf(bufferData, "AccelZ = %d \n", (int) AccelZ);
//				writeMsg(&handlerUsart1, bufferData);
//				rxData = '\0';
//			}
//			else{
//				rxData = '\0';
//			}
//

void Welcome(void){

	LCD_setCursor(&handlerLCD, 0, 0, LCD_ADDRESS);
	LCD_sendSTR(&handlerLCD, "Welcome", LCD_ADDRESS);

	LCD_setCursor(&handlerLCD, 2, 0, LCD_ADDRESS);
	LCD_sendSTR(&handlerLCD, "Escriba menu..", LCD_ADDRESS);

	        for ( int i = 0; i<10000; i++);
}
