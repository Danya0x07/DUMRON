/**
 * Функции для низкоуровневого взаимодействия
 * с модулем NRF24L01+ по интерфейсной шине SPI.
 */
#include "nrf24l01.h"
#include "nrf24l01_port.h"

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
#ifndef NRF24L01_PLUS
    ACTIVATE = 0x50,
#endif
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
    RPD_CD = 0x09,  /* Мощность принимаемого сигнала. Если младший бит = 1, то уровень более -64dBm */
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

/* Настройка CRC. */
#define EN_CRC      (1 << 3)
#define CRC_2B      (1 << 2)

#define PWR_UP      (1 << 1)
#define PRIM_RX     (1 << 0)

/* RF_SETUP */

#ifdef NRF24L01_PLUS
    #define CONT_WAVE  (1 << 7)  /* (Только для nRF24L01+) Непрерывная передача несущей (для тестов) */
    #define RF_DR_LOW  (1 << 5)  /* (Только для nRF24L01+) Включает скорость 250кбит/с. RF_DR_HIGH должен быть 0 */
    #define RF_DR_HIGH (1 << 3)  /* Выбор скорости обмена (при значении бита RF_DR_LOW = 0): 0 - 1Мбит/с; 1 - 2Мбит/с */
    /* Режим 250kbps не должен использоваться с контролем доставки. */
#else
    #define RF_DR       (1 << 3)
    #define LNA_HCURR   (1 << 0)
#endif

#define PLL_LOCK    (1 << 4)  /* Для тестов */

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


static inline void nrf_read_to_buffer(uint8_t reg_addr, uint8_t* buff, uint8_t size)
{
    nrf_rw_buff(R_REGISTER | (reg_addr & 0x1F), buff, size, NRF_OPERATION_READ);
}

static inline void nrf_write_from_buffer(uint8_t reg_addr, uint8_t* buff, uint8_t size)
{
    nrf_rw_buff(W_REGISTER | (reg_addr & 0x1F), buff, size, NRF_OPERATION_WRITE);
}

/**
 * @brief  Отправляет простую команду радиомодулю.
 * @param  cmd: Команда радиомодулю.
 * @retval Значение регистра STATUS радиомодуля.
 */
uint8_t nrf_cmd(NrfCommand cmd)
{
    uint8_t status;
    nrf_csn_0();
    status = nrf_spi_send_recv(cmd);
    nrf_csn_1();
    return status;
}

/**
 * @brief  Читает значение из однобайтового регистра радиомодуля.
 * @param  reg_address: Адрес регистра откуда читать.
 * @retval Прочитанное значение.
 */
uint8_t nrf_read_byte(NrfRegAddress reg_address)
{
    uint8_t reg_value;
    nrf_csn_0();
    nrf_spi_send_recv(R_REGISTER | (reg_address & 0x1F));
    reg_value = nrf_spi_send_recv(NOP);
    nrf_csn_1();
    return reg_value;
}

/**
 * @brief  Записывает значение в однобайтовый регистр радиомодуля.
 * @note   При этом предыдущее значение этого регистра перезаписывается.
 * @param  reg_address: Адрес регистра куда писать.
 * @param  bitmask: Новое значение для регистра.
 */
void nrf_overwrite_byte(NrfRegAddress reg_address, uint8_t bitmask)
{
    nrf_csn_0();
    nrf_spi_send_recv(W_REGISTER | (reg_address & 0x1F));
    nrf_spi_send_recv(bitmask);
    nrf_csn_1();
}

/**
 * @brief  Делает read-modify-write с однобайтовым регистром радиомодуля.
 * @param  reg_address: Адрес регистра для применения маски.
 * @param  bitmask: Битовые флаги для применения к регистру.
 * @param  mask_status: Установить/сбросить указанные флаги.
 */
void nrf_apply_mask(NrfRegAddress reg_address, uint8_t bitmask, _Bool mask_status)
{
    uint8_t reg_value;
    nrf_read_to_buffer(reg_address, &reg_value, 1);
    if (mask_status)
        reg_value |= bitmask;
    else
        reg_value &= ~bitmask;
    nrf_write_from_buffer(reg_address, &reg_value, 1);
}

/**
 * @brief  Отправляет радиомодулю составную команду и
 *         отправляет или читает дополнительные данные.
 * @param  composite_cmd: Команда радиомодулю.
 * @param  buff: Указатель на буфер для отправки
 *         или сохранения дополнительных данных.
 * @param  size: Сколько байт доп. данных или ответа.
 * @param  operation: Отправлять доп. данные или принимать ответ.
 */
void nrf_rw_buff(uint8_t composite_cmd, uint8_t* buff, uint8_t size, NrfOperation operation)
{
    nrf_csn_0();
    nrf_spi_send_recv(composite_cmd);
    if (operation == NRF_OPERATION_READ)
        while (size--)
            *buff++ = nrf_spi_send_recv(NOP);
    else if (operation == NRF_OPERATION_WRITE)
        while (size--)
            nrf_spi_send_recv(*buff++);
    nrf_csn_1();
}
