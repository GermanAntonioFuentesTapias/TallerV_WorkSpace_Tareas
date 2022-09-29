/**
 ******************************************************************************
 * @file           : main.c
 * @author         : German Antonio Fuentes Tapias
 * @Mail        : gafuentest@unal.edu.co
 * @Resum          : Elaboración de la tarea 3 Taller V
 ******************************************************************************
 * El codigo que se vera a continuación es la elaboración de la tarea #3 de taller 5
 * En la cual se desarrollan 3 puntos
 * 1. Hacer que el USER_LED actúe como el LED de Bandera
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

int variable               = 0; //Caracter encargado de hacer una adición al momento de ser pulsado el botón
int Bandera                = 0; //Bandera encargada de mostrar cuando se esta presionando USAR_BUTTON
int Bandera_Normal         = 0; //Bandera encargada de mostrar cuando no esta presionando el USAR_BUTTON
uint8_t BlinkySimple       = 0; //Asignación a el blinkyn de led de estado

/* La variables tendran los diferentes tipos */

BasicTimer_Handler_t   handlerTimer2    = {0}; // Handler para el timer 2
GPIO_Handler_t         BlinkySimplePin  = {0}; // Handler para el BlinkySimple
GPIO_Handler_t         UsarButton       = {0}; // Handler para el UsarButton
GPIO_Handler_t         handlerTx        = {0}; // Handler para la trasmisión serial bit a bit
USART_Handler_t        handlerUsar      = {0}; // Handler para el Usar1

/* Creación del programa principal a utilizar en la tarea*/

