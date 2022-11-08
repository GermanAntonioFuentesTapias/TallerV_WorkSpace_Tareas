/*
 * main.c
 *
 *  Created on: 1/11/2022
 *      Author: German Antonio Fuentes Tapias
 *
 *      Solución del Parcial Taller V
 *
 *      Tema 2.
 *
 *      Paridad Even
 *      Usart1 para comunicación serial
 *      Velocidad 19200
 *
 *
 *      Led de estado, haciendo blinky a 250 ms. (0 puntos)
        Display LCD o Display OLED funcionando. (15 puntos)
        Comunicación USART trabajando con comandos, con por lo menos 10 comandos verificables. (25 puntos)
        RTC interno del STM32 funcionando con los CMDs del punto 2, utilizando el cristal de 32KHz de la board (LSE). (15 puntos)
        Sensor funcionando (10 puntos):
        accel con lectura en dos ejes
        o Joystick con lectura en X e Y.
        Actuador funcionando (10 puntos)
        Motor servo (PWM), respondiendo a los datos del sensor: Los del Joystick o del acelerómetro deben controlar el movimiento del motor (si es de giro continuo controlar dirección y velocidad de giro y si es de 180° controlar la posición).

        LEDs RGB (PWM), respondiendo a los datos del sensor, espacio de color HSV.
 *
 *
 *
 *
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

int16_t  angulo = 0;

/* Ejes utilizados en el acelerometro */
int16_t AccelX = 0;
int16_t AccelY = 0;
int16_t AccelZ = 0;

/* Definición de variables Handler */
GPIO_Handler_t handlerBlinkyPin         = {0};
GPIO_Handler_t handlerTx     	    	= {0};
GPIO_Handler_t handlerRx	 	     	= {0};
GPIO_Handler_t handlerI2cSDA 		    = {0};
GPIO_Handler_t handlerI2cSCL		    = {0};
I2C_Handler_t handlerAcelerometro       = {0};
I2C_Handler_t handlerLCD                = {0};
USART_Handler_t handlerUsart1           = {0};
BasicTimer_Handler_t handlerBlinkyTimer = {0};

/* Definición de variables banderas*/
uint8_t handlerLed  = 0;
uint8_t rxData      =  0;
uint8_t rxDataCMD   = 0;
uint8_t Bandera     = 0;
uint8_t BanderaRGB  = 0;
uint8_t BanderaRGB2 = 0;

/* Handler de banderas de comandos */
BasicTimer_Handler_t handlerCommands = {0};
BasicTimer_Handler_t handlerRGB      = {0};

/* Cargar datos */
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

#define LCD_ADDRESS     0b0100111

/* Funciones para comandos */


uint8_t counterReception = 0;
char bufferReception[64] = {0};
char cmd[64];
char userMsg[64];
char data[64];
bool stringComplete = false;
bool makeUpdateLCD  = false;
unsigned int firstParameter;
unsigned int secondParameter;


/****************************************************************/




/* Definición de las cabezeras de las funciones */

void initSystem (void);
void parseCommands (char *ptrBufferReception); // Llama los comandos

