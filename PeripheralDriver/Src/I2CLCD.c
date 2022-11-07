/*
 * I2CLCD.c
 *
 *  Created on: 4/06/2022
 *      Author: CamiloÁlvarez
 */


#include <stdint.h>
#include "I2CLCD.h"

/*
 * Se debe configurar los pines para el I2C (SDA y SCL),
 * para lo cual se necesita el modulo GPIO y los pines configurados
 * en el modo Alternate Function.
 * Además, estos pines deben ser configurados como salidas open-drain
 * y con las resistencias en modo pull-up
 */

void I2C_ConfigLCD (I2C_HandlerLCD_t *ptrHandlerI2CLCD){

	/* 1. Activamos la señal de reloj para el modulo I2C seleccionado */
	if (ptrHandlerI2CLCD -> ptrI2Cx == I2C1){
		RCC -> APB1ENR |= RCC_APB1ENR_I2C1EN;
	}
	else if(ptrHandlerI2CLCD ->ptrI2Cx== I2C2){
		RCC -> APB1ENR |= RCC_APB1ENR_I2C2EN;
	}
	else if (ptrHandlerI2CLCD -> ptrI2Cx == I2C3){
		RCC -> APB1ENR |= RCC_APB1ENR_I2C3EN;
	}

	/* 2. Reiniciamos el periférico, de forma que inicia en un estado conocido */
	ptrHandlerI2CLCD -> ptrI2Cx -> CR1 |= I2C_CR1_SWRST;
	__NOP();
	ptrHandlerI2CLCD -> ptrI2Cx -> CR1 &= ~ I2C_CR1_SWRST;

	/* 3. Indicamos cual es la velocidad del reloj principal, que es la señal utilizada
	 * por el periférico para generar la señal de reloj para el bus I2C */
	ptrHandlerI2CLCD-> ptrI2Cx -> CR2 &= ~(0b111111 << I2C_CR2_FREQ_Pos); // Borramos la configuración
	ptrHandlerI2CLCD-> ptrI2Cx -> CR2 |= (MAIN_CLOCK_16_MHz_FOR_I2C<< I2C_CR2_FREQ_Pos);

	/* Configuramos el modo I2C en el que el sistema funciona.
	 * En esta configuración se incluye también la velocidad del reloj
	 * y el tiempo máximo para el cambio de señal (T-Rise).
	 * Todo comienza con los registros en 0
	 */

	ptrHandlerI2CLCD-> ptrI2Cx -> CCR = 0;
	ptrHandlerI2CLCD-> ptrI2Cx -> TRISE = 0;

	if(ptrHandlerI2CLCD -> modeI2C == I2C_MODE_SM){
		// Estamos en modo "standar" (SM MODE)
		// Seleccionamos el modo estandar
		ptrHandlerI2CLCD-> ptrI2Cx-> CCR &= ~ I2C_CCR_FS;

		// Configuramos el registro que se encarga de generar la señal de reloj
		ptrHandlerI2CLCD -> ptrI2Cx -> CCR |= (I2C_MODE_SM_SPEED_100KHz<< I2C_CCR_CCR_Pos);

		// Configuramos el registro que controla el tiempo T-Rise máximo
		ptrHandlerI2CLCD->ptrI2Cx -> TRISE |= I2C_MAX_RISE_TIME_SM;

	}

	else{
		// Estamos en el modo "Fast" (FM Mode)
		// Seleccionamos el modo fast
		ptrHandlerI2CLCD -> ptrI2Cx -> CCR |= I2C_CCR_FS;

		// Configuramos el registro que se encarga de generar la señal de reloj
		ptrHandlerI2CLCD -> ptrI2Cx -> CCR |= (I2C_MODE_FM_SPEED_400KHz << I2C_CCR_CCR_Pos);

		// Configuramos el registro que controla el timepo T-Rise máximo
		ptrHandlerI2CLCD -> ptrI2Cx -> TRISE |= I2C_MAX_RISE_TIME_FM;
	}

	/* 5. Activamos el módulo I2C */
	ptrHandlerI2CLCD -> ptrI2Cx ->CR1 |= I2C_CR1_PE;

}

