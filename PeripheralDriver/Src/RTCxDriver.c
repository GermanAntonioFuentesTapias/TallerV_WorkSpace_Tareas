/*
 * RTCxDriver.c
 *
 *  Created on: 4/11/2022
 *      Author: German
 */

#include <RTCxDriver.h>

void RTC_Config(RTC_handler_t *ptrRtcHandler) {

	/*Se debe configurar el bit DBP de PWR_CR para habilitar el acceso de escritura a los registros RTC
	 * *******************************************************
	 *
	 * Se escribe el RTC register para su habilitación
	 *
	 * utilizando en RTC_WPR register los valores 0xCA , 0x53
	 *
	 */

	/*Configuración */

	/* 1 . Escribir la señal de reloj */
	RCC->APB1ENR |= RCC_APB1ENR_PWREN; // Se activa en el PWREN

	/* 2 . Debo apuntar el PWR al Control register y escribir un 1 */

	PWR->CR  |= PWR_CR_DBP; // Y se accede a RTC

    /* 3. Activar las configuraciones a utilizar del RCC */

	RCC->BDCR |= RCC_BDCR_RTCEN;		  //  Disponible cambio de clock
	RCC->BDCR |= RCC_BDCR_LSEON;		  // ACtivar el LSEON
	RCC->BDCR |= RCC_BDCR_RTCSEL_0;		  // Selección de cristal de interes

	/*4.  LSE se tiene que leer, se espera*/

	// Como la configuraciones deben tener una esperar se genera un while mientras se cumple
	//indicador en el registro de control de dominio RCC Backup (RCC_BDCR) indica si el
	//El cristal LSE es estable o no. Al inicio, la señal del reloj de salida de cristal LSE no se libera
	//hasta que este bit sea establecido por hardware.

	while(!(RCC->BDCR & RCC_BDCR_LSERDY)){
		__NOP() ;
	}
	/* 5. Después de restablecer el dominio de respaldo, todos los registros RTC están protegidos contra escritura. Escribiendo a la RTC
	registros se habilita escribiendo una clave en el registro de protección contra escritura,
	*/

	//6 . Se requieren los siguientes pasos para desbloquear la protección contra escritura en todos los registros RTC
	// excepto para RTC_ISR[13:8]

	RTC->WPR = (0xCA ); // Clave 1 RTC_WPR_KEY_Pos
	RTC->WPR = (0x53 ); // Clave 2 RTC_WPR_KEY_Pos


    /* 7  En la configuración se activa el ISR estableciendo el bit INIT*/

	/*7.1 activación de INIT */

	RTC->ISR |= RTC_ISR_INIT;			  // Iniciacilación con escritura de 1

	/* 7.2 verificación de que se Sondeo el bit INITF en el registro RTC_ISR.  Ya que Se entra en el modo de fase de inicialización
	 *  cuando INITF se establece en 1. Toma de 1 a 2 ciclos de reloj RTCCLK (debido a la sincronización).
	 */

	while (!(RTC->ISR & RTC_ISR_INITF)){  //Después que sean ambos sigue el programa
		__NOP() ;
	}

	/* 7.3  Factor de preescalador síncrono; El factor de división del preescalador asíncrono
    * se establece en 128, y el factor de división síncrona en 256, para obtener un
     * frecuencia de reloj interno de 1 Hz (ck_spre) con una frecuencia LSE de 32,768 kHz. */

	RTC->PRER |= RTC_PRER_PREDIV_A;
	RTC->PRER |= 0xFF << 0;

	/* Después de un reinicio del sistema, la aplicación puede leer el indicador INITS en el registro RTC_ISR para
    compruebar si el calendario se ha inicializado o no. Si esta bandera es igual a 0, el calendario no ha
    inicializado desde que el campo de año se estableció en su valor predeterminado de restablecimiento de dominio de respaldo (0x00).
    Para leer el calendario después de la inicialización, el software primero debe verificar que la bandera RSF esté configurada
    en el registro RTC_ISR. */

	/* 8. Asi que se cargara la hora y fecha inicial en RTC_TR, RTC_DT
	 * y se configura el formato de hora --->> a través del bit FMT en el registro RTC_CR activando arriba */

	/* 8.1  Inicialización  de tiempo y fecha*/

    RTC->CR |= RTC_CR_BYPSHAD; // pasar por alto la sombra de registros

	RTC->TR = 0; // Para el tiempo en cero
	RTC->DR = 0; // Para la fecha en cero

	/* 9 Declaración fecha --> Año */
	RTC->DR |= ((ptrRtcHandler->DateTypeDef.RTC_Ano) / 10) << RTC_DR_YT_Pos;  // Seran las decenas en Decimal codificado en binario
	RTC->DR |= ((ptrRtcHandler->DateTypeDef.RTC_Ano) % 10) << RTC_DR_YU_Pos;  // Seran las unidades  en Decimal codificado en binario

	/* 10 Declaración dia de la semana */

	RTC->DR |= ((ptrRtcHandler->DateTypeDef.RTC_Semana)) << RTC_DR_WDU_Pos;  // Los dias de la semana en Decimal codificado en binario

	/* 11. Declaración dias del mes */

	RTC->DR |= ((ptrRtcHandler->DateTypeDef.RTC_Mes) / 10) << RTC_DR_MT_Pos;  // Mes decenas en Decimal codificado en binario
	RTC->DR |= ((ptrRtcHandler->DateTypeDef.RTC_Mes) % 10) << RTC_DR_MU_Pos;  // Mes unidades en Decimal codificado en binario

	/* 12 . Declaración fecha */

	RTC->DR |= ((ptrRtcHandler->DateTypeDef.RTC_Fecha) / 10) << RTC_DR_DT_Pos;   // fecha decenas en Decimal codificado en binario
	RTC->DR |= ((ptrRtcHandler->DateTypeDef.RTC_Fecha) % 10) << RTC_DR_DU_Pos;   // fecha unidades en Decimal codificado en binario

	/* 13. Formato de hora */

	RTC->TR |= ((ptrRtcHandler->TimeTypeDef.RTC_H12) << RTC_TR_PM_Pos);  // Se tendra 0: 24h para formato militar y formato, 1: 12h normal

	/* 14. Declaración para horas */

	RTC->TR |= ((ptrRtcHandler->TimeTypeDef.RTC_Hora) / 10) << RTC_TR_HT_Pos;  // Horas  decenas en Decimal codificado en binario
	RTC->TR |= ((ptrRtcHandler->TimeTypeDef.RTC_Hora) % 10) << RTC_TR_HU_Pos;  // Horas  decenas  en Decimal codificado en binario

	/* 15 Declaración para minutos */

	RTC->TR |= ((ptrRtcHandler->TimeTypeDef.RTC_Minutos) / 10) << RTC_TR_MNT_Pos;  // Minutos  decena en Decimal codificado en binario
	RTC->TR |= ((ptrRtcHandler->TimeTypeDef.RTC_Minutos) % 10) << RTC_TR_MNU_Pos;  // Minutos  unidades en Decimal codificado en binario

	/* 16 . Declaración para segundos */

	RTC->TR |= ((ptrRtcHandler->TimeTypeDef.RTC_Segundos) / 10) << RTC_TR_ST_Pos;   // Segundos decenas en Decimal codificado en binario
	RTC->TR |= ((ptrRtcHandler->TimeTypeDef.RTC_Segundos) % 10) << RTC_TR_SU_Pos;   // Segundos unidades en Decimal codificado en binario

	/* 17. Salir del modo de inicialización borrando el bit de inicio*/

		RCC->BDCR |= RCC_BDCR_RTCEN; // Se activa con 1
		RTC->ISR &= ~RTC_ISR_INIT; //
//		Después de un reinicio del sistema, la aplicación puede leer el indicador INITS en el registro RTC_ISR para
//		compruebe si el calendario se ha inicializado o no. Si esta bandera es igual a 0
		PWR->CR &= ~ PWR_CR_DBP;

    /* 18. Se  da el pasar por alto la sombra
     registros a cero y se protege WPR */
	    RTC->CR &= ~RTC_CR_BYPSHAD;
		RTC->WPR = (0xFF); // Llave prottección lectura
}

