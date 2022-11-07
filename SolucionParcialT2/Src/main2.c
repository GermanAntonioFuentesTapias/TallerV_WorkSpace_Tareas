




#include <stm32f4xx.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "BasicTimer.h"
#include "I2CDriver.h"
#include "I2CLCD.h"
#include "PwmDriver.h"
#include "math.h"// se describe los handlers y las variables que se usará para el proyecto

#include "RTCxDriver.h"

/* Definición de variables para operación matematicas */

#define MAX_X    16000
#define MIN_Y   -16000
#define MIN_X   -16000
#define MIN_Z   -16000

int16_t radio = 0;
int16_t  angulo = 0;

int16_t AccelX = 0;
int16_t AccelY = 0;
int16_t AccelZ = 0;

/* Definición de variables */
GPIO_Handler_t handlerBlinkyPin     = {0};

GPIO_Handler_t handlerTx     		= {0};
GPIO_Handler_t handlerRx	 		= {0};

GPIO_Handler_t handlerI2cSDA 		= {0};
GPIO_Handler_t handlerI2cSCL		= {0};

I2C_Handler_t handlerAcelerometro   = {0};
LCD_Handler_t handlerLCD         = {0};


USART_Handler_t handlerUsart1    = {0};

BasicTimer_Handler_t handlerBlinkyTimer = {0};

/* Definición de variables del proyecto */
uint8_t handlerLed  = 0;
uint8_t rxData      =  0;
uint8_t rxDataCMD   = 0;
uint8_t Bandera     = 0;
uint8_t BanderaRGB  = 0;
uint8_t BanderaRGB2 = 0;

BasicTimer_Handler_t handlerCommands = {0};
BasicTimer_Handler_t handlerRGB      = {0};

uint8_t i2cBuffer = 0;

/* Definición para RTC */

uint8_t  *ptrData;

RTC_handler_t handlerRTC = {0};

uint8_t  Seg ;
uint8_t  Min ;
uint8_t  Hor;
uint8_t  Sem ;
uint8_t  Day;
uint8_t  Mes;
uint8_t  Ano;

/* Para mensajes */
char DataRTC[64] = "Wipi";
char DataRTCDate[64] = "Wipi";
char bufferData[64] = "esto es una pequeña prueba";
char bufferDataY[64] = " Valor Y";
char welcomer[64]   = "Bienvenido";

/* Variables del I2C LCD */

GPIO_Handler_t handlerLCDcSDA   = {0};
GPIO_Handler_t handlerLCDcSCL   = {0};

/* Variables del PWM */

PWM_Handler_t  handlerPWMB       = {0};
PWM_Handler_t  handlerPWMR       = {0};
PWM_Handler_t  handlerPWMG       = {0};

BasicTimer_Handler_t  handlerTimer = {0};
GPIO_Handler_t        handlerBlue = {0};
GPIO_Handler_t        handlerGreen = {0};
GPIO_Handler_t        handlerRed = {0};

// StateLED
GPIO_Handler_t handlerBlinkyPin = {0}; // handler para el pin del stateLed
BasicTimer_Handler_t handlerBlinkyTimer = {0}; // handler para el timer2 para el blinky del stateLED

// USART
GPIO_Handler_t handlerTxPin = {0}; // handler para el pin de la transmision del usart2
GPIO_Handler_t handlerRxPin = {0}; // handler para el pin de la recepcion del usart2
USART_Handler_t handlerUSART2 = {0}; // handler para el usart2
uint8_t rxData = 0;
char bufferData[64] = "esto es una pequeña prueba";
#define BUFFER_SIZE 150
uint16_t datoLeido = 0; ;
uint8_t counterReception = 0;
char cmd[64];
char bufferReception[BUFFER_SIZE];
bool stringComplete = false;
unsigned int primero;
unsigned int segundo;
unsigned int tercero;
char userMsg[64] = "\0";

