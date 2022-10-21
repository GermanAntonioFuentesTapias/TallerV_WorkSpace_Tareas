/**
 ******************************************************************************
 * @file           : main.c
 * @author         : German Antonio Fuentes Tapias
 * @Mail           : gafuentest@unal.edu.co
 * @Resum          : Elaboración de la tarea 4 Taller V
 *
 *
 *  Nota --->>>>>>>> El encoder utilizado se debe realizar dos movimientos para que lea un estado y no hace los dos cambios de una.
 *                   pero si lee el flanco de subida o de bajada
 *
 * Velocidad utilizada --> 115200
 ******************************************************************************
 * 1. Implementación del LED de estado (obligatorio). 0 puntos.
 *
  2. Implementar adecuadamente una interrupción del EXTI driver, de forma que reciba la señal Clock del encoder

  y lance las interrupciones debida a un cambio de estado. La selección del flanco se deja a criterio del estudiante.

  3. Identificación de la dirección de giro del encoder, utilizando la interrupción generada por el pin Clock y la lectura del pin Data.

  4. Controlar los display de 7 segmentos de forma que cada movimiento del encoder en dirección de las manecillas del reloj (CW) aumenta

    en 1 el número presentado en el display (hasta llegar a 50), y cada movimiento del encoder en contra de las manecillas del reloj (CCW)

   decrementa en 1 el valor presentado en los display de 7 segmentos (hasta llegar a cero).

  5. Imprimir un mensaje cada vez que la posición del encoder cambia (aumenta o disminuye),

  en el cual se debe presentar hacia donde fue el cambio (CW o CCW) y el valor del contador

  6. Imprimir un mensaje cualquiera, cada vez que se presione el botón.

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


/* Variables necesarias para la activación de banderas,blinky y sumatoria */

uint8_t variable           = 0; //Caracter encargado de hacer una adición al momento de haber interrupción
int banderaUnidad          = 1; //Bandera encargada de las unidades del 7 segmentos
int banderaDecena          = 0; //Bandera encargada de las decenas del 7 segmentos
uint8_t blinkySimple       = 0; //Asignación a el blinky de led de estado
uint8_t banderaOperacion   = 0; // Bandera que opera al momento de la interrupción externa

// Se crea bandera unidad con valor 1 debido a que se necesita un cambio de estado con respecto a decenas,
// por lo que cuando uno esta encendido, el otro estara apagado, ocurriendo esto rapidamente,esto no es
// percibido por nuestro ojos.

/* La variables tendran los diferentes tipos */

BasicTimer_Handler_t   handlerTimer2      = {0}; // Handler para el timer 2
BasicTimer_Handler_t   handlerTimer3      = {0}; // Handler para el timer 3
GPIO_Handler_t         blinkySimplePin    = {0}; // Handler para el blinkySimple
GPIO_Handler_t         handlerGPIOBut     = {0}; // Handler para el handlerGPIOBut
GPIO_Handler_t         handlerGPIOClock   = {0}; // Handler para el Clock enconder
GPIO_Handler_t         handlerGPIOData    = {0}; // Handler para el Data del enconder
GPIO_Handler_t         handlerTx          = {0}; // Handler para transmisión
GPIO_Handler_t         handlerRx          = {0}; // Handler para la recepción serial bit a bit
USART_Handler_t        handlerUsart       = {0}; // Handler para el Usart

/* Interrupciones externas */

EXTI_Config_t          handlerExtiButton  = {0}; // Configuración para el boton externo
EXTI_Config_t          handlerClock       = {0}; // Configuración Clock
EXTI_Config_t          handlerData        = {0}; // Configuración Data

/* Segmetos de 7 Segmentos y boton de enconder */

