/*
 * USARTxDriver.c
 *
 *  Created on: Apr 6, 2022
 *      Author: German Antonio Fuentes modificación de Nerio para Taller V
 */

#include <stm32f4xx.h>
#include "USARTxDriver.h"

uint8_t auxRxData = 0;
/**
 * Configurando el puerto Serial...
 * Recordar que siempre se debe comenzar con activar la señal de reloj
 * del periferico que se está utilizando.
 */
void USART_Config(USART_Handler_t *ptrUsartHandler){
	/* 1. Activamos la señal de reloj que viene desde el BUS al que pertenece el periferico */
	/* Lo debemos hacer para cada uno de las posibles opciones que tengamos (USART1, USART2, USART6) */
    /* 1.1 Configuramos el USART1 */
	if(ptrUsartHandler->ptrUSARTx == USART1){

		RCC ->APB2ENR |= RCC_APB2ENR_USART1EN;
	}
	
    /* 1.2 Configuramos el USART2 */
    
	if(ptrUsartHandler->ptrUSARTx == USART2){ // Nota deje el afan y mire el datasheet de que bus esta

			RCC ->APB1ENR |= RCC_APB1ENR_USART2EN;
		}

    /* 1.3 Configuramos el USART6 */

	if(ptrUsartHandler->ptrUSARTx == USART6){

			RCC ->APB2ENR |= RCC_APB2ENR_USART6EN;
		}


	/* 2. Configuramos el tamaño del dato, la paridad y los bit de parada */
	/* En el CR1 estan parity (PCE y PS) y tamaño del dato (M) */
	/* Mientras que en CR2 estan los stopbit (STOP)*/
	/* Configuracion del Baudrate (registro BRR) */
	/* Configuramos el modo: only TX, only RX, o RXTX */
	/* Por ultimo activamos el modulo USART cuando todo esta correctamente configurado */

	// 2.1 Comienzo por limpiar los registros, para cargar la configuración desde cero
	ptrUsartHandler->ptrUSARTx->CR1 = 0;
	ptrUsartHandler->ptrUSARTx->CR2 = 0;

	// 2.2 Configuracion del Parity:
	// Verificamos si el parity esta activado o no
    // Tenga cuidado, el parity hace parte del tamaño de los datos...
	if(ptrUsartHandler->USART_Config.USART_parity != USART_PARITY_NONE){ //Si es distinto a desactivado entonces -->

		ptrUsartHandler->ptrUSARTx->CR1   |= (USART_CR1_PCE); //Activando segun el resultado de paridad

		// Verificamos si se ha seleccionado ODD or EVEN
			if(ptrUsartHandler->USART_Config.USART_parity == USART_PARITY_EVEN){
				// Es even, entonces cargamos la configuracion adecuada

			 ptrUsartHandler->ptrUSARTx->CR1	&= ~(USART_CR1_PS); //

			 ptrUsartHandler->ptrUSARTx->CR1   |= (USART_CR1_M);

			}else{
				// Si es "else" significa que la paridad seleccionada es ODD, y cargamos esta configuracion

			ptrUsartHandler->ptrUSARTx->CR1 |= (USART_CR1_PS);

			ptrUsartHandler->ptrUSARTx->CR1 |= (USART_CR1_M);
			}
	}else{
		// Si llegamos aca, es porque no deseamos tener el parity-check
		// La desactivamos en Parity control enable CR1 bit 10

		ptrUsartHandler->ptrUSARTx->CR1 &= ~(USART_CR1_PCE);

		ptrUsartHandler->ptrUSARTx->CR1 &= ~ (USART_CR1_M);
	}

	// 2.3 Configuramos el tamaño del dato
    // Miramos es el bit 10

    if(ptrUsartHandler->USART_Config.USART_datasize == USART_DATASIZE_9BIT){
   // Se configuro asi porque asi viene del usuario, asi que se verifica que coloco el usuario
       ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_M; // Asi se coloca con peso 9
    }
       else{
    	   ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_M; // Seria 8 porque la configuración del usuario fue 8
       }


	// 2.4 Configuramos los stop bits (SFR USART_CR2)
	switch(ptrUsartHandler->USART_Config.USART_stopbits){
	case USART_STOPBIT_1: {
		// Debemos cargar el valor 0b00 en los dos bits de STOP

		ptrUsartHandler->ptrUSARTx->CR2 &= ~(USART_CR2_STOP);

		//Asi porque son 1 , 2 y 3 en binario
		break;
	}
	case USART_STOPBIT_0_5: {
		// Debemos cargar el valor 0b01 en los dos bits de STOP

		ptrUsartHandler->ptrUSARTx->CR2 |= (USART_CR2_STOP_0);

		// Se realiza Or porque es como escribiendo un 1 en esa posición
		break;
	}
	case USART_STOPBIT_2: {
		// Debemos cargar el valor 0b10 en los dos bits de STOP

		ptrUsartHandler->ptrUSARTx->CR2 |= (USART_CR2_STOP_1);

		// Se realiza or porque es 1 en es pos

		break;
	}
	case USART_STOPBIT_1_5: {
		// Debemos cargar el valor 0b11 en los dos bits de STOP

		ptrUsartHandler->ptrUSARTx->CR2 |= (USART_CR2_STOP);


		// Se realiza OR porque es 00 y estamos escribiendo eso ahi segun el valor de USART_CR2_STOP
		break;
	}
	default: {
		// En el caso por defecto seleccionamos 1 bit de parada

		ptrUsartHandler->ptrUSARTx->CR2 &= ~ (USART_CR2_STOP);

		// Se copia este porque por defecto se coloca si el usuario es muy dormido y no coloca nada el AND del inicio
		break;
	}
	}

	// 2.5 Configuracion del Baudrate (SFR USART_BRR)
	// Ver tabla de valores (Tabla 73), Frec = 16MHz, overr = 0;
	if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_9600){
		// El valor a cargar es 104.1875 -> Mantiza = 104,fraction = 0.1875
		// Mantiza = 104 = 0x68, fraction = 16 * 0.1875 = 3
		// Valor a cargar 0x0683
		// Configurando el Baudrate generator para una velocidad de 9600bps
		ptrUsartHandler->ptrUSARTx->BRR = 0x0683;
	}

	else if (ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_19200) {
		// El valor a cargar es 52.0625 -> Mantiza = 52,fraction = 0.0625
		// Mantiza = 52 = 0x34, fraction = 16 * 0.0625 = 1
		// Valor a cargar 0x0341
		// Configurando el Baudrata generator bara una velocidad de 19200

		ptrUsartHandler->ptrUSARTx->BRR = 0x0341;
	}

	else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_115200){
		// El valor a cargar es 8.6875 -> Mantiza = 8,fraction = 0.6875
		// Mantiza = 8 = 0x8, fraction = 16 * 0.6875 = 1
	    // Valor a cargar 0x08b
		// Configurando el Baudrata generator bara una velocidad de 115200
		ptrUsartHandler->ptrUSARTx->BRR = 0x08B;

		// Recordar que estamos en Hex
	}

	// 2.6 Configuramos el modo: TX only, RX only, RXTX, disable
	switch(ptrUsartHandler->USART_Config.USART_mode){
	case USART_MODE_TX:
	{
		// Activamos la parte del sistema encargada de enviar

		// Debemos prender la función alternativa

		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TE;

		//ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_UE;

		break;
	}
	case USART_MODE_RX:
	{
		// Activamos la parte del sistema encargada de recibir

		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_RE;

		//ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_UE;

		break;
	}
	case USART_MODE_RXTX:
	{
		// Activamos ambas partes, tanto transmision como recepcion
		// Activar el UE para luego TX y RE

		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_UE;

		// Prenderlos recepción
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_RE;

		// Transmisión

		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TE;
		break;
	}
	case USART_MODE_DISABLE:
	{
		// Desactivamos ambos canales

		// Apagar  recepción

		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_RE;

		// Apagar trasmisión

		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_TE;

		// Apagado UE

		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_UE;
		break;
	}
	
	default:
	{
		// Actuando por defecto, desactivamos ambos canales
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_RE;

		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_TE;

		//ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_UE;

		break;
	}
	}

	// 2.7 Activamos el modulo serial.
	if(ptrUsartHandler->USART_Config.USART_mode != USART_MODE_DISABLE){ // El modo activa el modulo serial para las comunicaciones

		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_UE;
	}

	//2.8 Interrupciones
	//Interrupción para recepción
	if(ptrUsartHandler->USART_Config.USART_EI_Rx == USART_RX_INTERRUP_ENABLE){
      // Como se afirmaria que esta activida se debe configurar la interrupción por recepción

		/*Tenemos que activar la interrupción en el USART*/

		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_RXNEIE;

		// Despues de esto se tiene que matricular en el NVIC

		if(ptrUsartHandler->ptrUSARTx == USART1){
		   __NVIC_EnableIRQ(USART1_IRQn);
		}
		else if(ptrUsartHandler->ptrUSARTx == USART2){
			__NVIC_EnableIRQ(USART2_IRQn);
		}
		else if(ptrUsartHandler->ptrUSARTx == USART6){
			__NVIC_EnableIRQ(USART6_IRQn);
		}

		else{
			__NOP();
		}

	}



} // Va dentro del USART_Config

