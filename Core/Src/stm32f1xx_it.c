/* USER CODE BEGIN Header */
/**
  * Обработчики прерываний и критических ошибок процессора.
  * В обработчиках ошибок процессора состояние пьезобуззера устанавливается
  * и не меняется, а встроенный в плату светодиод мигает определённым образом.
  * А в прерыванийх мы обычно выдаём семафоры.
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"
#include "FreeRTOS.h"
#include "task.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_os.h"
#include "emmiters.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */
/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/**
  * Топорная неубиваемая реализация приблизительной миллисекундной задержки
  * для использования в обработчиках критических ошибок, где стандартная
  * HAL_Delay может оказаться неработоспособной.
  */
static void clumsyDelayMs(uint32_t ms);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim4;

/* USER CODE BEGIN EV */
extern osSemaphoreId_t dataRecieveSemaphoreHandle;
extern osSemaphoreId_t batteryMsrSemaphoreHandle;
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */ 
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */
  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */
  bool ledState = false;
  Buzzer_SetState(1);
  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    Led_SetState((ledState = !ledState));
    clumsyDelayMs(100);
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */
  bool ledState = false;
  Buzzer_SetState(1);
  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    Led_SetState((ledState = !ledState));
    clumsyDelayMs(500);
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */
  Buzzer_SetState(0);
  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    Led_SetState(1);
    clumsyDelayMs(2000);
    Led_SetState(0);
    clumsyDelayMs(1000);
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */
  Buzzer_SetState(0);
  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    Led_SetState(1);
    clumsyDelayMs(2000);
    Led_SetState(0);
    clumsyDelayMs(500);
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel1 global interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  osSemaphoreRelease(batteryMsrSemaphoreHandle);
  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[9:5] interrupts.
  */
void EXTI9_5_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI9_5_IRQn 0 */
  /* USER CODE END EXTI9_5_IRQn 0 */
  if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_5) != RESET)
  {
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_5);
    /* USER CODE BEGIN LL_EXTI_LINE_5 */

    osSemaphoreRelease(dataRecieveSemaphoreHandle);

    /*
     * Поскольку радиомодуль поднимет IRQ пин обратно только тогда,
     * когда мы считаем принятые данные функцией radio_take_incoming,
     * прерывание по спаду напряжения на пине необходимо выключить,
     * иначе по выходу из обработчика мы опять в него попадём.
     */
    LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_5);

    /* USER CODE END LL_EXTI_LINE_5 */
  }
  /* USER CODE BEGIN EXTI9_5_IRQn 1 */
  /* USER CODE END EXTI9_5_IRQn 1 */
}

/**
  * @brief This function handles TIM4 global interrupt.
  */
void TIM4_IRQHandler(void)
{
  /* USER CODE BEGIN TIM4_IRQn 0 */
  /* USER CODE END TIM4_IRQn 0 */
  HAL_TIM_IRQHandler(&htim4);
  /* USER CODE BEGIN TIM4_IRQn 1 */
  /* USER CODE END TIM4_IRQn 1 */
}

/* USER CODE BEGIN 1 */

static void clumsyDelayMs(uint32_t ms)
{
    for (uint_fast32_t i = 0; i < ms; i++)
        for (uint_fast16_t j = 0; j < 2000; j++)
            __NOP();
}
/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
