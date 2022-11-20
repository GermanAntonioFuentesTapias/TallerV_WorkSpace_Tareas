
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

GPIO_Handler_t    handlerBlinkyPin   = {0};

//Timer encargado del Blinky
BasicTimer_Handler_t  handlerBlinkyTimer = {0};
uint8_t handlerLed                      = 0;
GPIO_Handler_t	handlerI2CSDA			= {0};
GPIO_Handler_t	handlerI2CSCL 			= {0};
I2C_Handler_t	handlerLCD 				= {0};
char dataLCD[64] = {0};

#define LCD_ADRESS		0b0100111

//Definición de las cabeceras del main
void initSystem (void);

int main(void){
	initSystem();
    startCounterTimer(&handlerBlinkyTimer);
	LCD_Init(&handlerLCD);
	delay_10();
	LCD_ClearScreen(&handlerLCD);
	delay_10();
	sprintf(dataLCD, "Por fin perras");
	LCD_setCursor(&handlerLCD,1,0);
	LCD_sendSTR(&handlerLCD,dataLCD);

while (1){


}
return 0;
}



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
