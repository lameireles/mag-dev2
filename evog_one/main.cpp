/*
 * evog_one.cpp
 */

#include "Adca.h"
#include "ComHandler.h"
#include "Ms5525dso.h"
#include "Pump.h"
#include "Tcc0.h"
#include "TestHandler.h"
#include "Twie.h"
#include "UsartC1.h"
#include "UsartE0.h"
#include "Utils.h"
#include "Valve.h"
#include <avr/io.h>
#include <math.h>
#include <stdio.h>

//=========================//
//=== PRIVATE FUNCTIONS ===//
//=========================//

void reset_timer() {
	Tcc0::time_ms = 0;
}

//============//
//=== MAIN ===//
//============//

int main(void)
{
	Utils::setSystemClock(Utils::SC_32M); // 32MHz internal oscillator

	Adca myAdc(Adca::P_DIV512, Utils::IL_HIGH);
	Tcc0 myTimer(Tcc0::CS_DIV1, 2.048, Utils::IL_HIGH); // timeIncrement = 65536/32MHz
	Twie myTwie(155, Twie::IBT_50US); // 155 = 100 kHz
	UsartE0 myUsart0(Usart::BAUD_115k2, Utils::IL_MEDIUM);
	UsartC1 myUsart1(Usart::BAUD_9k6, Utils::IL_MEDIUM);
	
	Pump myPump;
	Valve sampleValve(1<<1);
	Valve gascalValve(1<<2);
	Valve outputValve(1<<4);
	
	Utils::gascal_s gascal = {0.5,-86.392}; // TODO: EEPROM saving of cal coeffs
	Ms5525dso myFlowSensor(Ms5525dso::OSR4096, 0x76, &myTwie);
	TestHandler myTestHandler(gascal, &myAdc, &myFlowSensor, &myPump, &myTimer, &myUsart0, &myUsart1, &gascalValve, &sampleValve, &outputValve);
	
	ComHandler myComHandler(ComHandler::IDLE, &myTimer, &myUsart0, &myUsart1);
	
	Utils::enableInterruptLevel(Utils::IL_MEDIUM);
	Utils::enableInterruptLevel(Utils::IL_HIGH);
	Utils::globalInterruptEnable();

	while (true)
	{		
		if (myUsart0.isRXC())
		{
			myUsart0.setRXC(false);
			char c = myUsart0.getChar();
			myComHandler.handle(c, ComHandler::USART0);
			// DEBUG:
//  		if (c == 'a') reset_timer();
//  		else if (c == 'b') start_test(gascal, myAdc, myFlowSensor, myPump, myTimer, myUsart0, sampleValve, outputValve);
//  		else if (c == 'c') gascal = start_gascal(myAdc, myPump, myTimer, myUsart0, sampleValve, gascalValve, outputValve);
//  		else if (c == 'd') gascal = start_aircal(gascal.gain, myAdc, myPump, myTimer, myUsart0, gascalValve, outputValve);
			// END OF DEBUG
		}
		
		if (myUsart1.isRXC())
		{
			myUsart1.setRXC(false);
			char d = myUsart1.getChar();
			myComHandler.handle(d, ComHandler::USART1);
			// DEBUG:
// 			if (c == 'a') reset_timer();
// 			else if (c == 'b') start_test(gascal, myAdc, myFlowSensor, myPump, myTimer, myUsart0, sampleValve, outputValve);
// 			else if (c == 'c') gascal = start_gascal(myAdc, myPump, myTimer, myUsart0, sampleValve, gascalValve, outputValve);
// 			else if (c == 'd') gascal = start_aircal(gascal.gain, myAdc, myPump, myTimer, myUsart0, gascalValve, outputValve);
			// END OF DEBUG
		}
	}
}
