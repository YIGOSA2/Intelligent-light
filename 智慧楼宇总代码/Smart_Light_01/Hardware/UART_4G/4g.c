#include "4g.h"
#include "main.h"
#include "gpio.h"
#include "usart.h"
#include "stdio.h"

 

uint8_t g_rx_byte;//串口接收字节数据变量
volatile uint8_t g_cmd_received = 0;//串口接收数据帧标志位
volatile uint8_t g_led_group = 0;//LED组变量
volatile uint8_t g_led_state = 0;//LED组新状态变量

uint16_t CRC16_MODBUS_Full_4G(uint8_t *data, uint16_t length)//4G_CRC
{
    uint16_t crc = 0xFFFF;  // 初始值：0xFFFF
    uint16_t i, j;
    uint8_t byte_reversed;
    
    for (i = 0; i < length; i++)
    {
        // 输入反转：逐位反转当前字节
        byte_reversed = 0;
        for (j = 0; j < 8; j++)
        {
            if (data[i] & (1 << j))  // 检查原字节的第j位（低位到高位）
            {
                byte_reversed |= (1 << (7 - j));  // 反转到第7-j位（高位到低位）
            }
        }
        
        // CRC核心计算（多项式0x8005）
        crc ^= (byte_reversed << 8);
        for (j = 0; j < 8; j++)
        {
            if (crc & 0x8000)  // 检查CRC最高位（第15位）
            {
                crc = (crc << 1) ^ 0x8005;  // 多项式：0x8005
            }
            else
            {
                crc = crc << 1;
            }
        }
        crc &= 0xFFFF;  // 保持CRC为16位，避免溢出（关键步骤）
    }
    
    // 输出反转：逐位反转16位CRC结果
    uint16_t crc_reversed = 0;
    for (i = 0; i < 16; i++)
    {
        if (crc & (1 << i))  // 检查原CRC的第i位（低位到高位）
        {
            crc_reversed |= (1 << (15 - i));  // 反转到第15-i位（高位到低位）
        }
    }
    
    // 结果异或0x0000（无操作，直接返回）
    return crc_reversed;
}


uint8_t Single_Frame[11] = {
        0x01,                  // 从机地址
        0x46,                  // 功能码（固定0x46）
        0x00, 0x00,            // 起始地址（动态设置）
        0x00, 0x01,            // 寄存器数量（固定1个）
        0x02,                  // 数据字节数（1个变量×2=2字节）
        0x00, 0x00,            // 变量值（动态设置）
        0x00, 0x00             // CRC（动态计算）
	};

//将网页上某一个灯点亮或熄灭
HAL_StatusTypeDef Control_OneLed(uint16_t num, uint8_t value) //num:0-49选择灯  value：0-1 0熄灭 1点亮
{
	uint16_t ADDR;
	switch(num)
	{
		case 1: ADDR = 0; break;
		case 2: ADDR = 10; break;	
		case 3: ADDR = 16; break;
		case 4: ADDR = 1; break;
		case 5: ADDR = 11; break;
		case 6: ADDR = 17; break;
		case 7: ADDR = 2; break;
		case 8: ADDR = 12; break;
		case 9: ADDR = 18; break;
		case 10: ADDR = 3; break;
		case 11: ADDR = 13; break;
		case 12: ADDR = 19; break;
		case 13: ADDR = 4; break;
		case 14: ADDR = 14; break;
		case 15: ADDR = 20; break;
		case 16: ADDR = 5; break;
		case 17: ADDR = 15; break;
		case 18: ADDR = 21; break;
		case 19: ADDR = 6; break;
		case 20: ADDR = 7; break;
		case 21: ADDR = 22; break;
		case 22: ADDR = 8; break;
		case 23: ADDR = 23; break;
		case 24: ADDR = 24; break;
		case 25: ADDR = 9; break;
		case 26: ADDR = 28; break;
		case 27: ADDR = 29; break;
		case 28: ADDR = 30; break;
		case 29: ADDR = 31; break;
		case 30: ADDR = 32; break;
		case 31: ADDR = 33; break;
		case 32: ADDR = 34; break;
		case 33: ADDR = 35; break;
		case 34: ADDR = 36; break;
		case 35: ADDR = 37; break;
		case 36: ADDR = 38; break;
		case 37: ADDR = 39; break;
		case 38: ADDR = 40; break;
		case 39: ADDR = 41; break;
		case 40: ADDR = 42; break;
		case 41: ADDR = 43; break;
		case 42: ADDR = 44; break;
		case 43: ADDR = 45; break;
		case 44: ADDR = 46; break;
		case 45: ADDR = 47; break;
		case 46: ADDR = 48; break;
		case 47: ADDR = 49; break;
		case 48: ADDR = 50; break;
		case 49: ADDR = 51; break;

	}
	// 设置起始地址 
	 
	Single_Frame[2] = (ADDR >> 8) & 0xFF;  // 地址高8位
	Single_Frame[3] = ADDR & 0xFF;         // 地址低8位
	
	// 设置变量值（0→0x0000，1→0x0001）	
    Single_Frame[8] = value;  // 低8位（高8位Single_Frame[7]固定为0）
	
	// 计算前9字节的CRC，填充到帧的最后2字节
    unsigned short crc = CRC16_MODBUS_Full_4G(Single_Frame, 9);
    Single_Frame[9] = crc & 0xFF;     // CRC低字节
    Single_Frame[10] = (crc >> 8) & 0xFF;  // CRC高字节
	// 阻塞发送（确保完成）
    return HAL_UART_Transmit(&huart1, Single_Frame, 11, HAL_MAX_DELAY);
	
	//此函数有返回值可验证是否发送成功，下面是验证代码例程
	/*
		if (ret == HAL_OK) {
		// 发送成功，执行后续逻辑（如记录日志）
	} else if (ret == HAL_TIMEOUT) {
		// 发送超时，可重试或报错
	} else {
		// 其他错误，检查硬件或参数
	}
	*/
}


