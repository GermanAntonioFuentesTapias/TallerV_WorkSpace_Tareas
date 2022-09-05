/*
 * GPIOxSlnTarea.c
 *
 *  Created on: 03/09/2022
 *      Author: German Fuentes
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
* asi que si se realizaba una primera operación sobre ese pin quedaba guardando esa posición, y al hacer otra seguira en el mismo
* estado, ese seria en error
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
* el codigo usado fue el siguiente: (omitiendo el main y el puerto del led del micro)
*
*     //Coonfigurando otro Pin para externo
        GPIO_Handler_t handlerUserLedExtern = {0};


        //configurando el pin
           handlerUserLedExtern.pGPIOx = GPIOB;
           handlerUserLedExtern.GPIO_PinConfig.GPIO_PinNumber              = PIN_9;
           handlerUserLedExtern.GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_OUT; // Salida
           handlerUserLedExtern.GPIO_PinConfig.GPIO_PinOPType              = GPIO_OTYPE_PUSHPULL; //Corriente
           handlerUserLedExtern.GPIO_PinConfig.GPIO_PinPuPdControl         = GPIO_PUPDR_NOTHING; // La mayoria de veces que se tiene salida va en nothing
           handlerUserLedExtern.GPIO_PinConfig.GPIO_PinSpeed               = GPIO_OSPEED_MEDIUM; // Velocidad
           handlerUserLedExtern.GPIO_PinConfig.GPIO_PinAltFunMode          = AF0;              // Mirar funciones y en pi

           //Cargarlo
           GPIO_Config(&handlerUserLedExtern);

      // Configurando pin del Boton
           GPIO_Handler_t handlerUserBExtern = {0};

       // Configurando
           handlerUserBExtern.pGPIOx = GPIOC;
           handlerUserBExtern.GPIO_PinConfig.GPIO_PinNumber              = PIN_6;
           handlerUserBExtern.GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_IN; // Salida
           handlerUserBExtern.GPIO_PinConfig.GPIO_PinOPType              = GPIO_OTYPE_PUSHPULL; //Corriente
           handlerUserBExtern.GPIO_PinConfig.GPIO_PinPuPdControl         = GPIO_PUPDR_PULLUP; // La mayoria de veces que se tiene salida va en nothing
           handlerUserBExtern.GPIO_PinConfig.GPIO_PinSpeed               = GPIO_OSPEED_MEDIUM; // Velocidad
           handlerUserBExtern.GPIO_PinConfig.GPIO_PinAltFunMode          = AF0;              // Mirar funciones y en pi

       // Lo cargo
           GPIO_Config(&handlerUserBExtern);

           //toca llamar la función que me lee el estado del pin
        while(1){
        	if(GPIO_ReadPin(&handlerUserBExtern) == 1 &&  GPIO_ReadPin(&handlerUserButtom) == 0 ){
        	for(unsigned int i = 0; i < 500000; i++){
        	    		NOP();
            }

        	GPIO_WritePin(&handlerUserLedPin, SET);
        	GPIO_WritePin(&handlerUserLedExtern, RESET);

        	for (unsigned int i = 0; i < 500000; i++){
        		NOP();
        	}

        	GPIO_WritePin(&handlerUserLedPin, RESET);
        	GPIO_WritePin(&handlerUserLedExtern, SET);
        	}
        	//Punto B //Si se presiona el botón externo, solo el LED externo debe hacer el blinky, y el userLED debe permanecer encendido.

        	 else if(GPIO_ReadPin(&handlerUserBExtern) == 0  &&  GPIO_ReadPin(&handlerUserButtom) == 1 ){

             GPIO_WritePin(&handlerUserLedPin, SET);

        	 for(unsigned int i = 0; i < 50000; i++){
        						NOP();
			  }

			 GPIO_WritePin(&handlerUserLedExtern, RESET);

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

*
*
*/


