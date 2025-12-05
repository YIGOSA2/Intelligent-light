#include "output.h"
#include "tim.h"

// 定时器计数器，用于1ms计时
volatile uint32_t timer_counter = 0; 

// 定时器控制标志：0-未使用，1-正在计时
volatile uint8_t timer_running_flag = 0;

void output_Init(void)	    //初始化灭灯和机械开关使能
{
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);
}

void Run_Light(void) 		    //控制灯常亮
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_14,GPIO_PIN_SET);
}

void SHUT_Light(void)		   //控制灯灭
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_14, GPIO_PIN_RESET);
}
//开关控制


void Switch_On(void)       //使能机械开关
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);
}

void Switch_Off(void)     //禁用机械开关
{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET);

}



void Sensor_Light(void)
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_14,GPIO_PIN_SET);
		timer_counter = 0;          // 重置计数器
							timer_running_flag = 1;     // 置位运行标志
							HAL_TIM_Base_Start_IT(&htim2); // 开启定时器中断

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) 
{
    //  
    if (htim->Instance == TIM2) //10ms进入一次
    {
		if (timer_running_flag == 1)
		{
			timer_counter++;
		
			// 检查是否达到了一分钟（60,000毫秒）
			if (timer_counter >= 3000)//定时半分钟
			{
	      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_14, GPIO_PIN_RESET);
				
				// 任务完成后，停止定时器并重置计数器
				HAL_TIM_Base_Stop_IT(&htim2);  
				timer_running_flag = 0;       // 重置运行标志
				timer_counter = 0;           // 重置计数器
			}
		}
		
    } 
}
