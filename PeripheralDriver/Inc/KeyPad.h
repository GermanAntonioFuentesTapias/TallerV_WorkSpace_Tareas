/*
 * KeyPad.h
 *
 *  Created on: 24/11/2022
 *      Author: German
 */
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "stdbool.h"

#ifndef KEYPAD_H_
#define KEYPAD_H_

/* Driver para el KeyPad a utilizar */

// El numero de utilizado en este KeyPad seran

#define KEYPAD_UNIDADES   1
#define FILAS             4
#define COLUMNA           4
#define KEYS              16

#define   KEYS_PRESSED    1
#define   KEYS_REALEASED  0



typedef struct
{
	GPIO_Handler_t *pGPIOHandler;	// Canal ADC que será utilizado para la conversión ADC

}Keypad_WiresTypeDef;


void Keypad4x4_Init(Keypad_WiresTypeDef  *KeypadWiringStruct);

//Function(1): Set Keypad pins and ports


/* Funciones */

// Función para iniciar
void KEYPAD_INIT(GPIO_Handler_t *ptrKey);



// Obtener posiciones de pines para uso de cambio de columna, solo para pines de salida

static void Keypad4x4_FindPins_positions(void);
//
//// Cambiar numero de columna
//
//static void KEYPAD4x4_Change(uint8_t colNum_0_to_3);

// Función para leer botón de keypad

void KEYPAD4x4_ReadKey(bool keys[16]);

// Obtener caracteristica

char *KEYPAD4x4_GetChar(uint8_t keypadSw);


void KEYPAD_SCAN(GPIO_Handler_t );
#endif /* KEYPAD_H_ */