GPIO_Handler_t         handlerSegmentoA   = {0};
GPIO_Handler_t         handlerSegmentoB   = {0};
GPIO_Handler_t         handlerSegmentoC   = {0};
GPIO_Handler_t         handlerSegmentoD   = {0};
GPIO_Handler_t         handlerSegmentoE   = {0};
GPIO_Handler_t         handlerSegmentoF   = {0};
GPIO_Handler_t         handlerSegmentoG   = {0};
GPIO_Handler_t         transistorDecenas  = {0};
GPIO_Handler_t         transistorUnidades = {0};


/*Variables para envio de mensaje e interrupción */

char bufferData[64]    =  {0};
char greetingMsgBu[]   = " Millonarios el mas grande :) \n \r";
uint8_t Code           =    0;
uint8_t DataEnconder   =    0;
uint8_t ClockEnconder  =    0;


//Variables de los numeros

uint8_t Decenas = 0;
uint8_t Unidades = 0;
uint8_t loadDisplay = 0;

/* Cabeceras de funciones */

void initSystem (void);                        // Función con configuración del GPIO

void DefNumerosPrueba(uint8_t numerosSwitch); // Función con numeros a visualizar



/* Creación del programa principal a utilizar en la tarea*/

int main(void){


	//Pasos para el desarrollo -->  Tengo decenas y unidades como variables y una bandera para ambas

	//Hago el primer condicional if la bandera de switch esta prendida o sea entonces sigo, entro a verificar

	//cual bandera tengo, tengo bandera de unidades y de centenas con distinta inicialización por lo que

	// encenderan a distinto periodo que se determinara por el Timer 3

initSystem(); // Se llama la función de configuración de pines

	while(1){

		/* Punto 2-6 */

		  /* Implementación de bandera de la interrupción externa */

		  if(banderaOperacion){

			 /* Caso de CW Y CCW del reloj Punto 2 y 3 */

			 // Si se tiene el Clock con valor cero o un Falling se entra a este condicional
			 // El clock nos da la interrupción y el data la información, por eso se llama al 11
			 // y no al 12 en el exti, este seria caso CW

			if(GPIO_ReadPin(&handlerGPIOClock) == RESET){

				/* Si nuestra variable es 50 queda igual y no supera este tope*/
				 if (variable == 50){

					 variable = 50;

				 } else {

			     /* Sino, empieza a sumar unidad por unidad segun el condicional de arriba*/
					 variable ++;
				 }

		  /* Punto 5,imprimir mensaje segun la dirección*/

		  // Si es menor a 51 se imprime el valor en el Coolterm de "Variable"

		  if( variable < 51){

			  // Almacenamiento de las variables en un arreglo para ser visualizadas en la
			  //comunicación serial --> almacenan en bufferData tomando el valor de "variable"
			 sprintf(bufferData, "Enconder CW, pasos = %u \n\r", (variable));

			 writeMsg(&handlerUsart, bufferData);

				}

			}

		// Si se tiene el Clock con valor uno o un Rising se entra a este condicional
		// El clock nos da la interrupción y el data la información, por eso se llama al 11
		// y no al 12 en el exti, este seria caso CW

		/* Caso CCW para la resta*/

		 if ((GPIO_ReadPin(&handlerGPIOClock) == SET )){

			 /* Condicionales */
			 // Si es cero, no baje de ahi, ya que habria desbordamiento

			 if (variable ==0){

				variable =0;

			 } else {

             // Como es distinto de cero, realizara la sustración

				variable --;

			 }

			 // Almacenamiento de las variables en un arreglo para ser visualizadas en la
		     //comunicación serial --> almacenan en bufferData tomando el valor de "variable"

			 sprintf(bufferData, "Enconder CCW, pasos = %u \n\r", (variable));

			 writeMsg(&handlerUsart, bufferData);

		     }

			banderaOperacion = 0; // Se baja la bandera

		    }

		/* Escritura de las variables con sus estados */

		GPIO_WritePin(&blinkySimplePin, blinkySimple);     // Para el led de estado

		GPIO_WritePin(&transistorUnidades, banderaUnidad); // Para la bandera de unidad

		GPIO_WritePin(&transistorDecenas, banderaDecena);  // Para la bandera de decena


     /* Operación matematica para el proceso */

		// Esta es una parte importante, ya que de aqui parten los valores para observar
		// mediante el 7 segmentos de manera externa. De manera interna ve en el Cool
		// el valor de variable que es el mismo valor que representan unidades y decenas

		    /* Condicional que actualiza dato*/

		    //Se estara entrando a esta condición a la velocidad configurada, no obstante si variable
		    // sigue igual no se realizara la operación y se baja la bandera, pero si sucede ocurre
		    // la operación descrita segun el caso

		    /* P U N T O  4*/

			if(loadDisplay){

				/* Caso unidades */

				if(variable < 10 ){
                    // Las unidades van del 0 al 9 por lo cual no esta el 10, decenas se tendra en cero
					// por gusto personal pero podría estar apagada también.

					// Operación modulo
					Unidades = variable % 10 ; // A medida que aumenta variable hace la operación modulo
					                           // dejando el valor del resIDUO y guardando ese valor en unidades
					                           // Que se utilizara más abajo para la visualización

					Decenas = 0;               // Visualización del cero en decenas
				}

				else{

					//Se supero unidades, se necesita el incremento en decenas por lo que se tienen dos operaciones
					// distintas para ambas variables.

					Decenas = variable / 10; // De aqui al dividir por 10 se queda el numero entero, asi que 12/10, nos da 1.2, pero como
					// trabajamos con enteros queda el 1 que seria la decena entre el 10 y el 19

					Unidades = (variable - Decenas* 10) % 10; //  Se esta la propiedad distributiva (a-b) mod c = ( a mod c- b mod c ) mod c
					// asi que con esto en cada proceso se garantizara obtener el valor de unidades deseado estan en cualquier decena.
					// asi que por ejemplo variable vale 12 --> el valor despues del la distribucion y modulo a cada uno es 2. que seria el
					// valor deseado para unidades en ese caso

				}

				/* Banderas para configuriación de encendido */
		if(banderaUnidad){

			/* Caso de unidades, la función trabajara sobre estos numeros */

			DefNumerosPrueba(Unidades); // Llamado de la función de los numeros
			GPIO_WritePin(&transistorDecenas, SET);
			GPIO_WritePin(&transistorUnidades, RESET);
		}

		else{

			/* Caso de decenas, la función trabajara sobre estos numeros */

				DefNumerosPrueba(Decenas); // Llamado de la función de los numeros
				GPIO_WritePin(&transistorDecenas, RESET);
				GPIO_WritePin(&transistorUnidades, SET);
				// En este bloque if-else tienes un pequeño error que hace que se te cree una pequeña sombra en tu display
				// pero lo puedes corregir facilmente apagando ambos display antes de actualizar y luego enciendes el que
				// va a mostrar la info, son solo 2 lineas antes de ejecutar el if()


		 }

		loadDisplay = 0; // Reinicio de bandera de visualización

		}

	  /* P u n t o 6 */

      /* Codigo para enviar el mensaje cuando el boton es oprimido */

	  // Si hay interrupción , la bandera Code se activa
      if (Code){
                // Se busca la configuración del handlerUsart para enviar el mensanje guardado en "greetingMsgBu"
      			writeMsg(&handlerUsart, greetingMsgBu);

      			// Se baja la bandera para otra interrupción
      			Code = 0;

      }
      }

   } // Cierre del main principal

  /******************************************************************************************************************************/