int main(void)
{
   /* Llamamos a la función initSystem para que se ejecute */

	initSystem();

	/* Comienza de PWM */

	startPwmSignal(&handlerPWMR);
	startPwmSignal(&handlerPWMB);
	startPwmSignal(&handlerPWMG);


  /* Configuración iniciación LCD */
//	LCD_Init(&handlerLCD);
//		delay_10();
//		LCD_Clear(&handlerLCD);
//		delay_10();
//		sprintf(data, "Hola ");
//		LCD_setCursor(&handlerLCD,1,0);
//		LCD_sendSTR(&handlerLCD,data);

	/* Ciclo principal del programa */

	while(1){

		/* Se escribe el valor de la variable estadoBlinky en el handlerBlinkyPin */
		GPIO_WritePin(&handlerBlinkyPin,handlerLed );

		/* Se llama el timer del Blinky */

		startCounterTimer(&handlerBlinkyTimer);


		/* Función del RTC que carga los datos */
		// Cargandolos de forma acedente

		ptrData =leer_datos();

		   Seg = ptrData[0];
		   Min = ptrData[1];
		   Hor = ptrData[2];
		   Sem = ptrData[3];
		   Day = ptrData[4];
		   Mes = ptrData[5];
		   Ano = ptrData[6];

       /* Para el punto  6 del actuador */

       if (Bandera){
    	   /* Carga la lectura del i2c */
    	   i2cBuffer = i2c_readSingleRegister(&handlerAcelerometro, WHO_AM_I);
			i2cBuffer = i2c_readSingleRegister(&handlerAcelerometro, PWR_MGMT_l);
			i2c_writeSingleRegister(&handlerAcelerometro, PWR_MGMT_l, 0x00);

			/* Tomas las posiciones leidas y las almacena */

			uint8_t AccelX_low = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_XOUT_L);
			uint8_t AccelX_high = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_XOUT_H);
			AccelX = AccelX_high << 8 | AccelX_low;

			uint8_t AccelY_low = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_YOUT_L);
			uint8_t AccelY_high = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_YOUT_H);
			AccelY = AccelY_high << 8 | AccelY_low;

			/* Imprime las posiciones */
			sprintf(bufferDataY, "AccelY = %d \n ", (int) AccelY);
			sprintf(bufferData, "AccelX = %d \n", (int) AccelX);
			writeMsg(&handlerUsart1, bufferData);
			writeMsg(&handlerUsart1, bufferDataY);
			rxDataCMD = '\0';

			Bandera = 0;

       }


          /* Para el punto 6 */

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

			 /* Se toman las posiciones y se organiza en un ciruculo HSV, los condicionales generan valores
			  * segun donde se encuentren acercandose a su referencia del circulo HSV
			  */

			/* Primer cuadrante */

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
				  updateDuttyCycle(&handlerPWMB, 30);
				  updateDuttyCycle(&handlerPWMR, 30);
				  updateDuttyCycle(&handlerPWMG, 0 );


			  } else{
				  __NOP();
			  }

			  /* Segundo cuadrante */

			  }else if( AccelX < 0 && AccelY > 0){
				  angulo = atan(AccelY/(-AccelX))*(180/(acos(-1)));
				  angulo = 180 - angulo;

				  if (angulo >= 90 && angulo < 120){

				  updateDuttyCycle(&handlerPWMB, 50);
				  updateDuttyCycle(&handlerPWMR, 50);
				  updateDuttyCycle(&handlerPWMG, 0);

				  } else if (angulo >= 120 && angulo < 150){
					  updateDuttyCycle(&handlerPWMB, 0);
					  updateDuttyCycle(&handlerPWMR, 0);
					  updateDuttyCycle(&handlerPWMG, 100);


				  } else if ( angulo >= 150 && angulo < 180) {
					  updateDuttyCycle(&handlerPWMB, 50);
					  updateDuttyCycle(&handlerPWMR, 0);
					  updateDuttyCycle(&handlerPWMG, 100 );


				  } else{
					  __NOP();
				  }


			 /* Tercer cuadrante */

			  } else if( AccelX <0 && AccelY < 0){
				  angulo = atan(-AccelY/(-AccelX))*(180/(acos(-1)));
				  angulo = 180 + angulo;

				  if(angulo >= 180 && angulo <210){

			  updateDuttyCycle(&handlerPWMB, 100);
				  updateDuttyCycle(&handlerPWMR, 0);
				  updateDuttyCycle(&handlerPWMG, 50);

			  } else if (angulo >= 210 && angulo < 240){
				  updateDuttyCycle(&handlerPWMB, 100);
				  updateDuttyCycle(&handlerPWMR, 0);
				  updateDuttyCycle(&handlerPWMG, 10);


			  } else if ( angulo >= 240 && angulo < 270) {
				  updateDuttyCycle(&handlerPWMB, 100);
				  updateDuttyCycle(&handlerPWMR, 30);
				  updateDuttyCycle(&handlerPWMG, 0 );


			  } else{
								  __NOP();
							  }

              /* Cuarto cuadrante */

			 } else if ( AccelX > 0 && AccelY <= 0){
			  angulo = atan(-AccelY/(AccelX))*(180/(acos(-1)));
			  angulo = 360 - angulo;

			  if( angulo >= 270 && angulo < 300){

			  updateDuttyCycle(&handlerPWMB, 100);
			  updateDuttyCycle(&handlerPWMR, 0);
			  updateDuttyCycle(&handlerPWMG, 0);

			  } else if (angulo >= 300 && angulo < 330){
				  updateDuttyCycle(&handlerPWMB, 100);
				  updateDuttyCycle(&handlerPWMR, 100);
				  updateDuttyCycle(&handlerPWMG, 0);


			  } else if ( angulo >= 330 && angulo <= 360) {
				  updateDuttyCycle(&handlerPWMB, 0);
				  updateDuttyCycle(&handlerPWMR, 100);
				  updateDuttyCycle(&handlerPWMG, 0 );

				  }

			  } else if (AccelX == 0 || AccelY == 0){
				  angulo = 0;
				  updateDuttyCycle(&handlerPWMR, 100);
				  updateDuttyCycle(&handlerPWMB, 100);
				  updateDuttyCycle(&handlerPWMG, 100);
			  }

				sprintf(welcomer, "Angulo = %d \n \r", (int) angulo);
				writeMsg(&handlerUsart1, welcomer);
				rxDataCMD = '\0';

    	   BanderaRGB = 0;
       }



        /* Para el punto 3 se necesita una recepción que me cierre el comando por lo que se usa @ */

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
		// asi que verifica el comando si es true entra, se carga y se
		// reinicia el valor a false

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

	/* Punto 1 */

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

	/* Configuración USART Transmisión */

	/* Handler para el PIN B6 para transmisión */
	handlerTx.pGPIOx                       			= GPIOB;
	handlerTx.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;				// Pin TX USART1
	handlerTx.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerTx.GPIO_PinConfig.GPIO_PinOPType		    = GPIO_OTYPE_PUSHPULL;
	handlerTx.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerTx.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerTx.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;

	/* Se carga la configuración */
	GPIO_Config(&handlerTx);

	/* Handler para el PIN B7  para recepción */
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


