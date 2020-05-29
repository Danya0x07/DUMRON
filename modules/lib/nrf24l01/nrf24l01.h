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
 *
 * @details для приёмника распространяется на все входящие соединения.
 * @note
 * Этот параметр у передатчика и приёмника должен быть одинаковым.
 */
enum nrf24l01_addr_size {
    NRF24L01_ADDRS_3BYTE = 0x01,
    NRF24L01_ADDRS_4BYTE = 0x02,
    NRF24L01_ADDRS_5BYTE = 0x03
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
    NRF24L01_CRC_OFF = 0,
    NRF24L01_CRC_1BYTE = 1 << 3,
    NRF24L01_CRC_2BYTE = 1 << 3 | 1 << 2
};

/**
 * @brief   Скорость обмена данными.
 *
 * @note
 * Этот параметр у передатчика и приёмника должен быть одинаковым.
 */
enum nrf24l01_datarate {
    NRF24L01_DATARATE_1MBPS = 0,
    NRF24L01_DATARATE_2MBPS = 1 << 3,
#ifdef NRF24L01_PLUS
    NRF24L01_DATARATE_250KBPS = 1 << 5
#endif
};

/** @brief   Мощность сигнала при передаче. */
enum nrf24l01_power {
    NRF24L01_POWER_N18DBM = 0 << 1,  /**< -18dBm */
    NRF24L01_POWER_N12DBM = 1 << 1,  /**< -12dBm */
    NRF24L01_POWER_N6DBM  = 2 << 1,  /**< -6dBm */
    NRF24L01_POWER_0DBM   = 3 << 1   /**<  0dBm */
};

/** @brief   Задержка между повторными отправками пакета. */
enum nrf24l01_retr_delay {
    NRF24L01_RETR_DELAY_250US  = 0x00,
    NRF24L01_RETR_DELAY_500US  = 0x10,
    NRF24L01_RETR_DELAY_750US  = 0x20,
    NRF24L01_RETR_DELAY_1000US = 0x30,
    NRF24L01_RETR_DELAY_1250US = 0x40,
    NRF24L01_RETR_DELAY_1500US = 0x50,
    NRF24L01_RETR_DELAY_1750US = 0x60,
    NRF24L01_RETR_DELAY_2000US  = 0x70,
    NRF24L01_RETR_DELAY_2250US = 0x80,
    NRF24L01_RETR_DELAY_2500US = 0x90,
    NRF24L01_RETR_DELAY_2750US = 0xA0,
    NRF24L01_RETR_DELAY_3000US = 0xB0,
    NRF24L01_RETR_DELAY_3250US = 0xC0,
    NRF24L01_RETR_DELAY_3500US = 0xD0,
    NRF24L01_RETR_DELAY_3750US = 0xE0,
    NRF24L01_RETR_DELAY_4000US = 0xF0
};

