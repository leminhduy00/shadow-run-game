/**
 ******************************************************************************
 * @author: GaoKong
 * @date:   05/09/2016
 ******************************************************************************
**/
#ifndef __IO_CFG_H__
#define __IO_CFG_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <string.h>

#include "stm32h7xx_hal.h"
#include "system_stm32h7xx.h"
#include "core_cm7.h"
#include "cmsis_gcc.h"

#include "stm32h7xx_ll_gpio.h"
#include "stm32h7xx_ll_bus.h"
/*
 * define pin for arduino pinMode/digitalWrite/digitalRead
 * NOTE: define value MUST be deferrent
 */

/******************************************************************************
 *Pin map button
*******************************************************************************/
#define BUTTON_DOWN_IO_PIN				(LL_GPIO_PIN_4)
#define BUTTON_DOWN_IO_PORT				(GPIOC)

#define BUTTON_UP_IO_PIN				(LL_GPIO_PIN_5)
#define BUTTON_UP_IO_PORT				(GPIOC)

#define BUTTON_MODE_IO_PIN				(LL_GPIO_PIN_0)
#define BUTTON_MODE_IO_PORT				(GPIOB)

/*****************************************************************************
 *Pin map led life
******************************************************************************/
#define LED_LIFE_IO_PIN					(LL_GPIO_PIN_3)
#define LED_LIFE_IO_PORT				(GPIOE)

/*****************************************************************************
 *Pin map buzzer
******************************************************************************/

#define BUZZER_IO_PIN					(LL_GPIO_PIN_7)
#define BUZZER_IO_PORT					(GPIOA)
#define BUZZER_TIM                      (TIM3)

/*****************************************************************************
 *Pin map Flash W25q64
******************************************************************************/

#define FLASH_CE_IO_PIN					(LL_GPIO_PIN_6)
#define FLASH_CE_IO_PORT				(GPIOD)

/****************************************************************************
 *Pin map OLED (SPI interface)
*****************************************************************************/
#define OLED_SCK_IO_PIN					(LL_GPIO_PIN_10)
#define OLED_SCK_IO_PORT				(GPIOB)

#define OLED_MOSI_IO_PIN				(LL_GPIO_PIN_1)
#define OLED_MOSI_IO_PORT				(GPIOC)

#define OLED_CS_IO_PIN					(LL_GPIO_PIN_1)
#define OLED_CS_IO_PORT					(GPIOA)

#define OLED_DC_IO_PIN					(LL_GPIO_PIN_5)
#define OLED_DC_IO_PORT					(GPIOA)

#define OLED_RST_IO_PIN					(LL_GPIO_PIN_11)
#define OLED_RST_IO_PORT				(GPIOB)

/* Arduino-style pin defines for wiring_digital.cpp */
#define OLED_SPI_SCK_PIN        (0x01)  // SPI Clock
#define OLED_SPI_MOSI_PIN       (0x02)  // SPI MOSI/Data
#define OLED_DC_PIN             (0x04)  // Data/Command pin
#define OLED_RES_PIN            (0x08)  // Reset pin
#define OLED_CS_PIN             (0x10)  // Chip Select pin

/******************************************************************************
* button function
*******************************************************************************/
extern void io_button_mode_init();
extern void io_button_up_init();
extern void io_button_down_init();

extern uint8_t io_button_mode_read();
extern uint8_t io_button_up_read();
extern uint8_t io_button_down_read();

/******************************************************************************
* led status function
*******************************************************************************/
extern void led_life_init();
extern void led_life_on();
extern void led_life_off();

/******************************************************************************
* flash IO function
*******************************************************************************/
extern void flash_io_ctrl_init();
extern void flash_cs_low();
extern void flash_cs_high();
extern uint8_t flash_transfer(uint8_t);

/******************************************************************************
* adc function
* + themistor sensor
*
* Note: MUST be enable internal clock for adc module.
*******************************************************************************/
/* configure adc peripheral */
extern void io_cfg_adc1(void);

/* adc configure for CT sensor */
extern void adc_bat_io_cfg();
extern uint16_t adc_bat_io_read(uint8_t);

/******************************************************************************
* OLED IO function
*******************************************************************************/
/* OLED SCK */
extern void oled_sck_input_mode();
extern void oled_sck_output_mode();
extern void oled_sck_digital_write_low();
extern void oled_sck_digital_write_high();
extern int  oled_sck_digital_read();

/* OLED MOSI */
extern void oled_mosi_input_mode();
extern void oled_mosi_output_mode();
extern void oled_mosi_digital_write_low();
extern void oled_mosi_digital_write_high();
extern int  oled_mosi_digital_read();

/* OLED CS */
extern void oled_cs_input_mode();
extern void oled_cs_output_mode();
extern void oled_cs_digital_write_low();
extern void oled_cs_digital_write_high();
extern int  oled_cs_digital_read();

/* OLED DC */
extern void oled_dc_input_mode();
extern void oled_dc_output_mode();
extern void oled_dc_digital_write_low();
extern void oled_dc_digital_write_high();
extern int  oled_dc_digital_read();

/* OLED RST */
extern void oled_rst_input_mode();
extern void oled_rst_output_mode();
extern void oled_rst_digital_write_low();
extern void oled_rst_digital_write_high();
extern int  oled_rst_digital_read();

/******************************************************************************
* eeprom function
*******************************************************************************/
extern uint8_t io_eeprom_read(uint32_t, uint8_t*, uint32_t);
extern uint8_t io_eeprom_write(uint32_t, uint8_t*, uint32_t);
extern uint8_t io_eeprom_erase(uint32_t, uint32_t);

/******************************************************************************
* internal flash function
*******************************************************************************/
extern void internal_flash_unlock();
extern void internal_flash_lock();
extern void internal_flash_erase_pages_cal(uint32_t address, uint32_t len);
extern uint8_t internal_flash_write_cal(uint32_t address, uint8_t* data, uint32_t len);

/******************************************************************************
* uart2 function
*******************************************************************************/
void io_uart2_cfg();
void io_uart2_putc(uint8_t);
extern uint8_t io_uart2_getc();

/*****************************************************************************
 *io uart for rs485-modbusRTU
 * see more in mbportserial.c
******************************************************************************/
extern void io_uart_rs485_cfg();
extern void io_rs485_dir_mode_output();
extern void io_rs485_dir_low();
extern void io_rs485_dir_high();

#ifdef __cplusplus
}
#endif

#endif //__IO_CFG_H__