/***************************************************************************************************/

	/* Para el blinky de estado */

	/* Handler para el Timer2 */
	handlerBlinkyTimer.ptrTIMx 								= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed				= BTIMER_SPEED_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period 				= 2500; 			// Periodo de 250ms

	/* Se carga la configuración */
		BasicTimer_Config(&handlerBlinkyTimer);

        /* Para bandera de commands */

		handlerCommands.ptrTIMx                                = TIM4;
		handlerCommands.TIMx_Config.TIMx_mode                  = BTIMER_MODE_UP;
		handlerCommands.TIMx_Config.TIMx_speed                 = BTIMER_SPEED_1ms;
		handlerCommands.TIMx_Config.TIMx_period                = 1000;

        /* Cargando configuración */
	    BasicTimer_Config(&handlerCommands);

	    /* Para I2C */

	    handlerRGB.ptrTIMx                                    = TIM3;
	    handlerRGB.TIMx_Config.TIMx_mode                      = BTIMER_MODE_UP;
	    handlerRGB.TIMx_Config.TIMx_speed                     = BTIMER_SPEED_100us;
	    handlerRGB.TIMx_Config.TIMx_period                    = 10000;

	    /* Cargo de configuración */
	    BasicTimer_Config(&handlerRGB);


	/* Punto 2 configuración */

	/* Aqui se encargara de lo de la LDC */

	//Definimos puertos

	/* Para el SCL */

	handlerLCDcSCL.pGPIOx                               = GPIOA;
	handlerLCDcSCL.GPIO_PinConfig.GPIO_PinNumber        = PIN_8;
	handlerLCDcSCL.GPIO_PinConfig.GPIO_PinMode          = GPIO_MODE_ALTFN;
	handlerLCDcSCL.GPIO_PinConfig.GPIO_PinOPType        = GPIO_OTYPE_OPENDRAIN;
	handlerLCDcSCL.GPIO_PinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_PULLUP;
	handlerLCDcSCL.GPIO_PinConfig.GPIO_PinSpeed         = GPIO_OSPEED_FAST;
	handlerLCDcSCL.GPIO_PinConfig.GPIO_PinAltFunMode    = AF4;

	/* Se carga a la configuración */
	GPIO_Config(&handlerLCDcSCL);

	/* Para el SDA */
	handlerLCDcSDA.pGPIOx                               = GPIOC;
	handlerLCDcSDA.GPIO_PinConfig.GPIO_PinNumber        = PIN_9;
	handlerLCDcSDA.GPIO_PinConfig.GPIO_PinMode          = GPIO_MODE_ALTFN;
	handlerLCDcSDA.GPIO_PinConfig.GPIO_PinOPType        = GPIO_OTYPE_OPENDRAIN;
	handlerLCDcSDA.GPIO_PinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_PULLUP;
	handlerLCDcSDA.GPIO_PinConfig.GPIO_PinSpeed         = GPIO_OSPEED_FAST;
	handlerLCDcSDA.GPIO_PinConfig.GPIO_PinAltFunMode    = AF4;

	/* Se carga la configuración */

	GPIO_Config(&handlerLCDcSDA);

	/* Se carga la configuración I2C  de la LCD*/

	/* Handler para el acelerometro */
	handlerLCD.ptrI2Cx				= I2C3;
	handlerLCD.modeI2C				= I2C_MODE_SM;
	handlerLCD.slaveAddress		    = LCD_ADDRESS  ;

	/* Cargo la configuración de LCD */

	i2c_config(&handlerLCD);

	/***********************************************************************/

	/* Acelerometro o actuador punto 5. */

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


   /* Se carga la configuración */
   i2c_config(&handlerAcelerometro);

   /***************************************************************************/

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
	handlerPWMB.config.duttyCicle                         = 120;
	handlerPWMB.config.periodo                            = 120;
	handlerPWMB.config.prescaler                          = BTIMER_SPEED_100us;

    /* Se carga la configuración */
	pwm_Config(&handlerPWMB);

     handlerPWMG.ptrTIMx                                   = TIM3;
     handlerPWMG.config.channel                            = PWM_CHANNEL_3;
	 handlerPWMG.config.duttyCicle                         = 100;
	 handlerPWMG.config.periodo                            = 120;
	 handlerPWMG.config.prescaler                          = BTIMER_SPEED_100us;

	 /* Se carga la configuración */
	 pwm_Config(&handlerPWMG);

	 handlerPWMR.ptrTIMx                                   = TIM3;
	 handlerPWMR.config.channel                            = PWM_CHANNEL_1;
	 handlerPWMR.config.duttyCicle                         = 120;
	 handlerPWMR.config.periodo                            = 120;
	 handlerPWMR.config.prescaler                          = BTIMER_SPEED_100us;

	 /* Se carga la configuración */
	 pwm_Config(&handlerPWMR);

	 /* Despliegue de handler PWM super necesario */

	 enableOutput(&handlerPWMR);
	 enableOutput(&handlerPWMG);
	 enableOutput(&handlerPWMB);

     /* Punto 4 */

	 /* Configuración RTC inicial que se actualizara si el sistema no se apaga o reinicia*/

	 handlerRTC.DateTypeDef.RTC_Fecha     = 17;
	 handlerRTC.DateTypeDef.RTC_Mes       = 11;
	 handlerRTC.DateTypeDef.RTC_Semana    = Thus;
	 handlerRTC.DateTypeDef.RTC_Ano       = 98;
	 handlerRTC.TimeTypeDef.RTC_H12       = 12;
	 handlerRTC.TimeTypeDef.RTC_Hora      = 20;
	 handlerRTC.TimeTypeDef.RTC_Minutos   = 30;
	 handlerRTC.TimeTypeDef.RTC_Segundos  = 44;


     /*Se carga la configuración */

	 RTC_Config(&handlerRTC);


}