// Configuracion para el Acelerometro
GPIO_Handler_t handlerI2cSDA = {0};
GPIO_Handler_t handlerI2cSCL = {0};
I2C_Handler_t handlerAccelerometer = {0};
uint8_t i2cBuffer = 0;

/********** LCD *************/
GPIO_Handler_t handlerLCDI2cSDA = {0};
GPIO_Handler_t handlerLCDI2cSCL = {0};
LCD_Handler_t handlerLCD = {0};
uint8_t commandLCD = 0;
uint8_t commandLED = 0;
uint8_t caso = 0;
uint64_t tic = 0;

/********* PWM **************/
PWM_Handler_t PWM_HandlerR = {0};
GPIO_Handler_t PWM_HandlerRPin = {0};

PWM_Handler_t PWM_HandlerG = {0};
GPIO_Handler_t PWM_HandlerGPin = {0};

PWM_Handler_t PWM_HandlerB = {0};
GPIO_Handler_t PWM_HandlerBPin = {0};

uint8_t duttyR = 50;
uint8_t duttyG = 50;
uint8_t duttyB = 50;
uint8_t dutty = 50;

uint8_t contador = 10;

/********* RTC ***********/
handlerFechaHora_t configFechaHora = {0};
handlerFechaHora_t actualFechaHora = {0};

// Direcciones de memora del acelerometro MPU 6050
#define ACCEL_ADDRESS 0b1101001; // 0xD2 --> Direccion del Accel con Logic_1
#define ACCEL_XOUT_H   59
#define ACCEL_XOUT_L   60
#define ACCEL_YOUT_H   61
#define ACCEL_YOUT_L   62

#define PWR_MGMT_1  107
#define WHO_AM_I    117

// Direccion de memoria del LCD LCM2004A1
#define LCD_ADRESS   0b100111


void initSystem (void);

void parseCommands(char *ptrBufferReception);

void LCDBienvenida(void);

int main(void){

	initSystem();

	LCD_Clear(&handlerLCD, LCD_ADRESS);
	LCD_Init(&handlerLCD, LCD_ADRESS);

	LCDBienvenida();

	setTime(&configFechaHora);
	getTime(&actualFechaHora);

	while(1){

		//Hacemos un "eco" con el valor que nos llega por el serial
		if(rxData != '\0'){ // condicion que se activa cuando la variable rxData es diferente de 0
			writeChar(&handlerUSART2, rxData);

			bufferReception[counterReception] = rxData;
			counterReception++;

			if(rxData == '@'){ // si el valor del rxData es @, termina la sentencia
				stringComplete = true;
				bufferReception[counterReception] = '\0';
				counterReception = 0;
			}

			rxData = 0; // se baja la bandera para recibir una nueva interrupcion
		}
		if(stringComplete){ // si se comppleta el string ia funcion parseCommands agarra los valores del buffer
			parseCommands(bufferReception);
			stringComplete = false;
		}


	}
	return 0;
}


