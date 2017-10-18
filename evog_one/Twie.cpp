/* 
* Twie.cpp
*/

#include "Twie.h"
#include <avr/interrupt.h>
#include <avr/io.h>

//===================//
//=== CONSTRUCTOR ===//
//===================//

Twie::Twie(uint8_t baudRate, ibt_e inactiveBusTimeout, UsartE0* myUsart)
{
	this->baudRate = baudRate;
	this->inactiveBusTimeout = inactiveBusTimeout;
	this->myUsart = myUsart;
	
	setBaudRate(baudRate);
	setInactiveBusTimeout(inactiveBusTimeout);
	enableSmartMode();
	enableMaster();
	setBusIdle();
}

//========================//
//=== PUBLIC FUNCTIONS ===//
//========================//

void Twie::waitRead()
{
	while (!(TWIE_MASTER_STATUS & TWI_MASTER_RIF_bm)) ;
}

void Twie::waitWrite()
{
	while (!(TWIE_MASTER_STATUS & TWI_MASTER_WIF_bm)) ;
}

bool Twie::isError()
{
	return TWIE_MASTER_STATUS & (TWI_MASTER_RXACK_bm|TWI_MASTER_ARBLOST_bm|TWI_MASTER_BUSERR_bm);
}

Twie::error_e Twie::getErrorStatus()
{
	if (TWIE_MASTER_STATUS & TWI_MASTER_RXACK_bm) return ERR_RXACK;
	if (TWIE_MASTER_STATUS & TWI_MASTER_ARBLOST_bm) return ERR_ARBLOST;
	if (TWIE_MASTER_STATUS & TWI_MASTER_BUSERR_bm) return ERR_BUSERR;
	return ERR_UNKNOWN;
}

void Twie::errorHandler(const char* str)
{
	myUsart->sendString(str);
	myUsart->sendString("Error status: ");
	switch (getErrorStatus()) {
		case Twie::ERR_RXACK: myUsart->sendString("RXACK\r\n"); break;
		case Twie::ERR_ARBLOST: myUsart->sendString("ARBLOST\r\n"); break;
		case Twie::ERR_BUSERR: myUsart->sendString("BUSERR\r\n"); break;
		case Twie::ERR_UNKNOWN: myUsart->sendString("UNKNOWN\r\n"); break;
	}
}

//================//
//=== ENABLERS ===//
//================//

void Twie::enableExternalDriverInterface()
{
	externalDriverInterface = true;
	TWIE_CTRL |= TWI_EDIEN_bm;
}

void Twie::enableMasterInterrupt(Utils::interruptLevel_e masterInterruptLevel)
{
	this->masterInterruptLevel = masterInterruptLevel;
	TWIE_MASTER_CTRLA = (masterInterruptLevel << TWI_MASTER_INTLVL_gp)|(TWIE_MASTER_CTRLA & ~TWI_MASTER_INTLVL_gm);
}

void Twie::enableReadInterrupt(Utils::interruptLevel_e readInterruptLevel)
{
	this->readInterruptLevel = readInterruptLevel;
	TWIE_MASTER_CTRLA = (readInterruptLevel << TWI_MASTER_RIEN_bp)|(TWIE_MASTER_CTRLA & ~TWI_MASTER_RIEN_bm);
}

void Twie::enableWriteInterrupt(Utils::interruptLevel_e writeInterruptLevel)
{
	this->writeInterruptLevel = writeInterruptLevel;
	TWIE_MASTER_CTRLA = (writeInterruptLevel << TWI_MASTER_WIEN_bp)|(TWIE_MASTER_CTRLA & ~TWI_MASTER_WIEN_bm);
}

void Twie::enableMaster()
{
	master = true;
	TWIE_MASTER_CTRLA |= TWI_MASTER_ENABLE_bm;
}

void Twie::enableQuickCommand()
{
	quickCommand = true;
	TWIE_MASTER_CTRLB |= TWI_MASTER_QCEN_bm;
}

void Twie::enableSmartMode()
{
	smartMode = true;
	TWIE_MASTER_CTRLB |= TWI_MASTER_SMEN_bm;
}

//=================//
//=== DISABLERS ===//
//=================//