//将网页上某组灯点亮或熄灭,此处是阻塞式发送，每次将一组数据的值改变时，最多需要等待2s，少的则为1.2s
HAL_StatusTypeDef Control_GroupLed(uint16_t num,uint8_t value) //num:1-6哪一组  value：0-1 0熄灭 1点亮
{ 
	switch(num)
	{
        case 1: // 第1组
             {
                uint8_t tx_data[30];
                uint16_t tx_size = 0;
				tx_data[tx_size++] = 0x01;       // 设备地址
				tx_data[tx_size++] = 0x46;       // 功能码
				tx_data[tx_size++] = 0x00;       // 起始地址高字节
				tx_data[tx_size++] = 0x00;       // 起始地址低字节
				tx_data[tx_size++] = 0x00;       // 寄存器单元长度高字节
				tx_data[tx_size++] = 0x09;       // 寄存器单元长度低字节 (9个灯)
				tx_data[tx_size++] = 0x12;       // 数据长度 (18字节)
				
				//9个灯的状态，每个灯用2字节表示
				for(int i = 0; i < 9; i++)
				{
					tx_data[tx_size++] = 0x00;// 数据高字节
					tx_data[tx_size++] = value ? 0x01 : 0x00;//数据低字节
				}
				 // 计算CRC
				uint16_t crc = CRC16_MODBUS_Full_4G(tx_data, tx_size);
				
				// 添加CRC到数据帧
				tx_data[tx_size++] = crc & 0xFF;      // CRC低字节
				tx_data[tx_size++] = (crc >> 8) & 0xFF; // CRC高字节
				
				// 通过串口发送数据
				return HAL_UART_Transmit(&huart1, tx_data, tx_size, HAL_MAX_DELAY);
			}	
		
        case 2: // 第2组LED
			{           
				uint8_t tx_data[30];
                uint16_t tx_size = 0;
				tx_data[tx_size++] = 0x01;       // 设备地址
				tx_data[tx_size++] = 0x46;       // 功能码
				tx_data[tx_size++] = 0x00;       // 起始地址高字节
				tx_data[tx_size++] = 0x09;       // 起始地址低字节
				tx_data[tx_size++] = 0x00;       // 寄存器单元长度高字节
				tx_data[tx_size++] = 0x06;       // 寄存器单元长度低字节 (6个灯)
				tx_data[tx_size++] = 0x0C;       // 数据长度 (12字节)
				
				//6个灯的状态，每个灯用2字节表示
				for(int i = 0; i < 6; i++)
				{
					tx_data[tx_size++] = 0x00;// 数据高字节
					tx_data[tx_size++] = value ? 0x01 : 0x00;//数据低字节
				}
				 // 计算CRC
				uint16_t crc = CRC16_MODBUS_Full_4G(tx_data, tx_size);
				
				// 添加CRC到数据帧
				tx_data[tx_size++] = crc & 0xFF;      // CRC低字节
				tx_data[tx_size++] = (crc >> 8) & 0xFF; // CRC高字节
				
				// 通过串口发送数据
				return HAL_UART_Transmit(&huart1, tx_data, tx_size, HAL_MAX_DELAY);
			}   
        case 3: // 第3组 
			{        
				uint8_t tx_data[30];
                uint16_t tx_size = 0;
				tx_data[tx_size++] = 0x01;       // 设备地址
				tx_data[tx_size++] = 0x46;       // 功能码
				tx_data[tx_size++] = 0x00;       // 起始地址高字节
				tx_data[tx_size++] = 0x0F;       // 起始地址低字节
				tx_data[tx_size++] = 0x00;       // 寄存器单元长度高字节
				tx_data[tx_size++] = 0x0A;       // 寄存器单元长度低字节 (10个灯)
				tx_data[tx_size++] = 0x14;       // 数据长度 (20字节)
				
				//10个灯的状态，每个灯用2字节表示
				for(int i = 0; i < 10; i++)
				{
					tx_data[tx_size++] = 0x00;// 数据高字节
					tx_data[tx_size++] = value ? 0x01 : 0x00;//数据低字节
				}
				 // 计算CRC
				uint16_t crc = CRC16_MODBUS_Full_4G(tx_data, tx_size);
				
				// 添加CRC到数据帧
				tx_data[tx_size++] = crc & 0xFF;      // CRC低字节
				tx_data[tx_size++] = (crc >> 8) & 0xFF; // CRC高字节
				
				// 通过串口发送数据
				return HAL_UART_Transmit(&huart1, tx_data, tx_size, HAL_MAX_DELAY);
			}  
        case 4: // 第4组 
			{
				uint8_t tx_data[30];
                uint16_t tx_size = 0;
				tx_data[tx_size++] = 0x01;       // 设备地址
				tx_data[tx_size++] = 0x46;       // 功能码
				tx_data[tx_size++] = 0x00;       // 起始地址高字节
				tx_data[tx_size++] = 0x1C;       // 起始地址低字节
				tx_data[tx_size++] = 0x00;       // 寄存器单元长度高字节
				tx_data[tx_size++] = 0x08;       // 寄存器单元长度低字节 (8个灯)
				tx_data[tx_size++] = 0x10;       // 数据长度 (16字节)
				
				//8个灯的状态，每个灯用2字节表示
				for(int i = 0; i < 8; i++)
				{
					tx_data[tx_size++] = 0x00;// 数据高字节
					tx_data[tx_size++] = value ? 0x01 : 0x00;//数据低字节
				}
				 // 计算CRC
				uint16_t crc = CRC16_MODBUS_Full_4G(tx_data, tx_size);
				
				// 添加CRC到数据帧
				tx_data[tx_size++] = crc & 0xFF;      // CRC低字节
				tx_data[tx_size++] = (crc >> 8) & 0xFF; // CRC高字节
				
				// 通过串口发送数据
				return HAL_UART_Transmit(&huart1, tx_data, tx_size, HAL_MAX_DELAY);
            }
		case 5: // 第4组 
			{
				uint8_t tx_data[30];
                uint16_t tx_size = 0;
				tx_data[tx_size++] = 0x01;       // 设备地址
				tx_data[tx_size++] = 0x46;       // 功能码
				tx_data[tx_size++] = 0x00;       // 起始地址高字节
				tx_data[tx_size++] = 0x24;       // 起始地址低字节
				tx_data[tx_size++] = 0x00;       // 寄存器单元长度高字节
				tx_data[tx_size++] = 0x08;       // 寄存器单元长度低字节 (8个灯)
				tx_data[tx_size++] = 0x10;       // 数据长度 (16字节)
				
				//8个灯的状态，每个灯用2字节表示
				for(int i = 0; i < 8; i++)
				{
					tx_data[tx_size++] = 0x00;// 数据高字节
					tx_data[tx_size++] = value ? 0x01 : 0x00;//数据低字节
				}
				 // 计算CRC
				uint16_t crc = CRC16_MODBUS_Full_4G(tx_data, tx_size);
				
				// 添加CRC到数据帧
				tx_data[tx_size++] = crc & 0xFF;      // CRC低字节
				tx_data[tx_size++] = (crc >> 8) & 0xFF; // CRC高字节
				
				// 通过串口发送数据
				return HAL_UART_Transmit(&huart1, tx_data, tx_size, HAL_MAX_DELAY);
            }
		case 6: // 第6组 
			{
				uint8_t tx_data[30];
                uint16_t tx_size = 0;
				tx_data[tx_size++] = 0x01;       // 设备地址
				tx_data[tx_size++] = 0x46;       // 功能码
				tx_data[tx_size++] = 0x00;       // 起始地址高字节
				tx_data[tx_size++] = 0x2C;       // 起始地址低字节
				tx_data[tx_size++] = 0x00;       // 寄存器单元长度高字节
				tx_data[tx_size++] = 0x08;       // 寄存器单元长度低字节 (8个灯)
				tx_data[tx_size++] = 0x12;       // 数据长度 (16字节)
				
				//8个灯的状态，每个灯用2字节表示
				for(int i = 0; i < 8; i++)
				{
					tx_data[tx_size++] = 0x00;// 数据高字节
					tx_data[tx_size++] = value ? 0x01 : 0x00;//数据低字节
				}
				 // 计算CRC
				uint16_t crc = CRC16_MODBUS_Full_4G(tx_data, tx_size);
				
				// 添加CRC到数据帧
				tx_data[tx_size++] = crc & 0xFF;      // CRC低字节
				tx_data[tx_size++] = (crc >> 8) & 0xFF; // CRC高字节
				
				// 通过串口发送数据
				return HAL_UART_Transmit(&huart1, tx_data, tx_size, HAL_MAX_DELAY);
            }
				 
			
        default:
            return HAL_ERROR; // 无效的组号
		
    }    
}
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)//串口中断回调函数
//{	 
//	if(huart->Instance == USART1)
//    { 
//		// 静态局部变量 - 只在中断函数内使用
//		static uint8_t rx_buffer[8];      // 接收缓冲区
//		static uint8_t buffer_index = 0;  // 缓冲区索引
//		static uint8_t rx1_state = 0;      // 接收状态
//		 
//        // 状态0：检查地址
//        if(rx1_state == 0)
//        {
//            if(g_rx_byte == 0x01)  //设备地址匹配
//            {
//                rx_buffer[buffer_index++] = g_rx_byte;				
//                rx1_state = 1;
//            }			
//        }
//        // 状态1：检查功能码
//        else if(rx1_state == 1)
//        {
//            if(g_rx_byte == 0x06)  //功能码匹配
//            {
//                rx_buffer[buffer_index++] = g_rx_byte;
//                rx1_state = 2;
//            }
//            else //功能码不匹配
//            {
//				rx1_state = 0;
//				buffer_index = 0;  //重置索引 
//            }
//        }
//		// 状态2：接收寄存器地址
//        else if(rx1_state == 2)
//        {
//			rx_buffer[buffer_index++] = g_rx_byte;
//			if(buffer_index == 4)
//			{
//				g_led_group = rx_buffer[3];//寄存器地址
//				rx1_state = 3;
//			}             
//        }
//		// 状态3：接收设置码
//        else if(rx1_state == 3)
//        {
//			rx_buffer[buffer_index++] = g_rx_byte;
//			if(buffer_index == 6)
//			{
//				g_led_state = rx_buffer[5];//从设备需要设置的值 0/1
//				rx1_state = 4;
//			}           
//        }
//		// 状态4：接收CRC校验
//        else if(rx1_state == 4)
//        {
//			rx_buffer[buffer_index++] = g_rx_byte;
//			if(buffer_index == 8)//数据帧接收完毕
//			{
//				unsigned short crc = CRC16_MODBUS_Full_4G(rx_buffer, 6);//判断数据帧是否正确
//				if((rx_buffer[6]==(crc & 0xFF)) && (rx_buffer[7]==((crc >> 8) & 0xFF)))
//				{
//					g_cmd_received = 1;//串口接收到了一个完整正确的数据帧
//					rx1_state = 0;
//					buffer_index = 0;  //重置索引 
//				}
//				else
//				{
//					g_cmd_received = 0;//串口接收到了一个完整但是错误的数据帧
//					rx1_state = 0;
//					buffer_index = 0;  //重置索引 					
//				}
//			}            
//        }
//        // 重新启动接收（使用同一个全局变量）
//        HAL_UART_Receive_IT(&huart1, &g_rx_byte, 1);
//    }
//}

/*此函数功能
一. 会自动接收并回复有人云发送下来的开灯指令
二. 返回有人云发送下来的指令
*/
//uint8_t UART_RC_LED(void)
//{
//	uint8_t result = 0;

//	return result;
//}
//	
	
	
	




