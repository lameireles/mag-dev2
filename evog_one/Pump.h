/* 
* Pump.h
*/

#ifndef __PUMP_H__
#define __PUMP_H__

class Pump
{
private:
	bool on;
public:
	Pump();
	void turnOn();
	void turnOff();
	void toggle();
	bool isOn(){return on;};
};

#endif //__PUMP_H__
