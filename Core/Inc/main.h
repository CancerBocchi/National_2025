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
#include "stm32g4xx_hal.h"

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
#define KEY_ROW2_Pin GPIO_PIN_13
#define KEY_ROW2_GPIO_Port GPIOC
#define KEY_ROW1_Pin GPIO_PIN_14
#define KEY_ROW1_GPIO_Port GPIOC
#define KEY_ROW0_Pin GPIO_PIN_15
#define KEY_ROW0_GPIO_Port GPIOC
#define KEY_COL0_Pin GPIO_PIN_0
#define KEY_COL0_GPIO_Port GPIOC
#define KEY_COL1_Pin GPIO_PIN_1
#define KEY_COL1_GPIO_Port GPIOC
#define KEY_COL2_Pin GPIO_PIN_2
#define KEY_COL2_GPIO_Port GPIOC
#define KEY_COL3_Pin GPIO_PIN_3
#define KEY_COL3_GPIO_Port GPIOC
#define OLED_CS_Pin GPIO_PIN_12
#define OLED_CS_GPIO_Port GPIOC
#define OLED_DC_Pin GPIO_PIN_2
#define OLED_DC_GPIO_Port GPIOD
#define OLED_RES_Pin GPIO_PIN_4
#define OLED_RES_GPIO_Port GPIOB
#define KEY_ROW3_Pin GPIO_PIN_9
#define KEY_ROW3_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
