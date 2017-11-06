/* 
* Adca.h
*/

#ifndef __ADCA_H__
#define __ADCA_H__


#include "Utils.h"

class Adca
{	
//================//
//=== TYPEDEFS ===//
//================//
public:
	enum drs_e {
		DRS_OFF = 0,
		DRS_CH01 = 1,
		DRS_CH012 = 2,
		DRS_CH0123 = 3
	};

	enum channel_e {
		CH_0 = 0,
		CH_1 = 1,
		CH_2 = 2,
		CH_3 = 3
	};
	
	enum gsim_e {
		GSIM_HIGHIMP,
		GSIM_LOWIMP
	};
	
	enum currentLimitation_e {
		CL_NO = 0,
		CL_LOW = 1,
		CL_MED = 2,
		CL_HIGH = 3
	};
	
	enum conversionMode_e {
		CM_UNSIGNED,
		CM_SIGNED
	};
	
	enum crr_e {
		CRR_12BIT = 0,
		CRR_8BIT = 2,
		CRR_LEFT12BIT = 3
	};
	
	enum referenceSelection_e {
		RS_INT1V = 0,
		RS_INTVCC = 1,
		RS_AREFA = 2,
		RS_AREFB = 3,
		RS_INTVCC2 = 4
	};
	
	enum channelSweep_e {
		CS_0 = 0,
		CS_01 = 1,
		CS_012 = 2,
		CS_0123 = 3
	};
	
	enum eci_e {
		ECI_0123 = 0,
		ECI_1234 = 1,
		ECI_2345 = 2,
		ECI_3456 = 3,
		ECI_4567 = 4,
		ECI_567 = 5,
		ECI_67 = 6,
		ECI_7 = 7
	};
	
	enum eventMode_e {
		EM_NONE,
		EM_CH0,
		EM_CH01,
		EM_CH012,
		EM_CH0123,
		EM_SWEEP,
		EM_SYNCSWEEP
	};
	
	enum prescaler_e {
		P_DIV4,
		P_DIV8,
		P_DIV16,
		P_DIV32,
		P_DIV64,
		P_DIV128,
		P_DIV256,
		P_DIV512
	};
	
	enum gainFactor_e {
		G_X1 = 0,
		G_X2 = 1,
		G_X4 = 2,
		G_X8 = 3,
		G_X16 = 4,
		G_X32 = 5,
		G_X64 = 6,
		G_XHALF = 7
	};
	
	enum cim_e {
		CIM_INTERNAL = 0,
		CIM_SINGLEENDED = 1,
		CIM_DIFF = 2,
		CIM_DIFFWGAIN = 3
	};
	
	enum mspi_e {
		MSPI_PIN0 = 0,
		MSPI_PIN1 = 1,
		MSPI_PIN2 = 2,
		MSPI_PIN3 = 3,
		MSPI_PIN4 = 4,
		MSPI_PIN5 = 5,
		MSPI_PIN6 = 6,
		MSPI_PIN7 = 7,
		MSPI_PIN8 = 8,
		MSPI_PIN9 = 9,
		MSPI_PIN10 = 10,
		MSPI_PIN11 = 11,
		MSPI_PIN12 = 12,
		MSPI_PIN13 = 13,
		MSPI_PIN14 = 14,
		MSPI_PIN15 = 15,
		MSPI_TEMP = 16,
		MSPI_BANDGAP = 17,
		MSPI_SCALEDVCC = 18,
		MSPI_DAC = 19
	};
	
	enum msni_e {
		MSNI_PIN0 = 0,
		MSNI_PIN1 = 1,
		MSNI_PIN2 = 2,
		MSNI_PIN3 = 3,
		MSNI_GND = 5,
		MSNI_INTGND = 7,
		MSNI_PIN4 = 8,
		MSNI_PIN5 = 9,
		MSNI_PIN6 = 10,
		MSNI_PIN7 = 11,
		MSNI_INTGND2 = 12,
		MSNI_GND2 = 15
	};
	