/* Para la comunicación */

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

	// Para actuador en el timer se mantenga activo cierto tiempo
	Bandera = 1;

	BanderaRGB = 1;


}


/*********** Punto   3 Comandos *******************/


void parseCommands(char *ptrBufferReception){

	sscanf(ptrBufferReception, " %s %u %u %s" , cmd , &firstParameter , &secondParameter ,userMsg);

	if(strcmp(cmd , "help") == 0){

		writeMsg(&handlerUsart1, "Help Menu CMDs: \n");
		writeMsg(&handlerUsart1, "1) Millos = Colores del grande \n ");
		writeMsg(&handlerUsart1, "2) Nacional= Color verde \n");
		writeMsg(&handlerUsart1, "3) Hora = en Hora - Minuto - Segundos \n");
		writeMsg(&handlerUsart1, "4) Fecha=  en Dia / Mes / Year \n" );
		writeMsg(&handlerUsart1, "5) Accel = Se inicializa el acelerometro \n ");
		writeMsg(&handlerUsart1, "6) StopA = Se detiene el acelerometro \n");
		writeMsg(&handlerUsart1, "7) Purpura = Se activa un bello color  \n)");
		writeMsg(&handlerUsart1, "8) Angulo = Se desea ver el angulo actual=  \n");
		writeMsg(&handlerUsart1, "9) z --> observación de tercer eje del acelerometro \n");
		writeMsg(&handlerUsart1, "10)Semaforo = Se hace una representacion de un semaforo por cada comando \n ");
		writeMsg(&handlerUsart1, "11) User = enviar 'User # # Mensaje @ para visualizar mensaje por ti \n ");



	} else if (strcmp(cmd , "Millos" ) == 0) {

		writeMsg(&handlerUsart1,  "CMD : Millos \n");
        updateDuttyCycle(&handlerPWMR, 0);
        updateDuttyCycle(&handlerPWMG, 0);
		updateDuttyCycle(&handlerPWMB, 100);
		writeMsg(&handlerUsart1, "Millos.FC \n \r");

	}


    else if (strcmp(cmd , "Nacional" ) == 0) {

    writeMsg(&handlerUsart1,  "CMD : Nacional \n");
    updateDuttyCycle(&handlerPWMR, 0);
    updateDuttyCycle(&handlerPWMG, 100);
	updateDuttyCycle(&handlerPWMB, 0);
	writeMsg(&handlerUsart1, "Nacional F.C \n \r");

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

		writeMsg(&handlerUsart1, "Conversion iniciada \n \r");
        startCounterTimer(&handlerCommands);
	}

	else if(strcmp(cmd , "StopA") == 0) {

		writeMsg(&handlerUsart1, "Conversion Detenida \n \r");
       StopCounterTimer(&handlerCommands);


	}

	else if(strcmp(cmd ,"Purpura") == 0){

	   writeMsg(&handlerUsart1, "Color purpura \n \r");

	   updateDuttyCycle(&handlerPWMR, 50);
		updateDuttyCycle(&handlerPWMG, 0);
		updateDuttyCycle(&handlerPWMB, 50);

	}

	else if(strcmp(cmd , "Rojo") ==0 ) {

		// Mirar comando
		}

	else if(strcmp(cmd, "Angulo") == 0){

		writeMsg(&handlerUsart1,  "CMD : Angulo \n");

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
			  updateDuttyCycle(&handlerPWMB, 30);
			  updateDuttyCycle(&handlerPWMR, 30);
			  updateDuttyCycle(&handlerPWMG, 0 );


		  } else{
			  __NOP();
		  }

		  }else if( AccelX < 0 && AccelY > 0){
			  angulo = atan(AccelY/(-AccelX))*(180/(acos(-1)));
			  angulo = 180 - angulo;

			  if (angulo >= 90 && angulo < 120){

				  updateDuttyCycle(&handlerPWMB, 50);
				  updateDuttyCycle(&handlerPWMR, 50);
				  updateDuttyCycle(&handlerPWMG, 0);

				  } else if (angulo >= 120 && angulo < 150){
					  updateDuttyCycle(&handlerPWMB, 0);
					  updateDuttyCycle(&handlerPWMR, 0);
					  updateDuttyCycle(&handlerPWMG, 100);


				  } else if ( angulo >= 150 && angulo < 180) {
					  updateDuttyCycle(&handlerPWMB, 50);
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
			  updateDuttyCycle(&handlerPWMG, 50);

			  } else if (angulo >= 210 && angulo < 240){
				  updateDuttyCycle(&handlerPWMB, 100);
				  updateDuttyCycle(&handlerPWMR, 0);
				  updateDuttyCycle(&handlerPWMG, 10);


			  } else if ( angulo >= 240 && angulo < 270) {
				  updateDuttyCycle(&handlerPWMB, 100);
				  updateDuttyCycle(&handlerPWMR, 30);
				  updateDuttyCycle(&handlerPWMG, 0 );


			  } else{
								  __NOP();
							  }



		  } else if ( AccelX > 0 && AccelY <= 0){
			  angulo = atan(-AccelY/(AccelX))*(180/(acos(-1)));
			  angulo = 360 - angulo;

			  if( angulo >= 270 && angulo < 300){

			  updateDuttyCycle(&handlerPWMB, 100);
			  updateDuttyCycle(&handlerPWMR, 0);
			  updateDuttyCycle(&handlerPWMG, 0);

			  } else if (angulo >= 300 && angulo < 330){
				  updateDuttyCycle(&handlerPWMB, 100);
				  updateDuttyCycle(&handlerPWMR, 100);
				  updateDuttyCycle(&handlerPWMG, 0);


			  } else if ( angulo >= 330 && angulo <= 360) {
				  updateDuttyCycle(&handlerPWMB, 0);
				  updateDuttyCycle(&handlerPWMR, 100);
				  updateDuttyCycle(&handlerPWMG, 0 );

			  }

		  } else if (AccelX == 0 || AccelY == 0){
			  angulo = 0;
			  updateDuttyCycle(&handlerPWMR, 100);
			  updateDuttyCycle(&handlerPWMB, 100);
			  updateDuttyCycle(&handlerPWMG, 100);
		  }

				sprintf(welcomer, "Angulo = %d \n \r", (int) angulo);
				writeMsg(&handlerUsart1, welcomer);
				rxDataCMD = '\0';


	} else if(strcmp(cmd, "z") == 0){


	uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_ZOUT_L);
	uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAcelerometro, ACCEL_ZOUT_H);
	AccelZ = AccelZ_high << 8 | AccelZ_low;
	sprintf(bufferData, "AccelZ = %d \n", (int) AccelZ);
	writeMsg(&handlerUsart1, bufferData);
	rxData = '\0';
	}

	else if( strcmp(cmd, "User") == 0) {

		writeMsg(&handlerUsart1,  "CMD : User \n");
		writeMsg(&handlerUsart1, userMsg);
		writeMsg(&handlerUsart1, "\n");
	}

	else if (strcmp( cmd , "Semaforo") == 0) {

		writeMsg(&handlerUsart1,  "CMD : Semaforo \n");
	    writeMsg(&handlerUsart1, "Semaforo \n \r");

	     for (int i=0;i<92500;i++){


	    updateDuttyCycle(&handlerPWMR, 100);
	    updateDuttyCycle(&handlerPWMG, 0);
	    updateDuttyCycle(&handlerPWMB, 0);
	    }

	     for (int i=0;i<92500;i++){

		updateDuttyCycle(&handlerPWMR, 50);
		updateDuttyCycle(&handlerPWMG, 30);
		updateDuttyCycle(&handlerPWMB, 0);


	    }

	    for (int i=0;i<92500;i++){

		updateDuttyCycle(&handlerPWMR, 0);
		updateDuttyCycle(&handlerPWMG, 100);
		updateDuttyCycle(&handlerPWMB, 0);
	    }


	}


	else{

		writeMsg(&handlerUsart1, " Wrong CMD \r");
	}


}

