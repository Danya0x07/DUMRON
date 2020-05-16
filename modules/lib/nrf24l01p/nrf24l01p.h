/**
 * Интерфейс для взаимодействия с радиомодулем NRF24L01+.
 */
#ifndef NRF24L01P_H_INCLUDED
#define NRF24L01P_H_INCLUDED

#include "main.h"

/* Команды радиомодуля */
typedef enum
{
    R_REGISTER = 0x00,   /* + n Прочитать регистр n */
    W_REGISTER = 0x20,   /* + n Записать регистр n */
    R_RX_PAYLOAD = 0x61, /* Принять данные данные из верхнего слота очереди приёмника. */
    W_TX_PAYLOAD = 0xA0, /* Записать в очередь передатчика данные для отправки */
    FLUSH_TX = 0xE1,     /* Сбросить очередь передатчика */
    FLUSH_RX = 0xE2,     /* Сбросить очередь приёмника */
    REUSE_TX_PL = 0xE3,  /* Использовать повторно последний переданный пакет */
    R_RX_PL_WID = 0x60,  /* Прочитать размер данных принятого пакета в начале очереди приёмника. */
    W_ACK_PAYLOAD = 0xA8,  /* + p Записать данные для отправки с пакетом подтверждения по каналу p. */
    W_TX_PAYLOAD_NOACK = 0xB0,  /* Записать в очередь передатчика данные, для отправки без подтверждения */
    NOP = 0xFF  /* Нет операции. Может быть использовано для чтения регистра статуса */
} NrfCommand;

/* Регистры радуомодуля */
typedef enum
{
    CONFIG = 0x00,  /* Регистр настроек */
    EN_AA = 0x01,   /* Выбор автоподтверждения */
    EN_RXADDR = 0x02,  /* Выбор каналов приёмника */
    SETUP_AW = 0x03,   /* Настройка размера адреса */
    SETUP_RETR = 0x04, /* Настройка повторной отправки */
    RF_CH = 0x05,  /* Номер радиоканала, на котором осуществляется работа. От 0 до 125. */
    RF_SETUP = 0x06,  /* Настройка радиоканала */
    STATUS = 0x07,    /* Регистр статуса. */
    OBSERVE_TX = 0x08,  /* Количество повторов передачи и потерянных пакетов */
    RPD = 0x09,  /* Мощность принимаемого сигнала. Если младший бит = 1, то уровень более -64dBm */
    RX_ADDR_P0 = 0x0A,  /* 3-5 байт (начиная с младшего байта). Адрес канала 0 приёмника. */
    RX_ADDR_P1 = 0x0B,  /* 3-5 байт (начиная с младшего байта). Адрес канала 1 приёмника. */
    RX_ADDR_P2 = 0x0C,  /* Младший байт адреса канала 2 приёмника. Старшие байты из RX_ADDR_P1 */
    RX_ADDR_P3 = 0x0D,  /* Младший байт адреса канала 3 приёмника. Старшие байты из RX_ADDR_P1 */
    RX_ADDR_P4 = 0x0E,  /* Младший байт адреса канала 4 приёмника. Старшие байты из RX_ADDR_P1 */
    RX_ADDR_P5 = 0x0F,  /* Младший байт адреса канала 5 приёмника. Старшие байты из RX_ADDR_P1 */
    TX_ADDR = 0x10,  /* 3-5 байт (начиная с младшего байта). Адрес удалённого устройства для передачи */
    RX_PW_P0 = 0x11,  /* Размер данных при приёме по каналу 0: от 1 до 32. 0 - канал не используется. */
    RX_PW_P1 = 0x12,  /* Размер данных при приёме по каналу 1: от 1 до 32. 0 - канал не используется. */
    RX_PW_P2 = 0x13,  /* Размер данных при приёме по каналу 2: от 1 до 32. 0 - канал не используется. */
    RX_PW_P3 = 0x14,  /* Размер данных при приёме по каналу 3: от 1 до 32. 0 - канал не используется. */
    RX_PW_P4 = 0x15,  /* Размер данных при приёме по каналу 4: от 1 до 32. 0 - канал не используется. */
    RX_PW_P5 = 0x16,  /* Размер данных при приёме по каналу 5: от 1 до 32. 0 - канал не используется. */
    FIFO_STATUS = 0x17, /* Состояние очередей FIFO приёмника и передатчика */
    DYNPD = 0x1C,   /* Выбор каналов приёмника для которых используется произвольная длина пакетов. */
    FEATURE = 0x1D  /* Регистр опций */
} NrfRegAddress;
/* Биты регистров */

/* CONFIG */
#define MASK_RX_DR  (1 << 6)  /* Запрещает прерывание по RX_DR (получение пакета) */
#define MASK_TX_DS  (1 << 5)  /* Запрещает прерывание по TX_DS (завершение отправки пакета) */
#define MASK_MAX_RT (1 << 4)  /* Запрещает прерывание по MAX_RT (превышение числа повторных попыток отправки) */
#define EN_CRC      (1 << 3)  /* Включает CRC */
#define CRCO        (1 << 2)  /* Размер поля CRC: 0 - 1 байт; 1 - 2 байта */
#define PWR_UP      (1 << 1)  /* Включение питания */
#define PRIM_RX     (1 << 0)  /* Выбор режима: 0 - PTX (передатчик) 1 - PRX (приёмник) */

