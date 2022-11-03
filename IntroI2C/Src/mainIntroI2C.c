/**
 ******************************************************************************
 * @file           : main.c
 * @author         : German Antonio Fuentes Tapias
 * @Mail           : gafuentest@unal.edu.co
 * @Resum          : Elaboración de la tarea 4 Taller V
 *
 *
 */

#include <stm32f4xx.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "BasicTimer.h"
#include "I2CDriver.h"


/* Definición de variables */

GPIO_Handler_t handlerLedOK = {0};
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};

BasicTimer_Handler_t handlerStateOKTimer = {0};

USART_Handler_t handlerCommTerminal = {0};
uint8_t rxData = 0;
//char bufferData[64] = "esto es una pequeña prueba";

char bufferData[64] =  {0};

uint32_t systemTicks = 0;
uint32_t systemTicksStart = 0;
uint32_t systemTicksEnd = 0;

/* Configuración para el I2C */

GPIO_Handler_t handlerI2cSDA = {0};
GPIO_Handler_t handlerI2cSCL = {0};
I2C_Handler_t  handlerAccelerometer = {0};
uint8_t i2cBuffer = 0;

#define ACCEL_ADDRESS 0b1101001; // 0xD2 -> Dirección del Accel con Logic_1 dirección creeria
#define ACCEL_XOUT_H  59        // 0x3B
#define ACCEL_XOUT_L  60        // 0x3C
#define ACCEL_YOUT_H  61        // 0x3D
#define ACCEL_YOUT_L  62        // 0x3E
#define ACCEL_ZOUT_H  63        // 0x3F
#define ACCEL_ZOUT_L  64        // 0x40

#define PWR_MGMT_1    107
#define WHO_AM_I      175


/* Definicion de prototipos de funciones */

void initSystem(void);

/*  Funcion principal del programa */

int main (void)


{

	// Llamamos a la función que nos inicialia el hadware del sistema

	initSystem();

	/* Main Loop */

	while(1){

		// Hacemos un "eco" con el valor que nos llega por el serial

		if(rxData != '\0'){
			writeChar(&handlerCommTerminal, rxData);

			if(rxData == 'd'){

				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);
				sprintf(bufferData, "dataRead = 0x2x \n", (unsigned int) i2cBuffer);				//sprintf(bufferData , " dataRead = 0x2x \n" , (unsigned int) i2cBuffer );
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}

			else if(rxData 'p'){
				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, PWR_MGMT_1);
				sprintf(bufferData , "dataRead = 0x%2x \n ", (unsigned int ) i2cBuffer);
				writeMsg(&handlerCommTerminal, bufferData);
			    rxData = '\0';
			}

			else if(rxData == 'r'){
				i2c_writeSingleRegister(&handlerAccelerometer, PWR_MGMT_1, 0x000);
				rxData = '\0';
			}

			else if( rxData == 'x'){
				uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
				uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
				int16_t AccelX = AccelX_high << 8 | AccelX_low;
				sprintf(bufferData, "AccelX = d \n", (int) AccelX);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';


			}

			else if(rxData == 'y'){
				uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
				uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
				sprintf(bufferData, " AccelY = %d \n", (int) AccelY);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';

			}

			else if(rxData == 'z'){

				uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
				uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
				int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
				sprintf(bufferData, "AccelZ = %d \n", (int) AccelZ);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}
			else {
				rxData = '\0';
			}
		}
	}
}

// minuto 1:31 :)
