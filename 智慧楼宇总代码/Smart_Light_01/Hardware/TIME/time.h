#ifndef __TIME_H__
#define __TIME_H__
//状态机配置
#include "main.h"

 
// 定时器计数器，用于1ms计时
extern volatile uint32_t timer_counter; 

// 定时器控制标志：0-未使用，1-正在计时
extern volatile uint8_t timer_running_flag;

 
 

#endif

