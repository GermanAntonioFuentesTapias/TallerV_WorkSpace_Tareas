/*
 * GPIOxSlnTarea.c
 *
 *  Created on: 03/09/2022
 *
 *
 *      Author: G e r m a n  A n t o n i o  F u e n t e s  T a p i a s
 *
 *     D E S A R R O L L O    D E   T A R E A   # 2
 * ************************************************
 *El desarrollo de este programa esta con fin de hacer la tarea numero #2, que cuenta con las siguientes preguntas:
 * 1. En el desarrollo mostrado en el video del Driver para el puerto GPIOx, la función GPIO_ReadPin() tiene un error,
 * el cuál no nos permite obtener el valor real del PinX que estamos leyendo. Explique sus respuestas en un comentario de bloque.
 * a)¿Cúal es este error?
 * b)¿Cómo lo puede solucionar?
 * c) Corrija el código y pruebe que efectivamente su propuesta de solución funciona.
 *
 *2. Cree una nueva función llamada GPIOxTooglePin, la cual reciba como parámetro solamente un elemento del tipo GPIO_Handler_t y
 * su función sea cambiar el estado de un PinX (seleccionado en el handler y debidamente configurado). Cambiar el
 * estado significa: sí está encendido que pase a apagado, si está apagado que pase a encendido. Hint: La operación XOR puede ser útil.
 *
 *3. Utilice el “USER_BUTTON” (azul) con la nueva función GPIOxToogle, de forma que el USER_LED (Led verde)
 * cambie de estado cada vez que pulsa USER_BUTTON.
 *
 *4. Agregue un botón externo al PIN_C6, configurado en modo Pull_down. La acción de este botón debe hacer que al pulsar este botón, se enciendan tres Leds (PC10, PC11 y PC12) de forma simultánea y luego de 5 segundos se apaguen en cascada:
 * a) Se apaga primero PC12.
 * b)Dos segundos después se apaga PC11.
 * c) Un segundo después se apaga PC10.
 *
 * HAL -> Hardware Abstraction Layer
 * Los puertos específicos que se usaran mas adelante son:
 *   - La velocidad a la que puede responder
 *   - Tipo de entrada (pull up, pull down, open drain (flotante))
 *   - Tipo de salida (push-pullm open drain...)
 *   - Entrada análoga
 *
 *   Debemos definir entonces todos los registros que manejan el periférico GPIOx y luego
 *   crear algunas funciones para utilizar adecuadamente el equipo.
 *****************************************************************************************

 */

#include <stdint.h>

