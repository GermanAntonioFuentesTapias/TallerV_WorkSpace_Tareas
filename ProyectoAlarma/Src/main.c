/*
 * main.c
 *
 *  Created on: 1/11/2022
 *      Author: German Antonio Fuentes Tapias
 *
 *      @ gafuentest@unal.edu.co
 *
 *     Elaboración proyecto
 *
 *      Paridad Even
 *      Usart1 para comunicación serial
 *      Velocidad 19200
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "stm32f4xx.h"
#include "BasicTimer.h"
#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "LCDDriver.h"
#include "I2CDriver.h"
#include "PwmDriver.h"
#include "ExtiDriver.h"
#include  "KeyPad.h"
#include  "RTCxDriver.h"

GPIO_Handler_t    handlerBlinkyPin   = {0};


//Timer encargado del Blinky
BasicTimer_Handler_t  handlerBlinkyTimer = {0};
BasicTimer_Handler_t  handlerReset       = {0};

GPIO_Handler_t handlerTx     	    	= {0};
GPIO_Handler_t handlerRx	 	     	= {0};
USART_Handler_t        handlerUsart     = {0}; // Handler para el Usart
uint8_t rxData      =  0;
char DataRTC[64]    = "Wipi";
uint16_t clave_acceso          = 1234;

uint8_t stateKeypad            = NOTHING;
uint8_t posicionMenu           = SALIO;
uint8_t seleccionConfiguracion = NOTHING;
uint8_t estadoPantalla         = INICIO;
uint8_t visualPantalla         = NOTHING;


/* Banderas interrupciones */

//uint8_t EstadoBoton = 0;
// Interrupciones importantes
//uint8_t stop                            = 1;
uint8_t movimiento_puerta                 = 0;
uint8_t desactivar_puerta                 = 0;

uint8_t Confi1                          = 0;
uint8_t Confi2                          = 0;
uint8_t Confi3                          = 0;
uint8_t Confi4                          = 0;

uint8_t Confi11                         = 0;
uint8_t Confi22                         = 0;
uint8_t Confi33                         = 0;
uint8_t Confi44                         = 0;

uint8_t activarA                        = 0;
uint8_t activarS                        = 0;
uint8_t devolverse                      = 0;
uint8_t desactivar                      = 0;

// Otras interrupciones
uint8_t reset                           = 0;
uint8_t handlerLed                      = 0;
uint8_t Code                            = 0;
uint8_t Columna_1                         = 0;
uint8_t Columna_2                           = 0;
uint8_t Columna_3                           = 0;
uint8_t Columna_4                           = 0;
uint8_t Bandera                         = 0;
uint8_t valor                           = 0;
uint8_t Puerta                          = 0;
uint16_t duttyValue                      = 15000;
uint16_t frequencyValue                  = 18000;

/* Para Muestro */

uint8_t  Columna1                        = {1};
uint8_t  Columna2                        = {1};
uint8_t  Columna3                        = {1};
uint8_t  Columna4                        = {1};


GPIO_Handler_t	handlerI2CSDA			= {0};
GPIO_Handler_t	handlerI2CSCL 			= {0};
GPIO_Handler_t  handlerSensor           = {0};
GPIO_Handler_t  handlerRojo             = {0};
GPIO_Handler_t  handlerPuerta           = {0};
GPIO_Handler_t  handlerBuzzer           = {0};

/* Configuración Pines Key Pad */

GPIO_Handler_t  handlerF1               = {0};
GPIO_Handler_t  handlerF2               = {0};
GPIO_Handler_t  handlerF3               = {0};
GPIO_Handler_t  handlerF4               = {0};
GPIO_Handler_t  handlerC1               = {0};
GPIO_Handler_t  handlerC2               = {0};
GPIO_Handler_t  handlerC3               = {0};
GPIO_Handler_t  handlerC4               = {0};

/**********************************************/

I2C_Handler_t	handlerLCD 				= {0};
EXTI_Config_t   handlerExtiSensor       = {0}; // Configuración para el boton externo
EXTI_Config_t   handlerExtiPuerta       = {0};
EXTI_Config_t   handlerF1Ex             = {0};
EXTI_Config_t   handlerF2Ex             = {0};
EXTI_Config_t   handlerF3Ex             = {0};
EXTI_Config_t   handlerF4Ex             = {0};
PWM_Handler_t   handlerPWM              = {0};
char dataLCD[64] = {0};
char bufferData[64]    =  {0};



/* Configuración para RTC */

uint8_t  Seg ;
uint8_t  Min ;
uint8_t  Hor;
uint8_t  Sem ;
uint8_t  Day;
uint8_t  Mes;
uint8_t  Ano;

/* Funciones para comandos */


uint8_t counterReception = 0;
char bufferReception[64] = {0};
char cmd[64];
char userMsg[64];
char data[64] = {0} ;
bool stringComplete = false;
bool makeUpdateLCD  = false;
unsigned int firstParameter;
unsigned int secondParameter;
//int Filas[];
//int Columnas[];

#define LCD_ADRESS		0b0100111

//Definición de las cabeceras del main
void initSystem (void);
//Para el KeyPad
void ConfigKeyPad(void);
// Alarma
void Alarma(void);

//Para las multiples opciones del KeyPad
//void read_keypad (void);

void read_keypad(uint8_t EstadoBoton);

void Muestreo(void);

//Se utiliza para llamar a la funciones de cada caso
// de manera inmediata

void config_general(void);

// Casos para despliegues de entrada
void config_inicial(uint8_t entrada);
// despliegue general
void menu(void);

// Encargado de lo que activa y desactiva
void menu_activacion(void);

// Encargado de configuraciones

void menu_configuracion(void);

// Encargado de parametros

void menu_parametros(void);

// Encargado de fecha y hora

void fechahora(void);