/* EN_AA */
#define ENAA_P5 (1 << 5)  /* Включает автоподтверждение данных, полученных по каналу 5 */
#define ENAA_P4 (1 << 4)  /* Включает автоподтверждение данных, полученных по каналу 4 */
#define ENAA_P3 (1 << 3)  /* Включает автоподтверждение данных, полученных по каналу 3 */
#define ENAA_P2 (1 << 2)  /* Включает автоподтверждение данных, полученных по каналу 2 */
#define ENAA_P1 (1 << 1)  /* Включает автоподтверждение данных, полученных по каналу 1 */
#define ENAA_P0 (1 << 0)  /* Включает автоподтверждение данных, полученных по каналу 0 */

/* EN_RXADDR */
#define ERX_P5 (1 << 5)  /* Включает канал 5 приёмника */
#define ERX_P4 (1 << 4)  /* Включает канал 4 приёмника */
#define ERX_P3 (1 << 3)  /* Включает канал 3 приёмника */
#define ERX_P2 (1 << 2)  /* Включает канал 2 приёмника */
#define ERX_P1 (1 << 1)  /* Включает канал 1 приёмника */
#define ERX_P0 (1 << 0)  /* Включает канал 0 приёмника */

/* SETUP_AW */
#define AW 0  /* 2 бита, Выбирает ширину поля адреса: 1 - 3 байта; 2 - 4 байта; 3 - 5 байт. */

#define SETUP_AW_3BYTES_ADDRESS (1 << AW)
#define SETUP_AW_4BYTES_ADDRESS (2 << AW)
#define SETUP_AW_5BYTES_ADDRESS (3 << AW)

/* SETUP_RETR */
#define ARD 4  /* 4 бита. Задаёт значение задержки перед повторной отправкой пакета: 250 x (n + 1) мкс */
#define ARC 0  /* 4 бита. Количество повторных попыток отправки, 0 - повторная отправка отключена. */

#define SETUP_RETR_DELAY_250MKS  (0x00 << ARD)
#define SETUP_RETR_DELAY_500MKS  (0x01 << ARD)
#define SETUP_RETR_DELAY_750MKS  (0x02 << ARD)
#define SETUP_RETR_DELAY_1000MKS (0x03 << ARD)
#define SETUP_RETR_DELAY_1250MKS (0x04 << ARD)
#define SETUP_RETR_DELAY_1500MKS (0x05 << ARD)
#define SETUP_RETR_DELAY_1750MKS (0x06 << ARD)
#define SETUP_RETR_DELAY_2000MKS (0x07 << ARD)
#define SETUP_RETR_DELAY_2250MKS (0x08 << ARD)
#define SETUP_RETR_DELAY_2500MKS (0x09 << ARD)
#define SETUP_RETR_DELAY_2750MKS (0x0A << ARD)
#define SETUP_RETR_DELAY_3000MKS (0x0B << ARD)
#define SETUP_RETR_DELAY_3250MKS (0x0C << ARD)
#define SETUP_RETR_DELAY_3500MKS (0x0D << ARD)
#define SETUP_RETR_DELAY_3750MKS (0x0E << ARD)
#define SETUP_RETR_DELAY_4000MKS (0x0F << ARD)

#define SETUP_RETR_NO_RETRANSMIT       (0x00 << ARC)
#define SETUP_RETR_UP_TO_1_RETRANSMIT  (0x01 << ARC)
#define SETUP_RETR_UP_TO_2_RETRANSMIT  (0x02 << ARC)
#define SETUP_RETR_UP_TO_3_RETRANSMIT  (0x03 << ARC)
#define SETUP_RETR_UP_TO_4_RETRANSMIT  (0x04 << ARC)
#define SETUP_RETR_UP_TO_5_RETRANSMIT  (0x05 << ARC)
#define SETUP_RETR_UP_TO_6_RETRANSMIT  (0x06 << ARC)
#define SETUP_RETR_UP_TO_7_RETRANSMIT  (0x07 << ARC)
#define SETUP_RETR_UP_TO_8_RETRANSMIT  (0x08 << ARC)
#define SETUP_RETR_UP_TO_9_RETRANSMIT  (0x09 << ARC)
#define SETUP_RETR_UP_TO_10_RETRANSMIT (0x0A << ARC)
#define SETUP_RETR_UP_TO_11_RETRANSMIT (0x0B << ARC)
#define SETUP_RETR_UP_TO_12_RETRANSMIT (0x0C << ARC)
#define SETUP_RETR_UP_TO_13_RETRANSMIT (0x0D << ARC)
#define SETUP_RETR_UP_TO_14_RETRANSMIT (0x0E << ARC)
#define SETUP_RETR_UP_TO_15_RETRANSMIT (0x0F << ARC)

/* RF_SETUP */
#define CONT_WAVE  (1 << 7)  /* (Только для nRF24L01+) Непрерывная передача несущей (для тестов) */
#define PLL_LOCK   (1 << 4)  /* Для тестов */

