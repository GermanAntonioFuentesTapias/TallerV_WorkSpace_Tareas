/*
 * KeyPad.c
 *
 *  Created on: 24/11/2022
 *      Author: German
 */

#include "KeyPad.h"
#include "GPIOxDriver.h"


//1. Keypad pinout variable
static Keypad_WiresTypeDef KeypadStruct;
//2. OUT pins position, or pin number in decimal for use in colomn change function
static uint8_t OutPositions[4];
//

static char *Keypad_keys[16] =
{
	"1",
	"2",
	"3",
	"A",
	"4",
	"5",
	"6",
	"B",
	"7",
	"8",
	"9",
	"C",
	"*",
	"0",
	"#",
	"D"
};

void Keypad4x4_Init(Keypad_WiresTypeDef  *KeypadWiringStruct){


KeypadStruct = *KeypadWiringStruct;

Keypad4x4_FindPins_positions();




}
