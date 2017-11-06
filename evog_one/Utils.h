/* 
* Utils.h
*/

#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>

#define TX_LEN 64

class Utils
{
public:
	enum systemClock_e
	{
		SC_2M = 0,
		SC_32M = 1,
		SC_32k768 = 2,
		SC_XOSC = 3,
		SC_PLL = 4
	};
	
	enum interruptLevel_e
	{
		IL_OFF = 0,
		IL_LOW = 1,
		IL_MEDIUM = 2,
		IL_HIGH = 3
	};
	
	static char txBuf[TX_LEN];
	
	static void setSystemClock(systemClock_e);
	static void enableInterruptLevel(interruptLevel_e);
	static void globalInterruptEnable();
	static void delay_ms(uint16_t);
	static void delay_us(uint16_t);
};

#endif //__UTILS_H__
