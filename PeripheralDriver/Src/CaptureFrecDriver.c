/*
 * CaptureFrecDriver.c
 *
 *  Created on: 18/11/2022
 *      Author: German
 */

#include "CaptureFrecDriver.h"

#include <math.h>


void capture_Config (Capture_Handler_t *ptrCaptureHandler){

	// Se desactivan las interrupciones globales
	__disable_irq();

	/* 1. Activar la señal de reloj del periférico requerido */
	if (ptrCaptureHandler -> ptrTIMx == TIM2){
		RCC -> APB1ENR |= RCC_APB1ENR_TIM2EN;

	}

		else if (ptrCaptureHandler -> ptrTIMx == TIM3){
			RCC -> APB1ENR |= RCC_APB1ENR_TIM3EN;
		}

		else if (ptrCaptureHandler -> ptrTIMx == TIM4){
			RCC -> APB1ENR |= RCC_APB1ENR_TIM4EN;
		}

		else if (ptrCaptureHandler -> ptrTIMx == TIM5){
			RCC -> APB1ENR |= RCC_APB1ENR_TIM5EN;
		}

		else{
			__NOP();
		}

		/* 2a. Estamos en UP Mode, el límite se carga en ARR y se comienza en 0 */
		ptrCaptureHandler -> ptrTIMx -> CR1 &= ~ TIM_CR1_DIR;

		/* 3. Configuramos los bits CCxS del registro TIMy_CCMR1, de forma que sea modo entrada
		 * 	  (para cada canal hay un conjunto CCxS)
		 *
		 * 4. Además, en el mismo "case" podemos configurar el modo de la captura,
		 *
		 * 5. Y además activamos el preload bit, para que cada vez que
		 */

		switch (ptrCaptureHandler -> config.channel){

		case CAPTURE_CHANNEL_1:{
			// Borramos el posible valor cargado
			ptrCaptureHandler -> ptrTIMx -> CCMR1 &= ~(TIM_CCMR1_CC1S);
			// Configuramos como entrada en el mismo canal
			ptrCaptureHandler -> ptrTIMx -> CCMR1 |= (1 << TIM_CCMR1_CC1S_Pos);

			// Configuramos el filtro ( sin filtro)
			ptrCaptureHandler -> ptrTIMx -> CCMR1 &= ~(0xF << TIM_CCMR1_IC1F_Pos);
			ptrCaptureHandler -> ptrTIMx -> CCMR1 |= (0x3 << TIM_CCMR1_IC1F_Pos);

			// Configuramos el prescaler
			ptrCaptureHandler -> ptrTIMx -> CCMR1 &= ~(0x3 << TIM_CCMR1_IC1PSC_Pos);
			ptrCaptureHandler -> ptrTIMx -> CCMR1 |= (ptrCaptureHandler -> config.prescalerCapture << TIM_CCMR1_IC1PSC_Pos);

			// Configuramos el flanco que deseamos capturar
			if (ptrCaptureHandler -> config.edgeSignal == CAPTURE_RISING_EDGE){
				// Configuramos 00, para rising edge
				ptrCaptureHandler -> ptrTIMx -> CCER &= ~(TIM_CCER_CC1P);
				ptrCaptureHandler -> ptrTIMx -> CCER &= ~(TIM_CCER_CC1NP);
			}
			else if (ptrCaptureHandler -> config.edgeSignal == CAPTURE_FALLING_EDGE){
				// Configuramos 01, para falling edge
				ptrCaptureHandler -> ptrTIMx -> CCER |= (TIM_CCER_CC1P);
				ptrCaptureHandler -> ptrTIMx -> CCER &= ~(TIM_CCER_CC1NP);
			}
			else {
				// Configuramos 11 para ambos flancos
				ptrCaptureHandler -> ptrTIMx -> CCER |= (0x3 << TIM_CCER_CC1P_Pos);
				ptrCaptureHandler -> ptrTIMx -> CCER &= ~(TIM_CCER_CC1NP);
			}

			// Activamos el módilo de captura
			ptrCaptureHandler -> ptrTIMx -> CCER |= TIM_CCER_CC1E;


			// Activación una interrupción para el canal 1

			ptrCaptureHandler -> ptrTIMx -> DIER |= TIM_DIER_CC1IE;



			break;
		}

		case CAPTURE_CHANNEL_2:{
			// Borramos el posible valor cargado
			ptrCaptureHandler -> ptrTIMx -> CCMR1 &= ~(TIM_CCMR1_CC2S);
			// Configuramos como entrada en el mismo canal
			ptrCaptureHandler -> ptrTIMx -> CCMR1 |= (1 << TIM_CCMR1_CC2S_Pos);

			// Configuramos el filtro
			ptrCaptureHandler -> ptrTIMx -> CCMR1 &= ~(0xF << TIM_CCMR1_IC2F_Pos);
			ptrCaptureHandler -> ptrTIMx -> CCMR1 |= (0x3 << TIM_CCMR1_IC2F_Pos);

			// Configuramos el prescaler
			ptrCaptureHandler -> ptrTIMx -> CCMR1 &= ~(0x3 << TIM_CCMR1_IC2PSC_Pos);
			ptrCaptureHandler -> ptrTIMx -> CCMR1 |= (ptrCaptureHandler -> config.prescalerCapture << TIM_CCMR1_IC2PSC_Pos);

			// Configuramos el flanco que deseamos capturar
			if (ptrCaptureHandler -> config.edgeSignal == CAPTURE_RISING_EDGE){
				// Configuramos 00
				ptrCaptureHandler -> ptrTIMx -> CCER &= ~(TIM_CCER_CC2P);
				ptrCaptureHandler -> ptrTIMx -> CCER &= ~(TIM_CCER_CC2NP);
			}
			else if (ptrCaptureHandler -> config.edgeSignal == CAPTURE_FALLING_EDGE){
				// Configuramos 01 para falling edge
				ptrCaptureHandler -> ptrTIMx -> CCER |= (TIM_CCER_CC2P);
				ptrCaptureHandler -> ptrTIMx -> CCER &= ~ (TIM_CCER_CC2NP);
			}
			else {
				// Configuramos 11 para ambos flancos
				ptrCaptureHandler -> ptrTIMx -> CCER |= (0x3 << TIM_CCER_CC2P_Pos);
				ptrCaptureHandler -> ptrTIMx -> CCER &= ~(TIM_CCER_CC2NP);
			}

			// Activamos el módulo de captura
			ptrCaptureHandler -> ptrTIMx -> CCER |= TIM_CCER_CC2E;

			// Activación una interrupción para el canal 2

			ptrCaptureHandler -> ptrTIMx -> DIER |= TIM_DIER_CC2IE;

			break;
		}

		case CAPTURE_CHANNEL_3:{
			//Borramos el posible valor cargado
			ptrCaptureHandler -> ptrTIMx -> CCMR2 &= ~(TIM_CCMR2_CC3S);
			// Configuramos como entrada en el mismo canal
			ptrCaptureHandler -> ptrTIMx -> CCMR2 |= (1 << TIM_CCMR2_CC3S_Pos);

			// Configuramos el filtro (sin filtro)
			ptrCaptureHandler -> ptrTIMx -> CCMR2 &= ~(0xF << TIM_CCMR2_IC3F_Pos);
			ptrCaptureHandler -> ptrTIMx -> CCMR2 |= (0x3 << TIM_CCMR2_IC3F_Pos);

			// Configuramos el prescaler
			ptrCaptureHandler -> ptrTIMx -> CCMR2 &= ~(0x3 << TIM_CCMR2_IC3PSC_Pos);
			ptrCaptureHandler -> ptrTIMx -> CCMR2 |= (ptrCaptureHandler -> config.prescalerCapture << TIM_CCMR2_IC3PSC_Pos);

			// Configuramos el flanco que deseamos capturar
			if (ptrCaptureHandler -> config.edgeSignal == CAPTURE_RISING_EDGE){
				// Configuramos 00
				ptrCaptureHandler -> ptrTIMx -> CCER &= ~(TIM_CCER_CC3P);
				ptrCaptureHandler -> ptrTIMx -> CCER &= ~(TIM_CCER_CC3NP);
			}
			else if (ptrCaptureHandler -> config.edgeSignal == CAPTURE_FALLING_EDGE){
				// Configuramos 01 para falling
				ptrCaptureHandler -> ptrTIMx -> CCER |= (TIM_CCER_CC3P);
				ptrCaptureHandler -> ptrTIMx -> CCER &= ~(TIM_CCER_CC3NP);
			}
			else {
				// Configuramos 11 para ambos
				ptrCaptureHandler -> ptrTIMx -> CCER |= (0x3 << TIM_CCER_CC3P_Pos);
				ptrCaptureHandler -> ptrTIMx -> CCER &= ~(TIM_CCER_CC3NP);
			}

			// Activamos el módulo de captura
			ptrCaptureHandler -> ptrTIMx -> CCER |= TIM_CCER_CC3E;


			// Activación una interrupción para el canal 3

			ptrCaptureHandler -> ptrTIMx -> DIER |= TIM_DIER_CC3IE;

			break;

		}

		case CAPTURE_CHANNEL_4:{
			// Borramos el posible valor cargado
			ptrCaptureHandler -> ptrTIMx -> CCMR2 &= ~(TIM_CCMR2_CC4S);
			// Configuramos como entrada en el mismo canal
			ptrCaptureHandler -> ptrTIMx -> CCMR2 |= (1 << TIM_CCMR2_CC4S_Pos);

			// Configuramos el filtro
			ptrCaptureHandler -> ptrTIMx -> CCMR2 &= ~(0xF << TIM_CCMR2_IC4F_Pos);
			ptrCaptureHandler -> ptrTIMx -> CCMR2 |= (0x3 << TIM_CCMR2_IC4F_Pos);

			// Configuramos el prescaler
			ptrCaptureHandler -> ptrTIMx -> CCMR2 &= ~(0x3 << TIM_CCMR2_IC4PSC_Pos);
			ptrCaptureHandler -> ptrTIMx -> CCMR2 |= (ptrCaptureHandler -> config.prescalerCapture << TIM_CCMR2_IC4PSC_Pos);

			// Configuramos el flanco que deseamos capturar
			if (ptrCaptureHandler -> config.edgeSignal == CAPTURE_RISING_EDGE){
				// Configuramos 00
				ptrCaptureHandler -> ptrTIMx -> CCER &= ~(TIM_CCER_CC4P);
				ptrCaptureHandler -> ptrTIMx -> CCER &= ~(TIM_CCER_CC4NP);
			}
			else if (ptrCaptureHandler -> config.edgeSignal == CAPTURE_FALLING_EDGE){
				// Configuramos 01
				ptrCaptureHandler -> ptrTIMx -> CCER |= (TIM_CCER_CC4P);
				ptrCaptureHandler -> ptrTIMx -> CCER &= ~(TIM_CCER_CC4NP);
			}
			else{
				// Configuramos 11 para ambos
				ptrCaptureHandler -> ptrTIMx -> CCER |= (0x3 << TIM_CCER_CC4P_Pos);
				ptrCaptureHandler -> ptrTIMx -> CCER &= ~ (TIM_CCER_CC4NP);
			}

			// Activamos el módulo de captura
			ptrCaptureHandler -> ptrTIMx -> CCER |= TIM_CCER_CC4E;


			// Activación una interrupción para el canal 4

			ptrCaptureHandler -> ptrTIMx -> DIER |= TIM_DIER_CC4IE;


			break;
		}

		default:{
			__NOP();
			break;
		}

		} // Fin del switch-case


		// Configuramos el prescaler del timer, el cual define a que velocidad se incrementa nuestro timer
		ptrCaptureHandler -> ptrTIMx -> PSC = ptrCaptureHandler -> config.timerSpeed;

		/* Parte para que funcione con interrupciones*/


		/* 6. Activamos el canal del sistema NVIC para que lea la interrupción*/
			if(ptrCaptureHandler->ptrTIMx == TIM2){
				// Activando en NVIC para la interrupción del TIM2
				NVIC_EnableIRQ(TIM2_IRQn);
			}
			else if(ptrCaptureHandler->ptrTIMx == TIM3){
				// Activando en NVIC para la interrupción del TIM3
				NVIC_EnableIRQ(TIM3_IRQn); // Si se quiere para mas se va agregando mas, periferico usted va lanzar interrupción
			}
			else if(ptrCaptureHandler->ptrTIMx == TIM4){
					// Activando en NVIC para la interrupción del TIM3

				NVIC_EnableIRQ(TIM4_IRQn); // Si se quiere para mas se va agregando mas, periferico usted va lanzar interrupción
			}
			else if(ptrCaptureHandler->ptrTIMx == TIM5){
						// Activando en NVIC para la interrupción del TIM3
				NVIC_EnableIRQ(TIM5_IRQn); // Si se quiere para mas se va agregando mas, periferico usted va lanzar interrupción
			}
			else{
				__NOP();
			}

			ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;





		/* 7. Volvemos a activar las interrupciones del sistema */
			__enable_irq();




	}


	/* Esta función se encarga de lanzar la captura de la frecuencia. En este caso funciona como pulling,
	 * examinando cuando se levanta la bandera del evento de captura.
	 */

