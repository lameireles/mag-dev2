/* 
* Tcc0.cpp
*/

#include "Tcc0.h"
#include <avr/interrupt.h>
#include <avr/io.h>

//==================//
//=== INTERRUPTS ===//
//==================//

volatile double Tcc0::time_ms = 0;
volatile double Tcc0::timeIncrement_ms = 0;

ISR(TCC0_OVF_vect)
{
	Tcc0::time_ms += Tcc0::timeIncrement_ms;
}

//===================//
//=== CONSTRUCTOR ===//
//===================//

Tcc0::Tcc0(clockSource_e clockSource, double timeIncrement_ms, Utils::interruptLevel_e level)
{
	setClockSource(clockSource);
	this->timeIncrement_ms = timeIncrement_ms;
	enableTimerOverflowInterrupt(level);
}

//================//
//=== ENABLERS ===//
//================//

void Tcc0::enableCompareOrCapture(port_e port)
{	
	switch (port) {
		case PORT_A:
			cca = true; 
			TCC0_CTRLB |= TC0_CCAEN_bm; 
			break;
		case PORT_B: 
			ccb = true;
			TCC0_CTRLB |= TC0_CCBEN_bm;
			break;
		case PORT_C:
			ccc = true;
			TCC0_CTRLB |= TC0_CCCEN_bm;
			break;
		case PORT_D:
			ccd = true;
			TCC0_CTRLB |= TC0_CCDEN_bm;
			break;
	}
}

void Tcc0::enableCompareOutputValue(port_e port)
{
	switch (port) {
		case PORT_A:
			cmpa = true;
			TCC0_CTRLC |= TC0_CMPA_bm;
			break;
		case PORT_B:
			cmpb = true;
			TCC0_CTRLC |= TC0_CMPB_bm;
			break;
		case PORT_C:
			cmpc = true;
			TCC0_CTRLC |= TC0_CMPC_bm;
			break;
		case PORT_D:
			cmpd = true;
			TCC0_CTRLC |= TC0_CMPD_bm;
			break;
	}
}

void Tcc0::enableTimerDelayEvent()
{
	tde = true;
	TCC0_CTRLD |= TC0_EVDLY_bm;
}

void Tcc0::enableTimerErrorInterrupt(Utils::interruptLevel_e timerErrorLevel)
{
	this->timerErrorLevel = timerErrorLevel;
	TCC0_INTCTRLA = (timerErrorLevel << TC0_ERRINTLVL_gp)|(TCC0_INTCTRLA & ~TC0_ERRINTLVL_gm);
}

void Tcc0::enableTimerOverflowInterrupt(Utils::interruptLevel_e timerOverflowLevel)
{
	this->timerOverflowLevel = timerOverflowLevel;
	TCC0_INTCTRLA = (timerOverflowLevel << TC0_OVFINTLVL_gp)|(TCC0_INTCTRLA & ~TC0_OVFINTLVL_gm);
}

void Tcc0::enableCompareOrCaptureInterrupt(port_e port, Utils::interruptLevel_e level)
{
	switch (port) {
		case PORT_A:
			ccaLevel = level;
			TCC0_INTCTRLB= (level << TC0_CCAINTLVL_gp)|(TCC0_INTCTRLB & ~TC0_CCAINTLVL_gm);
			break;
		case PORT_B:
			ccbLevel = level;
			TCC0_INTCTRLB= (level << TC0_CCBINTLVL_gp)|(TCC0_INTCTRLB & ~TC0_CCBINTLVL_gm);
			break;
		case PORT_C:
			cccLevel = level;
			TCC0_INTCTRLB= (level << TC0_CCCINTLVL_gp)|(TCC0_INTCTRLB & ~TC0_CCCINTLVL_gm);
			break;
		case PORT_D:
			ccdLevel = level;
			TCC0_INTCTRLB= (level << TC0_CCDINTLVL_gp)|(TCC0_INTCTRLB & ~TC0_CCDINTLVL_gm);
			break;	
	}
}

