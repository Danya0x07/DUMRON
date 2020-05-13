/* USER CODE BEGIN Header */
/*
 * Здесь описана основная "бизнес-логика" прошивки.
 * Задачи выполняются в режиме вытесняющей многозадачности.
 * Используемая ОС - FreeRTOS с обёрткой CMSIS-RTOS.
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "debug.h"
#include "battery.h"
#include "motors.h"
#include "manipulator.h"
#include "radio.h"
#include "hcsr04/sonar.h"
#include "temperature.h"
#include "emmiters.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/**
 * Структура, описывающая формат посылок в очереди выходных данных.
 * Подразумевается, что задачи, взаимодействующие с датчиками должны
 * упаковывать добытые данные в эту структуру, и класть в очередь по
 * дескриптору outDataQueueHandle, откуда эти данные считает
 * задача-сторож updateOutData.
 */
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
#define MAX_SAFE_TO_FALL_DISTANCE    12
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/**
 * Дескриптор очереди выходных данных.
 * Используется задачами, получающими данные с датчиков
 * и задачей обновления выходных данных.
 */
static osMailQId outDataQueueHandle;

/**
 * Глобальная переменная, которую используют только 2 задачи:
 * задача обновления выходных данных updateOutData и
 * задача обмена информацией с пультом exchangeDataIO.
 * Защищена мьютексом по дескриптору outDataMutexHandle.
 */
static DataFromRobot data_from_robot;

/**
 * Глобальная переменная, хранящая факт наличия обрыва позади робота.
 * Используется задачами checkDistance и exchangeDataIO.
 * Защищена мьютексом по дескриптору outDataMutexHandle.
 */
static _Bool cliff_behind_robot = 0;

/* USER CODE END Variables */
osThreadId bldHandle;
osThreadId uodHandle;
osThreadId chdHandle;
osThreadId chtHandle;
osThreadId chbHandle;
osThreadId exdHandle;
osTimerId manipTimerHandle;
osMutexId outDataMutexHandle;
osMutexId inDataMutexHandle;
osSemaphoreId inDataReadySemHandle;
osStaticSemaphoreDef_t inDataReadySemControlBlock;
osSemaphoreId adcConvReadySemHandle;
osStaticSemaphoreDef_t adcConvReadySemControlBlock;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
/* USER CODE END FunctionPrototypes */

void blinkLedTask(void const * argument);
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
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook(void);

/* USER CODE BEGIN 2 */
void vApplicationIdleHook( void )
{
    /*
     * Для экономии энергии, когда нет готовых
     * к выполнению задач, переводимся в спящий режим.
     */
    HAL_PWR_EnterSLEEPMode(0, PWR_SLEEPENTRY_WFI);
}
/* USER CODE END 2 */

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
    _Bool buzzer_state = 0;
    debug_led_set(1);

    debug_logs("SO:");
    debug_logs((char*) pcTaskName);
    for (;;) {
        buzzer_set((buzzer_state = !buzzer_state));
        delay_ms(200);
    }
}
/* USER CODE END 4 */

/* USER CODE BEGIN 5 */
void vApplicationMallocFailedHook(void)
{
    _Bool buzzer_state = 0;
    debug_led_set(0);

    debug_logs("MF\n");
    for (;;) {
        buzzer_set((buzzer_state = !buzzer_state));
        delay_ms(500);
    }
}
/* USER CODE END 5 */

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
  osMutexDef(outDataMutex);
  outDataMutexHandle = osMutexCreate(osMutex(outDataMutex));

  /* definition and creation of inDataMutex */
  osMutexDef(inDataMutex);
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
  osTimerDef(manipTimer, updateManipulator);
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
  /* definition and creation of bld */
  osThreadDef(bld, blinkLedTask, osPriorityLow, 0, 128);
  bldHandle = osThreadCreate(osThread(bld), NULL);

  /* definition and creation of uod */
  osThreadDef(uod, updateOutData, osPriorityBelowNormal, 0, 128);
  uodHandle = osThreadCreate(osThread(uod), (void*) &data_from_robot);

  /* definition and creation of chd */
  osThreadDef(chd, checkDistance, osPriorityBelowNormal, 0, 128);
  chdHandle = osThreadCreate(osThread(chd), NULL);

  /* definition and creation of cht */
  osThreadDef(cht, checkTemp, osPriorityLow, 0, 128);
  chtHandle = osThreadCreate(osThread(cht), NULL);

  /* definition and creation of chb */
  osThreadDef(chb, checkBatteries, osPriorityLow, 0, 128);
  chbHandle = osThreadCreate(osThread(chb), NULL);

  /* definition and creation of exd */
  osThreadDef(exd, exchangeDataIO, osPriorityNormal, 0, 128);
  exdHandle = osThreadCreate(osThread(exd), (void*) &data_from_robot);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  osTimerStart(manipTimerHandle, 70);
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_blinkLedTask */
/**
 * Задача, мигающая отладочным светодиодом.
 * Показывает, что робот в состоянии хотя бы помигать светодиодом.
 */
/* USER CODE END Header_blinkLedTask */
void blinkLedTask(void const * argument)
{
  /* USER CODE BEGIN blinkLedTask */

    _Bool led_state = 0;

    for(;;) {
        debug_led_set((led_state = !led_state));
        osDelay(1000);
    }
  /* USER CODE END blinkLedTask */
}

/* USER CODE BEGIN Header_updateOutData */
/**
 * Задача-сторож для данных, исходящих от робота к пульту.
 * Считывает посылки из очереди по дескриптору outDataQueueHandle
 * и обновляет поля структуры data_from_robot, которая потом будет
 * отправлена через радиомодуль на управляющую сторону.
 */