int main(void){

	initSystem();
	 ConfigKeyPad();

		LCD_Init(&handlerLCD);
		delay_10();
		LCD_ClearScreen(&handlerLCD);
		delay_10();

		LCD_sendSTR(&handlerLCD,"____________________");
		LCD_setCursor(&handlerLCD,0,0);
		LCD_sendSTR(&handlerLCD,"Alarmas German S.A.S");
		LCD_setCursor(&handlerLCD,1,4);
		LCD_sendSTR(&handlerLCD,"Bienvenidos    ");
		LCD_setCursor(&handlerLCD,3,0);
		LCD_sendSTR(&handlerLCD,"Menu con las Letras");

		startCounterTimer(&handlerBlinkyTimer);
		startCounterTimer(&handlerReset);

while (1){

	    config_general();


       /* Función que se encarga de prender la alarma */

        if(((Puerta) ||  (Code)) && ((movimiento_puerta) == 1) ) {



        	updateFrequency(&handlerPWM,frequencyValue);
        	updateDuttyCycle(&handlerPWM, duttyValue);
        	startPwmSignal(&handlerPWM);
        	enableOutput(&handlerPWM);

	    	LCD_ClearScreen(&handlerLCD);
			LCD_setCursor(&handlerLCD,0,1);
			LCD_sendSTR(&handlerLCD,"Detecto movimiento");
			LCD_setCursor(&handlerLCD,2,7);
			LCD_sendSTR(&handlerLCD,"Alerta!");
			LCD_setCursor(&handlerLCD,3,0);
			LCD_sendSTR(&handlerLCD,"Por favor desactive");

          GPIO_WritePin(&handlerRojo, SET);

		// Se baja la bandera para otra interrupción
		Code = 0;

		Puerta = 0;


            }

        /* Función encargada de activar solo puerta */

        if(movimiento_puerta == MOVIMIENTO){

        	frequencyValue = 18000;
			duttyValue     = 15000;
			updateFrequency(&handlerPWM,frequencyValue);
			updateDuttyCycle(&handlerPWM, duttyValue);
			startPwmSignal(&handlerPWM);
			enableOutput(&handlerPWM);

			LCD_ClearScreen(&handlerLCD);
			LCD_setCursor(&handlerLCD,0,1);
			LCD_sendSTR(&handlerLCD,"Detecto apertura");
			LCD_setCursor(&handlerLCD,2,7);
			LCD_sendSTR(&handlerLCD,"Alerta!");
			LCD_setCursor(&handlerLCD,3,0);
			LCD_sendSTR(&handlerLCD,"Por favor desactive");

		  GPIO_WritePin(&handlerRojo, SET);

		  movimiento_puerta = NOTHING;

        }

        /* Función que se encarga de apagar la alarma */

        if( desactivar_puerta == 1){
             disableOutPut(&handlerPWM);
//        	 frequencyValue = 0;
//        	 duttyValue     = 0;
//
//        	updateDuttyCycle(&handlerPWM, duttyValue );
//        	updateFrequency(&handlerPWM, frequencyValue);
        	stopPwmSignal(&handlerPWM);




        	LCD_ClearScreen(&handlerLCD);
			LCD_setCursor(&handlerLCD,0,3);
			LCD_sendSTR(&handlerLCD,"Se Desactivo");
			LCD_setCursor(&handlerLCD,1,1);
			LCD_sendSTR(&handlerLCD,"Recuerda activarla");
			LCD_setCursor(&handlerLCD,2,0);
			LCD_sendSTR(&handlerLCD,"Para retornar usa D");
			LCD_setCursor(&handlerLCD,3,0);
			LCD_sendSTR(&handlerLCD,"Alarmas German S.A.S");


        	 GPIO_WritePin(&handlerRojo, RESET);

        	 desactivar_puerta = 0;
        	 movimiento_puerta = 0;

        }

	      /* Comunicacion serial */

	      if (rxData != '\0'){
	      			writeChar(&handlerUsart, rxData);

	      			if (rxData == 'd'){



	      				LCD_ClearScreen(&handlerLCD);

	      				LCD_setCursor(&handlerLCD,0,1);
	      				LCD_sendSTR(&handlerLCD,"Correcion para de  prueba mas adelante");

//	      				sprintf (bufferData, "Vamos %u \n", (unsigned int) Bandera);
	      				writeMsg(&handlerUsart, "Mazato \n");
	      				rxData = '\0';
	      			}

	      			else if (rxData == 'u'){

	      				// Lanzamos un nuevo ciclo de adquisiciones

	      				sprintf (bufferData, "dutty = %u \n", (unsigned int) Bandera);
	      				writeMsg(&handlerUsart, bufferData);

	      			}
	      		}


			if(rxData != '\0'){


			bufferReception[counterReception] = rxData;
			counterReception++;
				 if (rxData == '@'){
					 stringComplete = true;

					 // Agrego linea

					 bufferReception[counterReception] = '\0';

					 counterReception = 0;
				 }

				 rxData = '\0';

		   }


     /********************************************************************************************************************************************/
//			  read_keypad(stateKeypad);

			/* Configuración Menu del KEYPAD */


			if(estadoPantalla == INICIO){

              if((stateKeypad == TECLA_UNO) || (stateKeypad == TECLA_DOS) ||(stateKeypad == TECLA_TRES) || (stateKeypad == TECLA_CUARTRO) ){


            	LCD_ClearScreen(&handlerLCD);
            	LCD_setCursor(&handlerLCD,2,0);
				LCD_sendSTR(&handlerLCD,"____________________");
				LCD_setCursor(&handlerLCD,0,0);
				LCD_sendSTR(&handlerLCD,"Alarmas German S.A.S");
				LCD_setCursor(&handlerLCD,1,4);
				LCD_sendSTR(&handlerLCD,"Bienvenidos    ");
				LCD_setCursor(&handlerLCD,3,0);
				LCD_sendSTR(&handlerLCD,"Menu con las Letras");
            	stateKeypad = NOTHING;


              }

              if((stateKeypad == TECLA_A) || (stateKeypad == TECLA_B) || (stateKeypad == TECLA_C) || (stateKeypad == TECLA_D)) {


            	LCD_ClearScreen(&handlerLCD);
			  	LCD_setCursor(&handlerLCD,0,0);
			  	LCD_sendSTR(&handlerLCD,"A| Configuracion");
			  	LCD_setCursor(&handlerLCD,1,0);
			  	LCD_sendSTR(&handlerLCD,"B| Parametros");
			  	LCD_setCursor(&handlerLCD,2,0);
			  	LCD_sendSTR(&handlerLCD,"C| Fecha y hora");
			  	LCD_setCursor(&handlerLCD,3,0);
			  	LCD_sendSTR(&handlerLCD,"D| Atras");


			   posicionMenu = ENTRO; // Case de opcion
			   estadoPantalla = MENUS; // No deja que se produzca el menu de inicio
			   visualPantalla = MENU_CONF; // Para saber que caso

			   stateKeypad = NOTHING;


              }
			}

			/************************************************************************/

			/* Configuración para  el menu CONFIGURACIÓN del menu */

			if(estadoPantalla == CONFIGURACION){

				if((stateKeypad == TECLA_A)){

					LCD_ClearScreen(&handlerLCD);
					LCD_setCursor(&handlerLCD,0,0);
					LCD_sendSTR(&handlerLCD,"A| Into to password");
					LCD_setCursor(&handlerLCD,1,0);
					LCD_sendSTR(&handlerLCD,"The password=");
					LCD_setCursor(&handlerLCD,2,6);
					LCD_sendSTR(&handlerLCD,"Digitos");
					LCD_setCursor(&handlerLCD,3,0);
					LCD_sendSTR(&handlerLCD,"D|atras");

                   posicionMenu = ACTIVAR_A;
				   estadoPantalla = MENUS;
				   visualPantalla = CONFIGURACION;

				   stateKeypad = NOTHING;
				}


				if((stateKeypad == TECLA_B)){

					LCD_ClearScreen(&handlerLCD);
					LCD_setCursor(&handlerLCD,0,0);
					LCD_sendSTR(&handlerLCD,"B| Into to password");
					LCD_setCursor(&handlerLCD,1,0);
					LCD_sendSTR(&handlerLCD,"The password=");
					LCD_setCursor(&handlerLCD,2,6);
					LCD_sendSTR(&handlerLCD,"Digitos");
					LCD_setCursor(&handlerLCD,3,0);
					LCD_sendSTR(&handlerLCD,"D|atras");

					posicionMenu = ACTIVAR_S;
				   estadoPantalla = MENUS;
				   visualPantalla = CONFIGURACION;

				   stateKeypad = NOTHING;

				}

				if((stateKeypad == TECLA_C)){

					LCD_ClearScreen(&handlerLCD);
					LCD_setCursor(&handlerLCD,0,0);
					LCD_sendSTR(&handlerLCD,"C| Into to password");
					LCD_setCursor(&handlerLCD,1,0);
					LCD_sendSTR(&handlerLCD,"The password=");
					LCD_setCursor(&handlerLCD,2,6);
					LCD_sendSTR(&handlerLCD,"Digitos");
					LCD_setCursor(&handlerLCD,3,0);
					LCD_sendSTR(&handlerLCD,"D|atras");

				   posicionMenu = DESACTIVAR;
				   estadoPantalla = MENUS;
				   visualPantalla = CONFIGURACION;

				   stateKeypad = NOTHING;
				}

				if((stateKeypad == TECLA_D)){

					LCD_ClearScreen(&handlerLCD);
					LCD_setCursor(&handlerLCD,0,0);
					LCD_sendSTR(&handlerLCD,"A| Configuracion");
					LCD_setCursor(&handlerLCD,1,0);
					LCD_sendSTR(&handlerLCD,"B| Parametros");
					LCD_setCursor(&handlerLCD,2,0);
					LCD_sendSTR(&handlerLCD,"C| Fecha y hora");
					LCD_setCursor(&handlerLCD,3,0);
					LCD_sendSTR(&handlerLCD,"D| Atras");

					posicionMenu = ENTRO; // Case de opcion
				   estadoPantalla = MENUS; // No deja que se produzca el menu de inicio
				   visualPantalla = MENU_CONF; // Para saber que caso

				   stateKeypad = NOTHING;
				}
			}

            /**************************************************************************/

			/* Configuración de la opcion PARAMETROS */

			if(estadoPantalla == PARAMETROS){


				if((stateKeypad == TECLA_A)){

					LCD_ClearScreen(&handlerLCD);
					LCD_setCursor(&handlerLCD,0,0);
					LCD_sendSTR(&handlerLCD,"A| Ingrese la actual");
					LCD_setCursor(&handlerLCD,1,0);
					LCD_sendSTR(&handlerLCD,"Los valores son =");
					LCD_setCursor(&handlerLCD,2,6);
					LCD_sendSTR(&handlerLCD,"Digitos");
					LCD_setCursor(&handlerLCD,3,0);
					LCD_sendSTR(&handlerLCD,"D|atras");

				   posicionMenu = CONTRASE;
				   estadoPantalla = MENUS;
				   visualPantalla = CONFIGURACION;

				   stateKeypad = NOTHING;


				}

				if((stateKeypad == TECLA_B)){


					updateDuttyCycle(&handlerPWM, duttyValue);
					LCD_ClearScreen(&handlerLCD);
					LCD_setCursor(&handlerLCD,0,0);
					LCD_sendSTR(&handlerLCD,"Configuracion sonido");
					sprintf(dataLCD, "El valor es = %u ", (unsigned int) duttyValue );
					LCD_setCursor(&handlerLCD,1,0);
					LCD_sendSTR(&handlerLCD,dataLCD);


//					LCD_ClearScreen(&handlerLCD);
//					LCD_setCursor(&handlerLCD,0,0);
//					LCD_sendSTR(&handlerLCD,"Valor actual =");
//					LCD_setCursor(&handlerLCD,1,0);
//					LCD_sendSTR(&handlerLCD,"Nuevo valor =");
//					LCD_setCursor(&handlerLCD,2,6);
//					LCD_sendSTR(&handlerLCD,"Digitos");
//					LCD_setCursor(&handlerLCD,3,0);
//					LCD_sendSTR(&handlerLCD,"D|atras");

				   posicionMenu = SONIDO;
				   estadoPantalla = MENUS;
				   visualPantalla = CONFIGURACION;

				   stateKeypad = NOTHING;

				}

				if((stateKeypad == TECLA_C)){


				}

				if((stateKeypad == TECLA_D)){

					LCD_ClearScreen(&handlerLCD);
					LCD_setCursor(&handlerLCD,0,0);
					LCD_sendSTR(&handlerLCD,"A| Configuracion");
					LCD_setCursor(&handlerLCD,1,0);
					LCD_sendSTR(&handlerLCD,"B| Parametros");
					LCD_setCursor(&handlerLCD,2,0);
					LCD_sendSTR(&handlerLCD,"C| Fecha y hora");
					LCD_setCursor(&handlerLCD,3,0);
					LCD_sendSTR(&handlerLCD,"D| Atras");

					posicionMenu = ENTRO; // Case de opcion
				   estadoPantalla = MENUS; // No deja que se produzca el menu de inicio
				   visualPantalla = MENU_CONF; // Para saber que caso

				   stateKeypad = NOTHING;


				}
			} // Fin de parametros opciones



			/**********************************************************************/

			/* Dentro de parametros para tener actualizaciones de sonido */

//			if(estadoPantalla ==  SONIDO){
//
//				//Subir
//				if(stateKeypad == TECLA_ESTRELLA){
//
//
//					duttyValue += 100;
//
//
//					updateDuttyCycle(&handlerPWM, duttyValue);
//					sprintf(dataLCD, "El nuevo valor es %u ", (unsigned int) duttyValue );
//				    LCD_setCursor(&handlerLCD,1,0);
//					LCD_sendSTR(&handlerLCD,dataLCD);
//
//
//
////					LCD_ClearScreen(&handlerLCD);
////					LCD_setCursor(&handlerLCD,0,0);
////					LCD_sendSTR(&handlerLCD,"A| Configuracion");
////					LCD_setCursor(&handlerLCD,1,0);
////					LCD_sendSTR(&handlerLCD,"B| Parametros");
////					LCD_setCursor(&handlerLCD,2,0);
////					LCD_sendSTR(&handlerLCD,"C| Fecha y hora");
////					LCD_setCursor(&handlerLCD,3,0);
////					LCD_sendSTR(&handlerLCD,"D| Atras");
//
//
//					stateKeypad = NOTHING;
//
//				}
//
//				// Bajar
//
//				if(stateKeypad == TECLA_NUMERAL){
//
//					duttyValue -= 100;
//
//					updateDuttyCycle(&handlerPWM, duttyValue);
//					sprintf(dataLCD, "El nuevo valor es %u ", (unsigned int) duttyValue );
//					LCD_setCursor(&handlerLCD,1,0);
//					LCD_sendSTR(&handlerLCD,dataLCD);
//
//					stateKeypad = NOTHING;
//				}
//
//                 /* Devolverse o retorna */
//
//				if(stateKeypad == TECLA_D){
//
//
//					stateKeypad = NOTHING;
//
//				}
//
//
//			}







			/******************************************************************/

			/* Configuración para configuraciones de fecha y hora */

			if(estadoPantalla == FECHAYHORA){

			if((stateKeypad == TECLA_D)){

				LCD_ClearScreen(&handlerLCD);
				LCD_setCursor(&handlerLCD,0,0);
				LCD_sendSTR(&handlerLCD,"A| Configuracion");
				LCD_setCursor(&handlerLCD,1,0);
				LCD_sendSTR(&handlerLCD,"B| Parametros");
				LCD_setCursor(&handlerLCD,2,0);
				LCD_sendSTR(&handlerLCD,"C| Fecha y hora");
				LCD_setCursor(&handlerLCD,3,0);
				LCD_sendSTR(&handlerLCD,"D| Atras");

				posicionMenu = ENTRO; // Case de opcion
			   estadoPantalla = MENUS; // No deja que se produzca el menu de inicio
			   visualPantalla = MENU_CONF; // Para saber que caso

			   stateKeypad = NOTHING;


			}
		}



			/***************************************************************/

			/* Se encarga de leer letras ya en el menu DESPUES DE INICIO */

           if(visualPantalla == MENU_CONF){

        	   if(stateKeypad == TECLA_A){

       		   visualPantalla  = SALIO;

			   seleccionConfiguracion = CONFIGURACION;

			   stateKeypad = NOTHING;

        	   }
        	   if(stateKeypad == TECLA_B){


        		   visualPantalla  = SALIO;

				   seleccionConfiguracion = PARAMETROS;

				   stateKeypad = NOTHING;

        	          	   }

        	   if(stateKeypad == TECLA_C){


        		   visualPantalla  = SALIO;

				   seleccionConfiguracion = FECHAYHORA;

				   stateKeypad = NOTHING;

        	          	   }

        	   if(stateKeypad == TECLA_D){

        		   visualPantalla  = SALIO;

        		   seleccionConfiguracion = ATRAS;

        		   stateKeypad = NOTHING;

        	          	   }

           } // FIN DE OPCIÓN 1 DE INICIO, LAS DISTINTAS OPCIONES


           /* Se encarga de opcion despues de oprimir A para pasar activar alarma */

       	/* Configuracion para pantalla a seguir en configuracion */

       			if(visualPantalla == CONFIGURACION){

       		    if(stateKeypad == TECLA_A){

				   visualPantalla  = SALIO;

				   seleccionConfiguracion = ACTIVAR_A;

				    stateKeypad = NOTHING;

       		   }
       			 if(stateKeypad == TECLA_B){


       			   visualPantalla  = SALIO;
       			   seleccionConfiguracion = ACTIVAR_S;

       			   stateKeypad = NOTHING;

       			        	          	   }

       			   if(stateKeypad == TECLA_C){


       			   visualPantalla  = SALIO;

       			   seleccionConfiguracion = DESACTIVAR;

       			   stateKeypad = NOTHING;

       			        	          	   }

       			   if(stateKeypad == TECLA_D){

       			   visualPantalla  = SALIO;

       			   seleccionConfiguracion = ATRAS;

       			   stateKeypad = NOTHING;

       			        	          	   }

       			           } // FIN DE OPCIÓN MENU CONFIGURACIÓN




       			/*****************************************************************/


       			/* Se encarga de la opción B despues del inicio, osea de parametros */

       			if(visualPantalla == PARAMETROS){

					if(stateKeypad == TECLA_A){

					   visualPantalla  = SALIO;

					   seleccionConfiguracion = CONTRASE;

						stateKeypad = NOTHING;

					   }
					 if(stateKeypad == TECLA_B){


					   visualPantalla  = SALIO;
					   seleccionConfiguracion = SONIDO;

					   stateKeypad = NOTHING;

											   }

					   if(stateKeypad == TECLA_C){


					   visualPantalla  = SALIO;

					   seleccionConfiguracion = PANICO;

					   stateKeypad = NOTHING;

												   }

						   if(stateKeypad == TECLA_D){

						   visualPantalla  = SALIO;

						   seleccionConfiguracion = ATRAS;

						   stateKeypad = NOTHING;

												   }

       			       			           } // FIN DE OPCIÓN MENU PARAMETROS

       			/* OPCION DE FECHA Y HORA */

       			if(visualPantalla == FECHAYHORA){

				if(stateKeypad == TECLA_A){

				   visualPantalla  = SALIO;

				   seleccionConfiguracion = CAMBIAR_FECHA;

					stateKeypad = NOTHING;

			   }

				 if(stateKeypad == TECLA_B){


				   visualPantalla  = SALIO;
				   seleccionConfiguracion = CAMBIAR_HORA;

				   stateKeypad = NOTHING;

										   }

				   if(stateKeypad == TECLA_C){


				   visualPantalla  = SALIO;

				   seleccionConfiguracion = EVENTOS;

				   stateKeypad = NOTHING;

										   }

				   if(stateKeypad == TECLA_D){

				   visualPantalla  = SALIO;

				   seleccionConfiguracion = ATRAS;

				   stateKeypad = NOTHING;

										   }

								   } // FIN DE OPCIÓN MENU FECHA Y HORA




           /****************************************************************/

              if(posicionMenu == ENTRO){

            	  switch(seleccionConfiguracion){

            	  case CONFIGURACION:{

            		  stateKeypad = NOTHING;
         		      posicionMenu = CONFIGURACION;

            		  break;
            	  }

            	  case PARAMETROS:{
            		  stateKeypad = NOTHING;
            		  posicionMenu = PARAMETROS;

            		  break;
            	  }

            	  case FECHAYHORA:{

            		  stateKeypad = NOTHING;
            		  posicionMenu = FECHAYHORA;

            		  break;
            	  }

            	  case ATRAS:{

            		  stateKeypad = NOTHING;
            		  posicionMenu = ATRAS;

            		  break;
            	  }

            	  default:

            		  __NOP();

            	  }
              }

              /* Estados de pantalla */

              if(posicionMenu == ATRAS){

            	LCD_ClearScreen(&handlerLCD);
				LCD_setCursor(&handlerLCD,2,0);
				LCD_sendSTR(&handlerLCD,"____________________");
				LCD_setCursor(&handlerLCD,0,0);
				LCD_sendSTR(&handlerLCD,"Alarmas German S.A.S");
				LCD_setCursor(&handlerLCD,1,4);
				LCD_sendSTR(&handlerLCD,"Bienvenidos    ");
				LCD_setCursor(&handlerLCD,3,0);
				LCD_sendSTR(&handlerLCD,"Menu con las Letras");

              seleccionConfiguracion = NOTHING;
			  stateKeypad = NOTHING;
			  estadoPantalla = INICIO;

			  posicionMenu = SALIO;
              }

              /* Configuración de atras de la OPCION configuración */

              if(posicionMenu == ATRAS_CONFI){


            		LCD_ClearScreen(&handlerLCD);
					LCD_setCursor(&handlerLCD,0,0);
					LCD_sendSTR(&handlerLCD,"A| Configuracion");
					LCD_setCursor(&handlerLCD,1,0);
					LCD_sendSTR(&handlerLCD,"B| Parametros");
					LCD_setCursor(&handlerLCD,2,0);
					LCD_sendSTR(&handlerLCD,"C| Fecha y hora");
					LCD_setCursor(&handlerLCD,3,0);
					LCD_sendSTR(&handlerLCD,"D| Atras");

					seleccionConfiguracion = NOTHING;
					posicionMenu = SALIO;
					stateKeypad = NOTHING;
					estadoPantalla = MENUS;



              }

              /* Configuración para configuración despues de inicio */

              if(posicionMenu == CONFIGURACION){

            	  	LCD_ClearScreen(&handlerLCD);
            	  	LCD_setCursor(&handlerLCD,0,0);
            	  	LCD_sendSTR(&handlerLCD,"A| Activar alarmas");
            	  	LCD_setCursor(&handlerLCD,1,0);
            	  	LCD_sendSTR(&handlerLCD,"B| Activar sensor");
            	  	LCD_setCursor(&handlerLCD,2,0);
            	  	LCD_sendSTR(&handlerLCD,"C| Desactivar");
            	  	LCD_setCursor(&handlerLCD,3,0);
            	  	LCD_sendSTR(&handlerLCD,"D| Atras");

                    seleccionConfiguracion = NOTHING;
                    estadoPantalla = CONFIGURACION;
            	  	stateKeypad = NOTHING;

            	  	posicionMenu = SALIO;

              }

              /* Configuración para parametros despues de inicio */


              if(posicionMenu == PARAMETROS){

					LCD_ClearScreen(&handlerLCD);
					LCD_setCursor(&handlerLCD,0,0);
					LCD_sendSTR(&handlerLCD,"A|Cambiar contrasena");
					LCD_setCursor(&handlerLCD,1,0);
					LCD_sendSTR(&handlerLCD,"B|Cambiar sonido");
					LCD_setCursor(&handlerLCD,2,0);
					LCD_sendSTR(&handlerLCD,"C|Panico");
					LCD_setCursor(&handlerLCD,3,0);
					LCD_sendSTR(&handlerLCD,"D| Atras");

					seleccionConfiguracion = NOTHING;
					estadoPantalla = PARAMETROS;
					stateKeypad = NOTHING;

			        posicionMenu = SALIO;

                            }

              if(posicionMenu == FECHAYHORA){

            	 LCD_ClearScreen(&handlerLCD);
				LCD_setCursor(&handlerLCD,0,0);
				LCD_sendSTR(&handlerLCD,"A|Cambiar Fecha");
				LCD_setCursor(&handlerLCD,1,0);
				LCD_sendSTR(&handlerLCD,"B|Cambiar Hora");
				LCD_setCursor(&handlerLCD,2,0);
				LCD_sendSTR(&handlerLCD,"C|Eventos");
				LCD_setCursor(&handlerLCD,3,0);
				LCD_sendSTR(&handlerLCD,"D| Atras");

				seleccionConfiguracion = NOTHING;
				estadoPantalla = FECHAYHORA;
				stateKeypad = NOTHING;

				posicionMenu = SALIO;


              }

 /******************************************************************************************************/

              /* Configuraciones particulares */

              if(posicionMenu == ACTIVAR_A){

            	  if(stateKeypad == TECLA_ESTRELLA){


					    LCD_ClearScreen(&handlerLCD);
						LCD_setCursor(&handlerLCD,0,2);
						LCD_sendSTR(&handlerLCD,"La alarma activa");
						LCD_setCursor(&handlerLCD,1,0);
						LCD_sendSTR(&handlerLCD,"pulsa D para retonar");
						LCD_setCursor(&handlerLCD,2,2);
						LCD_sendSTR(&handlerLCD,"Felices fiestas");
						LCD_setCursor(&handlerLCD,3,0);
						LCD_sendSTR(&handlerLCD,"Alarmas German S.A.S");

            		  movimiento_puerta = 1;

            		  seleccionConfiguracion = NOTHING;
					  estadoPantalla = CONFIGURACION;
					  stateKeypad = NOTHING;

					  posicionMenu = SALIO;
            	  }


            	  if(stateKeypad == TECLA_D){

            		  LCD_ClearScreen(&handlerLCD);
					LCD_setCursor(&handlerLCD,0,0);
					LCD_sendSTR(&handlerLCD,"A| Activar alarmas");
					LCD_setCursor(&handlerLCD,1,0);
					LCD_sendSTR(&handlerLCD,"B| Activar sensor");
					LCD_setCursor(&handlerLCD,2,0);
					LCD_sendSTR(&handlerLCD,"C| Desactivar");
					LCD_setCursor(&handlerLCD,3,0);
					LCD_sendSTR(&handlerLCD,"D| Atras");

					seleccionConfiguracion = NOTHING;
					estadoPantalla = CONFIGURACION;
					stateKeypad = NOTHING;

					posicionMenu = SALIO;


            	  }
              }
              /* Opción activar un solo sensor (movimiento) */


              if(posicionMenu == ACTIVAR_S){


            	if(stateKeypad == TECLA_CERO){

				LCD_ClearScreen(&handlerLCD);
				LCD_setCursor(&handlerLCD,0,2);
				LCD_sendSTR(&handlerLCD,"La alarma Puerta");
				LCD_setCursor(&handlerLCD,1,0);
				LCD_sendSTR(&handlerLCD,"pulsa 'D' para atras");
				LCD_setCursor(&handlerLCD,2,2);
				LCD_sendSTR(&handlerLCD,"Felices fiestas");
				LCD_setCursor(&handlerLCD,3,0);
				LCD_sendSTR(&handlerLCD,"Alarmas German S.A.S");

			   movimiento_puerta = 1;

			  seleccionConfiguracion = NOTHING;
			  estadoPantalla = CONFIGURACION;
			  desactivar_puerta = MOVIMIENTO; // Sensor puerta
			  stateKeypad = NOTHING;

			  posicionMenu = SALIO;
		  }


		  if(stateKeypad == TECLA_D){

			  LCD_ClearScreen(&handlerLCD);
			LCD_setCursor(&handlerLCD,0,0);
			LCD_sendSTR(&handlerLCD,"A| Activar alarmas");
			LCD_setCursor(&handlerLCD,1,0);
			LCD_sendSTR(&handlerLCD,"B| Activar sensor");
			LCD_setCursor(&handlerLCD,2,0);
			LCD_sendSTR(&handlerLCD,"C| Desactivar");
			LCD_setCursor(&handlerLCD,3,0);
			LCD_sendSTR(&handlerLCD,"D| Atras");

			seleccionConfiguracion = NOTHING;
			estadoPantalla = CONFIGURACION;
			stateKeypad = NOTHING;

			posicionMenu = SALIO;


		  }
              }

             /* Opción para desactivar */

              if(posicionMenu == DESACTIVAR){

            	  if(stateKeypad == TECLA_NUMERAL){

            		  seleccionConfiguracion = NOTHING;
            		  estadoPantalla = CONFIGURACION;
            		  desactivar_puerta = 1;
            		  posicionMenu = SALIO;
            		  stateKeypad = NOTHING;
            	  }


            	  if(stateKeypad == TECLA_D){

				 LCD_ClearScreen(&handlerLCD);
				LCD_setCursor(&handlerLCD,0,0);
				LCD_sendSTR(&handlerLCD,"A| Activar alarmas");
				LCD_setCursor(&handlerLCD,1,0);
				LCD_sendSTR(&handlerLCD,"B| Activar sensor");
				LCD_setCursor(&handlerLCD,2,0);
				LCD_sendSTR(&handlerLCD,"C| Desactivar");
				LCD_setCursor(&handlerLCD,3,0);
				LCD_sendSTR(&handlerLCD,"D| Atras");

				 seleccionConfiguracion = NOTHING;
				 estadoPantalla = CONFIGURACION;
				stateKeypad = NOTHING;

				posicionMenu = SALIO;
              }

              }

              /************************************************************/

              /* Otras confis --> Parametros */

              if(posicionMenu == CONTRASE){

            	  if(stateKeypad == TECLA_A){

            		  seleccionConfiguracion = NOTHING;
					  estadoPantalla = PARAMETROS;
					  posicionMenu = SALIO;
					  stateKeypad = NOTHING;
            	  }

            	  if(stateKeypad == TECLA_D){


            		LCD_ClearScreen(&handlerLCD);
					LCD_setCursor(&handlerLCD,0,0);
					LCD_sendSTR(&handlerLCD,"A|Cambiar contrasena");
					LCD_setCursor(&handlerLCD,1,0);
					LCD_sendSTR(&handlerLCD,"B|Cambiar sonido");
					LCD_setCursor(&handlerLCD,2,0);
					LCD_sendSTR(&handlerLCD,"C|Panico");
					LCD_setCursor(&handlerLCD,3,0);
					LCD_sendSTR(&handlerLCD,"D| Atras");

            	 	 seleccionConfiguracion = NOTHING;
            	 	 estadoPantalla = PARAMETROS;
            	 	 stateKeypad = NOTHING;

            	 	  posicionMenu = SALIO;

            	               }

              } // Fin de contrase


              /* Configuración para sonido  */

          if(posicionMenu == SONIDO){




        	if(stateKeypad == TECLA_ESTRELLA){


			duttyValue += 100;


			updateDuttyCycle(&handlerPWM, duttyValue);
			LCD_Clear(&handlerLCD);
			LCD_setCursor(&handlerLCD,0,0);
			LCD_sendSTR(&handlerLCD,"Configuracion sonido");
			sprintf(dataLCD, "Nuevo valor = %u ", (unsigned int) duttyValue );
			LCD_setCursor(&handlerLCD,1,0);
			LCD_sendSTR(&handlerLCD,dataLCD);
			LCD_setCursor(&handlerLCD,3,1);
			LCD_sendSTR(&handlerLCD,"Atras con tecla D");

        	stateKeypad = NOTHING;
        	estadoPantalla = MENUS;
        	posicionMenu = SONIDO;
        	seleccionConfiguracion = NOTHING;

        	  }

        	  // Bajar

			if(stateKeypad == TECLA_NUMERAL){

				duttyValue -= 100;


				updateDuttyCycle(&handlerPWM, duttyValue);
				LCD_Clear(&handlerLCD);
				LCD_setCursor(&handlerLCD,0,0);
				LCD_sendSTR(&handlerLCD,"Configuracion sonido");
				sprintf(dataLCD, "Nuevo valor = %u ", (unsigned int) duttyValue );
				LCD_setCursor(&handlerLCD,1,0);
				LCD_sendSTR(&handlerLCD,dataLCD);
				LCD_setCursor(&handlerLCD,3,1);
				LCD_sendSTR(&handlerLCD,"Atras con tecla D");

				stateKeypad = NOTHING;
				estadoPantalla = MENUS;
				posicionMenu = SONIDO;
				seleccionConfiguracion = NOTHING;
			}

			   /* Devolverse o retorna */

//        	  if(stateKeypad == TECLA_B){
//
//		  seleccionConfiguracion = NOTHING;
//		  estadoPantalla = SONIDO; //  NUEVO SONIDO, OJO VOLVER A PARAMETROS
//		  posicionMenu = SALIO;
//		  stateKeypad = NOTHING;
//
//
//        	            	}

        	  if(stateKeypad == TECLA_D){


			LCD_ClearScreen(&handlerLCD);
			LCD_setCursor(&handlerLCD,0,0);
			LCD_sendSTR(&handlerLCD,"A|Cambiar contrasena");
			LCD_setCursor(&handlerLCD,1,0);
			LCD_sendSTR(&handlerLCD,"B|Cambiar sonido");
			LCD_setCursor(&handlerLCD,2,0);
			LCD_sendSTR(&handlerLCD,"C|Panico");
			LCD_setCursor(&handlerLCD,3,0);
			LCD_sendSTR(&handlerLCD,"D| Atras");

			 seleccionConfiguracion = NOTHING;
			 estadoPantalla = PARAMETROS;
			 stateKeypad = NOTHING;

			  posicionMenu = SALIO;

        	             	               }






          } // Fin de sonido

          if(posicionMenu == PANICO){


          }


		} // While

	    } // Funcion

