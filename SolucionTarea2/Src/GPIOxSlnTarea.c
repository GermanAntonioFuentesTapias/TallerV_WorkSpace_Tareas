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
* asi que si se realizaba una primera operación sobre ese pin, queda  guardan en esa posición, y al hacer otra puede quedar
* como se encontraba, ya que no se sabe que hay en esa posición de memoria o modificar la posición, como también puede cambiar el estado,
* o alterarlo de alguna manera que no se quiere. El registro IDR está desplazado a el lado derecho tantas veces como la magnitud
* de la ubicación del pin especifico, pero no las otras posiciones que se desconocen.
*
* b)
* La solución sera crear una linea en particular, con el fin de  limpiar las posiciones tantas veces a derecha del pin de interes
* Esta solución seria agregar el siguiente comando:
* pinValue &= 0b1;  que sera el operador  And con la mascara ya nombrada, asi se podra cumplir que los otros bits estan en valor cero
*
*
* c) La correción del codigo se probo con los ejercicios de escritura del GPIO, por lo tal se uso el siguiente codigo:
*
* 1: Sin el codigo, se hacia alguno de los 3 estados que pedian y se desarrolla, no obstante al hacer otro (cambio de estado) no ocurria nada
* se mantenia en ese estado, sin sobreescribir en ningun otro punto. Ya después de corregirlo se observa que se puede
* variar entre los 3 estados que pedian y todo ocurre sin problemas.
*
* N O T A : el codigo usado fue el siguiente: (omitiendo el main y el puerto del led del micro)
*
*     //Coonfigurando otro Pin para externo utilizado
        GPIO_Handler_t handlerUserLedExtern = {0};


        //configurando el pin de interes
         * Se configuro el puerto del pin
           handlerUserLedExtern.pGPIOx = GPIOB;
           handlerUserLedExtern.GPIO_PinConfig.GPIO_PinNumber              = PIN_9;  // Numero de puerto
           handlerUserLedExtern.GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_OUT; // Salida // Como salida
           handlerUserLedExtern.GPIO_PinConfig.GPIO_PinOPType              = GPIO_OTYPE_PUSHPULL; //Corriente // Necesita prender el led
           handlerUserLedExtern.GPIO_PinConfig.GPIO_PinPuPdControl         = GPIO_PUPDR_NOTHING; // La mayoria de veces que se tiene salida va en nothing
           handlerUserLedExtern.GPIO_PinConfig.GPIO_PinSpeed               = GPIO_OSPEED_MEDIUM; // Velocidad siempre igual
           handlerUserLedExtern.GPIO_PinConfig.GPIO_PinAltFunMode          = AF0;              // Funciones que aun no usamos

           // Se cargo la configuración del pin de interes
           GPIO_Config(&handlerUserLedExtern);

      // Configurando pin del Boton de interes externo
           GPIO_Handler_t handlerUserBExtern = {0};

       // Configurando el boton
           handlerUserBExtern.pGPIOx = GPIOC;
           handlerUserBExtern.GPIO_PinConfig.GPIO_PinNumber              = PIN_6;
           handlerUserBExtern.GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_IN; // Salida
           handlerUserBExtern.GPIO_PinConfig.GPIO_PinOPType              = GPIO_OTYPE_PUSHPULL; //Corriente
           handlerUserBExtern.GPIO_PinConfig.GPIO_PinPuPdControl         = GPIO_PUPDR_PULLUP; // Asi a que es una entrada
           handlerUserBExtern.GPIO_PinConfig.GPIO_PinSpeed               = GPIO_OSPEED_MEDIUM; // Velocidad
           handlerUserBExtern.GPIO_PinConfig.GPIO_PinAltFunMode          = AF0;              // Funciones que aun no usamos

       //Se carga el pin externo
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


// Con  los puntos A-B-C de codigo anterior se observo que algo no funcionaba correctamente, ya que al querer hacer esos diversos
// cambios de estados , empezando por el B, y luego el A, al querer realizar el C no pasaba nada, ya que tenia en esta configuración
// una informacion ya aplicada, pero las nuevas no se modificaban para variar al estado, ya que no se le indicaba correctamente.

