/*
 * RTCxDriver.h
 *
 *  Created on: 4/11/2022
 *      Author: German Fuentes
 */

#ifndef RTCDRIVER_H_
#define RTCDRIVER_H_

#include "stm32f4xx.h"

#define time_formato_24	0
#define time_formato_12	1

#define Mon		1
#define Tues    2
#define Wedn	3
#define Thus    4
#define Frid	5
#define Satur   6
#define Sund	7



typedef struct
{
  uint8_t RTC_Hora;    /*!< Valor especifico para horas.
                        Su operación estara de 0-12 pero si se cambia de formato
                        se tendra en 0-23 con militar, cambiando
                        RTC_HourFormat_12 a  RTC_HourFormat_24 is. */

  uint8_t RTC_Minutos;  /* Especifica los minutos */

  uint8_t RTC_Segundos;  /* Especifica los segundos */

  uint8_t RTC_H12;      /* Especifica AM/PM */

}RTC_TimeTyp;

typedef struct
{
  uint8_t RTC_Semana; /* Dia de semana.*/

  uint8_t RTC_Mes;   /* Dia del mes*/

  uint8_t RTC_Fecha;     /*!< Fecha del mes 1-31 */

  uint8_t RTC_Ano;     /* Rango del año */
}RTC_FechaTp;

typedef struct
{
	RTC_FechaTp		    DateTypeDef;
	RTC_TimeTyp		    TimeTypeDef;

}RTC_handler_t;


void RTC_Config(RTC_handler_t *ptrRTCHandler);
uint8_t* leer_datos(void);

#endif /* RTCDRIVER_H_ */
