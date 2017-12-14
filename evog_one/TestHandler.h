/* 
* TestHandler.h
*/

#ifndef __TESTHANDLER_H__
#define __TESTHANDLER_H__

#include "Adca.h"
#include "Ms5525dso.h"
#include "Pump.h"
#include "Tcc0.h"
#include "UsartE0.h"
#include "UsartC1.h"
#include "Utils.h"
#include "Valve.h"

class TestHandler
{
//=================//
//=== VARIABLES ===//
//=================//
private:
	static Utils::gascal_s gascal;
	static Adca* myAdc;
	static Ms5525dso* myFlowSensor;
	static Pump* myPump;
	static Tcc0* myTimer;
	static UsartE0* myUsart0;
	static UsartC1* myUsart1;
	static Valve* gascalValve;
	static Valve* sampleValve;
	static Valve* outputValve;
	static bool stopFlag;
	
//====================//
//=== CONSTRUCTORS ===//
//====================//	
public:
	TestHandler(Utils::gascal_s, Adca*, Ms5525dso*, Pump*, Tcc0*, UsartE0*, UsartC1*, Valve*, Valve*, Valve*);
	
//=================//
//=== FUNCTIONS ===//
//=================//
private:
	static double oversampleO2(uint16_t);
	static double o2_counts2pct(double);
	static void zero_chamber(uint16_t);
	
public:
	static void flow_acq();
	static void start_test();
	static void stop_test() {stopFlag = true;};
	static Utils::gascal_s start_gascal();
	static Utils::gascal_s start_aircal();
}; //TestHandler

#endif //__TESTHANDLER_H__
