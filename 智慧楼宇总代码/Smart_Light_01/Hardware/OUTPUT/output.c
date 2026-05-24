#include "output.h"
#include "main.h"
#include "gpio.h"
#include "usart.h"
#include <math.h>
#include "stdio.h"
#include "function.h"
/**
  * @brief  开关继电器控制
  * @param  state: 1-高电平导通, 0-低电平断开
  * @retval None
  */

void Relay_KEY(uint8_t state)//1--机械开关失效 0--机械开关有效
{
    HAL_GPIO_WritePin(relay_01_GPIO_Port, relay_01_Pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
  * @brief  照明继电器控制
  * @param  state: 1-高电平导通, 0-低电平断开
  * @retval None
  */
void Relay_Light(uint8_t state)//1--开灯 0--关灯
{
    HAL_GPIO_WritePin(relay_02_GPIO_Port, relay_02_Pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