//	uint32_t getPeriodFreq (Capture_Handler_t *ptrCaptureHandler){
//
//		uint32_t timestamp1 = 0;
//		uint32_t timestamp2 = 0;
//		uint32_t deltaTimestamp = 0;
//
//		// Reiniciamos el contador del timer
//		ptrCaptureHandler -> ptrTIMx -> CNT = 0;
//
//		/* Todo lo que sigue depende del canal que se ha seleccionado, por lo cual se escribe cada
//		 * código dentro del "case" específico. Antes de lanzar la captura (encender el timer para
//		 * que cuente), debemos borrar el valor de los registro CCP, de forma que arranquen limpios
//		 */
//
//		switch (ptrCaptureHandler -> config.channel) {
//			case CAPTURE_CHANNEL_1:{
//				// Borramos el valor inicial del CCP
//				ptrCaptureHandler -> ptrTIMx -> CCR1 = 0;
//
//				// Bajamos la bandera que indica que existe un evento de captura
//				ptrCaptureHandler -> ptrTIMx -> SR &= ~TIM_SR_CC1IF;
//
//				// Encendemos el timer para que comience a contar
//				ptrCaptureHandler -> ptrTIMx -> CR1 |= TIM_CR1_CEN;
//
//				// Esperamos a que se de el primer evento
//				while (!(ptrCaptureHandler -> ptrTIMx -> SR & TIM_SR_CC1IF)){}
//
//				// Detenemos el timer
//				ptrCaptureHandler -> ptrTIMx -> CR1 &= ~TIM_CR1_CEN;
//
//				// Capturamos el valor del tiempo almacenando en el CCRx
//				timestamp1 = ptrCaptureHandler -> ptrTIMx -> CCR1;
//
//				// Bajamos la bandera que indica que existe un evento de captura
//				ptrCaptureHandler -> ptrTIMx -> SR &= ~TIM_SR_CC1IF;
//
//				// Encendemos el timer para que comience a contar
//				ptrCaptureHandler -> ptrTIMx -> CR1 |= TIM_CR1_CEN;
//
//				// Esperamos a que se de el primer evento
//				while (!(ptrCaptureHandler -> ptrTIMx -> SR & TIM_SR_CC1IF)){}
//
//				// Detenemos el timer
//				ptrCaptureHandler -> ptrTIMx -> CR1 &= ~TIM_CR1_CEN;
//
//				// Capturamos el valor del tiempo almacenado en el CCRx (sin reiniciar después de primera captura)
//				timestamp2 = ptrCaptureHandler -> ptrTIMx -> CCR1;
//
//				deltaTimestamp = timestamp2 - timestamp1;
//
//				break;
//			}
//
//			case CAPTURE_CHANNEL_2:{
//				// Borramos el valor inicial del CCP
//				ptrCaptureHandler -> ptrTIMx -> CCR2 = 0;
//
//				// Encendemos el timer para que comience a contar
//				ptrCaptureHandler -> ptrTIMx -> CR1 |= TIM_CR1_CEN;
//
//				// Bajamos la bandera que indica que existe un evento de captura
//				ptrCaptureHandler -> ptrTIMx -> SR &= ~TIM_SR_CC2IF;
//
//				// Esperamos a que se de el primer evento de captura
//				while (!(ptrCaptureHandler -> ptrTIMx -> SR & TIM_SR_CC2IF)){}
//
//				// Capturamos el valor del tiempo almacenado en el CCRx
//				timestamp1 = ptrCaptureHandler -> ptrTIMx -> CCR2;
//
//				// Bajamos la bandera que indica que existe un evento de captura
//				ptrCaptureHandler -> ptrTIMx -> SR &= ~TIM_SR_CC2IF;
//
//				// Esperamos a que se de el segundo evento
//				while (!(ptrCaptureHandler -> ptrTIMx -> SR & TIM_SR_CC2IF)){}
//
//				// Capturamos el valor del tiempo almacenado en el CCrx (sin haber reiniciado después de la primera captura)
//				timestamp2 = ptrCaptureHandler -> ptrTIMx -> CCR2;
//
//				deltaTimestamp = timestamp2 - timestamp1;
//
//				break;
//			}
//
//			case CAPTURE_CHANNEL_3: {
//				// Borramos el valor inicial del CCP
//				ptrCaptureHandler -> ptrTIMx -> CCR3 = 0;
//
//				// Encendemos el timer para que comience a contar
//				ptrCaptureHandler -> ptrTIMx -> CR1 |= TIM_CR1_CEN;
//
//				// Bajamos la bandera que indica que existe un evento de captura
//				ptrCaptureHandler -> ptrTIMx -> SR &= ~TIM_SR_CC3IF;
//
//				// Esperamos a que se de el primer evento
//				while (!(ptrCaptureHandler -> ptrTIMx -> SR & TIM_SR_CC3IF)){}
//
//				// Capturamos el valor del tiempo almacenado en el CCR3
//				timestamp1 = ptrCaptureHandler -> ptrTIMx -> CCR3;
//
//				// Bajamos la bandera que indica que existe un evento de captura
//				ptrCaptureHandler -> ptrTIMx -> SR &= ~TIM_SR_CC3IF;
//
//				// Esperamos a que se de el segundo evento
//				while (!(ptrCaptureHandler -> ptrTIMx -> SR & TIM_SR_CC3IF)){}
//
//				// Capturamos el valor del tiempo almacenado en el CCR3
//				timestamp2 = ptrCaptureHandler -> ptrTIMx -> CCR3;
//
//				deltaTimestamp = timestamp2 - timestamp1;
//
//				break;
//			}
//
//			case CAPTURE_CHANNEL_4: {
//				// Borramos el valor inicial del CCP
//				ptrCaptureHandler -> ptrTIMx -> CCR4 = 0;
//
//				// Encendemos el timer para que comience a contar
//				ptrCaptureHandler -> ptrTIMx -> CR1 |= TIM_CR1_CEN;
//
//				// Bajamos la bandera que indica que existe un evento de captura
//				ptrCaptureHandler -> ptrTIMx -> SR &= ~TIM_SR_CC4IF;
//
//				// Esperamos a que se de el primer evento
//				while (!(ptrCaptureHandler -> ptrTIMx -> SR & TIM_SR_CC4IF)){}
//
//				// Capturamos el valor del tiempo almacenado en el CCR4
//				timestamp1 = ptrCaptureHandler -> ptrTIMx -> CCR4;
//
//				// Bajamos la bandera que indica que existe un evento de captura
//				ptrCaptureHandler -> ptrTIMx -> SR &= ~TIM_SR_CC4IF;
//
//				// Esperamos a que se de el primer evento
//				while (!(ptrCaptureHandler -> ptrTIMx -> SR & TIM_SR_CC4IF)){}
//
//				// Capturamos el valor del tiempo almacenado en el CCR4
//				timestamp2 = ptrCaptureHandler -> ptrTIMx -> CCR4;
//
//				deltaTimestamp = timestamp2 - timestamp1;
//
//				break;
//			}
//
//			default:
//				break;
//		}
//
//		return deltaTimestamp;
//	}




	/*    P R U E B A */

	/* Función de remplazo del While  que nos inicia para cada canal*/

	uint32_t StartPeriod(Capture_Handler_t  *ptrCaptureHandler){

		uint32_t timestamp = 0;

		switch(ptrCaptureHandler->config.channel){
				case CAPTURE_CHANNEL_1:{

			// Capturamos el valor del tiempo almacenado en el CCR1
			timestamp = ptrCaptureHandler->ptrTIMx->CCR1;

			// Se baja la bandera que ocurrio un evento de captura
			ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC1OF; // Interrupción para canal 1

			break;
		}
		case CAPTURE_CHANNEL_2:{

			// Capturamos el valor del tiempo almacenado en el CCR2
			timestamp = ptrCaptureHandler->ptrTIMx->CCR2;

			// Se baja la bandera que ocurrio un evento de captura
			ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC2OF; // Interrupción para canal 2

			break;
		}
		case CAPTURE_CHANNEL_3:{

			// Capturamos el valor del tiempo almacenado en el CCR3
			timestamp = ptrCaptureHandler->ptrTIMx->CCR3;

			// Se baja la bandera que ocurrio un evento de captura
			ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC3OF; // Interrupción para canal 3
			break;
		}
		case CAPTURE_CHANNEL_4:{

			//Se baja la bandera que ocurrio un evento de captura
			ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC4IF;

			// Capturamos el valor del tiempo almacenado en el CCR4
			timestamp = ptrCaptureHandler->ptrTIMx->CCR4;

			// Bajamos la bandera del overcapture
			ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC4OF; // Interrupción para canal 4
			break;
		}
		default: {
			break;
		}
	}

	return timestamp;

		}

 /*  Función operativa para la captura de datos */

	// Se tiene la configuración del PWM que nos traera un periodo determinado y nosotros vamos a calcular cual es, por lo que necesitamos
	// traer los datos de los eventos y almacenarlos en las variables Capturas.

	uint32_t Period_Frecuen_Get(Capture_Handler_t  *ptrCaptureHandler, uint32_t Captura1, uint32_t Captura2){

		// Las capturas vendran por lo tanto el dato operado sera almacenado en RawData

		int RawData =Captura2 - Captura1; // La segunda interrupción por la primera

		RawData = fabs(RawData); // Se hace el absoluto para garantizar la toma positiva


		/* Casos de cambio de Prescaler en el  main por parte del usuario */

		switch(ptrCaptureHandler->config.prescalerCapture){

		case CAPTURE_PRESCALER_4_1:{

		RawData = RawData / (4); // Por la división del prescaler

			break;

		}

		case CAPTURE_PRESCALER_1_1:{

		RawData = RawData / (1); // Por la división del prescaler

					break;


		}

		case CAPTURE_PRESCALER_2_1:{

		RawData = RawData / (2); // Por la división del prescaler

					break;
        }

		case CAPTURE_PRESCALER_8_1:{

		RawData = RawData / (8); // Por la división del prescaler

					break;

		}

		default: {
						break;
					}

		return RawData;



		}

		/* Función encargada de velocidad */

		switch(ptrCaptureHandler->config.timerSpeed){

		// Se observa que cambia desde Micro hasta mili

			case CAPTURE_TIMER_SPEED_1us:{

				RawData = RawData* 1000;
				break;
			}
			case CAPTURE_TIMER_SPEED_10us:{

				RawData = RawData * 100;
				break;
			}
			case CAPTURE_TIMER_SPEED_100us:{

				RawData = RawData * 10;
				break;
			}
			case CAPTURE_TIMER_SPEED_1ms:{

				RawData = RawData;
				break;
			}
			default: {
				break;
			}

			}

		return RawData;
	}

	/* Se encarga de limpiar los valores que llegan para la captura 1 */

