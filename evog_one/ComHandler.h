/* 
* ComHandler.h
*/

#ifndef __COMHANDLER_H__
#define __COMHANDLER_H__

#include "Pump.h"
#include "Tcc0.h"
#include "TestHandler.h"
#include "UsartC1.h"
#include "UsartE0.h"

#define RX_LEN 64

class ComHandler
{
private:
	enum question_e {
		QUESTION_STATE = 0xA0,
		QUESTION_HEATING = 0xA1,
		QUESTION_DATE = 0xA3,
		QUESTION_TIME = 0xA4,
		QUESTION_ROOM_TEMPERATURE = 0xA8,
		QUESTION_SENSOR_TEMPERATURE = 0xA9
	};

	enum change_e {
		CHANGE_TO_HEATING = 0xC0,
		CHANGE_TO_AUTOCAL = 0xC1,
		CHANGE_TO_GASCAL = 0xC2,
		CHANGE_TO_DATA_SAMPLE = 0xC8,
		CHANGE_TO_VVM = 0xC9,
		CHANGE_START_VVM = 0xCA,
		CHANGE_START_DATA_SAMPLE = 0xCB,
		CHANGE_STOP_DATA_SAMPLE = 0xCF
	};

	enum error_e {
		UNKNOWN_COMMAND = 0xE0
	};

	enum status_e {
		CHANGED_TO_IDLE = 0xF1,
		CHANGED_TO_IDLEVVM = 0xF9
	};

public:

	enum state_e {
		IDLE = 0,
		HEATING = 1,
		AUTOCAL = 2,
		GASCAL = 3,
		DATA_SAMPLE = 4,
		VVM = 5
	};

	enum com_e {
		USART0,
		USART1
	};

private:
	
	static volatile state_e state;
	static volatile com_e com;
	static double timeFlag_ms;
	static char rxBuf[RX_LEN];
	
	static Tcc0* myTimer0;
	static UsartE0* myUsart0;
	static UsartC1* myUsart1;
	
	char fetchData();
	void questionHandler(question_e);
	void changeHandler(change_e);
	
public:

	ComHandler(state_e, Tcc0*, UsartE0*, UsartC1*);
	void handle(char, com_e);
	
	static void update();
	static void sendAnswer();
};

#endif //__COMHANDLER_H__