#include "stm32f4xx_hal.h" // Creada con el fin de tener la ubicación de cada pin, como su función y sus funciones alternativas
#include "GPIOxDriver.h"   //Elementos del hadware driver

 /* Solución Punto 1
* a)
* Principalmente el error seria por la falta de una linea en el codigo, ya que siempre es necesario
* hacer limpieza antes de sobreescribir en una posición, se esta sobre escribiendo o guardando en ese espacio de memoria
* asi que si se realizaba una primera operación sobre ese pin quedaba guardando esa posición, y al hacer otra puede quedar
* como se encontraba, ya que no se sabe que se tiene en ese lugar y modificarlo como esta, puede cambiar el estado, dejarlo igual
* o alterarlo de alguna manera que no se quiere. El registro IDR está desplazado a el lado derecho tantas veces como la magnitud
* de la ubicación del pin especifico, pero no las otras posiciones que se desconocen.
*
* b)
* La solución sera crear una linea en particular, con el fin de  limpiar las posiciones de la izquierda del pin de interes
* Esta solución seria agregar el siguiente comando:
* pinValue &= 0b1;  y se pondra obtener un mejor desarrollo del cogido
*
* c) La correción del codigo se probo con los ejercicios de escritura del GPIO, por lo tal se uso el siguiente cogido
*
* 1: Sin el codigo, se hacia alguno de los 3 estados que pedian y se desarrolla, no obstante al hacer otro no ocurria nada
* se mantenia en ese estado, sin sobreescribir en ningun otro punto. Ya después de corregirlo se observa que se puede
* variar entre los 3 estados que pedian y todo ocurre sin problemas.
*
* N O T A : el codigo usado fue el siguiente: (omitiendo el main y el puerto del led del micro)
*
*     //Coonfigurando otro Pin para externo
        GPIO_Handler_t handlerUserLedExtern = {0};


        //configurando el pin
         * Se configuro el puerto del pin
           handlerUserLedExtern.pGPIOx = GPIOB;
           handlerUserLedExtern.GPIO_PinConfig.GPIO_PinNumber              = PIN_9;  // Numero de puerto
           handlerUserLedExtern.GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_OUT; // Salida // Como salida
           handlerUserLedExtern.GPIO_PinConfig.GPIO_PinOPType              = GPIO_OTYPE_PUSHPULL; //Corriente // Necesita prender el led
           handlerUserLedExtern.GPIO_PinConfig.GPIO_PinPuPdControl         = GPIO_PUPDR_NOTHING; // La mayoria de veces que se tiene salida va en nothing
           handlerUserLedExtern.GPIO_PinConfig.GPIO_PinSpeed               = GPIO_OSPEED_MEDIUM; // Velocidad siempre igual
           handlerUserLedExtern.GPIO_PinConfig.GPIO_PinAltFunMode          = AF0;              // Funciones que aun no usamos

           // Se cargo la configuración
           GPIO_Config(&handlerUserLedExtern);

      // Configurando pin del Boton
           GPIO_Handler_t handlerUserBExtern = {0};

       // Configurando
           handlerUserBExtern.pGPIOx = GPIOC;
           handlerUserBExtern.GPIO_PinConfig.GPIO_PinNumber              = PIN_6;
           handlerUserBExtern.GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_IN; // Salida
           handlerUserBExtern.GPIO_PinConfig.GPIO_PinOPType              = GPIO_OTYPE_PUSHPULL; //Corriente
           handlerUserBExtern.GPIO_PinConfig.GPIO_PinPuPdControl         = GPIO_PUPDR_PULLUP; // Asi a que es una entrada
           handlerUserBExtern.GPIO_PinConfig.GPIO_PinSpeed               = GPIO_OSPEED_MEDIUM; // Velocidad
           handlerUserBExtern.GPIO_PinConfig.GPIO_PinAltFunMode          = AF0;              // Funciones que aun no usamos

       //Se cargo el pin
           GPIO_Config(&handlerUserBExtern);

           //toca llamar la función que me lee el estado del pin
            *
        while(1){
        	if(GPIO_ReadPin(&handlerUserBExtern) == 1 &&  GPIO_ReadPin(&handlerUserButtom) == 0 ){
        	for(unsigned int i = 0; i < 500000; i++){
        	    		NOP();
            }

        	GPIO_WritePin(&handlerUserLedPin, SET); // Prendo este pin
        	GPIO_WritePin(&handlerUserLedExtern, RESET); // Apago este pin

        	for (unsigned int i = 0; i < 500000; i++){
        		NOP();
        	}

        	GPIO_WritePin(&handlerUserLedPin, RESET);
        	GPIO_WritePin(&handlerUserLedExtern, SET);
        	}
        	//Punto B //Si se presiona el botón externo, solo el LED externo debe hacer el blinky, y el userLED debe permanecer encendido.

        	 else if(GPIO_ReadPin(&handlerUserBExtern) == 0  &&  GPIO_ReadPin(&handlerUserButtom) == 1 ){

             GPIO_WritePin(&handlerUserLedPin, SET);  // Prendo este pin

        	 for(unsigned int i = 0; i < 50000; i++){
        						NOP();
			  }

			 GPIO_WritePin(&handlerUserLedExtern, RESET); // Apago este pin

			 for (unsigned int i = 0; i < 50000; i++){
				 NOP();
			 }
			 GPIO_WritePin(&handlerUserLedExtern, SET);

			 }

        	// Si se presionan ambos botones, ambos LEDs deben permanecer apagados.

        	 else if(GPIO_ReadPin(&handlerUserBExtern) == 0  &&  GPIO_ReadPin(&handlerUserButtom) == 0) {




        	 GPIO_WritePin(&handlerUserLedExtern, RESET);
        	 GPIO_WritePin(&handlerUserLedPin, RESET);
        	 }

        	 else{

        	 GPIO_WritePin(&handlerUserLedExtern,SET);
             GPIO_WritePin(&handlerUserLedPin, SET);

        	 }
             }
*/

/* Desarrollo del punto 2  */


// Se procede a crear  una función para que este programa la reconoce, esta estara definida en la parte inferior ( despues del main)

void GPIOxTooglePin(GPIO_Handler_t *pPinHandler ); // Se crea para que se una función global en los While del main


