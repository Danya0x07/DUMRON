/**
 * @file
 * @brief   Драйвер трансиверов NRF24L01 и NRF24L01+.
 * @author  Danya0x07
 */

#ifndef NRF24L01_H_INCLUDED
#define NRF24L01_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf24l01_conf.h"

/**
 * @brief   Размер адреса соединений трансивера NRF24L01/+.
 * @details Распространяется на все соединения.
 * @note
 * Этот параметр у передатчика и приёмника должен быть одинаковым.
 */
enum nrf24l01_addr_size {
    NRF24L01_ADDRS_3BYTE = 1,
    NRF24L01_ADDRS_4BYTE = 2,
    NRF24L01_ADDRS_5BYTE = 3
};

/**
 * @brief   Настройки проверки трансивером контрольной суммы
 *          при получении пакета.
 *
 * Контрольная сумма может либо не проверяться, либо быть размером в 1 байт,
 * либо в 2 байта.
 *
 * @note
 * Этот параметр у передатчика и приёмника должен быть одинаковым.
 *
 * @attention
 * Если используется автоподтверждение хотя бы на одном соединении,
 * проверка контрольной суммы не может не использоваться.
 */
enum nrf24l01_crc_mode {
    NRF24L01_CRC_OFF,
    NRF24L01_CRC_1BYTE,
    NRF24L01_CRC_2BYTE
};

/**
 * @brief   Скорость обмена данными.
 *
 * @note
 * Этот параметр у передатчика и приёмника должен быть одинаковым.
 */
enum nrf24l01_datarate {
    NRF24L01_DATARATE_1MBPS,
    NRF24L01_DATARATE_2MBPS,
#ifdef NRF24L01_PLUS
    NRF24L01_DATARATE_250KBPS
#endif
};

/**
 * @brief   Мощность сигнала при передаче.
 */
enum nrf24l01_power {
    NRF24L01_POWER_N18DBM = 0 << 1,  /**< -18dbm */
    NRF24L01_POWER_N12DBM = 1 << 1,  /**< -12dbm */
    NRF24L01_POWER_N6DBM  = 2 << 1,  /**< -6dbm */
    NRF24L01_POWER_0DBM   = 3 << 1   /**<  0dbm */
};

/**
 * @brief   Задержка между повторными отправками пакета.
 */
enum nrf24l01_retr_delay {
    NRF24L01_RETR_DELAY_250US  = 0x00,
    NRF24L01_RETR_DELAY_500US  = 0x10,
    NRF24L01_RETR_DELAY_750US  = 0x20,
    NRF24L01_RETR_DELAY_1000US = 0x30,
    NRF24L01_RETR_DELAY_1000US = 0x40,
    NRF24L01_RETR_DELAY_1000US = 0x50,
    NRF24L01_RETR_DELAY_1000US = 0x60,
    NRF24L01_RETR_DELAY_750US  = 0x70,
    NRF24L01_RETR_DELAY_1000US = 0x80,
    NRF24L01_RETR_DELAY_1000US = 0x90,
    NRF24L01_RETR_DELAY_1000US = 0xA0,
    NRF24L01_RETR_DELAY_1000US = 0xB0,
    NRF24L01_RETR_DELAY_1000US = 0xC0,
    NRF24L01_RETR_DELAY_1000US = 0xD0,
    NRF24L01_RETR_DELAY_1000US = 0xE0,
    NRF24L01_RETR_DELAY_1000US = 0xF0
};

/**
 * @brief   Количество повторных попыток отправки пакета.
 */
enum nrf24l01_retr_count {
    NRF24L01_RETR_COUNT_0  = 0x00,
    NRF24L01_RETR_COUNT_1  = 0x01,
    NRF24L01_RETR_COUNT_2  = 0x02,
    NRF24L01_RETR_COUNT_3  = 0x03,
    NRF24L01_RETR_COUNT_4  = 0x04,
    NRF24L01_RETR_COUNT_5  = 0x05,
    NRF24L01_RETR_COUNT_6  = 0x06,
    NRF24L01_RETR_COUNT_7  = 0x07,
    NRF24L01_RETR_COUNT_8  = 0x08,
    NRF24L01_RETR_COUNT_9  = 0x09,
    NRF24L01_RETR_COUNT_10 = 0x0A,
    NRF24L01_RETR_COUNT_11 = 0x0B,
    NRF24L01_RETR_COUNT_12 = 0x0C,
    NRF24L01_RETR_COUNT_13 = 0x0D,
    NRF24L01_RETR_COUNT_14 = 0x0E,
    NRF24L01_RETR_COUNT_15 = 0x0F
};

