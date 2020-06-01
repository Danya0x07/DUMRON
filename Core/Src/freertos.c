/* USER CODE BEGIN Header */
/**
  * Основная "бизнес-логика", представленная в виде задач.
  * Задачи выполняются в режиме вытесняющей многозадачности.
  * Используемая ОС - FreeRTOS с обёрткой CMSIS-RTOS.
  * Обёртка так себе, но выбора не было, STM32CubeIDE по другому не умеет.
  * В callback-ах по обнаружению ошибок издаём звуковые сигналы пьезобуззером
  * при включённом/выключенном отладочном светодиоде.
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "battery.h"
#include "motors.h"
#include "manipulator.h"
#include "radio.h"
#include "temperature.h"
#include "distance.h"
#include "emmiters.h"
#include "debug.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/**
  * Структура, описывающая формат посылок в очереди выходных данных.
  * Подразумевается, что задачи, взаимодействующие с датчиками должны
  * упаковывать добытые данные в эту структуру, и класть в очередь по
  * дескриптору outcomingElementQueueHandle, откуда эти данные считает
  * задача-сторож Task_UpdateOutcomingData.
  */
typedef struct {
    enum {
        BRAINS_CHARGE,
        MOTORS_CHARGE,
        BACK_DISTANCE,
        AMBIENT_TEMPERATURE,
        INTERNAL_TEMPERATURE
    } kind;
    union {
        uint8_t udata;
        int8_t  sdata;
    };
} OutcomingElement_s;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
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
static osMailQId outcomingElementQueueHandle;

/**
  * Глобальная переменная, которую используют только 2 задачи:
  * задача обновления выходных данных Task_UpdateOutcomingData и
  * задача обмена информацией с пультом Task_ExchangeDataWithRC.
  * Защищена мьютексом по дескриптору outcomingDataMutexHandle.
  */
static DataFromRobot_s outcomingData;

/**
  * Глобальная переменная, хранящая факт наличия обрыва позади робота.
  * Используется задачами Task_CheckDistance и Task_ExchangeDataWithRC.
  * Защищена мьютексом по дескриптору outcomingDataMutexHandle.
  */
static bool cliffBehindRobotDetected = false;

/* USER CODE END Variables */
osThreadId bldHandle;
osThreadId uodHandle;
osThreadId chdHandle;
osThreadId chtHandle;
osThreadId chbHandle;
osThreadId exdHandle;
osTimerId manipulatorTimerHandle;
osMutexId outcomingDataMutexHandle;
osMutexId incomingDataMutexHandle;
osSemaphoreId dataRecieveSemaphoreHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
/* USER CODE END FunctionPrototypes */

void Task_BlinkLed(void const * argument);
void Task_UpdateOutcomingData(void const * argument);
void Task_CheckDistance(void const * argument);
void Task_CheckTemp(void const * argument);
void Task_CheckBatteries(void const * argument);
void Task_ExchangeDataWithRC(void const * argument);
void updateManipulator(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook(void);

/* USER CODE BEGIN 2 */

/** Когда нет готовых к выполнению задач. */
void vApplicationIdleHook(void)
{
    /* Переводимся в спящий режим. */
    HAL_PWR_EnterSLEEPMode(0, PWR_SLEEPENTRY_WFI);
}
/* USER CODE END 2 */

/* USER CODE BEGIN 4 */

/** Если обнаружено переполнение стека. */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
    bool buzzerState = false;

    /* Издаём звуковые сигналы при включённом светодиоде. */
    Led_SetState(true);

    debug_logs((char*) pcTaskName);
    debug_logs(" : stack overflow\n");
    for (;;) {
        Buzzer_SetState((buzzerState = !buzzerState));
        HAL_Delay(200);
    }
}
/* USER CODE END 4 */

/* USER CODE BEGIN 5 */

