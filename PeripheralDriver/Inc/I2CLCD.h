/*
 * I2CLCD.h
 *
 *  Created on: 4/11/2022
 *      Author: German
 */

#ifndef I2CLCDDRIVER_H_
#define I2CLCDDRIVER_H_


#include <stm32f4xx.h>



#define LCD_WRITE_DATA   0
#define LCD_READ_DATA    1

#define MAIN_CLOCK_4_MHz_FOR_LCD    4
#define MAIN_CLOCK_16_MHz_FOR_LCD  16
#define MAIN_CLOCK_20_MHz_FOR_LCD  20

#define LCD_MODE_SM   0
#define LCD_MODE_FM   1

#define LCD_MODE_SM_SPEED_100KHz   80
#define LCD_MODE_FM_SPEED_400KHz   14

#define LCD_MAX_RISE_TIME_SM   17
#define LCD_MAX_RISE_TIME_FM   6


typedef struct
{
	I2C_TypeDef *ptrLCDx;
	uint8_t slaveAddressLCD;
	uint8_t modeI2CLCD;
	uint16_t dataI2CLCD;

}LCD_Handler_t;

/* Prototipos de las funciones */

void LCD_Config (LCD_Handler_t *ptrHandlerLCD);
uint8_t LCD_ReadByte (LCD_Handler_t *ptrHandlerLCD,uint8_t memAddr);
void LCD_WriteByte (LCD_Handler_t *ptrHandlerLCD, uint8_t dataToWrite,uint8_t addres);

void LCD_Init (LCD_Handler_t *ptrHandlerLCD,uint8_t addres);
void LCD_sendCMD (LCD_Handler_t *ptrHandlerLCD, char cmd, uint8_t addres);
void LCD_sendata (LCD_Handler_t *ptrHandlerLCD, char data,uint8_t addres);
void LCD_sendSTR(LCD_Handler_t *ptrHandlerLCD, char *str,uint8_t addres);
void LCD_Clear(LCD_Handler_t *ptrHandlerLCD,uint8_t addres);
void LCD_setCursor(LCD_Handler_t *ptrHandlerLCD, uint8_t x, uint8_t y,uint8_t addres);
void delay_50 (void);
void delay_5 (void);
void delay_1 (void);
void delay_10 (void);


#endif /* I2CLCDDRIVER_H_ */
