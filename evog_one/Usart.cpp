/* 
* Usart.cpp
*/

#include "Usart.h"

//=================//
//=== CONSTANTS ===//
//=================//

const Usart::baudrate_s Usart::BAUD_2k4   = {12 , 6,false};
const Usart::baudrate_s Usart::BAUD_4k8   = {12 , 5,false};
const Usart::baudrate_s Usart::BAUD_9k6   = {12 , 4,false};
const Usart::baudrate_s Usart::BAUD_14k4  = {138, 0,false};
const Usart::baudrate_s Usart::BAUD_19k2  = {12 , 3,false};
const Usart::baudrate_s Usart::BAUD_28k8  = {137,-1,false};
const Usart::baudrate_s Usart::BAUD_38k4  = {12 , 2,false};
const Usart::baudrate_s Usart::BAUD_57k6  = {135,-2,false};
const Usart::baudrate_s Usart::BAUD_76k8  = {12 , 1,false};
const Usart::baudrate_s Usart::BAUD_115k2 = {131,-3,false};
const Usart::baudrate_s Usart::BAUD_230k4 = {123,-4,false};
const Usart::baudrate_s Usart::BAUD_460k8 = {107,-5,false};
const Usart::baudrate_s Usart::BAUD_921k6 = {75 ,-6,false};
const Usart::baudrate_s Usart::BAUD_1M382 = {57 ,-7,false};
const Usart::baudrate_s Usart::BAUD_1M843 = {11 ,-7,false};
const Usart::baudrate_s Usart::BAUD_2M    = {0  , 0,false};
const Usart::baudrate_s Usart::BAUD_2M304 = {47 ,-6,true};
const Usart::baudrate_s Usart::BAUD_2M5   = {77 ,-7,true};
const Usart::baudrate_s Usart::BAUD_3M    = {43 ,-7,true};
const Usart::baudrate_s Usart::BAUD_4M    = {0  , 0,true};
