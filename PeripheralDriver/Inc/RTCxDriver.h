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

#define LUNES		0x1
#define MARTES		0x2
#define MIERCOLES	0x3
#define JUEVES		0x4
#define VIERNES		0x5
#define SABADO		0x6
#define DOMINGO		0x7



typedef struct
{
  uint8_t RTC_Hours;    /*!< Valor especifico para horas.
                        Su operación estara de 0-12 pero si se cambia de formato
                        se tendra en 0-23 con militar, cambiando
                        RTC_HourFormat_12 a  RTC_HourFormat_24 is. */

  uint8_t RTC_Minutes;  /* Especifica los minutos */

  uint8_t RTC_Seconds;  /* Especifica los segundos */
  uint8_t RTC_H12;      /* Especifica AM/PM */

}RTC_TimeTyp;

typedef struct
{
  uint8_t RTC_WeekDay; /* Dia de semana.*/

  uint8_t RTC_Month;   /* Dia del mes*/

  uint8_t RTC_Date;     /*!< Fecha del mes 1-31 */

  uint8_t RTC_Year;     /* Rango del año */
}RTC_DateTyp;

typedef struct
{
	RTC_DateTyp		DateTypeDef;
	RTC_TimeTyp		    TimeTypeDef;

}RTC_handler_t;


void RTC_Config(RTC_handler_t *ptrRTCHandler);
uint8_t* read_date(void);

#endif /* RTCDRIVER_H_ */
