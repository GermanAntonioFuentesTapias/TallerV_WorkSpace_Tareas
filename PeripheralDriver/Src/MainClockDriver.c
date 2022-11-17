/*
 * MainClockDriver.c
 *
 *  Created on: 14/11/2022
 *      Author: German
 *
 *      Configuración para cambiar la velocidad del reloj de 16 MHz a 100 MHz con velocidad de lectura
 *      en el pin A8 a 20MHz por implementos que se tienen en el laboratorio para la observación de esta velocidad
 *
 *
 */
#include  "MainClockDriver.h"



 // Para activar el HSION, no obstante este ya esta activo por lo que no es necesario tocar el registro

void ResetClock(void){

	// Se desactivan por precaución las interrupciones

	__disable_irq();

    /* PWR Regulator voltage scaling output selection */

	// Se escribe para que funcione Scale mode 1, para esto se debe tener desactivado el PLL

	RCC -> CR &= ~(PWR_CR_VOS);

	RCC -> CR |= (PWR_CR_VOS);

	// Se escribio un 0b11 (3) al bit 15-14 para esto
// RCC -> CR |= RCC_CR_HSION;



 // Primero se limpia el registro 4 Hz

	// Es necesario dar una latencia ya que iria muy rapido el micro y no habria tiempo de leer, por lo que se
	// hace necesario la aplicación de latencia

	/* Borramos */

 FLASH -> ACR &= ~(FLASH_ACR_LATENCY);

 // Ahora escribimos el necesario para la velocidad que queremos

 FLASH -> ACR |=  FLASH_ACR_LATENCY_3WS;


  /* Para el PLL es necesario configurar el P,M Y N que recibira la señal */

 // Se definio en un .h los valores para M, N Y P respectivamente que se quieren implementar

 // Se trabaja en el RCC_PLLCFGR, por lo tanto

  /* Para M */

  // Se limpia el registro de M
  RCC -> PLLCFGR &= ~RCC_PLLCFGR_PLLM;

  // Luege se escribe ,y como yo quiero un 8 por lo tanto

  RCC -> PLLCFGR |= RCC_PLLCFGR_PLLM_3;


  /* Para N */

  // Se limpia el registro de N

  RCC -> PLLCFGR &= ~RCC_PLLCFGR_PLLN;

  // Ahora lo escribo 100

  RCC -> PLLCFGR |= RCC_PLLCFGR_PLLN_2; //4
  RCC -> PLLCFGR |= RCC_PLLCFGR_PLLN_5; //32
  RCC -> PLLCFGR |= RCC_PLLCFGR_PLLN_6; //64

  // Se acomada de manera que se tiene se tiene que escribir 001100100 por lo que se procede a utilizar
  // las variables intentas asi que la suma de los 3 de sobre escriben en 100 que es el multiplicador
  // en el main del PLL entonces ya tengo que me dividio por 8, tengo 2, luego lo multiplico por 100
  // tengo 200 y me faltaria una división en la salida para obtener el 100 por tal motivo ->>

  /* Para P */

  //Se necesita deja un 00 por lo que niego el 3 (11) y obtengo el registro deseado, P = 2 , asi obtengo el
  // valor de P

  RCC -> PLLCFGR &= ~RCC_PLLCFGR_PLLP;


  /* Despues de configurado se activa el PLL */

  // NO SE PUEDE CONFIGURAR SI NO ESTA APAGADO, por eso se prende hasta ahoa

  RCC -> CR |= RCC_CR_PLLON;

  // Se carga el System clock switch
	RCC -> CFGR &= ~RCC_CFGR_SW;
	RCC -> CFGR |= RCC_CFGR_SW_PLL;

	// Se crea una while para verificación de SW

	   while( !(RCC -> CFGR & RCC_CFGR_SWS_1)){

		   __NOP();
	   }

  // Se configura System clock switch status
//  RCC -> CFGR &= ~RCC_CFGR_SWS;
//  RCC -> CFGR |= RCC_CFGR_SWS_PLL;


  // Se configuran los 3 buses necesarios y la salida para el pin A8
    RCC -> CFGR &= ~RCC_CFGR_HPRE;
  	RCC -> CFGR |= RCC_CFGR_HPRE_DIV1;

  	// APB Low speed prescaler (APB1)
  	RCC -> CFGR &= ~RCC_CFGR_PPRE1;
  	RCC -> CFGR |= RCC_CFGR_PPRE1_DIV2;

    //APB high-speed prescaler (APB2)
  	RCC -> CFGR &= ~RCC_CFGR_PPRE2;
  	RCC -> CFGR |= RCC_CFGR_PPRE2_DIV1;

   /* Para el pin a sacar por el micro se configura su Prescaler */

  	RCC -> CFGR |= RCC_CFGR_MCO1;

  	RCC -> CFGR &= ~RCC_CFGR_MCO1PRE;

  	RCC -> CFGR |= RCC_CFGR_MCO1PRE;

  	__enable_irq(); // Se activan las interrupciones

   /* Fin del Driver */
}

