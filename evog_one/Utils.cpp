/* 
* Utils.cpp
*/

#include "Utils.h"
#define F_CPU 32000000
#include <util/delay.h>
#include <avr/io.h>

char Utils::txBuf[TX_LEN];

void Utils::setSystemClock(systemClock_e sc)
{
	OSC_CTRL |= 1 << sc; // enable oscillator
	while (!(OSC_STATUS & (1 << sc))) ; // wait until oscillator is ready
	CCP = CCP_IOREG_gc; // trigger protection mechanism
	CLK_CTRL = sc << CLK_SCLKSEL_gp; // select system clock
}

void Utils::enableInterruptLevel(interruptLevel_e il)
{
	PMIC_CTRL |= (1 << (il-1));
}

void Utils::globalInterruptEnable()
{
	CPU_SREG |= CPU_I_bm;
}

void Utils::delay_ms(uint16_t u)
{
	for (uint16_t i = 0; i < u; i++) _delay_ms(1);
}

void Utils::delay_us(uint16_t u)
{
	for (uint16_t i = 0; i < u; i++) _delay_us(1);
}