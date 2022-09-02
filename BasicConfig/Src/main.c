/**
 ******************************************************************************
 * @file           : main.c
 * @author         : German Antonio Fuentes
 * @brief          : Configuración inicial basica de un proyecto
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

#include <stdint.h>

uint16_t data = 0;
uint16_t *ptrEjemplo;
uint32_t ValorPuntero = 0;

int main(void){
//    Iniciando variables
	data= 57;

	// Cambiando el valor de data en +32
	data +=32;

	// A que el puntero apunte

	ptrEjemplo = &data;

	ValorPuntero = (uint32_t) ptrEjemplo;

    *ptrEjemplo +=2;

    // Incrementamos posición de memoria

    ptrEjemplo++;

    *ptrEjemplo= 0xAC;




	while(1){

	}

	return 0;
}
