/* 
* Adca.cpp
*/

#include "Adca.h"
#include <avr/interrupt.h>
#include <avr/io.h>

//==================//
//=== INTERRUPTS ===//
//==================//

volatile bool Adca::ch0flag, Adca::ch1flag, Adca::ch2flag, Adca::ch3flag;
volatile uint16_t Adca::ch0, Adca::ch1, Adca::ch2, Adca::ch3;

ISR(ADCA_CH0_vect)
{
	Adca::ch0flag = true;
	Adca::ch0 = ADCA_CH0RES;
}

ISR(ADCA_CH1_vect)
{
	Adca::ch1flag = true;
	Adca::ch1 = ADCA_CH1RES;
}

ISR(ADCA_CH2_vect)
{
	Adca::ch2flag = true;
	Adca::ch2 = ADCA_CH2RES;
}

ISR(ADCA_CH3_vect)
{
	Adca::ch3flag = true;
	Adca::ch3 = ADCA_CH3RES;
}

//====================//
//=== CONSTRUCTORS ===//
//====================//

Adca::Adca(prescaler_e prescaler, Utils::interruptLevel_e interruptLevel)
{
	setPrescaler(prescaler);
	setConversionMode(CM_UNSIGNED);
	setChannelInputMode(CH_0, CIM_SINGLEENDED);
	setMuxSelectionPositiveInput(CH_0, MSPI_PIN0);
	enableInterrupt(CH_0, interruptLevel);
	enableADC();
}

//=========================//
//=== PRIVATE FUNCTIONS ===//
//=========================//

void Adca::startSingleConversion(channel_e channel)
{
	switch (channel) {
		case CH_0:
			ch0flag = false;
			ADCA_CTRLA |= ADC_CH0START_bm;
			break;
		case CH_1:
			ch1flag = false;
			ADCA_CTRLA |= ADC_CH1START_bm;
			break;
		case CH_2:
			ch2flag = false;
			ADCA_CTRLA |= ADC_CH2START_bm;
			break;
		case CH_3:
			ch3flag = false;
			ADCA_CTRLA |= ADC_CH3START_bm;
			break;
	}
}

void Adca::flush()
{
	ADCA_CTRLA |= ADC_FLUSH_bm;
}

void Adca::waitConversion(channel_e channel)
{
	switch (channel) {
		case CH_0: while (!ch0flag) ; break;
		case CH_1: while (!ch1flag) ; break;
		case CH_2: while (!ch2flag) ; break;
		case CH_3: while (!ch3flag) ; break;
	}
}

//========================//
//=== PUBLIC FUNCTIONS ===//
//========================//

uint16_t Adca::readChannel(channel_e channel)
{
	startSingleConversion(channel);
	waitConversion(channel);
	switch (channel) {
		case CH_0: return ADCA_CH0RES;
		case CH_1: return ADCA_CH1RES;
		case CH_2: return ADCA_CH2RES;
		case CH_3: return ADCA_CH3RES;
	}
	return 0;
}

//================//
//=== ENABLERS ===//
//================//

void Adca::enableADC()
{
	adc = true;
	ADCA_CTRLA |= ADC_ENABLE_bm;
}

void Adca::enableFreeRunningMode()
{
	frm = true;
	ADCA_CTRLB |= ADC_FREERUN_bm;
}

void Adca::enableBandgap()
{
	bandgap = true;
	ADCA_REFCTRL |= ADC_BANDGAP_bm;
}

void Adca::enableTemperatureReference()
{
	temperatureReference = true;
	ADCA_REFCTRL |= ADC_TEMPREF_bm;
}