void Tcc0::enableLockUpdate()
{
	lockUpdate = true;
	TCC0_CTRLFSET |= TC0_LUPD_bm;
}

void Tcc0::enableDownCounting()
{
	counterDirection = true;
	TCC0_CTRLFSET |= TC0_DIR_bm;
}

//=================//
//=== DISABLERS ===//
//=================//

void Tcc0::disableCompareOrCapture(port_e port)
{
	switch (port) {
		case PORT_A:
			cca = false; 
			TCC0_CTRLB &= ~TC0_CCAEN_bm;
			break;
		case PORT_B:
			ccb = false;
			TCC0_CTRLB &= ~TC0_CCBEN_bm;
			break;
		case PORT_C:
			ccc = false;
			TCC0_CTRLB &= ~TC0_CCCEN_bm;
			break;
		case PORT_D:
			ccd = false;
			TCC0_CTRLB &= ~TC0_CCDEN_bm;
			break;
	}
}

void Tcc0::disableCompareOutputValue(port_e port)
{
	switch (port) {
		case PORT_A:
			cmpa = false;
			TCC0_CTRLC &= ~TC0_CMPA_bm;
			break;
		case PORT_B:
			cmpb = false;
			TCC0_CTRLC &= ~TC0_CMPB_bm;
			break;
		case PORT_C:
			cmpc = false;
			TCC0_CTRLC &= ~TC0_CMPC_bm;
			break;
		case PORT_D:
			cmpd = false;
			TCC0_CTRLC &= ~TC0_CMPD_bm;
			break;
	}
}

void Tcc0::disableTimerDelayEvent()
{
	tde = false;
	TCC0_CTRLD &= ~TC0_EVDLY_bm;
}

void Tcc0::disableLockUpdate()
{
	lockUpdate = false;
	TCC0_CTRLFCLR |= TC0_LUPD_bm;
}

void Tcc0::disableDownCouting()
{
	counterDirection = false;
	TCC0_CTRLFCLR |= TC0_DIR_bm;
}

//===============//
//=== GETTERS ===//
//===============//

bool Tcc0::getCompareOrCaptureEnabled(port_e port)
{
	switch (port) {
		case PORT_A: return cca;
		case PORT_B: return ccb;
		case PORT_C: return ccc;
		case PORT_D: return ccd;
		default: return false;
	}
}

bool Tcc0::getCompareOutputValueEnabled(port_e port)
{
	switch (port) {
		case PORT_A: return cmpa;
		case PORT_B: return cmpb;
		case PORT_C: return cmpc;
		case PORT_D: return cmpd;
		default: return false;
	}
}

Utils::interruptLevel_e Tcc0::getCompareOrCaptureLevel(port_e port)
{
	switch (port) {
		case PORT_A: return ccaLevel;
		case PORT_B: return ccbLevel;
		case PORT_C: return cccLevel;
		case PORT_D: return ccdLevel;
		default: return Utils::IL_OFF;
	}
}

bool Tcc0::getCompareOrCaptureBufferValid(port_e port)
{
	switch (port) {
		case PORT_A: return TCC0_CTRLGSET & TC0_CCABV_bm;
		case PORT_B: return TCC0_CTRLGSET & TC0_CCBBV_bm;
		case PORT_C: return TCC0_CTRLGSET & TC0_CCCBV_bm;
		case PORT_D: return TCC0_CTRLGSET & TC0_CCDBV_bm;
		default: return false;
	}
}

bool Tcc0::getPeriodBufferValid()
{
	return TCC0_CTRLGSET & TC0_PERBV_bm;
}

bool Tcc0::getCompareOrCaptureInterruptFlag(port_e port)
{
	switch (port) {
		case PORT_A: return TCC0_INTFLAGS & TC0_CCAIF_bm;
		case PORT_B: return TCC0_INTFLAGS & TC0_CCBIF_bm;
		case PORT_C: return TCC0_INTFLAGS & TC0_CCCIF_bm;
		case PORT_D: return TCC0_INTFLAGS & TC0_CCDIF_bm;
		default: return false;
	}
}