/* Desarrollo del punto 2  */


// Se procede a crear  una función para que este programa la reconoce, esta estara definida en la parte inferior ( despues del main)

//Se llama la función ya que al usarse en el Main y no estar llamada con anterioridad se genera un warning.
void GPIOxTooglePin(GPIO_Handler_t *pPinHandler ); // Se crea para que se una función global en los While del main


/* Función principal del programa. Es acá donde se ejecuta todo los puntos siguientes */

int main(void){

	// ***************
	// Definimos el handler para el pin que usaremos que tendra por nombre  USER_PIN y que se configurara


	GPIO_Handler_t handlerUserLedPin = {0};  // Creando un objeto con ese nombre iniciando en cero

	//Deseamos trabajar con el puerto GPIOA por facilidad, no obstante se podria usar otro puerto. En nuestro caso
	// el led interno del micro esta en el PIN 5 como salida, asi que:

    handlerUserLedPin.pGPIOx = GPIOA;
    handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber              = PIN_5;
    handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_OUT; // Salida del Pin que se tiene
    handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType              = GPIO_OTYPE_PUSHPULL; //Corriente pero no se desea prender en este caso
    handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl         = GPIO_PUPDR_NOTHING; // No hay entrada, asi que se mantiene este registro
    handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed               = GPIO_OSPEED_MEDIUM; // Velocidad
    handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode          = AF0;              // Función alternativa aun no se requiere

    // Recordar que es necesario cargar la configuración del PIN especifico

    GPIO_Config(&handlerUserLedPin);  // Este llama y configura al PIN en el pin especifico.

    // Ahora necesitamos configurar el boton para el led asi que se debe hacer lo mismo pero con el boton que nos cambiara
    // Los diferentes parametros del led

    // Hacemos que el PIN_A5 quede encedido
    GPIO_WritePin(&handlerUserLedPin, SET); // Se ve prendido porque lo estoy colocando en 1


    // Continuando con el Punto 2, se necesita hacer Handler para el boton
    //  recordar SIEMPRE definirlo, configurarlo y cargarlo
    //Coonfigurando otro Pin
    GPIO_Handler_t handlerButtom = {0};  // Creando un objeto con ese nombre iniciando en cero

    handlerButtom .pGPIOx = GPIOC; //Salida del buton respecto al diagrama
    handlerButtom .GPIO_PinConfig.GPIO_PinNumber              = PIN_13; // Numero del pin a configurar
    handlerButtom .GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_IN; // Entrada porque esta mandando informacion al micro con la presión
    handlerButtom .GPIO_PinConfig.GPIO_PinOPType              = GPIO_OTYPE_PUSHPULL; // Normalmente se deja en este registro
    handlerButtom .GPIO_PinConfig.GPIO_PinPuPdControl         = GPIO_PUPDR_PULLUP; // Es entrada en configuración PULL Up, lo que nos dice que 1 es inactivo y 0 es activo
    handlerButtom .GPIO_PinConfig.GPIO_PinSpeed               = GPIO_OSPEED_MEDIUM; // Velocidad no importa
    handlerButtom .GPIO_PinConfig.GPIO_PinAltFunMode          = AF0;              // Funciones alternativas no hay en esta parte


    // Cargando el handlerButtton

    GPIO_Config(&handlerButtom);  // Cargandolo



    /* Agregando botones para el punto 4 */

    /* Agregando boton externo en Pull Down*/


    //Configurando el boton externo
         GPIO_Handler_t handlerExtBut= {0};  // Creando un objeto con ese nombre iniciando en cero

         handlerExtBut .pGPIOx = GPIOC; //Salida del buton respecto al diagrama
         handlerExtBut .GPIO_PinConfig.GPIO_PinNumber              = PIN_6; // Numero del pin del boton externo
         handlerExtBut .GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_IN; // Entrada, esta dando info al micro
         handlerExtBut .GPIO_PinConfig.GPIO_PinOPType              = GPIO_OTYPE_PUSHPULL; // Corriente para operación
         handlerExtBut .GPIO_PinConfig.GPIO_PinPuPdControl         = GPIO_PUPDR_PULLDOWN; // Entrada con esa caracteristica.  0 sin operación y 1 operando
         handlerExtBut .GPIO_PinConfig.GPIO_PinSpeed               = GPIO_OSPEED_MEDIUM; // Velocidad
         handlerExtBut .GPIO_PinConfig.GPIO_PinAltFunMode          = AF0;              // Funciones alternativas no hay en esta parte


         // Cargando el boton externo

         GPIO_Config(&handlerExtBut);





    //Configurando Led PC10
      GPIO_Handler_t handlerLedPC10= {0};  // Creando un objeto con ese nombre iniciando en cero

      handlerLedPC10 .pGPIOx = GPIOC; //Puerto del pin
      handlerLedPC10 .GPIO_PinConfig.GPIO_PinNumber              = PIN_10; // Numero del pin Led
      handlerLedPC10.GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_OUT; // Salida porque se quiere prender el led
      handlerLedPC10 .GPIO_PinConfig.GPIO_PinOPType              = GPIO_OTYPE_PUSHPULL; // Corriente pa prender el led
      handlerLedPC10 .GPIO_PinConfig.GPIO_PinPuPdControl         = GPIO_PUPDR_NOTHING; // Es salida
      handlerLedPC10 .GPIO_PinConfig.GPIO_PinSpeed               = GPIO_OSPEED_MEDIUM; // Velocidad
      handlerLedPC10 .GPIO_PinConfig.GPIO_PinAltFunMode          = AF0;              // Funciones alternativas no hay en esta parte


      // Cargando el Led de posición 10

      GPIO_Config(&handlerLedPC10);  // Cargando el led del pin 10


 //Configurando Led PC11
   GPIO_Handler_t handlerLedPC11= {0};  // Creando un objeto con ese nombre iniciando en cero

   handlerLedPC11 .pGPIOx = GPIOC; //Puerto del pin
   handlerLedPC11 .GPIO_PinConfig.GPIO_PinNumber              = PIN_11; // Numero del pin Led
   handlerLedPC11.GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_OUT; // Salida porque se quiere prender el led
   handlerLedPC11 .GPIO_PinConfig.GPIO_PinOPType              = GPIO_OTYPE_PUSHPULL; // Corriente pa prender el led
   handlerLedPC11 .GPIO_PinConfig.GPIO_PinPuPdControl         = GPIO_PUPDR_NOTHING; // Es salida
   handlerLedPC11 .GPIO_PinConfig.GPIO_PinSpeed               = GPIO_OSPEED_MEDIUM; // Velocidad
   handlerLedPC11 .GPIO_PinConfig.GPIO_PinAltFunMode          = AF0;              // Funciones alternativas no hay en esta parte


 //  Cargando el Led de posición 11

   GPIO_Config(&handlerLedPC11);  //  Cargando el led del pin 11



   //Configurando Led PC12
	GPIO_Handler_t handlerLedPC12= {0};  // Creando un objeto con ese nombre iniciando en cero

	handlerLedPC12 .pGPIOx = GPIOC;                            //Puerto del pin
	handlerLedPC12 .GPIO_PinConfig.GPIO_PinNumber              = PIN_12; // Numero del pin Led
	handlerLedPC12.GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_OUT; // Salida porque se quiere prender el led
	handlerLedPC12 .GPIO_PinConfig.GPIO_PinOPType              = GPIO_OTYPE_PUSHPULL; // Corriente pa prender el led
	handlerLedPC12 .GPIO_PinConfig.GPIO_PinPuPdControl         = GPIO_PUPDR_NOTHING; // Es salida, por lo que va en NOTHING
	handlerLedPC12 .GPIO_PinConfig.GPIO_PinSpeed               = GPIO_OSPEED_MEDIUM; // Velocidad del pin
	handlerLedPC12 .GPIO_PinConfig.GPIO_PinAltFunMode          = AF0;              // Funciones alternativas no hay en esta parte


  //  Cargando el Led de posición 12

	GPIO_Config(&handlerLedPC12);  //  Cargando el led del pin 12


   /* Se encargara de hacer la segunda parte para la prueba usando la función creada GPIOxTooglePin */

	// Hacer que cambie de estado cada vez que se oprima el boton, asi que
    while(1){

		 if(GPIO_ReadPin(&handlerButtom) == RESET){ // Se uso con read para leer el estado del boton, si su estado es cero o uno y comparar

			GPIOxTooglePin(&handlerUserLedPin); // Se escribe con el Toogle de la nueva función, haciendo lo que hace la función con la configuracion que tiene el handlerUserLedPin

			for(int i = 0; i < 500000; i++){ // Se hace una variación en el tiempo del micro para variar su estado

		     NOP();// Para que no haga nada mientras ocurren los ciclos

			}
		 } else {
			NOP(); // Para que no haga nada mientras ocurren los ciclos
		 }

	/* ELABORACIÓN DEL CUARTO PUNTO */

	// Ya se tiene el pin_6 con el cual se configuro el boton externo en Pull Down y la configuración de los
	// otros 3 botones PC10, PC11 Y PC12

    // Debemos encender los 3 leds cuando presione asi que

	 if(GPIO_ReadPin(&handlerExtBut) == 1){ // Cuando el boton este presionado se cumple ya que es Pull Down

		 // Debido a que es Pull Down es necesario agregar al circuito los 3.3 V porque si se coloca como tierra
		 // La comparación sera con cero y se obtendra cero en ambos estados.

		 // Se prenden los 3 leds inmediato despues de presionarlo

		 GPIO_WritePin(&handlerLedPC10, SET); // Prende led 10
		 GPIO_WritePin(&handlerLedPC11, SET); // Prende led 11
		 GPIO_WritePin(&handlerLedPC12, SET); // Prende led 12

		 for(unsigned int i = 0; i < 6000000; i++){ // Con esto pasarian aproximadamente esa cantidad de ciclos dentro del micro
			 // que se acerca a los 5 segundos requeridos
			 // Fue sin signo pero se puede garantizar desde i=0 un el contador i++

			 NOP(); // Para que no haga nada mientras ocurren los ciclos

		     }
		      GPIO_WritePin(&handlerLedPC12, RESET); // Se apaga este led despues de cumplidos los ciclos anteriores

			   for(unsigned int i = 0; i < 2400000; i++){

			  NOP();  // Para que no haga nada mientras ocurren los ciclos

			   }
			   /* Despues de ocurrido lo anterior necesitamos que el led 11 se apaga despues de 2 segundos */

			   GPIO_WritePin(&handlerLedPC11, RESET);   // Se apaga este led despues de cumplidos los ciclos anteriores del for garantizando el tiempo aproximado

			   for(unsigned int i = 0; i < 1200000; i++){

               NOP(); // Para que no haga nada mientras ocurren los ciclos

			   }
			   /* Despues de ocurrido lo anterior necesitamos que el led 10 se apague despues de 1 segundo */

			   GPIO_WritePin(&handlerLedPC10, RESET);

			   // Se apaga este led despues de cumplidos los ciclos anteriores del for garantizando el tiempo aproximado

	 }
	 }

    return 0;

    }
    /* Se crea la condición de GPIOxTooglePin*/

    void GPIOxTooglePin(GPIO_Handler_t *pPinHandler){ // Se creo con un parametro tipo GPIO_Handler_t

	  // ODR me dice el registro del pin a la salida
     // Void seria una función que no retorna nada

    pPinHandler -> pGPIOx -> ODR ^=(0b1 << (pPinHandler -> GPIO_PinConfig.GPIO_PinNumber));

    // Se elaboro la función de manera que el puntero señale la estructura GPIO_Handler_t, en esta se esta dirigiendo dentro de esta
    // a la dirección del puerto que se esta utilizando y este en nuestro caso sera el puerto:  port output data register
    // Este se encargara de leer y escribir en la posición dada por el GPIO_PinConfig.GPIO_PinNumber asi que ya en operacion la funcion
    // por ejemplo si es posicion 5, con el XOR se encargara de escribir unicamente en esa posición manteniendo las otras en el mismo estado
    // y asi respectivamente, entonces ese una función general para distintos pines.
    // Si se le coloca un condicional se podria crear un codigo de manera que lea y mire si ya esta registrado ese pin o es necesario agregarlo
    }
