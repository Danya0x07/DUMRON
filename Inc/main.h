/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define MOT_GPIO_Port	GPIOB
/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_DEBUG_Pin GPIO_PIN_13
#define LED_DEBUG_GPIO_Port GPIOC
#define SERVO_SHOULDER_Pin GPIO_PIN_0
#define SERVO_SHOULDER_GPIO_Port GPIOA
#define SERVO_CLAW_Pin GPIO_PIN_1
#define SERVO_CLAW_GPIO_Port GPIOA
#define NRF_CSN_Pin GPIO_PIN_4
#define NRF_CSN_GPIO_Port GPIOA
#define BRAIN_BATTERY_Pin GPIO_PIN_0
#define BRAIN_BATTERY_GPIO_Port GPIOB
#define MOT_BATTERY_Pin GPIO_PIN_1
#define MOT_BATTERY_GPIO_Port GPIOB
#define MOT_R_P2_Pin GPIO_PIN_12
#define MOT_R_P2_GPIO_Port GPIOB
#define MOT_R_P1_Pin GPIO_PIN_13
#define MOT_R_P1_GPIO_Port GPIOB
#define MOT_L_P2_Pin GPIO_PIN_14
#define MOT_L_P2_GPIO_Port GPIOB
#define MOT_L_P1_Pin GPIO_PIN_15
#define MOT_L_P1_GPIO_Port GPIOB
#define MOT_R_EN_Pin GPIO_PIN_8
#define MOT_R_EN_GPIO_Port GPIOA
#define MOT_L_EN_Pin GPIO_PIN_9
#define MOT_L_EN_GPIO_Port GPIOA
#define SONAR_ECHO_Pin GPIO_PIN_10
#define SONAR_ECHO_GPIO_Port GPIOA
#define SONAR_TRIG_Pin GPIO_PIN_11
#define SONAR_TRIG_GPIO_Port GPIOA
#define ONEWIRE_Pin GPIO_PIN_12
#define ONEWIRE_GPIO_Port GPIOA
#define NRF_CE_Pin GPIO_PIN_15
#define NRF_CE_GPIO_Port GPIOA
#define NRF_IRQ_Pin GPIO_PIN_5
#define NRF_IRQ_GPIO_Port GPIOB
#define LIGHTS_Pin GPIO_PIN_8
#define LIGHTS_GPIO_Port GPIOB
#define BUZZER_Pin GPIO_PIN_9
#define BUZZER_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
