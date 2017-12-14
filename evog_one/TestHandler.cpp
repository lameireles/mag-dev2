/* 
* TestHandler.cpp
*/

#include "ComHandler.h"
#include "TestHandler.h"
#include <stdio.h>
#include <math.h>

//===============//
//=== STATICS ===//
//===============//

Utils::gascal_s TestHandler::gascal;
Adca* TestHandler::myAdc;
Ms5525dso* TestHandler::myFlowSensor;
Pump* TestHandler::myPump;
Tcc0* TestHandler::myTimer;
UsartE0* TestHandler::myUsart0;
UsartC1* TestHandler::myUsart1;
Valve* TestHandler::gascalValve;
Valve* TestHandler::sampleValve;
Valve* TestHandler::outputValve;
bool TestHandler::stopFlag = false;

//===================//
//=== CONSTRUCTOR ===//
//===================//

TestHandler::TestHandler(Utils::gascal_s gascal, Adca* myAdc, Ms5525dso* myFlowSensor, Pump* myPump, Tcc0* myTimer, UsartE0* myUsart0, UsartC1* myUsart1, Valve* gascalValve, Valve* sampleValve, Valve* outputValve)
{
	TestHandler::gascal = gascal;
	TestHandler::myAdc = myAdc;
	TestHandler::myFlowSensor = myFlowSensor;
	TestHandler::myPump = myPump;
	TestHandler::myTimer = myTimer;
	TestHandler::myUsart0 = myUsart0;
	TestHandler::myUsart1 = myUsart1;
	TestHandler::gascalValve = gascalValve;
	TestHandler::sampleValve = sampleValve;
	TestHandler::outputValve = outputValve;
}

//=========================//
//=== PRIVATE FUNCTIONS ===//
//=========================//

double TestHandler::oversampleO2(uint16_t I)
{
	uint32_t totalO2 = 0;
	for (uint16_t i = 0; i < I; i++) totalO2 += myAdc->readChannel(Adca::CH_0);
	return ((double) totalO2)/I;
}

double TestHandler::o2_counts2pct(double counts)
{
	return counts*gascal.gain + gascal.offset;
}

void TestHandler::zero_chamber(uint16_t duration_ms)
{
	double timeflag_ms = myTimer->getTime_ms();
	outputValve->open();
	while (myTimer->getTime_ms() - timeflag_ms < duration_ms) ;
	outputValve->close();
}

//========================//
//=== PUBLIC FUNCTIONS ===//
//========================//

void TestHandler::flow_acq()
{
	const char* header = "sep=,\r\nTime [ms],Pressure [psi],Temperature [oC],O2 [counts]\r\n";
	myUsart0->sendString(header);
	myUsart1->sendString(header);
	
	while (!(myUsart0->isRXC()) || !(myUsart1->isRXC()))
	{
		Ms5525dso::data_s flowData = myFlowSensor->read();
		uint16_t o2_counts = myAdc->readChannel(Adca::CH_0);
		snprintf(Utils::txBuf, TX_LEN, "%.3f,%.4f,%.2f,%d\r\n", myTimer->getTime_ms(), flowData.pressure, flowData.temperature, o2_counts);
		myUsart0->sendString(Utils::txBuf);
		myUsart1->sendString(Utils::txBuf);
	}
	myUsart0->setRXC(false);
	myUsart1->setRXC(false);
}