bool Tcc0::getErrorInterruptFlag()
{
	return TCC0_INTFLAGS & TC0_ERRIF_bm;
}

bool Tcc0::getOverflowInterruptFlag()
{
	return TCC0_INTFLAGS & TC0_OVFIF_bm;
}

uint16_t Tcc0::getCounter()
{
	return TCC0_CNT;
}

uint16_t Tcc0::getPeriod()
{
	return TCC0_PER;
}

uint16_t Tcc0::getCompareOrCapture(port_e port)
{
	switch (port) {
		case PORT_A: return TCC0_CCA;
		case PORT_B: return TCC0_CCB;
		case PORT_C: return TCC0_CCC;
		case PORT_D: return TCC0_CCD;
		default: return 0;
	}
}

uint16_t Tcc0::getPeriodBuffer()
{
	return TCC0_PERBUF;
}

uint16_t Tcc0::getCompareOrCaptureBuffer(port_e port)
{
	switch (port) {
		case PORT_A: return TCC0_CCABUF;
		case PORT_B: return TCC0_CCBBUF;
		case PORT_C: return TCC0_CCCBUF;
		case PORT_D: return TCC0_CCDBUF;
		default: return 0;
	}
}


//===============//
//=== SETTERS ===//
//===============//

void Tcc0::setClockSource(clockSource_e clockSource)
{
	this->clockSource = clockSource;
	TCC0_CTRLA = clockSource;
};

void Tcc0::setWaveformGenerationMode(wgm_e wgm)
{
	this->wgm = wgm;
	TCC0_CTRLB = (wgm << TC0_WGMODE_gp)|(TCC0_CTRLB & ~TC0_WGMODE_gm);
}

void Tcc0::setEventAction(eventAction_e eventAction)
{
	this->eventAction = eventAction;
	TCC0_CTRLD = (eventAction << TC0_EVACT_gp)|(TCC0_CTRLD & ~TC0_EVACT_gm);
}

void Tcc0::setTimerEventSource(tes_e tes)
{
	this->tes = tes;
	TCC0_CTRLD = (tes << TC0_EVSEL_gp)|(TCC0_CTRLD & ~TC0_EVSEL_gm);
}

void Tcc0::setByteMode(byteMode_e byteMode)
{
	this->byteMode = byteMode;
	TCC0_CTRLE = (byteMode << TC0_BYTEM_gp)|(TCC0_CTRLE & ~TC0_BYTEM_gm);
}

void Tcc0::setCommand(command_e command)
{
	switch (command) {
		case COM_NONE: TCC0_CTRLFCLR |= TC0_CMD_gm; break;
		case COM_UPDATE: TCC0_CTRLFSET |= TC0_CMD0_bm; break;
		case COM_RESTART: TCC0_CTRLFSET |= TC0_CMD1_bm; break;
		case COM_RESET: TCC0_CTRLFSET |= TC0_CMD_gm; break;
	}
}

void Tcc0::setCounter(uint16_t counter)
{
	TCC0_CNT = counter;
}

void Tcc0::setPeriod(uint16_t period)
{
	TCC0_PER = period;
}

void Tcc0::setCompareOrCapture(port_e port, uint16_t coc)
{
	switch (port) {
		case PORT_A: TCC0_CCA = coc; break;
		case PORT_B: TCC0_CCB = coc; break;
		case PORT_C: TCC0_CCC = coc; break;
		case PORT_D: TCC0_CCD = coc; break;
	}
}

void Tcc0::setPeriodBuffer(uint16_t periodBuffer)
{
	TCC0_PERBUF = periodBuffer;
}

void Tcc0::setCompareOrCaptureBuffer(port_e port, uint16_t cocb)
{
	switch (port) {
		case PORT_A: TCC0_CCABUF = cocb; break;
		case PORT_B: TCC0_CCBBUF = cocb; break;
		case PORT_C: TCC0_CCCBUF = cocb; break;
		case PORT_D: TCC0_CCDBUF = cocb; break;
	}
}
