#include "485_tx.h"
#include "485_rx.h"
#include "main.h"
#include "gpio.h"
#include "usart.h"
#include "stdio.h"
#include "output.h"
#include "tim.h"
#include "4g.h" 
#include "input.h"

volatile uint8_t rx_data_ready;    //接收状态位 1--成功接收一帧数据
volatile uint8_t rx_data_fuction;  //接收的功能位
volatile uint8_t rx_data_addr;     //接收的从机地址(那个从机返回的数据就是那个从机地址)
//接收的三个变量
volatile uint8_t rx_data_01;    // 数据1
volatile uint8_t rx_data_02;    // 数据2 
volatile uint8_t rx_data_03;    // 数据3

// 设置为发送模式
void RS485_SetTxMode(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
}

// 设置为接收模式  
void RS485_SetRxMode(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
}

// 485初始化
void RS485_Init(void)
{
    // 初始设置为接收模式
    RS485_SetRxMode();
	HAL_UART_Receive_IT(&huart3,&Rx_byte485, 1);//接收字节
}



uint32_t send_time = 0; // 记录最后一次发送数据的时间戳
uint8_t isResponseTimeout() //超时检测函数
{
    return (HAL_GetTick() - send_time) > response_timeout;
}

uint8_t current_slave_addr = SLAVE_ADDR_1; // 当前正在查询的从机地址
// 辅助函数：获取下一个要查询的从机地址
uint8_t get_next_slave_addr(uint8_t current_addr) 
{
    if (current_addr == SLAVE_ADDR_2) return MAIN_ADDR;
//	if (current_addr == SLAVE_ADDR_2) return SLAVE_ADDR_3;
//	if (current_addr == SLAVE_ADDR_3) return SLAVE_ADDR_4;
//	if (current_addr == SLAVE_ADDR_4) return SLAVE_ADDR_5;
//	if (current_addr == SLAVE_ADDR_5) return SLAVE_ADDR_6;
//	if (current_addr == SLAVE_ADDR_6) return SLAVE_ADDR_7;
//	if (current_addr == SLAVE_ADDR_7) return MAIN_ADDR;
    return SLAVE_ADDR_1; // 默认
}

//通信状态枚举定义
typedef enum 
{
    COMM_IDLE,         // 空闲状态
    COMM_RECEIVING,     // 接收数据中
    COMM_PROCESSING,    // 处理数据中
    COMM_SENDING        // 发送数据中
}comm_state_t;
 
comm_state_t current_state = COMM_IDLE;//定义一个枚举变量表示当前485的状态


// 功能码定义
typedef enum 
{
    READ_STATE  = 0x03,//轮询指令
    OPEN_LED    = 0x04,//感应到人操作指令(接收到后对应灯亮一分钟)
	YOU_Open    = 0x05,//云平台开灯接收到后灯一直开
	YOU_Close   = 0x06,//云平台关灯接收到后关灯(并不影响之后灯随之感应模块的打开)
	You_DISKEY  = 0x07,//云平台使机械开关失效
	You_ENKEY   = 0x08 //云平台使机械开关使能
	
} Modbus_Function;
 
Modbus_Function Send_Fuction = READ_STATE;//定义一个枚举变量为485发送函数提供功能码

unsigned short CalculateCRC16(const unsigned char *data, unsigned int length)//485CRC校验函数
{
    unsigned short crc = 0xFFFF;
    unsigned int i;
    
    for (i = 0; i < length; i++) {
        crc ^= (unsigned short)data[i] << 8;
        
        for (unsigned int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc = crc << 1;
            }
        }
    }
    return crc;
}

void Master_SendData(uint8_t Address,uint8_t Fuction)//主机485发送函数
{
	RS485_SetTxMode();//将485设置为发送模式
    HAL_Delay(1);	
	uint8_t  tx_buffer[8];  // 发送缓冲区
	// 构建Modbus查询帧
    tx_buffer[0] = FRAME_HEADER_1;   //  起始码1
    tx_buffer[1] = FRAME_HEADER_2;   //  起始码2
    tx_buffer[2] = Address;          //  从机地址
    tx_buffer[3] = Fuction;          //  功能码
	tx_buffer[4] = 0x03;             //  变量长度
	// 计算CRC16校验（校验前6字节）
    unsigned short crc = CalculateCRC16(tx_buffer, 5);
	//CRC大端在前
	tx_buffer[5] = (crc >> 8) & 0xFF;//CRC高位
    tx_buffer[6] = crc & 0xFF;       //CRC低位
   
	tx_buffer[7] = FRAME_TAIL;       //结束位
	HAL_UART_Transmit(&huart3, tx_buffer, 8, HAL_MAX_DELAY);//阻塞发送啥时候发送完成啥时候继续往下运行
//	HAL_UART_Transmit_IT(&huart3, tx_buffer, sizeof(tx_buffer)); //非阻塞式发送
    send_time = HAL_GetTick();

    RS485_SetRxMode();//485切换为接收函数
} 

