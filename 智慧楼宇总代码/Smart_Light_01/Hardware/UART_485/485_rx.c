#include "485_rx.h"
#include "main.h"
#include "gpio.h"
#include "usart.h"
#include "stdio.h"
#include "4g.h"


uint8_t Rx_byte485;//串口接收字节变量 

/////此处注意在主函数要加上第一次接收代码
// UART3接收完成回调函数（每次接收1字节）
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) 
{
	static uint8_t rx_buffer485[15];    // 接收缓冲区
	static uint8_t rx_len485 = 0;       // 缓冲区索引
	static uint8_t rx_state485 = 0;      // 接收状态
    if (huart->Instance == USART3) 
	{
        
        // 状态0：检查起始位
        if(rx_state485 == 0)
        {
            if(Rx_byte485 == FRAME_HEADER_1)   
            {
                rx_buffer485[rx_len485++] = Rx_byte485;				
                rx_state485 = 1;
            }			
        }
        // 状态1：检查起始位
        else if(rx_state485 == 1)
        {
            if(Rx_byte485 == FRAME_HEADER_2)   
            {
                rx_buffer485[rx_len485++] = Rx_byte485;
                rx_state485 = 2;
            }
            else  //地址二不对
            {
				rx_state485 = 0;
				rx_len485 = 0;  //重置索引 
            }
        }
		// 状态2：读取从机地址
        else if(rx_state485 == 2)
        {	 
			rx_buffer485[rx_len485++] = Rx_byte485;//  rx_buffer485[2]是接收到的从机地址
			rx_state485 = 3;			 
        }
		// 状态3：接收功能码
        else if(rx_state485 == 3)
        {
			rx_buffer485[rx_len485++] = Rx_byte485;//  rx_buffer485[3]是接收到的功能位
			rx_state485 = 4;			
        }
		// 状态4：接收数据长度位
        else if(rx_state485 == 4)
        {
			if(Rx_byte485  == 0x03)//固定数据长度位为0x03
			{
				rx_buffer485[rx_len485++] = Rx_byte485;//   
				rx_state485 = 5;	
			}		
			else 
			{
				rx_state485 = 0;	
				rx_len485 = 0;  //重置索引 
			}
        }
		// 状态5：接收数据1
        else if(rx_state485 == 5)
        {
			rx_buffer485[rx_len485++] = Rx_byte485;//   rx_buffer[5]是接收到的数据1
			if(rx_len485 == 8)						 //   rx_buffer[6]是接收到的数据2
			{											 //   rx_buffer[7]是接收到的数据3
				rx_state485 = 6;
			}				
						
        }
		// 状态6：验证CRC
        else if(rx_state485 == 6)
        {
			rx_buffer485[rx_len485++] = Rx_byte485;
			if(rx_len485 == 10)						 //   rx_buffer[6]是接收到的数据2
			{	
				unsigned short crc = CalculateCRC16(rx_buffer485, 8); 
				if((rx_buffer485[8]==((crc >> 8) & 0xFF)) && (rx_buffer485[9]==(crc & 0xFF)))//如果CRC验证成功
				{ 
					rx_state485 = 7; 
				}
				else
				{
					rx_state485 = 0;  	
					rx_len485 = 0;  //重置索引 
				}
			}			
        }
		// 状态7：验证终止位
        else if(rx_state485 == 7)
        {
			if(Rx_byte485 == FRAME_TAIL)//终止位正确
			{
				rx_buffer485[rx_len485++] = Rx_byte485;
				rx_data_ready = 1;//接收状态位变量置一  
				rx_data_addr = rx_buffer485[2];//接收的从机地址
				rx_data_fuction = rx_buffer485[3];//接收的功能码
				rx_data_01 = rx_buffer485[5];
				rx_data_02 = rx_buffer485[6];
				rx_data_03 = rx_buffer485[7];
			}
			rx_state485 = 0;  	
			rx_len485 = 0;  //重置索引
        }
    	
        HAL_UART_Receive_IT(&huart3,&Rx_byte485, 1);//继续接收字节
    }
	if(huart->Instance == USART1)
    { 
		// 静态局部变量 - 只在中断函数内使用
		static uint8_t rx_buffer[8];      // 接收缓冲区
		static uint8_t buffer_index = 0;  // 缓冲区索引
		static uint8_t rx1_state = 0;      // 接收状态
		 
        // 状态0：检查地址
        if(rx1_state == 0)
        {
            if(g_rx_byte == 0x01)  //设备地址匹配
            {
                rx_buffer[buffer_index++] = g_rx_byte;				
                rx1_state = 1;
            }			
        }
        // 状态1：检查功能码
        else if(rx1_state == 1)
        {
            if(g_rx_byte == 0x06)  //功能码匹配
            {
                rx_buffer[buffer_index++] = g_rx_byte;
                rx1_state = 2;
            }
            else //功能码不匹配
            {
				rx1_state = 0;
				buffer_index = 0;  //重置索引 
            }
        }
		// 状态2：接收寄存器地址
        else if(rx1_state == 2)
        {
			rx_buffer[buffer_index++] = g_rx_byte;
			if(buffer_index == 4)
			{
				g_led_group = rx_buffer[3];//寄存器地址
				rx1_state = 3;
			}             
        }
		// 状态3：接收设置码
        else if(rx1_state == 3)
        {
			rx_buffer[buffer_index++] = g_rx_byte;
			if(buffer_index == 6)
			{
				g_led_state = rx_buffer[5];//从设备需要设置的值 0/1
				rx1_state = 4;
			}           
        }
		// 状态4：接收CRC校验
        else if(rx1_state == 4)
        {
			rx_buffer[buffer_index++] = g_rx_byte;
			if(buffer_index == 8)//数据帧接收完毕
			{
				unsigned short crc = CRC16_MODBUS_Full_4G(rx_buffer, 6);//判断数据帧是否正确
				if((rx_buffer[6]==(crc & 0xFF)) && (rx_buffer[7]==((crc >> 8) & 0xFF)))
				{
					g_cmd_received = 1;//串口接收到了一个完整正确的数据帧
					rx1_state = 0;
					buffer_index = 0;  //重置索引 
				}
				else
				{
					g_cmd_received = 0;//串口接收到了一个完整但是错误的数据帧
					rx1_state = 0;
					buffer_index = 0;  //重置索引 					
				}
			}            
        }
        // 重新启动接收（使用同一个全局变量）
        HAL_UART_Receive_IT(&huart1, &g_rx_byte, 1);
    }
} 