void parseCommands(char *ptrBufferReception){

	sscanf(ptrBufferReception, "%s %u %u %s", cmd, &primero, &segundo, userMsg);
	if(strcmp(cmd, "menu") == 0){ // si se escribe la palabra menu, se muestra el menu por medio del USART2
		writeMsg(&handlerUSART2, "Menu de comandos para controlar los colores del RGB\n");
		writeMsg(&handlerUSART2, "1) encenderacel ----------> Enciende el acelerometro\n");
		writeMsg(&handlerUSART2, "2) apagaracel   ----------> Apaga el acelerometro\n");
		writeMsg(&handlerUSART2, "3) verde        ----------> Coloca el LED en color verde\n");
		writeMsg(&handlerUSART2, "4) amarillo     ----------> Coloca el LED en color amarillo\n");
		writeMsg(&handlerUSART2, "5) rojo         ----------> Coloca el LED en color rojo\n");
		writeMsg(&handlerUSART2, "6) magenta      ----------> Coloca el LED en color magenta\n");
		writeMsg(&handlerUSART2, "7) azul         ----------> Coloca el LED en color azul\n");
		writeMsg(&handlerUSART2, "8) celeste      ----------> Coloca el LED en color celeste\n");
		writeMsg(&handlerUSART2, "9) fiesta       ----------> Prende los colores del LED segun los datos del acelerometro\n");
		writeMsg(&handlerUSART2, "10) x           ----------> Muestra los datos del eje X del acelerometro\n");
		writeMsg(&handlerUSART2, "11) y           ----------> Muestra los datos del eje Y del acelerometro\n");
		//writeMsg(&handlerUSART2, "10) rgb\n");


		LCD_Clear(&handlerLCD, LCD_ADRESS); // se limpia la pantalla LCD

		LCD_setCursor(&handlerLCD,0,0,LCD_ADRESS); // se direcciona el nuevo mensaje a la direccion de la pantalla LCD
		LCD_sendSTR(&handlerLCD,"Esta en el menu",LCD_ADRESS); // se escribe el nuevo mensaje en la LCD

	}
	else if(strcmp(cmd, "encenderacel") == 0){
		i2c_writeSingleRegister(&handlerAccelerometer, PWR_MGMT_1, 0x00); // se enciende el aclerometro
		writeMsg(&handlerUSART2, "Acelerometro Activado\n");

		LCD_Clear(&handlerLCD, LCD_ADRESS);

		LCD_setCursor(&handlerLCD,0,0,LCD_ADRESS);
		LCD_sendSTR(&handlerLCD,"Acelerometro ON",LCD_ADRESS);

	}
	else if(strcmp(cmd, "apagaracel") == 0){
		i2c_writeSingleRegister(&handlerAccelerometer, PWR_MGMT_1, 0x80); // se apaga el acelerometro
		writeMsg(&handlerUSART2, "Acelerometro Desactivado\n");

		LCD_Clear(&handlerLCD, LCD_ADRESS);

		LCD_setCursor(&handlerLCD,0,0,LCD_ADRESS);
		LCD_sendSTR(&handlerLCD,"Acelerometro OFF",LCD_ADRESS);


	}
	else if(strcmp(cmd, "verde") == 0){ // comando para encender el LED de determinado color
		//commandLED = 3;
		writeMsg(&handlerUSART2, "LED en verde\n");

		LCD_Clear(&handlerLCD, LCD_ADRESS);

		LCD_setCursor(&handlerLCD,0,0,LCD_ADRESS);
		LCD_sendSTR(&handlerLCD,"LED en verde",LCD_ADRESS);

		duttyR = 0;     // se asignan nuevos valores para el dutty
		duttyG = 100;
		duttyB = 0;

		updateDuttyCycle(&PWM_HandlerR, duttyR);      // el nuevo valor del dutty se le asiga a la señal PWM
		updateDuttyCycle(&PWM_HandlerR, duttyR);
		updateDuttyCycle(&PWM_HandlerB, duttyB);
	}
	else if(strcmp(cmd, "amarillo") == 0){
		//commandLED = 4;
		writeMsg(&handlerUSART2, "LED en amarillo\n");
		LCD_Clear(&handlerLCD, LCD_ADRESS);

		LCD_setCursor(&handlerLCD,0,0,LCD_ADRESS);
		LCD_sendSTR(&handlerLCD,"LED en amarillo",LCD_ADRESS);

		duttyR = 50;
		duttyG = 50;
		duttyB = 0;

		updateDuttyCycle(&PWM_HandlerR, duttyR);
		updateDuttyCycle(&PWM_HandlerR, duttyR);
		updateDuttyCycle(&PWM_HandlerB, duttyB);
	}
	else if(strcmp(cmd, "rojo") == 0){
		//commandLED = 1;
		writeMsg(&handlerUSART2, "LED en rojo\n");
		LCD_Clear(&handlerLCD, LCD_ADRESS);

		LCD_setCursor(&handlerLCD,0,0,LCD_ADRESS);
		LCD_sendSTR(&handlerLCD,"LED en rojo",LCD_ADRESS);

		duttyR = 100;
		duttyG = 0;
		duttyB = 0;

		updateDuttyCycle(&PWM_HandlerR, duttyR);
		updateDuttyCycle(&PWM_HandlerR, duttyR);
		updateDuttyCycle(&PWM_HandlerB, duttyB);
	}
	else if(strcmp(cmd, "magenta") == 0){
		//commandLED = 5;
		writeMsg(&handlerUSART2, "LED en magenta\n");
		LCD_Clear(&handlerLCD, LCD_ADRESS);

		LCD_setCursor(&handlerLCD,0,0,LCD_ADRESS);
		LCD_sendSTR(&handlerLCD,"LED en magenta",LCD_ADRESS);

		duttyR = 50;
		duttyG = 0;
		duttyB = 50;

		updateDuttyCycle(&PWM_HandlerR, duttyR);
		updateDuttyCycle(&PWM_HandlerR, duttyR);
		updateDuttyCycle(&PWM_HandlerB, duttyB);
	}
	else if(strcmp(cmd, "azul") == 0){
		//commandLED = 2;
		writeMsg(&handlerUSART2, "LED en azul\n");
		LCD_Clear(&handlerLCD, LCD_ADRESS);

		LCD_setCursor(&handlerLCD,0,0,LCD_ADRESS);
		LCD_sendSTR(&handlerLCD,"LED en azul",LCD_ADRESS);

		duttyR = 0;
		duttyG = 0;
		duttyB = 100;

		updateDuttyCycle(&PWM_HandlerR, duttyR);
		updateDuttyCycle(&PWM_HandlerR, duttyR);
		updateDuttyCycle(&PWM_HandlerB, duttyB);
	}
	else if(strcmp(cmd, "celeste") == 0){
		//commandLED = 6;
		writeMsg(&handlerUSART2, "LED en celeste\n");
		LCD_Clear(&handlerLCD, LCD_ADRESS);

		LCD_setCursor(&handlerLCD,0,0,LCD_ADRESS);
		LCD_sendSTR(&handlerLCD,"LED en celeste",LCD_ADRESS);

		duttyR = 0;
		duttyG = 100;
		duttyB = 0;

		updateDuttyCycle(&PWM_HandlerR, duttyR);
		updateDuttyCycle(&PWM_HandlerR, duttyR);
		updateDuttyCycle(&PWM_HandlerB, duttyB);
	}
	else if(strcmp(cmd, "fiesta") == 0){ // funcion para encender el acelerometro y asignarle sus valor de lectura al dutty del PWM
		commandLED = 7;
		caso = primero;
		writeMsg(&handlerUSART2, "Mueve el acelerometro para la fiesta\n");
	}
//	else if(strcmp(cmd, "rgb") == 0){
//		setDuttyCycle(&PWM_HandlerR);
//		setDuttyCycle(&PWM_HandlerG);
//		setDuttyCycle(&PWM_HandlerB);
//		writeMsg(&handlerUSART2, "Se activa la senal del RGB\n");
//	}
	else if(strcmp(cmd, "x") == 0){

		uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
		uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
		int16_t AccelX = AccelX_high << 8 | AccelX_low;
		sprintf(bufferData,"AccelX = %d \n", (int) AccelX);
		writeMsg(&handlerUSART2, bufferData);



//			if ((AccelX < 10000)){
//				duttyB = 50-(AccelX/200);
//				duttyR = 50+(AccelX/200);
//
//				updateDuttyCycle(&PWM_HandlerR, duttyR);
//				updateDuttyCycle(&PWM_HandlerB, duttyB);
//
//
//			}
//			if ((AccelX < 65535)  && (AccelX > 55535) ){
//				duttyB = 100- ((AccelX-55535)/200);
//				duttyR = ((AccelX-55535)/200);
//
//				updateDuttyCycle(&PWM_HandlerR, duttyR);
//				updateDuttyCycle(&PWM_HandlerB, duttyB);
//			}
//
//			// Valores que se toman en el eje X para el color Rojo y Verde
//			if ((AccelX < 10000)){
//				duttyG = 50-(AccelX/200);
//				duttyR = 50+(AccelX/200);
//
//				updateDuttyCycle(&PWM_HandlerR, duttyR);
//				updateDuttyCycle(&PWM_HandlerB, duttyG);
//			}
//			if ((AccelX < 65535)  && (AccelX > 55535) ){
//				duttyG = 100- ((AccelX-55535)/200);
//				duttyR = ((AccelX-55535)/200);
//
//				updateDuttyCycle(&PWM_HandlerR, duttyR);
//				updateDuttyCycle(&PWM_HandlerB, duttyG);
//			}
//
//			// Valores que se toman en el eje X para el color Verde y Azul
//
//			if ((AccelX < 10000)){
//				duttyB = 50-(AccelX/200);
//				duttyG = 50+(AccelX/200);
//
//				updateDuttyCycle(&PWM_HandlerR, duttyR);
//				updateDuttyCycle(&PWM_HandlerB, duttyG);
//			}
//			if ((AccelX < 65535)  && (AccelX > 55535) ){
//				duttyB = 100- ((AccelX-55535)/200);
//				duttyG = ((AccelX-55535)/200);
//
//				updateDuttyCycle(&PWM_HandlerR, duttyR);
//				updateDuttyCycle(&PWM_HandlerB, duttyB);
//			}
//			else{
//				__NOP();
//			}
		}
		else if(strcmp(cmd, "y") == 0){
			uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
			uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
			int16_t AccelY = AccelY_high << 8 | AccelY_low;
			sprintf(bufferData,"AccelY = %d \n", (int) AccelY);
			writeMsg(&handlerUSART2, bufferData);

//			if ((AccelY < 10000)){
//				duttyG = 50-(AccelY/200);
//			}
//			if ((AccelY < 65535)  && (AccelY > 55535) ){
//				duttyG = 100- ((AccelY-55535)/200);
//			}
//			if ((AccelY < 10000)){
//				duttyB = 50-(AccelY/200);
//			}
//			if ((AccelY < 65535)  && (AccelY > 55535) ){
//				duttyB = 100- ((AccelY-55535)/200);
//			}
//			if ((AccelY < 10000)){
//				duttyR = 50-(AccelY/200);
//			}
//			if ((AccelY < 65535)  && (AccelY > 55535) ){
//				duttyR = 100- ((AccelY-55535)/200);
//			}
//			else{
//				__NOP();
//			}
		}
	else{
		writeMsg(&handlerUSART2, "Comando no valido\n");
	}
}

