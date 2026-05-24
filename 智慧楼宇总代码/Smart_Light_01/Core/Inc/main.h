/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "flash.h"
#include "function.h"
#include "input.h"
#include "output.h"
#include "time.h"
 
#include "485_rx.h"
#include "485_tx.h"
#include "gps_rx.h"
#include "gps_tx.h"
#include "datastore.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define relay_01_Pin GPIO_PIN_13
#define relay_01_GPIO_Port GPIOC
#define relay_02_Pin GPIO_PIN_14
#define relay_02_GPIO_Port GPIOC
#define RDE_485_Pin GPIO_PIN_1
#define RDE_485_GPIO_Port GPIOB
#define Input_10_Pin GPIO_PIN_11
#define Input_10_GPIO_Port GPIOA
#define Input_09_Pin GPIO_PIN_12
#define Input_09_GPIO_Port GPIOA
#define Input_08_Pin GPIO_PIN_15
#define Input_08_GPIO_Port GPIOA
#define Input_07_Pin GPIO_PIN_3
#define Input_07_GPIO_Port GPIOB
#define Input_06_Pin GPIO_PIN_4
#define Input_06_GPIO_Port GPIOB
#define Input_05_Pin GPIO_PIN_5
#define Input_05_GPIO_Port GPIOB
#define Input_04_Pin GPIO_PIN_6
#define Input_04_GPIO_Port GPIOB
#define Input_03_Pin GPIO_PIN_7
#define Input_03_GPIO_Port GPIOB
#define Input_02_Pin GPIO_PIN_8
#define Input_02_GPIO_Port GPIOB
#define Input_01_Pin GPIO_PIN_9
#define Input_01_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