uint8_t calendario [7] = {0};

uint8_t RTC_BcdToByte(uint16_t ByteAleer){

	uint16_t valor_bcd = ByteAleer;     			     // bcd_value = 55, o 0x37
	uint16_t Decenas = valor_bcd >> 4;                   // Decenas = 3 (Se shiftea 4 posiciones )
	uint16_t Unidades = valor_bcd & 0x0F;                // Unidades = 7 el 0x0F filtra el dígito alto)
	uint16_t valor_final = (Decenas * 10) + Unidades ;   // final_value = 37

	return valor_final;
}

uint8_t* leer_datos(void){



		 uint8_t RTC_Hora	 = 0;
		 uint8_t RTC_Minutos = 0;
		 uint8_t RTC_Segundos = 0;

		 uint8_t RTC_Semana= 0;

		 uint8_t RTC_Ano = 0;
		 uint8_t RTC_Mes = 0;
		 uint8_t RTC_Day = 0;

		 uint32_t RTC_Time = 0;
		 RTC_Time = RTC->TR;

		 uint32_t RTC_Fecha = 0;
		 RTC_Fecha = RTC->DR;

		 // Mascaras necesarias para asegurar que se desplaze a esa posición
		  RTC_Hora   = RTC_BcdToByte(((RTC_Time & 0x3F0000) >> RTC_TR_HU_Pos));
		  RTC_Minutos = RTC_BcdToByte(((RTC_Time & 0x007F00) >> RTC_TR_MNU_Pos));
		  RTC_Segundos = RTC_BcdToByte((RTC_Time  & 0x0000FF));

		  RTC_Semana  = RTC_BcdToByte(((RTC_Fecha & 0xE000)   >> 13));
		  RTC_Ano     = RTC_BcdToByte(((RTC_Fecha & 0xFF0000) >> 16));
		  RTC_Mes    = RTC_BcdToByte(((RTC_Fecha & 0x1F00)   >> 8));
		  RTC_Day 	   = RTC_BcdToByte((RTC_Fecha  & 0x3F));


		    calendario[0] = RTC_Segundos;
			calendario[1] = RTC_Minutos;
			calendario[2] = RTC_Hora;
		    calendario[3] = RTC_Semana;
		    calendario[4] = RTC_Day;
			calendario[5] = RTC_Mes;
			calendario[6] = RTC_Ano;


	return calendario;
}

void RTC_SetFecha(RTC_handler_t *ptrRTCHandler){


}

void RTC_UpdateFecha(RTC_handler_t *ptrRTCHandler, uint16_t New){


}


