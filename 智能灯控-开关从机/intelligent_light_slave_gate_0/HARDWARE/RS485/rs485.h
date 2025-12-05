#ifndef __rs485_H
#define __rs485_H

#include "main.h"
#include "gpio.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>

// 帧头帧尾定义
#define FRAME_HEADER_1     0xEE
#define FRAME_HEADER_2     0x02
#define FRAME_TAIL         0xEF

// 功能位定义
#define FUNC_GET_PARAM     0x03  // 参数读取
#define FUNC_RUN_LIGHT     0x04  //执行主机开灯命令

// 帧长度
#define FRAME_LENGTH       32

// 从机地址
#define SLAVE_ADDRESS      0x04

// 发送相关变量
extern unsigned char uart_tx_buffer[FRAME_LENGTH];
extern unsigned char uart_tx_busy;
extern unsigned char TxSend_Flag;  // 1s发送计时

// 从机数据变量
extern unsigned char Sensor_1;
extern unsigned char Sensor_2;
extern unsigned char Sensor_3;
extern unsigned char Sensor_4;
extern unsigned char Sensor_5;
extern unsigned char Sensor_6;
extern unsigned char Sensor_7;
extern unsigned char Sensor_8;
extern unsigned char Sensor_9;
extern unsigned char Sensor_10;


// 函数声明
void Slave_Init(void);
void Slave_Process_Command(unsigned char *rx_data, unsigned char len);
void Slave_Send_Response(unsigned char function, unsigned char *data, unsigned char data_len);
void Slave_Send_Data(void);
unsigned short CalculateCRC16(const unsigned char *data, unsigned int length);
void RS485_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);


// 485控制宏
#define RS485_TX_MODE()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET)
#define RS485_RX_MODE()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET)

#endif