/** @brief   Количество повторных попыток отправки пакета. */
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
 * блокируется до тех пор, пока флаг прерывание не будет сброшен вызовом
 * nrf24l01_clear_interrupts().
 *
 * @{
 */
#define NRF24L01_IRQ_RX_DR  (1 << 6)
#define NRF24L01_IRQ_TX_DS  (1 << 5)
#define NRF24L01_IRQ_MAX_RT (1 << 4)

#define NRF24L01_IRQ_ALL    0x70
/** @} */

/**
 * @brief   Длительность неопределённого состояния трансивера
 *          после подачи питания.
 *
 * Пользователь должен выдержать данное время между включением системы
 * и обращением к API-функциям бибилотеки.
 */
#define NRF24L01_PWR_ON_DELAY_MS    100

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
 * Предназначены для заполнения атрибута nrf24l01_tx_config.features или
 * nrf24l01_rx_config.features.
 *
 * @{
 */

/** Включает у передатчика ожидание подтверждения после отправки пакета. */
#define NRF24L01_FEATURE_ACK    0x03

/** Если требуется принимать пакеты непостоянной длины. */
#define NRF24L01_FEATURE_DYNPL  0x04

/**
 * Если требуется возможность прикреплять полезную нагрузку к
 * пакетам автоподтверждения либо принимать такие пакеты.
 *
 * @attention
 * Использование полезной нагрузки с пакетами автоподтверждения автоматически
 * подразумевает использование @ref NRF24L01_FEATURE_ACK и
 * @ref NRF24L01_FEATURE_DYNPL. Передатчик должен проверять размер принятой
 * полезной нагрузки вызовом nrf24l01_read_pldnrf24l01_set_rf_channel(config->rf_channel);_size() перед считыванием,
 * чтобы убедиться в её валидности.
 */
#define NRF24L01_FEATURE_ACK_PLD    0x07
/** @} */

/**
 * @defgroup NRF24L01_PIPE_FEATURES
 *
 * Битовые маски для включения
 * расширенных возможностей для конкретного соединения.
 * Предназначены для заполнения атрибута nrf24l01_pipe_config.features.
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


/** @brief   Структура с конфигурацией передатчика. */
struct nrf24l01_tx_config {
    /**
     * Адрес соединения. Должен быть равен адресу одного из входящих соединений
     * приёмной стороны. Должен указывать на массив, размер которого
     * определяется атрибутом @ref addr_size и гарантируется пользователем.
     */
    uint8_t *address;
    enum nrf24l01_addr_size addr_size;
    enum nrf24l01_crc_mode crc_mode;
    enum nrf24l01_datarate datarate;
    enum nrf24l01_power power;
    enum nrf24l01_retr_delay retr_delay;
    enum nrf24l01_retr_count retr_count;
    /**
     * Включённые прерывания. Является маской из @ref NRF24L01_IRQ.
     * Определяет, в случае происхождения каких событий логический
     * уровень на пине IRQ трансивера будет сбасываться в 0.
     */
    uint8_t en_irq;
    /**
     * Радиочастотный канал. Должен находиться в пределе от 0 до
     * @ref NRF24L01_CHANNELS - 1. Должен совпадать с каналом приёмника.
     */
    uint8_t rf_channel;
    /**
     * Используемые возможности трансивера, относящиеся к расширенным.
     * Является маской из @ref NRF24L01_FEATURES.
     */
    uint8_t features;
};

/** @brief   Структура с конфигурацией приёмника. */
struct nrf24l01_rx_config {
    enum nrf24l01_addr_size addr_size;
    enum nrf24l01_crc_mode crc_mode;
    enum nrf24l01_datarate datarate;
    /**
     * Включённые прерывания. Является маской из @ref NRF24L01_IRQ.
     * Определяет, в случае происхождения каких событий логический
     * уровень на пине IRQ трансивера будет сбасываться в 0.
     */
    uint8_t en_irq;
    /**
     * Радиочастотный канал. Должен находиться в пределе от 0 до
     * @ref NRF24L01_CHANNELS - 1. Должен совпадать с каналом передатчика.
     */
    uint8_t rf_channel;
    /**
     * Используемые возможности трансивера, относящиеся к расширенным.
     * Является маской из @ref NRF24L01_FEATURES.
     */
    uint8_t features;
};

/**
 * @brief   Структура с настройками входящего соединения.
 *
 * @note
 * Подразумевается, что настройка соединений будет производиться после
 * инициализации трансивера, т.е. после создания и заполнения структуры
 * nrf24l01_rx_config и передачи её аргументом в функцию
 * nrf24l01_rx_configure().
 */
struct nrf24l01_pipe_config {
    /**
     * Адрес соединения. Для соединений 0 и 1 используется указатель
     * @ref array, который должен указывать на массив, размер которого
     * определяется атрибутом nrf24l01_rx_config.addr_size,
     * установленным ранее вызовом nrf24l01_rx_configure().
     * Для соединений 2-5 используется только младший байт - @ref lsb.
     * (Старшие байты наследуются от адреса соединения 1).
     */
    union {
        uint8_t *array;  /**< для соединений 0 и 1 */
        uint8_t lsb;     /**< для соединений 2-5 */
    } address;
    enum nrf24l01_pipe_number number;
    /**
     * Размер пакетов, принимаемых по этому соединению (от 1 до 32 байт).
     * Это поле не имеет значения, если используется хотя бы одна из
     * расширенных возможностей соединения.
     */
    uint8_t pld_size;
    /**
     * Используемые возможности соединения, относящиеся к расширенным.
     * Является битовой маской из @ref NRF24L01_PIPE_FEATURES.
     */
    uint8_t features;
};

#ifndef NRF24L01_PLUS
/**
 * @brief   Включает возможности стандартные возможности NRF24L01,
 *          которые у NRF24L01+ включены по умолчанию.
 *
 * @warning
 * Пользователь должен вызвать эту функцию ровно один раз в программе
 * до использования любых других API-функций библиотеки.
 */
void nrf24l01_activate(void);
#endif

/**
 * @defgroup NRF24L01_TRANSMITTER_FUNCTIONS
 *
 * Функции трансивера, которые есть смысл вызывать только в режиме передатчика.
 *
 * @{
 */

/**
 * @brief   Инициализирует трансивер как передатчик.
 *
 * @param config    Адрес структуры с конфигурацией передатчика.
 *
 * @return  0, если инициализация прошла успешно,
 * @return  -1, в противном случае (скорее всего плохой контакт).
 */
int nrf24l01_tx_configure(struct nrf24l01_tx_config *config);

/**
 * @brief   Инициализирует трансивер как передатчик настройками по-умолчанию.
 *
 * Минимальная конфигурация передатчика и приёмника позволяет протестировать
 * их работоспособность. Она подразумевает односторонний обмен данными с
 * подтверждениями и фиксированным размером полезной нагрузки.
 *
 * @attention
 * Данная функция будет корректно работать только если настройки трансивера
 * ранее не менялись (со времени подачи на него питания).
 *
 * @return  0, если инициализация прошла успешно,
 * @return  -1, в противном случае (скорее всего плохой контакт).
 */
int nrf24l01_tx_configure_minimal(void);

/**
 * @brief   Записывает в очередь отправки полезную нагрузку.
 *
 * @param pld   Указатель на начало полезной нагрузки.
 * @param size  Размер полезной нагрузки (от 1 до 32 байт).
 */
void nrf24l01_tx_write_pld(const void *pld, uint8_t size);

/**
 * @brief   Записывает в очередь отправки полезную нагрузку, помеченную
 *          как не требующую автоподтверждения.
 *
 * @note
 * Имеет смысл только если включены автоподтверждения.
 *
 * @param pld   Указатель на начало полезной нагрузки.
 * @param size  Размер полезной нагрузки (от 1 до 32 байт).
 */
void nrf24l01_tx_write_noack_pld(const void *pld, uint8_t size);

/**
 * @brief   Заставляет передатчик не удалять пакет из очереди отправки
 *          и при следующем вызове nrf24l01_tx_transmit() отправлять его же.
 * @note
 * Вернуть стандартное поведение при отправке можно вызовом
 * nrf24l01_tx_write_pld() или nrf24l01_flush_tx_fifo().
 */
void nrf24l01_tx_reuse_pld(void);

/**
 * @brief   Инициирует отправку первого в очереди пакета.
 *
 * @note
 * Функция удаляет отправленный пакет из очереди отправки, если до неё не было
 * включено повторное использование пакетов вызовом nrf24l01_tx_reuse_pld().
 *
 * @attention
 * Если ранее был включён режим постоянной отправки, он будет отключён.
 *
 * @see nrf24l01_tx_start_cont_transmission()
 */
void nrf24l01_tx_transmit(void);

/**
 * @brief   Включает у передатчика режим постоянной отправки.
 *
 * В этом режиме передатчик отправляет пакет, как только тот будет записан
 * в очередь отправки, не дожидаясь вызова nrf24l01_tx_transmit().
 * Таким образом очередь отправки постоянно опустошается.
 *
 * @see nrf24l01_tx_stop_cont_transmission()
 */
void nrf24l01_tx_start_cont_transmission(void);

/** @brief   Выключает режим постоянной отправки, если ранее он был включен. */
void nrf24l01_tx_stop_cont_transmission(void);

/**
 * @brief   Собирает статистику радиообмена.
 *
 * @param[out] lost Количество пакетов, для которых так и не пришло
 *                  автоподтверждение. Счётчик доходит до 15 и далее
 *                  останавливается. Сбрасывается при смене частотного канала.
 *
 * @param[out] retr Количество повторных отправок последнего пакета.
 *                  Счётчик сбрасывается при отправке каждого нового пакета.
 */
void nrf24l01_tx_get_statistics(uint8_t *lost, uint8_t *retr);
/** @} */

/**
 * @defgroup NRF24L01_RECIEVER_FUNCTIONS
 *
 * Функции трансивера, которые есть смысл вызывать только в режиме приёмника.
 *
 * @{
 */

/**
 * @brief   Инициализирует трансивер как приёмник.
 *
 * @param config    Адрес структуры с конфигурацией приёмника.
 *
 * @return  0, если инициализация прошла успешно,
 * @return  -1, в противном случае (скорее всего плохой контакт).
 */
int nrf24l01_rx_configure(struct nrf24l01_rx_config *config);

/**
 * @brief   Инициализирует трансивер как приёмник настройками по-умолчанию.
 *
 * Минимальная конфигурация приёмника и передатчика позволяет протестировать
 * их работоспособность. Она подразумевает односторонний обмен данными с
 * подтверждениями и фиксированным размером полезной нагрузки.
 *
 * @note
 * Эта функция сама настраивает входное соединение 0 для приёма данных от
 * передатчика. Все остальные настройки трансивера остаются в значениях по
 * умолчанию, указанных в даташите.
 *
 * @attention
 * Данная функция будет корректно работать только если настройки трансивера
 * ранее не менялись (со времени подачи на него питания).
 *
 * @param pld_size  Размер полезной нагрузки (от 1 до 32 байт).
 *                  Передатчик должен соблюдать такой же.
 *
 * @return  0, если инициализация прошла успешно,
 * @return  -1, в противном случае (скорее всего плохой контакт).
 */
int nrf24l01_rx_configure_minimal(uint8_t pld_size);

/**
 * @brief   Открывает и настраивает входящее соединение.
 *
 * @param config    Указатель на струкуру с конфигурацией соединения.
 */
void nrf24l01_rx_setup_pipe(struct nrf24l01_pipe_config *config);

/**
 * @brief   Закрывает входящее соединение.
 *
 * @note
 * Настройки соединения при этом не меняются.
 *
 * @param pipe_no   Номер соединения.
 */
void nrf24l01_rx_close_pipe(enum nrf24l01_pipe_number pipe_no);

/**
 * @brief   Открывает входящее соединение.
 *
 * @note
 * Вызов имеет смысл только если ранее соединение было закрыто,
 * но настроено.
 *
 * @param pipe_no   Номер соединения.
 *
 * @see nrf24l01_rx_setup_pipe(), nrf24l01_close_pipe()
 */
void nrf24l01_rx_open_pipe(enum nrf24l01_pipe_number pipe_no);

/**
 * @brief   Включает прослушивание эфира на предмет валидных пакетов.
 *
 * @see nrf24l01_rx_stop_listening()
 */
void nrf24l01_rx_start_listening(void);

/** @brief   Выключает прослушивание эфира на предмет валидных пакетов. */
void nrf24l01_rx_stop_listening(void);

/**
 * @brief   Получает номер соединения, полезная нагрузка для которого находится
 *          первой в очереди приёма.
 *
 * @return  Номер соединения из @ref nrf24l01_pipe_number,
 * @return  -1, если очередь приёма пуста.
 */
int nrf24l01_rx_get_pld_pipe_no(void);

/**
 * @brief   Записывает в очередь полезную нагрузку, для отправки вместе
 *          с пакетом автоподтверждения.
 * @note
 * Запись производится в очередь отправки, как если бы это был передатчик.
 *
 * @param pipeno    Номер соединения, для автоподтверждения пакетов по которому
 *                  будет записана полезная нагрузка.
 * @param pld   Указатель на начало полезной нагрузки.
 * @param size  Размер полезной нагрузки (от 1 до 32 байт).
 */
void nrf24l01_rx_write_ack_pld(enum nrf24l01_pipe_number pipeno,
                               const void *pld, uint8_t size);

#ifndef NRF24L01_PLUS
/**
 * @brief   Позволяет управлять встроенным усилителем (LNA) трансивера.
 *
 * Если выключить LNA, то чувствительность уменьшится на 1.5dBm,
 * а энергопотребление уменьшится на 0.8mA.
 *
 * @note
 * По умолчанию LNA включён
 *
 * @param state Состояние LNA: 1 - включено, 0 - выключено.
 */
void nrf24l01_rx_set_lna(bool state);
#endif
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
 * @brief   Выключает микросхему трансивера.
 *
 * @note
 * В выключенном состоянии трансивер можно только включить.
 *
 * @see nrf24l01_power_up()
 */
void nrf24l01_power_down(void);

/**
 * @brief   Включает микросхему трансивера.
 *
 * @note
 * Эту функцию имеет смысл вызывать, только если ранее был
 * вызов nrf24l01_power_down().
 */
void nrf24l01_power_up(void);

/**
 * @brief   Получает информацию о произошедших событиях трансивера.
 *
 * @return  Битовую маску из @ref NRF24L01_IRQ.
 *
 * @see nrf24l01_clear_interrupts()
 */
uint8_t nrf24l01_get_interrupts(void);

/**
 * @brief   Сбрасывает флаги произошедших событий трансивера.
 *
 * @param irq   Битовая маска из @ref NRF24L01_IRQ, содержащая прерывания,
 *              флаги которых нужно сбросить.
 */
void nrf24l01_clear_interrupts(uint8_t irq);

/**
 * @brief   Проверяет, есть ли в очереди отправки неотправленные пакеты.
 *
 * @return  true, если есть данные в очереди отправки,
 * @return  false, если очередь отправки пуста.
 */
bool nrf24l01_data_in_tx_fifo(void);

/**
 * @brief   Проверяет, есть ли принятые пакеты в очереди приёма.
 *
 * @return  true, если есть данные в очереди приёма,
 * @return  false, если очередь приёма пуста.
 */
bool nrf24l01_data_in_rx_fifo(void);

/**
 * @brief   Проверяет, заполнена ли очередь отправки.
 *
 * @note
 * Если в заполненную очередь отправки записать данные, то последний элемент
 * очереди будет перезаписан ими.
 *
 * @return  true, если очередь отправки заполнена,
 * @return  false, если в очереди отправки есть свободные слоты.
 */
bool nrf24l01_full_tx_fifo(void);

/**
 * @brief   Проверяет, заполнена ли очередь приёма.
 *
 * @note
 * Если очередь приёма заполнена, новые пакеты приниматься не будут.
 *
 * @return  true, если очередь приёма заполнена,
 * @return  false, если в очереди приёма есть свободные слоты.
 */
bool nrf24l01_full_rx_fifo(void);

/**
 * @brief   Считывает размер полезной нагрузки, находящейся первой
 *          в очереди приёма.
 *
 * Имеет смысл, если используется динамическая длина полезной нагрузки.
 *
 * @note
 * Имеет смысл как для приёмника, так и для передатчика: чтобы узнать
 * размер полезной нагрузки, принятой вместе с пакетом автоподтверждения.
 *
 * @return  Размер полезной нагрузки (от 1 до 32 байт).
 */
int nrf24l01_read_pld_size(void);

/**
 * @brief   Считывает принятую полезную нагрузку из очереди приёма, удаляя её
 *          из очереди.
 *
 * @param[out] pld  Указатель на буфер, куда будет записана полезная нагрузка.
 * @param size  Размер полезной нагрузки (от 1 до 32 байт).
 */
void nrf24l01_read_pld(void *pld, uint8_t size);

/** @brief   Очищает очередь отправки. */
void nrf24l01_flush_tx_fifo(void);

/** @brief   Очищает очередь приёма. */
void nrf24l01_flush_rx_fifo(void);

/**
 * @brief   Устанавливает радиочастотный канал трансивера.
 *
 * @param channel   Радиочастотный канал. Должен находиться в пределе
 *                  от 0 до @ref NRF24L01_CHANNELS - 1.
 */
void nrf24l01_set_rf_channel(uint8_t channel);
/** @} */

/**
 * @defgroup NRF24L01_TEST_UTILITY
 *
 * Функции трансивера, позволяющие анализировать работу радиосвязи и его самого.
 *
 * @{
 */

/**
 * @brief   Измеряет уровень шума на текущем канале.
 *
 * @note
 * Если трансивер находится в режиме передатчика, то для измерения
 * функция кратковременно переведёт его в режим приёмника, затем обратно.
 *
 * @return  true, если был обнаружен сигнал мощностью не менее -64dbm,
 * @return  false в противном случае.
 */
bool nrf24l01_detect_signal(void);

/**
 * @brief   Делает "снимок" зашумлённости частотного диапазона.
 *
 * 15 раз проходит по диапазону каналов, на каждом вызывая
 * nrf24l01_detect_signal(), соответственно, для каждого канала накапливается
 * значение от 0 до 0xF, показывающее его зашумлённость.
 *
 * @param[out] snapshot_buff    Массив, в который будут записаны значения
 *                              зашумлённости для каждого канала. Должен быть
 *                              размером не менее end_ch - start_ch + 1.
 *
 * @param start_ch  Начальный канал снимка. Должен быть в диапазоне от 0 до
 *                  @ref NRF_CHANNELS - 1.
 *
 * @param end_ch    Конечный канал снимка. Должен быть в диапазоне от 0 до
 *                  @ref NRF_CHANNELS - 1.
 *
 * @warning start_ch должен быть < end_ch.
 */
void nrf24l01_measure_noise(uint8_t *snapshot_buff,
                            uint8_t start_ch, uint8_t end_ch);

/**
 * @brief   Заставляет трансивер транслировать свою несущую волну.
 *
 * @warning
 * Данная функция должна использоваться только для тестирования трансивера
 * на предмет бракованности, она не совместима со стандартными режимами работы
 * и при вызове перезаписывает настройки на специальные для трансляции несущей.
 * Однако до неё можно вызвать nrf24l01_set_rf_channel(), чтобы установить
 * частотный канал, на котором будет трансляция.
 *
 * @param power Мощность сигнала, с которой нужно транслировать.
 *
 * @param channel   Радиочастотный канал, на котором нужно транслировать.
 *
 * @see nrf24l01_stop_output_carrier()
 */
void nrf24l01_start_output_carrier(enum nrf24l01_power power, uint8_t channel);

/**
 * @brief   Выключает трансляцию несущей.
 *
 * @note
 * Вызывать имеет смысл только после nrf24l01_start_output_carrier().
 */
#define nrf24l01_stop_output_carrier()  nrf24l01_stop_cont_transmission()
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01_H_INCLUDED */
