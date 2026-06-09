/**
 ******************************************************************************
 * @author: GaoKong
 * @date:   05/09/2016
 ******************************************************************************
 **/
#include <stdint.h>
#include <stdbool.h>

#include "io_cfg.h"
#include "spi.h"
#include "sys_dbg.h"

#include "system.h"

// #pragma GCC optimize ("O3")

/******************************************************************************
 * button function
 *******************************************************************************/
void io_button_mode_init()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOA);

    GPIO_InitStruct.Pin = BUTTON_MODE_IO_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;

    LL_GPIO_Init(BUTTON_MODE_IO_PORT, &GPIO_InitStruct);
}

void io_button_up_init()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOC);

    GPIO_InitStruct.Pin = BUTTON_UP_IO_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;

    LL_GPIO_Init(BUTTON_UP_IO_PORT, &GPIO_InitStruct);
}

void io_button_down_init()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOD);

    GPIO_InitStruct.Pin = BUTTON_DOWN_IO_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;

    LL_GPIO_Init(BUTTON_DOWN_IO_PORT, &GPIO_InitStruct);
}

uint8_t io_button_mode_read()
{
    return (uint8_t)LL_GPIO_IsInputPinSet(BUTTON_MODE_IO_PORT, BUTTON_MODE_IO_PIN);
}

uint8_t io_button_up_read()
{
    return (uint8_t)LL_GPIO_IsInputPinSet(BUTTON_UP_IO_PORT, BUTTON_UP_IO_PIN);
}

uint8_t io_button_down_read()
{
    return (uint8_t)LL_GPIO_IsInputPinSet(BUTTON_DOWN_IO_PORT, BUTTON_DOWN_IO_PIN);
}

/******************************************************************************
 * led status function
 *******************************************************************************/
void led_life_init()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOE);

    LL_GPIO_ResetOutputPin(LED_LIFE_IO_PORT, LED_LIFE_IO_PIN);

    GPIO_InitStruct.Pin = LED_LIFE_IO_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;

    LL_GPIO_Init(LED_LIFE_IO_PORT, &GPIO_InitStruct);
}

void led_life_on()
{
    LL_GPIO_SetOutputPin(LED_LIFE_IO_PORT, LED_LIFE_IO_PIN);
}

void led_life_off()
{
    LL_GPIO_ResetOutputPin(LED_LIFE_IO_PORT, LED_LIFE_IO_PIN);
}

/******************************************************************************
 * flash IO config
 *******************************************************************************/
void flash_io_ctrl_init()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOD);

    LL_GPIO_ResetOutputPin(FLASH_CE_IO_PORT, FLASH_CE_IO_PIN);

    GPIO_InitStruct.Pin = FLASH_CE_IO_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(FLASH_CE_IO_PORT, &GPIO_InitStruct);
}

void flash_cs_low()
{
    LL_GPIO_ResetOutputPin(FLASH_CE_IO_PORT, FLASH_CE_IO_PIN);
}

void flash_cs_high()
{
    LL_GPIO_SetOutputPin(FLASH_CE_IO_PORT, FLASH_CE_IO_PIN);
}

uint8_t flash_transfer(uint8_t data)
{
    uint8_t rx_data;

   HAL_SPI_TransmitReceive(&hspi1, &data, &rx_data, 1, 100);

    return rx_data;
}

/******************************************************************************
 * eeprom function
 * when using function DATA_EEPROM_ProgramByte,
 * must be DATA_EEPROM_unlock- DATA_EEPROM_ProgramByte- DATA_EEPROM_lock
 *******************************************************************************/
uint8_t io_eeprom_read(uint32_t address, uint8_t *pbuf, uint32_t len)
{
    /*    UNUSED    */
    (void)address;
    (void)pbuf;
    (void)len;
    return 0;
}

uint8_t io_eeprom_write(uint32_t address, uint8_t *pbuf, uint32_t len)
{
    /*    UNUSED    */
    (void)address;
    (void)pbuf;
    (void)len;
    return 0;
}

uint8_t io_eeprom_erase(uint32_t address, uint32_t len)
{
    /*    UNUSED    */
    (void)address;
    (void)len;
    return 0;
}

void internal_flash_unlock()
{
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS_BANK1 | FLASH_FLAG_ALL_ERRORS_BANK2);
}

void internal_flash_lock()
{
    HAL_FLASH_Lock();
}

void internal_flash_erase_pages_cal(uint32_t address, uint32_t len)
{
    uint32_t sector_number;
    FLASH_EraseInitTypeDef erase_init;
    uint32_t sector_error;

    sector_number = len / (128 * 1024);
    if ((sector_number * 128 * 1042) < len)
        sector_number++;

    uint32_t first_sector = (address - FLASH_BASE) / (128 * 1024);

    erase_init.TypeErase    = FLASH_TYPEERASE_SECTORS;
    erase_init.Banks        = FLASH_BANK_1;
    erase_init.Sector       = first_sector;
    erase_init.NbSectors    = sector_number;
    erase_init.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    HAL_FLASHEx_Erase(&erase_init, &sector_error);
}

uint8_t internal_flash_write_cal(uint32_t address, uint8_t *data, uint32_t len)
{
    HAL_StatusTypeDef flash_status;
    uint32_t aligned_len = ((len + 31) / 32) + 32;
    uint8_t flash_word[32];
    uint32_t index = 0;

    while (index < aligned_len)
    {
        memset(flash_word, 0xFF, 32);
        uint32_t copy_len = ((len - index) > 32) ? 32 : (len - index);

        if (index < len)
            memcpy(flash_word, &data[index], copy_len);

        flash_status = HAL_FLASH_Program(
                        FLASH_TYPEPROGRAM_FLASHWORD,
                        address + index,
                        (uint32_t)flash_word);



        if (flash_status == HAL_OK)
        {
            index += sizeof(uint32_t);
        }
        else
        {
            __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGSERR);

            return 0;
        }
    }

    return 1;
}