void CleanCapture(Capture_Handler_t *ptrCaptureHandler){

	// Por lo que reiniciamos en Counter

	ptrCaptureHandler->ptrTIMx->CNT = 0;

	/* Se hace una configuracion para cada canal */

	switch(ptrCaptureHandler->config.channel){
		case CAPTURE_CHANNEL_1:{
			ptrCaptureHandler->ptrTIMx->CCR1 = 0;
			break;
		}
		case CAPTURE_CHANNEL_2:{
			ptrCaptureHandler->ptrTIMx->CCR2 = 0;
			break;
		}
		case CAPTURE_CHANNEL_3:{
			ptrCaptureHandler->ptrTIMx->CCR3 = 0;
			break;
		}
		case CAPTURE_CHANNEL_4:{
			ptrCaptureHandler->ptrTIMx->CCR4 = 0;
			break;
		}
		default: {
			break;
		}
	}
}

/* Call Back de frecuencias */

__attribute__((weak)) void capturefrecuencia1(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimerX_Callback could be implemented in the main file
	   */
	__NOP();
}

__attribute__((weak)) void capturefrecuencia2(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimerX_Callback could be implemented in the main file
	   */
	__NOP();
}

__attribute__((weak)) void capturefrecuencia3(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimerX_Callback could be implemented in the main file
	   */
	__NOP();
}

__attribute__((weak)) void capturefrecuencia4(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimerX_Callback could be implemented in the main file
	   */
	__NOP();
}