void initSystem(void){

    /* Punto 1/*/

    /* Led de estado */

	blinkySimplePin.pGPIOx= GPIOA; // Selección del GPIOA

	/*Configuración del Led de Blinky*/
	blinkySimplePin.GPIO_PinConfig.GPIO_PinNumber           = PIN_5;
	blinkySimplePin.GPIO_PinConfig.GPIO_PinMode             = GPIO_MODE_OUT;
	blinkySimplePin.GPIO_PinConfig.GPIO_PinOPType           = GPIO_OTYPE_PUSHPULL;
	blinkySimplePin.GPIO_PinConfig.GPIO_PinPuPdControl      = GPIO_PUPDR_NOTHING;
	blinkySimplePin.GPIO_PinConfig.GPIO_PinSpeed            = GPIO_OSPEED_MEDIUM;
	blinkySimplePin.GPIO_PinConfig.GPIO_PinAltFunMode       = AF0;

     // Cargando la configuración del Led de Blinky
	 GPIO_Config(&blinkySimplePin);


	/* Configuración de segmentos */

     /* Segmento A*/
	 handlerSegmentoA.pGPIOx = GPIOB; // Selección del GPIOB

	 handlerSegmentoA.GPIO_PinConfig.GPIO_PinNumber        = PIN_2;
	 handlerSegmentoA.GPIO_PinConfig.GPIO_PinMode          = GPIO_MODE_OUT;
	 handlerSegmentoA.GPIO_PinConfig.GPIO_PinOPType        = GPIO_OTYPE_PUSHPULL;
	 handlerSegmentoA.GPIO_PinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;
	 handlerSegmentoA.GPIO_PinConfig.GPIO_PinSpeed         = GPIO_OSPEED_MEDIUM;
	 handlerSegmentoA.GPIO_PinConfig.GPIO_PinAltFunMode    = AF0;

     /* Cagando configuración A*/
	 GPIO_Config(&handlerSegmentoA);

     /* Segmento B*/

	 handlerSegmentoB.pGPIOx  =GPIOC; // Selección del GPIOC

	 handlerSegmentoB.GPIO_PinConfig.GPIO_PinNumber        = PIN_4;
	 handlerSegmentoB.GPIO_PinConfig.GPIO_PinMode          = GPIO_MODE_OUT;
	 handlerSegmentoB.GPIO_PinConfig.GPIO_PinOPType        = GPIO_OTYPE_PUSHPULL;
	 handlerSegmentoB.GPIO_PinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;
	 handlerSegmentoB.GPIO_PinConfig.GPIO_PinSpeed         = GPIO_OSPEED_MEDIUM;
	 handlerSegmentoB.GPIO_PinConfig.GPIO_PinAltFunMode    = AF0;

	/* Cagando configuración B*/
	 GPIO_Config(&handlerSegmentoB);

     /* Segmento C*/

	 handlerSegmentoC.pGPIOx  =GPIOC;// Selección del GPIOC

	 handlerSegmentoC.GPIO_PinConfig.GPIO_PinNumber        = PIN_1;
	 handlerSegmentoC.GPIO_PinConfig.GPIO_PinMode          = GPIO_MODE_OUT;
	 handlerSegmentoC.GPIO_PinConfig.GPIO_PinOPType        = GPIO_OTYPE_PUSHPULL;
	 handlerSegmentoC.GPIO_PinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;
	 handlerSegmentoC.GPIO_PinConfig.GPIO_PinSpeed         = GPIO_OSPEED_MEDIUM;
	 handlerSegmentoC.GPIO_PinConfig.GPIO_PinAltFunMode    = AF0;

	 /* Cagando configuración C*/
	 GPIO_Config(&handlerSegmentoC);

     /* Segmento D*/

	 handlerSegmentoD.pGPIOx  =GPIOC;// Selección del GPIOC

	 handlerSegmentoD.GPIO_PinConfig.GPIO_PinNumber        = PIN_3;
	 handlerSegmentoD.GPIO_PinConfig.GPIO_PinMode          = GPIO_MODE_OUT;
	 handlerSegmentoD.GPIO_PinConfig.GPIO_PinOPType        = GPIO_OTYPE_PUSHPULL;
	 handlerSegmentoD.GPIO_PinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;
	 handlerSegmentoD.GPIO_PinConfig.GPIO_PinSpeed         = GPIO_OSPEED_MEDIUM;
	 handlerSegmentoD.GPIO_PinConfig.GPIO_PinAltFunMode    = AF0;

	 /* Cagando configuración D*/
	 GPIO_Config(&handlerSegmentoD);

	 /* Segmento E*/

	 handlerSegmentoE.pGPIOx  =GPIOC;// Selección del GPIOC

	 handlerSegmentoE.GPIO_PinConfig.GPIO_PinNumber        = PIN_2;
	 handlerSegmentoE.GPIO_PinConfig.GPIO_PinMode          = GPIO_MODE_OUT;
	 handlerSegmentoE.GPIO_PinConfig.GPIO_PinOPType        = GPIO_OTYPE_PUSHPULL;
	 handlerSegmentoE.GPIO_PinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;
	 handlerSegmentoE.GPIO_PinConfig.GPIO_PinSpeed         = GPIO_OSPEED_MEDIUM;
	 handlerSegmentoE.GPIO_PinConfig.GPIO_PinAltFunMode    = AF0;

	 /* Cagando configuración E*/
	 GPIO_Config(&handlerSegmentoE);

	 /* Segmento F*/

	 handlerSegmentoF.pGPIOx  =GPIOB;// Selección del GPIOB

	 handlerSegmentoF.GPIO_PinConfig.GPIO_PinNumber        = PIN_13;
	 handlerSegmentoF.GPIO_PinConfig.GPIO_PinMode          = GPIO_MODE_OUT;
	 handlerSegmentoF.GPIO_PinConfig.GPIO_PinOPType        = GPIO_OTYPE_PUSHPULL;
	 handlerSegmentoF.GPIO_PinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;
	 handlerSegmentoF.GPIO_PinConfig.GPIO_PinSpeed         = GPIO_OSPEED_MEDIUM;
	 handlerSegmentoF.GPIO_PinConfig.GPIO_PinAltFunMode    = AF0;

	 /* Cagando configuración F*/
	  GPIO_Config(&handlerSegmentoF);

      /* Segmento G*/
	 handlerSegmentoG.pGPIOx  =GPIOC;// Selección del GPIOC

	 handlerSegmentoG.GPIO_PinConfig.GPIO_PinNumber        = PIN_0;
	 handlerSegmentoG.GPIO_PinConfig.GPIO_PinMode          = GPIO_MODE_OUT;
	 handlerSegmentoG.GPIO_PinConfig.GPIO_PinOPType        = GPIO_OTYPE_PUSHPULL;
	 handlerSegmentoG.GPIO_PinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;
	 handlerSegmentoG.GPIO_PinConfig.GPIO_PinSpeed         = GPIO_OSPEED_MEDIUM;
	 handlerSegmentoG.GPIO_PinConfig.GPIO_PinAltFunMode    = AF0;

	 /* Cagando configuración G*/
	 GPIO_Config(&handlerSegmentoG);

     /* Salidas de los transistores del micro */


	 transistorDecenas.pGPIOx  =GPIOA;// Selección del GPIOA

	 transistorDecenas.GPIO_PinConfig.GPIO_PinNumber        = PIN_1;
	 transistorDecenas.GPIO_PinConfig.GPIO_PinMode          = GPIO_MODE_OUT;
	 transistorDecenas.GPIO_PinConfig.GPIO_PinOPType        = GPIO_OTYPE_PUSHPULL;
	 transistorDecenas.GPIO_PinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;
	 transistorDecenas.GPIO_PinConfig.GPIO_PinSpeed         = GPIO_OSPEED_MEDIUM;
	 transistorDecenas.GPIO_PinConfig.GPIO_PinAltFunMode    = AF0;

	 /* Cagando configuración de decenas*/
	 GPIO_Config(&transistorDecenas);

	 transistorUnidades.pGPIOx  =GPIOB; // Selección del GPIOB

	 transistorUnidades.GPIO_PinConfig.GPIO_PinNumber       = PIN_5;
	 transistorUnidades.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	 transistorUnidades.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	 transistorUnidades.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	 transistorUnidades.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	 transistorUnidades.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;

	 /* Cagando configuración de unidades*/
     GPIO_Config(&transistorUnidades);


     /* Configuración para el Blinky */
	 handlerTimer2.ptrTIMx = TIM2;
	 handlerTimer2.TIMx_Config.TIMx_mode                    = BTIMER_MODE_UP;     // Cuenta hacia arriba
	 handlerTimer2.TIMx_Config.TIMx_speed                   = BTIMER_SPEED_100us; // La velocidad
	 handlerTimer2.TIMx_Config.TIMx_period                  = 2500;               //250ms
	 handlerTimer2.TIMx_Config.TIMx_interruptEnable         = 1;                  // Activando la configuración, aunque todavia
	 //no estamos aputando a ese registro

	 /* Configuración para las banderas de unidades y decenas*/
	 handlerTimer3.ptrTIMx = TIM3;
	 handlerTimer3.TIMx_Config.TIMx_mode                    = BTIMER_MODE_UP;   // Cuenta hacia arriba
	 handlerTimer3.TIMx_Config.TIMx_speed                   = BTIMER_SPEED_1ms; // La velocidad
	 handlerTimer3.TIMx_Config.TIMx_period                  = 10;               //10ms
	 handlerTimer3.TIMx_Config.TIMx_interruptEnable         = 1;


	 /* Cargando la configuración del timer 2 y 3*/
	 BasicTimer_Config(&handlerTimer2);
	 BasicTimer_Config(&handlerTimer3);

    /******************************************************************************************************************************/
	/*Esta configuracion es para el punto -->> 5 & 6 */

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
	handlerRx.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;					// Función alternativa USART2

	/* Se carga la configuración */
	GPIO_Config(&handlerRx);

	/* Inicialización de la configuración para el USART2*/

	handlerUsart.ptrUSARTx = USART2 ;
	// Configuración del usuario para el pin -->> Paridad activa en modo Even

	handlerUsart.USART_Config.USART_mode          =  USART_MODE_RXTX;       // En modo de transmisión
	handlerUsart.USART_Config.USART_baudrate      =  USART_BAUDRATE_115200;
	handlerUsart.USART_Config.USART_datasize      =  USART_DATASIZE_8BIT;
	handlerUsart.USART_Config.USART_parity        =  USART_PARITY_NONE;     // Paridad 0
	handlerUsart.USART_Config.USART_stopbits      =  USART_STOPBIT_1;

	/*Cargando la configuración del Usart para la comunicación serial */
	USART_Config(&handlerUsart);

	/******************************************************************************************************************************/

	// Configuración del Exti

    //Se requiere presionar el Boton para cambiar estados, por lo cual se configura
	/* Inicialización de la configuración del Exti*/

	/* Boton del 7 segmentos */
	handlerGPIOBut .pGPIOx = GPIOC;

	//Configuración de usuario para el Usar_Button
	handlerGPIOBut .GPIO_PinConfig.GPIO_PinNumber      =  PIN_7;
	handlerGPIOBut .GPIO_PinConfig.GPIO_PinMode        =  GPIO_MODE_IN;         // MODO ENTRADA
	handlerGPIOBut .GPIO_PinConfig.GPIO_PinOPType      =  GPIO_OTYPE_PUSHPULL;
	handlerGPIOBut .GPIO_PinConfig.GPIO_PinPuPdControl =  GPIO_PUPDR_PULLUP;   //Lectura PULL-UP
	handlerGPIOBut .GPIO_PinConfig.GPIO_PinSpeed       =  GPIO_OSPEED_HIGH;
	handlerGPIOBut .GPIO_PinConfig.GPIO_PinAltFunMode  =  AF0;

	/*Cargando la configuración para el Usar_Button */
	GPIO_Config(&handlerGPIOBut );

    /* Enconder lectura Clock */

	handlerGPIOClock.pGPIOx = GPIOC;

	handlerGPIOClock.GPIO_PinConfig.GPIO_PinNumber      = PIN_12;
	handlerGPIOClock.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_IN;         // MODO ENTRADA
	handlerGPIOClock.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
	handlerGPIOClock.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerGPIOClock.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_HIGH;
	handlerGPIOClock.GPIO_PinConfig.GPIO_PinAltFunMode  = AF0;

	/* Cargando configuración Clock */
	GPIO_Config(&handlerGPIOClock);

	/* Enconder lectura Data*/
	handlerGPIOData.pGPIOx = GPIOC;

	handlerGPIOData.GPIO_PinConfig.GPIO_PinNumber       = PIN_11;
	handlerGPIOData.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_IN;         // MODO ENTRADA
	handlerGPIOData.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerGPIOData.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerGPIOData.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_HIGH;
	handlerGPIOData.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;

	/* Cargando configuración Clock */
	GPIO_Config(&handlerGPIOData);


	/* Configuraciones directas del Exti */

	handlerClock.pGPIOHandler       = &handlerGPIOClock ;
	handlerClock.edgeType           = EXTERNAL_INTERRUPT_RISING_EDGE;  // Flanco de subida

	/* Cargando configuración */

	extInt_Config(&handlerClock);

	/* Button Externo Coder */

	handlerExtiButton.pGPIOHandler  = &handlerGPIOBut ;
	handlerExtiButton.edgeType      = EXTERNAL_INTERRUPT_FALLING_EDGE; // Flanco de bajada

	/* Cargando configuración */

	extInt_Config(&handlerExtiButton);

	/*  Data */

	handlerData.pGPIOHandler        = &handlerGPIOData;
	handlerData.edgeType            = EXTERNAL_INTERRUPT_FALLING_EDGE; // Flanco de bajada

	/* Cargando configuración */

	extInt_Config(&handlerData);

   } // Finalización de Init

  /******************************************************************************************************************************/

