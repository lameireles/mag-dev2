/* 
* Ms5525dso.cpp
*/

#include "Ms5525dso.h"
#include "Utils.h"
#include <math.h>
#include <stdio.h>

//=================//
//=== CONSTANTS ===//
//=================//

const Ms5525dso::osr_s Ms5525dso::OSR256 = {0,600};
const Ms5525dso::osr_s Ms5525dso::OSR512 = {2,1170};
const Ms5525dso::osr_s Ms5525dso::OSR1024 = {4,2280};
const Ms5525dso::osr_s Ms5525dso::OSR2048 = {6,4540};
const Ms5525dso::osr_s Ms5525dso::OSR4096 = {8,9040};
		
//===================//
//=== CONSTRUCTOR ===//
//===================//

Ms5525dso::Ms5525dso(osr_s osr, uint8_t address, Twie* myTwie, UsartE0* myUsart)
{
	this->osr = osr;
	this->address = address;
	this->myTwie = myTwie;
	this->myUsart = myUsart;
	
	reset();
	readPROM();
}

//=========================//
//=== PRIVATE FUNCTIONS ===//
//=========================//

uint16_t Ms5525dso::readCoef(coef_e c)
{
	const char err[] = "Error: could not read flow sensor PROM.";
	
	myTwie->setAddress(address, Twie::DIR_WRITE);
	if (myTwie->isError()) {
		myTwie->setCommand(Twie::COM_STOP, Twie::AA_NACK);
		myTwie->errorHandler(err);
		return 0;
	}
	myTwie->setData(c);
	myTwie->setCommand(Twie::COM_STOP, Twie::AA_NACK);
	if (myTwie->isError()) {
		myTwie->errorHandler(err);
		return 0;
	}
	
	myTwie->setAddress(address, Twie::DIR_READ);
	if (myTwie->isError()) {
		myTwie->setCommand(Twie::COM_STOP, Twie::AA_NACK);
		myTwie->errorHandler(err);
		return 0;
	}
	uint16_t temp = myTwie->getData(Twie::AA_ACK) << 8;
	myTwie->setCommand(Twie::COM_BYTEREC, Twie::AA_ACK);
	temp |= myTwie->getData(Twie::AA_NACK);
	
	int i = (c-0xA0)/2;
	snprintf(Utils::txBuf,TX_LEN,"\tC%d: %u (0x%04x)\tQ%u: %d\r\n",i,temp,temp,i,Qs[i-1]);
	myUsart->sendString(Utils::txBuf);
	
	return temp;
}

//========================//
//=== PUBLIC FUNCTIONS ===//
//========================//

void Ms5525dso::reset()
{
	const char err[] = "Error: could not reset flow sensor.";
	
	myUsart->sendString("\tResetting Flow Sensor... ");
	
	myTwie->setAddress(address, Twie::DIR_WRITE);
	if (myTwie->isError()) {
		myTwie->setCommand(Twie::COM_STOP, Twie::AA_NACK);
		myTwie->errorHandler(err);
		return;
	}
	myTwie->setData(0x1E);
	myTwie->setCommand(Twie::COM_STOP, Twie::AA_NACK);
	if (myTwie->isError()) {
		myTwie->errorHandler(err);
		return;
	}
	Utils::delay_ms(3);
	
	myUsart->sendString("DONE!\r\n");
}

void Ms5525dso::readPROM()
{
	myUsart->sendString("\tReading Flow Sensor PROM...\r\n");
	
	C1 = readCoef(C1_addr);
	C2 = readCoef(C2_addr);
	C3 = readCoef(C3_addr);
	C4 = readCoef(C4_addr);
	C5 = readCoef(C5_addr);
	C6 = readCoef(C6_addr);
	
	myUsart->sendString("\tDONE!\r\n");
}

void Ms5525dso::convertPressure()
{
	const char err[] = "Error: could not convert pressure in flow sensor.";
	
	myTwie->setAddress(address, Twie::DIR_WRITE);
	if (myTwie->isError()) {
		myTwie->setCommand(Twie::COM_STOP, Twie::AA_NACK);
		myTwie->errorHandler(err);
		return;
	}
	myTwie->setData(0x40 + osr.cmd);
	myTwie->setCommand(Twie::COM_STOP, Twie::AA_NACK);
	if (myTwie->isError()) {
		myTwie->errorHandler(err);
		return;
	}
	Utils::delay_us(osr.delay_us);
}

void Ms5525dso::convertTemperature()
{	
	const char err[] = "Error: could not convert temperature in flow sensor.";

	myTwie->setAddress(address, Twie::DIR_WRITE);
	if (myTwie->isError()) {
		myTwie->setCommand(Twie::COM_STOP, Twie::AA_NACK);
		myTwie->errorHandler(err);
		return;
	}
	myTwie->setData(0x50 + osr.cmd);
	myTwie->setCommand(Twie::COM_STOP, Twie::AA_NACK);
	if (myTwie->isError()) {
		myTwie->errorHandler(err);
		return;
	}
	Utils::delay_us(osr.delay_us);
}

uint32_t Ms5525dso::readADC()
{
	const char err[] = "Error: could not read ADC in flow sensor.";
	
	myTwie->setAddress(address, Twie::DIR_WRITE);
	if (myTwie->isError()) {
		myTwie->setCommand(Twie::COM_STOP, Twie::AA_NACK);
		myTwie->errorHandler(err);
		return 0;
	}
	myTwie->setData(0x00);
	myTwie->setCommand(Twie::COM_STOP, Twie::AA_NACK);
	if (myTwie->isError()) {
		myTwie->errorHandler(err);
		return 0;
	}
		
	myTwie->setAddress(address, Twie::DIR_READ);
	if (myTwie->isError()) {
		myTwie->setCommand(Twie::COM_STOP, Twie::AA_NACK);
		myTwie->errorHandler(err);
		return 0;
	}
	
	uint32_t b1 = myTwie->getData(Twie::AA_ACK);
	myTwie->setCommand(Twie::COM_BYTEREC, Twie::AA_ACK);
	uint32_t b2 = myTwie->getData(Twie::AA_ACK);
	myTwie->setCommand(Twie::COM_BYTEREC, Twie::AA_ACK);
	uint32_t b3 = myTwie->getData(Twie::AA_NACK);
	
	return (b1 << 16)|(b2 << 8)|b3;
}

Ms5525dso::data_s Ms5525dso::read()
{	
	convertPressure();
	uint32_t D1 = readADC();
	convertTemperature();
	uint32_t D2 = readADC();
			
	double dT = D2 - C5*pow(2,Q5);
	double TEMP = 2000 + dT*C6/pow(2,Q6);
		
	double OFF = C2*pow(2,Q2) + dT*C4/pow(2,Q4);
	double SENS = C1*pow(2,Q1) + dT*C3/pow(2,Q3);
	double P = (D1*SENS/pow(2,21) - OFF)/pow(2,15);
	
	data_s data;
	data.pressure = P/100.;
	data.temperature = TEMP/100.;
	return data;
}