void initSystem(void){

	//Configurando el Timer2 para que funcione con el Blinky
		handlerBlinkyTimer.ptrTIMx                            = TIM2;
		handlerBlinkyTimer.TIMx_Config.TIMx_mode              = BTIMER_MODE_UP;
		handlerBlinkyTimer.TIMx_Config.TIMx_speed             = BTIMER_SPEED_100us;
		handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable   = 1;
		handlerBlinkyTimer.TIMx_Config.TIMx_period            = 2500;
		BasicTimer_Config(&handlerBlinkyTimer);

		//Configurando el Timer4 para que funcione para el muestreo

		//MUESTREO

		handlerReset .ptrTIMx                            = TIM4;
		handlerReset .TIMx_Config.TIMx_mode              = BTIMER_MODE_UP;
		handlerReset .TIMx_Config.TIMx_speed             = BTIMER_SPEED_1ms;
		handlerReset .TIMx_Config.TIMx_interruptEnable   = 1;
		handlerReset .TIMx_Config.TIMx_period            = 5000;
		BasicTimer_Config(&handlerReset);

	handlerBlinkyPin.pGPIOx                              = GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber       = PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_FAST;
	GPIO_Config(&handlerBlinkyPin);


	/* Configurando los pines sobre los que funciona el I2C1 */
	handlerI2CSCL.pGPIOx								= GPIOB;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_OPENDRAIN;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_HIGH;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinAltFunMode		= AF4;

	GPIO_Config(&handlerI2CSCL);



	/* Configurando los pines sobre los que funciona el I2C1 */
	handlerI2CSDA.pGPIOx								= GPIOB;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinNumber			= PIN_7;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_OPENDRAIN;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_HIGH;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinAltFunMode		= AF4;

	GPIO_Config(&handlerI2CSDA);

	handlerLCD.ptrI2Cx		= I2C1;
	handlerLCD.modeI2C		= I2C_MODE_SM;
	handlerLCD.slaveAddress	= LCD_ADRESS;

	i2c_config(&handlerLCD);


	/* Configuración de entradas sensor */


	handlerSensor.pGPIOx                             = GPIOC;
	handlerSensor.GPIO_PinConfig.GPIO_PinNumber      = PIN_7;
	handlerSensor.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_IN;
	handlerSensor.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
	handlerSensor.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	handlerSensor.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_FAST;

	GPIO_Config(&handlerSensor);

	/* Configuración led a prender */

	handlerRojo.pGPIOx                              = GPIOC;
	handlerRojo.GPIO_PinConfig.GPIO_PinNumber       = PIN_2;
	handlerRojo.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerRojo.GPIO_PinConfig.GPIO_PinOPType       = GPIO_OTYPE_PUSHPULL;
	handlerRojo.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerRojo.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_FAST;

	GPIO_Config(&handlerRojo);

	/* Configuración para Pin externo */

	handlerPuerta.pGPIOx                             = GPIOB;
	handlerPuerta.GPIO_PinConfig.GPIO_PinNumber      = PIN_1;
	handlerPuerta.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_IN;
	handlerPuerta.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
	handlerPuerta.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	handlerPuerta.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_FAST;

	GPIO_Config(&handlerPuerta);

    /* Para PWM del Buzzer */

	handlerBuzzer.pGPIOx                             = GPIOC;
	handlerBuzzer.GPIO_PinConfig.GPIO_PinNumber      = PIN_6;
	handlerBuzzer.GPIO_PinConfig.GPIO_PinMode        = GPIO_MODE_ALTFN;
	handlerBuzzer.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OTYPE_PUSHPULL;
	handlerBuzzer.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerBuzzer.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_OSPEED_FAST;
	handlerBuzzer.GPIO_PinConfig.GPIO_PinAltFunMode  = AF2;


	GPIO_Config(&handlerBuzzer);

	// Handler para el PWM

	handlerPWM.ptrTIMx                               = TIM3;
	handlerPWM.config.channel                        = PWM_CHANNEL_1;
	handlerPWM.config.duttyCicle                     = duttyValue;
    handlerPWM.config.periodo                        = frequencyValue;
    handlerPWM.config.prescaler                      = 16;

    pwm_Config(&handlerPWM);

	/* Configuraciones Exti */

	// Sensor

	handlerExtiSensor.pGPIOHandler  = &handlerSensor ;
	handlerExtiSensor.edgeType      = EXTERNAL_INTERRUPT_RISING_EDGE; // Flanco de bajada


	// Puerta

	handlerExtiPuerta.pGPIOHandler  = &handlerPuerta;
	handlerExtiPuerta.edgeType      = EXTERNAL_INTERRUPT_RISING_EDGE;


	extInt_Config(&handlerExtiSensor);
	extInt_Config(&handlerExtiPuerta);


	    handlerTx.pGPIOx                       			= GPIOA;
		handlerTx.GPIO_PinConfig.GPIO_PinNumber			= PIN_2;				// Pin TX USART2
		handlerTx.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
		handlerTx.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
		handlerTx.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
		handlerTx.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
		handlerTx.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;					// Función alternativa USART2

		/* Se carga la configuración */
		GPIO_Config(&handlerTx);

		/* Handler para el PIN A3 */
		handlerRx.pGPIOx                       			= GPIOA;
		handlerRx.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;				// Pin RX USART2
		handlerRx.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
		handlerRx.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
		handlerRx.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
		handlerRx.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
		handlerRx.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;					// Función alternativa USART2

		/* Se carga la configuración */
		GPIO_Config(&handlerRx);

		/* Inicialización de la configuración para el USART2*/

		handlerUsart.ptrUSARTx = USART2 ;
		// Configuración del usuario para el pin -->> Paridad activa en modo Even

		handlerUsart.USART_Config.USART_mode          =  USART_MODE_RXTX;       // En modo de transmisión
		handlerUsart.USART_Config.USART_baudrate      =  USART_BAUDRATE_115200;
		handlerUsart.USART_Config.USART_datasize      =  USART_DATASIZE_9BIT;
		handlerUsart.USART_Config.USART_parity        =  USART_PARITY_EVEN;
		handlerUsart.USART_Config.USART_stopbits      =  USART_STOPBIT_1;
		handlerUsart.USART_Config.USART_IntRx         =  USART_RX_INTERRUPT_ENABLE;
		handlerUsart.USART_Config.USART_IntTx         =  USART_TX_INTERRUP_DISABLE;

		/*Cargando la configuración del Usart para la comunicación serial */
		USART_Config(&handlerUsart);

}

