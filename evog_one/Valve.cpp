/* 
* Valve.cpp
*/

#include "Valve.h"
#include <avr/io.h>

Valve::Valve(uint8_t pin)
{
	this->pin = pin;
	PORTC_DIRSET = pin;
	close();
}

void Valve::open()
{
	PORTC_OUTSET = pin;
	open_b = true;
}

void Valve::close()
{
	PORTC_OUTCLR = pin;
	open_b = false;
}

void Valve::toggle()
{
	PORTC_OUTTGL = pin;
	open_b = !open_b;
}