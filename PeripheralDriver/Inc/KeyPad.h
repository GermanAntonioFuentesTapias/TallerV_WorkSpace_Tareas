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


#endif /* KEYPAD_H_ */
