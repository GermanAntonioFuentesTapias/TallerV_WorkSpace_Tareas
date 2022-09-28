/**
 ******************************************************************************
 * @file           : main.c
 * @author         : German Antonio Fuentes Tapias
 * @Correo         : gafuentest@unal.edu.co
 * @brief          : Elaboración de la tarea 3 Taller V
 ******************************************************************************
 * El codigo que se vera a continuación es la elaboración de la tarea #3 de taller 5
 * En la cual se desarrollan 3 puntos
 * 1. Hacer que el USER_LED actúe como el LED de estado
 * 2 .El sistema debe enviar cada 250ms un carácter por el puerto serial de su elección (USART_1 o USART_6).
 * 3 .Utilizando el USER_BUTTON, si se presiona el botón, se continúa enviando el carácter cada 250ms,
 * pero esta vez el carácter a ser enviado debe cambiar en cada envío, o sea,  siempre será diferente del anterior.
 *  Si se deja de presionar, el sistema regresa a la descripción de cómo funciona en el punto 2
 ******************************************************************************
 */

/* Inc necesario para la realización de la tarea */

#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"

/* Variables necesarias la activación de banderas,blinky y sumatoria al ser presionado el boton*/

int variable          = 0;
int Lectura           = 0;
int Lectura2          = 0;
uint8_t BlinkySimple  = 0;

/* La variables tendran los diferentes tipos */

BasicTimer_Handler_t   handlerTimer2    = {0};
GPIO_Handler_t         BlinkySimplePin  = {0};
GPIO_Handler_t         UsarButton       = {0};
GPIO_Handler_t         handlerTx        = {0};
USART_Handler_t        handlerUsar      = {0};

/* Creacioón de el programa principal a utilizar en la tarea */

int main(void){
//   Esta es la inicialización del punto 1


//Hacer que el USER_LED actúe como el LED de estado

	BlinkySimplePin.pGPIOx= GPIOA;
//
	BlinkySimplePin.GPIO_PinConfig.GPIO_PinNumber =        PIN_5;
	BlinkySimplePin.GPIO_PinConfig.GPIO_PinMode   =        GPIO_MODE_OUT;
	BlinkySimplePin.GPIO_PinConfig.GPIO_PinOPType  =       GPIO_OTYPE_PUSHPULL;
	BlinkySimplePin.GPIO_PinConfig.GPIO_PinPuPdControl =   GPIO_PUPDR_NOTHING;
	BlinkySimplePin.GPIO_PinConfig.GPIO_PinSpeed =         GPIO_OSPEED_MEDIUM;
	BlinkySimplePin.GPIO_PinConfig.GPIO_PinAltFunMode =    AF0;

// Este el BlinkySimple
	GPIO_Config(&BlinkySimplePin);

//	GPIO_WritePin(&BlinkySimplePin, SET);

	 handlerTimer2.ptrTIMx = TIM2;
	 handlerTimer2.TIMx_Config.TIMx_mode = BTIMER_MODE_UP; // CUENTA HACIA ARRIBA
	 handlerTimer2.TIMx_Config.TIMx_speed = BTIMER_SPEED_100us; // La velocidad
	 handlerTimer2.TIMx_Config.TIMx_period = 2500;
     handlerTimer2.TIMx_Config.TIMx_interruptEnable = 0;
	 BasicTimer_Config(&handlerTimer2);

//Este es el SEGUNDO punto -->> El sistema debe enviar cada 250ms un carácter por el puerto serial de su elección
	 //   Se elige el GPIOB  y el USART 1
	 	handlerTx.pGPIOx= GPIOB;
	 //
	 	handlerTx.GPIO_PinConfig.GPIO_PinNumber =        PIN_6;
	 	handlerTx.GPIO_PinConfig.GPIO_PinMode   =        GPIO_MODE_ALTFN; // ah mk jajajaj, no olvidar colocar funcion alternativa ome hueva
	 	handlerTx.GPIO_PinConfig.GPIO_PinOPType  =       GPIO_OTYPE_PUSHPULL;
	 	handlerTx.GPIO_PinConfig.GPIO_PinPuPdControl =   GPIO_PUPDR_NOTHING;
	 	handlerTx.GPIO_PinConfig.GPIO_PinSpeed =         GPIO_OSPEED_MEDIUM;
	 	handlerTx.GPIO_PinConfig.GPIO_PinAltFunMode =    AF7;

//Listo sigamos viendo pai, mera hueva ome, R

	 	GPIO_Config(&handlerTx);

	 	// Configuración Usart

	 	handlerUsar.ptrUSARTx = USART1 ;
//Yo entiendo ome
		handlerUsar.USART_Config.USART_mode =       USART_MODE_TX;
	    handlerUsar.USART_Config.USART_baudrate =   USART_BAUDRATE_19200;
	    handlerUsar.USART_Config.USART_datasize =   USART_DATASIZE_9BIT;
	    handlerUsar.USART_Config.USART_parity   =   USART_PARITY_EVEN;
	    handlerUsar.USART_Config.USART_stopbits  =  USART_STOPBIT_1;

// Cargo de configuracion

	    USART_Config(&handlerUsar);
// Configuración para el USARBotton

	    //Inicialiación
	    UsarButton.pGPIOx = GPIOC;

	    //Configuración de usuario
	    UsarButton.GPIO_PinConfig.GPIO_PinNumber      =  PIN_13;
	    UsarButton.GPIO_PinConfig.GPIO_PinMode        =  GPIO_MODE_IN;
	    UsarButton.GPIO_PinConfig.GPIO_PinOPType      =  GPIO_OTYPE_PUSHPULL;
	    UsarButton.GPIO_PinConfig.GPIO_PinPuPdControl =  GPIO_PUPDR_PULLUP;
	    UsarButton.GPIO_PinConfig.GPIO_PinSpeed       =  GPIO_OSPEED_HIGH;
	    UsarButton.GPIO_PinConfig.GPIO_PinAltFunMode  =  AF0;

	    GPIO_Config(&UsarButton);

//
	while(1){

		//Cada 250 ms estaria enviando un cambio de estado


			if(GPIO_ReadPin(&UsarButton) == (RESET)){

				if(Lectura){
					writeChar(&handlerUsar,variable);
					variable ++;
					Lectura = 0;
				}
			  }

			else{
				 if(Lectura2){

					 writeChar(&handlerUsar,'m');
					 Lectura2 = 0;
				 }
               }
			}


	return 0;
}
// CallBack del timer!!!!

void BasicTimer2_CallBack(void){
    //Se tiene el blinker segun los 250 ms
	BlinkySimple = !BlinkySimple;

	if(BlinkySimple){
		GPIO_WritePin(&BlinkySimplePin, SET);
	}else{
		GPIO_WritePin(&BlinkySimplePin, RESET);
	}

	Lectura = 1;

	Lectura2 = 1;

    }



