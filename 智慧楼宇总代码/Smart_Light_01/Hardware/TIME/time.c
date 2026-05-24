#include "tim.h"
#include "time.h"
#include "main.h"
#include "usart.h"
#include "flash.h"
#include "stdio.h"
#include "function.h"

// 定时器计数器，用于1ms计时
volatile uint32_t timer_counter = 0; 

// 定时器控制标志：0-未使用，1-正在计时
volatile uint8_t timer_running_flag = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) 
{
    //  
    if (htim->Instance == TIM3) //10ms进入一次
    {
		if (timer_running_flag == 1)
		{
			timer_counter++;
		
			// 检查是否达到了一分钟（60,000毫秒）
			if (timer_counter >= 3000)//定时半分钟
			{
				Relay_Light(0); // 关闭西边的灯
				
				// 任务完成后，停止定时器并重置计数器
				HAL_TIM_Base_Stop_IT(&htim3);  
				timer_running_flag = 0;       // 重置运行标志
				timer_counter = 0;           // 重置计数器
			}
		}
		
    } 
}



