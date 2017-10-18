/* 
* Ms5525dso.h
*/

#ifndef __MS5525DSO_H__
#define __MS5525DSO_H__

#include "Tcc0.h"
#include "Twie.h"
#include "UsartE0.h"
#include <stdint.h>

class Ms5525dso
{
private:
	struct osr_s {
		uint8_t cmd;
		uint16_t delay_us;
	};
	
	enum coef_e {
		C1_addr = 0xA2,
		C2_addr = 0xA4,
		C3_addr = 0xA6,
		C4_addr = 0xA8,
		C5_addr = 0xAA,
		C6_addr = 0xAC
	};
	
	osr_s osr;
	uint8_t address;
	Twie* myTwie;
	UsartE0* myUsart;
	uint16_t C1, C2, C3, C4, C5, C6;
	uint8_t Q1 = 14, Q2 = 16, Q3 = 7, Q4 = 5, Q5 = 7, Q6 = 21;
	uint8_t Qs[6] = {Q1, Q2, Q3, Q4, Q5, Q6};
	
	uint16_t readCoef(coef_e);
	
public:	
	struct data_s {
		double pressure;
		double temperature;
	};

	static const osr_s OSR256;
	static const osr_s OSR512;
	static const osr_s OSR1024;
	static const osr_s OSR2048;
	static const osr_s OSR4096;

	Ms5525dso(osr_s, uint8_t, Twie*, UsartE0*);
	void reset();
	void readPROM();	
	void convertPressure();
	void convertTemperature();
	uint32_t readADC();
	data_s read();
};

#endif //__MS5525DSO_H__