	enum interruptMode_e {
		IM_COMPLETE = 0,
		IM_BELOW = 1,
		IM_ABOVE = 3
	};
	
//====================//
//=== CONSTRUCTORS ===//
//====================//
public:
	Adca(prescaler_e, Utils::interruptLevel_e);
	
//=========================//
//=== PRIVATE VARIABLES ===//
//=========================//
private:
	drs_e drs;
	bool adc;
	gsim_e gsim;
	currentLimitation_e currentLimitation;
	conversionMode_e conversionMode;
	bool frm;
	crr_e crr;
	referenceSelection_e referenceSelection;
	bool bandgap, temperatureReference;
	channelSweep_e channelSweep;
	eci_e eci;
	eventMode_e eventMode;
	prescaler_e prescaler;
	gainFactor_e gf0, gf1, gf2, gf3;
	cim_e cim0, cim1, cim2, cim3;
	mspi_e mspi0, mspi1, mspi2, mspi3;
	msni_e msni0, msni1, msni2, msni3;
	interruptMode_e im0, im1, im2, im3;
	Utils::interruptLevel_e il0, il1, il2, il3;
	
//========================//
//=== PUBLIC VARIABLES ===//
//========================//
public:
	static volatile bool ch0flag, ch1flag, ch2flag, ch3flag;
	static volatile uint16_t ch0, ch1, ch2, ch3;
	
//=========================//
//=== PRIVATE FUNCTIONS ===//
//=========================//
private:
	void startSingleConversion(channel_e);
	void flush();
	void waitConversion(channel_e);

//========================//
//=== PUBLIC FUNCTIONS ===//
//========================//
public:
	uint16_t readChannel(channel_e);

//================//
//=== ENABLERS ===//
//================//
public:
	void enableADC();
	void enableFreeRunningMode();
	void enableBandgap();
	void enableTemperatureReference();
	void enableInterrupt(channel_e,Utils::interruptLevel_e);
	
//=================//
//=== DISABLERS ===//
//=================//
public:
	void disableADC();
	void disableFreeRunningMode();
	void disableBandgap();
	void disableTemperatureReference();

//===============//
//=== GETTERS ===//
//===============//
public:
	drs_e getDMArequestSelection(){return drs;};
	bool getADCenabled(){return adc;};
	gsim_e getGainStageImpedanceMode(){return gsim;};
	currentLimitation_e getCurrentLimitation(){return currentLimitation;};
	conversionMode_e getConversionMode(){return conversionMode;};
	bool getFreeRunningMode(){return frm;};
	crr_e getConversionResultResolution(){return crr;};
	referenceSelection_e getReferenceSelection(){return referenceSelection;};
	bool getBandgap(){return bandgap;};
	bool getTemperatureReference(){return temperatureReference;};
	channelSweep_e getChannelSweep(){return channelSweep;};
	eci_e getEventChannelInput(){return eci;};
	eventMode_e getEventMode(){return eventMode;};
	prescaler_e getPrescaler(){return prescaler;};
	bool getInterruptFlag(channel_e);
	uint16_t getChannelResult(channel_e);
	uint16_t getCompare();
	gainFactor_e getGainFactor(channel_e);
	cim_e getChannelInputMode(channel_e);
	mspi_e getMuxSelectionPositiveInput(channel_e);
	msni_e getMuxSelectionNegativeInput(channel_e);
	interruptMode_e getInterruptMode(channel_e);
		
//===============//
//=== SETTERS ===//
//===============//
public:
	void setDMArequestSelection(drs_e);
	void setGainStageImpedanceMode(gsim_e);
	void setCurrentLimitation(currentLimitation_e);
	void setConversionMode(conversionMode_e);
	void setConversionResultResolution(crr_e);
	void setReferenceSelection(referenceSelection_e);
	void setChannelSweep(channelSweep_e);
	void setEventChannelInput(eci_e);
	void setEventMode(eventMode_e);
	void setPrescaler(prescaler_e);
	void setCompare(uint16_t);
	void setGainFactor(channel_e, gainFactor_e);
	void setChannelInputMode(channel_e, cim_e);
	void setMuxSelectionPositiveInput(channel_e, mspi_e);
	void setMuxSelectionNegativeInput(channel_e, msni_e);
	void setInterruptMode(channel_e,interruptMode_e);
};

#endif //__ADCA_H__

