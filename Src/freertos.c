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
#include "robot_interface.h"
#include "modules/debug.h"
#include "modules/battery.h"
#include "modules/motors.h"
#include "modules/manipulator.h"
#include "modules/radio.h"
#include "modules/sonar.h"
#include "modules/temperature.h"
#include "modules/emmiters.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef struct {
    enum {
        BATTERY_BRAINS,
        BATTERY_MOTORS,
        BACK_DISTANCE,
        TEMP_AMBIENT,
        TEMP_RADIATORS
    } kind;
    union {
        uint8_t ub_data;
        int8_t  sb_data;
    };
} OutDataElement;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osMailQId outDataQueueHandle;
DataFromRobot data_from_robot;
/* USER CODE END Variables */
osThreadId _blinkLedHandle;
uint32_t blinkLedBuffer[ 128 ];
osStaticThreadDef_t blinkLedControlBlock;
osThreadId _updateODataHandle;
uint32_t updateODataBuffer[ 128 ];
osStaticThreadDef_t updateODataControlBlock;
osThreadId _checkDistanceHandle;
uint32_t checkDistanceBuffer[ 128 ];
osStaticThreadDef_t checkDistanceControlBlock;
osThreadId _checkTempHandle;
uint32_t checkTempBuffer[ 128 ];
osStaticThreadDef_t checkTempControlBlock;
osThreadId _checkBatsHandle;
uint32_t checkBatteriesBuffer[ 128 ];
osStaticThreadDef_t checkBatteriesControlBlock;
osThreadId _exchangeDataHandle;
uint32_t exchangeDataBuffer[ 128 ];
osStaticThreadDef_t exchangeDataControlBlock;
osTimerId manipTimerHandle;
osStaticTimerDef_t manipTimerControlBlock;
osMutexId outDataMutexHandle;
osStaticMutexDef_t outDataMutexControlBlock;
osMutexId inDataMutexHandle;
osStaticMutexDef_t inDataMutexControlBlock;
osSemaphoreId inDataReadySemHandle;
osStaticSemaphoreDef_t inDataReadySemControlBlock;
osSemaphoreId adcConvReadySemHandle;
osStaticSemaphoreDef_t adcConvReadySemControlBlock;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void blinkLed(void const * argument);
void updateOutData(void const * argument);
void checkDistance(void const * argument);
void checkTemp(void const * argument);
void checkBatteries(void const * argument);
void exchangeDataIO(void const * argument);
void updateManipulator(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

/* Hook prototypes */
void vApplicationIdleHook(void);

/* USER CODE BEGIN 2 */
__weak void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
    HAL_PWR_EnterSLEEPMode(0, PWR_SLEEPENTRY_WFI);
}
/* USER CODE END 2 */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
  
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}                   
/* USER CODE END GET_IDLE_TASK_MEMORY */

