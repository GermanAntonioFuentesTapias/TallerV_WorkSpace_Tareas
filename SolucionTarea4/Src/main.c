/**
 ******************************************************************************
 * @file           : main.c
 * @author         : German Antonio Fuentes Tapias
 * @Mail           : gafuentest@unal.edu.co
 * @Resum          : Elaboración de la tarea 4 Taller V
 ******************************************************************************
 * 1. Implementación del LED de estado (obligatorio). 0 puntos.
 2. Implementar adecuadamente una interrupción del EXTI driver, de forma que reciba la señal Clock del encoder
 y lance las interrupciones debida a un cambio de estado. La selección del flanco se deja a criterio del estudiante. 15 puntos.
 3.Identificación de la dirección de giro del encoder, utilizando la interrupción generada por el pin Clock y la lectura del pin Data.
 4 .Controlar los display de 7 segmentos de forma que cada movimiento del encoder en dirección de las manecillas del reloj (CW) aumenta
 en 1 el número presentado en el display (hasta llegar a 50), y cada movimiento del encoder en contra de las manecillas del reloj (CCW)
  decrementa en 1 el valor presentado en los display de 7 segmentos (hasta llegar a cero). 10 puntos.
 5. Imprimir un mensaje cada vez que la posición del encoder cambia (aumenta o disminuye),
 en el cual se debe presentar hacia donde fue el cambio (CW o CCW) y el valor del contador
 (que debe ser igual al valor presentado en los display de 7 segmentos). 10 puntos.
 6. Imprimir un mensaje cualquiera, cada vez que se presione el botón. En el video del ejemplo,
este botón era utilizado para cambiar la frecuencia de refresco del display de 7 segmentos.
 Por favor solo imprima un mensaje, el que usted desee. 5 puntos.
 ******************************************************************************
 */

/* Inc necesario para la realización de la tarea */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "ExtiDriver.h"


/* Variables necesarias para la activación de banderas,blinky y sumatoria al ser presionado el boton*/

uint8_t variable               = 0; //Caracter encargado de hacer una adición al momento de ser pulsado el botón
uint8_t variable1          = 0;
int BanderaUnidad          = 1; //Bandera encargada de mostrar cuando se esta presionando USAR_BUTTON
int BanderaDecena          = 0; //Bandera encargada de mostrar cuando no esta presionando el USAR_BUTTON
uint8_t BlinkySimple       = 0; //Asignación a el blinky de led de estado
uint8_t BanderaOperacion   = 0;

/* La variables tendran los diferentes tipos */

BasicTimer_Handler_t   handlerTimer2      = {0}; // Handler para el timer 2
BasicTimer_Handler_t   handlerTimer3      = {0}; // Handler para el timer 3
GPIO_Handler_t         BlinkySimplePin    = {0}; // Handler para el BlinkySimple
GPIO_Handler_t         handlerGPIOBut     = {0}; // Handler para el handlerGPIOBut
GPIO_Handler_t         handlerGPIOClock   = {0};
GPIO_Handler_t         handlerGPIOData    = {0};
GPIO_Handler_t         handlerTx          = {0};
GPIO_Handler_t         handlerRx          = {0};// Handler para la transmisión serial bit a bit
USART_Handler_t        handlerUsar        = {0}; // Handler para el Usar1

/* Interrupciones externas */

EXTI_Config_t          handlerExtiButton  = {0}; // Configuración para el exti
EXTI_Config_t          handlerClock       = {0}; // Configuración Clock
EXTI_Config_t          handlerData        = {0}; // Configuración Data

/* Segmetos de 7 Segmentos */

GPIO_Handler_t         handlerSegmentoA   = {0};
GPIO_Handler_t         handlerSegmentoB   = {0};
GPIO_Handler_t         handlerSegmentoC   = {0};
GPIO_Handler_t         handlerSegmentoD   = {0};
GPIO_Handler_t         handlerSegmentoE   = {0};
GPIO_Handler_t         handlerSegmentoF   = {0};
GPIO_Handler_t         handlerSegmentoG   = {0};
GPIO_Handler_t         UsarButton         = {0};
GPIO_Handler_t         TransistorDecenas  = {0};
GPIO_Handler_t         TransistorUnidades = {0};