// Punto 1. Se llama y se implementa la funcion callback del timer2
void BasicTimer2_Callback (void){

    // con esta sentencia se realiza el blinky del userLED cada 250 ms
	handlerBlinkyPin.pGPIOx->ODR ^= GPIO_ODR_OD5;

}

void usart2Rx_Callback(void){

	rxData = getRxData();
}

void initSystem(void){

	/*********** CONFIGURACION PARA EL BLINKY DEL STATE LED ****************/

	// Se configura el userLED que está en el PIN A5
	handlerBlinkyPin.pGPIOx = GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	// Se carga la configuracion del PIN
	GPIO_Config(&handlerBlinkyPin);

	// Se configura el timer2 para el blinky del stateLED
	handlerBlinkyTimer.ptrTIMx = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = 1;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_period = 250;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;

	// Se carga la configuracion del timer2
	BasicTimer_Config(&handlerBlinkyTimer);

	/************** CONFIGURACION PARA EL USART2 *********************/

	// Se configura el USART2

	handlerUSART2.ptrUSARTx = USART2;
	handlerUSART2.USART_Config.USART_baudrate = USART_BAUDRATE_57600;
	handlerUSART2.USART_Config.USART_datasize = USART_DATASIZE_9BIT;
	handlerUSART2.USART_Config.USART_mode = USART_MODE_RXTX;
	handlerUSART2.USART_Config.USART_parity = USART_PARITY_EVEN;
	handlerUSART2.USART_Config.USART_stopbits = USART_STOPBIT_1;
	handlerUSART2.USART_Config.USART_interrupt = USART_INTERRUPT_ENABLE;

	// Se carga la configuracion del USART2
	USART_Config(&handlerUSART2);

	// Se configura el pin A2 para la transmision de datos mediante el USART2
	handlerTxPin.pGPIOx = GPIOA;
	handlerTxPin.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	handlerTxPin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerTxPin.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
	handlerTxPin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerTxPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerTxPin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;

	// Se carga la configuracion del PINA2
	GPIO_Config(&handlerTxPin);

	// Se configura el pin A3 para la recepcion de datos mediante el USART2
	handlerRxPin.pGPIOx = GPIOA;
	handlerRxPin.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	handlerRxPin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerRxPin.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
	handlerRxPin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerRxPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerRxPin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;

	// Se carga la configuracion del PINA3
	GPIO_Config(&handlerRxPin);

	/*********** CONFIGURACION DEL ACELEROMETRO ***********/

	// Se configura los pines sobre los que trabaj el I2C1
	handlerI2cSCL.pGPIOx = GPIOB;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinAltFunMode = AF4;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinNumber = PIN_10;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_OPENDRAIN;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;

	// Se garga la configuracion del I2C
	GPIO_Config(&handlerI2cSCL);

	// Se configura los pines sobre los que trabaj el I2C1
	handlerI2cSDA.pGPIOx = GPIOB;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinAltFunMode = AF9;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_OPENDRAIN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;

	// Se garga la configuracion del I2C
	GPIO_Config(&handlerI2cSDA);

	// Se configura el Acelerometro
	handlerAccelerometer.ptrI2Cx = I2C2;
	handlerAccelerometer.modeI2C = I2C_MODE_FM;
	handlerAccelerometer.slaveAddress = ACCEL_ADDRESS;

	i2c_config(&handlerAccelerometer);

	/***************** PWM ***********************/

	// Configuracion para el LED Rojo
	PWM_HandlerRPin.pGPIOx = GPIOA;
	PWM_HandlerRPin.GPIO_PinConfig.GPIO_PinAltFunMode = AF2;
	PWM_HandlerRPin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	PWM_HandlerRPin.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
	PWM_HandlerRPin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	PWM_HandlerRPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	PWM_HandlerRPin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;

	GPIO_Config(&PWM_HandlerRPin);

	PWM_HandlerR.ptrTIMx = TIM5;
	PWM_HandlerR.config.channel = PWM_CHANNEL_2;
	PWM_HandlerR.config.duttyCicle = 50;
	PWM_HandlerR.config.periodo = 10000;
	PWM_HandlerR.config.prescaler = BTIMER_SPEED_1ms;

	pwm_Config(&PWM_HandlerR);
	startPwmSignal(&PWM_HandlerR);

	//Configuracion para el LED Verde
	PWM_HandlerGPin.pGPIOx = GPIOB;
	PWM_HandlerGPin.GPIO_PinConfig.GPIO_PinAltFunMode = AF2;
	PWM_HandlerGPin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	PWM_HandlerGPin.GPIO_PinConfig.GPIO_PinNumber = PIN_0;
	PWM_HandlerGPin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_OPENDRAIN;
	PWM_HandlerGPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	PWM_HandlerGPin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;

	GPIO_Config(&PWM_HandlerGPin);

	PWM_HandlerG.ptrTIMx = TIM3;
	PWM_HandlerG.config.channel = PWM_CHANNEL_3;
	PWM_HandlerG.config.duttyCicle = 50;
	PWM_HandlerG.config.periodo = 10000;
	PWM_HandlerG.config.prescaler = BTIMER_SPEED_1ms;

	pwm_Config(&PWM_HandlerG);
	startPwmSignal(&PWM_HandlerG);

	// Configuracion para el LED Azul
	PWM_HandlerBPin.pGPIOx = GPIOA;
	PWM_HandlerBPin.GPIO_PinConfig.GPIO_PinAltFunMode = AF2;
	PWM_HandlerBPin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	PWM_HandlerBPin.GPIO_PinConfig.GPIO_PinNumber = PIN_0;
	PWM_HandlerBPin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_OPENDRAIN;
	PWM_HandlerBPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	PWM_HandlerBPin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;

	GPIO_Config(&PWM_HandlerBPin);

	PWM_HandlerB.ptrTIMx = TIM5;
	PWM_HandlerB.config.channel = PWM_CHANNEL_3;
	PWM_HandlerB.config.duttyCicle = 50;
	PWM_HandlerB.config.periodo = 10000;
	PWM_HandlerB.config.prescaler = 1600;

	pwm_Config(&PWM_HandlerB);
	startPwmSignal(&PWM_HandlerB);

	/****************** CONFIGURACION DE LA PANTALLA LCD *****************************/

   	handlerLCDI2cSCL.pGPIOx                                 = GPIOB ;//puerto B para recepcion de datos
   	handlerLCDI2cSCL.GPIO_PinConfig.GPIO_PinNumber          = PIN_6;//pin 6 activado para recepcion de datos
   	handlerLCDI2cSCL.GPIO_PinConfig.GPIO_PinMode            = GPIO_MODE_ALTFN ; // salida
   	handlerLCDI2cSCL.GPIO_PinConfig.GPIO_PinOPType          = GPIO_OTYPE_OPENDRAIN;
   	handlerLCDI2cSCL.GPIO_PinConfig.GPIO_PinPuPdControl     = GPIO_PUPDR_PULLUP;
   	handlerLCDI2cSCL.GPIO_PinConfig.GPIO_PinSpeed           = GPIO_OSPEED_FAST;
   	handlerLCDI2cSCL.GPIO_PinConfig.GPIO_PinAltFunMode      = AF4 ; // funcion alternativa

   	GPIO_Config(&handlerLCDI2cSCL);


   	//configuramos los pines sobre los que funciona el 	LCD
   	handlerLCDI2cSDA.pGPIOx                                 = GPIOB ;//puerto B para recepcion de datos
   	handlerLCDI2cSDA.GPIO_PinConfig.GPIO_PinNumber          = PIN_7;//pin 7 activado para recepcion de datos
   	handlerLCDI2cSDA.GPIO_PinConfig.GPIO_PinMode            = GPIO_MODE_ALTFN ; // salida
   	handlerLCDI2cSDA.GPIO_PinConfig.GPIO_PinOPType          = GPIO_OTYPE_OPENDRAIN;
   	handlerLCDI2cSDA.GPIO_PinConfig.GPIO_PinPuPdControl     = GPIO_PUPDR_PULLUP;
   	handlerLCDI2cSDA.GPIO_PinConfig.GPIO_PinSpeed           = GPIO_OSPEED_FAST;
   	handlerLCDI2cSDA.GPIO_PinConfig.GPIO_PinAltFunMode      = AF4 ; // funcion alternativa

   	GPIO_Config(&handlerLCDI2cSDA);

   	handlerLCD.ptrLCDx         = I2C1 ;//
   	handlerLCD.modeLCD         = I2C_MODE_SM;//I2C_MODE_FM
   	handlerLCD.slaveAddressLCD = LCD_ADRESS;//0b0100111

   	LCD_Config(&handlerLCD);

}

void LCDBienvenida(void){
	LCD_setCursor(&handlerLCD,0,0,LCD_ADRESS);
	LCD_sendSTR(&handlerLCD,"BienvenidoSenorStark",LCD_ADRESS);

	LCD_setCursor(&handlerLCD,2,0,LCD_ADRESS);
	LCD_sendSTR(&handlerLCD,"Escriba menu...",LCD_ADRESS);

			for (int i=0;i<10000;i++);

}