void TestHandler::start_test()
{
	// USER DEFINED CONSTANTS
	const uint16_t OUTPUT_OPEN_DURATION_ms = 2000;
	const uint16_t OUTPUT_CLOSE_DURATION_ms = 8000;
	const uint16_t SAMPLE_PERIOD_ms = 10000;
	const uint16_t OVERSAMPLE_COEFF = 1024;
	const uint8_t ZERO_FLOW_NUMBER_OF_SAMPLES = 16;
	const uint16_t AUTOZERO_DURATION_ms = 30000;
	const double FLOW_THRESHOLD = 0.075;
	const double VE_GAIN = 0.0741;
	
	// Zero-flow set
	double zero_press = 0;
	for (int i = 0; i < ZERO_FLOW_NUMBER_OF_SAMPLES; i++) {
		Ms5525dso::data_s zero = myFlowSensor->read();
		zero_press += zero.pressure/ZERO_FLOW_NUMBER_OF_SAMPLES;
	}

// 	snprintf(Utils::txBuf, TX_LEN, "sep=,\r\nTime [ms],O2 [%], VE [l]\r\n");
// 	ComHandler::sendAnswer();
	
	double totalVE = 0;
	double outputTimeFlag = myTimer->getTime_ms();
	double sampleTimeFlag = myTimer->getTime_ms();
	
	sampleValve->close();
	outputValve->open();
	myPump->turnOn();
	
	while (!(myUsart0->isRXC()) || !(myUsart1->isRXC()) || !(stopFlag))
	{
		double timeNow_ms = myTimer->getTime_ms();
		
		// Handle output valve
		if (outputValve->isOpen() && timeNow_ms - outputTimeFlag > OUTPUT_OPEN_DURATION_ms) {
			outputTimeFlag = timeNow_ms;
			outputValve->close();
		} else if (outputValve->isClosed() && timeNow_ms - outputTimeFlag > OUTPUT_CLOSE_DURATION_ms) {
			outputTimeFlag = timeNow_ms;
			outputValve->open();
		}
		
		// Handle o2 sample
		if (timeNow_ms - sampleTimeFlag > SAMPLE_PERIOD_ms) {
			sampleTimeFlag = timeNow_ms;
			double o2_pct = o2_counts2pct(oversampleO2(OVERSAMPLE_COEFF));
			double ve_l = VE_GAIN*totalVE;
			snprintf(Utils::txBuf, TX_LEN, "%04d;%03d;%02.2f;%01.2f;%01.2f;%02.2f;%02.2f;%01.2f\r\n", (int)(timeNow_ms/1000), 0, ve_l, 0., 0., ve_l*6, o2_pct, 0.);
			ComHandler::sendAnswer();
			totalVE = 0;
		}
		
		// Handle sample valve
		Ms5525dso::data_s data = myFlowSensor->read();
		double diff = data.pressure - zero_press;
		if (diff >= 0) totalVE += sqrt(diff);
		if (diff > FLOW_THRESHOLD) sampleValve->open();
		else sampleValve->close();
	}
	myUsart0->setRXC(false);
	myUsart1->setRXC(false);
	stopFlag = false;
	
	sampleValve->open();
	zero_chamber(AUTOZERO_DURATION_ms);
	sampleValve->close();
	myPump->turnOff();
}

