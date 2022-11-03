/*
 * mainP.c
 *
 *  Created on: 1/11/2022
 *      Author: German
 *
 *      Solución del Parcial Taller V
 */

#include <stm32f4xx.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "GPIOxDriver.h"
#include "ADCDriver.h"
#include "USARTxDriver.h"
#include "BasicTimer.h"
#include "EXTIDriver.h"
#include "PwmDriver.h"
#include "LCDxDriver.h"

// Variables para el blinky
GPIO_Handler_t handlerStateLedPin 		= {0};
BasicTimer_Handler_t handlerTIM2 		= {0};
uint8_t state1 = 0;

// Variables para el PWM
GPIO_Handler_t handlerPinPWM 			= {0};
PWM_Handler_t handlerPWM				= {0};

// Variables USART6
GPIO_Handler_t handlerTx 				= {0};
GPIO_Handler_t handlerRx 				= {0};
USART_Handler_t handlerRxTxUSART 		= {0};
uint8_t text 							= 0;
uint8_t letter 							= 0;
BasicTimer_Handler_t handlerTIM3 		= {0};
GPIO_Handler_t handlerExternalLedPin 	= {0};
int8_t state2 = 0;
BasicTimer_Handler_t handlerTIM4 		= {0};

/* Configuración para el I2C */
GPIO_Handler_t	handlerI2CSDA			= {0};
GPIO_Handler_t	handlerI2CSCL 			= {0};
I2C_Handler_t	handlerLCD 				= {0};
uint8_t			i2cBuffer				= 0;
#define LCD_ADRESS		0b0100111

uint8_t counterReception = 0;
char bufferReception[64] = {0};
char cmd[64];
char userMsg[64];
bool stringComplete = false;
unsigned int firstParameter;
unsigned int secondParameter;


void initSystem(void);
void parseCommands (char *ptrBufferReception);

int main (void)
{
	initSystem();
	LCD_Init(&handlerLCD);

	// Configuración: State Led
	handlerStateLedPin.pGPIOx								= GPIOA;
	handlerStateLedPin.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;
	handlerStateLedPin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerStateLedPin.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerStateLedPin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerStateLedPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerStateLedPin.GPIO_PinConfig.GPIO_PinAltFunMode	= AF0;      //Ninguna función

	// Configuración: TIM2
	handlerTIM2.ptrTIMx								= TIM2;
	handlerTIM2.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerTIM2.TIMx_Config.TIMx_speed				= BTIMER_SPEED_1ms;
	handlerTIM2.TIMx_Config.TIMx_period				= 250;

//	// Configuración: PWM
//	handlerPWM.ptrTIMx					= TIM3;
//	handlerPWM.pwmConfig.PWM_channel 	= PWM_CHANNEL_1;
//	handlerPWM.pwmConfig.PWM_mode		= PWM_MODE_1;
//	handlerPWM.pwmConfig.Timer_mode		= TIMER_PWM_MODE_UP;
//	handlerPWM.pwmConfig.Timer_speed	= TIMER_PWM_INCR_SPEED_10us;
//	handlerPWM.pwmConfig.Timer_period	= 2000;
//	handlerPWM.pwmConfig.PWM_dutycycle	= 0;

	//Configuramos el TIM3
	handlerTIM3.ptrTIMx								= TIM3;
	handlerTIM3.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerTIM3.TIMx_Config.TIMx_speed				= BTIMER_SPEED_1ms;
	handlerTIM3.TIMx_Config.TIMx_period				= 300;

	// Configuración: Pin PWM
	handlerPinPWM.pGPIOx							= GPIOA;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinNumber      = PIN_6;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinMode 		 = GPIO_MODE_ALTFN;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinOPType		 = GPIO_OTYPE_PUSHPULL;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinSpeed 		 = GPIO_OSPEED_MEDIUM;
	handlerPinPWM.GPIO_PinConfig.GPIO_PinAltFunMode	 = AF2;

	//Configuramos el PIN Tx
	handlerTx.pGPIOx                       			= GPIOA;
	handlerTx.GPIO_PinConfig.GPIO_PinNumber			= PIN_2;
	handlerTx.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerTx.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerTx.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerTx.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerTx.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF7;
			;

	//Configuramos el PIN Rx
	handlerRx.pGPIOx                       			= GPIOA;
	handlerRx.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;
	handlerRx.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerRx.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerRx.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerRx.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerRx.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF7;

	//Configuramos el USART
	handlerRxTxUSART.ptrUSARTx                       		= USART2;
	handlerRxTxUSART.USART_Config.USART_mode				= USART_MODE_RXTX;
	handlerRxTxUSART.USART_Config.USART_baudrate			= USART_BAUDRATE_9600;
	handlerRxTxUSART.USART_Config.USART_datasize			= USART_DATASIZE_8BIT;
	handlerRxTxUSART.USART_Config.USART_parity				= USART_PARITY_NONE;
	handlerRxTxUSART.USART_Config.USART_stopbits			= USART_STOPBIT_1;
	handlerRxTxUSART.USART_Config.USART_IntRx	= USART_RX_INTERRUPT_ENABLE;
	handlerRxTxUSART.USART_Config.USART_IntTx	= USART_TX_INTERRUP_DISABLE;

	//Configuramos el External Led
	handlerExternalLedPin.pGPIOx								= GPIOB;
	handlerExternalLedPin.GPIO_PinConfig.GPIO_PinNumber			= PIN_9;
	handlerExternalLedPin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerExternalLedPin.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerExternalLedPin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerExternalLedPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerExternalLedPin.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;      //Ninguna función

	//Configuramos el TIM4
	handlerTIM3.ptrTIMx								= TIM4;
	handlerTIM3.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerTIM3.TIMx_Config.TIMx_speed				= BTIMER_SPEED_1ms;
	handlerTIM3.TIMx_Config.TIMx_period				= 300;

	// Cargamos configuraciones
	GPIO_Config(&handlerStateLedPin);
	BasicTimer_Config(&handlerTIM2);

//	GPIO_Config(&handlerPinPWM);
////	PWM_Config(&handlerPWM);

	GPIO_Config(&handlerTx);
	GPIO_Config(&handlerRx);
	USART_Config(&handlerRxTxUSART);
	GPIO_Config(&handlerExternalLedPin);
	BasicTimer_Config(&handlerTIM3);

	BasicTimer_Config(&handlerTIM4);


	// Prueba PWM
//	GPIO_Config(&handlerPinPWM);
//	GPIO_WritePin(&handlerPinPWM, SET);
////	starPwmSignal(&handlerPWM);

	while(1){

		// Activar blinky del LED de estado
		GPIO_WritePin(&handlerStateLedPin,state1);
		GPIO_WritePin(&handlerExternalLedPin,state2);

		//Hacemos un "eco" con el valor que nos llega por el serial
		if (text != '\0'){

			bufferReception[counterReception] = text;
			counterReception++;

			if(text =='@'){
				stringComplete = true;
				bufferReception [counterReception - 1] = '\0';

				counterReception = 0;

			}

			text = '\0';

		}

		if (stringComplete){
			parseCommands(bufferReception);
			stringComplete = false;

			for(int i = 0; i < 64; i++){
				bufferReception[i] = '\0';
			}

		}

		// Prueba USART6
		//texto = readChar(&handlerRxTxUSART);
//			if ((text != 0) && (text != 'O')){
//				writeChar(&handlerRxTxUSART, text);
//				// Si el carácter es "n" se genera un blinky 100 ms
//				if (text == 'n'){
//					// se edita el período
//					handlerTIM3.TIMx_Config.TIMx_period	= 100;
//					// Se carga la configuración
//					BasicTimer_Config(&handlerTIM3);
//
//					// se actualiza la variable para guardar el último periodo seleccionado
//					letter = 'n';
//					// Se reinicia la variable
//					text = 0;
//				}
//			}
	}
	return 0;
}