/* Configuración del Key Pad */

void ConfigKeyPad(void){

     	handlerF1.pGPIOx                               =  GPIOB;
	    handlerF1.GPIO_PinConfig.GPIO_PinNumber        =  PIN_8;
		handlerF1.GPIO_PinConfig.GPIO_PinMode          =  GPIO_MODE_OUT;
		handlerF1.GPIO_PinConfig.GPIO_PinOPType        =  GPIO_OTYPE_PUSHPULL;
		handlerF1.GPIO_PinConfig.GPIO_PinPuPdControl   =  GPIO_PUPDR_NOTHING;
		handlerF1.GPIO_PinConfig.GPIO_PinSpeed         =  GPIO_OSPEED_FAST;

		handlerF2.pGPIOx                               =  GPIOB;
		handlerF2.GPIO_PinConfig.GPIO_PinNumber        =  PIN_9;
		handlerF2.GPIO_PinConfig.GPIO_PinMode          =  GPIO_MODE_OUT;
		handlerF2.GPIO_PinConfig.GPIO_PinOPType        =  GPIO_OTYPE_PUSHPULL;
		handlerF2.GPIO_PinConfig.GPIO_PinPuPdControl   =  GPIO_PUPDR_NOTHING;
		handlerF2.GPIO_PinConfig.GPIO_PinSpeed         =  GPIO_OSPEED_FAST;

		handlerF3.pGPIOx                               =  GPIOC;
		handlerF3.GPIO_PinConfig.GPIO_PinNumber        =  PIN_9;
		handlerF3.GPIO_PinConfig.GPIO_PinMode          =  GPIO_MODE_OUT;
		handlerF3.GPIO_PinConfig.GPIO_PinOPType        =  GPIO_OTYPE_PUSHPULL;
		handlerF3.GPIO_PinConfig.GPIO_PinPuPdControl   =  GPIO_PUPDR_NOTHING;
		handlerF3.GPIO_PinConfig.GPIO_PinSpeed         =  GPIO_OSPEED_FAST;

		handlerF4.pGPIOx                               =  GPIOC;
		handlerF4.GPIO_PinConfig.GPIO_PinNumber        =  PIN_8;
		handlerF4.GPIO_PinConfig.GPIO_PinMode          =  GPIO_MODE_OUT;
		handlerF4.GPIO_PinConfig.GPIO_PinOPType        =  GPIO_OTYPE_PUSHPULL;
		handlerF4.GPIO_PinConfig.GPIO_PinPuPdControl   =  GPIO_PUPDR_NOTHING;
		handlerF4.GPIO_PinConfig.GPIO_PinSpeed         =  GPIO_OSPEED_FAST;

		handlerC1.pGPIOx                               =  GPIOA;
		handlerC1.GPIO_PinConfig.GPIO_PinNumber        =  PIN_12;
		handlerC1.GPIO_PinConfig.GPIO_PinMode          =  GPIO_MODE_IN;
		handlerC1.GPIO_PinConfig.GPIO_PinOPType        =  GPIO_OTYPE_PUSHPULL;
		handlerC1.GPIO_PinConfig.GPIO_PinPuPdControl   =  GPIO_PUPDR_NOTHING;
		handlerC1.GPIO_PinConfig.GPIO_PinSpeed         =  GPIO_OSPEED_FAST;


		handlerC2.pGPIOx                               =  GPIOA;
		handlerC2.GPIO_PinConfig.GPIO_PinNumber        =  PIN_11;
		handlerC2.GPIO_PinConfig.GPIO_PinMode          =  GPIO_MODE_IN;
		handlerC2.GPIO_PinConfig.GPIO_PinOPType        =  GPIO_OTYPE_PUSHPULL;
		handlerC2.GPIO_PinConfig.GPIO_PinPuPdControl   =  GPIO_PUPDR_NOTHING;
		handlerC2.GPIO_PinConfig.GPIO_PinSpeed         =  GPIO_OSPEED_FAST;

		handlerC3.pGPIOx                               =  GPIOB;
		handlerC3.GPIO_PinConfig.GPIO_PinNumber        =  PIN_14;
		handlerC3.GPIO_PinConfig.GPIO_PinMode          =  GPIO_MODE_IN;
		handlerC3.GPIO_PinConfig.GPIO_PinOPType        =  GPIO_OTYPE_PUSHPULL;
		handlerC3.GPIO_PinConfig.GPIO_PinPuPdControl   =  GPIO_PUPDR_NOTHING;
		handlerC3.GPIO_PinConfig.GPIO_PinSpeed         =  GPIO_OSPEED_FAST;

		handlerC4.pGPIOx                               =  GPIOB;
		handlerC4.GPIO_PinConfig.GPIO_PinNumber        =  PIN_13;
		handlerC4.GPIO_PinConfig.GPIO_PinMode          =  GPIO_MODE_IN;
		handlerC4.GPIO_PinConfig.GPIO_PinOPType        =  GPIO_OTYPE_PUSHPULL;
		handlerC4.GPIO_PinConfig.GPIO_PinPuPdControl   =  GPIO_PUPDR_NOTHING;
		handlerC4.GPIO_PinConfig.GPIO_PinSpeed         =  GPIO_OSPEED_FAST;


	     /* Configuración de handlers */

		GPIO_Config(&handlerF1);
		GPIO_Config(&handlerF2);
		GPIO_Config(&handlerF3);
		GPIO_Config(&handlerF4);
		GPIO_Config(&handlerC1);
		GPIO_Config(&handlerC2);
		GPIO_Config(&handlerC3);
		GPIO_Config(&handlerC4);

		//Lecturas Exti

		handlerF1Ex.pGPIOHandler       = &handlerC1;
		handlerF1Ex.edgeType           = EXTERNAL_INTERRUPT_RISING_EDGE;

		handlerF2Ex.pGPIOHandler       = &handlerC2;
		handlerF2Ex.edgeType           = EXTERNAL_INTERRUPT_RISING_EDGE;

		handlerF3Ex.pGPIOHandler       = &handlerC3;
		handlerF3Ex.edgeType           = EXTERNAL_INTERRUPT_RISING_EDGE;

		handlerF4Ex.pGPIOHandler       = &handlerC4;
		handlerF4Ex.edgeType           = EXTERNAL_INTERRUPT_RISING_EDGE;


		/* Cargando configuración */


		extInt_Config(&handlerF1Ex);
		extInt_Config(&handlerF2Ex);
		extInt_Config(&handlerF3Ex);
		extInt_Config(&handlerF4Ex);



}

