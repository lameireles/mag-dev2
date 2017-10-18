/* 
* UsartC1.cpp
*/

#include <avr/interrupt.h>
#include <avr/io.h>
#include "UsartC1.h"

//==================//
//=== INTERRUPTS ===//
//==================//

volatile bool UsartC1::rxcFlag = false;
volatile bool UsartC1::txcFlag = false;
volatile bool UsartC1::dreFlag = false;
volatile char UsartC1::rxChar = '\0';

ISR(USARTC1_RXC_vect) {
	UsartC1::rxcFlag = true;
	UsartC1::rxChar = USARTC1_DATA;
}
ISR(USARTC1_TXC_vect) {UsartC1::txcFlag = true;}
ISR(USARTC1_DRE_vect) {UsartC1::dreFlag = true;}

//===================//
//=== CONSTRUCTOR ===//
//===================//

UsartC1::UsartC1(	Usart::baudrate_s baudrate,
					Utils::interruptLevel_e rxcil,
					Utils::interruptLevel_e txcil,
					Utils::interruptLevel_e dreil)
{
	USARTC1_BAUDCTRLA = baudrate.bsel << USART_BSEL_gp;
	USARTC1_BAUDCTRLB = baudrate.bscale << USART_BSCALE_gp;
	if (baudrate.clk2x) USARTC1_CTRLB |= USART_CLK2X_bm;
	USARTC1_CTRLB |= USART_RXEN_bm|USART_TXEN_bm;
	USARTC1_CTRLA = (rxcil << USART_RXCINTLVL_gp)|(txcil << USART_TXCINTLVL_gp)|(dreil << USART_DREINTLVL_gp);
	PORTC_DIRSET = PIN7_bm;
}

//=================//
//=== FUNCTIONS ===//
//=================//

void UsartC1::sendChar(char c)
{
	while (!(USARTC1_STATUS & USART_DREIF_bm)) ;
	USARTC1_DATA = c;
}

void UsartC1::sendString(const char* str)
{
	int i = 0;
	while (str[i] != '\0') sendChar(str[i++]);
}