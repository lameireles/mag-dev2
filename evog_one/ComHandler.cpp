/* 
* ComHandler.cpp
*/

#include "ComHandler.h"
#include "Utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//===============//
//=== STATICS ===//
//===============//

ComHandler::state_e volatile ComHandler::state = ComHandler::IDLE;
ComHandler::com_e volatile ComHandler::com = ComHandler::USART1;
double ComHandler::timeFlag_ms;
char ComHandler::rxBuf[RX_LEN];
Tcc0* ComHandler::myTimer0;
UsartE0* ComHandler::myUsart0;
UsartC1* ComHandler::myUsart1;

//=================//
//=== CONSTANTS ===//
//=================//

const uint16_t HEATING_DURATION_ms = 20000;

//===================//
//=== CONSTRUCTOR ===//
//===================//

ComHandler::ComHandler(state_e state, Tcc0* myTimer0, UsartE0* myUsart0, UsartC1* myUsart1)
{
	ComHandler::state = state;
	ComHandler::myTimer0 = myTimer0;
	ComHandler::myUsart0 = myUsart0;
	ComHandler::myUsart1 = myUsart1;
}

//========================//
//=== PUBLIC FUNCTIONS ===//
//========================//

void ComHandler::handle(char c, com_e com)
{
	ComHandler::com = com;
	if ((c >> 4) == 0x0A) questionHandler((question_e)c); // Questions commands
	else if ((c >> 4) == 0x0C) changeHandler((change_e)c); // Change states
	else if (c == '\r') ; // ignore
	else {
		Utils::txBuf[0] = UNKNOWN_COMMAND;
		Utils::txBuf[1] = '\r';
		Utils::txBuf[2] = '\0';
		sendAnswer();
	}
}

//========================//
//=== STATIC FUNCTIONS ===//
//========================//

void ComHandler::update()
{
	switch(state) {
		default: break;
		
		case HEATING:
		if (myTimer0->getTime_ms() - timeFlag_ms > HEATING_DURATION_ms)
		{
			state = IDLE;
			switch (ComHandler::com) {
				case USART0:
				myUsart0->sendChar(CHANGED_TO_IDLE);
				myUsart0->sendChar('\r');
				myUsart0->sendChar('\0');
				break;
				case USART1:
				myUsart1->sendChar(CHANGED_TO_IDLE);
				myUsart1->sendChar('\r');
				myUsart1->sendChar('\0');
				break;
			}
		}
		break;
	}
}

void ComHandler::sendAnswer()
{
	switch(com) {
		default:
		case USART0:
		myUsart0->sendString(Utils::txBuf);
		break;
		case USART1:
		myUsart1->sendString(Utils::txBuf);
		break;
	}
}

//=========================//
//=== PRIVATE FUNCTIONS ===//
//=========================//

char ComHandler::fetchData()
{
	switch(com) {
		default:
		case USART0:
		while(!(myUsart0->isRXC())) ;
		myUsart0->setRXC(false);
		return myUsart0->getChar();

		case USART1:
		while(!(myUsart1->isRXC())) ;
		myUsart1->setRXC(false);
		return myUsart1->getChar();
	}
}

void ComHandler::questionHandler(question_e q)
{
	switch(q)
	{
		case QUESTION_STATE:
		Utils::txBuf[0] = q + 0x10;
		snprintf(Utils::txBuf+1, TX_LEN-1, "%d\r", state);
		break;
		
		case QUESTION_HEATING:
		Utils::txBuf[0] = q + 0x10;
		if (state == IDLE) snprintf(Utils::txBuf+1, TX_LEN-1, "0\r");
		else if (state == HEATING) snprintf(Utils::txBuf+1, TX_LEN-1, "1\r");
		else snprintf(Utils::txBuf+1, TX_LEN-1, "2\r");
		break;
		
		case QUESTION_DATE:
		Utils::txBuf[0] = q + 0x10;
		snprintf(Utils::txBuf+1, TX_LEN-1, "20170610\r");
		break;
		
		case QUESTION_TIME:
		Utils::txBuf[0] = q + 0x10;
		snprintf(Utils::txBuf+1, TX_LEN-1, "123456\r");
		break;
		
		case QUESTION_ROOM_TEMPERATURE:
		Utils::txBuf[0] = q + 0x10;
		snprintf(Utils::txBuf+1, TX_LEN-1, "250\r");
		break;
		
		case QUESTION_SENSOR_TEMPERATURE:
		Utils::txBuf[0] = q + 0x10;
		snprintf(Utils::txBuf+1, TX_LEN-1, "340\r");
		break;
		
		default:
		Utils::txBuf[0] = UNKNOWN_COMMAND;
		Utils::txBuf[1] = '\r';
		Utils::txBuf[2] = '\0';
		break;
	}
	sendAnswer();
}

