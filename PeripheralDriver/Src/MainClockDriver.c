/*
 * MainClockDriver.c
 *
 *  Created on: 14/11/2022
 *      Author: German
 */
#include  "MainClockDriver.h"



 // Activar el HSION

void ResetClock(void){

 //Se activa HSI para escribir
 RCC -> CR |= RCC_CR_HSION;

 // Asi que mientras se activa esto es necesario

// RCC -> CR |= RCC_APB1ENR_PWREN;



 /* Se hace la configuración de latencia */

 // Primero se limpia el registro 4 Hz

 FLASH -> ACR &= ~(0xF << (FLASH_ACR_LATENCY_Pos));

 // Ahora escribimos el necesario para la velocidad que queremos

 FLASH -> ACR |=  FLASH_ACR_LATENCY_3WS;



  // Se configuran los 3 buses necesarios

  //HPRE: AHB prescaler
  RCC -> CFGR &= ~RCC_CFGR_HPRE;

  //APB high-speed prescaler (APB2)
  RCC -> CFGR &= ~RCC_CFGR_PPRE2; // No se divide por nada, por lo que se coloca un cero

  // APB Low speed prescaler (APB1)
  RCC -> CFGR |= RCC_CFGR_PPRE1_2;// Se escribe un 4


  // Configura la salida del Pin A8
  RCC -> CFGR |= RCC_CFGR_MCO1; // Se escribe un 3 para estar el modo PLL

  /* Luego configuramos el P,M Y N que recibira la señal */

 // Se definio en un .h los valores para M, N Y P respectivamente

 // Se trabaja en el RCC_PLLCFGR, por lo tanto

  /* Para M */

  // Primero lo limpio
  RCC -> PLLCFGR &= ~RCC_PLLCFGR_PLLM;

  // Ahora lo escribo ,y yo quiero un 8 por lo tanto

  RCC -> PLLI2SCFGR |= RCC_PLLCFGR_PLLM_3;

  /* Para N */

  //Primero lo limpio

  RCC -> PLLCFGR &= ~RCC_PLLCFGR_PLLN;

  // Ahora lo escribo

  RCC -> PLLCFGR |= RCC_PLLCFGR_PLLN_8;

  /* Para P */

  // Primero lo limpio

  RCC -> PLLCFGR &= ~RCC_PLLCFGR_PLLP;

  // Ahora lo escribo

  RCC -> PLLCFGR |= RCC_PLLCFGR_PLLP_1;


  RCC -> CFGR |= RCC_CFGR_SW_1;

//   while( !(RCC -> CFGR & RCC_CFGR_SWS_1)){
//
//	   __NOP();
//   }

  // Despues de lo anterior se porce a habilitar PLL para eso nos dirigimos RCC_CR y se activa la configguración

  RCC -> CR |= RCC_CR_PLLON;

  while (!(RCC -> CR & RCC_CR_PLLRDY)){

 	  __NOP();
   }

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
// RCC->CR |= CR