#define RF_DR_LOW  (1 << 5)  /* (Только для nRF24L01+) Включает скорость 250кбит/с. RF_DR_HIGH должен быть 0 */
#define RF_DR_HIGH (1 << 3)  /* Выбор скорости обмена (при значении бита RF_DR_LOW = 0): 0 - 1Мбит/с; 1 - 2Мбит/с */
/* Режим 250kbps не должен использоваться с контролем доставки. */

#define RF_PWR 1  /* 2 бита. Выбирает мощность передатчика: 0 - -18dBm; 1 - -16dBm; 2 - -6dBm; 3 - 0dBm */
#define RF_SETUP_MINUS18DBM (0 << RF_PWR)
#define RF_SETUP_MINUS12DBM (1 << RF_PWR)
#define RF_SETUP_MINUS6DBM  (2 << RF_PWR)
#define RF_SETUP_0DBM       (3 << RF_PWR)

/* STATUS */
#define RX_DR   (1 << 6)  /* Флаг получения новых данных в FIFO приёмника. Для сброса флага нужно записать 1 */
#define TX_DS   (1 << 5)  /* Флаг завершения передачи. Для сброса флага нужно записать 1 */
#define MAX_RT  (1 << 4)  /* Флаг превышения установленного числа повторов. Без сброса (записать 1) обмен невозможен */
#define RX_P_NO (1 << 1)  /* 3 бита. Номер канала, данные для которого доступны в FIFO приёмника. 7 -  FIFO пусто. */
/* Признак заполнения FIFO передатчика: 1 - заполнено; 0 - есть доступные слоты
  (переименовано из TX_FULL во избежание путаницы с одноимённым битом из регистра FIFO_STATUS) */
#define TX_FULL_STATUS (1 << 0)

/* OBSERVE_TX */
#define PLOS_CNT 4  /* 4 бита. Общее количество пакетов без подтверждения. Сбрасывается записью RF_CH */
#define ARC_CNT  0  /* 4 бита. Количество предпринятых повторов при последней отправке. */

/* FIFO_STATUS */
#define TX_REUSE (1 << 6)  /* Признак готовности последнего пакета для повторной отправки. */
/* Флаг переполнения FIFO очереди передатчика.
  (переименовано из TX_FULL во избежание путаницы с одноимённым битом из регистра STATUS) */
#define TX_FULL_FIFO (1 << 5)
#define TX_EMPTY (1 << 4)  /* Флаг освобождения FIFO очереди передатчика. */
#define RX_FULL  (1 << 1)  /* Флаг переполнения FIFO очереди приёмника. */
#define RX_EMPTY (1 << 0)  /* Флаг освобождения FIFO очереди приёмника. */

/* DYNDP */
#define DPL_P5 (1 << 5)  /* Включает приём пакетов произвольной длины по каналу 5 */
#define DPL_P4 (1 << 4)  /* Включает приём пакетов произвольной длины по каналу 4 */
#define DPL_P3 (1 << 3)  /* Включает приём пакетов произвольной длины по каналу 3 */
#define DPL_P2 (1 << 2)  /* Включает приём пакетов произвольной длины по каналу 2 */
#define DPL_P1 (1 << 1)  /* Включает приём пакетов произвольной длины по каналу 1 */
#define DPL_P0 (1 << 0)  /* Включает приём пакетов произвольной длины по каналу 0 */

/* FEATURE */
#define EN_DPL     (1 << 2)  /* Включает поддержку приёма и передачи пакетов произвольной длины */
#define EN_ACK_PAY (1 << 1)  /* Разрешает передачу данных с пакетами подтверждения приёма */
#define EN_DYN_ACK (1 << 0)  /* Разрешает использование W_TX_PAYLOAD_NOACK */

typedef enum {
    NRF_OPERATION_READ,
    NRF_OPERATION_WRITE
} NrfOperation;

#define SET     1
#define RESET   0

uint8_t nrf_cmd(NrfCommand cmd);
uint8_t nrf_read_byte(NrfRegAddress reg_address);
void nrf_overwrite_byte(NrfRegAddress reg_address, uint8_t bitmask);
void nrf_apply_mask(NrfRegAddress reg_address, uint8_t bitmask, _Bool mask_status);
void nrf_rw_buff(uint8_t composite_cmd, uint8_t* buff, uint8_t size, NrfOperation operation);

#define nrf_get_status()  nrf_cmd(NOP)
#define nrf_clear_interrupts()  nrf_apply_mask(STATUS, 0, RESET)

#define nrf_csn_1() LL_GPIO_SetOutputPin(NRF_CSN_GPIO_Port, NRF_CSN_Pin)
#define nrf_csn_0() LL_GPIO_ResetOutputPin(NRF_CSN_GPIO_Port, NRF_CSN_Pin)
#define nrf_ce_1()  LL_GPIO_SetOutputPin(NRF_CE_GPIO_Port, NRF_CE_Pin)
#define nrf_ce_0()  LL_GPIO_ResetOutputPin(NRF_CE_GPIO_Port, NRF_CE_Pin)

#endif
