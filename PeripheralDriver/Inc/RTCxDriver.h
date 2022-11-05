/*
 * RTCxDriver.h
 *
 *  Created on: 4/11/2022
 *      Author: German Fuentes
 */

#ifndef RTCDRIVER_H_
#define RTCDRIVER_H_

#include "stm32f4xx.h"

#define time_format_24	0
#define time_format_12	1

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

  uint8_t RTC_Minutes;  /*!< Specifies the RTC Time Minutes.
                        This parameter must be set to a value in the 0-59 range. */

  uint8_t RTC_Seconds;  /*!< Specifies the RTC Time Seconds.
                        This parameter must be set to a value in the 0-59 range. */

  uint8_t RTC_H12;      /*!< Specifies the RTC AM/PM Time. */

}RTC_TimeTypeDef;

typedef struct
{
  uint8_t RTC_WeekDay; /*!< Specifies the RTC Date WeekDay.*/

  uint8_t RTC_Month;   /*!< Specifies the RTC Date Month (in BCD format) */

  uint8_t RTC_Date;     /*!< Specifies the RTC Date.
                        This parameter must be set to a value in the 1-31 range. */

  uint8_t RTC_Year;     /*!< Specifies the RTC Date Year.
                        This parameter must be set to a value in the 0-99 range. */
}RTC_DateTypeDef;

typedef struct
{
	RTC_DateTypeDef		DateTypeDef;
	RTC_TimeTypeDef		TimeTypeDef;

}RTC_handler_t;


void RTC_Config(RTC_handler_t *ptrRTCHandler);
uint8_t* read_date(void);

#endif /* RTCDRIVER_H_ */