//Variables para boton
uint8_t rxData       =  0;
char bufferData[64] =  {0};
char greetingMsg    [] = "No se puede restar más \n";
char greetingMsgBu  [] = "Por muy dificil que sea el problema, tiene solucion \n";
bool adcIsComplete 	 = false;
uint16_t adcData = 0;

uint8_t Code   = 0;
uint8_t DataEnconder = 0;
uint8_t ClockEnconder  = 0;

//uint8_t NumerosSwitch = 0;

//Variables de los numeros

uint8_t Decenas = 0;
uint8_t Unidades = 0;
uint8_t LoadDisplay = 0;
/* Cabeceras de funciones */
void initSystem (void);

void DefNumerosPrueba(uint8_t NumerosSwitch);
/* Creación del programa principal a utilizar en la tarea*/

int main(void){


	//Pasos para el desarrollo -->  Tengo decenas y unidades como variables y una bandera para ambas

	//Hago el primer condicional if la bandera de switch esta prendida o sea entonces sigo, entro a verificar

	//cual bandera tengo, tengo bandera de unidades y de centena

initSystem();

//	DefNumerosPrueba(NumerosSwitch);



	while(1){



		  if(BanderaOperacion){

//		      	if(GPIO_ReadPin(&handlerGPIOClock) == RESET && (GPIO_ReadPin(&handlerGPIOData))  == RESET){
             	if(GPIO_ReadPin(&handlerGPIOClock) == RESET){

             		 if (variable == 50){

						 variable = 50;

					 } else {

						 variable ++;
					 }


//		      		variable ++;

		      if( variable < 51){

				 sprintf(bufferData, "Enconder CW, pasos = %u \n\r", (variable));
				 writeMsg(&handlerUsar, bufferData);

				 	} else {
				 		variable = 0;

						sprintf(bufferData, "Enconder CW, pasos = %u \n\r", (variable));
						 writeMsg(&handlerUsar, bufferData);

				 	}

		      	}
//
	//	      	else if ((GPIO_ReadPin(&handlerGPIOClock) == SET && (GPIO_ReadPin(&handlerGPIOData))  == SET)){

	      	 if ((GPIO_ReadPin(&handlerGPIOClock) == SET )){

		      		 if (variable ==0){

		      			 variable =0;

		      		 } else {

		      			 variable --;
		      		 }

		      		sprintf(bufferData, "Enconder CCW, pasos = %u \n\r", (variable));
		      		writeMsg(&handlerUsar, bufferData);

			 }

		      	BanderaOperacion = 0;
			}

		/* Escritura de las variables con sus estados */

		GPIO_WritePin(&BlinkySimplePin, BlinkySimple);

		GPIO_WritePin(&TransistorUnidades, BanderaUnidad);

		GPIO_WritePin(&TransistorDecenas, BanderaDecena);


     /* Operación matematica para el proceso */
			if(LoadDisplay){

				if(variable < 10 ){

					Unidades = variable % 10 ;

					Decenas = 0;
				}

				else{

					Decenas = variable / 10;

					Unidades = (variable - Decenas* 10) % 10;

				}
		if(BanderaUnidad){

			DefNumerosPrueba(Unidades);
			GPIO_WritePin(&TransistorDecenas, SET);
			GPIO_WritePin(&TransistorUnidades, RESET);
		}

		else{

				DefNumerosPrueba(Decenas);
				GPIO_WritePin(&TransistorDecenas, RESET);
				GPIO_WritePin(&TransistorUnidades, SET);

			//prendo
		 }

		LoadDisplay = 0;


		}



      /* Codigo para enviar el mensaje cuando el boton es oprimido */

      if (Code){

      			writeMsg(&handlerUsar, greetingMsgBu);

      			Code = 0;

      					}

      }

} // Cierra el main

