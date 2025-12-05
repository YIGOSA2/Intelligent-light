#include "rs485.h"
#include "output.h"

// 发送相关变量
unsigned char uart_tx_buffer[FRAME_LENGTH];
unsigned char uart_tx_busy = 0;
unsigned char TxSend_Flag = 0;  // 1s发送计时
// 从机数据变量
unsigned char Sensor_1 = 1;
unsigned char Sensor_2 = 1;
unsigned char Sensor_3 = 1;
unsigned char Sensor_4 = 1;
unsigned char Sensor_5 = 1;
unsigned char Sensor_6 = 1;
unsigned char Sensor_7 = 1;
unsigned char Sensor_8 = 1;
unsigned char Sensor_9 = 1;
unsigned char Sensor_10 = 1;

// 接收相关变量
unsigned char uart_rx_buffer[FRAME_LENGTH];
unsigned char uart_rx_index = 0;
unsigned char uart_rx_complete = 0;

void Slave_Init(void)
{
    // 初始化485为接收模式
    RS485_RX_MODE();
    // 启动UART接收中断
    HAL_UART_Receive_IT(&huart3, &uart_rx_buffer[uart_rx_index], 1);
	
}

void RS485_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
    if (uart_tx_busy == 0) 
    {
        uart_tx_busy = 1;
        
        // 切换到发送模式
        RS485_TX_MODE();
        
        // 短暂延时确保模式切换稳定
        for(volatile int i = 0; i < 100; i++);
        
        // 启动UART发送
        HAL_UART_Transmit_IT(huart, pData, Size);
    }
}

void Slave_Process_Command(unsigned char *rx_data, unsigned char len)
{
//    unsigned char nack_data[] = {0x02}; // 不支持的功能码
//    
    // 基本帧结构检查
    if (len < 8) return;
    
    // 检查帧头和帧尾
    if (rx_data[0] != FRAME_HEADER_1 || rx_data[1] != FRAME_HEADER_2 || 
        rx_data[len-1] != FRAME_TAIL) {
        return;
    }
    
    // 检查CRC
    unsigned short received_crc = (rx_data[len-3] << 8) | rx_data[len-2];
    unsigned short calculated_crc = CalculateCRC16(rx_data, len-3);
    if (received_crc != calculated_crc) {
        return;
    }
    
    // 提取命令字段
    unsigned char target_addr = rx_data[2];
    unsigned char function = rx_data[3];
    unsigned char data_len = rx_data[4];
    
    // 检查地址匹配
    if (target_addr != SLAVE_ADDRESS) {
        return;  // 地址不匹配，不响应
    }

    // 根据功能码处理命令
    switch (function) {
        case FUNC_GET_PARAM:    // 参数读取
            Slave_Send_Data();
            break;
				
				case FUNC_SENSOR_LIGHT:   //依据传感器开灯
						Sensor_Light();
						break;
				
				case FUNC_RUN_LIGHT:      //执行开灯命令
						Run_Light();
            break;
						
				case FUNC_SHUT_LIGHT:			//执行关灯命令
						SHUT_Light();
						break;
				
				case FUNC_SWTICH_OFF:			//禁用机械开关
						Switch_Off();
						break;
				
				case FUNC_SWITCH_ON:      //使能机械开关
						Switch_On();
						break;
				
        default:
					 
            break;
    }
}

void Slave_Send_Response(unsigned char function, unsigned char *data, unsigned char data_len)
{
    // 构建数据帧
    uart_tx_buffer[0] = FRAME_HEADER_1;
    uart_tx_buffer[1] = FRAME_HEADER_2;
    uart_tx_buffer[2] = SLAVE_ADDRESS;
    uart_tx_buffer[3] = function;
    uart_tx_buffer[4] = data_len;
    
    // 复制数据
    for (int i = 0; i < data_len; i++) {
        uart_tx_buffer[5 + i] = data[i];
    }
    
    // 计算CRC16校验
    unsigned short crc = CalculateCRC16(uart_tx_buffer, 5 + data_len);
    uart_tx_buffer[5 + data_len] = (crc >> 8) & 0xFF;
    uart_tx_buffer[6 + data_len] = crc & 0xFF;
    uart_tx_buffer[7 + data_len] = FRAME_TAIL;
    
    // 启动发送
    RS485_UART_Transmit_IT(&huart3, uart_tx_buffer, 8 + data_len);
}

