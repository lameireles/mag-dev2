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
volatile char UsartC1::rxChar = '\0';

ISR(USARTC1_RXC_vect) {
	UsartC1::rxcFlag = true;
	UsartC1::rxChar = USARTC1_DATA;
}

//===================//
//=== CONSTRUCTOR ===//
//===================//

UsartC1::UsartC1(Usart::baudrate_s baudrate,Utils::interruptLevel_e receiveCompleteInterruptLevel)
{
	setBSEL(baudrate.bsel);
	setBSCALE(baudrate.bscale);
	if (baudrate.clk2x) enableDoubleTransmissionSpeed();
	enableReceiver();
	enableTransmitter();
	enableReceiveCompleteInterrupt(receiveCompleteInterruptLevel);
	PORTC_DIRSET = PIN7_bm;
}

//=========================//
//=== PRIVATE FUNCTIONS ===//
//=========================//

void UsartC1::waitDataRegisterEmpty()
{
	while (!(USARTC1_STATUS & USART_DREIF_bm)) ;
}

//========================//
//=== PUBLIC FUNCTIONS ===//
//========================//

void UsartC1::sendChar(char c)
{
	waitDataRegisterEmpty();
	USARTC1_DATA = c;
}

void UsartC1::sendString(const char* str)
{
	int i = 0;
	while (str[i] != '\0') sendChar(str[i++]);
}

//================//
//=== ENABLERS ===//
//================//

void UsartC1::enableReceiveCompleteInterrupt(Utils::interruptLevel_e receiveCompleteInterruptLevel)
{
	this->receiveCompleteInterruptLevel = receiveCompleteInterruptLevel;
	USARTC1_CTRLA = (receiveCompleteInterruptLevel << USART_RXCINTLVL_gp)|(USARTC1_CTRLA & ~USART_RXCINTLVL_gm);
}

void UsartC1::enableTransmitCompleteInterrupt(Utils::interruptLevel_e transmitCompleteInterrupt)
{
	this->transmitCompleteInterrupt = transmitCompleteInterrupt;
	USARTC1_CTRLA = (transmitCompleteInterrupt << USART_TXCINTLVL_gp)|(USARTC1_CTRLA & ~USART_TXCINTLVL_gm);
}

void UsartC1::enableDataRegisterEmptyInterrupt(Utils::interruptLevel_e dataRegisterEmptyInterrupt)
{
	this->dataRegisterEmptyInterrupt = dataRegisterEmptyInterrupt;
	USARTC1_CTRLA = (dataRegisterEmptyInterrupt << USART_DREINTLVL_gp)|(USARTC1_CTRLA & ~USART_DREINTLVL_gm);
}

void UsartC1::enableReceiver()
{
	receiver = true;
	USARTC1_CTRLB |= USART_RXEN_bm;
}

void UsartC1::enableTransmitter()
{
	transmitter = true;
	USARTC1_CTRLB |= USART_TXEN_bm;
}

void UsartC1::enableDoubleTransmissionSpeed()
{
	dts = true;
	USARTC1_CTRLB |= USART_CLK2X_bm;
}

void UsartC1::enableMultiprocessorCommunicationMode()
{
	mcm = true;
	USARTC1_CTRLB |= USART_MPCM_bm;
}

//=================//
//=== DISABLERS ===//
//=================//

void UsartC1::disableReceiver()
{
	receiver = false;
	USARTC1_CTRLB &= ~USART_RXEN_bm;
}

void UsartC1::disableTransmitter()
{
	transmitter = false;
	USARTC1_CTRLB &= ~USART_TXEN_bm;
}

void UsartC1::disableDoubleTransmissionSpeed()
{
	dts = false;
	USARTC1_CTRLB &= ~USART_CLK2X_bm;
}

void UsartC1::disableMultiprocessorCommunicationMode()
{
	mcm = false;
	USARTC1_CTRLB &= ~USART_MPCM_bm;
}

//===============//
//=== GETTERS ===//
//===============//

uint8_t UsartC1::getData()
{
	return USARTC1_DATA;
}

bool UsartC1::getReceiveCompleteInterruptFlag()
{
	return USARTC1_STATUS & USART_RXCIF_bm;
}

bool UsartC1::getTransmitCompleteInterruptFlag()
{
	return USARTC1_STATUS & USART_TXCIF_bm;
}

bool UsartC1::getDataRegisterEmptyFlag()
{
	return USARTC1_STATUS & USART_DREIF_bm;
}

bool UsartC1::getFrameError()
{
	return USARTC1_STATUS & USART_FERR_bm;
}

bool UsartC1::getBufferOverflow()
{
	return USARTC1_STATUS & USART_BUFOVF_bm;
}

bool UsartC1::getParityError()
{
	return USARTC1_STATUS & USART_PERR_bm;
}

bool UsartC1::getRXB8()
{
	return USARTC1_STATUS & USART_RXB8_bm;
}

bool UsartC1::getTXB8()
{
	return USARTC1_CTRLB & USART_TXB8_bm;
}

//===============//
//=== SETTERS ===//
//===============//

void UsartC1::setData(uint8_t data)
{
	USARTC1_DATA = data;
}

void UsartC1::setTXB8(bool b)
{
	if (b) USARTC1_CTRLB |= USART_TXB8_bm;
	else USARTC1_CTRLB &= ~USART_TXB8_bm;
}

void UsartC1::setCommunicationMode(communicationMode_e communicationMode)
{
	this->communicationMode = communicationMode;
	USARTC1_CTRLC = (communicationMode << USART_CMODE_gp)|(USARTC1_CTRLC & ~USART_CMODE_gm);
}

void UsartC1::setParityMode(parityMode_e parityMode)
{
	this->parityMode = parityMode;
	USARTC1_CTRLC = (parityMode << USART_PMODE_gp)|(USARTC1_CTRLC & ~USART_PMODE_gm);
}

void UsartC1::setStopMode(stopMode_e stopMode)
{
	this->stopMode = stopMode;
	switch (stopMode) {
		case SM_1BIT: USARTC1_CTRLC &= ~USART_SBMODE_bm; break;
		case SM_2BIT: USARTC1_CTRLC |= USART_SBMODE_bm; break;
	}
}

void UsartC1::setCharacterSize(characterSize_e characterSize)
{
	this->characterSize = characterSize;
	USARTC1_CTRLC = (characterSize << USART_CHSIZE_gp)|(USARTC1_CTRLC & ~USART_CHSIZE_gm);
}

void UsartC1::setBSEL(uint16_t bsel)
{
	this->bsel = bsel;
	USARTC1_BAUDCTRLA = bsel;
	USARTC1_BAUDCTRLB = (bsel >> 8)|(USARTC1_BAUDCTRLB & 0x0F);
}

void UsartC1::setBSCALE(uint8_t bscale)
{
	this->bscale = bscale;
	USARTC1_BAUDCTRLB = (bscale << USART_BSCALE_gp)|(USARTC1_BAUDCTRLB & ~USART_BSCALE_gm);
}