/** Если не хватило памяти в куче. */
void vApplicationMallocFailedHook(void)
{
    bool buzzerState = false;

    /* Издаём звуковые сигналы при выключённом светодиоде. */
    Led_SetState(false);

    debug_logs("malloc failed\n");
    for (;;) {
        Buzzer_SetState((buzzerState = !buzzerState));
        HAL_Delay(500);
    }
}
/* USER CODE END 5 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* definition and creation of outcomingDataMutex */
  osMutexDef(outcomingDataMutex);
  outcomingDataMutexHandle = osMutexCreate(osMutex(outcomingDataMutex));

  /* definition and creation of incomingDataMutex */
  osMutexDef(incomingDataMutex);
  incomingDataMutexHandle = osMutexCreate(osMutex(incomingDataMutex));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of dataRecieveSemaphore */
  osSemaphoreDef(dataRecieveSemaphore);
  dataRecieveSemaphoreHandle = osSemaphoreCreate(osSemaphore(dataRecieveSemaphore), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of manipulatorTimer */
  osTimerDef(manipulatorTimer, updateManipulator);
  manipulatorTimerHandle = osTimerCreate(osTimer(manipulatorTimer), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  osMailQDef(outDataQueue, 8, OutcomingElement_s);
  outcomingElementQueueHandle = osMailCreate(osMailQ(outDataQueue), NULL);
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of bld */
  osThreadDef(bld, Task_BlinkLed, osPriorityLow, 0, 128);
  bldHandle = osThreadCreate(osThread(bld), NULL);

  /* definition and creation of uod */
  osThreadDef(uod, Task_UpdateOutcomingData, osPriorityBelowNormal, 0, 128);
  uodHandle = osThreadCreate(osThread(uod), NULL);

  /* definition and creation of chd */
  osThreadDef(chd, Task_CheckDistance, osPriorityBelowNormal, 0, 128);
  chdHandle = osThreadCreate(osThread(chd), NULL);

  /* definition and creation of cht */
  osThreadDef(cht, Task_CheckTemp, osPriorityLow, 0, 128);
  chtHandle = osThreadCreate(osThread(cht), NULL);

  /* definition and creation of chb */
  osThreadDef(chb, Task_CheckBatteries, osPriorityLow, 0, 128);
  chbHandle = osThreadCreate(osThread(chb), NULL);

  /* definition and creation of exd */
  osThreadDef(exd, Task_ExchangeDataWithRC, osPriorityNormal, 0, 128);
  exdHandle = osThreadCreate(osThread(exd), NULL);

  /* USER CODE BEGIN RTOS_THREADS */

  osTimerStart(manipulatorTimerHandle, 70);
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_Task_BlinkLed */
/**
  * Задача, мигающая отладочным светодиодом.
  * Показывает, что робот в состоянии хотя бы помигать светодиодом.
  */
/* USER CODE END Header_Task_BlinkLed */
void Task_BlinkLed(void const * argument)
{
  /* USER CODE BEGIN Task_BlinkLed */

    for(;;) {
        for (int_fast8_t i = 0; i < 2; i++) {
            Led_SetState(true);
            osDelay(140);
            Led_SetState(false);
            osDelay(70);
        }
        osDelay(1500);
    }

  /* USER CODE END Task_BlinkLed */
}

/* USER CODE BEGIN Header_Task_UpdateOutcomingData */
/**
  * Задача-сторож для данных, исходящих от робота к пульту.
  * Считывает посылки из очереди по дескриптору outcomingElementQueueHandle
  * и обновляет поля структуры outcomingData, которая потом будет
  * отправлена через радиомодуль на управляющую сторону.
  */
/* USER CODE END Header_Task_UpdateOutcomingData */
void Task_UpdateOutcomingData(void const * argument)
{
  /* USER CODE BEGIN Task_UpdateOutcomingData */

    OutcomingElement_s *element;
    osEvent event;

    for(;;) {
        event = osMailGet(outcomingElementQueueHandle, osWaitForever);
        if (event.status == osEventMail) {
            element = event.value.p;
            if (osMutexWait(outcomingDataMutexHandle, osWaitForever) == osOK) {
                switch (element->kind)
                {
                case BRAINS_CHARGE:
                    outcomingData.brainsCharge = element->udata;
                    break;
                case MOTORS_CHARGE:
                    outcomingData.motorsCharge = element->udata;
                    break;
                case BACK_DISTANCE:
                    outcomingData.backDistance = element->udata;
                    break;
                case AMBIENT_TEMPERATURE:
                    outcomingData.ambientTemperature = element->sdata;
                    break;
                case INTERNAL_TEMPERATURE:
                    outcomingData.internalTemperature = element->sdata;
                    break;
                }
                osMutexRelease(outcomingDataMutexHandle);
            } else {
                debug_logs("uod: odm failed\n");
            }
            osMailFree(outcomingElementQueueHandle, element);
        }
    }

  /* USER CODE END Task_UpdateOutcomingData */
}

/* USER CODE BEGIN Header_Task_CheckDistance */
/**
  * Задача, проверяющая расстояние от бампера робота до поверхности.
  * Если расстояние > MAX_SAFE_TO_FALL_DISTANCE_CM и в данный момент
  * робот двигается назад, то движение блокируется.
  */
/* USER CODE END Header_Task_CheckDistance */
void Task_CheckDistance(void const * argument)
{
  /* USER CODE BEGIN Task_CheckDistance */

    OutcomingElement_s *element;
    bool cliff = false;

    for(;;) {
        cliff = Distance_CliffDetected();

        if ((element = osMailAlloc(outcomingElementQueueHandle, 0)) != NULL) {
            element->kind = BACK_DISTANCE;
            element->udata = cliff * 12;
            osMailPut(outcomingElementQueueHandle, element);
        } else {
            debug_logs("chd: alloc failed\n");
        }

        if (osMutexWait(incomingDataMutexHandle, 100) == osOK) {
            if (cliff) {
                cliffBehindRobotDetected = true;
                if (Motors_GetDirection() == ROBOT_DIRECTION_BACKWARD)
                    Motors_SetSpeed(0, 0);
            } else {
                cliffBehindRobotDetected = false;
            }
            osMutexRelease(incomingDataMutexHandle);
        } else {
            debug_logs("chd: idm failed\n");
        }

        osDelay(200);
    }

  /* USER CODE END Task_CheckDistance */
}

/* USER CODE BEGIN Header_Task_CheckTemp */
/**
  * Задача, считывающая значение температуры
  * внутренних радиаторов и окружающей среды.
  */
/* USER CODE END Header_Task_CheckTemp */
void Task_CheckTemp(void const * argument)
{
  /* USER CODE BEGIN Task_CheckTemp */

    OutcomingElement_s *element;

    for(;;) {
        Temperature_StartMeasurement();
        osDelay(TEMPERATURE_MEASURE_TIME_MS);

        if ((element = osMailAlloc(outcomingElementQueueHandle, 0)) != NULL) {
            element->kind = INTERNAL_TEMPERATURE;
            element->sdata = Temperature_GetInternal();
            osMailPut(outcomingElementQueueHandle, element);
        } else {
            debug_logs("cht: alloc_1 failed\n");
        }
        osDelay(10);

        if ((element = osMailAlloc(outcomingElementQueueHandle, 0)) != NULL) {
            element->kind = AMBIENT_TEMPERATURE;
            element->sdata = Temperature_GetAmbient();
            osMailPut(outcomingElementQueueHandle, element);
        } else {
            debug_logs("cht: alloc_2 failed\n");
        }
        osDelay(1000);
    }

  /* USER CODE END Task_CheckTemp */
}

/* USER CODE BEGIN Header_Task_CheckBatteries */
/**
  * Задача, считывающая значение заряда аккумуляторов робота
  * с выходных узлов делителей напряжения.
  */
/* USER CODE END Header_Task_CheckBatteries */
void Task_CheckBatteries(void const * argument)
{
  /* USER CODE BEGIN Task_CheckBatteries */

    OutcomingElement_s *element;

    for(;;) {
        if ((element = osMailAlloc(outcomingElementQueueHandle, 0)) != NULL) {
            element->kind = BRAINS_CHARGE;
            element->udata = Battery_GetPercentageBrains();
            osMailPut(outcomingElementQueueHandle, element);
        } else {
            debug_logs("chb: alloc_1 failed\n");
        }
        osThreadYield();

        if ((element = osMailAlloc(outcomingElementQueueHandle, 0)) != NULL) {
            element->kind = MOTORS_CHARGE;
            element->udata = Battery_GetPercentageMotors();
            osMailPut(outcomingElementQueueHandle, element);
        } else {
            debug_logs("chb: alloc_2 failed\n");
        }
        osDelay(3000);
    }

  /* USER CODE END Task_CheckBatteries */
}

/* USER CODE BEGIN Header_Task_ExchangeDataWithRC */
/**
  * Задача, осуществляющая обмен информацией между роботом и пультом.
  * Срабатывает, когда обработчик прерывания по спаду напряжения
  * на IRQ пине радиомодуля отдаёт семафор по дескриптору
  * dataRecieveSemaphoreHandle.
  */
/* USER CODE END Header_Task_ExchangeDataWithRC */
void Task_ExchangeDataWithRC(void const * argument)
{
  /* USER CODE BEGIN Task_ExchangeDataWithRC */

    DataToRobot_s incomingData = {
        .direction = ROBOT_DIRECTION_NONE,
        .speedL = 0,
        .speedR = 0,
        .ctrlReg = 0
    };
    ArmDirection_e armDirection;
    ClawDirection_e clawDirection;

    for(;;) {
        osSemaphoreWait(dataRecieveSemaphoreHandle, osWaitForever);
        Radio_TakeIncoming(&incomingData);

        if (osMutexWait(incomingDataMutexHandle, 100) == osOK) {
            Motors_SetDirection(incomingData.direction);

            if (!cliffBehindRobotDetected ||
                    incomingData.direction != ROBOT_DIRECTION_BACKWARD)
                Motors_SetSpeed(incomingData.speedL, incomingData.speedR);

            if (incomingData.ctrlReg & ROBOT_CFLAG_ARM_UP)
                armDirection = ARM_UP;
            else if (incomingData.ctrlReg & ROBOT_CFLAG_ARM_DOWN)
                armDirection = ARM_DOWN;
            else
                armDirection = ARM_STOP;

            if (incomingData.ctrlReg & ROBOT_CFLAG_CLAW_SQUEEZE)
                clawDirection = CLAW_SQUEESE;
            else if (incomingData.ctrlReg & ROBOT_CFLAG_CLAW_RELEASE)
                clawDirection = CLAW_RELEASE;
            else
                clawDirection = CLAW_STOP;

            Manipulator_SetDirections(armDirection, clawDirection);

            Lights_SetState(incomingData.ctrlReg & ROBOT_CFLAG_LIGHTS_EN);
            Buzzer_SetState(incomingData.ctrlReg & ROBOT_CFLAG_KLAXON_EN);
            osMutexRelease(incomingDataMutexHandle);
        } else {
            debug_logs("exd: idm failed\n");
        }

        if (osMutexWait(outcomingDataMutexHandle, 50) == osOK) {
            Radio_PutOutcoming(&outcomingData);
            osMutexRelease(outcomingDataMutexHandle);
        } else {
             debug_logs("exd: odm failed\n");
        }

        /*
         * Включаем прерывание по спаду напряжение на IRQ пине радиомодуля
         * которое мы выключили в обработчике прерывания.
         */
        LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_5);
    }

  /* USER CODE END Task_ExchangeDataWithRC */
}

/* updateManipulator function */
void updateManipulator(void const * argument)
{
  /* USER CODE BEGIN updateManipulator */

    /*
     * Чтобы не вызывать сильные просадки напряжения силового аккумулятора,
     * положение манипулятора обновляется плавно, шагами.
     */
    if (osMutexWait(incomingDataMutexHandle, 100) != osErrorOS) {
        Manipulator_Move();
        osMutexRelease(incomingDataMutexHandle);
    } else {
         debug_logs("um: idm failed\n");
    }

  /* USER CODE END updateManipulator */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
