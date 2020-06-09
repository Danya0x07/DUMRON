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
#include "errors.h"
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
        BACK_DISTANCE,
        BRAINS_CHARGE,
        MOTORS_CHARGE,
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
  * Глобальная переменная, которую используют только 2 задачи:
  * задача обновления выходных данных Task_UpdateOutcomingData и
  * задача обмена информацией с пультом Task_ExchangeDataWithRC.
  * Защищена мьютексом по дескриптору outcomingDataMutexHandle.
  */
static DataFromRobot_s outcomingData = {0};

/**
  * Глобальная переменная, хранящая факт наличия обрыва позади робота.
  * Используется задачами Task_CheckDistance и Task_ExchangeDataWithRC.
  * Защищена мьютексом по дескриптору outcomingDataMutexHandle.
  */
static bool cliffBehindRobotDetected = false;

/* USER CODE END Variables */
/* Definitions for bld */
osThreadId_t bldHandle;
const osThreadAttr_t bld_attributes = {
  .name = "bld",
  .priority = (osPriority_t) osPriorityLow3,
  .stack_size = 128 * 4
};
/* Definitions for uod */
osThreadId_t uodHandle;
const osThreadAttr_t uod_attributes = {
  .name = "uod",
  .priority = (osPriority_t) osPriorityBelowNormal,
  .stack_size = 128 * 4
};
/* Definitions for chd */
osThreadId_t chdHandle;
const osThreadAttr_t chd_attributes = {
  .name = "chd",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for cht */
osThreadId_t chtHandle;
const osThreadAttr_t cht_attributes = {
  .name = "cht",
  .priority = (osPriority_t) osPriorityLow5,
  .stack_size = 128 * 4
};
/* Definitions for chb */
osThreadId_t chbHandle;
const osThreadAttr_t chb_attributes = {
  .name = "chb",
  .priority = (osPriority_t) osPriorityLow7,
  .stack_size = 128 * 4
};
/* Definitions for exd */
osThreadId_t exdHandle;
const osThreadAttr_t exd_attributes = {
  .name = "exd",
  .priority = (osPriority_t) osPriorityHigh,
  .stack_size = 128 * 4
};
/* Definitions for outcomingElementQueue */
osMessageQueueId_t outcomingElementQueueHandle;
const osMessageQueueAttr_t outcomingElementQueue_attributes = {
  .name = "outcomingElementQueue"
};
/* Definitions for manipulatorTimer */
osTimerId_t manipulatorTimerHandle;
const osTimerAttr_t manipulatorTimer_attributes = {
  .name = "manipulatorTimer"
};
/* Definitions for outcomingDataMutex */
osMutexId_t outcomingDataMutexHandle;
const osMutexAttr_t outcomingDataMutex_attributes = {
  .name = "outcomingDataMutex"
};
/* Definitions for incomingDataMutex */
osMutexId_t incomingDataMutexHandle;
const osMutexAttr_t incomingDataMutex_attributes = {
  .name = "incomingDataMutex"
};
/* Definitions for dataRecieveSemaphore */
osSemaphoreId_t dataRecieveSemaphoreHandle;
const osSemaphoreAttr_t dataRecieveSemaphore_attributes = {
  .name = "dataRecieveSemaphore"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
/* USER CODE END FunctionPrototypes */

void Task_BlinkLed(void *argument);
void Task_UpdateOutcomingData(void *argument);
void Task_CheckDistance(void *argument);
void Task_CheckTemp(void *argument);
void Task_CheckBatteries(void *argument);
void Task_ExchangeDataWithRC(void *argument);
void Callback_UpdateManipulator(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName);
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

void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName)
{
    debug_logs((char *)pcTaskName);
    debug_logs(" : stack overflow\n");
    ErrorCritical_StackOverflow();
}
/* USER CODE END 4 */

/* USER CODE BEGIN 5 */

/** Если не хватило памяти в куче. */
void vApplicationMallocFailedHook(void)
{
    debug_logs("malloc failed\n");
    ErrorCritical_MallocFailed();
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
  /* creation of outcomingDataMutex */
  outcomingDataMutexHandle = osMutexNew(&outcomingDataMutex_attributes);

  /* creation of incomingDataMutex */
  incomingDataMutexHandle = osMutexNew(&incomingDataMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of dataRecieveSemaphore */
  dataRecieveSemaphoreHandle = osSemaphoreNew(1, 1, &dataRecieveSemaphore_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of manipulatorTimer */
  manipulatorTimerHandle = osTimerNew(Callback_UpdateManipulator, osTimerPeriodic, NULL, &manipulatorTimer_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of outcomingElementQueue */
  outcomingElementQueueHandle = osMessageQueueNew (6, sizeof(OutcomingElement_s), &outcomingElementQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of bld */
  bldHandle = osThreadNew(Task_BlinkLed, NULL, &bld_attributes);

  /* creation of uod */
  uodHandle = osThreadNew(Task_UpdateOutcomingData, NULL, &uod_attributes);

  /* creation of chd */
  chdHandle = osThreadNew(Task_CheckDistance, NULL, &chd_attributes);

  /* creation of cht */
  chtHandle = osThreadNew(Task_CheckTemp, NULL, &cht_attributes);

  /* creation of chb */
  chbHandle = osThreadNew(Task_CheckBatteries, NULL, &chb_attributes);

  /* creation of exd */
  exdHandle = osThreadNew(Task_ExchangeDataWithRC, NULL, &exd_attributes);

  /* USER CODE BEGIN RTOS_THREADS */

  osTimerStart(manipulatorTimerHandle, pdMS_TO_TICKS(70));
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_Task_BlinkLed */
/**
  * Задача, мигающая отладочным светодиодом.
  * Показывает, что робот в состоянии хотя бы помигать светодиодом.
  */
/* USER CODE END Header_Task_BlinkLed */
void Task_BlinkLed(void *argument)
{
  /* USER CODE BEGIN Task_BlinkLed */

    for (;;) {
        for (int_fast8_t i = 0; i < 2; i++) {
            Led_SetState(1);
            osDelay(pdMS_TO_TICKS(140));
            Led_SetState(0);
            osDelay(pdMS_TO_TICKS(70));
        }
        osDelay(pdMS_TO_TICKS(1500));
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
void Task_UpdateOutcomingData(void *argument)
{
  /* USER CODE BEGIN Task_UpdateOutcomingData */

    OutcomingElement_s element;

    for (;;) {
        osMessageQueueGet(outcomingElementQueueHandle, &element, 0, osWaitForever);

        if (osMutexAcquire(outcomingDataMutexHandle, 5) == osOK) {
            switch (element.kind)
            {
            case BACK_DISTANCE:
                outcomingData.status.bf.backDistance = element.udata;
                break;
            case BRAINS_CHARGE:
                outcomingData.brainsCharge = element.udata;
                break;
            case MOTORS_CHARGE:
                outcomingData.motorsCharge = element.udata;
                break;
            case AMBIENT_TEMPERATURE:
                outcomingData.ambientTemperature = element.sdata;
                break;
            case INTERNAL_TEMPERATURE:
                outcomingData.internalTemperature = element.sdata;
                break;
            }
            osMutexRelease(outcomingDataMutexHandle);
        } else {
            debug_logs("uod: odm failed\n");
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
void Task_CheckDistance(void *argument)
{
  /* USER CODE BEGIN Task_CheckDistance */

    OutcomingElement_s element;
    Distance_e distanceStatus;

    for (;;) {
        distanceStatus = Distance_GetBack();
        element.kind = BACK_DISTANCE;
        element.udata = distanceStatus;

        if (osMessageQueuePut(outcomingElementQueueHandle, &element, 0, 3) != osOK) {
            debug_logs("chd: oeq failed\n");
        }

        if (osMutexAcquire(incomingDataMutexHandle, 1) == osOK) {
            if (distanceStatus == DIST_CLIFF) {
                cliffBehindRobotDetected = true;
                if (Motors_GetDirection() == MOVEDIR_BACKWARD)
                    Motors_SetSpeed(0, 0);
            } else {
                cliffBehindRobotDetected = false;
            }
            osMutexRelease(incomingDataMutexHandle);
        } else {
            debug_logs("chd: idm failed\n");
        }

        osDelay(pdMS_TO_TICKS(200));
    }

  /* USER CODE END Task_CheckDistance */
}

/* USER CODE BEGIN Header_Task_CheckTemp */
/**
  * Задача, считывающая значение температуры
  * внутренних радиаторов и окружающей среды.
  */
/* USER CODE END Header_Task_CheckTemp */
void Task_CheckTemp(void *argument)
{
  /* USER CODE BEGIN Task_CheckTemp */

    OutcomingElement_s element;

    for (;;) {
        Temperature_StartMeasurement();
        osDelay(pdMS_TO_TICKS(1000));

        element.kind = INTERNAL_TEMPERATURE;
        element.sdata = Temperature_GetInternal();

        if (osMessageQueuePut(outcomingElementQueueHandle, &element, 0, 3) != osOK) {
            debug_logs("cht: oeq1 failed\n");
        }
        osDelay(pdMS_TO_TICKS(10));

        element.kind = AMBIENT_TEMPERATURE;
        element.sdata = Temperature_GetAmbient();

        if (osMessageQueuePut(outcomingElementQueueHandle, &element, 0, 3) != osOK) {
            debug_logs("cht: oeq2 failed\n");
        }
        osDelay(pdMS_TO_TICKS(1000));
    }

  /* USER CODE END Task_CheckTemp */
}

/* USER CODE BEGIN Header_Task_CheckBatteries */
/**
  * Задача, считывающая значение заряда аккумуляторов робота
  * с выходных узлов делителей напряжения.
  */
/* USER CODE END Header_Task_CheckBatteries */
void Task_CheckBatteries(void *argument)
{
  /* USER CODE BEGIN Task_CheckBatteries */

    OutcomingElement_s element;

    for (;;) {
        element.kind = BRAINS_CHARGE;
        element.udata = Battery_GetChargeBrains();

        if (osMessageQueuePut(outcomingElementQueueHandle, &element, 0, 3) != osOK) {
            debug_logs("chb: oeq1 failed\n");
        }
        osThreadYield();

        element.kind = MOTORS_CHARGE;
        element.udata = Battery_GetChargeMotors();

        if (osMessageQueuePut(outcomingElementQueueHandle, &element, 0, 3) != osOK) {
            debug_logs("chb: oeq2 failed\n");
        }
        osDelay(pdMS_TO_TICKS(3000));
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
void Task_ExchangeDataWithRC(void *argument)
{
  /* USER CODE BEGIN Task_ExchangeDataWithRC */

    DataToRobot_s incomingData = {
        .ctrl.reg = 0,
        .speedL = 0,
        .speedR = 0,
    };

    for (;;) {
        osSemaphoreAcquire(dataRecieveSemaphoreHandle, osWaitForever);
        Radio_TakeIncoming(&incomingData);

        if (osMutexAcquire(incomingDataMutexHandle, 10) == osOK) {
            Motors_SetDirection(incomingData.ctrl.bf.moveDir);
            if (!cliffBehindRobotDetected ||
                    incomingData.ctrl.bf.moveDir != MOVEDIR_BACKWARD)
                Motors_SetSpeed(incomingData.speedL, incomingData.speedR);

            Manipulator_SetArm(incomingData.ctrl.bf.armCtrl);
            Manipulator_SetClaw(incomingData.ctrl.bf.clawCtrl);

            Lights_SetState(incomingData.ctrl.bf.lightsEn);
            Buzzer_SetState(incomingData.ctrl.bf.buzzerEn);

            osMutexRelease(incomingDataMutexHandle);
        } else {
            debug_logs("exd: idm failed\n");
        }

        if (osMutexAcquire(outcomingDataMutexHandle, 5) == osOK) {
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

/* Callback_UpdateManipulator function */
void Callback_UpdateManipulator(void *argument)
{
  /* USER CODE BEGIN Callback_UpdateManipulator */

    /*
     * Чтобы не вызывать сильные просадки напряжения силового аккумулятора,
     * положение манипулятора обновляется плавно, шагами.
     */
    if (osMutexAcquire(incomingDataMutexHandle, 10) != osErrorOS) {
        Manipulator_Move();
        osMutexRelease(incomingDataMutexHandle);
    } else {
         debug_logs("um: idm failed\n");
    }

  /* USER CODE END Callback_UpdateManipulator */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