void Adca::enableInterrupt(channel_e channel, Utils::interruptLevel_e il)
{
	switch (channel) {
		case CH_0:
			il0 = il;
			ADCA_CH0_INTCTRL = (il << ADC_CH_INTLVL_gp)|(ADCA_CH0_INTCTRL & ~ADC_CH_INTLVL_gm);
			break;
		case CH_1:
			il1 = il;
			ADCA_CH1_INTCTRL = (il << ADC_CH_INTLVL_gp)|(ADCA_CH1_INTCTRL & ~ADC_CH_INTLVL_gm);
			break;
		case CH_2:
			il2 = il;
			ADCA_CH2_INTCTRL = (il << ADC_CH_INTLVL_gp)|(ADCA_CH2_INTCTRL & ~ADC_CH_INTLVL_gm);
			break;
		case CH_3:
			il3 = il;
			ADCA_CH3_INTCTRL = (il << ADC_CH_INTLVL_gp)|(ADCA_CH3_INTCTRL & ~ADC_CH_INTLVL_gm);
			break;
	}
}

//=================//
//=== DISABLERS ===//
//=================//

void Adca::disableADC()
{
	adc = false;
	ADCA_CTRLA &= ~ADC_ENABLE_bm;
}

void Adca::disableFreeRunningMode()
{
	frm = false;
	ADCA_CTRLB &= ~ADC_FREERUN_bm;
}

void Adca::disableBandgap()
{
	bandgap = false;
	ADCA_REFCTRL &= ~ADC_BANDGAP_bm;
}

void Adca::disableTemperatureReference()
{
	temperatureReference = false;
	ADCA_REFCTRL &= ~ADC_TEMPREF_bm;
}

//===============//
//=== GETTERS ===//
//===============//

bool Adca::getInterruptFlag(channel_e channel)
{
	return ADCA_INTFLAGS & (1 << channel);
}

uint16_t Adca::getChannelResult(channel_e channel)
{
	switch (channel) {
		case CH_0: return ADCA_CH0RES;
		case CH_1: return ADCA_CH1RES;
		case CH_2: return ADCA_CH2RES;
		case CH_3: return ADCA_CH3RES;
	}
	return 0;
}

uint16_t Adca::getCompare()
{
	return ADCA_CMP;
}

Adca::gainFactor_e Adca::getGainFactor(channel_e channel)
{
	switch (channel) {
		case CH_0: return gf0;
		case CH_1: return gf1;
		case CH_2: return gf2;
		case CH_3: return gf3;
	}
	return G_X1;
}

Adca::cim_e Adca::getChannelInputMode(channel_e channel)
{
	switch (channel) {
		case CH_0: return cim0;
		case CH_1: return cim1;
		case CH_2: return cim2;
		case CH_3: return cim3;
	}
	return CIM_INTERNAL;
}

Adca::mspi_e Adca::getMuxSelectionPositiveInput(channel_e channel)
{
	switch (channel) {
		case CH_0: return mspi0;
		case CH_1: return mspi1;
		case CH_2: return mspi2;
		case CH_3: return mspi3;
	}
	return MSPI_PIN0;
}

Adca::msni_e Adca::getMuxSelectionNegativeInput(channel_e channel)
{
	switch (channel) {
		case CH_0: return msni0;
		case CH_1: return msni1;
		case CH_2: return msni2;
		case CH_3: return msni3;
	}
	return MSNI_PIN0;
}

Adca::interruptMode_e Adca::getInterruptMode(channel_e channel)
{
	switch (channel) {
		case CH_0: return im0;
		case CH_1: return im1;
		case CH_2: return im2;
		case CH_3: return im3;
	}
	return IM_COMPLETE;
}

//===============//
//=== SETTERS ===//
//===============//

void Adca::setDMArequestSelection(drs_e drs)
{
	this->drs = drs;
	ADCA_CTRLA = (drs << ADC_DMASEL_gp)|(ADCA_CTRLA & ~ADC_DMASEL_gm);
}

void Adca::setGainStageImpedanceMode(gsim_e gsim)
{
	this->gsim = gsim;
	switch (gsim) {
		case GSIM_HIGHIMP: ADCA_CTRLB &= ~ADC_IMPMODE_bm; break;
		case GSIM_LOWIMP: ADCA_CTRLB |= ADC_IMPMODE_bm; break;
	}
}

