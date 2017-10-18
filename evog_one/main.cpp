/*
 * evog_one.cpp
 */ 

#include "Adca.h"
#include "Ms5525dso.h"
#include "Tcc0.h"
#include "Twie.h"
#include "UsartE0.h"
#include "Utils.h"
#include <stdio.h>

//============//
//=== MAIN ===//
//============//

void flow_acq(Ms5525dso & myFlowSensor, Adca & myAdc, Tcc0 & myTimer, UsartE0 & myUsart)
{
	myUsart.sendString("sep=,\r\nTime [ms],Pressure [psi],Temperature [oC],O2 [mV]\r\n");
	while (!(myUsart.isRXC()))
	{
		Ms5525dso::data_s flowData = myFlowSensor.read();
		double o2_mV = myAdc.readChannel(Adca::CH_0)*1000./65535.;
		snprintf(Utils::txBuf, TX_LEN, "%.3f,%.4f,%.2f,%.4f\r\n",myTimer.getTime_ms(),flowData.pressure,flowData.temperature,o2_mV);
		myUsart.sendString(Utils::txBuf);
	}
	myUsart.setRXC(false);
}

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
	Adca myAdc(Adca::P_DIV512, Adca::G_X64, Utils::IL_HIGH);
	myUsart0.sendString("DONE!\r\n");
	
	myUsart0.sendString("Initializing Flow Sensor...\r\n");
	Ms5525dso myFlowSensor(Ms5525dso::OSR4096, 0x76, &myTwie, &myUsart0);
	myUsart0.sendString("DONE!\r\n");

    while (true)
    {
		if (myUsart0.isRXC())
		{
			myUsart0.setRXC(false);
			if (myUsart0.getChar() == 'a') flow_acq(myFlowSensor, myAdc, myTimer, myUsart0);
		}
	}
}
