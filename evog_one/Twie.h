/* 
* Twie.h
*/

#ifndef __TWIE_H__
#define __TWIE_H__

#include "UsartE0.h"
#include "Utils.h"
#include <stdint.h>

class Twie
{
//================//
//=== TYPEDEFS ===//
//================//
public:
	enum sdaHoldTime_e {
		SDAHT_OFF = 0,
		SDAHT_50NS = 1,
		SDAHT_100NS = 2,
		SDAHT_400NS = 3
	};
	
	enum ibt_e {
		IBT_DISABLED = 0,
		IBT_50US = 1,
		IBT_100US = 2,
		IBT_200US = 3
	};
	
	enum acknowledgeAction_e {
		AA_ACK = 0,
		AA_NACK = 1
	};
	
	enum command_e {
		COM_NOACT = 0,
		COM_START = 1,
		COM_BYTEREC = 2,
		COM_STOP = 3
	};
	
	enum error_e {
		ERR_RXACK,
		ERR_ARBLOST,
		ERR_BUSERR,
		ERR_UNKNOWN
	};
	
	enum busState_e {
		BS_UNKNOWN = 0,
		BS_IDLE = 1,
		BS_OWNER = 2,
		BS_BUSY = 3
	};
	
	enum direction_e {
		DIR_WRITE,
		DIR_READ
	};
	
//===================//
//=== CONSTRUCTOR ===//
//===================//
public:
	Twie(uint8_t, ibt_e, UsartE0*);
	
//========================//
//=== PUBLIC FUNCTIONS ===//
//========================//
public:
	bool isError();
	void errorHandler(const char *);

//=========================//
//=== PRIVATE FUNCTIONS ===//
//=========================//
private:
	void waitRead();
	void waitWrite();
	error_e getErrorStatus();

//=========================//
//=== PRIVATE VARIABLES ===//
//=========================//
private:
	sdaHoldTime_e sdaHoldTime;
	bool externalDriverInterface;
	Utils::interruptLevel_e masterInterruptLevel, readInterruptLevel, writeInterruptLevel;
	bool master;
	ibt_e inactiveBusTimeout;
	bool quickCommand, smartMode;
	acknowledgeAction_e acknowledgeAction;
	uint8_t baudRate;
	UsartE0* myUsart;

//================//
//=== ENABLERS ===//
//================//
public:
	void enableExternalDriverInterface();
	void enableMasterInterrupt(Utils::interruptLevel_e);
	void enableReadInterrupt(Utils::interruptLevel_e);
	void enableWriteInterrupt(Utils::interruptLevel_e);
	void enableMaster();
	void enableQuickCommand();
	void enableSmartMode();
	
//=================//
//=== DISABLERS ===//
//=================//
public:
	void disableExternalDriverInterface();
	void disableMaster();
	void disableQuickCommand();
	void disableSmartMode();

//===============//
//=== GETTERS ===//
//===============//
public:
	sdaHoldTime_e getSdaHoldTime(){return sdaHoldTime;};
	bool getExternalDriverInterfaceEnabled(){return externalDriverInterface;};
	Utils::interruptLevel_e getMasterInterruptLevel(){return masterInterruptLevel;};
	Utils::interruptLevel_e getReadInterruptLevel(){return readInterruptLevel;};
	Utils::interruptLevel_e getWriteInterruptLevel(){return writeInterruptLevel;};
	bool getMasterEnabled(){return master;};
	ibt_e getInactiveBusTimeout(){return inactiveBusTimeout;};
	bool getQuickCommandEnabled(){return quickCommand;};
	bool getSmartModeEnabled(){return smartMode;};
	acknowledgeAction_e getAcknowledgeAction(){return acknowledgeAction;};
	bool getReadInterruptFlag();
	bool getWriteInterruptFlag();
	bool getClockHold();
	bool getReceivedAcknowledge();
	bool getArbitrationLost();
	bool getBusError();
	busState_e getBusState();
	uint8_t getBaudRate(){return baudRate;};
	uint8_t getData(acknowledgeAction_e);
	
//===============//
//=== SETTERS ===//
//===============//
public:
	void setSdaHoldTime(sdaHoldTime_e);
	void setInactiveBusTimeout(ibt_e);
	void setAcknowledgeAction(acknowledgeAction_e);
	void setCommand(command_e,acknowledgeAction_e);
	void setBusIdle();
	void setBaudRate(uint8_t);
	void setAddress(uint8_t,direction_e);
	void setData(uint8_t);
};

#endif //__TWIE_H__