void parseCommands (char *ptrBufferReception){
	sscanf(ptrBufferReception, "%s %u %u %s", cmd, &firstParameter, &secondParameter, userMsg);
	__NOP();

	if((strcmp(cmd, "HighBlinky") == 0)){
		writeMsg(&handlerRxTxUSART, "HighBlinky\n\r");
		//writeChar(&handlerRxTxUSART, '\n');
		handlerTIM3.TIMx_Config.TIMx_period	= 100;
		BasicTimer_Config(&handlerTIM3);
		LCD_setCursor(&handlerLCD,0,0);
		LCD_sendSTR(&handlerLCD,"Blinky:100 ms    ");
		LCD_setCursor(&handlerLCD,1,0);
		LCD_sendSTR(&handlerLCD,"                    ");
	}
	else if(strcmp(cmd, "NoBlinky") == 0){
		writeMsg(&handlerRxTxUSART, "NoBlinky\n\r");
		//writeChar(&handlerRxTxUSART, '\n');
		 StopCounterTimer(&handlerTIM3);
		//handlerTIM3.TIMx_Config.TIMx_period	= 0;
		//BasicTimer_Config(&handlerTIM3);
		GPIO_WritePin(&handlerExternalLedPin, SET);
		LCD_setCursor(&handlerLCD,0,0);
		LCD_sendSTR(&handlerLCD,"Blinky:0 ms      ");
		LCD_setCursor(&handlerLCD,1,0);
			LCD_sendSTR(&handlerLCD,"                    ");
	}
	else if(strcmp(cmd, "StartMove") == 0){
		writeMsg(&handlerRxTxUSART, "StartMove\n\r");
		handlerPWM.config.duttyCicle =170;
//		handlerPWM.pwmConfig.PWM_dutycycle = 170;
		pwm_Config(&handlerPWM);
		LCD_setCursor(&handlerLCD,0,0);
		LCD_sendSTR(&handlerLCD,"Start Move       ");
		LCD_setCursor(&handlerLCD,1,0);
		LCD_sendSTR(&handlerLCD,"Duty = 170 ");
	}
	else if(strcmp(cmd, "StopMove") == 0){
		writeMsg(&handlerRxTxUSART, "StopMove\n\r");
		handlerPWM.config.duttyCicle =0;
//		handlerPWM.pwmConfig.PWM_dutycycle = 0;
		pwm_Config(&handlerPWM);
		LCD_setCursor(&handlerLCD,0,0);
		LCD_sendSTR(&handlerLCD,"Stop Move      ");
		LCD_setCursor(&handlerLCD,1,0);
		LCD_sendSTR(&handlerLCD,"Duty = 0     ");
	}
	else if(strcmp(cmd, "CWHigh") == 0){
		writeMsg(&handlerRxTxUSART, "CW-High\n\r");
//		handlerPWM.pwmConfig.PWM_dutycycle = 10;
		handlerPWM.config.duttyCicle =10;
		pwm_Config(&handlerPWM);
		LCD_setCursor(&handlerLCD,0,0);
		LCD_sendSTR(&handlerLCD,"CW-High         ");
		LCD_setCursor(&handlerLCD,1,0);
		LCD_sendSTR(&handlerLCD,"Duty = 10       ");
	}
	else if(strcmp(cmd, "CWLow") == 0){
		writeMsg(&handlerRxTxUSART, "CW-Low\n\r");
//		handlerPWM.pwmConfig.PWM_dutycycle = 140;
		handlerPWM.config.duttyCicle =140;
		pwm_Config(&handlerPWM);
		LCD_setCursor(&handlerLCD,0,0);
		LCD_sendSTR(&handlerLCD,"CW-Low        ");
		LCD_setCursor(&handlerLCD,1,0);
		LCD_sendSTR(&handlerLCD,"Duty = 140 ");
	}
	else if(strcmp(cmd, "CCWHigh") == 0){
		writeMsg(&handlerRxTxUSART, "CCW-High\n\r");
//		handlerPWM.pwmConfig.PWM_dutycycle = 160;
		handlerPWM.config.duttyCicle =160;
		pwm_Config(&handlerPWM);
		LCD_setCursor(&handlerLCD,0,0);
		LCD_sendSTR(&handlerLCD,"CCW-High       ");
		LCD_setCursor(&handlerLCD,1,0);
		LCD_sendSTR(&handlerLCD,"Duty = 160     ");
	}
	else if(strcmp(cmd, "CCWLow") == 0){
		writeMsg(&handlerRxTxUSART, "CCW-Low\n\r");
//		handlerPWM.pwmConfig.PWM_dutycycle = 400;
		handlerPWM.config.duttyCicle =400;
		pwm_Config(&handlerPWM);
		LCD_setCursor(&handlerLCD,0,0);
		LCD_sendSTR(&handlerLCD,"CCW-Low      ");
		LCD_setCursor(&handlerLCD,1,0);
		LCD_sendSTR(&handlerLCD,"Duty = 400      ");
	}
	else if(strcmp(cmd, "LowBlinky") == 0){
		writeMsg(&handlerRxTxUSART, "Low Blinky\n\r");
		handlerTIM3.TIMx_Config.TIMx_period	= 450;
		BasicTimer_Config(&handlerTIM3);
		LCD_setCursor(&handlerLCD,0,0);
		LCD_sendSTR(&handlerLCD,"Blinky:450 ms     ");
		LCD_setCursor(&handlerLCD,1,0);
		LCD_sendSTR(&handlerLCD,"                    ");
	}
	else if(strcmp(cmd, "AllCMD") == 0){
		writeMsg(&handlerRxTxUSART, "AllCMD");
		LCD_setCursor(&handlerLCD,0,0);
		LCD_sendSTR(&handlerLCD,"AllCMD");
		LCD_setCursor(&handlerLCD,1,0);
		LCD_sendSTR(&handlerLCD,"Hay 9 CMD más:");
	}

	else{
		writeMsg(&handlerRxTxUSART, "Wrong CMD: ");
		writeMsg(&handlerRxTxUSART, cmd);
		writeMsg(&handlerRxTxUSART, "\n\r");
	}

}

void initSystem(void) {

	/* Configurando los pines sobre los que funciona el I2C1 */
	handlerI2CSCL.pGPIOx								= GPIOB;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_OPENDRAIN;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
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
	handlerLCD.modeI2C		= I2C_MODE_FM;
	handlerLCD.slaveAddress	= LCD_ADRESS;
	I2C_Config(&handlerLCD);
}
void BasicTimer2_CallBack(void){
	state1 ^= 1;
}

void BasicTimer4_CallBack(void){
	state2 ^= 1;
}

void USART2Rx_CallBack(void){
	//text = readChar(&handlerRxTxUSART);
	text = getRxData();
}