void BasicTimer2_CallBack(void){

	handlerLed = !handlerLed; // Activación de Blinky

	Muestreo();

}

void BasicTimer4_CallBack(void){

  reset = 1;

}

void USART2Rx_CallBack(void){

	rxData = getRxData();
}

void callback_extInt7(void){


	Code = 1; // Bandera que da inicio a alerta


}

void callback_extInt11(void){

	Columna_2 = 1; // Bandera que da inicio a alerta de Key

}

void callback_extInt12(void){

	Columna_1 = 1;
}

void callback_extInt13(void){

	Columna_4 = 1;
}

void callback_extInt14(void){

	Columna_3 = 1;
}

void callback_extInt1(void){

	Puerta = 1;
}


void Alarma(void){

	LCD_ClearScreen(&handlerLCD);
	delay_10();

	LCD_setCursor(&handlerLCD,0,0);
	LCD_sendSTR(&handlerLCD,"____________________");
	LCD_setCursor(&handlerLCD,1,1);
	LCD_sendSTR(&handlerLCD,"Alarmas German S.A");
	LCD_setCursor(&handlerLCD,2,4);
	LCD_sendSTR(&handlerLCD,"Bienvenidos");
	LCD_setCursor(&handlerLCD,3,0);
	LCD_sendSTR(&handlerLCD,"Menu Letras");

}