/**
 * @brief   Номер входящего соединения.
 *
 * 6 входящих соединений позволяют приёмнику принимать пакеты
 * от 6-ти передатчиков, работающих на одинаковой частоте с ним.
 */
enum nrf24l01_pipe_number {
    NRF24L01_PIPE0 = 0,
    NRF24L01_PIPE1,
    NRF24L01_PIPE2,
    NRF24L01_PIPE3,
    NRF24L01_PIPE4,
    NRF24L01_PIPE5,
};

/**
 * @defgroup NRF24L01_IRQ
 *
 * Прерывания трансивера NRF24L01/+.
 *
 * У данных радиомодулей есть 3 прерывания: по приёму пакета(RX_DR),
 * по успешной отправке пакета(TX_DS) и по истечению максимального количества
 * повторных попыток отправки(MAX_RT). С помощью битовой маски можно выбирать,
 * на какие их них реагировать, а на какие нет. Реакция заключается в
 * спаде напряжения на пине IRQ трансивера.
 *
 * @note
 * Для приёмника обычно характерно только прерывание RX_DR.
 * Для передатчика, в случае с трансивером NRF24L01 - TX_DS и MAX_RT,
 * а в случае с NRF24L01+ - ещё и RX_DR (если используется возможность
 * прикрепления полезной нагрузки к пакетам автоподтверждения).
 *
 * @attention
 * В случае происхождения прерывания MAX_RT, дальнейшая отправка данных
 * блокируется до тех пор, пока флаг прерывание не будет сброшен.
 *
 * @{
 */
#define NRF24L01_IRQ_RX_DR  (1 << 6)
#define NRF24L01_IRQ_TX_DS  (1 << 5)
#define NRF24L01_IRQ_MAX_RT (1 << 4)
/** @} */

/**
 * @brief   Количество радиочастотных каналов, на которых может работать
 *          трансивер NRF24L01/+.
 */
#define NRF24L01_CHANNELS   126

/**
 * @defgroup NRF24L01_FEATURES
 *
 * Битовые маски для включения
 * расширенных возможностей трансивера NRF24L01/+.
 * Предназначены для заполнения атрибута @ref nrf24l01_tx_config.features или
 * @ref nrf24l01_rx_config.features.
 *
 * @{
 */

/** Если требуется работать с пакетами непостоянной длины. */
#define NRF24L01_FEATURE_DYNPL   0x04

/**
 * Если требуется возможность прикреплять полезную нагрузку к
 * пакетам автоподтверждения либо принимать такие пакеты.
 */
#define NRF24L01_FEATURE_ACK_PLD 0x07
/** @} */

/**
 * @defgroup NRF24L01_PIPE_FEATURES
 *
 * Битовые маски для включения
 * расширенных возможностей для конкретного соединения.
 * Предназначены для заполнения атрибута @ref nrf24l01_pipe_config.features.
 *
 * @{
 */

/** Если размер пакетов, приходящих по этому соединению, варьируется. */
#define NRF24L01_PIPE_FEATURE_DYNPL 0x01

/**
 * Если для пакетов, приходящих по этому соединению,
 * нужно высылать автоподтверждения.
 */
#define NRF24L01_PIPE_FEATURE_ACK   0x02
/** @} */


/**
 * @brief   Структура с конфигурацией передатчика.
 *
 * @ref address     Адрес соединения. Должен быть равен адресу одного
 *                  из входящих соединений приёмной стороны.
 *                  Должен указывать на массив, размер которого определяется
 *                  атрибутом @ref addr_size и гарантируется пользователем.
 *
 * @ref en_irq      Включённые прерывания. Является маской из @ref NRF24L01_IRQ.
 *                  Определяет, в случае происхождения каких событий логический
 *                  уровень на пине IRQ трансивера будет сбрасываться в 0.
 *
 * @ref rf_channel  Радиочастотный канал. Должен находиться в пределе
 *                  от 0 до @ref NRF24L01_CHANNELS - 1. Должен совпадать
 *                  с каналом приёмника.
 *
 * @ref features    Используемые возможности трансивера, относящиеся к
 *                  расширенным. Является маской из @ref NRF24L01_FEATURES.
 */
struct nrf24l01_tx_config {
    uint8_t *address;
    enum nrf24l01_addr_size addr_size;
    enum nrf24l01_crc_mode crc_mode;
    enum nrf24l01_datarate datarate;
    enum nrf24l01_power power;
    enum nrf24l01_retr_delay retr_delay;
    enum nrf24l01_retr_count retr_count;
    uint8_t en_irq;
    uint8_t rf_channel;
    uint8_t features;
};

