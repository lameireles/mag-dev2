/* 
* Valves.cpp
*/

#include "Valves.h"
#include <avr/io.h>

//===================//
//=== CONSTRUCTOR ===//
//===================//

Valves::Valves()
{
	PORTC_DIRSET = PIN1_bm|PIN2_bm;
	closeInput();
	closeOutput();
}

//========================//
//=== PUBLIC FUNCTIONS ===//
//========================//

void Valves::openInput()
{
	PORTC_OUTSET = PIN1_bm;
	input_open = true;
}

void Valves::openOutput()
{
	PORTC_OUTSET = PIN2_bm;
	output_open = true;
}

void Valves::closeInput()
{
	PORTC_OUTCLR = PIN1_bm;
	input_open = false;
}

void Valves::closeOutput()
{
	PORTC_OUTCLR = PIN2_bm;
	output_open = false;
}

void Valves::toggleInput()
{
	PORTC_OUTTGL = PIN1_bm;
	input_open = !input_open;
}

void Valves::toggleOutput()
{
	PORTC_OUTTGL = PIN2_bm;
	input_open = !input_open;
}