void Twie::disableExternalDriverInterface()
{
	externalDriverInterface = false;
	TWIE_CTRL &= ~TWI_EDIEN_bm;
}

void Twie::disableMaster()
{
	master = false;
	TWIE_MASTER_CTRLA &= ~TWI_MASTER_ENABLE_bm;
}

void Twie::disableQuickCommand()
{
	quickCommand = false;
	TWIE_MASTER_CTRLB &= ~TWI_MASTER_QCEN_bm;
}

void Twie::disableSmartMode()
{
	smartMode = false;
	TWIE_MASTER_CTRLB &= ~TWI_MASTER_SMEN_bm;
}

//===============//
//=== GETTERS ===//
//===============//

bool Twie::getReadInterruptFlag()
{
	return TWIE_MASTER_STATUS & TWI_MASTER_RIF_bm;
}

bool Twie::getWriteInterruptFlag()
{
	return TWIE_MASTER_STATUS & TWI_MASTER_WIF_bm;
}

bool Twie::getClockHold()
{
	return TWIE_MASTER_STATUS & TWI_MASTER_CLKHOLD_bm;
}

bool Twie::getReceivedAcknowledge()
{
	return TWIE_MASTER_STATUS & TWI_MASTER_RXACK_bm;
}

bool Twie::getArbitrationLost()
{
	return TWIE_MASTER_STATUS & TWI_MASTER_ARBLOST_bm;
}

bool Twie::getBusError()
{
	return TWIE_MASTER_STATUS & TWI_MASTER_BUSERR_bm;
}

Twie::busState_e Twie::getBusState()
{
	return static_cast<busState_e>(TWIE_MASTER_STATUS & TWI_MASTER_BUSSTATE_gm);
}

uint8_t Twie::getData(acknowledgeAction_e acknowledgeAction)
{
	setAcknowledgeAction(acknowledgeAction);
	return TWIE_MASTER_DATA;
}

//===============//
//=== SETTERS ===//
//===============//

void Twie::setSdaHoldTime(sdaHoldTime_e sdaHoldTime)
{
	this->sdaHoldTime = sdaHoldTime;
	TWIE_CTRL = (sdaHoldTime << TWI_SDAHOLD_gp)|(TWIE_CTRL & ~TWI_SDAHOLD_gm);
}

void Twie::setInactiveBusTimeout(ibt_e inactiveBusTimeout)
{
	this->inactiveBusTimeout = inactiveBusTimeout;
	TWIE_MASTER_CTRLB = (inactiveBusTimeout << TWI_MASTER_TIMEOUT_gp)|(TWIE_MASTER_CTRLB & ~TWI_MASTER_TIMEOUT_gm);
}

void Twie::setAcknowledgeAction(acknowledgeAction_e acknowledgeAction)
{
	this->acknowledgeAction = acknowledgeAction;
	switch (acknowledgeAction) {
		case AA_ACK: TWIE_MASTER_CTRLC &= ~TWI_MASTER_ACKACT_bm; break;
		case AA_NACK:TWIE_MASTER_CTRLC |= TWI_MASTER_ACKACT_bm; break;
	}
}

void Twie::setCommand(command_e command, acknowledgeAction_e acknowledgeAction)
{
	setAcknowledgeAction(acknowledgeAction);
	TWIE_MASTER_CTRLC = (command << TWI_MASTER_CMD_gp)|(TWIE_MASTER_CTRLC & ~TWI_MASTER_CMD_gm);
	if (command == COM_BYTEREC) waitRead();
}

void Twie::setBusIdle()
{
	TWIE_MASTER_STATUS |= TWI_MASTER_BUSSTATE0_bm;
}

void Twie::setBaudRate(uint8_t baudRate)
{
	this->baudRate = baudRate;
	TWIE_MASTER_BAUD = baudRate;
}

void Twie::setAddress(uint8_t address, direction_e direction)
{
	if (direction == DIR_READ) {
		TWIE_MASTER_ADDR = (address << 1) | 1;
		waitRead();
	} else {
		TWIE_MASTER_ADDR = (address << 1);
		waitWrite();
	}
}

void Twie::setData(uint8_t data)
{
	TWIE_MASTER_DATA = data;
	waitWrite();
}
