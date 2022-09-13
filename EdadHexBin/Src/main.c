/*
 * main.c
 *
 *  Created on: Aug 19, 2022
 *
 *
 *      Author: German Antonio Fuentes Tapias
 *      Estudiante de Ingeniería Física
 *
 *      Fecha de nacimiento = 17 / 11 / 1998
 *
 *                    E L A B O R A C I Ó N   D E  L A  T A R E A - I
 *
 *      <<<<<<<<<Nota= Recordar que cumplo el mismo dia de Luis Felipe>>>>>>>>>>
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

/*Se creara una variable de 16 bits, ya que 8 no es suficiente para los dias que se ha vivido desde el dia de mi nacimiento*/
unsigned short N_days_live = 0; // Numero de dias vividos desde la fecha de nacimiento hasta el 17/08/2022

/*Se creara una variable de 32 bits, ya que 8 ni 16 fueron suficientes para las horas de dias vividos */
unsigned int N_days_live_hours = 0; // Numero de horas vividas desde el dia de nacimiento hasta el 17/08/2022 a medio dia

/*Se creara una variable de 32 bits, ya que 8 ni 16 fueron suficientes para segundos de dias vividos */
unsigned int N_days_live_seconds = 0; // Numero de segundos vividos desde el dia de nacimiento hasta el 17/08/2022 a medio dia


/*Las siguientes variables estan con fin de no perder los valores iniciales calculados*/
unsigned short N_days_live_Shift = 0; // Se crea esta variable para la operación de shift-izquierdo, en el cual
// se almacenara el resultado de la operación

unsigned short N_days_live_Shift_4 = 0; // Se crea esta variable para la operación de shift-izquierdo cuatro veces

unsigned short N_days_live_hours_Shift_4 = 0; // Se crea esta variable para la operación Shift-derecho cuatro veces

unsigned int N_days_live_hours_Shift = 0;// Se crea esta variable para la operacion de Shift-derecho, en el cual
// se almacenara el resultado de la operación

unsigned short N_days_live_Not = 0; // Se crea esta variable para la operación Not, en el cual se almacenara
//el resultado de la operación

unsigned short V_prueba = 0;// Se crea esta variable para asignarle  otro valor se obtiene el mismo resultaod que con N_days_live
// el cual se encargara de verificar la prueba del punto 8-b
unsigned short V_prueba_Not = 0;// Variable para prueba despues de realizar negación

unsigned int mascara = 0;//Se desarrola una variable para aplicarle la Mascara creada para la operación del punto 9.

unsigned int mascara_10 = 0; // Se desarrolla una variable para aplicarle la mascara a numero de segundos

unsigned int operador_logica = 0; // Variable para la operación ! en el punto 11



/*Desarrollo de la funcion principal del proyecto*/

