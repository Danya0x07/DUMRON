/* USER CODE BEGIN Header */
/**
  * Макросы для портов и пинов перифферии.
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
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_exti.h"
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_utils.h"
#include "stm32f1xx_ll_pwr.h"
#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx_ll_spi.h"
#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx.h"
#include "stm32f1xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "tim.h"
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
#define LED_BUILTIN_Pin LL_GPIO_PIN_13
#define LED_BUILTIN_GPIO_Port GPIOC
#define SERVO_ARM_Pin LL_GPIO_PIN_0
#define SERVO_ARM_GPIO_Port GPIOA
#define SERVO_CLAW_Pin LL_GPIO_PIN_1
#define SERVO_CLAW_GPIO_Port GPIOA
#define NRF_CSN_Pin LL_GPIO_PIN_4
#define NRF_CSN_GPIO_Port GPIOA
#define BATTERY_MOTORS_Pin LL_GPIO_PIN_0
#define BATTERY_MOTORS_GPIO_Port GPIOB
#define BATTERY_BRAINS_Pin LL_GPIO_PIN_1
#define BATTERY_BRAINS_GPIO_Port GPIOB
#define MOTOR_R1_Pin LL_GPIO_PIN_12
#define MOTOR_R1_GPIO_Port GPIOB
#define MOTOR_R2_Pin LL_GPIO_PIN_13
#define MOTOR_R2_GPIO_Port GPIOB
#define MOTOR_L1_Pin LL_GPIO_PIN_14
#define MOTOR_L1_GPIO_Port GPIOB
#define MOTOR_L2_Pin LL_GPIO_PIN_15
#define MOTOR_L2_GPIO_Port GPIOB
#define MOTOR_RPWM_Pin LL_GPIO_PIN_8
#define MOTOR_RPWM_GPIO_Port GPIOA
#define MOTOR_LPWM_Pin LL_GPIO_PIN_9
#define MOTOR_LPWM_GPIO_Port GPIOA
#define SONAR_ECHO_Pin LL_GPIO_PIN_10
#define SONAR_ECHO_GPIO_Port GPIOA
#define SONAR_TRIG_Pin LL_GPIO_PIN_11
#define SONAR_TRIG_GPIO_Port GPIOA
#define ONEWIRE_Pin LL_GPIO_PIN_12
#define ONEWIRE_GPIO_Port GPIOA
#define NRF_CE_Pin LL_GPIO_PIN_15
#define NRF_CE_GPIO_Port GPIOA
#define NRF_IRQ_Pin LL_GPIO_PIN_5
#define NRF_IRQ_GPIO_Port GPIOB
#define NRF_IRQ_EXTI_IRQn EXTI9_5_IRQn
#define LIGHTS_Pin LL_GPIO_PIN_8
#define LIGHTS_GPIO_Port GPIOB
#define BUZZER_Pin LL_GPIO_PIN_9
#define BUZZER_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define MOTOR_GPIO_Port   GPIOB
#define MOTOR_TIM   TIM1
#define MOTOR_L_PWM_Reg   CCR2
#define MOTOR_R_PWM_Reg   CCR1

#define SERVO_TIM   TIM2
#define SERVO_ARM_PWM_Reg   CCR1
#define SERVO_CLAW_PWM_Reg  CCR2

#define BATTERY_BRAIN_Channel       ADC_CHANNEL_9
#define BATTERY_RADIATORS_Channel   ADC_CHANNEL_8

#define SONAR_GPIO_Port   GPIOA
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
