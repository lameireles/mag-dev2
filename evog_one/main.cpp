/*
 * evog_one.cpp
 */

#include "Adca.h"
#include "Ms5525dso.h"
#include "Pump.h"
#include "Tcc0.h"
#include "Twie.h"
#include "UsartE0.h"
#include "Utils.h"
#include "Valve.h"
#include <avr/io.h>
#include <stdio.h>

//=========================//
//=== PRIVATE FUNCTIONS ===//
//=========================//

double oversampleO2(uint16_t I, Adca & myAdc)
{
	uint32_t totalO2 = 0;
	for (uint16_t i = 0; i < I; i++) totalO2 += myAdc.readChannel(Adca::CH_0);
	return ((double) totalO2)/I;
}

double o2_counts2pct(double counts, Utils::gascal_s gascal)
{
	return counts*gascal.gain + gascal.offset;
}

void flow_acq(Ms5525dso & myFlowSensor, Adca & myAdc, Tcc0 & myTimer, UsartE0 & myUsart)
{
	myUsart.sendString("sep=,\r\nTime [ms],Pressure [psi],Temperature [oC],O2 [counts]\r\n");
	while (!(myUsart.isRXC()))
	{
		Ms5525dso::data_s flowData = myFlowSensor.read();
		uint16_t o2_counts = myAdc.readChannel(Adca::CH_0);
		snprintf(Utils::txBuf, TX_LEN, "%.3f,%.4f,%.2f,%d\r\n", myTimer.getTime_ms(), flowData.pressure, flowData.temperature, o2_counts);
		myUsart.sendString(Utils::txBuf);
	}
	myUsart.setRXC(false);
}

void start_test(
	Utils::gascal_s gascal,
	Adca & myAdc,
	Ms5525dso & myFlowSensor,
	Pump & myPump,
	Tcc0 & myTimer,
	UsartE0 & myUsart,
	Valve & sampleValve,
	Valve & outputValve)
{
	// USER DEFINED CONSTANTS
	const uint16_t OUTPUT_OPEN_DURATION_ms = 2500;
	const uint16_t OUTPUT_CLOSE_DURATION_ms = 2500;
	const uint16_t SAMPLE_PERIOD_ms = 5000;
	const uint16_t OVERSAMPLE_COEFF = 1024;
	const uint8_t BREATH_CYCLE_UP_THRESH = 8;
	const uint8_t BREATH_CYCLE_DOWN_THRESH = 8;
		
	myUsart.sendString("sep=,\r\nTime [ms],O2 [%]\r\n");
	uint8_t upCount = 0, downCount = 0;
	double outputTimeFlag = myTimer.getTime_ms();
	double sampleTimeFlag = myTimer.getTime_ms();
	Ms5525dso::data_s last_flowData = myFlowSensor.read();
	myPump.turnOn();
	while (!(myUsart.isRXC()))
	{
		double timeNow = myTimer.getTime_ms();
						
		// Handle output valve
		if (outputValve.isOpen() && timeNow - outputTimeFlag > OUTPUT_OPEN_DURATION_ms) {
			outputTimeFlag = timeNow;
			outputValve.close();
		} else if (!outputValve.isOpen() && timeNow - outputTimeFlag > OUTPUT_CLOSE_DURATION_ms) {
			outputTimeFlag = timeNow;
			outputValve.open();
		}
		
		// Handle o2 sample
		if (timeNow - sampleTimeFlag > SAMPLE_PERIOD_ms) {
			sampleTimeFlag = timeNow;
			double o2_pct = o2_counts2pct(oversampleO2(OVERSAMPLE_COEFF, myAdc),gascal);
			snprintf(Utils::txBuf, TX_LEN, "%.3f,%.3f\r\n", timeNow, o2_pct);
			myUsart.sendString(Utils::txBuf);
		}
		
		// Handle sample valve
		Ms5525dso::data_s flowData = myFlowSensor.read();
		if (flowData.pressure > last_flowData.pressure) {
			downCount = 0;
			if (!(sampleValve.isOpen()) && ++upCount > BREATH_CYCLE_UP_THRESH) sampleValve.open();
		} else {
			upCount = 0;
			if (sampleValve.isOpen() && ++downCount > BREATH_CYCLE_DOWN_THRESH) sampleValve.close();
		}
		last_flowData = flowData;
	}
	myPump.turnOff();
	sampleValve.close();
	outputValve.close();
}