/**/
uint8_t I2C_readByteLCD (I2C_HandlerLCD_t *ptrHandlerI2CLCD, uint8_t memAddr){

	/* 0. Definimos una variable auxiliar */
	uint8_t auxByte = 0;
	(void) auxByte;

	/* 1. Verificamos que la línea no esta ocupada - bit "busy" en I2C_CR2 */
	while (ptrHandlerI2CLCD -> ptrI2Cx -> SR2 & I2C_SR2_BUSY){
		__NOP();
	}

	/* 2. Generamos la señal "start" */
	ptrHandlerI2CLCD -> ptrI2Cx -> CR1 |= I2C_CR1_START;

	/* 2a. Esperamos a que la bandera del evento " start" se levante */
	/* Mientras esperamos, el valor de SB es 0, entonces la negación (!) es 1 */
	while (!(ptrHandlerI2CLCD -> ptrI2Cx -> SR1 & I2C_SR1_SB)){
		__NOP();
	}

	/* 3. Enviamos la dirección del Slave y el bit que indica que deseamos escribir (0)
	 * (en el siguiente paso se envía la dirección de memoria que se desea leer */
	ptrHandlerI2CLCD -> ptrI2Cx -> DR = (ptrHandlerI2CLCD -> slaveAddress <<1)  | I2C_WRITE_DATA;

	/* 3.1 Esperamos que la bandera del evento "addr" se levante
	 * (esto nos indica que la dirección fue enviada satisfactoriamente */
	while(!(ptrHandlerI2CLCD -> ptrI2Cx -> SR1 & I2C_SR1_ADDR)){
		__NOP();
	}

	/* 3.2 Debemos limpiar la bandera de la recepción de ACK de la " adr", para lo cual
	 * debemos leer en secuencia primero el I2C_SR1 y luego I2C_SR2 */
	auxByte = ptrHandlerI2CLCD -> ptrI2Cx -> SR1;
	auxByte = ptrHandlerI2CLCD -> ptrI2Cx -> SR2;

	/* 4. Enviamos la dirección de memoria que deseamos leer */
	ptrHandlerI2CLCD -> ptrI2Cx -> DR = memAddr;

	/* 4.1 Esperamos hasta que el byte sea transmitido */
	while (!(ptrHandlerI2CLCD -> ptrI2Cx -> SR1 & I2C_SR1_TXE)){
		__NOP();
	}

	/* 5. Debemos generar una señal de RESTART o sea, enviar un nuevo START */
	ptrHandlerI2CLCD -> ptrI2Cx -> CR1 |= I2C_CR1_START;

	/* 5.1 Esperamos a que la bandera del evento " start" se levante */
	/* Mientras esperamos, el valor de SB es 0, entonces la negacion (!) es 1 */
	while(!(ptrHandlerI2CLCD -> ptrI2Cx -> SR1 & I2C_SR1_SB)){
		__NOP();
	}

	/* 6. Enviamos la dirección del Slave, pero ahora con la indicación de leer */
	ptrHandlerI2CLCD -> ptrI2Cx -> DR = (ptrHandlerI2CLCD -> slaveAddress << 1) | I2C_READ_DATA ;

	/* 6.1 Esperamos hasta que la bandera del evento " addr" se levante
	 * (esto nos indica que la dirección fue enviada satisfactoriamente) */

	while (!(ptrHandlerI2CLCD -> ptrI2Cx -> SR1 & I2C_SR1_ADDR)){
		__NOP();
	}

	/* 6.2 Debemos limpiar la bandera de la recepción de ACK de la "addr", para lo cual
	 * debemos leer en secuencia primero el I2C_SR1 y luego I2C_SR2 */

	auxByte = ptrHandlerI2CLCD -> ptrI2Cx -> SR1;
	auxByte = ptrHandlerI2CLCD -> ptrI2Cx -> SR2;

	/* 7. Activamos la indicación para no-ACK (indicación para el Slave de terminar)
	 * (Debemos escribir cero en la posición ACK del registro de control 1)*/
	ptrHandlerI2CLCD -> ptrI2Cx-> CR1 &= ~ I2C_CR1_ACK;

	/* 8. Generamos la condición de stop */
	ptrHandlerI2CLCD -> ptrI2Cx -> CR1 |= I2C_CR1_STOP;

	/* 9. Esperamos hasta que el byte entrante sea recibido */
	while(!(ptrHandlerI2CLCD-> ptrI2Cx -> SR1 & I2C_SR1_RXNE)){
		__NOP();
	}

	ptrHandlerI2CLCD -> dataI2C = ptrHandlerI2CLCD -> ptrI2Cx -> DR;

	return ptrHandlerI2CLCD -> dataI2C;

}