int main(){

    /* Se definen las 3 variables necesarias durante todo el codigo, las cuales son Año, numero de dias y segundos de una hora*/

	Age = 23; // Mi edad completa desde el 17/11/1998 hasta el 17/11/2021
	// Te costaba mucho poner la efcha de nacimiento en la cabecera del código, te rebajo por ponerme a perder tiempo...
	N_days_Year = 365;// Los dias del año sin año bisiesto
	N_Seconds_hours = 3600;// Segundos que tiene una hora

    /*  La siguiente declaracion se encargara de calcular los dias que hay entre el dia de nacimiento y el 17/08/22*/

	N_days_live = Age * N_days_Year + 272; // 272 los dias que hay entre el 17/11/2021 y el 17/08/2022
			// Se multiplico la edad por el numero de dias del año y se le sumo el 272

/*  La siguiente declaracion se encargara de calcular las horas de los dias que hay
*entre el dia de nacimiento y el 17/08/22 a medio dia*/
	// Se declara una nueva variable a utilizar

	 N_hours_days = 24; // Horas del dia normalmente


 /* Variable que contiene en numero de horas desde el dia de mi nacimiento */

	N_days_live_hours = N_days_live * N_hours_days + 12; /* La suma con 12 se debe a que nos dara el resultado hasta el 16/08, asi que haciendo la adicion
	 se tendra hasta el medio dia del 17/08*/

/* Variable que contiene en numero de segundos desde el dia de mi nacimiento hasta 17/08/2022 a las 12 pm*/

	N_days_live_seconds = N_days_live_hours * N_Seconds_hours; // Se calcula la cantidad de segundos, operandose
	//con las horas vividas, por la cantidad de segundos que tiene una hora, llegando asi al resultado de segundos
	//vivídos desde el nacimiento hasta el medio dia del 17/08/2022

/* A continuación se mostrara los numeros de dias y numero de horas en formato bin
 *
 *Primeramente se dara a observar el numero de dias en bin y luego el de horas en bin
 *El resultado de numero de dias tiene 16 bits, pero el de numero de horas tiene 32 bits,
 *Se hara manuealmente y se compara este resultado con el dado por el visualizador de Debugger de Live Expressions
 *
 * Se tienen 8667 dias de vida, lo cual representa en tamaño de variable como minimo 2^13, que son 8192 del tamaño de la variable,
 * en donde 8667-8192 nos da un resultado de 475 de ,con el resultado anterior se debe ir hasta 2^8 que es 256, teniedo asi ya:
 * 8448 de espacio, pero aun nos queda faltando 219, por lo que se requiere aun 2^7 y 2^6, nos queda faltando 27 para la meta asi que,
 * Yendo a  espacios inferiores,se toman los valores de 2^4 y 2^3, ademas de los tamaños de 2^1 y 2^0, logrando asi
 * obtener el numero 8667 que en Binario que seria entonces = 10000111011011  con su escritura de forma:
 *  N_days_live =  0b10000111011011
 *
 *  Ahora el mismo procedimiento para numero de horas, que tiene un valor de 208020, siendo este de  32 bits
 *  comenzando por 2^17 y realiando el mismo procedimiento anterior,el valor de  2^17 es 131072 de tamaño de variable, faltando aun valores para llegar
 *  a 208020,se toma entonces 2^16 ya que la suma de ambas no superá el valor de 208020, sumando ambas se obtiene el valor de
 *  196608, acercandose al valor que se quiere llegar, por logica se tendra a consideración que el numero ha añadir debe ser
 *  de un espacio de variable mas pequeño,por lo que nos iremos hasta el tamaño de  2^13, acumulando con la suma de este valor 204800, pero aún falta, por lo que se debe
 *  tomar el bit de 2^11, acumulando asi 206848, faltando aun espacio, se añade 2^10 y con este se llega al valor de 207872, estando mas cerca
 *  del valor que se busca. Ahora se le añade 2^7, llegando al valor de 20800, solo nos falta la representación de 20, por lo que nos
 *  vamos hasta 2^4 y 2^2 añadiendolos al acumulado, con lo cual se llega al valor buscado de 208020, por lo que nuestro numero en binario sera 110010110010010100
 *  que  entonces se representa como  N_days_live_hours = 0b110010110010010100
 *
 *  Ahora para el numero de segundos es mas favorable presentarlo en Hexagesimal dado la magnitud de este
 *  visto en binario, que tiene un valor de 0b101100101000101110000101000000, asi que recordando que en Hexa las letras representan numeros desde A hasta F en grupos de 4 bits,
 *  se tiene entonces que los 4 ultimos bits serian 0, los otros 4 siguientes sera el valor de 4 en Hexagesimal, siguiendole el siguiente cuarteto, con valor
 *  de 1 en Hexagesimal y asi sucesivamente hasta cumplir el tamaño total de la variable, teniendo entonces  a N_days_live_seconds = 0x2ca2e140 que es mas comodo en su lectura..
 */

	/*
	 * Se usaron esos tipos de signos porque: Para Age: una variable de 8 bits sera suficiente para la edad, ya que no supera los 255 de espacio de almacenamiento
	 * lo cual es el tamaño apropiado para ser representado el numero, definiendo como "unsigned char", para los dias del año se USA una variable de 16 bits, que es
	 * requerida para el tamaño de numero de dias del año, esta se declaro con "unsigned short" y por ultimo para horas del dia se declaro con "unsigned short" de 8 bits
	 * que era suficiente en tamaño para ocupar la cantidad de horas del dia.
	 *
	 */

	/*Se realiza la operacion shift << sobre la variable N_days_live */

	N_days_live_Shift  = N_days_live << 1; // Al realizar la operación se agrego un cero a la derecha lo que
	// representaria un aumento del espacio o tamaño, lo que se represento como una multiplicación
	// por 2 en la parte decimal y pasando en binario de 0b10000111011011 a -> 0b100110000110
	//Asi que tecnicamente se movio una unidad hacia la izquierda lo que nos representa pasar de
	//2^13 a 2^14 y el cero a la derecha.
    //Asi que en terminos generales la operación Shift-izquierda se encarga de desplazar bit a bit en esa dirección del
	//numero binario, el desplazamiento lo hara y se iran desbordando los numeros en caso que sea 1

	/* Se realiza la operacion shift << de nuevo, pero esta vez se realizara sobre la variable ya operada con shift izquierdo */

	N_days_live_Shift = N_days_live_Shift  << 1; // Despues de realizar la operación, se agrego nuevamente un cero
	// obteniendo asi un valor mayor, lo que nos estaria diciendo que se esta aumentado el valor de la variable
	//como si en cada desplazamiento a la izquierda se multiplicara por 2


	/* Por ultimo se hace el comprobante de que pasa si se hace cuatro veces seguidas */

	N_days_live_Shift_4 = N_days_live << 4; // Por ultimo cuatro veces seguidas causa un desbordamiento
	// o Overflow en la memoria, en nuestro caso, el numero original aguanta 2 << shift,pero
	// en el tercero se desborda haciendo comenzar en un numero menor y volviendo al ciclo de multiplicar
	// por dos los bits que se van aumentando. Esto seria a causa de que no hay espacio disponible en los 16 bits
	// y se "caerian" al no tener donde ocupar mas espacio. Si la variable se declara como int y no como short
	// No pasara ese problema, ya que se seguira desplaando hasta que ocurra el overflow en ese espacio de memoria

	// No explicas que se observa, cual es la operación matemática.


	/* Se realizaran operaciones shif-derecha >> */

	N_days_live_hours_Shift = N_days_live_hours >> 1; //Al realizar la operación se corre todo un bit a la derecha
	//y ya que esto pasa, se observa con esta primera operación del valor como la operación de una división por 2
	// Desplazando asi del numero binario esta unidad y agregando un cero a la izquierda

	/* Se realizara de nueva la operacion shift-derecha, pero con la variable ya operada con un shift-derecho aplicado  */

	N_days_live_hours_Shift = N_days_live_hours_Shift >> 1;//Al realizar la operación se corre todo un bit a la derecha
	// de nuevo, por lo que con esta nueva operacion se observa el numero en binario como = 0b1100101100100101  y se representa como
	// una nueva division por 2, los nuevos ceros a izquierda no se observan


	/* Se realiza 4 shift-derechos seguidos*/

	N_days_live_hours_Shift_4 = N_days_live_hours >> 4;//Operando 4 veces seguidas se estaria desplazando
	// 4 bits hacia la derecha, agregando 4 ceros a la izquierda y todas las 4 unidades cayendo
	// del almacenamiento, esto en decimal se vera como si estuvieramos dividiendo por 16, que seria
	//dividir por 2 en cada desplazamiento. Paso de 0b110010110010010100 a 0b11001011001001
	//esto sin haber operado las >> anteriores de 1 unidad.

	// Falta explicar mejor...


    /* Se hara operacion NOT a numero de dias*/

	/*
	 * La operación NOT es un operador Bit a Bit por lo que se encargara de  cambiar los valores de 0 y 1, por lo que
	 * donde se encuentre un cero, sera reemplazado por uno y donde se encuentre el uno, sera reemplanzado con el cero.
	 */

	N_days_live_Not = ~N_days_live; // Se realiza la operación NOT, que invierte el valor de sus operandos, transformando el valor 0 en 1 y el valor 1 en 0.
	//Esto se hace bit a bit. Teniendo el valor original como 8667 y su operación NOT Siendo 56868,llegando a un tamaño de
	// 65535

	/* Se le suma una unidad*/

	N_days_live_Not = N_days_live_Not + 1; // Sumando esta unidad se obtiene el valor de 56869,

	/* Sumar a la variable original */

	N_days_live_Not = N_days_live_Not + N_days_live; // Se obtiene el valor  de cero
	//Esto ocurre por el espacio de almacenamiento, asi que operacionalmente ya que tenemos unsigned no se ve correctamente la representación,
	//asi que la negación tomaria una parte negativa, por lo tanto en este caso se tomaria el valor de  -8668
	//al sumarle una unidad se obtendria el valor de -8667 y sumandole la original se obtendria el valor de cero, observado esto desde la
	// recta numerica de espacios de memorias de bits.
	// Una explicación adicional, seria verlo con el complemento, al hacer el operador bit-wise y sumarle uno se tendria un complemento de la variable original
	// entonces ya que el complemento tiene signo negativo y al sumarse con la original, se obtiene el valor de cero
	//También se puede ver como el inverso adictivo ya que estamos en variable sin signos.


	/* Se define la variable nueva*/

	// Se realizara la prueba con la nueva variable
	V_prueba = 2856;// Se define la nueva variable

	V_prueba_Not = ~V_prueba; // Se le hace negación a la nueva declaración

	V_prueba_Not = V_prueba_Not + 1; // Se le suma 1 a la nueva declaración

	V_prueba_Not = V_prueba + V_prueba_Not; //Se observa que aplicando otro valor definido anteriormente
	// Se llega al mismo resultado, ya que ocurre lo mismo que con la variable N_days_live


      /* Punto 09*/

	 /* Se hace una mascara observando el formato Hex de numero de segundos */
	// El valor en Hex de numero de segundos o como variable N_days_live_seconds = 0x2ca2e140
	// El numero de 4 contando de derecha a izquierda es 2, por lo que es par,asi que debo anular
	// El resto menos uno y cinco, observando el numero Hex como Bin se tiene como 101100101000101110000101000000
	// Donde la posicion 1 y 5 se encerran entre comillas 00101100"1010"001011100001"0100"0000
    // Asi que la mascara para la siguiente operación sera -->> 0b00000000101000000000000001000000
	//o en forma Hex 0x00f000f0


	mascara = N_days_live_seconds & 0x00f000f0; // Despues de la mascara se mantiene la posicion 1 y 5
    // Obteniendo un resultado de Hex como 0xa00040
	//Ahora se va a realizar una operación And con al siguiente mascara 0x040

	/* Punto 10 */

   mascara_10 = N_days_live_seconds & 0x040;

   // Dado que el valor obtenido despues de aplicar la mascara es diferente de cero
   // particulamente el numero 64, se procedera a borrar la posición 6 Hex

   mascara_10 =  N_days_live_seconds & 0xf0ffffff; // Se obtiene el resultado en Hex de 0x20a2e140
   // observando que en la posición 6 se tiene el cero, borrandolo y dejando las demas posiciones intactas
   // Esto nos podra servir para eliminar un valor desconocido en un registro o variable mas grande y requiriendo eliminar un valor de este
   // sin eliminar los otros bits o modificarlos


   /* Punto 11 */

   //Se usara la variable N_days_live_hours para el procedimiento del punto con un declaración nombrada
   // operador_logica

  operador_logica = !N_days_live_hours; // Al realizarse la primera vez se convirtio todos los valores en cero


   operador_logica = !operador_logica; // Al realizarse de nuevo se obtiene un 0b1, que matematicamente estaria negando el valor de entrada

// De los resultados anteriores se puede decir que: A diferencia de los operadores anteriores este lo hace para todo el conjunto, siendo un operador booleano
   // que se encarga de hacer una compación de verdadero o falso, entonces con la inicial tomo todo el valor como falso y por lo tanto se imprime cero, pero al volverse a operar
   // se obtiene el resultado 1, asi que nos dice que cero era verdadero, y se imprime el 1, si se volviera a proceder pasaria de nuevo el valor de cero y asi sucesivamente
   // Por lo que en general el operador ! significara NO, siendo util para anteponer estados de reloj que se requieran, teniendo condiciones de condicional, pero optimizando los recursos
   // y la cantidad de escritura


}
