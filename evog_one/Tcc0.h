/* 
* Tcc0.h
*/

#ifndef __TCC0_H__
#define __TCC0_H__

#include "Utils.h"
#include <stdint.h>

class Tcc0
{
//================//
//=== TYPEDEFS ===//
//================//
public:
	enum clockSource_e {
		CS_OFF = 0,
		CS_DIV1 = 1,
		CS_DIV2 = 2,
		CS_DIV4 = 3,
		CS_DIV8 = 4,
		CS_DIV64 = 5,
		CS_DIV256 = 6,
		CS_DIV1024 = 7,
		CS_EVCH0 = 8,
		CS_EVCH1 = 9,
		CS_EVCH2 = 10,
		CS_EVCH3 = 11,
		CS_EVCH4 = 12,
		CS_EVCH5 = 13,
		CS_EVCH6 = 14,
		CS_EVCH7 = 15
	};
	
	enum port_e {
		PORT_A,
		PORT_B,
		PORT_C,
		PORT_D
	};
	
	enum wgm_e {
		WGM_NORMAL = 0,
		WGM_FRQ = 1,
		WGM_SINGLESLOPE = 3,
		WGM_DSTOP = 5,
		WGM_DSBOTH = 6,
		WGM_DSBOTTOM = 7
	};
	
	enum eventAction_e {
		EA_OFF = 0,
		EA_CAPT = 1,
		EA_UPDOWN = 2,
		EA_QDEC = 3,
		EA_RESTART = 4,
		EA_FRQ = 5,
		EA_PW = 6
	};
	
	enum tes_e {
		TES_OFF = 0,
		TES_CH0 = 8,
		TES_CH1 = 9,
		TES_CH2 = 10,
		TES_CH3 = 11,
		TES_CH4 = 12,
		TES_CH5 = 13,
		TES_CH6 = 14,
		TES_CH7 = 15
	};
	
	enum byteMode_e {
		BM_NORMAL = 0,
		BM_BYTEMODE = 1,
		BM_SPLITMODE = 2
	};
	
	enum command_e {
		COM_NONE = 0,
		COM_UPDATE = 1,
		COM_RESTART = 2,
		COM_RESET = 3
	};

//====================//
//=== CONSTRUCTORS ===//
//====================//
public:
	Tcc0(clockSource_e, double, Utils::interruptLevel_e);

//========================//
//=== PUBLIC VARIABLES ===//
//========================//
public:
	static volatile double time_ms;
	static volatile double timeIncrement_ms;
	double getTime_ms(){return time_ms;};
	double getTime_s(){return time_ms/1000.;};

//=========================//
//=== PRIVATE VARIABLES ===//
//=========================//
private:
	clockSource_e clockSource;
	bool cca, ccb, ccc, ccd;
	wgm_e wgm;
	bool cmpa, cmpb, cmpc, cmpd;
	eventAction_e eventAction;
	bool tde;
	tes_e tes;
	byteMode_e byteMode;
	Utils::interruptLevel_e timerErrorLevel, timerOverflowLevel;
	Utils::interruptLevel_e ccaLevel, ccbLevel, cccLevel, ccdLevel;
	bool lockUpdate, counterDirection;

//================//
//=== ENABLERS ===//
//================//
public:
	void enableCompareOrCapture(port_e);
	void enableCompareOutputValue(port_e);
	void enableTimerDelayEvent();
	void enableTimerErrorInterrupt(Utils::interruptLevel_e);
	void enableTimerOverflowInterrupt(Utils::interruptLevel_e);
	void enableCompareOrCaptureInterrupt(port_e, Utils::interruptLevel_e);
	void enableLockUpdate();
	void enableDownCounting();

//=================//
//=== DISABLERS ===//
//=================//
public:
	void disableCompareOrCapture(port_e);
	void disableCompareOutputValue(port_e);
	void disableTimerDelayEvent();
	void disableLockUpdate();
	void disableDownCouting();

//===============//
//=== GETTERS ===//
//===============//
public:
	clockSource_e getClockSource(){return clockSource;};
	bool getCompareOrCaptureEnabled(port_e);
	wgm_e getWaveformGenerationMode(){return wgm;};
	bool getCompareOutputValueEnabled(port_e);
	eventAction_e getEventAction(){return eventAction;};
	bool getTimerDelayEventEnabled(){return tde;};
	tes_e getTimerEventSource(){return tes;};
	byteMode_e getByteMode(){return byteMode;};
	Utils::interruptLevel_e getTimerErrorLevel(){return timerErrorLevel;};
	Utils::interruptLevel_e getTimerOverflowLevel(){return timerOverflowLevel;};
	Utils::interruptLevel_e getCompareOrCaptureLevel(port_e);
	bool getLockUpdateEnabled(){return lockUpdate;};
	bool getCounterDirectionEnabled(){return counterDirection;};
	bool getCompareOrCaptureBufferValid(port_e);
	bool getPeriodBufferValid();
	bool getCompareOrCaptureInterruptFlag(port_e);
	bool getErrorInterruptFlag();
	bool getOverflowInterruptFlag();
	uint16_t getCounter();
	uint16_t getPeriod();
	uint16_t getCompareOrCapture(port_e);
	uint16_t getPeriodBuffer();
	uint16_t getCompareOrCaptureBuffer(port_e);
	
//===============//
//=== SETTERS ===//
//===============//
public:
	void setClockSource(clockSource_e);
	void setWaveformGenerationMode(wgm_e);
	void setEventAction(eventAction_e);
	void setTimerEventSource(tes_e);
	void setByteMode(byteMode_e);
	void setCommand(command_e);
	void setCounter(uint16_t);
	void setPeriod(uint16_t);
	void setCompareOrCapture(port_e, uint16_t);
	void setPeriodBuffer(uint16_t);
	void setCompareOrCaptureBuffer(port_e, uint16_t);
};

#endif //__TCC0_H__