/**/
void I2C_writeByteLCD (I2C_HandlerLCD_t *ptrHandlerI2CLCD, uint8_t dataToWrite){
	/* 0. Definimos una variable auxiliar */
	uint8_t auxByte = 0;
	(void) auxByte;

	/* 1. Verificamos que la línea no esta ocupada - bit " busy" en I2C_SR2 */
	while(ptrHandlerI2CLCD->ptrI2Cx->SR2 & I2C_SR2_BUSY){
		__NOP();
	}

	/* 2. Generamos la señal de "start" */
	ptrHandlerI2CLCD -> ptrI2Cx -> CR1 |= I2C_CR1_START;

	/* 2a. Esperamos a que la bandera del evento "start" se levante */
	/* Mientras esperamos, el valor de SB es 0, entonces la negación (!) es 1 */
	while (!(ptrHandlerI2CLCD->ptrI2Cx -> SR1 & I2C_SR1_SB)){
		__NOP();
	}

	/* 3. Enviamos la dirección del Slave y el bit que indica que deseamos escribir (0)
	 * (en el siguiente paso se envía la dirección de memoria que se desea escribir */
	ptrHandlerI2CLCD -> ptrI2Cx -> DR = (ptrHandlerI2CLCD -> slaveAddress <<1) | I2C_WRITE_DATA ;

	/* 3.1 Esperamos hasta que la bandera del evento "addr" se levante
	 * (esto nos indica que la dirección fue enviada satisfactoriamente */
	while (!(ptrHandlerI2CLCD -> ptrI2Cx -> SR1 & I2C_SR1_ADDR)){
		__NOP();
	}

	/* 3.2 Debemos limpiar la bandera de la recepción de ACK de la "addr", para lo cual
	 * debemos leer en secuencia primero el I2C_SR1 y el I2C_SR2
	 */

	auxByte = ptrHandlerI2CLCD->  ptrI2Cx -> SR1;
	auxByte = ptrHandlerI2CLCD -> ptrI2Cx -> SR2;

	/* 4. Enviamos la dirección de memoria que deseamos leer */
//	ptrHandlerI2CLCD -> ptrI2Cx -> DR = memAddr;
//
//	/* 4.1 Esperamos hasta que el byte sea transmitido */
//	while (!(ptrHandlerI2CLCD -> ptrI2Cx -> SR1 & I2C_SR1_TXE)){
//		__NOP();
//	}

	/* 5. Cargamos el valor que deseamos escribir */
	ptrHandlerI2CLCD -> ptrI2Cx -> DR = dataToWrite;

	/* 6. Esperamos hasta que el byte sea transmitido */
	while(!(ptrHandlerI2CLCD -> ptrI2Cx -> SR1 & I2C_SR1_BTF)){
		__NOP();
	}

	/* 7. Generamos la condición de stop */
	ptrHandlerI2CLCD -> ptrI2Cx -> CR1 |= I2C_CR1_STOP;

}