void initSystem(void){


    /* Led de estado */

	BlinkySimplePin.pGPIOx= GPIOA; // Selección del GPIOA

	/*Configuración del Led de Blinky*/
	BlinkySimplePin.GPIO_PinConfig.GPIO_PinNumber         = PIN_5;
	BlinkySimplePin.GPIO_PinConfig.GPIO_PinMode           = GPIO_MODE_OUT;
	BlinkySimplePin.GPIO_PinConfig.GPIO_PinOPType         = GPIO_OTYPE_PUSHPULL;
	BlinkySimplePin.GPIO_PinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_NOTHING;
	BlinkySimplePin.GPIO_PinConfig.GPIO_PinSpeed          = GPIO_OSPEED_MEDIUM;
	BlinkySimplePin.GPIO_PinConfig.GPIO_PinAltFunMode     = AF0;

     // Cargando la configuración del Led de Blinky
	 GPIO_Config(&BlinkySimplePin);


	/* Configuración de segmentos */

     /* Segmento A*/
	 handlerSegmentoA.pGPIOx = GPIOB;

	 handlerSegmentoA.GPIO_PinConfig.GPIO_PinNumber      = PIN_2;
	 handlerSegmentoA.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_OUT;
	 handlerSegmentoA.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
	 handlerSegmentoA.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	 handlerSegmentoA.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_MEDIUM;
	 handlerSegmentoA.GPIO_PinConfig.GPIO_PinAltFunMode  = AF0;

     /* Cagando configuración A*/
	 GPIO_Config(&handlerSegmentoA);

     /* Segmento B*/

	 handlerSegmentoB.pGPIOx  =GPIOC;

		 handlerSegmentoB.GPIO_PinConfig.GPIO_PinNumber = PIN_4;
		 handlerSegmentoB.GPIO_PinConfig.GPIO_PinMode   = GPIO_MODE_OUT;
		 handlerSegmentoB.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
		 handlerSegmentoB.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
		 handlerSegmentoB.GPIO_PinConfig.GPIO_PinSpeed  = GPIO_OSPEED_MEDIUM;
		 handlerSegmentoB.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;

	/* Cagando configuración B*/
	 GPIO_Config(&handlerSegmentoB);

     /* Segmento C*/

	 handlerSegmentoC.pGPIOx  =GPIOC;

		 handlerSegmentoC.GPIO_PinConfig.GPIO_PinNumber      = PIN_1;
		 handlerSegmentoC.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_OUT;
		 handlerSegmentoC.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
		 handlerSegmentoC.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		 handlerSegmentoC.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_MEDIUM;
		 handlerSegmentoC.GPIO_PinConfig.GPIO_PinAltFunMode  = AF0;

	 /* Cagando configuración C*/
		 GPIO_Config(&handlerSegmentoC);

	   /* Segmento D*/

		 handlerSegmentoD.pGPIOx  =GPIOC;

		 handlerSegmentoD.GPIO_PinConfig.GPIO_PinNumber        = PIN_3;
		 handlerSegmentoD.GPIO_PinConfig.GPIO_PinMode          = GPIO_MODE_OUT;
		 handlerSegmentoD.GPIO_PinConfig.GPIO_PinOPType        = GPIO_OTYPE_PUSHPULL;
		 handlerSegmentoD.GPIO_PinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;
		 handlerSegmentoD.GPIO_PinConfig.GPIO_PinSpeed         = GPIO_OSPEED_MEDIUM;
		 handlerSegmentoD.GPIO_PinConfig.GPIO_PinAltFunMode    = AF0;

	 /* Cagando configuración D*/
		 GPIO_Config(&handlerSegmentoD);

	    /* Segmento E*/

		 handlerSegmentoE.pGPIOx  =GPIOC;

		 handlerSegmentoE.GPIO_PinConfig.GPIO_PinNumber       = PIN_2;
		 handlerSegmentoE.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		 handlerSegmentoE.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
		 handlerSegmentoE.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
		 handlerSegmentoE.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
		 handlerSegmentoE.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;

		/* Cagando configuración E*/
		GPIO_Config(&handlerSegmentoE);

	     /* Segmento F*/

		 handlerSegmentoF.pGPIOx  =GPIOB;

		 handlerSegmentoF.GPIO_PinConfig.GPIO_PinNumber        = PIN_13;
		 handlerSegmentoF.GPIO_PinConfig.GPIO_PinMode          = GPIO_MODE_OUT;
		 handlerSegmentoF.GPIO_PinConfig.GPIO_PinOPType        = GPIO_OTYPE_PUSHPULL;
		 handlerSegmentoF.GPIO_PinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;
		 handlerSegmentoF.GPIO_PinConfig.GPIO_PinSpeed         = GPIO_OSPEED_MEDIUM;
		 handlerSegmentoF.GPIO_PinConfig.GPIO_PinAltFunMode    = AF0;

		 /* Cagando configuración F*/
	      GPIO_Config(&handlerSegmentoF);



	      /* Segmento G*/
	     handlerSegmentoG.pGPIOx  =GPIOC;

		 handlerSegmentoG.GPIO_PinConfig.GPIO_PinNumber        = PIN_0;
		 handlerSegmentoG.GPIO_PinConfig.GPIO_PinMode          = GPIO_MODE_OUT;
		 handlerSegmentoG.GPIO_PinConfig.GPIO_PinOPType        = GPIO_OTYPE_PUSHPULL;
		 handlerSegmentoG.GPIO_PinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;
		 handlerSegmentoG.GPIO_PinConfig.GPIO_PinSpeed         = GPIO_OSPEED_MEDIUM;
		 handlerSegmentoG.GPIO_PinConfig.GPIO_PinAltFunMode    = AF0;

		 /* Cagando configuración G*/
	     GPIO_Config(&handlerSegmentoG);

// Salidas de los transistores


	     TransistorDecenas.pGPIOx  =GPIOA;

		 TransistorDecenas.GPIO_PinConfig.GPIO_PinNumber        = PIN_1;
		 TransistorDecenas.GPIO_PinConfig.GPIO_PinMode          = GPIO_MODE_OUT;
		 TransistorDecenas.GPIO_PinConfig.GPIO_PinOPType        = GPIO_OTYPE_PUSHPULL;
		 TransistorDecenas.GPIO_PinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;
		 TransistorDecenas.GPIO_PinConfig.GPIO_PinSpeed         = GPIO_OSPEED_MEDIUM;
		 TransistorDecenas.GPIO_PinConfig.GPIO_PinAltFunMode    = AF0;


	     GPIO_Config(&TransistorDecenas);

	     TransistorUnidades.pGPIOx  =GPIOB;

		 TransistorUnidades.GPIO_PinConfig.GPIO_PinNumber       = PIN_5;
		 TransistorUnidades.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
		 TransistorUnidades.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
		 TransistorUnidades.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
		 TransistorUnidades.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
		 TransistorUnidades.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;


		 GPIO_Config(&TransistorUnidades);
     // Configuración para el Blinky
	 handlerTimer2.ptrTIMx = TIM2;
	 handlerTimer2.TIMx_Config.TIMx_mode            = BTIMER_MODE_UP; // CUENTA HACIA ARRIBA
	 handlerTimer2.TIMx_Config.TIMx_speed           = BTIMER_SPEED_100us; // La velocidad
	 handlerTimer2.TIMx_Config.TIMx_period          = 2500;
	 handlerTimer2.TIMx_Config.TIMx_interruptEnable = 1; // Activando la configuración, aunque todavia
	 //no estamos aputando a ese registro

	 handlerTimer3.ptrTIMx = TIM3;
	 handlerTimer3.TIMx_Config.TIMx_mode            = BTIMER_MODE_UP;
	 handlerTimer3.TIMx_Config.TIMx_speed           = BTIMER_SPEED_1ms;
	 handlerTimer3.TIMx_Config.TIMx_period          = 10;
	 handlerTimer3.TIMx_Config.TIMx_interruptEnable = 1;


	/* Cargando la configuración del timer 2 y 3*/
	BasicTimer_Config(&handlerTimer2);
	BasicTimer_Config(&handlerTimer3);

/******************************************************************************************************************************/
	/*Este es el punto -->> */

	// Se elige el GPIOB  y el USART 1 ya que estan en el mismo Bus

	/* Inicialización de la configuración para el puerto GPIO serial*/
	/* Handler para el PIN A2 */
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
		handlerRx.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;					// Función alternativa USART1

		/* Se carga la configuración */
		GPIO_Config(&handlerRx);

	/* Inicialización de la configuración para el USART1*/
	handlerUsar.ptrUSARTx = USART2 ;
	// Configuración del usuario para el pin -->> Paridad activa en modo Even
	handlerUsar.USART_Config.USART_mode     =   USART_MODE_RXTX;       // En modo de transmisión
	handlerUsar.USART_Config.USART_baudrate =   USART_BAUDRATE_115200;
	handlerUsar.USART_Config.USART_datasize =   USART_DATASIZE_8BIT; // Se tiene paridad por lo que se asigna ese tamaño
	handlerUsar.USART_Config.USART_parity   =   USART_PARITY_NONE;   // Paridad en even
	handlerUsar.USART_Config.USART_stopbits =   USART_STOPBIT_1;

	/*Cargando la configuración del Usar para la comunicación serial */
	USART_Config(&handlerUsar);

/********************************************************************************/

	// Configuración del Exti

    //Se requiere presionar el Boton para cambiar estados, por lo cual se configura
	/* Inicialización de la configuración del Exti*/
	handlerGPIOBut .pGPIOx = GPIOC;

	//Configuración de usuario para el Usar_Button
	handlerGPIOBut .GPIO_PinConfig.GPIO_PinNumber      =  PIN_7;
	handlerGPIOBut .GPIO_PinConfig.GPIO_PinMode        =  GPIO_MODE_IN;
	handlerGPIOBut .GPIO_PinConfig.GPIO_PinOPType      =  GPIO_OTYPE_PUSHPULL;
	handlerGPIOBut .GPIO_PinConfig.GPIO_PinPuPdControl =  GPIO_PUPDR_PULLUP;
	handlerGPIOBut .GPIO_PinConfig.GPIO_PinSpeed       =  GPIO_OSPEED_HIGH;
	handlerGPIOBut .GPIO_PinConfig.GPIO_PinAltFunMode  =  AF0;

	/*Cargando la configuración para el Usar_Button */
	GPIO_Config(&handlerGPIOBut );


	handlerGPIOClock.pGPIOx = GPIOB;

	handlerGPIOClock.GPIO_PinConfig.GPIO_PinNumber      = PIN_3;
	handlerGPIOClock.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_IN;
	handlerGPIOClock.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
	handlerGPIOClock.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerGPIOClock.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_HIGH;
	handlerGPIOClock.GPIO_PinConfig.GPIO_PinAltFunMode  = AF0;

	GPIO_Config(&handlerGPIOClock);


	handlerGPIOData.pGPIOx = GPIOB;

	handlerGPIOData.GPIO_PinConfig.GPIO_PinNumber      = PIN_1;
	handlerGPIOData.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_IN;
	handlerGPIOData.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
	handlerGPIOData.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerGPIOData.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_HIGH;
	handlerGPIOData.GPIO_PinConfig.GPIO_PinAltFunMode  = AF0;

	GPIO_Config(&handlerGPIOData);

    /* Clock */

	handlerClock.pGPIOHandler = &handlerGPIOClock ;
	handlerClock.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;

	extInt_Config(&handlerClock);

	/* Button Externo Coder */

	handlerExtiButton.pGPIOHandler = &handlerGPIOBut ;
	handlerExtiButton.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;


	extInt_Config(&handlerExtiButton);

	/*  Data */

	handlerData.pGPIOHandler = &handlerGPIOData;

	handlerData.edgeType     = EXTERNAL_INTERRUPT_FALLING_EDGE;

	extInt_Config(&handlerData);


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

}
/* CallBack del timer 2*/

