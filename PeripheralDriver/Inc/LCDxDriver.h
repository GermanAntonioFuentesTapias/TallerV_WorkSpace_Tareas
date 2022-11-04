/*
 * LCDxDriver.h
 *
 *  Created on: 2/11/2022
 *      Author: German
 */

#ifndef LCDXDRIVER_H_
#define LCDXDRIVER_H_


#include <stm32f4xx.h>

#define I2C_WRITE_DATA						0
#define I2C_READ_DATA						1

#define MAIN_CLOCK_4_MHz_FOR_I2C			4
#define MAIN_CLOCK_16_MHz_FOR_I2C			16
#define MAIN_CLOCK_20_MHz_FOR_I2C			20

#define I2C_MODE_SM			0
#define I2C_MODE_FM			1

#define I2C_MODE_SM_SPEED_100KHz			80
#define I2C_MODE_FM_SPEED_400KHz			14

#define I2C_MAX_RISE_TIME_SM				17
#define I2C_MAX_RISE_TIME_FM				6


typedef struct
{
	I2C_TypeDef		*ptrI2Cx;
	uint8_t			slaveAddress;
	uint8_t			modeI2C;
	uint8_t			dataI2C;
}I2CLCD_Handler_t;

/* Prototipos de las funciones públicas a utilizar */


void i2c_config_LCD (I2CLCD_Handler_t *ptrHandlerI2C);
uint8_t I2C_readByte (I2CLCD_Handler_t *ptrHandlerI2C, uint8_t memAddr);
void I2C_writeByte (I2CLCD_Handler_t *ptrHandlerI2C, uint8_t dataToWrite);
void LCD_Init(I2CLCD_Handler_t *ptrHandlerI2C);
void LCD_sendCMD (I2CLCD_Handler_t *ptrHandlerI2C, char cmd);
void LCD_sendata (I2CLCD_Handler_t *ptrHandlerI2C, char data);
void LCD_sendSTR(I2CLCD_Handler_t *ptrHandlerI2C, char *str);
void LCD_Clear (I2CLCD_Handler_t *ptrHandlerI2C);
void LCD_setCursor (I2CLCD_Handler_t *ptrHandlerI2C, uint8_t x, uint8_t y);
void delay_50 (void);
void delay_5 (void);
void delay_1 (void);
void delay_10 (void);


#endif /* LCDXDRIVER_H_ */
