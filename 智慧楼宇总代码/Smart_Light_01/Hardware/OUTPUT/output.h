#ifndef __OUTPUT_H__
#define __OUTPUT_H__
#include "main.h"

//void Relay_Light(uint8_t state);
//void Relay_Switch(uint8_t state);
void Relay_KEY(uint8_t state);//1--机械开关失效 0--机械开关有效

void Relay_Light(uint8_t state);//1--开灯 0--关灯
#endif