void Slave_Send_Data(void)
{
    unsigned char data[3];
		Sensor_1  = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9);
		Sensor_2  = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8);
		Sensor_3  = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7);
		Sensor_4  = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_6);
		Sensor_5  = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5);
		Sensor_6  = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4);
		Sensor_7  = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_3);
		Sensor_8  = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15);
		Sensor_9  = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_12);
		Sensor_10 = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_11);
		
	if((Sensor_1 == GPIO_PIN_RESET) || (Sensor_2 == GPIO_PIN_RESET) || (Sensor_3 == GPIO_PIN_RESET) || (Sensor_4 == GPIO_PIN_RESET) || (Sensor_5 == GPIO_PIN_RESET)
		|| (Sensor_6 == GPIO_PIN_RESET) || (Sensor_7 == GPIO_PIN_RESET) || (Sensor_8 == GPIO_PIN_RESET) ||(Sensor_9 == GPIO_PIN_RESET) || (Sensor_10 == GPIO_PIN_RESET))
	{
		data[0] = 0;
		data[1] = 0;
		data[2] = 0;
	}
	else if((Sensor_1 == GPIO_PIN_SET) || (Sensor_2 == GPIO_PIN_SET) || (Sensor_3 == GPIO_PIN_SET) || (Sensor_4 == GPIO_PIN_SET) || (Sensor_5 == GPIO_PIN_SET)
		|| (Sensor_6 == GPIO_PIN_SET) || (Sensor_7 == GPIO_PIN_SET) || (Sensor_8 == GPIO_PIN_SET) ||(Sensor_9 == GPIO_PIN_SET) || (Sensor_10 == GPIO_PIN_SET))
	{
		data[0] = 1;
		data[1] = 1;
		data[2] = 1;
	}


    
    Slave_Send_Response(FUNC_GET_PARAM, data, 3);
}

unsigned short CalculateCRC16(const unsigned char *data, unsigned int length)
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

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3) {
        // 发送完成，切换回接收模式
        RS485_RX_MODE();
        
        // 短暂延时确保模式切换稳定
        for(volatile int i = 0; i < 50; i++);
        
        uart_tx_busy = 0;
			  HAL_UART_Receive_IT(&huart3, &uart_rx_buffer[uart_rx_index], 1);
    }
}



/**
  * @brief  UART接收完成回调（简化稳定版）
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // 仅处理USART3的回调（新增：确保仅响应USART3，避免其他串口干扰）
    if (huart->Instance != USART3)
    {
        return;
    }

    // 处理接收错误（快速清除错误，避免阻塞）
    if (huart->ErrorCode != HAL_UART_ERROR_NONE)
    {
        __HAL_UART_CLEAR_OREFLAG(huart);  // 清除溢出错误
        __HAL_UART_CLEAR_NEFLAG(huart);  // 清除噪声错误
        __HAL_UART_CLEAR_FEFLAG(huart);  // 清除帧错误
        __HAL_UART_CLEAR_PEFLAG(huart);  // 清除奇偶校验错误  
        uart_rx_index = 0; // 重置接收状态
        HAL_UART_Receive_IT(&huart3, &uart_rx_buffer[uart_rx_index], 1); // 改为huart3
        return;
    }

    // 正常接收：先递增索引（修复原逻辑错误）
    uart_rx_index++;

    // 缓冲区溢出保护（避免越界）
    if (uart_rx_index >= FRAME_LENGTH) uart_rx_index = 0;

    // 检查帧尾（当前接收的最后一个字节）
    if (uart_rx_buffer[uart_rx_index - 1] == FRAME_TAIL && uart_rx_index >= 8)
    {
        Slave_Process_Command(uart_rx_buffer, uart_rx_index); // 处理完整帧
        memset(uart_rx_buffer, 0, uart_rx_index); // 清空已处理数据
        uart_rx_index = 0; // 重置接收索引
    }

    // 强制重启接收（确保下一字节正常接收）
    HAL_UART_Receive_IT(&huart3, &uart_rx_buffer[uart_rx_index], 1); // 改为huart3
}