Utils::gascal_s TestHandler::start_gascal()
{
	// USER DEFINED CONSTANTS
	const uint8_t NUMBER_OF_REPETITIONS = 8;
	const uint16_t CHAMBER_ZERO_ms = 3000;
	const uint16_t SAMPLE_OPEN_DURATION_ms = 12000;
	const uint16_t GASCAL_OPEN_DURATION_ms = 12000;
	const uint16_t OVERSAMPLE_COEFF = 1024;
	const uint16_t FINAL_AUTOZERO_DURATION_ms = 30000;
	const double GASCAL_PCT = 17;
	const double SAMPLE_PCT = 20.97;

	double avgSample = 0, avgGascal = 0;
	double sampleCounts = 0, gasCounts = 0;
	double targetTimeFlag = myTimer->getTime_ms();
	
	sampleValve->open();
	gascalValve->close();
	outputValve->close();
	myPump->turnOn();
	
	uint8_t i = 0;
	while (i < NUMBER_OF_REPETITIONS) {
		double timeNow = myTimer->getTime_ms();

		// Handle sample and gascal valve
		if (sampleValve->isOpen() && timeNow - targetTimeFlag > SAMPLE_OPEN_DURATION_ms) {
			
			targetTimeFlag = timeNow;
			sampleCounts = oversampleO2(OVERSAMPLE_COEFF);
			avgSample += sampleCounts/NUMBER_OF_REPETITIONS;
			
			sampleValve->close();
			zero_chamber(CHAMBER_ZERO_ms);
			gascalValve->open();
			
		} else if (gascalValve->isOpen() && timeNow - targetTimeFlag > GASCAL_OPEN_DURATION_ms) {
			
			targetTimeFlag = timeNow;
			gasCounts = oversampleO2(OVERSAMPLE_COEFF);
			avgGascal += gasCounts/NUMBER_OF_REPETITIONS;
			
// 			snprintf(Utils::txBuf, TX_LEN, "Rep: %d\r\nSample: %.3f\r\nGas: %.3f", i, sampleCounts, gasCounts);
// 			ComHandler::sendAnswer();
			i++;
			
			gascalValve->close();
			zero_chamber(CHAMBER_ZERO_ms);
			sampleValve->open();
		}
	}
	gascalValve->close();
	sampleValve->open();
	zero_chamber(FINAL_AUTOZERO_DURATION_ms);
	sampleValve->close();
	myPump->turnOff();
	
	// Output gascal coefficients
// 	snprintf(Utils::txBuf, TX_LEN, "avgSample: %.3f counts\r\navgGascal: %.3f counts\r\n", avgSample, avgGascal);
// 	ComHandler::sendAnswer();
	Utils::gascal_s result;
	result.gain = (SAMPLE_PCT - GASCAL_PCT)/(avgSample-avgGascal);
	result.offset = SAMPLE_PCT - result.gain*avgSample;
// 	snprintf(Utils::txBuf, TX_LEN, "Gain: %.3f[%%/counts]\r\nOffset: %.3f%%\r\n", result.gain, result.offset);
// 	ComHandler::sendAnswer();
	return result;
}

Utils::gascal_s TestHandler::start_aircal()
{
	// USER DEFINED CONSTANTS
	const uint8_t NUMBER_OF_REPETITIONS = 8;
	const uint16_t CHAMBER_ZERO_ms = 3000;
	const uint16_t VALVE_OPEN_DURATION_ms = 12000;
	const uint16_t OVERSAMPLE_COEFF = 1024;
	const uint16_t FINAL_AUTOZERO_DURATION_ms = 30000;
	const double SAMPLE_PCT = 20.97;

	double avgCounts = 0, counts = 0;
	double targetTimeFlag = myTimer->getTime_ms();
	
	gascalValve->open();
	outputValve->close();
	myPump->turnOn();
	
	uint8_t i = 0;
	while (i < NUMBER_OF_REPETITIONS) {
		double timeNow = myTimer->getTime_ms();
		
		if (gascalValve->isOpen() && timeNow - targetTimeFlag > VALVE_OPEN_DURATION_ms) {
			
			targetTimeFlag = timeNow;
			counts = oversampleO2(OVERSAMPLE_COEFF);
			avgCounts += counts/NUMBER_OF_REPETITIONS;
			
// 			snprintf(Utils::txBuf, TX_LEN, "Rep: %d\r\nCounts: %.3f", i, counts);
// 			ComHandler::sendAnswer();
			i++;
			
			gascalValve->close();
			zero_chamber(CHAMBER_ZERO_ms);
			gascalValve->open();
		}
	}
	gascalValve->open();
	zero_chamber(FINAL_AUTOZERO_DURATION_ms);
	gascalValve->close();
	myPump->turnOff();
	
	// Output aircal coefficients
// 	snprintf(Utils::txBuf, TX_LEN, "avgCounts: %.3f counts\r\n", avgCounts);
// 	ComHandler::sendAnswer();
	Utils::gascal_s result;
	result.gain = gascal.gain;
	result.offset = SAMPLE_PCT - result.gain*avgCounts;
// 	snprintf(Utils::txBuf, TX_LEN, "Gain: %.3f[%%/counts]\r\nOffset: %.3f%%\r\n", result.gain, result.offset);
// 	ComHandler::sendAnswer();
	return result;
}