/* USER CODE END Header_updateOutData */
void updateOutData(void const * argument)
{
  /* USER CODE BEGIN updateOutData */

    DataFromRobot* p_out_data = (DataFromRobot*) argument;
    osEvent event;
    OutDataElement* data;

    for(;;) {
        event = osMailGet(outDataQueueHandle, osWaitForever);
        if (event.status == osEventMail) {
            data = event.value.p;
            osMutexWait(outDataMutexHandle, osWaitForever);
            switch (data->kind)
            {
            case BATTERY_BRAINS:
                p_out_data->battery_brains = data->ub_data;
                break;
            case BATTERY_MOTORS:
                p_out_data->battery_motors = data->ub_data;
                break;
            case BACK_DISTANCE:
                p_out_data->back_distance = data->ub_data;
                break;
            case TEMP_AMBIENT:
                p_out_data->temperature_ambient = data->sb_data;
                break;
            case TEMP_RADIATORS:
                p_out_data->temperature_radiators = data->sb_data;
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
 * Задача, проверяющая расстояние от бампера робота до поверхности.
 * Если расстояние > MAX_SAFE_TO_FALL_DISTANCE: сзади робота обрыв.
 */
/* USER CODE END Header_checkDistance */
void checkDistance(void const * argument)
{
  /* USER CODE BEGIN checkDistance */

    OutDataElement* data;
    uint16_t distance;

    for(;;) {
        distance = sonar_scan();
        data = osMailAlloc(outDataQueueHandle, osWaitForever);
        data->kind = BACK_DISTANCE;
        data->ub_data = distance;
        osMailPut(outDataQueueHandle, data);

        osMutexWait(inDataMutexHandle, osWaitForever);
        if (distance > MAX_SAFE_TO_FALL_DISTANCE) {
            cliff_behind_robot = 1;
            if (motors_get_direction() == ROBOT_DIRECTION_BACKWARD)
                motors_set_speed(0, 0);
        } else {
            cliff_behind_robot = 0;
        }
        osMutexRelease(inDataMutexHandle);

        osDelay(200);
    }

  /* USER CODE END checkDistance */
}

/* USER CODE BEGIN Header_checkTemp */
/**
 * Задача, считывающая значение температуры внутренних радиаторов
 * и окружающей среды.
 */
/* USER CODE END Header_checkTemp */
void checkTemp(void const * argument)
{
  /* USER CODE BEGIN checkTemp */

    OutDataElement* data;

    for(;;) {
        temperature_start_conversion();
        osDelay(TEMPERATURE_CONVERSION_TIME);

        data = osMailAlloc(outDataQueueHandle, osWaitForever);
        data->kind = TEMP_RADIATORS;
        data->sb_data = temperature_get_radiators();
        osMailPut(outDataQueueHandle, data);

        osDelay(10);

        data = osMailAlloc(outDataQueueHandle, osWaitForever);
        data->kind = TEMP_AMBIENT;
        data->sb_data = temperature_get_ambient();
        osMailPut(outDataQueueHandle, data);
    }

  /* USER CODE END checkTemp */
}

/* USER CODE BEGIN Header_checkBatteries */
/**
 * Задача, считывающая значение заряда аккумуляторов робота
 * с выходных узлов делителей напряжения.
 */
/* USER CODE END Header_checkBatteries */
void checkBatteries(void const * argument)
{
  /* USER CODE BEGIN checkBatteries */

    OutDataElement* data;

    for(;;) {
        data = osMailAlloc(outDataQueueHandle, osWaitForever);
        data->kind = BATTERY_BRAINS;
        data->ub_data = battery_get_percentage_brains();
        osMailPut(outDataQueueHandle, data);
        osDelay(2000);

        data = osMailAlloc(outDataQueueHandle, osWaitForever);
        data->kind = BATTERY_MOTORS;
        data->ub_data = battery_get_percentage_motors();
        osMailPut(outDataQueueHandle, data);
        osDelay(2000);
    }

  /* USER CODE END checkBatteries */
}

/* USER CODE BEGIN Header_exchangeDataIO */
/**
 * Задача, осуществляющая обмен информацией между роботом и пультом.
 * Срабатывает, когда обработчик прерывания по спаду напряжения
 * на IRQ пине радиомодуля отдаёт семафор по дескриптору inDataSemHandle.
 */
/* USER CODE END Header_exchangeDataIO */
void exchangeDataIO(void const * argument)
{
  /* USER CODE BEGIN exchangeDataIO */

    static DataToRobot data_to_robot;
    DataFromRobot* p_out_data = (DataFromRobot*) argument;
    ArmServoDirection arm_servo_dir;
    ClawServoDirection claw_servo_dir;

    for(;;) {
        osSemaphoreWait(inDataReadySemHandle, osWaitForever);
        radio_take_incoming(&data_to_robot);

        osMutexWait(inDataMutexHandle, osWaitForever);
        motors_set_direction(data_to_robot.direction);

        if (!cliff_behind_robot || data_to_robot.direction != ROBOT_DIRECTION_BACKWARD)
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
        radio_put_outcoming(p_out_data);
        osMutexRelease(outDataMutexHandle);

        /*
         * Включаем прерывание по спаду напряжение на IRQ пине радиомодуля
         * которое мы выключили в обработчике прерывания.
         */
        LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_5);
    }

  /* USER CODE END exchangeDataIO */
}

/* updateManipulator function */
void updateManipulator(void const * argument)
{
  /* USER CODE BEGIN updateManipulator */

    /*
     * Чтобы не вызывать сильные просадки напряжения силового аккумулятора,
     * положение манипулятора обновляется плавно, шагами.
     */
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
