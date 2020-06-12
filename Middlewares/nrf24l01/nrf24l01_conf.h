/**
 * @file    nrf24l01_conf.h
 * @brief   Файл с настройками библиотеки.
 *
 * Данный файл предназначен адаптировать библиотеку под конкретный проект.
 */

#ifndef _NRF24L01_CONF_H
#define _NRF24L01_CONF_H

/** 1 для NRF24L01+, 0 для NRF24L01. */
#define NRF24L01_PLUS   1

/** 1 если микросекундная задержка доступна, 0 если нет. */
#define NRF24L01_DELAY_US_AVAILABLE  1

#endif /* _NRF24L01_CONF_H */
