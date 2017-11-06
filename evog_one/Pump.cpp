/* 
* Pump.cpp
*/

#include <avr/io.h>
#include "Pump.h"

//===================//
//=== CONSTRUCTOR ===//
//===================//

Pump::Pump()
{
	PORTC_DIRSET = PIN3_bm;
	turnOff();
}

//========================//
//=== PUBLIC FUNCTIONS ===//
//========================//

void Pump::turnOn()
{
	PORTC_OUTSET = PIN3_bm;
	on = true;
}

void Pump::turnOff()
{
	PORTC_OUTCLR = PIN3_bm;
	on = false;
}

void Pump::toggle()
{
	PORTC_OUTTGL = PIN3_bm;
	on = !on;
}