/* Función principal del programa. Es acá donde se ejecuta todo */
int main(void){

	// ***************
	// Definimos el handler para el pin que usaremos que tendra por nombre  USER_PIN y que se configurara


	GPIO_Handler_t handlerUserLedPin = {0};  // Creando un objeto con ese nombre iniciando en cero

	//Deseamos trabajar con el puerto GPIOA por facilidad, no obstante se podria usar otro puerto. En nuestro caso
	// el led interno del micro esta en el PIN 5 como salida, asi que:

    handlerUserLedPin.pGPIOx = GPIOA;
    handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber              = PIN_5;
    handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_OUT; // Salida del Pin que se tiene
    handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType              = GPIO_OTYPE_PUSHPULL; //Corriente pero no se tiene aun entrada
    handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl         = GPIO_PUPDR_NOTHING; // No hay entrada, asi que se mantiene este registro
    handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed               = GPIO_OSPEED_MEDIUM; // Velocidad
    handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode          = AF0;              // Función alternativa aun no se requiere

    // Recordar que es necesario cargar la configuración del PIN especifico

    GPIO_Config(&handlerUserLedPin);  // Este llama y configura al PIN el pin especifico.

    // Ahora necesitamos configurar el boton para el led asi que se debe hacer lo mismo pero con el boton que nos cambiara
    // Los diferentes parametros del led

    // Hacemos que el PIN_A5 quede encedido
    GPIO_WritePin(&handlerUserLedPin, SET); // Se ve prendido porque lo estoy colocando en 1


    // Continuando con el Punto 2, se necesita hacer Handler para el boton
    //  recordar SIEMPRE definirlo, configurarlo y cargarlo
    //Coonfigurando otro Pin
    GPIO_Handler_t handlerButtom = {0};  // Creando un objeto con ese nombre iniciando en cero

    handlerButtom .pGPIOx = GPIOC; //Salida del buton respecto al diagrama
    handlerButtom .GPIO_PinConfig.GPIO_PinNumber              = PIN_13; // Numero del pin
    handlerButtom .GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_IN; // Entrada porque esta mandando informacion al micro con la presión
    handlerButtom .GPIO_PinConfig.GPIO_PinOPType              = GPIO_OTYPE_PUSHPULL; // Normalmente se deja en este registro
    handlerButtom .GPIO_PinConfig.GPIO_PinPuPdControl         = GPIO_PUPDR_PULLUP; // Es entrada en configuración PULL Up, lo que nos dice que 1 es inactivo y 0 es activo
    handlerButtom .GPIO_PinConfig.GPIO_PinSpeed               = GPIO_OSPEED_MEDIUM; // Velocidad no importa
    handlerButtom .GPIO_PinConfig.GPIO_PinAltFunMode          = AF0;              // Funciones alternativas no hay en esta parte


    // Cargando el handlerButtton

    GPIO_Config(&handlerButtom);  // Cargandolo



    /* Agregando botones para el punto 4 */

    /* Agregando boton externo en Pull Down*/


    //Configurando boton externo
         GPIO_Handler_t handlerExtBut= {0};  // Creando un objeto con ese nombre iniciando en cero

         handlerExtBut .pGPIOx = GPIOC; //Salida del buton respecto al diagrama
         handlerExtBut .GPIO_PinConfig.GPIO_PinNumber              = PIN_6; // Numero del pin del boton externo
         handlerExtBut .GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_IN; // Entrada, esta dando info al micro
         handlerExtBut .GPIO_PinConfig.GPIO_PinOPType              = GPIO_OTYPE_PUSHPULL; // Corriente pa operación
         handlerExtBut .GPIO_PinConfig.GPIO_PinPuPdControl         = GPIO_PUPDR_PULLDOWN; // Entrada con esa caracteristica.  0 sin operación y 1 operando
         handlerExtBut .GPIO_PinConfig.GPIO_PinSpeed               = GPIO_OSPEED_MEDIUM; // Velocidad
         handlerExtBut .GPIO_PinConfig.GPIO_PinAltFunMode          = AF0;              // Funciones alternativas no hay en esta parte


         // Cargando el boton externo

         GPIO_Config(&handlerExtBut);  // Cargandolo





    //Configurando Led PC10
      GPIO_Handler_t handlerLedPC10= {0};  // Creando un objeto con ese nombre iniciando en cero

      handlerLedPC10 .pGPIOx = GPIOC; //Salida del buton respecto al diagrama
      handlerLedPC10 .GPIO_PinConfig.GPIO_PinNumber              = PIN_10; // Numero del pin Led
      handlerLedPC10.GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_OUT; // Salida porque se quiere prender el led
      handlerLedPC10 .GPIO_PinConfig.GPIO_PinOPType              = GPIO_OTYPE_PUSHPULL; // Corriente pa prenderlo
      handlerLedPC10 .GPIO_PinConfig.GPIO_PinPuPdControl         = GPIO_PUPDR_NOTHING; // Es salida
      handlerLedPC10 .GPIO_PinConfig.GPIO_PinSpeed               = GPIO_OSPEED_MEDIUM; // Velocidad
      handlerLedPC10 .GPIO_PinConfig.GPIO_PinAltFunMode          = AF0;              // Funciones alternativas no hay en esta parte


      // Cargando el Led de posición 10

      GPIO_Config(&handlerLedPC10);  // Cargandolo


 //Configurando Led PC11
   GPIO_Handler_t handlerLedPC11= {0};  // Creando un objeto con ese nombre iniciando en cero

   handlerLedPC11 .pGPIOx = GPIOC; //Salida del buton respecto al diagrama
   handlerLedPC11 .GPIO_PinConfig.GPIO_PinNumber              = PIN_11; // Numero del pin Led
   handlerLedPC11.GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_OUT; // Salida porque se quiere prender el led
   handlerLedPC11 .GPIO_PinConfig.GPIO_PinOPType              = GPIO_OTYPE_PUSHPULL; // Corriente pa prenderlo
   handlerLedPC11 .GPIO_PinConfig.GPIO_PinPuPdControl         = GPIO_PUPDR_NOTHING; // Es salida
   handlerLedPC11 .GPIO_PinConfig.GPIO_PinSpeed               = GPIO_OSPEED_MEDIUM; // Velocidad
   handlerLedPC11 .GPIO_PinConfig.GPIO_PinAltFunMode          = AF0;              // Funciones alternativas no hay en esta parte


 //  Cargando el Led de posición 11

   GPIO_Config(&handlerLedPC11);  // Cargandolo



   //Configurando Led PC11
	GPIO_Handler_t handlerLedPC12= {0};  // Creando un objeto con ese nombre iniciando en cero

	handlerLedPC12 .pGPIOx = GPIOC; //Salida del buton respecto al diagrama
	handlerLedPC12 .GPIO_PinConfig.GPIO_PinNumber              = PIN_12; // Numero del pin Led
	handlerLedPC12.GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_OUT; // Salida porque se quiere prender el led
	handlerLedPC12 .GPIO_PinConfig.GPIO_PinOPType              = GPIO_OTYPE_PUSHPULL; // Corriente pa prenderlo
	handlerLedPC12 .GPIO_PinConfig.GPIO_PinPuPdControl         = GPIO_PUPDR_NOTHING; // Es salida
	handlerLedPC12 .GPIO_PinConfig.GPIO_PinSpeed               = GPIO_OSPEED_MEDIUM; // Velocidad
	handlerLedPC12 .GPIO_PinConfig.GPIO_PinAltFunMode          = AF0;              // Funciones alternativas no hay en esta parte


  //  Cargando el Led de posición 12

	GPIO_Config(&handlerLedPC12);  // Cargandolo


   /* Se encargara de hacer la segunda parte para la prueba usando la función creada GPIOxTooglePin */

	// Hacer que cambie de estado cada vez que se oprima el boton, asi que
    while(1){

		 if(GPIO_ReadPin(&handlerButtom) == RESET){ // Se leyo con read

			GPIOxTooglePin(&handlerUserLedPin); // Se escribe con el Toogle

			for(int i = 0; i < 500000; i++){
		     NOP();

			}
		 } else {
			NOP();
		 }




    }


    return 0;

    }
    // Ahora se hara el cambio de configuración

    void GPIOxTooglePin(GPIO_Handler_t *pPinHandler){
    	// Se crea un condiciona

    	if ((pPinHandler -> pGPIOx -> ODR) == SET){ // Aqui me estaria leyendo de lo apuntado a comparar
	   // ODR me dice el registro del pin a la salida

    		pPinHandler -> pGPIOx -> ODR ^=(0b1 << (pPinHandler -> GPIO_PinConfig.GPIO_PinNumber));

    }

    	else{

	   // Registro alto o superior

    		pPinHandler -> pGPIOx -> ODR ^=(0b1 << (pPinHandler -> GPIO_PinConfig.GPIO_PinNumber));

    }
    }