/**
 * @brief   Структура с конфигурацией приёмника.
 *
 * @ref en_irq  Включённые прерывания. Является маской из @ref NRF24L01_IRQ.
 *              Определяет, в случае происхождения каких событий логический
 *              уровень на пине IRQ трансивера будет сбасываться в 0.
 *
 * @ref rf_channel  Радиочастотный канал. Должен находиться в пределе
 *                  от 0 до @ref NRF24L01_CHANNELS - 1. Должен совпадать
 *                  с каналом передатчика.
 *
 * @ref features    Используемые возможности трансивера, относящиеся к
 *                  расширенным. Является маской из @ref NRF24L01_FEATURES.
 */
struct nrf24l01_rx_config {
    enum nrf24l01_addr_size addr_size;
    enum nrf24l01_crc_mode crc_mode;
    enum nrf24l01_datarate datarate;
    uint8_t en_irq;
    uint8_t rf_channel;
    uint8_t features;
};

/**
 * @brief   Структура с настройками входящего соединения.
 *
 * @note
 * Подразумевается, что настройка соединений будет производиться после
 * инициализации трансивера, т.е. после создания и заполнения структуры
 * @ref nrf24l01_rx_config и передачи её аргументом в функцию
 * @ref nrf24l01_rx_configure.
 *
 * @ref address     Адрес соединения. Для соединений 0 и 1 используется
 *                  указатель @ref array, который должен указывать на массив,
 *                  размер которого определяется атрибутом
 *                  @ref nrf24l01_rx_config.addr_size, установленным ранее
 *                  вызовом @ref nrf24l01_rx_configure. Для соединений 2-5
 *                  используется только младший байт - @ref lsb.
 *                  (Старшие байты наследуются от соединения 1).
 *
 * @ref pld_size    Размер пакетов, принимаемых по этому соединению.
 *                  Должен находиться в пределах от 0 до 32 байт.
 *                  Это поле не имеет значения, если используется хотя бы
 *                  одна из расширенных возможностей соединения.
 *
 * @ref features    Используемые возможности соединения, относящиеся к
 *                  расширенным. Является битовой маской из
 *                  @ref NRF24L01_PIPE_FEATURES.
 */
struct nrf24l01_pipe_config {
    union {
        uint8_t *array;  /**< для соединений 0 и 1 */
        uint8_t lsb;     /**< для соединений 2-5 */
    } address;
    enum nrf24l01_pipe_number number;
    uint8_t pld_size;
    uint8_t features;
};

/**
 * @defgroup NRF24L01_TRANSMITTER_FUNCTIONS
 *
 * Функции трансивера, которые есть смысл вызывать только в режиме передатчика.
 *
 * @{
 */

/**
 * @brief   Инициализирует трансивер как передатчик.
 * @param config    Адрес структуры с конфигурацией передатчика.
 * @return
 */
int nrf24l01_tx_configure(struct nrf24l01_tx_config *config);

/**
 * @brief   Инициализирует трансивер как передатчик настройками по-умолчанию.
 *
 * @return
 */
int nrf24l01_tx_configure_default(void);

/**
 * @brief
 */
int nrf24l01_tx_write_pld(const void *pld, uint8_t size);

/**
 * @brief
 */
int nrf24l01_tx_write_noack_pld(const void *pld, uint8_t size);

/**
 * @brief
 */
void nrf24l01_tx_reuse_pld(void);

/**
 * @brief
 */
void nrf24l01_tx_transmit(void);
/** @} */

/**
 * @defgroup NRF24L01_RECIEVER_FUNCTIONS
 *
 * Функции трансивера, которые есть смысл вызывать только в режиме приёмника.
 *
 * @{
 */

/**
 * @brief
 */
int nrf24l01_rx_configure(struct nrf24l01_rx_config *config);

/**
 * @brief
 */
int nrf24l01_rx_configure_default(void);

/**
 * @brief
 */
void nrf24l01_rx_setup_pipe(struct nrf24l01_pipe_config *config);

/**
 * @brief
 */
int nrf24l01_rx_write_ack_pld(enum nrf24l01_pipe_number pipeno,
                              const void *pld, uint8_t size);

/** @} */

/**
 * @defgroup NRF24L01_COMMON_FUNCTIONS
 *
 * Функции трансивера, которые есть смысл вызывать как в режиме передатчика,
 * так и в режиме приёмника.
 *
 * @{
 */

/**
 * @brief
 */
void nrf24l01_flush_out_fifo(void);

/**
 * @brief
 */
void nrf24l01_flush_in_fifo(void);

/**
 * @brief
 */
int nrf24l01_read_pld_size(void);

/**
 * @brief
 */
int nrf24l01_read_pld(const void *pld, uint8_t size);

/**
 * @brief
 */
void nrf24l01_set_rf_channel(uint8_t channel);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01_H_INCLUDED */