void LUN_XUN(void)//状态机实现485轮询
{
	if(g_cmd_received == 1)//接收到了云平台指令
	{
		g_cmd_received = 0;
		switch(g_led_group)
		{
			case 25://一楼一组
			{
				if(g_led_state == 1)
				{
					uint8_t i[8]={0x01,0x06,0x00,0x19,0x00,0x01,0x99,0xCD};
					HAL_UART_Transmit(&huart1, i, 8, HAL_MAX_DELAY);
					HAL_Delay(100);
					Control_GroupLed(1,1);
					Relay_Light(1);
					
					 
				}
				else if(g_led_state == 0)
				{
					uint8_t i[8]={0x01,0x06,0x00,0x19,0x00,0x00,0x58,0x0D};
					HAL_UART_Transmit(&huart1, i, 8, HAL_MAX_DELAY);HAL_Delay(100);
					Control_GroupLed(1,0);
					Relay_Light(0);
				}
			break;
			}
			case 26://一楼二组
			{
				if(g_led_state == 1)
				{
					uint8_t i[8]={0x01,0x06,0x00,0x1a,0x00,0x01,0x69,0xCD};
					HAL_UART_Transmit(&huart1, i, 8, HAL_MAX_DELAY);HAL_Delay(100);
					Control_GroupLed(2,1);
					Master_SendData(SLAVE_ADDR_3,YOU_Open);//通知相关继电器  
				}
				else if(g_led_state == 0)
				{
					uint8_t i[8]={0x01,0x06,0x00,0x1a,0x00,0x00,0xa8,0x0d};
					HAL_UART_Transmit(&huart1, i, 8, HAL_MAX_DELAY);HAL_Delay(100);
					Control_GroupLed(2,0);
					Master_SendData(SLAVE_ADDR_3,YOU_Close);//通知相关继电器 
				}
			break;
			}
			case 27://一楼三组
			{
				
				if(g_led_state == 1)
				{
					uint8_t i[8]={0x01,0x06,0x00,0x1b,0x00,0x01,0x38,0x0d};
					HAL_UART_Transmit(&huart1, i, 8, HAL_MAX_DELAY);HAL_Delay(100);
					Control_GroupLed(3,1);
					Master_SendData(SLAVE_ADDR_4,YOU_Open);//通知相关继电器   
				}
				else if(g_led_state == 0)
				{
					uint8_t i[8]={0x01,0x06,0x00,0x1b,0x00,0x00,0xf9,0xcd};
					HAL_UART_Transmit(&huart1, i, 8, HAL_MAX_DELAY);HAL_Delay(100);
					Control_GroupLed(3,0);
					Master_SendData(SLAVE_ADDR_4,YOU_Close);//通知相关继电器  
				}
			break;
			}	
			case 53://二楼一组
			{
				
				if(g_led_state == 1)
				{
					uint8_t i[8]={0x01,0x06,0x00,0x35,0x00,0x01,0x58,0x04};
					HAL_UART_Transmit(&huart1, i, 8, HAL_MAX_DELAY);HAL_Delay(100);
					Control_GroupLed(4,1);
					Master_SendData(SLAVE_ADDR_5,YOU_Open);//通知相关继电器  
				}
				else if(g_led_state == 0)
				{
					uint8_t i[8]={0x01,0x06,0x00,0x35,0x00,0x00,0x99,0xc4};
					HAL_UART_Transmit(&huart1, i, 8, HAL_MAX_DELAY);HAL_Delay(100);
					Control_GroupLed(4,0);
					Master_SendData(SLAVE_ADDR_5,YOU_Close);//通知相关继电器 
				}
			break;
			}	
			case 54://二楼二组
			{
				
				if(g_led_state == 1)
				{
					uint8_t i[8]={0x01,0x06,0x00,0x36,0x00,0x01,0xa8,0x04};
					HAL_UART_Transmit(&huart1, i, 8, HAL_MAX_DELAY);HAL_Delay(100);
					Control_GroupLed(5,1);
					Master_SendData(SLAVE_ADDR_6,YOU_Open);//通知相关继电器  
				}
				else if(g_led_state == 0)
				{
					uint8_t i[8]={0x01,0x06,0x00,0x36,0x00,0x00,0x69,0xc4};
					HAL_UART_Transmit(&huart1, i, 8, HAL_MAX_DELAY);HAL_Delay(100);
					Control_GroupLed(5,0);
					Master_SendData(SLAVE_ADDR_6,YOU_Close);//通知相关继电器
				}
			break;
			}	
			case 55://二楼三组
			{
				
				if(g_led_state == 1)
				{
					uint8_t i[8]={0x01,0x06,0x00,0x37,0x00,0x01,0xf9,0xc4};
					HAL_UART_Transmit(&huart1, i, 8, HAL_MAX_DELAY);HAL_Delay(100);
					Control_GroupLed(6,1);
					Master_SendData(SLAVE_ADDR_7,YOU_Open);//通知相关继电器 
				}
				else if(g_led_state == 0)//有人云要关闭二楼第三组灯
				{
					uint8_t i[8]={0x01,0x06,0x00,0x37,0x00,0x00,0x38,0x04};
					HAL_UART_Transmit(&huart1, i, 8, HAL_MAX_DELAY);HAL_Delay(100);
					Control_GroupLed(6,0);
					Master_SendData(SLAVE_ADDR_7,YOU_Close);//通知相关继电器
				}
			break;
			}
			case 56://机械面板开关
			{
				
				if(g_led_state == 1)//使能机械开关
				{
					uint8_t i[8]={0x01,0x06,0x00,0x38,0x00,0x01,0xc9,0xc7};
					HAL_UART_Transmit(&huart1, i, 8, HAL_MAX_DELAY); 
					//机械开关打开，此时要连续给五个从机发送指令，从机不能占总线
					Relay_KEY(0);//机械开关有效
					Master_SendData(SLAVE_ADDR_3,You_ENKEY); 
					Master_SendData(SLAVE_ADDR_4,You_ENKEY);
					Master_SendData(SLAVE_ADDR_5,You_ENKEY);
					Master_SendData(SLAVE_ADDR_6,You_ENKEY);
					Master_SendData(SLAVE_ADDR_7,You_ENKEY);
					 
				}
				else if(g_led_state == 0)//关闭机械开关
				{
					
					uint8_t i[8]={0x01,0x06,0x00,0x38,0x00,0x00,0x08,0x97};
					HAL_UART_Transmit(&huart1, i, 8, HAL_MAX_DELAY); 
					Relay_KEY(1);//机械开关无效//此处1，0逻辑就这样没写反
					Master_SendData(SLAVE_ADDR_3,You_DISKEY); 
					Master_SendData(SLAVE_ADDR_4,You_DISKEY);
					Master_SendData(SLAVE_ADDR_5,You_DISKEY);
					Master_SendData(SLAVE_ADDR_6,You_DISKEY);
					Master_SendData(SLAVE_ADDR_7,You_DISKEY);
				}
			break;
			}				
			default:
			{
				 
			break;
			}
		}	
	}	
	
	else//没有接收到云平台指令，继续开始轮询
	{
		switch (current_state) 
		{
				case COMM_IDLE:// 空闲状态(继续轮询)
					 
					Master_SendData(current_slave_addr,READ_STATE);//发送函数			
					current_state = COMM_RECEIVING; // 切换状态为等待响应
					break;

				case COMM_RECEIVING:// 检查是否接收到数据
					
					if ((rx_data_ready == 1 && rx_data_addr == current_slave_addr) || current_slave_addr == MAIN_ADDR)
					{
						current_state = COMM_PROCESSING; //切换状态为处理数据
						rx_data_ready = 0; // 清除标志位
					}
					// 检查是否超时
					else if (isResponseTimeout())
					{
						// 超时处理，可以记录错误次数或直接跳过
						current_slave_addr = get_next_slave_addr(current_slave_addr); //切换到下一个从机
						 
						current_state = COMM_IDLE; // 回到空闲状态，准备下一次发送
					}
					break;

				case COMM_PROCESSING:
					// 处理数据
					ProcessModbusData(rx_data_addr, rx_data_fuction, rx_data_01, rx_data_02, rx_data_03);
					
					// 数据处理完毕，决定下一步
					// 这里可以根据需要，决定是继续轮询还是进入其他状态
					// 例如，如果ProcessModbusData触发了其他任务，可能需要短暂延迟
					// 这里我们简单处理，直接切换到下一个从机继续轮询
					current_slave_addr = get_next_slave_addr(current_slave_addr);
					current_state = COMM_IDLE;
					break;
				
				default:
					current_state = COMM_IDLE; // 异常处理
					break;
		}
	}
	
}
void ProcessModbusData(uint8_t addr, uint8_t fuction, uint8_t data1, uint8_t data2, uint8_t data3)//数据处理变量
{
	if(current_slave_addr == MAIN_ADDR)//自己的也查询一下灯亮没亮
	{
		uint8_t i = 2;
	    i =	CheckInputs();//读取光敏传感器状态
		if(i)//实际灯亮
		{
			Control_GroupLed(1,1);//网页上对应的灯 亮
			HAL_Delay(100);
		}
		else//实际灯灭
		{
			Control_GroupLed(1,0);//网页上对应的灯 灭
			HAL_Delay(100);
		}
	}
	
	else
	{
		switch(addr)
		{
			case 2: //从机二 ：一楼检测 
			{
				if(fuction == 0x03)//从机返回的功能码
				{
					uint8_t i = 4*data3 + 2*data2 + data1;
					switch(i)
					{
						case 0://没有信号，继续轮询
						{
							//没有信号，继续轮询
							current_slave_addr = get_next_slave_addr(current_slave_addr);
							current_state = COMM_IDLE;
						break;
						}
						case 1://西边传感器有信号
						{
							Control_GroupLed(1,1);//网页上对应的灯 亮
							HAL_Delay(100);
							Relay_Light(1);//主机自己打开西边的灯
							timer_counter = 0;          // 重置计数器
							timer_running_flag = 1;     // 置位运行标志
							HAL_TIM_Base_Start_IT(&htim3); // 开启定时器中断
							 
							
						break;
						}
						case 2://中间传感器有信号
						{
							Master_SendData(SLAVE_ADDR_3,OPEN_LED);//发送函数//从机接收到后自行亮灯1分钟
							 
							
						break;
						}
						case 4://东边传感器有信号
						{
							Master_SendData(SLAVE_ADDR_4,OPEN_LED);//发送函数//从机接收到后自行亮灯1分钟
							 
						break;
						}
						case 3://西边和中间传感器有信号
						{
							Control_GroupLed(1,1);//网页上对应的灯 亮
							HAL_Delay(100);
							Relay_Light(1);//主机自己打开西边的灯
							timer_counter = 0;          // 重置计数器
							timer_running_flag = 1;     // 置位运行标志
							HAL_TIM_Base_Start_IT(&htim3); // 开启定时器中断
							Master_SendData(SLAVE_ADDR_3,OPEN_LED);//发送函数
							 
						break;
						}
						case 5://西边和东边传感器有信号
						{
							Control_GroupLed(1,1);//网页上对应的灯 亮
							HAL_Delay(100);
							Relay_Light(1);//主机自己打开西边的灯
							timer_counter = 0;          // 重置计数器
							timer_running_flag = 1;     // 置位运行标志
							HAL_TIM_Base_Start_IT(&htim3); // 开启定时器中断
							Master_SendData(SLAVE_ADDR_4,OPEN_LED);//发送函数
							 
							 
						break;
						}
						case 6://中间和东边传感器有信号
						{
							Master_SendData(SLAVE_ADDR_3,OPEN_LED);//发送函数
							Master_SendData(SLAVE_ADDR_4,OPEN_LED);//发送函数
							 
						break;
						}
						case 7://三个传感器同时有有信号
						{
							Control_GroupLed(1,1);//网页上对应的灯 亮
							HAL_Delay(100);
							Relay_Light(1);//主机自己打开西边的灯
							timer_counter = 0;          // 重置计数器
							timer_running_flag = 1;     // 置位运行标志
							HAL_TIM_Base_Start_IT(&htim3); // 开启定时器中断
							Master_SendData(SLAVE_ADDR_3,OPEN_LED);//发送函数
							Master_SendData(SLAVE_ADDR_4,OPEN_LED);//发送函数
							 
						break;
						}
					}
				}		
			break;
			}
			case 3: //从机三 ：一楼检测 
			{
				if(fuction == 0x03)//从机返回的功能码
				{
					uint8_t i = 4*data3 + 2*data2 + data1;
					switch(i)
					{
						case 0://没有信号，继续轮询
						{
							current_slave_addr = get_next_slave_addr(current_slave_addr);
							current_state = COMM_IDLE;
						break;
						}
						case 1://西边传感器有信号
						{
							Master_SendData(SLAVE_ADDR_5,OPEN_LED);//发送函数//从机接收到后自行亮灯1分钟 
						break;
						}
						case 2://中间传感器有信号
						{
							Master_SendData(SLAVE_ADDR_6,OPEN_LED);//发送函数//从机接收到后自行亮灯1分钟
							 
							
						break;
						}
						case 4://东边传感器有信号
						{
							
							Master_SendData(SLAVE_ADDR_7,OPEN_LED);//发送函数//从机接收到后自行亮灯1分钟
						 
						break;
						}
						case 3://西边和中间传感器有信号
						{
							Master_SendData(SLAVE_ADDR_5,OPEN_LED);//发送函数//从机接收到后自行亮灯1分钟  
							Master_SendData(SLAVE_ADDR_6,OPEN_LED);//发送函数
							 
						break;
						}
						case 5://西边和东边传感器有信号
						{
							 
							Master_SendData(SLAVE_ADDR_5,OPEN_LED);//发送函数
							Master_SendData(SLAVE_ADDR_7,OPEN_LED);//发送函数
							 
							 
						break;
						}
						case 6://中间和东边传感器有信号
						{
							Master_SendData(SLAVE_ADDR_6,OPEN_LED);//发送函数
							Master_SendData(SLAVE_ADDR_7,OPEN_LED);//发送函数
						 
						break;
						}
						case 7://三个传感器同时有有信号
						{
							 
							 
							Master_SendData(SLAVE_ADDR_5,OPEN_LED);//发送函数
							Master_SendData(SLAVE_ADDR_6,OPEN_LED);//发送函数
							Master_SendData(SLAVE_ADDR_7,OPEN_LED);//发送函数
							 
						break;
						}
					}
				}
			break;
			}
			case 4: //  继电器从机01，地址码是04
			{
				if(fuction == 0x03)//从机返回的功能码
				{
					if((data3 == 0x01) && (data2 == 0x01) && (data1 ==0x01))//现实中的灯灭
					{
						Control_GroupLed(2,0);//网页上对应的灯灭
						HAL_Delay(100);
						 
					}
					else if((data3 == 0x00) && (data2 == 0x00) && (data1 ==0x00))//现实中的灯亮
					{
						Control_GroupLed(2,1);//网页上对应的灯亮
						HAL_Delay(100);
						 
					}
				}
				
			break;
			}
			case 5: //  继电器从机02，地址码是05
			{
				if(fuction == 0x03)//从机返回的功能码
				{
					if((data3 == 0x01) && (data2 == 0x01) && (data1 ==0x01))
					{
						Control_GroupLed(3,0);//网页上对应的灯灭
						 HAL_Delay(100);
					}
					else if((data3 == 0x00) && (data2 == 0x00) && (data1 ==0x00))
					{
						Control_GroupLed(3,1);//网页上对应的灯亮
						 HAL_Delay(100);
					}
				}
			break;
			}
			case 6: //  继电器从机03，地址码是06
			{
				if(fuction == 0x03)//从机返回的功能码
				{
					if((data3 == 0x01) && (data2 == 0x01) && (data1 ==0x01))
					{
						Control_GroupLed(4,0);//网页上对应的灯灭
						 HAL_Delay(100);
					}
					else if((data3 == 0x00) && (data2 == 0x00) && (data1 ==0x00))
					{
						Control_GroupLed(4,1);//网页上对应的灯亮
						 HAL_Delay(100);
					}
				}
			break;
			}
			case 7: //  继电器从机04，地址码是07
			{
				if(fuction == 0x03)//从机返回的功能码
				{
					if((data3 == 0x01) && (data2 == 0x01) && (data1 ==0x01))
					{
						Control_GroupLed(5,0);//网页上对应的灯灭
						 HAL_Delay(100);
					}
					else if((data3 == 0x00) && (data2 == 0x00) && (data1 ==0x00))
					{
						Control_GroupLed(5,1);//网页上对应的灯亮
						 HAL_Delay(100);
					}
				}
			break;
			}
			case 8: //  继电器从机05，地址码是08
			{
				if(fuction == 0x03)//从机返回的功能码
				{
					if((data3 == 0x01) && (data2 == 0x01) && (data1 ==0x01))
					{
						Control_GroupLed(6,0);//网页上对应的灯灭
						 HAL_Delay(100);
					}
					else if((data3 == 0x010) && (data2 == 0x00) && (data1 ==0x00))
					{
						Control_GroupLed(6,1);//网页上对应的灯亮
						 HAL_Delay(100);
					}
				}
			break;
			}
		
		
		}
	}
}

 









