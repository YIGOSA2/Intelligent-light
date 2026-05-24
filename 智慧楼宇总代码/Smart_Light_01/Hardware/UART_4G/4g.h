#ifndef __4G_H__
#define __4G_H__
#include "main.h"
extern uint8_t g_rx_byte;//串口接收字节数据变量
extern volatile uint8_t g_cmd_received;//串口接收数据帧标志位
extern volatile uint8_t g_led_group;//LED组变量
extern volatile uint8_t g_led_state;//LED组新状态变量

//num:1-49第几个灯   value:0-1 0熄灭 1点亮
HAL_StatusTypeDef Control_OneLed(uint16_t num, uint8_t value);//将网页上某一个灯点亮或熄灭

//num:1-6第几组灯   value:0-1 0熄灭 1点亮
HAL_StatusTypeDef Control_GroupLed(uint16_t num, uint8_t value);//将网页上某一组灯点亮或熄灭
uint16_t CRC16_MODBUS_Full_4G(uint8_t *data, uint16_t length);//4G专属CRC

/*此函数功能
一. 会自动接收并回复有人云发送下来的开灯指令
二. 返回有人云发送下来的指令
*/
//uint8_t UART_RC_LED(void);




#endif