void LCD_sendCMD (I2C_HandlerLCD_t *ptrHandlerI2CLCD, char cmd){
	//char = 0b11010010
	char _U;
	char _L;
	uint8_t _T[4];
	_U=(cmd & 0xf0);
	//_u= 0b11010010 & 0b11110000
	//_u= 0b11010000
	_L=((cmd<<4) & 0xf0);
	//_L=00100000 & 0b11110000
	//_L=0b00100000
	_T[0] = _U|0x0C;
	//_T[0]=0B11011100
	I2C_writeByteLCD(ptrHandlerI2CLCD, _T[0]);
	_T[1] = _U|0x08;
	I2C_writeByteLCD(ptrHandlerI2CLCD, _T[1]);
	_T[2] = _L|0x0C;
	I2C_writeByteLCD(ptrHandlerI2CLCD, _T[2]);
	_T[3] = _L|0x08;
	I2C_writeByteLCD(ptrHandlerI2CLCD, _T[3]);
}


void LCD_sendata (I2C_HandlerLCD_t *ptrHandlerI2CLCD, char data){
	char _U;
	char _L;
	uint8_t _T[4];
	_U=(data & 0xf0);
	_L=((data<<4) & 0xf0);
	_T[0] = _U|0x0D;
	I2C_writeByteLCD(ptrHandlerI2CLCD, _T[0]);
	_T[1] = _U|0x09;
	I2C_writeByteLCD(ptrHandlerI2CLCD, _T[1]);
	_T[2] = _L|0x0D;
	I2C_writeByteLCD(ptrHandlerI2CLCD, _T[2]);
	_T[3] = _L|0x09;
	I2C_writeByteLCD(ptrHandlerI2CLCD, _T[3]);
}

void LCD_Clear (I2C_HandlerLCD_t *ptrHandlerI2CLCD) {
	LCD_sendata (ptrHandlerI2CLCD, 0x00);
	for (int i=0; i<100; i++) {
		LCD_sendata (ptrHandlerI2CLCD,' ');
	}
}

void LCD_Init (I2C_HandlerLCD_t *ptrHandlerI2CLCD) {
	delay_50();
	LCD_sendCMD (ptrHandlerI2CLCD, 0x30);
	delay_5();
	LCD_sendCMD (ptrHandlerI2CLCD, 0x30);
	delay_1();
	LCD_sendCMD (ptrHandlerI2CLCD, 0x30);
	delay_10();
	LCD_sendCMD (ptrHandlerI2CLCD, 0x20);
	delay_10();

	LCD_sendCMD (ptrHandlerI2CLCD, 0x28);
	delay_1();
	LCD_sendCMD (ptrHandlerI2CLCD, 0x08);
	delay_1();
	LCD_sendCMD (ptrHandlerI2CLCD, 0x01);
	delay_1();
	delay_1();
	LCD_sendCMD (ptrHandlerI2CLCD, 0x06);
	delay_1();
	LCD_sendCMD (ptrHandlerI2CLCD, 0x0C);
}