void Adca::setCurrentLimitation(currentLimitation_e currentLimitation)
{
	this->currentLimitation = currentLimitation;
	ADCA_CTRLB = (currentLimitation << ADC_CURRLIMIT_gp)|(ADCA_CTRLB & ~ADC_CURRLIMIT_gm);
}

void Adca::setConversionMode(conversionMode_e conversionMode)
{
	this->conversionMode = conversionMode;
	switch(conversionMode) {
		case CM_UNSIGNED: ADCA_CTRLB &= ~ADC_CONMODE_bm; break;
		case CM_SIGNED: ADCA_CTRLB |= ADC_CONMODE_bm; break;
	}
}

void Adca::setConversionResultResolution(crr_e crr)
{
	this->crr = crr;
	ADCA_CTRLB = (crr << ADC_RESOLUTION_gp)|(ADCA_CTRLB & ~ADC_RESOLUTION_gm);
}

void Adca::setReferenceSelection(referenceSelection_e referenceSelection)
{
	this->referenceSelection = referenceSelection;
	ADCA_REFCTRL = (referenceSelection << ADC_REFSEL_gp)|(ADCA_REFCTRL & ~ADC_REFSEL_gm);
}

void Adca::setChannelSweep(channelSweep_e channelSweep)
{
	this->channelSweep = channelSweep;
	ADCA_EVCTRL = (channelSweep << ADC_SWEEP_gp)|(ADCA_EVCTRL & ~ADC_SWEEP_gm);
}

void Adca::setEventChannelInput(eci_e eci)
{
	this->eci = eci;
	ADCA_EVCTRL = (eci << ADC_EVSEL_gp)|(ADCA_EVCTRL & ~ADC_EVSEL_gm);
}

void Adca::setEventMode(eventMode_e eventMode)
{
	this->eventMode = eventMode;
	ADCA_EVCTRL = (eventMode << ADC_EVACT_gp)|(ADCA_EVCTRL & ~ADC_EVACT_gm);
}

void Adca::setPrescaler(prescaler_e prescaler)
{
	this->prescaler = prescaler;
	ADCA_PRESCALER = prescaler;
}

void Adca::setCompare(uint16_t compare)
{
	ADCA_CMP = compare;
}

void Adca::setGainFactor(channel_e channel, gainFactor_e gainFactor)
{
	switch (channel) {
		case CH_0:
			gf0 = gainFactor;
			ADCA_CH0_CTRL = (gainFactor << ADC_CH_GAIN_gp)|(ADCA_CH0_CTRL & ~ADC_CH_GAIN_gm);
			break;
		case CH_1:
			gf1 = gainFactor;
			ADCA_CH1_CTRL = (gainFactor << ADC_CH_GAIN_gp)|(ADCA_CH1_CTRL & ~ADC_CH_GAIN_gm);
			break;
		case CH_2:
			gf2 = gainFactor;
			ADCA_CH2_CTRL = (gainFactor << ADC_CH_GAIN_gp)|(ADCA_CH2_CTRL & ~ADC_CH_GAIN_gm);
			break;
		case CH_3:
			gf3 = gainFactor;
			ADCA_CH3_CTRL = (gainFactor << ADC_CH_GAIN_gp)|(ADCA_CH3_CTRL & ~ADC_CH_GAIN_gm);
			break;
	}
}

void Adca::setChannelInputMode(channel_e channel, cim_e cim)
{
	switch (channel) {
		case CH_0:
			cim0 = cim;
			ADCA_CH0_CTRL = (cim << ADC_CH_INPUTMODE_gp)|(ADCA_CH0_CTRL & ~ADC_CH_INPUTMODE_gm);
			break;
		case CH_1:
			cim1 = cim;
			ADCA_CH1_CTRL = (cim << ADC_CH_INPUTMODE_gp)|(ADCA_CH1_CTRL & ~ADC_CH_INPUTMODE_gm);
			break;
		case CH_2:
			cim2 = cim;
			ADCA_CH2_CTRL = (cim << ADC_CH_INPUTMODE_gp)|(ADCA_CH2_CTRL & ~ADC_CH_INPUTMODE_gm);
			break;
		case CH_3:
			cim3 = cim;
			ADCA_CH3_CTRL = (cim << ADC_CH_INPUTMODE_gp)|(ADCA_CH3_CTRL & ~ADC_CH_INPUTMODE_gm);
			break;
	}
}