int main(void){
    //   Esta es la inicialización del punto 1

   /*Hacer que el USER_LED actúe como el LED de Bandera*/

	BlinkySimplePin.pGPIOx= GPIOA; // Selección del GPIOA

	/*Configuración del Led de Blinky*/
	BlinkySimplePin.GPIO_PinConfig.GPIO_PinNumber =        PIN_5;
	BlinkySimplePin.GPIO_PinConfig.GPIO_PinMode   =        GPIO_MODE_OUT;
	BlinkySimplePin.GPIO_PinConfig.GPIO_PinOPType  =       GPIO_OTYPE_PUSHPULL;
	BlinkySimplePin.GPIO_PinConfig.GPIO_PinPuPdControl =   GPIO_PUPDR_NOTHING;
	BlinkySimplePin.GPIO_PinConfig.GPIO_PinSpeed =         GPIO_OSPEED_MEDIUM;
	BlinkySimplePin.GPIO_PinConfig.GPIO_PinAltFunMode =    AF0;

     // Cargando la configuración del Led de Blinky
	 GPIO_Config(&BlinkySimplePin);

	 //	GPIO_WritePin(&BlinkySimplePin, SET);
	 handlerTimer2.ptrTIMx = TIM2;
	 handlerTimer2.TIMx_Config.TIMx_mode            = BTIMER_MODE_UP; // CUENTA HACIA ARRIBA
	 handlerTimer2.TIMx_Config.TIMx_speed           = BTIMER_SPEED_100us; // La velocidad
	 handlerTimer2.TIMx_Config.TIMx_period          = 2500;
	 handlerTimer2.TIMx_Config.TIMx_interruptEnable = 1; // Activando la configuración, aunque todavia
	 //no estamos aputando a ese registro

	/* Cargando la configuración del timer 2*/
	BasicTimer_Config(&handlerTimer2);
/******************************************************************************************************************************/
	/*Este es el SEGUNDO punto -->> El sistema debe enviar cada 250ms un carácter por el puerto serial de su elección*/

	// Se elige el GPIOB  y el USART 1 ya que estan en el mismo Bus

	/* Inicialización de la configuración para el puerto GPIO serial*/
	handlerTx.pGPIOx= GPIOB;
	// Configuración del usuario para el pin
	handlerTx.GPIO_PinConfig.GPIO_PinNumber      =   PIN_6;
	handlerTx.GPIO_PinConfig.GPIO_PinMode        =   GPIO_MODE_ALTFN; // Función para la comunicación serian
	handlerTx.GPIO_PinConfig.GPIO_PinOPType      =   GPIO_OTYPE_PUSHPULL;
	handlerTx.GPIO_PinConfig.GPIO_PinPuPdControl =   GPIO_PUPDR_NOTHING;
	handlerTx.GPIO_PinConfig.GPIO_PinSpeed       =   GPIO_OSPEED_MEDIUM;
	handlerTx.GPIO_PinConfig.GPIO_PinAltFunMode  =   AF7; // Se eligio la Función alternativa 7

	/*Cargando la configuración del puerto GPIO serial */
	GPIO_Config(&handlerTx);

	/* Inicialización de la configuración para el USART1*/
	handlerUsar.ptrUSARTx = USART1 ;
	// Configuración del usuario para el pin -->> Paridad actividad en modo Even
	handlerUsar.USART_Config.USART_mode     =   USART_MODE_TX;       // En modo de trasmisión
	handlerUsar.USART_Config.USART_baudrate =   USART_BAUDRATE_19200;
	handlerUsar.USART_Config.USART_datasize =   USART_DATASIZE_9BIT; // Se tiene paridad por lo que se asigna ese tamaño
	handlerUsar.USART_Config.USART_parity   =   USART_PARITY_EVEN;   // Paridad en even
	handlerUsar.USART_Config.USART_stopbits =   USART_STOPBIT_1;

	/*Cargando la configuración del Usar para la comunicación serial */
	USART_Config(&handlerUsar);

/********************************************************************************/
    //Para el punto 3 se requiere presionar el Usar_Button, por lo cual se configura
	//en este espacio

	/* Inicialización de la configuración para Usar_Button del micro*/
	UsarButton.pGPIOx = GPIOC;

	//Configuración de usuario para el Usar_Button
	UsarButton.GPIO_PinConfig.GPIO_PinNumber      =  PIN_13;
	UsarButton.GPIO_PinConfig.GPIO_PinMode        =  GPIO_MODE_IN;
	UsarButton.GPIO_PinConfig.GPIO_PinOPType      =  GPIO_OTYPE_PUSHPULL;
	UsarButton.GPIO_PinConfig.GPIO_PinPuPdControl =  GPIO_PUPDR_PULLUP;
	UsarButton.GPIO_PinConfig.GPIO_PinSpeed       =  GPIO_OSPEED_HIGH;
	UsarButton.GPIO_PinConfig.GPIO_PinAltFunMode  =  AF0;

	/*Cargando la configuración para el Usar_Button */
	GPIO_Config(&UsarButton);

	while(1){

		/*
		 * Necesitamos enviar cada 250 ms, por lo que se asigno una bandera llamada  Bandera y Bandera_Normal
		 * asi que cada que suceda el evento asignado la bandera se levanta, tendra el valor asignado en el Call
		 * y se desarrollara, se reinicia el valor a cero y vuelve a preguntar cual desarrollar (evento 1 o 2) y
		 *  asi ciclicamente. Con esto se desarrolla el punto 3
		 */
           // Tenemos presionado el botón
			if(GPIO_ReadPin(&UsarButton) == (RESET)){
                //Nuestra bandera dara un afirmativo
				if(Bandera){
                    //Escribimos el valor de variable
					writeChar(&handlerUsar,variable);
                    // Sumamos una unidad a variable
					variable ++;
                    // Se reinicia el valor de la bandera para volver a estar lista para otro evento
					Bandera = 0;
				}
			  }

			else{
				 //
				 if(Bandera_Normal){
                     // Si no se presiona el boton, la bandera_normal se activa y escribe el caracter
					 writeChar(&handlerUsar,'m');
					 //Se reinicia el valor de la bandera para volver a estar lista para otro evento
					 Bandera_Normal = 0;
				 }
               }
			}
	return 0;
}
/* CallBack del timer*/

void BasicTimer2_CallBack(void){

    //Nuestro CallBack ocurre a la velocidad asignada en la configuración del usuario,
	//En nuestro caso 250 ms

	BlinkySimple = !BlinkySimple; // Activación de Blinky

	if(BlinkySimple){

		GPIO_WritePin(&BlinkySimplePin, SET); // Desactiva

	}else{

		GPIO_WritePin(&BlinkySimplePin, RESET); // Activa
	}

	// Asignaciones a comparar con el condicional del if para cada evento
	Bandera = 1;
	Bandera_Normal = 1;

    }



