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
#include "Valves.h"
#include <stdio.h>

//=========================//
//=== PRIVATE FUNCTIONS ===//
//=========================//

void flow_acq(Ms5525dso & myFlowSensor, Adca & myAdc, Tcc0 & myTimer, UsartE0 & myUsart)
{
	myUsart.sendString("sep=,\r\nTime [ms],Pressure [psi],Temperature [oC],O2 [counts]\r\n");
	while (!(myUsart.isRXC()))
	{
		Ms5525dso::data_s flowData = myFlowSensor.read();
		uint16_t o2_counts = myAdc.readChannel(Adca::CH_0);
		snprintf(Utils::txBuf, TX_LEN, "%.3f,%.4f,%.2f,%d\r\n", myTimer.getTime_ms(), flowData.pressure, flowData.temperature,o2_counts);
		myUsart.sendString(Utils::txBuf);
	}
	myUsart.setRXC(false);
}

void start_test(Pump & myPump, Valves & myValves, Adca & myAdc, Tcc0 & myTimer, UsartE0 & myUsart)
{
	const uint16_t INPUT_TOGGLE_DURATION_ms = 3000;
	const uint8_t INPUT_TOGGLE_PERIOD_ms = 50;
	const uint16_t INPUT_CLOSE_DURATION_ms = 2000;
	const uint8_t NUMBER_OF_READINGS = 16;
	const uint16_t OUTPUT_OPEN_DURATIONS_ms = 5000;
	
	myUsart.sendString("sep=,\r\nTime [ms],O2 [counts]\r\n");
	myPump.turnOn();
	while (!(myUsart.isRXC()))
	{
		// Toggle input valve for specified duration and period
		double timeFlag = myTimer.getTime_ms();
		while (myTimer.getTime_ms() - timeFlag < INPUT_TOGGLE_DURATION_ms)
		{
			myValves.toggleInput();
			Utils::delay_ms(INPUT_TOGGLE_PERIOD_ms);
		}
		myPump.turnOff();
		myValves.closeInput();
		Utils::delay_ms(INPUT_CLOSE_DURATION_ms);
		
		// Get sensor readings
		uint32_t totalO2 = 0;
		for (int i = 0; i < NUMBER_OF_READINGS; i++) totalO2 += myAdc.readChannel(Adca::CH_0);
		snprintf(Utils::txBuf, TX_LEN, "%.3f,%ld\r\n", myTimer.getTime_ms(), totalO2/NUMBER_OF_READINGS);
		myUsart.sendString(Utils::txBuf);
		
		// Open output for specified duration
		myValves.openOutput();
		myPump.turnOn();
		Utils::delay_ms(OUTPUT_OPEN_DURATIONS_ms);
		myValves.closeOutput();
	}
	myPump.turnOff();
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
	Valves myValves;
	myUsart0.sendString("DONE!\r\n");

    while (true)
    {
		if (myUsart0.isRXC())
		{
			myUsart0.setRXC(false);
			char c = myUsart0.getChar();
			if (c == 'a') flow_acq(myFlowSensor, myAdc, myTimer, myUsart0);
			else if (c == 'b') start_test(myPump, myValves, myAdc, myTimer, myUsart0);
		}
	}
}