/* Función principal del programa. Es acá donde se ejecuta todo */
int main(void){

	// ***************
	// Definimos el handler para el PIN que deseamos configurar

	GPIO_Handler_t handlerUserLedPin = {0};  // Creando un objeto con ese nombre iniciando en cero

	//Deseamos trabajar con el puerto GPIOA

    handlerUserLedPin.pGPIOx = GPIOA;
    handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber              = PIN_5;
    handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_OUT; // Salida
    handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType              = GPIO_OTYPE_PUSHPULL; //Corriente
    handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl         = GPIO_PUPDR_NOTHING; // La mayoria de veces que se tiene salida va en nothing
    handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed               = GPIO_OSPEED_MEDIUM; // Velocidad
    handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode          = AF0;              // Mirar funciones y en pin 5 no hacer nada

    // Cargamos al configuración del PIN especifico
    GPIO_Config(&handlerUserLedPin);  // Lllama y configura al PIN

    // Hacemos que el PIN_A5 quede encedido
    GPIO_WritePin(&handlerUserLedPin, SET); // Se ve prendido porque lo estoy colocando en 1


    // Punto 2, se necesita hacer Handler
    //  SIEMPRE definirlo, configurarlo y cargarlo
    //Coonfigurando otro Pin
    GPIO_Handler_t handlerUserButtom = {0};  // Creando un objeto con ese nombre iniciando en cero nueva entonces se creo con otro nombre poeque es otro pin

	handlerUserButtom.pGPIOx = GPIOC; //Salida del buton respecto al diagrama
	handlerUserButtom.GPIO_PinConfig.GPIO_PinNumber              = PIN_13; // Numero del pin
	handlerUserButtom.GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_IN; // Entrada porque esta mandando informacion al micro
	handlerUserButtom.GPIO_PinConfig.GPIO_PinOPType              = GPIO_OTYPE_PUSHPULL; // Dejar asi porque casi no se usa
	handlerUserButtom.GPIO_PinConfig.GPIO_PinPuPdControl         = GPIO_PUPDR_PULLUP; // La mayoria de veces que se tiene salida va en nothing, va en pullup segun diaggrama cierto y es la entrada que nos da el boton
	handlerUserButtom.GPIO_PinConfig.GPIO_PinSpeed               = GPIO_OSPEED_MEDIUM; // Velocidad no importa
	handlerUserButtom.GPIO_PinConfig.GPIO_PinAltFunMode          = AF0;              // Mirar funciones y en pin 5 no hacer nada


    // cargarlo

    GPIO_Config(&handlerUserButtom);  // Cargandolo por si

    //Este es el ciclo principal, donde se ejecuta todo el programa
//     Punto 1
//    while(1){ // Se queda girando infinitamente
//    Control + shift + c
//    	for(unsigned int i = 0; i < 5000000; i++){
//    		NOP();
//    	}
//    	GPIO_WritePin(&handlerUserLedPin, RESET);
//
//    	for(unsigned int i = 0; i < 5000000; i++){
//
//    		NOP();  // Aca se pone la logica de los programas
//        }
//    	GPIO_WritePin(&handlerUserLedPin, SET);
//      }

       // Punto 2

//       while (1){
//    	   if( GPIO_ReadPin(&handlerUserButtom) == 1){  // Aqui estamos leyendo el estado de ese pin en especifico
//
//    	    for(unsigned int i = 0; i < 500000; i++){
//    		    		NOP();
//    		   	}
//			GPIO_WritePin(&handlerUserLedPin, RESET);
//
//			for(unsigned int i = 0; i < 500000; i++){
//
//				NOP();  // Aca se pone la logica de los programas
//			   }
//			GPIO_WritePin(&handlerUserLedPin, SET);
//    	    } else{
//
//    		   for(unsigned int i = 0; i < 250000; i++){
//    		       		    		NOP();
//    		       		   	}
//			GPIO_WritePin(&handlerUserLedPin, RESET);
//
//			for(unsigned int i = 0; i < 250000; i++){
//
//				NOP();  // Aca se pone la logica de los programas
//			   }
//			GPIO_WritePin(&handlerUserLedPin, SET);
//    	   }
//       }




         }
