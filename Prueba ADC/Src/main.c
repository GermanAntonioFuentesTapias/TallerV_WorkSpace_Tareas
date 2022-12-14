#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USART.h"
#include "AdcDriver.h"

/* Definición de los handlers necesarios */
GPIO_Handler_t handlerBlinkyPin     = {0};
GPIO_Handler_t handlerUserButton 	= {0};
GPIO_Handler_t handlerTx         	= {0};
GPIO_Handler_t handlerRx			= {0};

BasicTimer_Handler_t handlerBlinkyTimer = {0};

//Comunicación RS-232 con el PC,, ya habilitada en la board Nucleo
//Utiliza la conexión USB

USART_Handler_t handlerUsart2    = {0};
char bufferData[64]              = {0};
uint8_t rxData                   =   0;

// Adc Config
ADC_Config_t adcConfig = {0};
uint8_t adcData = 0;
bool adcIsComplete 	 = false;

/* Definición de Blinky*/
uint8_t estadoBlinky =  0;

/* Mensaje que se imprime */
char greetingMsg[] = "SOLO MILLOS LOKA\n";

/* Definición de la función initSystem*/
void initSystem (void);


int main(void){
	//Inicialización
	initSystem();

	//writeChar(&handlerUsart2,' ');

    /* Ciclo principal del programa*/
	while(1){

		GPIO_WritePin(&handlerBlinkyPin, estadoBlinky);

		if (rxData != '\0'){
			writeChar(&handlerUsart2, rxData);

			if (rxData == 'm'){

				writeMsg(&handlerUsart2,greetingMsg);
			}

			if (rxData == 's'){
				startSingleADC();
			}

			rxData = '\0';
		}

		/* Esta es una nueva bandera que corresponde a cuando se dio una nueva conversión ADC,
		 * entonces el sistema entra a este bloque if para dar un formato al dato y enviarlo
		 * por la comunicación serial.
		 */

		if(adcIsComplete == true){
			sprintf(bufferData, "ADC = %u \n\r", (unsigned int) adcData);
			writeMsg(&handlerUsart2, bufferData);
			adcIsComplete = false;
		}
	}

	//return 0;
}





/* Función encargada de configurar el sistema */
void initSystem(void){


	/* Handler para el PIN A5 */
	handlerBlinkyPin.pGPIOx									= GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber 			= PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;

	/* Se carga la configuración */
	GPIO_Config(&handlerBlinkyPin);

	/* Handler para el PIN C13 */ //Boton azul
	handlerUserButton.pGPIOx   										= GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber					= PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinOPType					= GPIO_OTYPE_PUSHPULL;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_PULLUP;
	handlerUserButton.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_MEDIUM;
	handlerUserButton.GPIO_PinConfig.GPIO_PinAltFunMode				= AF0;

	/* Se carga la configuración */
	GPIO_Config(&handlerUserButton);

	/* Handler para el PIN A2 */
	handlerTx.pGPIOx                       			= GPIOA;
	handlerTx.GPIO_PinConfig.GPIO_PinNumber			= PIN_2;				// Pin TX USART2
	handlerTx.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerTx.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerTx.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerTx.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerTx.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;					// Función alternativa USART1

	/* Se carga la configuración */
	GPIO_Config(&handlerTx); // Puerto de transmision

	/* Handler para el PIN A3 */
	handlerRx.pGPIOx                       			= GPIOA;
	handlerRx.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;				// Pin RX USART2
	handlerRx.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerRx.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerRx.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerRx.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerRx.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;					// Función alternativa USART1

	/* Se carga la configuración */ //COnfiguración de recepción
	GPIO_Config(&handlerRx);


	/* Handler para el Timer2 */
	handlerBlinkyTimer.ptrTIMx 								= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed				= BTIMER_SPEED_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period 				= 2500; 			// Periodo de 250ms
    //handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable     = USART_RX_INTERRUP_ENABLE;
	/* Se carga la configuración */
	BasicTimer_Config(&handlerBlinkyTimer);

	/* Handler para el USART2 */
	handlerUsart2.ptrUSARTx								= USART2;
	handlerUsart2.USART_Config.USART_baudrate			= USART_BAUDRATE_19200;
	handlerUsart2.USART_Config.USART_datasize			= USART_DATASIZE_8BIT;
	handlerUsart2.USART_Config.USART_mode				= USART_MODE_RXTX;
	handlerUsart2.USART_Config.USART_parity				= USART_PARITY_NONE;
	handlerUsart2.USART_Config.USART_stopbits			= USART_STOPBIT_1;
	handlerUsart2.USART_Config.USART_IntRx		    = USART_RX_INTERRUPT_ENABLE;

	/* Se carga la configuración */
	USART_Config(&handlerUsart2);

//	adcConfig.channel     			 = ADC_CHANNEL_0;
//	adcConfig.resolution  			 = ADC_RESOLUTION_12_BIT;
//	adcConfig.dataAlignment 		 = ADC_ALIGNMENT_RIGHT;
//	adcConfig.samplingPeriod		 = ADC_SAMPLING_PERIOD_28_CYCLES;
//
//	/* La interrupción está activada por defecto */
//	adc_Config(&adcConfig);
//
}

void BasicTimer2_Callback(void){

	estadoBlinky ^= 1;

}

void Usart2Rx_Callback(void){

	//Usart2Rx_CallBack();
	rxData = getRxData();
}

void adcComplete_CallBack(void){
	adcData = getADC();
	adcIsComplete  = true;
}


