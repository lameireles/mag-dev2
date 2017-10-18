/* 
* Usart.h
*/

#ifndef __USART_H__
#define __USART_H__

#include <stdint.h>

class Usart
{
//================//
//=== TYPEDEFS ===//
//================//
public:
	struct baudrate_s
	{
		uint16_t bsel;
		int8_t bscale;
		bool clk2x;
	};
	
//=================//
//=== CONSTANTS ===//
//=================//
public:
	static const baudrate_s BAUD_2k4;
	static const baudrate_s BAUD_4k8;
	static const baudrate_s BAUD_9k6;
	static const baudrate_s BAUD_14k4;
	static const baudrate_s BAUD_19k2;
	static const baudrate_s BAUD_28k8;
	static const baudrate_s BAUD_38k4;
	static const baudrate_s BAUD_57k6;
	static const baudrate_s BAUD_76k8;
	static const baudrate_s BAUD_115k2;
	static const baudrate_s BAUD_230k4;
	static const baudrate_s BAUD_460k8;
	static const baudrate_s BAUD_921k6;
	static const baudrate_s BAUD_1M382;
	static const baudrate_s BAUD_1M843;
	static const baudrate_s BAUD_2M;
	static const baudrate_s BAUD_2M304;
	static const baudrate_s BAUD_2M5;
	static const baudrate_s BAUD_3M;
	static const baudrate_s BAUD_4M;
};

#endif //__USART_H__
