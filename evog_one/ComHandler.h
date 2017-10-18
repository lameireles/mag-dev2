/* 
* ComHandler.h
*/

#ifndef __COMHANDLER_H__
#define __COMHANDLER_H__

#include "Pump.h"
#include "Tcc0.h"
#include "UsartE0.h"

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
		CHANGE_TO_DATA_SAMPLE = 0xC8,
		CHANGE_TO_AUTOCAL = 0xC1,
		CHANGE_TO_GASCAL = 0xC2,
		CHANGE_TO_VVM = 0xC9,
		CHANGE_START_VVM = 0xCA,
		CHANGE_START_DATA_SAMPLE = 0xCB
	};

	enum duration_e {
		HEATING_DURATION_s = 0,
		DATA_SAMPLE_PERIOD_s = 5,
		DATA_SAMPLE_DURATION_s = 150,
		AUTOCAL_DURATION_s = 10,
		GASCAL_DURATION_s = 10,
		VVM_PERIOD_s = 2,
		VVM_DURATION_s = 12
	};

	enum error_e {
		UNKNOWN_COMMAND = 0xE0
	};

	enum status_e {
		CHANGED_TO_IDLE = 0xF1,
		CHANGED_TO_IDLEVVM = 0xF9
	};

	enum state_e {
		IDLE = 0,
		HEATING = 1,
		AUTOCAL = 2,
		GASCAL = 3,
		DATA_SAMPLE = 4,
		VVM = 5
	};
	
	state_e state;
	int sampleIndex;
	double timeFlag1;
	double timeFlag2;
	double gascal_o2;
	double gascal_co2;
	
	Pump* myPump;
	Tcc0* myTimer0;
	UsartE0* myUsart0;
	
	#define COMHANDLER_BUFLEN 64
	char responseBuf[COMHANDLER_BUFLEN];
	char receiveBuf[COMHANDLER_BUFLEN];
	
	const double MIN_O2 = 0;
	const double MAX_O2 = 100;
	const double MIN_CO2 = 0;
	const double MAX_CO2 = 100;
	
	void questionHandler(question_e);
	void changeHandler(change_e);
	
public:
	ComHandler(Pump*, Tcc0*, UsartE0*);
	char* handle(char);
	void update();
};

#endif //__COMHANDLER_H__