/* funcion para escribir un solo char */
int writeChar(USART_Handler_t *ptrUsartHandler, int dataToSend ){
	while( !(ptrUsartHandler->ptrUSARTx->SR & USART_SR_TXE)){
		__NOP();
	} // Entonces si en SR se tiene negación, se tiene vacio, pero si
	// se encuentra un dato, seria true, por lo que si tiene algo, si debe escribir
	// en el DR y guardarse en dataToSend para ser enviado bit a bit

	ptrUsartHandler ->ptrUSARTx ->DR = dataToSend;

	return dataToSend;
}

/**/
void writeMsg(USART_Handler_t * ptrUsartHandler, char *msgToSend){
	while(*msgToSend != '\0'){
		writeChar(ptrUsartHandler, *msgToSend);
		msgToSend++;
	}

}

uint8_t getRxData(void){
	return auxRxData;
}

/* Handler de la interrupción del USART
 * Se asociana todas la interrupciones: TX, RX , PE...
 */


void USART2_IRQHandler(void){
	//Evaluamos si la interrupción que se dio es por RX
	if(USART2->SR & USART_SR_RXNE){
		auxRxData = (uint8_t) USART2->DR;
		Usart2Rx_CallBack();
	}
}

void USART1_IRQHandler(void){
	//Evaluamos si la interrupción que se dio es por RX
	if(USART1->SR & USART_SR_RXNE){
		auxRxData = (uint8_t) USART1->DR;
		Usart1Rx_CallBack();
	}
}

void USART6_IRQHandler(void){
	//Evaluamos si la interrupción que se dio es por RX
	if(USART6->SR & USART_SR_RXNE){
		auxRxData = (uint8_t) USART6->DR;
		Usart6Rx_CallBack();
	}
}

__attribute__((weak)) void Usart1Rx_CallBack(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the Usart1Rx could be implemented in the main file
	   */
	__NOP();
}

__attribute__((weak)) void Usart2Rx_CallBack(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the Usart1Rx could be implemented in the main file
	   */
	__NOP();
}

__attribute__((weak)) void Usart6Rx_CallBack(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the Usart1Rx could be implemented in the main file
	   */
	__NOP();
}

/* Asocianones de la interrupcion del USART*/