void Muestreo(void){


	if(Columna1){

		GPIO_WritePin(&handlerF1, SET );
	}else{

		GPIO_WritePin(&handlerF1, RESET );
	}

	if(Columna2){

		GPIO_WritePin(&handlerF2, SET );
	}else{

		GPIO_WritePin(&handlerF2, RESET );
	}

	if(Columna3){

		GPIO_WritePin(&handlerF3, SET );
	}else{

		GPIO_WritePin(&handlerF3, RESET );
	}

	if(Columna4){

		GPIO_WritePin(&handlerF4, SET );
	}else{

		GPIO_WritePin(&handlerF4, RESET );
	}

	if(handlerLed){
		//
					// Activa
					GPIO_WritePin(&handlerBlinkyPin,  SET);
		//
				}else{
		//
					GPIO_WritePin(&handlerBlinkyPin, RESET); // Desactiva

	}



}

void read_keypad(uint8_t EstadoBoton){



	switch(EstadoBoton){

	case TECLA_UNO:{

		if(Columna_1){

			GPIO_WritePin(&handlerF1, SET);
			GPIO_WritePin(&handlerF2, RESET);
			GPIO_WritePin(&handlerF3, RESET);
			GPIO_WritePin(&handlerF4, RESET);

			if((GPIO_ReadPin(&handlerF1) == 1) && GPIO_ReadPin(&handlerC1) == 1){

				stateKeypad = TECLA_UNO;

			   }
		     }
		     break;
	        }

	case TECLA_DOS:{

		if(Columna_2){

			GPIO_WritePin(&handlerF1, SET);
			GPIO_WritePin(&handlerF2, RESET);
			GPIO_WritePin(&handlerF3, RESET);
			GPIO_WritePin(&handlerF4, RESET);

			if((GPIO_ReadPin(&handlerF1) == 1) && GPIO_ReadPin(&handlerC2) == 1){

				stateKeypad = TECLA_DOS;
			}
		}

		break;
	 }

	case TECLA_TRES:{

		if(Columna_3){

			GPIO_WritePin(&handlerF1, SET);
			GPIO_WritePin(&handlerF2, RESET);
			GPIO_WritePin(&handlerF3, RESET);
			GPIO_WritePin(&handlerF4, RESET);

			if((GPIO_ReadPin(&handlerF1) == 1) && GPIO_ReadPin(&handlerC3) == 1){

				stateKeypad = TECLA_TRES;

			 }

		 }

		break;
	  }

	case TECLA_CUARTRO:{

		if(Columna_1){

			GPIO_WritePin(&handlerF1, RESET);
			GPIO_WritePin(&handlerF2, SET);
			GPIO_WritePin(&handlerF3, RESET);
			GPIO_WritePin(&handlerF4, RESET);

			if((GPIO_ReadPin(&handlerF2) == 1) && GPIO_ReadPin(&handlerC1) == 1){

				stateKeypad = TECLA_CUARTRO;

			}
		}

		break;
	}

	case TECLA_CINCO:{

		if(Columna_2){

			GPIO_WritePin(&handlerF1, RESET);
			GPIO_WritePin(&handlerF2, SET);
			GPIO_WritePin(&handlerF3, RESET);
			GPIO_WritePin(&handlerF4, RESET);

	if((GPIO_ReadPin(&handlerF2) == 1) && GPIO_ReadPin(&handlerC2) == 1){

		stateKeypad = TECLA_CINCO;

	     }

	   }

	   break;
	}

	case TECLA_SEIS: {

		if(Columna_3){

			GPIO_WritePin(&handlerF1, RESET);
			GPIO_WritePin(&handlerF2, SET);
			GPIO_WritePin(&handlerF3, RESET);
			GPIO_WritePin(&handlerF4, RESET);

		if((GPIO_ReadPin(&handlerF2) == 1) && GPIO_ReadPin(&handlerC3) == 1){

			stateKeypad = TECLA_SEIS;
		}

	  }

	  break;
	}

	case TECLA_SIETE: {

		if(Columna_1){

			GPIO_WritePin(&handlerF1, RESET);
			GPIO_WritePin(&handlerF2, RESET);
			GPIO_WritePin(&handlerF3, SET);
			GPIO_WritePin(&handlerF4, RESET);

			if((GPIO_ReadPin(&handlerF3) == 1) && GPIO_ReadPin(&handlerC1) == 1){

				stateKeypad = TECLA_SIETE;

			}

		}

		break;
	}

	case TECLA_OCHO: {

		if(Columna_2){

			GPIO_WritePin(&handlerF1, RESET);
			GPIO_WritePin(&handlerF2, RESET);
			GPIO_WritePin(&handlerF3, SET);
			GPIO_WritePin(&handlerF4, RESET);


			if((GPIO_ReadPin(&handlerF3) == 1) && GPIO_ReadPin(&handlerC2) == 1){

				stateKeypad = TECLA_OCHO;

			}

		}

		break;
	}

	case TECLA_NUEVE: {

	  if(Columna_3){


			GPIO_WritePin(&handlerF1, RESET);
			GPIO_WritePin(&handlerF2, RESET);
			GPIO_WritePin(&handlerF3, SET);
			GPIO_WritePin(&handlerF4, RESET);


			if((GPIO_ReadPin(&handlerF3) == 1) && GPIO_ReadPin(&handlerC3) == 1){

				stateKeypad = TECLA_NUEVE;

			}

	  }

	  break;
	}

	case TECLA_ESTRELLA:{

		if(Columna_1){

			GPIO_WritePin(&handlerF1, RESET);
			GPIO_WritePin(&handlerF2, RESET);
			GPIO_WritePin(&handlerF3, RESET);
			GPIO_WritePin(&handlerF4, SET);

			if((GPIO_ReadPin(&handlerF4) == 1) && GPIO_ReadPin(&handlerC1) == 1){

				stateKeypad = TECLA_ESTRELLA;

			}
		}

		break;
	}

	case TECLA_NUMERAL:{

		if(Columna_3){

			GPIO_WritePin(&handlerF1, RESET);
			GPIO_WritePin(&handlerF2, RESET);
			GPIO_WritePin(&handlerF3, RESET);
			GPIO_WritePin(&handlerF4, SET);

			if((GPIO_ReadPin(&handlerF4) == 1) && GPIO_ReadPin(&handlerC3) == 1){

				stateKeypad = TECLA_NUMERAL;
			}
		}

		break;
	}

	case TECLA_CERO:{

		if(Columna_2){

			GPIO_WritePin(&handlerF1, RESET);
			GPIO_WritePin(&handlerF2, RESET);
			GPIO_WritePin(&handlerF3, RESET);
			GPIO_WritePin(&handlerF4, SET);


			if((GPIO_ReadPin(&handlerF4) == 1) && GPIO_ReadPin(&handlerC2) == 1){

				stateKeypad = TECLA_CERO;

			}
		}

		break;
	}

	case TECLA_A:{

		if(Columna_4){

			GPIO_WritePin(&handlerF1, SET);
			GPIO_WritePin(&handlerF2, RESET);
			GPIO_WritePin(&handlerF3, RESET);
			GPIO_WritePin(&handlerF4, RESET);

			if((GPIO_ReadPin(&handlerF1) == 1) && GPIO_ReadPin(&handlerC4) == 1){

			stateKeypad = TECLA_A;
			}

		}

		break;
	}

	case TECLA_B:{

		if(Columna_4){

			GPIO_WritePin(&handlerF1, RESET);
			GPIO_WritePin(&handlerF2, SET);
			GPIO_WritePin(&handlerF3, RESET);
			GPIO_WritePin(&handlerF4, RESET);

			if((GPIO_ReadPin(&handlerF2) == 1) && GPIO_ReadPin(&handlerC4) == 1){

				stateKeypad = TECLA_B;

			}

		}
		break;

	}

	case TECLA_C:{

		if(Columna_4){

			GPIO_WritePin(&handlerF1, RESET);
			GPIO_WritePin(&handlerF2, RESET);
			GPIO_WritePin(&handlerF3, SET);
			GPIO_WritePin(&handlerF4, RESET);


			if((GPIO_ReadPin(&handlerF3) == 1) && GPIO_ReadPin(&handlerC4) == 1){

				stateKeypad = TECLA_C;

			}

		}

		break;

	}

	case TECLA_D:{

		if(Columna_4){

			GPIO_WritePin(&handlerF1, RESET);
			GPIO_WritePin(&handlerF2, RESET);
			GPIO_WritePin(&handlerF3, RESET);
			GPIO_WritePin(&handlerF4, SET);

			if((GPIO_ReadPin(&handlerF4) == 1) && GPIO_ReadPin(&handlerC4) == 1){

				stateKeypad = TECLA_D;
			}

		}

		break;
	}


   } // Fin  Switch case
 } // Fin de la función

