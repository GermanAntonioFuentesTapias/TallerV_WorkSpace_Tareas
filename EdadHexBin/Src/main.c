/*
 * main.c
 *
 *  Created on: Aug 19, 2022
 *      Author: German Antonio Fuentes Tapias
 *      Estudiante de Ingeniería Física
 *
 *                    E L A B O R A C I Ó N   D E  L A  T A R E A I
 *
 *      <<<<<<<<<Nota= Recordar que cumplo el mismo dia de LuisFelipe>>>>>>>>>>
 */
/**
 *  Se declara las variables necesarias para el funcionamiento de la tarea
 *  Ademas se tomo las variables con declaración inicial cero
 */
/*Se creara una variable de 8 bits que sera suficiente para la edad*/
unsigned char Age = 0; // La edad que se tiene al momento de diseñar el programa

/*Se creara una variable de 16 bits, ya que 8 no es suficiente para los dias del año */
unsigned short N_days_Year = 0; //Numero de dias  en un año sin contar años bisiesto

/*Se creara una variable de 8 bits que es suficiente para las horas del dia*/
unsigned char N_hours_days = 0; //Numero de horas de un dia

/*Se creara una variable de 16 bits, ya que 8 no es suficiente para los segundos de una hora */
unsigned short N_Seconds_hours = 0; //Numero de segundos que se tiene en una hora

/*Se creara una variable de 16 bits, ya que 8 no es suficiente para los dias que se ha vivido desde el nacimiento*/
unsigned short N_days_live = 0; // Numero de dias vividos desde la fecha de nacimiento hasta el 17/08/2022

/*Se creara una variable de 32 bits, ya que 8 ni 16 fueron suficientes para las horas de dias vividos */
unsigned int N_days_live_hours = 0; // Numero de horas vividas desde el dia de nacimiento hasta el 17/08/2022 a medio dia

/*Se creara una variable de 32 bits, ya que 8 ni 16 fueron suficientes para segundos de dias vividos */
unsigned int N_days_live_seconds = 0; // Numero de segundos vividos desde el dia de nacimiento hasta el 17/08/2022 a medio dia

/*Funcion principal del proyecto*/

int main(){
    /* Se definen las 3 variables necesarias durante todo el codigo, las cuales son Año, numero de dias y segundos de una hora*/

	Age = 23; // Mi edad completa desde el 17/11/1998 hasta el 17/11/2021
	N_days_Year = 365;// Los dias del año sin bisiesto
	N_Seconds_hours = 3600;// Segundos que tiene una hora

    /*  La siguiente declaracion se encargara de calcular los dias que hay entre el dia de nacimiento y el 17/08/22*/

	N_days_live = Age * N_days_Year + 272; // 272 los dias que ha entre el 17/11 y el 17/08
			// Se multiplico la edad por el numero de dias del año y se le sumo el 272

/*  La siguiente declaracion se encargara de calcular las horas de los dias que hay
*entre el dia de nacimiento y el 17/08/22 a medio dia*/
	// Se declara una nueva variable a utilizar

	 N_hours_days = 24; // Horas del dia


 /* Variable que contiene en numero de horas desde el dia de mi nacimiento */

	N_days_live_hours = N_days_live * N_hours_days + 12; /* La suma con 12 se debe a que nos dara el resultado hasta el 16/08, asi que haciendo la adicion
	 se tendra hasta el medio dia del 17/08*/

/* Variable que contiene en numero de segundos desde el dia de mi nacimiento hasta 17/08/2022 a las 12 pm*/

	N_days_live_seconds = N_days_live_hours * N_Seconds_hours; // Se calcula la cantidad de segundos, operandose
	//con las horas vividas, por la cantidad de segundos que tiene una hora, llegando asi al resultado de segundos
	//vividos desde el nacimiento

/* Acontinuación se mostrara los numeros de dias y numero de horas en formato bin
 *Primeramente se dara a observar el numero de dias en bin y luego el de horas en bin
 *El resultado de numero de dias tiene 16 bits, pero el de numero de horas tiene 32 bits,
 *Se hara manuealmente y se compara este resultado con el dado por el visualizador de Debugger de Live Expressions
 * Se tienen 8667 dias de vida, lo que nos dice que es requerido 2^13 como minimo, que son 8192
 * de donde 8667-8192 nos da 475 de espacio, lo que nos hace ir hasta 2^8 que es 256, teniedo asi ya:
 * 8448 de espacio, pero aun nos falta 219, por lo que usamos 2^7 y 2^6, falta 27,
 * Yendo a espacio mas bajos se toman 2^4 y 2^3, y para completar 2^1 y 2^0, logrando asi
 * obtener el numero 8667 en Binario que seria entonces = 10000111011011  con su escritura de forma
 *  N_days_live =  0b10000111011011
 *
 *  Ahora el mismo procedimiento para numero de horas 208020 que es de 32 bits
 *  comenzando por 2^17 y haciendo el mismo procedimiento anterio se llega a 110010110010010100
 *  que sera entonces    N_days_live_hours = 0b110010110010010100
 *
 *  Ahora para el numero de segundos es mas favorable presentarlo en Hexagesimal dado la magnitud de este
 *  visto en binario, asi que recordando que en Hexa las letras representan numeros desde A hasta F, y teniendo el numero
 *  se tiene a N_days_live_seconds = 0x2ca2e140 que es mas comodo de ver, que su formato bin o decimal.
 *
 *
 */






  while(1){
  }


  }



