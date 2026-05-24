#include "input.h"
#include "main.h"
#include "gpio.h"
#include "usart.h"
#include <math.h>
#include "stdio.h"
 
//定义变量来接收输入引脚的值，初始是高电平,也就是输入端没有信号
GPIO_PinState input1_state =GPIO_PIN_SET ;
GPIO_PinState input2_state =GPIO_PIN_SET ;
GPIO_PinState input3_state =GPIO_PIN_SET ;
GPIO_PinState input4_state =GPIO_PIN_SET ;
GPIO_PinState input5_state =GPIO_PIN_SET ;
GPIO_PinState input6_state =GPIO_PIN_SET ;
GPIO_PinState input7_state =GPIO_PIN_SET ;
GPIO_PinState input8_state =GPIO_PIN_SET ;
GPIO_PinState input9_state =GPIO_PIN_SET ;
GPIO_PinState input10_state =GPIO_PIN_SET;

/* 在主循环或其他函数中使用 */
uint8_t CheckInputs(void)//读取光敏传感器状态
{
	//这里默认为输入端为0V时是有信号
	//此时PB系列引脚为低电平
    input1_state = (HAL_GPIO_ReadPin(Input_01_GPIO_Port, Input_01_Pin));
    input2_state = (HAL_GPIO_ReadPin(Input_02_GPIO_Port, Input_02_Pin));
    input3_state = (HAL_GPIO_ReadPin(Input_03_GPIO_Port, Input_03_Pin));
    input4_state = (HAL_GPIO_ReadPin(Input_04_GPIO_Port, Input_04_Pin));
    input5_state = (HAL_GPIO_ReadPin(Input_05_GPIO_Port, Input_05_Pin));
    input6_state = (HAL_GPIO_ReadPin(Input_06_GPIO_Port, Input_06_Pin));
    input7_state = (HAL_GPIO_ReadPin(Input_07_GPIO_Port, Input_07_Pin));
	input8_state = (HAL_GPIO_ReadPin(Input_08_GPIO_Port, Input_08_Pin));
	input9_state = (HAL_GPIO_ReadPin(Input_09_GPIO_Port, Input_09_Pin));
	input10_state = (HAL_GPIO_ReadPin(Input_10_GPIO_Port, Input_10_Pin));
    if((input1_state == GPIO_PIN_RESET) || (input2_state == GPIO_PIN_RESET) || (input3_state == GPIO_PIN_RESET) 
		|| (input4_state == GPIO_PIN_RESET)|| (input5_state == GPIO_PIN_RESET)
		  || (input6_state == GPIO_PIN_RESET)|| (input7_state == GPIO_PIN_RESET)
	    || (input8_state == GPIO_PIN_RESET)|| (input9_state == GPIO_PIN_RESET)|| (input10_state == GPIO_PIN_RESET))
	{
		 return 1;//有信号
	}
	else if((input1_state == GPIO_PIN_SET) || (input2_state == GPIO_PIN_SET) || (input3_state == GPIO_PIN_SET) 
		|| (input4_state == GPIO_PIN_SET)|| (input5_state == GPIO_PIN_SET)
		  || (input6_state == GPIO_PIN_SET)|| (input7_state == GPIO_PIN_SET)
	    || (input8_state == GPIO_PIN_SET)|| (input9_state == GPIO_PIN_SET)|| (input10_state == GPIO_PIN_SET))
	{
		return 0;//没信号
	}
	return 0;//没有输入信号就返回没信号
}




