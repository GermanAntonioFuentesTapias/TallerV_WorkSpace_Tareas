/*
 * GPIOxDriverMain.c
 *
 *  Created on: 29/08/2022
 *      Author: German Fuentes
 *
 *
 * ************************************************
 *
 * Con este programa se desea mostrar el uso básico de los registros que controlan
 * al Micro (SFR) y la forma adecuada para utilizar los operadores &, |, ~ y =, para
 * cambiar la configuración de algún registro
 * También es importante para entender la utilidad de los numeros BIN y Hex
 *
 * Es necesario tener a la mano tanto el manual de referencia del micro, como la hoja de
 * datos del micro ( ambos son documentos diferentes y complementarios el uno del otro)
 *
 * HAL -> Hardware Abstraction Layer
 *
 * Este programa introduce el periférico mas simple que tiene el micro, que es el
 * encargado de manejar los pines de cada puerto del micro.
 *
 * Cada PINx de cada puerto GPIO puede desarrollar funciones básicas de tipo entrada
 * y salida de datos digitales (o sea 1's y 0's), además se les puede asignar funciones
 * especiales que generalmente están ligadas a otro periferico adicional
 *
 * De igual forma, varias características pueden ser configuradas para cada el PINx
 * específico como son:
 *   - La velocidad a la que puede responder
 *   - Tipo de entrada (pull up, pull down, open drain (flotante))
 *   - Tipo de salida (push-pullm open drain...)
 *   - Entrada análoga
 *
 *   Debemos definir entonces todos los registros que manejan el periférico GPIOx y luego
 *   crear algunas funciones para utilizar adecuadamente el equipo.
 *
 *****************************************************************************************
 *
 */


#include <stdint.h>

#include "stm32f4xx_hal.h" //Las creadas antes, ojo con los nombres
#include "GPIOxDriver.h"   //Elementos del hadware driver

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

    // Punto 3 externo


    //Coonfigurando otro Pin para externo
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


         }