Utils::gascal_s start_gascal(
	Adca & myAdc,
	Pump & myPump,
	Tcc0 & myTimer,
	UsartE0 & myUsart,
	Valve & sampleValve,
	Valve & gascalValve,
	Valve & outputValve)
{
	// USER DEFINED CONSTANTS
	const uint8_t NUMBER_OF_REPETITIONS = 8;
	const uint16_t OUTPUT_OPEN_DURATION_ms = 2500;
	const uint16_t OUTPUT_CLOSE_DURATION_ms = 2500;
	const uint16_t SAMPLE_OPEN_DURATION_ms = 15000;
	const uint16_t GASCAL_OPEN_DURATION_ms = 15000;
	const uint16_t OVERSAMPLE_COEFF = 1024;
	const double GASCAL_PCT = 17;
	const double SAMPLE_PCT = 20.97;

	double avgSample = 0, avgGascal = 0;
	double outputTimeFlag = myTimer.getTime_ms();
	double targetTimeFlag = myTimer.getTime_ms();
	myPump.turnOn();
	sampleValve.open();
	gascalValve.close();
	outputValve.close();
	uint8_t i = 0;
	while (i < NUMBER_OF_REPETITIONS) {
		double timeNow = myTimer.getTime_ms();

		// Handle output valve
		if (outputValve.isOpen() && timeNow - outputTimeFlag > OUTPUT_OPEN_DURATION_ms) {
			outputTimeFlag = timeNow;
			outputValve.close();
		} else if (!(outputValve.isOpen()) && timeNow - outputTimeFlag > OUTPUT_CLOSE_DURATION_ms) {
			outputTimeFlag = timeNow;
			outputValve.open();
		}
		
		// Handle sample and gascal valve
		if (sampleValve.isOpen() && timeNow - targetTimeFlag > SAMPLE_OPEN_DURATION_ms) {
			targetTimeFlag = timeNow;
			avgSample += oversampleO2(OVERSAMPLE_COEFF, myAdc)/NUMBER_OF_REPETITIONS;
			sampleValve.close();
			gascalValve.open();
		} else if (gascalValve.isOpen() && timeNow - targetTimeFlag > GASCAL_OPEN_DURATION_ms) {
			targetTimeFlag = timeNow;
			avgGascal += oversampleO2(OVERSAMPLE_COEFF, myAdc)/NUMBER_OF_REPETITIONS;
			i++;
			gascalValve.close();
			sampleValve.open();
		}
	}
	myPump.turnOff();
	sampleValve.close();
	gascalValve.close();
	outputValve.close();

	// Output gascal coefficients
	Utils::gascal_s result;
	result.gain = (SAMPLE_PCT - GASCAL_PCT)/(avgSample-avgGascal);
	result.offset = SAMPLE_PCT - result.gain*avgSample;
	snprintf(Utils::txBuf, TX_LEN, "Gain: %.3f Offset: %.3f\r\n", result.gain, result.offset);
	myUsart.sendString(Utils::txBuf);
	return result;
}