void Adca::setMuxSelectionPositiveInput(channel_e channel, mspi_e mspi)
{
	switch (channel) {
		case CH_0:
			mspi0 = mspi;
			ADCA_CH0_MUXCTRL = ((mspi&0x0f) << ADC_CH_MUXPOS_gp)|(ADCA_CH0_MUXCTRL & ~ADC_CH_MUXPOS_gm);
			break;
		case CH_1:
			mspi1 = mspi;
			ADCA_CH1_MUXCTRL = ((mspi&0x0f) << ADC_CH_MUXPOS_gp)|(ADCA_CH1_MUXCTRL & ~ADC_CH_MUXPOS_gm);
			break;
		case CH_2:
			mspi2 = mspi;
			ADCA_CH2_MUXCTRL = ((mspi&0x0f) << ADC_CH_MUXPOS_gp)|(ADCA_CH2_MUXCTRL & ~ADC_CH_MUXPOS_gm);
			break;
		case CH_3:
			mspi3 = mspi;
			ADCA_CH3_MUXCTRL = ((mspi&0x0f) << ADC_CH_MUXPOS_gp)|(ADCA_CH3_MUXCTRL & ~ADC_CH_MUXPOS_gm);
			break;
	}
}

void Adca::setMuxSelectionNegativeInput(channel_e channel, msni_e msni)
{
	switch (channel) {
		case CH_0:
			msni0 = msni;
			ADCA_CH0_MUXCTRL = ((msni&0x0f) << ADC_CH_MUXNEG_gp)|(ADCA_CH0_MUXCTRL & ~ADC_CH_MUXNEG_gm);
			break;
		case CH_1:
			msni1 = msni;
			ADCA_CH1_MUXCTRL = ((msni&0x0f) << ADC_CH_MUXNEG_gp)|(ADCA_CH1_MUXCTRL & ~ADC_CH_MUXNEG_gm);
			break;
		case CH_2:
			msni2 = msni;
			ADCA_CH2_MUXCTRL = ((msni&0x0f) << ADC_CH_MUXNEG_gp)|(ADCA_CH2_MUXCTRL & ~ADC_CH_MUXNEG_gm);
			break;
		case CH_3:
			msni3 = msni;
			ADCA_CH3_MUXCTRL = ((msni&0x0f) << ADC_CH_MUXNEG_gp)|(ADCA_CH3_MUXCTRL & ~ADC_CH_MUXNEG_gm);
			break;
	}
}

void Adca::setInterruptMode(channel_e channel, interruptMode_e im)
{
	switch (channel) {
		case CH_0:
			im0 = im;
			ADCA_CH0_INTCTRL = (im << ADC_CH_INTMODE_gp)|(ADCA_CH0_INTCTRL & ~ADC_CH_INTMODE_gm);
			break;
		case CH_1:
			im1 = im;
			ADCA_CH1_INTCTRL = (im << ADC_CH_INTMODE_gp)|(ADCA_CH1_INTCTRL & ~ADC_CH_INTMODE_gm);
			break;
		case CH_2:
			im2 = im;
			ADCA_CH2_INTCTRL = (im << ADC_CH_INTMODE_gp)|(ADCA_CH2_INTCTRL & ~ADC_CH_INTMODE_gm);
			break;
		case CH_3:
			im3 = im;
			ADCA_CH3_INTCTRL = (im << ADC_CH_INTMODE_gp)|(ADCA_CH3_INTCTRL & ~ADC_CH_INTMODE_gm);
			break;
	}
}