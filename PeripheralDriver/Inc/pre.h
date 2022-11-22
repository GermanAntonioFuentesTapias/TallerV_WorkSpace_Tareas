/*
 * pre.h
 *
 *  Created on: 21/11/2022
 *      Author: German
 */

#ifndef PRE_H_
#define PRE_H_
#include <math.h>

/**/
/**/
void capture_Config(Capture_Handler_t *ptrCaptureHandler){

	__disable_irq();

	/* 1. Activar la señal de reloj del periférico requerido */
	if(ptrCaptureHandler->ptrTIMx == TIM2){
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM3){
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM4){
		RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM5){
		RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
	}
	else{
		__NOP();
	}

	/* 2. Estamos en UP Mode, el límite se carga en ARR y se comienza en 0 */
	ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_DIR;

	/* 3. Configuramos los bits CCxS del registro TIMy_CCMR1, de forma que sea modo salida
	 * (para cada canal hay un conjunto CCxS)
	 *
	 * 4. Además, en el mismo "case" podemos configurar el modo PWM, su polaridad.
	 *
	 * 5. Y además activamos el preload bit, para que cada vez que exista un update-event
	 * el valor cargado en el CCRx será recargado en el registro "shadow" del PWM */
	switch(ptrCaptureHandler->config.channel){
	case CAPTURE_CHANNEL_1:{
		// Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(TIM_CCMR1_CC1S);
		// Configuramos como entrada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR1 = (1 << TIM_CCMR1_CC1S_Pos);

		// Configuramos el filtro (sin filtro)
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0xF << TIM_CCMR1_IC1F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (0x3 << TIM_CCMR1_IC1F_Pos);

		// Configuramos el preescaler
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0x3 <<TIM_CCMR1_IC1PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (ptrCaptureHandler->config.prescalerCapture << TIM_CCMR1_IC1PSC_Pos);

		// Configuramos el flanco que deseamos capturar
		if(ptrCaptureHandler->config.edgeSignal == CAPTURE_RISING_EDGE){
			// Configuración 00, para rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC1P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC1NP);
		}
		else{
			// Configuración 01, para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= (TIM_CCER_CC1P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC1NP);
		}

		// Activamos el modulo captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC1E;

		// Activamos las interrupciones para el modo Capture/Compare del TIMx
		ptrCaptureHandler->ptrTIMx->DIER |= TIM_DIER_CC1IE;

		break;
	}

	case CAPTURE_CHANNEL_2:{
		// Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(TIM_CCMR1_CC2S);
		// Configuramos como entrada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR1 = (1 << TIM_CCMR1_CC2S_Pos);

		// Configuramos el filtro (sin filtro)
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0xF << TIM_CCMR1_IC2F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (0x3 << TIM_CCMR1_IC2F_Pos);

		// Configuramos el preescaler
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0x3 <<TIM_CCMR1_IC2PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (ptrCaptureHandler->config.prescalerCapture << TIM_CCMR1_IC2PSC_Pos);

		// Configuramos el flanco que deseamos capturar
		if(ptrCaptureHandler->config.edgeSignal == CAPTURE_RISING_EDGE){
			// Configuración 00, para rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC2P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC2NP);
		}else{
			// Configuración 01, para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= (TIM_CCER_CC2P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC2NP);
		}

		// Activamos el modulo captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC2E;

		// Activamos las interrupciones para el modo Capture/Compare del TIMx
		ptrCaptureHandler->ptrTIMx->DIER |= TIM_DIER_CC2IE;

		break;
	}

	case CAPTURE_CHANNEL_3:{
		// Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(TIM_CCMR2_CC3S);
		// Configuramos como entrada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR2 = (1 << TIM_CCMR2_CC3S_Pos);

		// Configuramos el filtro (sin filtro)
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(0xF << TIM_CCMR2_IC3F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR2 |= (0x3 << TIM_CCMR2_IC3F_Pos);

		// Configuramos el preescaler
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(0x3 <<TIM_CCMR2_IC3PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR2 |= (ptrCaptureHandler->config.prescalerCapture << TIM_CCMR2_IC3PSC_Pos);

		// Configuramos el flanco que deseamos capturar
		if(ptrCaptureHandler->config.edgeSignal == CAPTURE_RISING_EDGE){
			// Configuración 00, para rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC3P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC3NP);
		}else{
			// Configuración 01, para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= (TIM_CCER_CC3P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC3NP);
		}

		// Activamos el modulo captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC3E;

		// Activamos las interrupciones para el modo Capture/Compare del TIMx
		ptrCaptureHandler->ptrTIMx->DIER |= TIM_DIER_CC3IE;

		break;
	}

	case CAPTURE_CHANNEL_4:{
		// Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(TIM_CCMR2_CC4S);
		// Configuramos como entrada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR2 = (1 << TIM_CCMR2_CC4S_Pos);

		// Configuramos el filtro (sin filtro)
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(0xF << TIM_CCMR2_IC4F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR2 |= (0x3 << TIM_CCMR2_IC4F_Pos);

		// Configuramos el preescaler
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(0x3 <<TIM_CCMR2_IC4PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR2 |= (ptrCaptureHandler->config.prescalerCapture << TIM_CCMR2_IC4PSC_Pos);

		// Configuramos el flanco que deseamos capturar
		if(ptrCaptureHandler->config.edgeSignal == CAPTURE_RISING_EDGE){
			// Configuración 00, para rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC4P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC4NP);
		}else{
			// Configuración 01, para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= (TIM_CCER_CC4P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC4NP);
		}

		// Activamos el modulo captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC4E;

		// Activamos las interrupciones para el modo Capture/Compare del TIMx
		ptrCaptureHandler->ptrTIMx->DIER |= TIM_DIER_CC4IE;

		break;
	}


	default: {
		__NOP();
		break;
	}

	}// fin del switch-case

	// Configuramos el preescaler del timer, el cual define a que velocidad se incrementa nuestro timer
	ptrCaptureHandler->ptrTIMx->PSC = ptrCaptureHandler->config.timerSpeed;

	/* Activamos el canal del sistema NVIC para que lea la interrupción*/
	if(ptrCaptureHandler->ptrTIMx == TIM2){
		// Activando en NVIC para la interrupción del TIM2
		NVIC_EnableIRQ(TIM2_IRQn);
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM3){
		// Activando en NVIC para la interrupción del TIM3
		NVIC_EnableIRQ(TIM3_IRQn);
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM4){
		// Activando en NVIC para la interrupción del TIM4
		NVIC_EnableIRQ(TIM4_IRQn);
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM5){
		// Activando en NVIC para la interrupción del TIM5
		NVIC_EnableIRQ(TIM5_IRQn);
	}
	else{
		__NOP();
	}

	ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

	__enable_irq();
}

__attribute__((weak)) void CaptureFrec1_Callback(void){

	__NOP();
}
__attribute__((weak)) void CaptureFrec2_Callback(void){

	__NOP();
}
__attribute__((weak)) void CaptureFrec3_Callback(void){

	__NOP();
}
__attribute__((weak)) void CaptureFrec4_Callback(void){

	__NOP();
}


/* Esta función se encarga de lanzar la captura de la frecuencia... en este caso funciona
 * con pulling, examinando cuando se levanta la bandera del evento de captura.
 */
uint32_t getTimeStamp(Capture_Handler_t *ptrCaptureHandler){

	uint32_t timestamp = 0;

	/* Todo lo que sigue de aca en adelante depende del canal que se ha seleccionado,
	 * por lo cual se escribe cada código dentro del "case" específico.
	 * Antes de lanzar la captura (encender el Timer para que cuente),
	 * debemos borrar el valor de los registros CCP, de forma que arranquen limpios
	 */
	switch(ptrCaptureHandler->config.channel){
		case CAPTURE_CHANNEL_1:{

			// Capturamos el valor del tiempo almacenado en el CCRx
			timestamp = ptrCaptureHandler->ptrTIMx->CCR1;

			// Bajamos la bandera del overcapture
			ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC1OF;

			break;
		}
		case CAPTURE_CHANNEL_2:{

			// Capturamos el valor del tiempo almacenado en el CCRx
			timestamp = ptrCaptureHandler->ptrTIMx->CCR2;

			// Bajamos la bandera del overcapture
			ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC2OF;

			break;
		}
		case CAPTURE_CHANNEL_3:{

			// Capturamos el valor del tiempo almacenado en el CCRx
			timestamp = ptrCaptureHandler->ptrTIMx->CCR3;

			// Bajamos la bandera del overcapture
			ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC3OF;
			break;
		}
		case CAPTURE_CHANNEL_4:{

			// Bajamos la bandera que indica que existe un evento de captura
			ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC4IF;

			// Capturamos el valor del tiempo almacenado en el CCRx
			timestamp = ptrCaptureHandler->ptrTIMx->CCR4;

			// Bajamos la bandera del overcapture
			ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC4OF;
			break;
		}
		default: {
			break;
		}
	}

	return timestamp;

}

uint32_t getPeriodFrec(Capture_Handler_t *ptrCaptureHandler, uint32_t ts1, uint32_t ts2){


	int rawPeriod = ts2 - ts1;
	rawPeriod = abs(rawPeriod);


	rawPeriod = rawPeriod / (8);

	switch(ptrCaptureHandler->config.timerSpeed){
	case CAPTURE_TIMER_SPEED_1us:{

		rawPeriod = rawPeriod * 1000;
		break;
	}
	case CAPTURE_TIMER_SPEED_10us:{

		rawPeriod = rawPeriod * 100;
		break;
	}
	case CAPTURE_TIMER_SPEED_100us:{

		rawPeriod = rawPeriod * 10;
		break;
	}
	case CAPTURE_TIMER_SPEED_1ms:{

		rawPeriod = rawPeriod;
		break;
	}
	default: {
		break;
	}

	}

	return rawPeriod;


}

void limpiar(Capture_Handler_t *ptrCaptureHandler){

	ptrCaptureHandler->ptrTIMx->CNT = 0;

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



#endif /* PRE_H_ */



/*************************************************/

void capture_Config(Capture_Handler_t *ptrCaptureHandler){

	__disable_irq();

	/* 1. Activar la señal de reloj del periférico requerido */
	if(ptrCaptureHandler->ptrTIMx == TIM2){
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM3){
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM4){
		RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM5){
		RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
	}
	else{
		__NOP();
	}

	/* 2. Estamos en UP Mode, el límite se carga en ARR y se comienza en 0 */
	ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_DIR;

	/* 3. Configuramos los bits CCxS del registro TIMy_CCMR1, de forma que sea modo salida
	 * (para cada canal hay un conjunto CCxS)
	 *
	 * 4. Además, en el mismo "case" podemos configurar el modo PWM, su polaridad.
	 *
	 * 5. Y además activamos el preload bit, para que cada vez que exista un update-event
	 * el valor cargado en el CCRx será recargado en el registro "shadow" del PWM */
	switch(ptrCaptureHandler->config.channel){
	case CAPTURE_CHANNEL_1:{
		// Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(TIM_CCMR1_CC1S);
		// Configuramos como entrada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR1 = (1 << TIM_CCMR1_CC1S_Pos);

		// Configuramos el filtro (sin filtro)
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0xF << TIM_CCMR1_IC1F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (0x3 << TIM_CCMR1_IC1F_Pos);

		// Configuramos el preescaler
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0x3 <<TIM_CCMR1_IC1PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (ptrCaptureHandler->config.prescalerCapture << TIM_CCMR1_IC1PSC_Pos);

		// Configuramos el flanco que deseamos capturar
		if(ptrCaptureHandler->config.edgeSignal == CAPTURE_RISING_EDGE){
			// Configuración 00, para rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC1P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC1NP);
		}
		else{
			// Configuración 01, para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= (TIM_CCER_CC1P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC1NP);
		}

		// Activamos el modulo captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC1E;

		// Activamos las interrupciones para el modo Capture/Compare del TIMx
		ptrCaptureHandler->ptrTIMx->DIER |= TIM_DIER_CC1IE;

		break;
	}

	case CAPTURE_CHANNEL_2:{
		// Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(TIM_CCMR1_CC2S);
		// Configuramos como entrada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR1 = (1 << TIM_CCMR1_CC2S_Pos);

		// Configuramos el filtro (sin filtro)
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0xF << TIM_CCMR1_IC2F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (0x3 << TIM_CCMR1_IC2F_Pos);

		// Configuramos el preescaler
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0x3 <<TIM_CCMR1_IC2PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (ptrCaptureHandler->config.prescalerCapture << TIM_CCMR1_IC2PSC_Pos);

		// Configuramos el flanco que deseamos capturar
		if(ptrCaptureHandler->config.edgeSignal == CAPTURE_RISING_EDGE){
			// Configuración 00, para rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC2P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC2NP);
		}else{
			// Configuración 01, para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= (TIM_CCER_CC2P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC2NP);
		}

		// Activamos el modulo captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC2E;

		// Activamos las interrupciones para el modo Capture/Compare del TIMx
		ptrCaptureHandler->ptrTIMx->DIER |= TIM_DIER_CC2IE;

		break;
	}

	case CAPTURE_CHANNEL_3:{
		// Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(TIM_CCMR2_CC3S);
		// Configuramos como entrada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR2 = (1 << TIM_CCMR2_CC3S_Pos);

		// Configuramos el filtro (sin filtro)
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(0xF << TIM_CCMR2_IC3F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR2 |= (0x3 << TIM_CCMR2_IC3F_Pos);

		// Configuramos el preescaler
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(0x3 <<TIM_CCMR2_IC3PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR2 |= (ptrCaptureHandler->config.prescalerCapture << TIM_CCMR2_IC3PSC_Pos);

		// Configuramos el flanco que deseamos capturar
		if(ptrCaptureHandler->config.edgeSignal == CAPTURE_RISING_EDGE){
			// Configuración 00, para rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC3P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC3NP);
		}else{
			// Configuración 01, para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= (TIM_CCER_CC3P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC3NP);
		}

		// Activamos el modulo captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC3E;

		// Activamos las interrupciones para el modo Capture/Compare del TIMx
		ptrCaptureHandler->ptrTIMx->DIER |= TIM_DIER_CC3IE;

		break;
	}

	case CAPTURE_CHANNEL_4:{
		// Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(TIM_CCMR2_CC4S);
		// Configuramos como entrada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR2 = (1 << TIM_CCMR2_CC4S_Pos);

		// Configuramos el filtro (sin filtro)
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(0xF << TIM_CCMR2_IC4F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR2 |= (0x3 << TIM_CCMR2_IC4F_Pos);

		// Configuramos el preescaler
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(0x3 <<TIM_CCMR2_IC4PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR2 |= (ptrCaptureHandler->config.prescalerCapture << TIM_CCMR2_IC4PSC_Pos);

		// Configuramos el flanco que deseamos capturar
		if(ptrCaptureHandler->config.edgeSignal == CAPTURE_RISING_EDGE){
			// Configuración 00, para rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC4P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC4NP);
		}else{
			// Configuración 01, para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= (TIM_CCER_CC4P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC4NP);
		}

		// Activamos el modulo captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC4E;

		// Activamos las interrupciones para el modo Capture/Compare del TIMx
		ptrCaptureHandler->ptrTIMx->DIER |= TIM_DIER_CC4IE;

		break;
	}


	default: {
		__NOP();
		break;
	}

	}// fin del switch-case

	// Configuramos el preescaler del timer, el cual define a que velocidad se incrementa nuestro timer
	ptrCaptureHandler->ptrTIMx->PSC = ptrCaptureHandler->config.timerSpeed;

	/* Activamos el canal del sistema NVIC para que lea la interrupción*/
	if(ptrCaptureHandler->ptrTIMx == TIM2){
		// Activando en NVIC para la interrupción del TIM2
		NVIC_EnableIRQ(TIM2_IRQn);
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM3){
		// Activando en NVIC para la interrupción del TIM3
		NVIC_EnableIRQ(TIM3_IRQn);
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM4){
		// Activando en NVIC para la interrupción del TIM4
		NVIC_EnableIRQ(TIM4_IRQn);
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM5){
		// Activando en NVIC para la interrupción del TIM5
		NVIC_EnableIRQ(TIM5_IRQn);
	}
	else{
		__NOP();
	}

	ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

	__enable_irq();
}

__attribute__((weak)) void CaptureFrec1_Callback(void){

	__NOP();
}
__attribute__((weak)) void CaptureFrec2_Callback(void){

	__NOP();
}
__attribute__((weak)) void CaptureFrec3_Callback(void){

	__NOP();
}
__attribute__((weak)) void CaptureFrec4_Callback(void){

	__NOP();
}


/* Esta función se encarga de lanzar la captura de la frecuencia... en este caso funciona
 * con pulling, examinando cuando se levanta la bandera del evento de captura.
 */
uint32_t getTimeStamp(Capture_Handler_t *ptrCaptureHandler){

	uint32_t timestamp = 0;

	/* Todo lo que sigue de aca en adelante depende del canal que se ha seleccionado,
	 * por lo cual se escribe cada código dentro del "case" específico.
	 * Antes de lanzar la captura (encender el Timer para que cuente),
	 * debemos borrar el valor de los registros CCP, de forma que arranquen limpios
	 */
	switch(ptrCaptureHandler->config.channel){
		case CAPTURE_CHANNEL_1:{

			// Capturamos el valor del tiempo almacenado en el CCRx
			timestamp = ptrCaptureHandler->ptrTIMx->CCR1;

			// Bajamos la bandera del overcapture
			ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC1OF;

			break;
		}
		case CAPTURE_CHANNEL_2:{

			// Capturamos el valor del tiempo almacenado en el CCRx
			timestamp = ptrCaptureHandler->ptrTIMx->CCR2;

			// Bajamos la bandera del overcapture
			ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC2OF;

			break;
		}
		case CAPTURE_CHANNEL_3:{

			// Capturamos el valor del tiempo almacenado en el CCRx
			timestamp = ptrCaptureHandler->ptrTIMx->CCR3;

			// Bajamos la bandera del overcapture
			ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC3OF;
			break;
		}
		case CAPTURE_CHANNEL_4:{

			// Bajamos la bandera que indica que existe un evento de captura
			ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC4IF;

			// Capturamos el valor del tiempo almacenado en el CCRx
			timestamp = ptrCaptureHandler->ptrTIMx->CCR4;

			// Bajamos la bandera del overcapture
			ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC4OF;
			break;
		}
		default: {
			break;
		}
	}

	return timestamp;

}

uint32_t getPeriodFrec(Capture_Handler_t *ptrCaptureHandler, uint32_t ts1, uint32_t ts2){


	int rawPeriod = ts2 - ts1;
	rawPeriod = abs(rawPeriod);


	rawPeriod = rawPeriod / (8);

	switch(ptrCaptureHandler->config.timerSpeed){
	case CAPTURE_TIMER_SPEED_1us:{

		rawPeriod = rawPeriod * 1000;
		break;
	}
	case CAPTURE_TIMER_SPEED_10us:{

		rawPeriod = rawPeriod * 100;
		break;
	}
	case CAPTURE_TIMER_SPEED_100us:{

		rawPeriod = rawPeriod * 10;
		break;
	}
	case CAPTURE_TIMER_SPEED_1ms:{

		rawPeriod = rawPeriod;
		break;
	}
	default: {
		break;
	}

	}

	return rawPeriod;


}

void limpiar(Capture_Handler_t *ptrCaptureHandler){

	ptrCaptureHandler->ptrTIMx->CNT = 0;

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



//include <stdint.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include "BasicTimer.h"
//#include "GPIOxDriver.h"
//#include "USARTxDriver.h"
//#include "PwmDriver.h"
//#include "CaptureFrecDriver.h"

// Definimos los handler para los perifericos necesarios en el proyecto

USART_Handler_t 				handlerUSART2			= {0};

BasicTimer_Handler_t 	        handlerTimer2 			= {0};

GPIO_Handler_t 					handlerBlinkyLed 		= {0};
GPIO_Handler_t					handlerTxPin			= {0};
GPIO_Handler_t					handlerRxPin			= {0};
GPIO_Handler_t					handlerSenalPin			= {0};

GPIO_Handler_t					handlerSenalCapture 	= {0};

Capture_Handler_t				handlerCapture          = {0};
PWM_Handler_t 					handlerPwm 				= {0};

// Definimos las variables que utilizaremos

uint8_t flagStatus 										= 0;
uint8_t flagCap 										= 0;
uint8_t rxData	 										= 0;
uint8_t capCounter 										= 0;
uint32_t ts1 											= 0;
uint32_t ts2		 									= 0;

char bufferData[64] 									= {0};

uint32_t periodoMelo									= 0;

// Definimos las funciones que vamos a utilizar:

//Funcion para inicializar las configuraciones del sistema

void InitSystem(void);

// Funcion principal del programa

int main(void)
{
	InitSystem();

	/* Ciclo infinito del main */
	while(1){


		// Condicional para la interrupcion del blinky
		if(flagStatus){
			flagStatus = 0;
			GPIOxTogglePin(&handlerBlinkyLed);
		}

		if(flagCap){
			if(capCounter){
				ts2 = getTimeStamp(&handlerCapture);
				capCounter = 0;
				periodoMelo = getPeriodFrec(&handlerCapture, ts1, ts2);
				sprintf(bufferData, "\n\r %u , %u ", (unsigned int) ts1, (unsigned int) ts2);

				writeMsg(&handlerUSART2, bufferData);

				sprintf(bufferData, "\n\r %u", (unsigned int) periodoMelo);

				writeMsg(&handlerUSART2, bufferData);
			}else{
				limpiar(&handlerCapture);
				ts1 = getTimeStamp(&handlerCapture);
				capCounter = 1;
				}
			flagCap = 0;
		}

		if (rxData != '\0'){
			//imprimimos el caracter recibido
			writeChar(&handlerUSART2, rxData);
			if (rxData == 'm'){
				//presentamos un mensaje
				sprintf(bufferData, "\n\r %u", (unsigned int) periodoMelo);

				writeMsg(&handlerUSART2, bufferData);
			}
			rxData = '\0';
		}
	}

	return 0;
}

void InitSystem(void){

	handlerBlinkyLed.pGPIOx = GPIOA;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinNumber 				= PIN_5;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_OUT;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinOPType 				= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEED_FAST;
	handlerBlinkyLed.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinkyLed);

	handlerTxPin.pGPIOx = GPIOA;
	handlerTxPin.GPIO_PinConfig.GPIO_PinNumber 					= PIN_2;
	handlerTxPin.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_ALTFN;
	handlerTxPin.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
	handlerTxPin.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerTxPin.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	handlerTxPin.GPIO_PinConfig.GPIO_PinAltFunMode				= AF7;
	GPIO_Config(&handlerTxPin);

	handlerRxPin.pGPIOx = GPIOA;
	handlerRxPin.GPIO_PinConfig.GPIO_PinNumber 					= PIN_3;
	handlerRxPin.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_ALTFN;
	handlerRxPin.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
	handlerRxPin.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerRxPin.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	handlerRxPin.GPIO_PinConfig.GPIO_PinAltFunMode				= AF7;
	GPIO_Config(&handlerRxPin);

	handlerTimer2.ptrTIMx 										= TIM2;
	handlerTimer2.TIMx_Config.TIMx_mode 						= BTIMER_MODE_UP;
	handlerTimer2.TIMx_Config.TIMx_speed						= BTIMER_SPEED_100us;
	handlerTimer2.TIMx_Config.TIMx_period						= 2500;
	handlerTimer2.TIMx_Config.TIMx_interruptEnable 				= 1;
	BasicTimer_Config(&handlerTimer2);

	handlerUSART2.ptrUSARTx 									= USART2;
	handlerUSART2.USART_Config.USART_mode 						= USART_MODE_RXTX;
	handlerUSART2.USART_Config.USART_baudrate 					= USART_BAUDRATE_115200;
	handlerUSART2.USART_Config.USART_datasize 					= USART_DATASIZE_8BIT;
	handlerUSART2.USART_Config.USART_parity 					= USART_PARITY_NONE;
	handlerUSART2.USART_Config.USART_stopbits             		= USART_STOPBIT_1;
	handlerUSART2.USART_Config.USART_enableIntRX          		= USART_RX_INTERRUP_ENABLE;
	USART_Config(&handlerUSART2);

	handlerSenalPin.pGPIOx = GPIOB;
	handlerSenalPin.GPIO_PinConfig.GPIO_PinNumber 			= PIN_6;
	handlerSenalPin.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	handlerSenalPin.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerSenalPin.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerSenalPin.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	handlerSenalPin.GPIO_PinConfig.GPIO_PinAltFunMode		= AF2;
	GPIO_Config(&handlerSenalPin);


	handlerSenalCapture.pGPIOx = GPIOA;
	handlerSenalCapture.GPIO_PinConfig.GPIO_PinNumber 			= PIN_6;
	handlerSenalCapture.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	handlerSenalCapture.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerSenalCapture.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEED_FAST;
	handlerSenalCapture.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	handlerSenalCapture.GPIO_PinConfig.GPIO_PinAltFunMode		= AF2;
	GPIO_Config(&handlerSenalCapture);

	// PWM
	handlerPwm.config.channel		= PWM_CHANNEL_1;
	handlerPwm.config.prescaler 	= 1599;
	handlerPwm.config.periodo 		= 200;
	handlerPwm.config.duttyCicle 	= 25;
	handlerPwm.ptrTIMx = TIM4;
	pwm_Config(&handlerPwm);
	startPwmSignal(&handlerPwm);

	handlerCapture.ptrTIMx										= TIM3;
	handlerCapture.config.channel								= CAPTURE_CHANNEL_1;
	handlerCapture.config.edgeSignal							= CAPTURE_RISING_EDGE;
	handlerCapture.config.prescalerCapture						= CAPTURE_PREESCALER_8_1;
	handlerCapture.config.timerSpeed							= CAPTURE_TIMER_SPEED_1ms;
	capture_Config(&handlerCapture);


}

void BasicTimer2_Callback(void){
	flagStatus = 1;
}

void usart2Rx_Callback(void){
	rxData = getRxData();
}


void CaptureFrec1_Callback(void){
	flagCap = 1;
}