void BasicTimer2_CallBack(void){

    //Nuestro CallBack ocurre a la velocidad asignada en la configuración del usuario,
	//En nuestro caso 250 ms

	BlinkySimple = !BlinkySimple; // Activación de Blinky

	if(BlinkySimple){

		GPIO_WritePin(&BlinkySimplePin, SET); // Activa

	}else{

		GPIO_WritePin(&BlinkySimplePin, RESET); // Desactiva
	}
    }

void BasicTimer3_CallBack(void){

//	if(&handlerGPIOData == SET & &handlerGPIOClock == RESET){
//
//		NumerosSwitch ++;
//	}
//
//	else if(&handlerGPIOData == RESET & &handlerGPIOClock == RESET){
//
//		NumerosSwitch --;
	//}
	BanderaDecena ^= 1;

	BanderaUnidad ^= 1;

	LoadDisplay   = 1;

}
void USART2Rx_CallBack(void){
	rxData = getRxData();
}

void callback_extInt7(void){

	Code = 1;

}

void callback_extInt1(void){

	BanderaOperacion = 1;
//
//	if(GPIO_ReadPin(&handlerGPIOClock) == RESET && (GPIO_ReadPin(&handlerGPIOData))  == RESET){
//
//					sprintf(bufferData, "CW = %u \n\r", (variable));
//					writeMsg(&handlerUsar, bufferData);
//
//
//
//		variable ++;
//	}
//
//	else if ((GPIO_ReadPin(&handlerGPIOClock) == SET && (GPIO_ReadPin(&handlerGPIOData))  == RESET)){
//
//		sprintf(bufferData, "CWW = %u \n\r", (variable));
//		writeMsg(&handlerUsar, bufferData);
//
//	variable --;
//}
}
//
//void callback_extInt5(void){
////
////	ClockEnconder =  1;
//
////	variable ++;
//}

