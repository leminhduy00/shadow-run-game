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
 *Pin map ssd1106
*****************************************************************************/
#define OLED_RST_PIN					(LL_GPIO_PIN_11)
#define OLED_RST_GPIO_PORT				(GPIOB)

#define OLED_DC_PIN						(LL_GPIO_PIN_5)
#define OLED_DC_GPIO_PORT				(GPIOA)

#define OLED_CS_PIN						(LL_GPIO_PIN_1)
#define OLED_CS_GPIO_PORT				(GPIOA)

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
* OLED IO function
*******************************************************************************/
extern void oled_io_init(void);

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


#ifdef __cplusplus
}
#endif

#endif //__IO_CFG_H__
