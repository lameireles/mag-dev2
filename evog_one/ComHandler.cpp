/* 
* ComHandler.cpp
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ComHandler.h"
#include "data_sample.h"
#include "vvm.h"

ComHandler::ComHandler(Pump* myPump, Tcc0* myTimer0, UsartE0* myUsart0)
{
	state = HEATING;
	this->myPump = myPump;
	this->myTimer0 = myTimer0;
	this->myUsart0 = myUsart0;
}

char* ComHandler::handle(char c)
{
	if ((c >> 4) == 0x0A) questionHandler((question_e)c); // Questions commands
	else if ((c >> 4) == 0x0C) changeHandler((change_e)c); // Change states
	else if (c == '\r') ; // ignore
	else {
		responseBuf[0] = UNKNOWN_COMMAND;
		responseBuf[1] = '\r';
		responseBuf[2] = '\0';
	}
	return responseBuf;
}

void ComHandler::questionHandler(question_e q)
{
	switch(q)
	{
		case QUESTION_STATE:
		responseBuf[0] = q + 0x10;
		snprintf(responseBuf+1, COMHANDLER_BUFLEN-1, "%d\r", state);
		break;
		
		case QUESTION_HEATING:
		responseBuf[0] = q + 0x10;
		if (state == IDLE) snprintf(responseBuf+1, COMHANDLER_BUFLEN-1, "0\r");
		else if (state == HEATING) snprintf(responseBuf+1, COMHANDLER_BUFLEN-1, "1\r");
		else snprintf(responseBuf+1, COMHANDLER_BUFLEN-1, "2\r");
		break;
		
		case QUESTION_DATE:
		responseBuf[0] = q + 0x10;
		snprintf(responseBuf+1, COMHANDLER_BUFLEN-1, "20170610\r");
		break;
		
		case QUESTION_TIME:
		responseBuf[0] = q + 0x10;
		snprintf(responseBuf+1, COMHANDLER_BUFLEN-1, "123456\r");
		break;
		
		case QUESTION_ROOM_TEMPERATURE:
		responseBuf[0] = q + 0x10;
		snprintf(responseBuf+1, COMHANDLER_BUFLEN-1, "250\r");
		break;
		
		case QUESTION_SENSOR_TEMPERATURE:
		responseBuf[0] = q + 0x10;
		snprintf(responseBuf+1, COMHANDLER_BUFLEN-1, "340\r");
		break;
		
		default:
		responseBuf[0] = UNKNOWN_COMMAND;
		responseBuf[1] = '\r';
		responseBuf[2] = '\0';
		break;
	}
}

void ComHandler::changeHandler(change_e c)
{
	int index = 0;
	char* decayed_buf = &receiveBuf[0];
	char** endptr1 = &decayed_buf;
	char** endptr2 = endptr1;
	
	switch (c)
	{
		case CHANGE_START_DATA_SAMPLE:
		responseBuf[0] = '\0';
		if (state == IDLE)
		{
			//snprintf(global_txBuf+1, TX_LEN-1, "0\r");
			sampleIndex = -1;
			timeFlag1 = myTimer0->getTime_s();
			timeFlag2 = myTimer0->getTime_s();
			myPump->turnOn();
			state = DATA_SAMPLE;
		}
		else snprintf(responseBuf+1, COMHANDLER_BUFLEN-1, "1\r");
		break;

		case CHANGE_TO_DATA_SAMPLE:
		responseBuf[0] = c + 0x10;
		if (state == IDLE)
		{
			snprintf(responseBuf+1, COMHANDLER_BUFLEN-1, "0\r");
			timeFlag1 = myTimer0->getTime_s();
			state = IDLE;
		}
		else snprintf(responseBuf+1, COMHANDLER_BUFLEN-1, "1\r");
		break;
		
		case CHANGE_TO_AUTOCAL:
		responseBuf[0] = c + 0x10;
		if (state == IDLE)
		{
			snprintf(responseBuf+1, COMHANDLER_BUFLEN-1, "0\r");
			timeFlag1 = myTimer0->getTime_s();
			state = AUTOCAL;
		}
		else snprintf(responseBuf+1, COMHANDLER_BUFLEN-1, "1\r");
		break;
		
		case CHANGE_TO_GASCAL:
		responseBuf[0] = c + 0x10;
		while (true) // breaks off when '\r' is received
		{
			while (!myUsart0->isRXC()) ; // wait for data
			receiveBuf[index++] = myUsart0->getChar(); // append to buffer
			myUsart0->setRXC(false);
			if (receiveBuf[index-1] == '\r') break;
		}
		gascal_o2 = strtod(receiveBuf, endptr1); // read O2
		gascal_co2 = strtod(*endptr1, endptr2); // read CO2
		memset(receiveBuf,'\0', *endptr2-receiveBuf); // clear buffer
		if (gascal_o2 < MIN_O2 || gascal_o2 > MAX_O2 ||
		gascal_co2 < MIN_CO2 || gascal_co2 > MAX_CO2 ||
		gascal_o2 + gascal_co2 > 100 || state != IDLE) // error cases
		{
			snprintf(responseBuf+1, COMHANDLER_BUFLEN-1, "1\r");
			} else {
			//snprintf(global_txBuf+1, TX_LEN-1, "0 %.2f %.2f\r", global_gascal_o2, global_gascal_co2); // debug
			snprintf(responseBuf+1, COMHANDLER_BUFLEN-1, "0\r");
			timeFlag1 = myTimer0->getTime_s();
			state = GASCAL;
		}
		break;
		
		case CHANGE_START_VVM:
		responseBuf[0] = '\0';
		if (state == IDLE)
		{
			//snprintf(global_txBuf+1, TX_LEN-1, "0\r");
			sampleIndex = 0;
			timeFlag1 = myTimer0->getTime_s();
			timeFlag2 = timeFlag1;
			state = VVM;
		}
		else snprintf(responseBuf+1, COMHANDLER_BUFLEN-1, "1\r");
		break;
		
		case CHANGE_TO_VVM:
		responseBuf[0] = c + 0x10;
		if (state == IDLE)
		{
			snprintf(responseBuf+1, COMHANDLER_BUFLEN-1, "0\r");
			timeFlag1 = myTimer0->getTime_s();
			state = IDLE;
		}
		else snprintf(responseBuf+1, COMHANDLER_BUFLEN-1, "1\r");
		break;
		
		default:
		responseBuf[0] = UNKNOWN_COMMAND;
		responseBuf[1] = '\r';
		responseBuf[2] = '\0';
		break;
	}
}

void ComHandler::update()
{
	switch (state)
	{
		case IDLE:
		default:
		break;
		
		case HEATING:
		if (myTimer0->getTime_s() - timeFlag1 > HEATING_DURATION_s)
		{
			state = IDLE;
			myUsart0->sendChar(CHANGED_TO_IDLE);
			myUsart0->sendChar('\r');
		}
		break;
		
		case AUTOCAL:
		if (myTimer0->getTime_s() - timeFlag1 > AUTOCAL_DURATION_s)
		{
			state = IDLE;
			myUsart0->sendChar(CHANGED_TO_IDLE);
			myUsart0->sendChar('\r');
		}
		break;
		
		case GASCAL:
		if (myTimer0->getTime_s() - timeFlag1 > GASCAL_DURATION_s)
		{
			state = IDLE;
			myUsart0->sendChar(CHANGED_TO_IDLE);
			myUsart0->sendChar('\r');
		}
		break;
		
		case DATA_SAMPLE:
		if (myTimer0->getTime_s() - timeFlag1 > DATA_SAMPLE_DURATION_s)
		{
			if(sampleIndex >= 0 && sampleIndex < NUM_OF_SAMPLES)
			{
				myUsart0->sendChar(0x03);
				myUsart0->sendString(SAMPLE[sampleIndex]);
				myUsart0->sendChar('\r');
			}
			myPump->turnOff();
			state = IDLE;
			myUsart0->sendChar(CHANGED_TO_IDLE);
			myUsart0->sendChar('\r');
		}
		else if (myTimer0->getTime_s() - timeFlag2 > DATA_SAMPLE_PERIOD_s)
		{
			if(sampleIndex >= 0 && sampleIndex < NUM_OF_SAMPLES)
			{
				myUsart0->sendChar(0x03);
				myUsart0->sendString(SAMPLE[sampleIndex]);
				myUsart0->sendChar('\r');
			}
			sampleIndex += 1;
			timeFlag2 = myTimer0->getTime_s();
		}
		break;
		
		case VVM:
		if (myTimer0->getTime_s() - timeFlag1 > VVM_DURATION_s)
		{
			if(sampleIndex >= 0 && sampleIndex < NUM_OF_SAMPLES_VVM)
			{
				myUsart0->sendChar(0x02);
				myUsart0->sendString(VVM_SAMPLE[sampleIndex]);
				myUsart0->sendChar('\r');
			}
			state = IDLE;
			myUsart0->sendChar(CHANGED_TO_IDLEVVM);
			myUsart0->sendChar('\r');
		}
		else if (myTimer0->getTime_s() - timeFlag2 > VVM_PERIOD_s)
		{
			if(sampleIndex >= 0 && sampleIndex < NUM_OF_SAMPLES_VVM)
			{
				myUsart0->sendChar(0x02);
				myUsart0->sendString(VVM_SAMPLE[sampleIndex]);
				myUsart0->sendChar('\r');
				
			}
			sampleIndex += 1;
			timeFlag2 = myTimer0->getTime_s();
		}
		break;
	}
}