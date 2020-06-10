/* USER CODE BEGIN Header */
/**
  * Обработчики прерываний.
  * Прерывание EXTI9_5 вызывается при падении напряжения на ноге,
  * к которой подключён радиомодуль.
  * В прерываниях по всякого рода исключительным ситуациям
  * мигаем с разной скоростью отладочным светодиодом
  * при включённом/выключённом пьезобуззере.
  * Прерывание TIM4 нужно для отсчёта квантов ОС.
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F1xx_IT_H
#define __STM32F1xx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 

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
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void DMA1_Channel1_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void TIM4_IRQHandler(void);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_IT_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
