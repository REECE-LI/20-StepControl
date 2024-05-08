/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32f3xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
#define BUTTON_UP_Pin GPIO_PIN_3
#define BUTTON_UP_GPIO_Port GPIOA
#define VREF_A_Pin GPIO_PIN_4
#define VREF_A_GPIO_Port GPIOA
#define VREF_B_Pin GPIO_PIN_5
#define VREF_B_GPIO_Port GPIOA
#define MT6818_CLK_Pin GPIO_PIN_13
#define MT6818_CLK_GPIO_Port GPIOB
#define MT6816_MISO_Pin GPIO_PIN_14
#define MT6816_MISO_GPIO_Port GPIOB
#define MT6818_MOSI_Pin GPIO_PIN_15
#define MT6818_MOSI_GPIO_Port GPIOB
#define MT6816_CS_Pin GPIO_PIN_5
#define MT6816_CS_GPIO_Port GPIOB
#define BM_Pin GPIO_PIN_6
#define BM_GPIO_Port GPIOB
#define BP_Pin GPIO_PIN_7
#define BP_GPIO_Port GPIOB
#define AP_Pin GPIO_PIN_8
#define AP_GPIO_Port GPIOB
#define AM_Pin GPIO_PIN_9
#define AM_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