/* USER CODE BEGIN GET_TIMER_TASK_MEMORY */
static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];
  
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )  
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = &xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
  /* place for user code */
}                   
/* USER CODE END GET_TIMER_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* Create the mutex(es) */
  /* definition and creation of outDataMutex */
  osMutexStaticDef(outDataMutex, &outDataMutexControlBlock);
  outDataMutexHandle = osMutexCreate(osMutex(outDataMutex));

  /* definition and creation of inDataMutex */
  osMutexStaticDef(inDataMutex, &inDataMutexControlBlock);
  inDataMutexHandle = osMutexCreate(osMutex(inDataMutex));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of inDataReadySem */
  osSemaphoreStaticDef(inDataReadySem, &inDataReadySemControlBlock);
  inDataReadySemHandle = osSemaphoreCreate(osSemaphore(inDataReadySem), 1);

  /* definition and creation of adcConvReadySem */
  osSemaphoreStaticDef(adcConvReadySem, &adcConvReadySemControlBlock);
  adcConvReadySemHandle = osSemaphoreCreate(osSemaphore(adcConvReadySem), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of manipTimer */
  osTimerStaticDef(manipTimer, updateManipulator, &manipTimerControlBlock);
  manipTimerHandle = osTimerCreate(osTimer(manipTimer), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  osMailQDef(outDataQueue, 8, OutDataElement);
  outDataQueueHandle = osMailCreate(osMailQ(outDataQueue), NULL);
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of _blinkLed */
  osThreadStaticDef(_blinkLed, blinkLed, osPriorityLow, 0, 128, blinkLedBuffer, &blinkLedControlBlock);
  _blinkLedHandle = osThreadCreate(osThread(_blinkLed), NULL);

  /* definition and creation of _updateOData */
  osThreadStaticDef(_updateOData, updateOutData, osPriorityBelowNormal, 0, 128, updateODataBuffer, &updateODataControlBlock);
  _updateODataHandle = osThreadCreate(osThread(_updateOData), NULL);

  /* definition and creation of _checkDistance */
  osThreadStaticDef(_checkDistance, checkDistance, osPriorityBelowNormal, 0, 128, checkDistanceBuffer, &checkDistanceControlBlock);
  _checkDistanceHandle = osThreadCreate(osThread(_checkDistance), NULL);

  /* definition and creation of _checkTemp */
  osThreadStaticDef(_checkTemp, checkTemp, osPriorityLow, 0, 128, checkTempBuffer, &checkTempControlBlock);
  _checkTempHandle = osThreadCreate(osThread(_checkTemp), NULL);

  /* definition and creation of _checkBats */
  osThreadStaticDef(_checkBats, checkBatteries, osPriorityLow, 0, 128, checkBatteriesBuffer, &checkBatteriesControlBlock);
  _checkBatsHandle = osThreadCreate(osThread(_checkBats), NULL);

  /* definition and creation of _exchangeData */
  osThreadStaticDef(_exchangeData, exchangeDataIO, osPriorityNormal, 0, 128, exchangeDataBuffer, &exchangeDataControlBlock);
  _exchangeDataHandle = osThreadCreate(osThread(_exchangeData), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  osTimerStart(manipTimerHandle, 200);
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_blinkLed */
/**
  * @brief  Function implementing the blink_a_led thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_blinkLed */
void blinkLed(void const * argument)
{
  /* USER CODE BEGIN blinkLed */
    _Bool led_state = 0;

    for(;;)
    {
        debug_led_set(led_state = !led_state);
        osDelay(500);
    }
  /* USER CODE END blinkLed */
}

/* USER CODE BEGIN Header_updateOutData */
/**
* @brief Function implementing the _updateOData thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_updateOutData */
void updateOutData(void const * argument)
{
  /* USER CODE BEGIN updateOutData */
    osEvent event;
    OutDataElement* data;

    for(;;)
    {
        event = osMailGet(outDataQueueHandle, osWaitForever);
        if (event.status == osEventMail) {
            data = event.value.p;
            osMutexWait(outDataMutexHandle, osWaitForever);
            switch (data->kind) {
                case BATTERY_BRAINS:
                    data_from_robot.battery_brains = data->ub_data;
                    break;
                case BATTERY_MOTORS:
                    data_from_robot.battery_motors = data->ub_data;
                    break;
                case BACK_DISTANCE:
                    data_from_robot.back_distance = data->ub_data;
                    break;
                case TEMP_AMBIENT:
                    data_from_robot.temperature_ambient = data->sb_data;
                    break;
                case TEMP_RADIATORS:
                    data_from_robot.temperature_radiators = data->sb_data;
                    break;
            }
            osMutexRelease(outDataMutexHandle);
            osMailFree(outDataQueueHandle, data);
        }

    }
  /* USER CODE END updateOutData */
}

/* USER CODE BEGIN Header_checkDistance */
/**
* @brief Function implementing the _checkDistance thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_checkDistance */
void checkDistance(void const * argument)
{
  /* USER CODE BEGIN checkDistance */
    OutDataElement* data;

    for(;;)
    {
        data = osMailAlloc(outDataQueueHandle, osWaitForever);
        *data = (OutDataElement) {
            .kind = BACK_DISTANCE,
            .ub_data = sonar_scan(),
        };
        osMailPut(outDataQueueHandle, data);
        osDelay(300);
    }
  /* USER CODE END checkDistance */
}

/* USER CODE BEGIN Header_checkTemp */
/**
* @brief Function implementing the _checkTemp thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_checkTemp */
void checkTemp(void const * argument)
{
  /* USER CODE BEGIN checkTemp */
    OutDataElement* data;

    for(;;)
    {
        temperature_start_conversion();
        osDelay(2000);

        data = osMailAlloc(outDataQueueHandle, osWaitForever);
        *data = (OutDataElement) {
            .kind = TEMP_RADIATORS,
            .sb_data = temperature_get_radiators()
        };
        osMailPut(outDataQueueHandle, data);

        data = osMailAlloc(outDataQueueHandle, osWaitForever);
        *data = (OutDataElement) {
            .kind = TEMP_AMBIENT,
            .sb_data = temperature_get_ambient()
        };
        osMailPut(outDataQueueHandle, data);
    }
  /* USER CODE END checkTemp */
}

/* USER CODE BEGIN Header_checkBatteries */
/**
* @brief Function implementing the _checkBats thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_checkBatteries */
void checkBatteries(void const * argument)
{
  /* USER CODE BEGIN checkBatteries */
    OutDataElement* data;

    for(;;)
    {
        data = osMailAlloc(outDataQueueHandle, osWaitForever);
        *data = (OutDataElement) {
            .kind = BATTERY_BRAINS,
            .ub_data = battery_get_percentage_brains()
        };
        osMailPut(outDataQueueHandle, data);
        osDelay(2000);

        data = osMailAlloc(outDataQueueHandle, osWaitForever);
        *data = (OutDataElement) {
            .kind = BATTERY_MOTORS,
            .ub_data = battery_get_percentage_motors()
        };
        osMailPut(outDataQueueHandle, data);
        osDelay(2000);
    }
  /* USER CODE END checkBatteries */
}

/* USER CODE BEGIN Header_exchangeDataIO */
/**
* @brief Function implementing the _exchangeData thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_exchangeDataIO */
void exchangeDataIO(void const * argument)
{
  /* USER CODE BEGIN exchangeDataIO */
    static DataToRobot data_to_robot;
    ArmServoDirection arm_servo_dir;
    ClawServoDirection claw_servo_dir;

    for(;;)
    {
        osSemaphoreWait(inDataReadySemHandle, osWaitForever);
        radio_take_incoming(&data_to_robot);

        osMutexWait(inDataMutexHandle, osWaitForever);
        motors_set_direction(data_to_robot.direction);
        motors_set_speed(data_to_robot.speed_left, data_to_robot.speed_right);

        if (data_to_robot.control_reg & ROBOT_CFLAG_ARM_UP)
            arm_servo_dir = ARM_UP;
        else if (data_to_robot.control_reg & ROBOT_CFLAG_ARM_DOWN)
            arm_servo_dir = ARM_DOWN;
        else
            arm_servo_dir = ARM_STOP;

        if (data_to_robot.control_reg & ROBOT_CFLAG_CLAW_SQUEEZE)
            claw_servo_dir = CLAW_SQUEESE;
        else if (data_to_robot.control_reg & ROBOT_CFLAG_CLAW_RELEASE)
            claw_servo_dir = CLAW_RELEASE;
        else
            claw_servo_dir = CLAW_STOP;

        manipulator_set_directions(arm_servo_dir, claw_servo_dir);

        lights_set(data_to_robot.control_reg & ROBOT_CFLAG_LIGHTS_EN);
        buzzer_set(data_to_robot.control_reg & ROBOT_CFLAG_KLAXON_EN);
        osMutexRelease(inDataMutexHandle);

        osMutexWait(outDataMutexHandle, osWaitForever);
        radio_put_outcoming(&data_from_robot);
        osMutexRelease(outDataMutexHandle);

        LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_5);
    }
  /* USER CODE END exchangeDataIO */
}

/* updateManipulator function */
void updateManipulator(void const * argument)
{
  /* USER CODE BEGIN updateManipulator */
    if (osMutexWait(inDataMutexHandle, 0) != osErrorOS) {
        manipulator_move();
        osMutexRelease(inDataMutexHandle);
    }
  /* USER CODE END updateManipulator */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