/* CallBack del timer 2*/

void BasicTimer2_CallBack(void){

    //Nuestro CallBack ocurre a la velocidad asignada en la configuración del usuario,
	//En nuestro caso 250 ms

	blinkySimple = !blinkySimple; // Activación de Blinky

	if(blinkySimple){

		GPIO_WritePin(&blinkySimplePin, SET); // Activa

	}else{

		GPIO_WritePin(&blinkySimplePin, RESET); // Desactiva
		// Y para qué teniamos la funcion toogle?
	}
    }

/* CallBack del timer 3*/

void BasicTimer3_CallBack(void){

	// La función es crear el cambio de estado en los segmentos del 7 segmentos, al estar conectado
	// decenas y unidades debe alternarse el encendido de estos para lograr tener ambos encendidos
	// con esto se busca generar este efecto a un tiempo deseado por el usuario en la configuración

	banderaDecena ^= 1; // Cambio de estado a velocidad configurada de bandera Decena

	banderaUnidad ^= 1; // Cambio de estado a velocidad configurada de bandera Unidad

	loadDisplay    = 1; // Bandera con la función  de la creación de los distintos numeros del
	                   // 7 segmentos

}

/* CallBack para la comunicación serial */

void USART2Rx_CallBack(void){
}

/* CallBack encargado de la interrupción del boton del enconder */

void callback_extInt7(void){

	Code = 1; // Bandera que da inicio al envio del mensaje mediante comunicación serial

}

/* CallBack de la interrupción del enconder y la lectura del Data*/

void callback_extInt11(void){

	banderaOperacion = 1;  // Bandera que informa la interrupción para ver si es operación de adición o sustracción

}

/* Función con los casos para cada numero que se visualizaran en el 7 segmentos */


void DefNumerosPrueba(uint8_t numerosSwitch){

	switch(numerosSwitch){

	/* Cada case es la representación de un numero, del 0 al 9 */

	//La configuración de estos esta basado en que el 7 segmetos es de anodo común
	// segun esto se requiere un cero --> reset para encender el segmento deseado
	// y un uno --> set para mantener el segmento apagado

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

	default:{
        // Caso de error, se visualizaran dos lineas

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

/* F I N    D E L     P R O G R A M A */


