/*
 * USARTxDriver.c
 *
 *  Created on: 7 Septiembre, 2022
 *      Author: German Fuentes
 */



#include <stm32f4xx.h>
#include "USARTxDriver.h"

/**
 * Configurando el puerto Serial...
 * Recordar que siempre se debe comenzar con activar la señal de reloj
 * del periferico que se está utilizando.
 */
uint8_t auxRxData = 0;

void USART_Config(USART_Handler_t *ptrUsartHandler){

	/* Desactivamos las interrupciones globales mientras configuramos el sistema */
	__disable_irq();

	/* 1. Activamos la señal de reloj que viene desde el BUS al que pertenece el periferico */
	/* Lo debemos hacer para cada uno de las pisbles opciones que tengamos (USART1, USART2, USART6) */
    /* 1.1 Configuramos el USART1 */
	if(ptrUsartHandler->ptrUSARTx == USART1){
		RCC -> APB2ENR |= RCC_APB2ENR_USART1EN;
	}
	
    /* 1.2 Configuramos el USART2 */
	else if (ptrUsartHandler -> ptrUSARTx == USART2){
    	RCC -> APB1ENR |= RCC_APB1ENR_USART2EN;
    }
    
    /* 1.3 Configuramos el USART6 */
	else if (ptrUsartHandler -> ptrUSARTx == USART6){
    	RCC -> APB2ENR |= RCC_APB2ENR_USART6EN;
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
	if(ptrUsartHandler->USART_Config.USART_parity != USART_PARITY_NONE){

		// Verificamos si se ha seleccionado ODD or EVEN
		if(ptrUsartHandler->USART_Config.USART_parity == USART_PARITY_EVEN){
			// Es even, entonces cargamos la configuracion adecuada
			ptrUsartHandler -> ptrUSARTx -> CR1 |= USART_CR1_PCE;
			ptrUsartHandler -> ptrUSARTx -> CR1 &= ~USART_CR1_PS;
			ptrUsartHandler -> ptrUSARTx -> CR1 |= USART_CR1_M;

		}else{
			// Si es "else" significa que la paridad seleccionada es ODD, y cargamos esta configuracion
			ptrUsartHandler -> ptrUSARTx -> CR1 |= USART_CR1_PCE;
			ptrUsartHandler -> ptrUSARTx -> CR1 |= USART_CR1_PS;
			ptrUsartHandler -> ptrUSARTx -> CR1 |= USART_CR1_M;
		}
	}else{
		// Si llegamos aca, es porque no deseamos tener el parity-check
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~USART_CR1_PCE;
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~USART_CR1_M;

	}

	// 2.3 Configuramos el tamaño del dato

	if(ptrUsartHandler->USART_Config.USART_datasize == USART_DATASIZE_8BIT ){

		ptrUsartHandler -> ptrUSARTx -> CR1 &=  ~USART_CR1_M;

	}

	else{ // Se carga la configuracion adecuada para tamaño de 9 bits.
		ptrUsartHandler -> ptrUSARTx -> CR1 |= USART_CR1_M;

	}

	// 2.4 Configuramos los stop bits (SFR USART_CR2)
	switch(ptrUsartHandler->USART_Config.USART_stopbits){
	case USART_STOPBIT_1: {
		// Debemos cargar el valor 0b00 en los dos bits de STOP
		ptrUsartHandler -> ptrUSARTx -> CR2 &= ~ USART_CR2_STOP;
		break;
	}
	case USART_STOPBIT_0_5: {
		// Debemoscargar el valor 0b01 en los dos bits de STOP
		ptrUsartHandler -> ptrUSARTx -> CR2 |= USART_CR2_STOP_0;
		break;
	}
	case USART_STOPBIT_2: {
		// Debemos cargar el valor 0b10 en los dos bits de STOP
		ptrUsartHandler -> ptrUSARTx -> CR2 |= USART_CR2_STOP_1;
		break;
	}
	case USART_STOPBIT_1_5: {
		// Debemos cargar el valor 0b11 en los dos bits de STOP
		ptrUsartHandler -> ptrUSARTx -> CR2 |= USART_CR2_STOP;
		break;
	}
	default: {
		// En el caso por defecto seleccionamos 1 bit de parada
		ptrUsartHandler -> ptrUSARTx -> CR2 &= ~USART_CR2_STOP;
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
		// Mantiza = 52 = 0x34, fraction = 16 * 0.1875 = 1
		ptrUsartHandler -> ptrUSARTx -> BRR = 0x0341;
	}

	else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_115200){
		// El valor a cargar es 8.6875 -> Mantiza = 8, fraction = 0.6875
		// Mantiza = 8 = 8, fraction = 0.6875*16= 11 = B
		// valor a cargar 0x0811
		ptrUsartHandler -> ptrUSARTx -> BRR = 0x08B;
	}

	// 2.6 Configuramos el modo: TX only, RX only, RXTX, disable
	switch(ptrUsartHandler->USART_Config.USART_mode){
	case USART_MODE_TX:
	{
		// Activamos la parte del sistema encargada de enviar
//		ptrUsartHandler -> ptrUSARTx -> CR1 |= USART_CR1_UE;
		ptrUsartHandler -> ptrUSARTx -> CR1 |= USART_CR1_TE;
		break;
	}
	case USART_MODE_RX:
	{
		// Activamos la parte del sistema encargada de recibir
//		ptrUsartHandler -> ptrUSARTx -> CR1 |= USART_CR1_UE;
		ptrUsartHandler -> ptrUSARTx -> CR1 |= USART_CR1_RE;
		break;
	}
	case USART_MODE_RXTX:
	{
		// Activamos ambas partes, tanto transmision como recepcion
//		ptrUsartHandler -> ptrUSARTx -> CR1 |= USART_CR1_UE;
		ptrUsartHandler -> ptrUSARTx -> CR1 |= USART_CR1_RE;
		ptrUsartHandler -> ptrUSARTx -> CR1 |= USART_CR1_TE;

		break;
	}
	case USART_MODE_DISABLE:
	{
		// Desactivamos ambos canales
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~USART_CR1_RE;
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~USART_CR1_TE;
		ptrUsartHandler->  ptrUSARTx ->CR1  &= ~USART_CR1_UE;
		break;
	}
	
	default:
	{
		// Actuando por defecto, desactivamos ambos canales
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~USART_CR1_RE;
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~USART_CR1_TE;
		ptrUsartHandler -> ptrUSARTx ->CR1  &= ~USART_CR1_UE;
		break;
	}
	}

	/* Verificamos la configuración de las interrupciones */
	/* 2.8 Interrupción por recepción */

	if (ptrUsartHandler -> USART_Config.USART_IntRx == USART_RX_INTERRUPT_ENABLE){
		/* Como está activada configuramos la interrupción por recepción */
		/* Debemos activar la interrupción RX en la configuración del USART */

		ptrUsartHandler -> ptrUSARTx -> CR1 |= USART_CR1_RXNEIE;

		/* Debemos matricular la interrupción en el NVIC */
		/* Para el USART 1 : */
		if(ptrUsartHandler -> ptrUSARTx == USART1){
			__NVIC_EnableIRQ(USART1_IRQn);
		}

		else if (ptrUsartHandler -> ptrUSARTx == USART2){
			__NVIC_EnableIRQ(USART2_IRQn);
		}

		else if (ptrUsartHandler -> ptrUSARTx == USART6){
			__NVIC_EnableIRQ(USART6_IRQn);
		}
	}

	else{
		__NOP();
	}


	// 2.7 Activamos el modulo serial.
	if(ptrUsartHandler->USART_Config.USART_mode != USART_MODE_DISABLE){
		ptrUsartHandler -> ptrUSARTx -> CR1 |= USART_CR1_UE;
	}

	/* X. Volvemos a activar las interrupciones del sistema */

	__enable_irq();

}

/* funcion para escribir un solo char */
int writeChar(USART_Handler_t *ptrUsartHandler, int dataToSend ){
	while( !(ptrUsartHandler->ptrUSARTx->SR & USART_SR_TXE)){
		__NOP();
	}

	/* Se carga en el registro DataRegister el valor de la variable dataToSend */
	ptrUsartHandler->ptrUSARTx->DR = dataToSend;

	return dataToSend;
}

/**/
void writeMsg(USART_Handler_t *ptrUsartHandler, char *msgToSend){
	while (*msgToSend != '\0'){
		writeChar(ptrUsartHandler, *msgToSend);
		msgToSend++;
	}
}

uint8_t getRxData(void){
	return auxRxData;
}


__attribute__((weak)) void USART2Rx_CallBack(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the USARTx_Callback could be implemented in the main file
	   */
	__NOP();
}

__attribute__((weak)) void USART1Rx_CallBack(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the USARTx_Callback could be implemented in the main file
	   */
	__NOP();
}

__attribute__((weak)) void USART6Rx_CallBack(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the USARTx_Callback could be implemented in the main file
	   */
	__NOP();
}

void USART1_IRQHandler(void){
	/* Evaluamos la interrupción que se dio por RX */
	if(USART1 -> SR & USART_SR_RXNE){
		auxRxData = (uint8_t) USART1 -> DR;
		USART1Rx_CallBack();
	}
}


void USART2_IRQHandler(void){
	/* Evaluamos la interrupción que se dio por RX */
	if(USART2 -> SR & USART_SR_RXNE){
		auxRxData = (uint8_t) USART2 -> DR;
		USART2Rx_CallBack();
	}
}

void USART6_IRQHandler(void){
	/* Evaluamos la interrupción que se dio por RX */
	if(USART6 -> SR & USART_SR_RXNE){
		auxRxData = (uint8_t) USART6 -> DR;
		USART6Rx_CallBack();
	}
}


