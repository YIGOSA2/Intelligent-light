#ifndef __OUTPUT_H
#define __OUTPUT_H


#include "main.h"
#include "gpio.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>

extern volatile uint32_t timer_counter; 
extern volatile uint8_t timer_running_flag;


void output_Init(void);
void Run_Light(void);
void SHUT_Light(void);
void Switch_On(void);
void Switch_Off(void); 
void Sensor_Light(void);

#endif
