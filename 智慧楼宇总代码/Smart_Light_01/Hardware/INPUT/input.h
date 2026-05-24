#ifndef __INPUT_H__
#define __INPUT_H__
#include "main.h"

extern uint8_t input1_state;
extern uint8_t input2_state;
extern uint8_t input3_state;
extern uint8_t input4_state;
extern uint8_t input5_state;
extern uint8_t input6_state;


/* 输入状态读取函数声明 */
uint8_t Read_Input_01(void);
uint8_t Read_Input_02(void);
uint8_t Read_Input_03(void);
uint8_t Read_Input_04(void);
uint8_t Read_Input_05(void);
uint8_t Read_Input_06(void);
 
uint8_t CheckInputs(void);
#endif