Utils::gascal_s start_aircal(
	double gascal_gain,
	Adca & myAdc,
	Pump & myPump,
	Tcc0 & myTimer,
	UsartE0 & myUsart,
	Valve & sampleValve,
	Valve & gascalValve,
	Valve & outputValve)
{
	// USER DEFINED CONSTANTS
	const uint8_t NUMBER_OF_REPETITIONS = 8;
	const uint16_t OUTPUT_OPEN_DURATION_ms = 2500;
	const uint16_t OUTPUT_CLOSE_DURATION_ms = 2500;
	const uint16_t SAMPLE_PERIOD_ms = 15000;
	const uint16_t OVERSAMPLE_COEFF = 1024;
	const double SAMPLE_PCT = 20.97;

	double avgSample = 0;
	double outputTimeFlag = myTimer.getTime_ms();
	double targetTimeFlag = myTimer.getTime_ms();
	myPump.turnOn();
	sampleValve.open();
	outputValve.close();
	uint8_t i = 0;
	while (i < NUMBER_OF_REPETITIONS) {
		double timeNow = myTimer.getTime_ms();

		// Handle output valve
		if (outputValve.isOpen() && timeNow - outputTimeFlag > OUTPUT_OPEN_DURATION_ms) {
			outputTimeFlag = timeNow;
			outputValve.close();
		} else if (!(outputValve.isOpen()) && timeNow - outputTimeFlag > OUTPUT_CLOSE_DURATION_ms) {
			outputTimeFlag = timeNow;
			outputValve.open();
		}
		
		// Handle sample and gascal valve
		if (timeNow - targetTimeFlag > SAMPLE_PERIOD_ms) {
			targetTimeFlag = timeNow;
			avgSample += oversampleO2(OVERSAMPLE_COEFF, myAdc)/NUMBER_OF_REPETITIONS;
			i++;
		}
	}
	myPump.turnOff();
	sampleValve.close();
	outputValve.close();

	// Output aircal coefficients
	Utils::gascal_s result;
	result.gain = gascal_gain;
	result.offset = SAMPLE_PCT - result.gain*avgSample;
	snprintf(Utils::txBuf, TX_LEN, "Gain: %.3f Offset: %.3f\r\n", result.gain, result.offset);
	myUsart.sendString(Utils::txBuf);
	return result;
}

//============//
//=== MAIN ===//
//============//

int main(void)
{
	Utils::setSystemClock(Utils::SC_32M); // 32MHz internal oscillator

	UsartE0 myUsart0(Usart::BAUD_115k2, Utils::IL_MEDIUM);
	myUsart0.sendString("\r\n");
	myUsart0.sendString("+-------------------------------+\r\n");
	myUsart0.sendString("| Hello World! This is USARTE0! |\r\n");
	myUsart0.sendString("+-------------------------------+\r\n");

	myUsart0.sendString("Enabling Interrupts... ");
	Utils::enableInterruptLevel(Utils::IL_MEDIUM);
	Utils::enableInterruptLevel(Utils::IL_HIGH);
	Utils::globalInterruptEnable();
	myUsart0.sendString("DONE!\r\n");

	myUsart0.sendString("Initializing TCC0... ");
	Tcc0 myTimer(Tcc0::CS_DIV1, 2.048, Utils::IL_HIGH); // timeIncrement = 65536/32MHz
	myUsart0.sendString("DONE!\r\n");

	myUsart0.sendString("Initializing TWIE... ");
	Twie myTwie(155, Twie::IBT_50US, &myUsart0); // 155 = 100 kHz
	myUsart0.sendString("DONE!\r\n");

	myUsart0.sendString("Initializing ADC... ");
	Adca myAdc(Adca::P_DIV512, Utils::IL_HIGH);
	myUsart0.sendString("DONE!\r\n");

	myUsart0.sendString("Initializing Flow Sensor...\r\n");
	Ms5525dso myFlowSensor(Ms5525dso::OSR4096, 0x76, &myTwie, &myUsart0);
	myUsart0.sendString("DONE!\r\n");

	myUsart0.sendString("Initializing Pump... ");
	Pump myPump;
	myUsart0.sendString("DONE!\r\n");

	myUsart0.sendString("Initializing Valves... ");
	Valve sampleValve(1<<1);
	Valve gascalValve(1<<2);
	Valve outputValve(1<<4);
	myUsart0.sendString("DONE!\r\n");

	// Utils::gascal_s gascal;
	Utils::gascal_s gascal = {0.415,-67.522};
	while (true)
	{
		if (myUsart0.isRXC())
		{
			myUsart0.setRXC(false);
			char c = myUsart0.getChar();
			if (c == 'a') flow_acq(myFlowSensor, myAdc, myTimer, myUsart0);
			else if (c == 'b') start_test(gascal, myAdc, myFlowSensor, myPump, myTimer, myUsart0, sampleValve, outputValve);
			else if (c == 'c') gascal = start_gascal(myAdc, myPump, myTimer, myUsart0, sampleValve, gascalValve, outputValve);
			else if (c == 'd') gascal = start_aircal(gascal.gain, myAdc, myPump, myTimer, myUsart0, sampleValve, gascalValve, outputValve);
		}
	}
}
