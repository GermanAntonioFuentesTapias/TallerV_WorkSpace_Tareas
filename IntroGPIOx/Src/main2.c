/*
 * main2.c
 *
 *  Created on: 16/09/2022
 *      Author: German
 */

#include <stdint.h>

int main(void){

	// Ejemplo punteros

	uint8_t dato = 124;

	//Creación de un puntero

	uint8_t *pDato; // La asignamos porque es el tamaño que estaremos dirigiendonos, de 8 bits


	// Asignando el valor en memoria al puntero

	pDato=&dato;  // tiene la dirreción de memoria no fisica

	// el puntero es una variable que me da la información de la memoria de dato por lo que se crea un variable de 32 bit
	//Casteo al puntero osea cambiarle el tamaño en la operación

	//Casteo me convierte el tipo caracter puntero y me lo devuelve a caracter normal

	// casteo pierda el atributo de puntero y sea la etiqueta del buzon

	uint32_t direccionDato = (uint32_t) pDato; // Se paso porque queriamos pasar el tipo de variable puntero a información. ya que es un tipo de variable puntero, pero su cualidad es ser puntero y lo necesitamos normal

	*pDato = 200;

     // Pasar a la siguiente direccion de memoria

	pDato++;

	// asterisco accedo al valor

	uint8_t nuevoDato = *pDato;

    //ahora con una variable de 16 bits

	uint16_t variable = 12;

	// Creamos un puntero para variable de 16 bits

	uint16_t *pVariable;

	// asignar dirección de memoria

	pVariable = &variable;

	uint32_t dir = (uint32_t) pVariable;

	pVariable++;

	// guardar un nuevo valor a traves de un puntero

	*pVariable = 12;


 // Arreglo tener en cuenta que permite almacenar información del mismo tipo, tipo de dato del arreglo

	// tipo arreglo   Nombre_arreglo (elementos)  Nombre_arreglo--> es un puntero del primero elemento del arreglo


	  // NombreArreglo[elemento1, elemento2]

    // 2.1 Creación de arreglo

    #define sizeOfArray 4 //tamaño de 4

	uint8_t miPrimerArreglo[sizeOfArray] = {5,0xA, 'a' , 254};

	// 2.1 Recorrer los arrelgos con ciclos

	for(uint8_t i =0 ; i < sizeOfArray;i++){

		uint8_t contenido = miPrimerArreglo[i];
	}

	//2.2 Valores del arreglo a través  de un puntero

	for(uint8_t i = 0; i < sizeOfArray; i++){

	uint8_t contenido = * (miPrimerArreglo + i); // Tomando desde el valor cero del elemento y irse desplazandose


	// 3.0 estructuras algo que me permite guarda dato de diferente tipo

	// Grupo de almacenamiento de dato

	// Crear un tipo de dato

	// typedef struct {
	 //   unit8_t elemento0,
	//   unit8_t elemento1;
	// nombreTipodeDato

	// Definicion general

   typedef struct{

	   uint8_t elemento1;
	   uint16_t elemento2;
	   uint32_t elemento3;
	   // pueden almacenar mas estructuras o arreglos

	   uint8_t arreglo[sizeOfArray];
   }miPrimeraEstructura_t; // es definida por nosotros

   miPrimeraEstructura_t estructuraEjemplo = {0}; // inicializando la estructura

   // Como accedo a los elementos de una estructura, los inicializamos

   // Asignamos valores

   estructuraEjemplo.elemento1 = 'F';
   estructuraEjemplo.arreglo[0] = 10; // en la posición cero del arreglo guardo un 10


   // Como acceder a las estructuras con arreglos

   //Que es lo primero, definir el tipo de dato al que apunta

   miPrimeraEstructura_t *ptrMiPrimeraEstructura;

   ptrMiPrimeraEstructura-> elemento1 = 9;

   ptrMiPrimeraEstructura-> arreglo[0] = 3;

   // en main se emntra con . porque ya existe, con --> puntero se entra a elementos que cuando el main se creee se lleve toda la configuración y se la lleve a alla



	}

}
