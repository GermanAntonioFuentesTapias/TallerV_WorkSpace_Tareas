/*
 * main.c
 *
 *  Created on: 8/09/2022
 *      Author: German
 */


//  Taller  4

#include <stdint.h>

#include "stm32f4xx_hal.h" //Las creadas antes, ojo con los nombres
#include "GPIOxDriver.h"

#define SUMA '+' // Numero de 8 bits
#define MULTIPLICACION '*'
#define RESTA '-'
#define DIVISION '/'

int16_t resultadoOperacion(uint8_t numero1, int8_t numero2, uint8_t Operacion);;
// 3 como caracter no es el 3 de la variable

int main(void){

     int8_t VariableA = 150;
     int16_t VariableB = 25;

     int16_t resultadoSuma = resultadoOperacion(VariableA, VariableB, DIVISION);

     resultadoSuma++;
	while(1){

	}
}

int16_t resultadoOperacion(uint8_t numero1, int8_t numero2, uint8_t Operacion){ // ingresa 8 bits pero puede sacar 16 pa que quepa

	int16_t resultado = 0;

	switch(Operacion){

	case SUMA:

	{
		resultado = numero1+ numero2;
		break;
	}

	case RESTA:
	{
		resultado = numero1 - numero2;
		break;
	}
	case MULTIPLICACION:
	{
		resultado = numero1 * numero2;
		break;
	}
	case DIVISION:
	{
		resultado = numero1 / numero2;
		break;
	}
	default:{
		resultado=0;
		break;
	}
	}

	return resultado;

}




// Crear una funcion con While


void delay(uint32_t iteraciones){

	uint32_t i = 0;

	while(i < iteraciones){
		i++;
		NOP();
	}


}
