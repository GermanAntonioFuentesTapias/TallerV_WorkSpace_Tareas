/*
 * MainClockDriver.c
 *
 *  Created on: 14/11/2022
 *      Author: German
 */
#include  "MainClockDriver.h"



 // Activar el HSION

void ResetClock(void){

	// Se desactivan por precaución

	__disable_irq();
 //Se activa HSI para escribir

	RCC -> CR &= ~(PWR_CR_VOS);

	RCC -> CR |= (PWR_CR_VOS);

// RCC -> CR |= RCC_CR_HSION;

 /* Se hace la configuración de latencia */

 // Primero se limpia el registro 4 Hz

// FLASH -> ACR &= ~(0xF << (FLASH_ACR_LATENCY_Pos));
 FLASH -> ACR &= ~(FLASH_ACR_LATENCY);

 // Ahora escribimos el necesario para la velocidad que queremos

 FLASH -> ACR |=  FLASH_ACR_LATENCY_3WS;


  /* Luego configuramos el P,M Y N que recibira la señal */

 // Se definio en un .h los valores para M, N Y P respectivamente

 // Se trabaja en el RCC_PLLCFGR, por lo tanto

  /* Para M */

  // Primero lo limpio
  RCC -> PLLCFGR &= ~RCC_PLLCFGR_PLLM;

  // Ahora lo escribo ,y yo quiero un 8 por lo tanto

  RCC -> PLLCFGR |= RCC_PLLCFGR_PLLM_3;


  /* Para N */

  //Primero lo limpio

  RCC -> PLLCFGR &= ~RCC_PLLCFGR_PLLN;

  // Ahora lo escribo 100

  RCC -> PLLCFGR |= RCC_PLLCFGR_PLLN_2;
  RCC -> PLLCFGR |= RCC_PLLCFGR_PLLN_5;
  RCC -> PLLCFGR |= RCC_PLLCFGR_PLLN_6;


  /* Para P */

  // Primero lo escribo

  RCC -> PLLCFGR &= ~RCC_PLLCFGR_PLLP;

  // Ahora lo escribo


  /* Despues de configurado se activa el PLL */

  // NO SE PUEDE CONFIGURAR SI NO ESTA APAGADO

  RCC -> CR |= RCC_CR_PLLON;

  // Se carga el System clock switch
	RCC -> CFGR &= ~RCC_CFGR_SW;
	RCC -> CFGR |= RCC_CFGR_SW_PLL;


	   while( !(RCC -> CFGR & RCC_CFGR_SWS_1)){

		   __NOP();
	   }

  // Se configura System clock switch status
  RCC -> CFGR &= ~RCC_CFGR_SWS;
  RCC -> CFGR |= RCC_CFGR_SWS_PLL;


  // Se configuran los 3 buses necesarios y la salida para el pin A8
  RCC -> CFGR &= ~RCC_CFGR_HPRE;
  	RCC -> CFGR |= RCC_CFGR_HPRE_DIV1;

  	// APB Low speed prescaler (APB1)
  	RCC -> CFGR &= ~RCC_CFGR_PPRE1;
  	RCC -> CFGR |= RCC_CFGR_PPRE1_DIV2;

    //APB high-speed prescaler (APB2)
  	RCC -> CFGR &= ~RCC_CFGR_PPRE2;
  	RCC -> CFGR |= RCC_CFGR_PPRE2_DIV1;

   /* Para el pin a sacar por el micro */


  	RCC -> CFGR |= RCC_CFGR_MCO1;

  	RCC -> CFGR &= ~RCC_CFGR_MCO1PRE;

  	RCC -> CFGR |= RCC_CFGR_MCO1PRE;

  	__enable_irq();

//   while( !(RCC -> CFGR & RCC_CFGR_SWS_1)){
//
//	   __NOP();
//   }

  // Despues de lo anterior se porce a habilitar PLL para eso nos dirigimos RCC_CR y se activa la configguración

//  RCC -> CR |= RCC_CR_PLLON;

//  while (!(RCC -> CR & RCC_CR_PLLRDY)){
//
// 	  __NOP();
//   }

  // Se escribre en SW un 2 debido a que se requiere seleccionar el PLL
//
//  RCC -> CFGR |= RCC_CFGR_SW_1;
//
//   while( !(RCC -> CFGR & RCC_CFGR_SWS_1)){
//
//	   __NOP();
//   }

 // PLLM  010000 en  osea 16

 // PLLN  110010000 se multiplica 400

 // PLLP se divide por  01: PLLP = 4


}