void LCD_sendSTR(I2C_HandlerLCD_t *ptrHandlerI2CLCD, char *str) {
	while (*str) LCD_sendata (ptrHandlerI2CLCD, *str++);
}
void LCD_setCursor(I2C_HandlerLCD_t *ptrHandlerI2CLCD, uint8_t x, uint8_t y) {
	uint8_t cursor;
	switch (x) {
	case 0 :
		switch (y) {
		case 0 : cursor = 0x00; break;
		case 1 : cursor = 0x01; break;
		case 2 : cursor = 0x02; break;
		case 3 : cursor = 0x03; break;
		case 4 : cursor = 0x04; break;
		case 5 : cursor = 0x05; break;
		case 6 : cursor = 0x06; break;
		case 7 : cursor = 0x07; break;
		case 8 : cursor = 0x08; break;
		case 9 : cursor = 0x09; break;
		case 10 : cursor = 0x0A; break;
		case 11 : cursor = 0x0B; break;
		case 12 : cursor = 0x0C; break;
		case 13 : cursor = 0x0D; break;
		case 14 : cursor = 0x0E; break;
		case 15 : cursor = 0x0F; break;
		case 16 : cursor = 0x10; break;
		case 17 : cursor = 0x11; break;
		case 18 : cursor = 0x12; break;
		case 19 : cursor = 0x13; break;
		} break;

	case 1 :
		switch (y) {
		case 0 : cursor = 0x40; break;
		case 1 : cursor = 0x41; break;
		case 2 : cursor = 0x42; break;
		case 3 : cursor = 0x43; break;
		case 4 : cursor = 0x44; break;
		case 5 : cursor = 0x45; break;
		case 6 : cursor = 0x46; break;
		case 7 : cursor = 0x47; break;
		case 8 : cursor = 0x48; break;
		case 9 : cursor = 0x49; break;
		case 10 : cursor = 0x4A; break;
		case 11 : cursor = 0x4B; break;
		case 12 : cursor = 0x4C; break;
		case 13 : cursor = 0x4D; break;
		case 14 : cursor = 0x4E; break;
		case 15 : cursor = 0x4F; break;
		case 16 : cursor = 0x50; break;
		case 17 : cursor = 0x51; break;
		case 18 : cursor = 0x52; break;
		case 19 : cursor = 0x53; break;
		} break;

	case 2 :
		switch (y) {
		case 0 : cursor = 0x14; break;
		case 1 : cursor = 0x15; break;
		case 2 : cursor = 0x16; break;
		case 3 : cursor = 0x17; break;
		case 4 : cursor = 0x18; break;
		case 5 : cursor = 0x19; break;
		case 6 : cursor = 0x1A; break;
		case 7 : cursor = 0x1B; break;
		case 8 : cursor = 0x1C; break;
		case 9 : cursor = 0x1D; break;
		case 10 : cursor = 0x1E; break;
		case 11 : cursor = 0x1F; break;
		case 12 : cursor = 0x20; break;
		case 13 : cursor = 0x21; break;
		case 14 : cursor = 0x22; break;
		case 15 : cursor = 0x23; break;
		case 16 : cursor = 0x24; break;
		case 17 : cursor = 0x25; break;
		case 18 : cursor = 0x26; break;
		case 19 : cursor = 0x27; break;
		} break;

	case 3 :
		switch (y) {
		case 0 : cursor = 0x54; break;
		case 1 : cursor = 0x55; break;
		case 2 : cursor = 0x56; break;
		case 3 : cursor = 0x57; break;
		case 4 : cursor = 0x58; break;
		case 5 : cursor = 0x59; break;
		case 6 : cursor = 0x5A; break;
		case 7 : cursor = 0x5B; break;
		case 8 : cursor = 0x5C; break;
		case 9 : cursor = 0x5D; break;
		case 10 : cursor = 0x5E; break;
		case 11 : cursor = 0x5F; break;
		case 12 : cursor = 0x60; break;
		case 13 : cursor = 0x61; break;
		case 14 : cursor = 0x62; break;
		case 15 : cursor = 0x63; break;
		case 16 : cursor = 0x64; break;
		case 17 : cursor = 0x65; break;
		case 18 : cursor = 0x66; break;
		case 19 : cursor = 0x67; break;
		} break;
	}
	LCD_sendCMD(ptrHandlerI2CLCD, 0x80|cursor);
}
void delay_50 (void){
	for (int i=0;i<62500;i++){
		__NOP();
	}
}

void delay_5 (void){
	for (int i=0; i<6250; i++){
		__NOP();
	}
}

void delay_1 (void){
	for (int i=0;i<1250;i++){
		__NOP();
	}
}

void delay_10 (void){
	for (int i=0;i<12500;i++){
		__NOP();
	}
}
