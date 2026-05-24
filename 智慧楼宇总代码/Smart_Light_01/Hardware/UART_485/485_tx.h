#ifndef __485_TX_H__
#define __485_TX_H__

#include "main.h"

// 帧头帧尾定义
#define FRAME_HEADER_1     0xEE
#define FRAME_HEADER_2     0x02
#define FRAME_TAIL         0xEF
// 定义从机地址
#define MAIN_ADDR    0x01//主机地址
#define SLAVE_ADDR_1 0x02//人体感应从机1
#define SLAVE_ADDR_2 0x03//人体感应从机2
#define SLAVE_ADDR_3 0x04//控继电器从机1
#define SLAVE_ADDR_4 0x05//控继电器从机2
#define SLAVE_ADDR_5 0x06//控继电器从机3
#define SLAVE_ADDR_6 0x07//控继电器从机4
#define SLAVE_ADDR_7 0x08//控继电器从机5
 
#define response_timeout 2000// 等待响应的超时时间，单位ms  

extern volatile uint8_t rx_data_ready;    //接收状态位 1--成功接收一帧数据
extern volatile uint8_t rx_data_fuction;  //接收的功能位
extern volatile uint8_t rx_data_addr;     //接收的从机地址(就是那个从机返回的数据就是那个从机地址)
//接收的三个变量
extern volatile uint8_t rx_data_01;    // 数据1
extern volatile uint8_t rx_data_02;    // 数据2 
extern volatile uint8_t rx_data_03;    // 数据3
unsigned short CalculateCRC16(const unsigned char *data, unsigned int length);//CRC校验函数
void ProcessModbusData(uint8_t addr, uint8_t fuction, uint8_t data1, uint8_t data2, uint8_t data3);//接收数据分析函数
void LUN_XUN(void);//485轮询函数
void Master_SendData(uint8_t Address,uint8_t Fuction);//主机发送数据帧函数
void RS485_Init(void);//485初始化函数
#endif

