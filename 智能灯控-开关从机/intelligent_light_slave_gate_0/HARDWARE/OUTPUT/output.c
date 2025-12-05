#include  "output.h"

void output_Init(void)
{
HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
	
}
void Run_Light(void) //控制灯亮灭
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_14,GPIO_PIN_SET);
}

//开关控制

