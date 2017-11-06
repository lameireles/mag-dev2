/* 
* Valves.h
*/

#ifndef __VALVES_H__
#define __VALVES_H__

class Valves
{
private:
	bool input_open;
	bool output_open;
public:
	Valves();
	void openInput();
	void openOutput();
	void closeInput();
	void closeOutput();
	void toggleInput();
	void toggleOutput();
	bool isInputOpen(){return input_open;};
	bool isOutputOpen(){return output_open;};
};

#endif //__VALVES_H__
