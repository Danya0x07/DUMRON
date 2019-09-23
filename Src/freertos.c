/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId distanceHandle;
osThreadId temperatureHandle;
osThreadId batteriesHandle;
osThreadId manipulatorHandle;
osThreadId odataHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void check_back_distance(void const * argument);
void check_temp(void const * argument);
void check_batteries(void const * argument);
void update_manipulator(void const * argument);
void update_outcoming_data(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of distance */
  osThreadDef(distance, check_back_distance, osPriorityNormal, 0, 128);
  distanceHandle = osThreadCreate(osThread(distance), NULL);

  /* definition and creation of temperature */
  osThreadDef(temperature, check_temp, osPriorityNormal, 0, 128);
  temperatureHandle = osThreadCreate(osThread(temperature), NULL);

  /* definition and creation of batteries */
  osThreadDef(batteries, check_batteries, osPriorityNormal, 0, 128);
  batteriesHandle = osThreadCreate(osThread(batteries), NULL);

  /* definition and creation of manipulator */
  osThreadDef(manipulator, update_manipulator, osPriorityNormal, 0, 128);
  manipulatorHandle = osThreadCreate(osThread(manipulator), NULL);

  /* definition and creation of odata */
  osThreadDef(odata, update_outcoming_data, osPriorityNormal, 0, 128);
  odataHandle = osThreadCreate(osThread(odata), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
    
    
    
    
    
    
    

  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_check_back_distance */
/**
* @brief Function implementing the distance thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_check_back_distance */
void check_back_distance(void const * argument)
{
  /* USER CODE BEGIN check_back_distance */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END check_back_distance */
}

/* USER CODE BEGIN Header_check_temp */
/**
* @brief Function implementing the temperature thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_check_temp */
void check_temp(void const * argument)
{
  /* USER CODE BEGIN check_temp */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END check_temp */
}

/* USER CODE BEGIN Header_check_batteries */
/**
* @brief Function implementing the batteries thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_check_batteries */
void check_batteries(void const * argument)
{
  /* USER CODE BEGIN check_batteries */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END check_batteries */
}

/* USER CODE BEGIN Header_update_manipulator */
/**
* @brief Function implementing the manipulator thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_update_manipulator */
void update_manipulator(void const * argument)
{
  /* USER CODE BEGIN update_manipulator */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END update_manipulator */
}

/* USER CODE BEGIN Header_update_outcoming_data */
/**
* @brief Function implementing the odata thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_update_outcoming_data */
void update_outcoming_data(void const * argument)
{
  /* USER CODE BEGIN update_outcoming_data */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END update_outcoming_data */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
