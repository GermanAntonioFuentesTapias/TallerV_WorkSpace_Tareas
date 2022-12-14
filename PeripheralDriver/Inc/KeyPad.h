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

#define TECLA_UNO       0
#define TECLA_DOS       1
#define TECLA_TRES      2
#define TECLA_CUARTRO   3
#define TECLA_CINCO     4
#define TECLA_SEIS      5
#define TECLA_SIETE     6
#define TECLA_OCHO      7
#define TECLA_NUEVE     8
#define TECLA_ESTRELLA  9
#define TECLA_NUMERAL   10
#define TECLA_CERO      11
#define TECLA_A         12
#define TECLA_B         13
#define TECLA_C         14
#define TECLA_D         15
#define NOTHING         16

#define ACTIVAR_A       17
#define ACTIVAR_S       18
#define DESACTIVAR      19
#define ATRAS           20
#define ATRAS_CONFI     29
// Menus y estados
#define INICIO          21
#define MENUS           22
#define MENU_CONF       23

/* Parametros menu principal */
#define CONFIGURACION   26
#define FECHAYHORA      28
#define PARAMETROS      27

#define ENTRO           24
#define SALIO           25

/* Parametros opciones */

#define CONTRASE        30
#define SONIDO          31
#define PANICO          32
#define ATRAS_PARA      33

/* Fecha y hora opciones */

#define CAMBIAR_FECHA   34
#define CAMBIAR_HORA    35
#define EVENTOS         36
#define ATRAS_FyH       37

#define MOVIMIENTO      38
#define CAMBIO          39


typedef struct
{
  uint8_t Primera_Posicion; /* Primera posición*/

  uint8_t Segunda_Posicion; /* Segunda posición */

  uint8_t Tercera_Posicion; /* Tercera posición */

}handler_Contrasena;

uint8_t* datos_Posicion(void);

void Set_KeyPad(handler_Contrasena *ptrHandlerKey);

void Key_Update(handler_Contrasena  *ptrRTCHandler, uint16_t New);


#endif /* KEYPAD_H_ */
