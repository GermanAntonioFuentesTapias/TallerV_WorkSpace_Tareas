/*
 * GPIOxDriver.h
 *
 *  Created on: 29/08/2022
 *      Author: German Fuentes
 *
 *  Este programa se encarga de las definiciones relacionadas únicamente con el
 *  debido manejo y control del periférico GPIOx (funciones del controlador)
 */

#ifndef GPIOXDRIVER_H_
#define GPIOXDRIVER_H_

// Incluyendo este archivo estamos incluyendo también el correspondiente al GPIOx
#include "stm32f4xx_hal.h"

typedef struct
{
	  uint8_t GPIO_PinNumber;             //PIN con el que deseamos trabajar
	  uint8_t GPIO_PinMode;               //Modo de la configuración: Entrada, salida, ánalogo, f.alternativa
	  uint8_t GPIO_PinSpeed;              //Velocidad de respuesta del PIN (Solo para digital)
	  uint8_t GPIO_PinPuPdControl;        //Seleccionamos si deseamos una salida Pull-up,Pull-down o "libre"
	  uint8_t GPIO_PinOPType;             // Trabaja de la mano con el anteriro,, selecciona salida PUPD o OpenDrain
	  uint8_t GPIO_PinAltFunMode;         // Seleccion cual es la función alternativa que se esta configurando
}GPIO_PinConfig_t;


/* Esta es una estructura que contiene dos elementos:
 *    - La dirección del puerto que se esta utilizando ( La referencia al puerto)
 *    - La configuración específica del PIN que se está utilizando
 */

typedef struct
{
	  GPIOx_RegDef_t    *pGPIOx;         /* !< Dirección del puerto al que el PIN corresponder >*/ // Los regristros
	  GPIO_PinConfig_t   GPIO_PinConfig; /* !< Configuración del PIN > */ // La configuracion de arriba
}GPIO_Handler_t;


/* Definición de las cabeceras de las funciones del GPIOxDriver */

void GPIO_Config (GPIO_Handler_t *pGPIOHandler);  // Para configurar el Pin
void GPIO_WritePin(GPIO_Handler_t *pPinHandler, uint8_t newState); // Para escribir sobre un pin
uint32_t GPIO_ReadPin (GPIO_Handler_t *pPinHandler);  // Para leer un pin en especifico


#endif /* GPIOXDRIVER_H_ */
