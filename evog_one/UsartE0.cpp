/* 
* UsartE0.cpp
*/

#include <avr/interrupt.h>
#include <avr/io.h>
#include "UsartE0.h"

//==================//
//=== INTERRUPTS ===//
//==================//

volatile bool UsartE0::rxcFlag = false;
volatile char UsartE0::rxChar = '\0';

ISR(USARTE0_RXC_vect)
{
	UsartE0::rxcFlag = true;
	UsartE0::rxChar = USARTE0_DATA;
}

//====================//
//=== CONSTRUCTORS ===//
//====================//

UsartE0::UsartE0(Usart::baudrate_s baudrate,Utils::interruptLevel_e receiveCompleteInterruptLevel)
{
	setBSEL(baudrate.bsel);
	setBSCALE(baudrate.bscale);
	if (baudrate.clk2x) enableDoubleTransmissionSpeed();
	enableReceiver();
	enableTransmitter();
	enableReceiveCompleteInterrupt(receiveCompleteInterruptLevel);
	PORTE_DIRSET = PIN3_bm;
}

//=========================//
//=== PRIVATE FUNCTIONS ===//
//=========================//

void UsartE0::waitDataRegisterEmpty()
{
	while (!(USARTE0_STATUS & USART_DREIF_bm)) ;
}

//========================//
//=== PUBLIC FUNCTIONS ===//
//========================//

void UsartE0::sendChar(char c)
{
	waitDataRegisterEmpty();
	USARTE0_DATA = c;
}

void UsartE0::sendString(const char* str)
{
	int i = 0;
	while (str[i] != '\0') sendChar(str[i++]);
}

//================//
//=== ENABLERS ===//
//================//

void UsartE0::enableReceiveCompleteInterrupt(Utils::interruptLevel_e receiveCompleteInterruptLevel)
{
	this->receiveCompleteInterruptLevel = receiveCompleteInterruptLevel;
	USARTE0_CTRLA = (receiveCompleteInterruptLevel << USART_RXCINTLVL_gp)|(USARTE0_CTRLA & ~USART_RXCINTLVL_gm);
}

void UsartE0::enableTransmitCompleteInterrupt(Utils::interruptLevel_e transmitCompleteInterrupt)
{
	this->transmitCompleteInterrupt = transmitCompleteInterrupt;
	USARTE0_CTRLA = (transmitCompleteInterrupt << USART_TXCINTLVL_gp)|(USARTE0_CTRLA & ~USART_TXCINTLVL_gm);
}

void UsartE0::enableDataRegisterEmptyInterrupt(Utils::interruptLevel_e dataRegisterEmptyInterrupt)
{
	this->dataRegisterEmptyInterrupt = dataRegisterEmptyInterrupt;
	USARTE0_CTRLA = (dataRegisterEmptyInterrupt << USART_DREINTLVL_gp)|(USARTE0_CTRLA & ~USART_DREINTLVL_gm);
}

void UsartE0::enableReceiver()
{
	receiver = true;
	USARTE0_CTRLB |= USART_RXEN_bm;
}

void UsartE0::enableTransmitter()
{
	transmitter = true;
	USARTE0_CTRLB |= USART_TXEN_bm;
}

void UsartE0::enableDoubleTransmissionSpeed()
{
	dts = true;
	USARTE0_CTRLB |= USART_CLK2X_bm;
}

void UsartE0::enableMultiprocessorCommunicationMode()
{
	mcm = true;
	USARTE0_CTRLB |= USART_MPCM_bm;
}

//=================//
//=== DISABLERS ===//
//=================//

void UsartE0::disableReceiver()
{
	receiver = false;
	USARTE0_CTRLB &= ~USART_RXEN_bm;
}

void UsartE0::disableTransmitter()
{
	transmitter = false;
	USARTE0_CTRLB &= ~USART_TXEN_bm;
}

void UsartE0::disableDoubleTransmissionSpeed()
{
	dts = false;
	USARTE0_CTRLB &= ~USART_CLK2X_bm;
}

void UsartE0::disableMultiprocessorCommunicationMode()
{
	mcm = false;
	USARTE0_CTRLB &= ~USART_MPCM_bm;
}

//===============//
//=== GETTERS ===//
//===============//

uint8_t UsartE0::getData()
{
	return USARTE0_DATA;
}

bool UsartE0::getReceiveCompleteInterruptFlag()
{
	return USARTE0_STATUS & USART_RXCIF_bm;
}

bool UsartE0::getTransmitCompleteInterruptFlag()
{
	return USARTE0_STATUS & USART_TXCIF_bm;
}

bool UsartE0::getDataRegisterEmptyFlag()
{
	return USARTE0_STATUS & USART_DREIF_bm;
}

bool UsartE0::getFrameError()
{
	return USARTE0_STATUS & USART_FERR_bm;
}

bool UsartE0::getBufferOverflow()
{
	return USARTE0_STATUS & USART_BUFOVF_bm;
}

bool UsartE0::getParityError()
{
	return USARTE0_STATUS & USART_PERR_bm;
}

bool UsartE0::getRXB8()
{
	return USARTE0_STATUS & USART_RXB8_bm;
}

bool UsartE0::getTXB8()
{
	return USARTE0_CTRLB & USART_TXB8_bm;
}

//===============//
//=== SETTERS ===//
//===============//

void UsartE0::setData(uint8_t data)
{
	USARTE0_DATA = data;
}

void UsartE0::setTXB8(bool b)
{
	if (b) USARTE0_CTRLB |= USART_TXB8_bm;
	else USARTE0_CTRLB &= ~USART_TXB8_bm;
}

void UsartE0::setCommunicationMode(communicationMode_e communicationMode)
{
	this->communicationMode = communicationMode;
	USARTE0_CTRLC = (communicationMode << USART_CMODE_gp)|(USARTE0_CTRLC & ~USART_CMODE_gm);
}

void UsartE0::setParityMode(parityMode_e parityMode)
{
	this->parityMode = parityMode;
	USARTE0_CTRLC = (parityMode << USART_PMODE_gp)|(USARTE0_CTRLC & ~USART_PMODE_gm);
}

void UsartE0::setStopMode(stopMode_e stopMode)
{
	this->stopMode = stopMode;
	switch (stopMode) {
		case SM_1BIT: USARTE0_CTRLC &= ~USART_SBMODE_bm; break;
		case SM_2BIT: USARTE0_CTRLC |= USART_SBMODE_bm; break;
	}
}

void UsartE0::setCharacterSize(characterSize_e characterSize)
{
	this->characterSize = characterSize;
	USARTE0_CTRLC = (characterSize << USART_CHSIZE_gp)|(USARTE0_CTRLC & ~USART_CHSIZE_gm);
}

void UsartE0::setBSEL(uint16_t bsel)
{
	this->bsel = bsel;
	USARTE0_BAUDCTRLA = bsel;
	USARTE0_BAUDCTRLB = (bsel >> 8)|(USARTE0_BAUDCTRLB & 0x0F);
}

void UsartE0::setBSCALE(uint8_t bscale)
{
	this->bscale = bscale;
	USARTE0_BAUDCTRLB = (bscale << USART_BSCALE_gp)|(USARTE0_BAUDCTRLB & ~USART_BSCALE_gm);
}