void DefNumerosPrueba(uint8_t NumerosSwitch){

	switch(NumerosSwitch){

	case 0:{

		GPIO_WritePin(&handlerSegmentoA, RESET);
		GPIO_WritePin(&handlerSegmentoB, RESET);
		GPIO_WritePin(&handlerSegmentoC, RESET);
		GPIO_WritePin(&handlerSegmentoD, RESET);
		GPIO_WritePin(&handlerSegmentoE, RESET);
		GPIO_WritePin(&handlerSegmentoF, RESET);
		GPIO_WritePin(&handlerSegmentoG, SET);

		break;

	}

	case 1: {

		GPIO_WritePin(&handlerSegmentoA, SET);
		GPIO_WritePin(&handlerSegmentoB, RESET);
		GPIO_WritePin(&handlerSegmentoC, RESET);
		GPIO_WritePin(&handlerSegmentoD, SET);
		GPIO_WritePin(&handlerSegmentoE, SET);
		GPIO_WritePin(&handlerSegmentoF, SET);
		GPIO_WritePin(&handlerSegmentoG, SET);


		break;

	}

	case 2: {

		GPIO_WritePin(&handlerSegmentoA, RESET);
		GPIO_WritePin(&handlerSegmentoB, RESET);
		GPIO_WritePin(&handlerSegmentoC, SET);
		GPIO_WritePin(&handlerSegmentoD, RESET);
		GPIO_WritePin(&handlerSegmentoE, RESET);
		GPIO_WritePin(&handlerSegmentoF, SET);
		GPIO_WritePin(&handlerSegmentoG, RESET);

		break;

	}

	case 3: {

		GPIO_WritePin(&handlerSegmentoA, RESET);
		GPIO_WritePin(&handlerSegmentoB, RESET);
		GPIO_WritePin(&handlerSegmentoC, RESET);
		GPIO_WritePin(&handlerSegmentoD, RESET);
		GPIO_WritePin(&handlerSegmentoE, SET);
		GPIO_WritePin(&handlerSegmentoF, SET);
		GPIO_WritePin(&handlerSegmentoG, RESET);

		break;
	}

	case 4: {

		GPIO_WritePin(&handlerSegmentoA, SET);
		GPIO_WritePin(&handlerSegmentoB, RESET);
		GPIO_WritePin(&handlerSegmentoC, RESET);
		GPIO_WritePin(&handlerSegmentoD, SET);
		GPIO_WritePin(&handlerSegmentoE, SET);
		GPIO_WritePin(&handlerSegmentoF, RESET);
		GPIO_WritePin(&handlerSegmentoG, RESET);

		break;
      }

	case 5: {


	    GPIO_WritePin(&handlerSegmentoA, RESET);
		GPIO_WritePin(&handlerSegmentoB, SET);
		GPIO_WritePin(&handlerSegmentoC, RESET);
		GPIO_WritePin(&handlerSegmentoD, RESET);
		GPIO_WritePin(&handlerSegmentoE, SET);
		GPIO_WritePin(&handlerSegmentoF, RESET);
		GPIO_WritePin(&handlerSegmentoG, RESET);

		break;

	  }

	case 6: {

		GPIO_WritePin(&handlerSegmentoA, RESET);
		GPIO_WritePin(&handlerSegmentoB, SET);
		GPIO_WritePin(&handlerSegmentoC, RESET);
		GPIO_WritePin(&handlerSegmentoD, RESET);
		GPIO_WritePin(&handlerSegmentoE, RESET);
		GPIO_WritePin(&handlerSegmentoF, RESET);
		GPIO_WritePin(&handlerSegmentoG, RESET);

		break;
    }

	case 7: {

		GPIO_WritePin(&handlerSegmentoA, RESET);
		GPIO_WritePin(&handlerSegmentoB, RESET);
		GPIO_WritePin(&handlerSegmentoC, RESET);
		GPIO_WritePin(&handlerSegmentoD, SET);
		GPIO_WritePin(&handlerSegmentoE, SET);
		GPIO_WritePin(&handlerSegmentoF, SET);
		GPIO_WritePin(&handlerSegmentoG, SET);

		break;
	}

	case 8: {

		GPIO_WritePin(&handlerSegmentoA, RESET);
		GPIO_WritePin(&handlerSegmentoB, RESET);
		GPIO_WritePin(&handlerSegmentoC, RESET);
		GPIO_WritePin(&handlerSegmentoD, RESET);
		GPIO_WritePin(&handlerSegmentoE, RESET);
		GPIO_WritePin(&handlerSegmentoF, RESET);
		GPIO_WritePin(&handlerSegmentoG, RESET);

		break;
	}

	case 9: {

		GPIO_WritePin(&handlerSegmentoA, RESET);
		GPIO_WritePin(&handlerSegmentoB, RESET);
		GPIO_WritePin(&handlerSegmentoC, RESET);
		GPIO_WritePin(&handlerSegmentoD, RESET);
		GPIO_WritePin(&handlerSegmentoE, SET);
		GPIO_WritePin(&handlerSegmentoF, RESET);
		GPIO_WritePin(&handlerSegmentoG, RESET);

		break;
	}
//    return  NumeroSwitch;

	default:{


		GPIO_WritePin(&handlerSegmentoA, SET);
		GPIO_WritePin(&handlerSegmentoB, SET);
		GPIO_WritePin(&handlerSegmentoC, SET);
		GPIO_WritePin(&handlerSegmentoD, SET);
		GPIO_WritePin(&handlerSegmentoE, SET);
		GPIO_WritePin(&handlerSegmentoF, SET);
		GPIO_WritePin(&handlerSegmentoG, RESET);

		break;
	}
	}

    }


