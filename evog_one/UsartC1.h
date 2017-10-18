/* 
* UsartC1.h
*/

#ifndef __USARTC1_H__
#define __USARTC1_H__

#include "Usart.h"
#include "Utils.h"

class UsartC1
{
public:
	UsartC1(Usart::baudrate_s,
			Utils::interruptLevel_e,
			Utils::interruptLevel_e,
			Utils::interruptLevel_e);
			
	void sendChar(char);
	void sendString(const char* str);
	
	static volatile bool rxcFlag;
	static volatile bool txcFlag;
	static volatile bool dreFlag;
	static volatile char rxChar;
	
	bool isRXC(){return rxcFlag;}
	bool isTXC(){return txcFlag;}
	bool isDRE(){return dreFlag;}
	char getChar(){return rxChar;}
	
	void setRXC(bool b){rxcFlag = b;}
	void setTXC(bool b){txcFlag = b;}
	void setDRE(bool b){dreFlag = b;}
};

#endif //__USARTC1_H__