void ComHandler::changeHandler(change_e c)
{
	int index = 0;
	bool r = false;
	double gascal_o2_pct, gascal_co2_pct;
	char* decayed_buf = &Utils::txBuf[0];
	char** endptr1 = &decayed_buf;
	char** endptr2 = endptr1;
	
	Utils::txBuf[0] = c+0x10;
	switch (c)
	{
		case CHANGE_TO_HEATING:
		if (state == IDLE) {
			state = HEATING;
			snprintf(Utils::txBuf+1, TX_LEN-1, "0\r");
			timeFlag_ms = myTimer0->getTime_ms();
		} else {
			snprintf(Utils::txBuf+1, TX_LEN-1, "1\r");
		}
		sendAnswer();
		break;

		case CHANGE_START_DATA_SAMPLE:
		if (state == IDLE) {
			state = DATA_SAMPLE;
			TestHandler::start_test();
		} else  {
			snprintf(Utils::txBuf+1, TX_LEN-1, "1\r");
			sendAnswer();
		}
		break;
		
		case CHANGE_STOP_DATA_SAMPLE:
		if (state == DATA_SAMPLE) {
			state = IDLE;
			TestHandler::stop_test();
			snprintf(Utils::txBuf+1, TX_LEN-1, "0\r");
		} else {
			snprintf(Utils::txBuf+1, TX_LEN-1, "1\r");
		}
		sendAnswer();
		break;

		case CHANGE_TO_DATA_SAMPLE:
		if (state == IDLE) {
			snprintf(Utils::txBuf+1, TX_LEN-1, "0\r");
		} else {
			snprintf(Utils::txBuf+1, TX_LEN-1, "1\r");
		}
		sendAnswer();
		break;
		
		case CHANGE_TO_AUTOCAL:
		if (state == IDLE) {
			state = AUTOCAL;
			snprintf(Utils::txBuf+1, TX_LEN-1, "0\r");
			sendAnswer();
			TestHandler::start_aircal();
		} else {
			snprintf(Utils::txBuf+1, TX_LEN-1, "1\r");
			sendAnswer();
		}
		break;
		
		case CHANGE_TO_GASCAL:
		while (!r) // breaks off when '\r' is received
		{
			rxBuf[index++] = fetchData(); // append to buffer
			if (Utils::txBuf[index-1] == '\r') r = true;
		}
		gascal_o2_pct = strtod(Utils::txBuf, endptr1); // read O2
		gascal_co2_pct = strtod(*endptr1, endptr2); // read CO2
		memset(Utils::txBuf,'\0', *endptr2-Utils::txBuf); // clear buffer
		if (gascal_o2_pct < 0 || gascal_o2_pct > 100 ||
			gascal_co2_pct < 0 || gascal_co2_pct > 100 ||
			gascal_o2_pct + gascal_co2_pct > 100 || state != IDLE) // error cases
		{
			snprintf(Utils::txBuf+1, TX_LEN-1, "1\r");
			sendAnswer();
		} else {
			//snprintf(Utils::txBuf+1, TX_LEN-1, "0 %.2f %.2f\r", gascal_o2_pct, gascal_co2_pct); // debug
			state = GASCAL;
			snprintf(Utils::txBuf+1, TX_LEN-1, "0\r");
			sendAnswer();
			TestHandler::start_gascal();
		}
		break;
		
		case CHANGE_START_VVM:
		snprintf(Utils::txBuf+1, TX_LEN-1, "1\r");
		sendAnswer();
		break;
		
		case CHANGE_TO_VVM:
		snprintf(Utils::txBuf+1, TX_LEN-1, "1\r");
		sendAnswer();
		break;
		
		default:
		Utils::txBuf[0] = UNKNOWN_COMMAND;
		Utils::txBuf[1] = '\r';
		Utils::txBuf[2] = '\0';
		sendAnswer();
		break;
	}
}