void config_general(void){

     // El stop es para que no se devuelva la función sin querer

	if((Columna_1)){

	read_keypad(TECLA_UNO);
	read_keypad(TECLA_CUARTRO);
	read_keypad(TECLA_SIETE);
	read_keypad(TECLA_ESTRELLA);

	Columna_1=0;

    }
    if((Columna_2)){

	read_keypad(TECLA_DOS);
	read_keypad(TECLA_CINCO);
	read_keypad(TECLA_OCHO);
	read_keypad(TECLA_CERO);

	Columna_2=0;

    }

    if((Columna_3)){


	read_keypad(TECLA_TRES);
	read_keypad(TECLA_SEIS);
	read_keypad(TECLA_NUEVE);
	read_keypad(TECLA_NUMERAL);

	Columna_3=0;

    }

    	if((Columna_4)){

	read_keypad(TECLA_A);
	read_keypad(TECLA_B);
	read_keypad(TECLA_C);
	read_keypad(TECLA_D);

	Columna_4=0;

    }


}



//void parseCommands(char *ptrBufferReception){
//
//	sscanf(ptrBufferReception, " %s %u %u %s" , cmd , &firstParameter , &secondParameter ,userMsg);
//
//	if(strcmp(cmd , "help") == 0){
//
//		writeMsg(&handlerUsart, "Help Menu CMDs: \n");
//		writeMsg(&handlerUsart, "1) Millos = Colores del grande \n ");
//		writeMsg(&handlerUsart, "2) Nacional= Color verde \n");
//
//
//	}
//
//	else if(strcmp(cmd ,"Millos") == 0){
//
////		   writeMsg(&handlerUsart1, "Color purpura \n \r");
//
//		Bandera = 1;
//
//
//		}
//
//	else if(strcmp(cmd ,"Nacional") == 0){
//
//		updateFrequency(&handlerPWM, 0);
//		  updateDuttyCycle(&handlerPWM, RESET);
//	       stopPwmSignal(&handlerPWM);
//
//
//}
//
//}

