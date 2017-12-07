/* 
* Valve.h
*/

#ifndef __VALVE_H__
#define __VALVE_H__

#include <stdint.h>

class Valve
{
private:
	uint8_t pin;
	bool open_b;
public:
	Valve(uint8_t);
	void open();
	void close();
	void toggle();
	bool isOpen() {return open_b;};
	bool isClosed() {return !open_b;};
};

#endif //__VALVE_H__
