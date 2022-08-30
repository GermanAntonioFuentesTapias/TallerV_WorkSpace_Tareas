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
	GPIO_Handler_tt handlerUserLedPin = {0};  // Creando un objeto con ese nombre iniciando en cero

	//Deseamos trabajar con el puerto GPIOA

    handlerUserLedPin.pGPIOx = GPIOA;
    handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber              = PIN_5;
    handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_OUT; // Salida
    handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType              = GPIO_OTYPE_PUSHPULL; //Corriente
    handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl         = GPIO_PUPDR_NOTHING;
    handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed               = GPIO_OSPEED_MEDIUM; // Velocidad
    handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode          = AF0;              // Mirar funciones y en pin 5 no hacer nada

    // Cargamos al configuración del PIN especifico
    GPIO_Config(&handlerUserLedPin);  // Lllama y configura al micro

    // Hacemos que el PIN_A5 quede encedido
    GPIO_WritePin(&handlerUserLedPin, SET);

    //Este es el ciclo principal, donde se ejecuta todo el programa

    while(1){ // Se queda girando infinitamente
    	NOP();  // Aca se pone la logica de los programas
    }
}
