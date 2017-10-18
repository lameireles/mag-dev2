/* 
* UsartE0.h
*/

#ifndef __USARTE0_H__
#define __USARTE0_H__

#include "Usart.h"
#include "Utils.h"

class UsartE0
{
//================//
//=== TYPEDEFS ===//
//================//
public:
	enum communicationMode_e {
		CM_ASYNCHRONOUS = 0,
		CM_SYNCHRONOUS = 1,
		CM_IRCOM = 2,
		CM_MSPI = 3
	};
	
	enum parityMode_e {
		PM_DISABLED = 0,
		PM_EVEN = 2,
		PM_ODD = 3
	};
	
	enum stopMode_e {
		SM_1BIT = 0,
		SM_2BIT = 1
	};
	
	enum characterSize_e {
		CS_5BIT = 0,
		CS_6BIT = 1,
		CS_7BIT = 2,
		CS_8BIT = 3,
		CS_9BIT = 7
	};
	
//====================//
//=== CONSTRUCTORS ===//
//====================//
public:
	UsartE0(Usart::baudrate_s, Utils::interruptLevel_e);

//========================//
//=== PUBLIC VARIABLES ===//
//========================//
public:	
	static volatile bool rxcFlag;
	static volatile char rxChar;
	bool isRXC(){return rxcFlag;};
	void setRXC(bool b){rxcFlag = b;};
	char getChar(){return rxChar;};
		
//=========================//
//=== PRIVATE VARIABLES ===//
//=========================//
private:
	Utils::interruptLevel_e receiveCompleteInterruptLevel, transmitCompleteInterrupt, dataRegisterEmptyInterrupt;
	bool receiver, transmitter, dts, mcm;
	communicationMode_e communicationMode;
	parityMode_e parityMode;
	stopMode_e stopMode;
	characterSize_e characterSize;
	uint16_t bsel;
	uint8_t bscale;

//=========================//
//=== PRIVATE FUNCTIONS ===//
//=========================//
private:
	void waitDataRegisterEmpty();

//========================//
//=== PUBLIC FUNCTIONS ===//
//========================//
public:
	void sendChar(char);
	void sendString(const char* str);

//================//
//=== ENABLERS ===//
//================//
public:
	void enableReceiveCompleteInterrupt(Utils::interruptLevel_e);
	void enableTransmitCompleteInterrupt(Utils::interruptLevel_e);
	void enableDataRegisterEmptyInterrupt(Utils::interruptLevel_e);
	void enableReceiver();
	void enableTransmitter();
	void enableDoubleTransmissionSpeed();
	void enableMultiprocessorCommunicationMode();

//=================//
//=== DISABLERS ===//
//=================//
public:
	void disableReceiver();
	void disableTransmitter();
	void disableDoubleTransmissionSpeed();
	void disableMultiprocessorCommunicationMode();
	
//===============//
//=== GETTERS ===//
//===============//
public:
	uint8_t getData();
	bool getReceiveCompleteInterruptFlag();
	bool getTransmitCompleteInterruptFlag();
	bool getDataRegisterEmptyFlag();
	bool getFrameError();
	bool getBufferOverflow();
	bool getParityError();
	bool getRXB8();
	Utils::interruptLevel_e getReceiveCompleteInterruptLevel(){return receiveCompleteInterruptLevel;};
	Utils::interruptLevel_e getTransmitCompleteInterruptLevel(){return transmitCompleteInterrupt;};
	Utils::interruptLevel_e getDataRegisterEmptyInterruptLevel(){return dataRegisterEmptyInterrupt;};
	bool getReceiver(){return receiver;};
	bool getTransmitter(){return transmitter;};
	bool getDoubleTransmissionSpeed(){return dts;};
	bool getMultiprocessorCommunicationMode(){return mcm;};
	bool getTXB8();
	communicationMode_e getCommunicationMode(){return communicationMode;};
	parityMode_e getParityMode(){return parityMode;};
	stopMode_e getStopMode(){return stopMode;};
	characterSize_e getCharacterSize(){return characterSize;};
	uint16_t getBSEL(){return bsel;};
	uint8_t getBSCALE(){return bscale;};

//===============//
//=== SETTERS ===//
//===============//
public:
	void setData(uint8_t);
	void setTXB8(bool);
	void setCommunicationMode(communicationMode_e);
	void setParityMode(parityMode_e);
	void setStopMode(stopMode_e);
	void setCharacterSize(characterSize_e);
	void setBSEL(uint16_t);
	void setBSCALE(uint8_t);
};

#endif //__USARTE0_H__
