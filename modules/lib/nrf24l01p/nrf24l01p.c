/**
 * Функции для низкоуровневого взаимодействия
 * с модулем NRF24L01+ по интерфейсной шине SPI.
 */
#include "main.h"

#include "nrf24l01p.h"

static uint8_t nrf_spi_send_recv(uint8_t);

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
    while (size--) {
        if (operation == NRF_OPERATION_READ)
            *buff++ = nrf_spi_send_recv(NOP);
        else if (operation == NRF_OPERATION_WRITE)
            nrf_spi_send_recv(*buff++);
    }
    nrf_csn_1();
}

/**
 * @brief  Передаёт радиомодулю 1 байт по SPI и принимает ответный.
 * @note   Эта функция платформозависимая.
 * @param  byte: байт для передачи радиомодулю.
 * @retval Ответный байт от радиомодуля.
 */
static uint8_t nrf_spi_send_recv(uint8_t byte)
{
    while (!LL_SPI_IsActiveFlag_TXE(SPI1))
        ;
    LL_SPI_TransmitData8(SPI1, byte);
    while (!LL_SPI_IsActiveFlag_RXNE(SPI1))
        ;
    return LL_SPI_ReceiveData8(SPI1);
}
