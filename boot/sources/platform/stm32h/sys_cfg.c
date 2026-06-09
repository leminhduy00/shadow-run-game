/**
 ******************************************************************************
 * @Author: GaoKong
 * @Date:   05/09/2016
 ******************************************************************************
**/
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "sys_cfg.h"
#include "system.h"

#include "sys_dbg.h"
#include "sys_def.h"
#include "sys_ctrl.h"

#include "system.h"

#include "app.h"

#include "iwdg.h"
#include "usart.h"

#include "stm32h7xx_ll_system.h"
#include "stm32h7xx_ll_pwr.h"
#include "stm32h7xx_ll_cortex.h"
#include "stm32h7xx_ll_utils.h"
#include "stm32h7xx_ll_rcc.h"

/* Private define */
static uint32_t delay_coeficient = 0;
static void xputchar(uint8_t c);

/******************************************************************************
* system configure function
*******************************************************************************/
void sys_cfg_clock() {
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
    while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_4)
    {
    }
    LL_PWR_ConfigSupply(LL_PWR_LDO_SUPPLY);
    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE3);
    while (LL_PWR_IsActiveFlag_VOS() == 0)
    {
    }
    LL_RCC_HSE_Enable();

    /* Wait till HSE is ready */
    while(LL_RCC_HSE_IsReady() != 1)
    {

    }
    LL_RCC_LSI_Enable();

    /* Wait till LSI is ready */
    while(LL_RCC_LSI_IsReady() != 1)
    {

    }
    LL_RCC_PLL_SetSource(LL_RCC_PLLSOURCE_HSE);
    LL_RCC_PLL1P_Enable();
    LL_RCC_PLL1Q_Enable();
    LL_RCC_PLL1R_Enable();
    LL_RCC_PLL1_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_8_16);
    LL_RCC_PLL1_SetVCOOutputRange(LL_RCC_PLLVCORANGE_WIDE);
    LL_RCC_PLL1_SetM(2);
    LL_RCC_PLL1_SetN(32);
    LL_RCC_PLL1_SetP(2);
    LL_RCC_PLL1_SetQ(4);
    LL_RCC_PLL1_SetR(2);
    LL_RCC_PLL1_Enable();

    /* Wait till PLL is ready */
    while(LL_RCC_PLL1_IsReady() != 1)
    {
    }

    /* Intermediate AHB prescaler 2 when target frequency clock is higher than 80 MHz */
    LL_RCC_SetAHBPrescaler(LL_RCC_AHB_DIV_2);

    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1);

    /* Wait till System clock is ready */
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL1)
    {

    }
    LL_RCC_SetSysPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAHBPrescaler(LL_RCC_AHB_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_4);
    LL_RCC_SetAPB3Prescaler(LL_RCC_APB3_DIV_2);
    LL_RCC_SetAPB4Prescaler(LL_RCC_APB4_DIV_2);
    LL_SetSystemCoreClock(200000000);

    /* Update the time base */
    if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
    {
    }
}

void sys_cfg_tick()
{
    volatile uint32_t ticks = SystemCoreClock / 1000;

    SysTick->LOAD = ticks - 1; /* set reload register */

    NVIC_SetPriority(SysTick_IRQn, 1);

    SysTick->VAL = 0;                                                                                /* Load the SysTick Counter Value */
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk; /* Enable SysTick IRQ and SysTick Timer */
}

void sys_cfg_console() {
    MX_USART1_UART_Init();

	xfunc_out = xputchar;
}

void sys_cfg_svc() {
    NVIC_SetPriority(SVCall_IRQn, 3);
    NVIC_EnableIRQ(SVCall_IRQn);
}

void sys_cfg_update_info() {
}

/******************************************************************************
* system utilities function
*******************************************************************************/
void xputchar(uint8_t c) {
	/* wait last transmission completed */
	while (LL_USART_IsActiveFlag_TXE(USART_CONSOLE) == RESET);

	/* put transnission data */
	LL_USART_TransmitData8(USART_CONSOLE, (uint8_t)c);

    /* wait transmission completed */
	while (LL_USART_IsActiveFlag_TC(USART_CONSOLE) == RESET);
}

void sys_ctrl_shell_put_char(uint8_t c) {
	/* wait last transmission completed */
	while (LL_USART_IsActiveFlag_TXE(USART_CONSOLE) == RESET);

	/* put transnission data */
	LL_USART_TransmitData8(USART_CONSOLE, (uint8_t)c);

    /* wait transmission completed */
	while (LL_USART_IsActiveFlag_TC(USART_CONSOLE) == RESET);
}

uint8_t sys_ctrl_shell_get_char() {
    volatile uint8_t c;

    if (LL_USART_IsActiveFlag_ORE(USART_CONSOLE) == SET)
    {
        LL_USART_ClearFlag_ORE(USART_CONSOLE);
    }
    
    if (LL_USART_IsActiveFlag_RXNE_RXFNE(USART_CONSOLE) == SET)
    {
        c = (uint8_t)LL_USART_ReceiveData8(USART_CONSOLE);
    }
    
    return c;
}

void sys_ctrl_reset() {
	NVIC_SystemReset();
}

void sys_ctrl_delay_us(volatile uint32_t count) {
	volatile uint32_t delay_value = 0;
	delay_value = count*delay_coeficient / 8;
	while(delay_value--);
}

void sys_ctrl_delay_ms(volatile uint32_t count) {
#if 0
	volatile uint32_t current_time = 0;
	volatile uint32_t current;
	volatile int32_t start = sys_ctrl_millis();

	while(current_time < count) {
		current = sys_ctrl_millis();

		if (current < start) {
			current_time += ((uint32_t)0xFFFFFFFF - start) + current;
		}
		else {
			current_time += current - start;
		}

		start = current;
	}
#else
	volatile uint32_t delay = 1000 * count;
	sys_ctrl_delay_us(delay);
#endif
}

void sys_ctrl_independent_watchdog_init() {
    MX_IWDG1_Init();
}

void sys_ctrl_independent_watchdog_reset() {
	ENTRY_CRITICAL();
	IWDG_ReloadCounter();
	EXIT_CRITICAL();
}

static uint32_t sys_ctrl_soft_counter = 0;
static uint32_t sys_ctrl_soft_time_out;

void sys_ctrl_soft_watchdog_init(uint32_t time_out) {
    sys_ctrl_soft_time_out = time_out;
}

void sys_ctrl_soft_watchdog_reset() {
	ENTRY_CRITICAL();
	sys_ctrl_soft_counter = 0;
	EXIT_CRITICAL();
}

void sys_ctrl_soft_watchdog_enable() {
    /*    UNUSED    */
}

void sys_ctrl_soft_watchdog_disable() {
    /*    UNUSED    */
}

void sys_ctrl_soft_watchdog_increase_counter() {
	sys_ctrl_soft_counter++;
	if (sys_ctrl_soft_counter >= sys_ctrl_soft_time_out) {
		/* FATAL("SWDG", 0x01); */
	}
}

void sys_ctrl_get_firmware_info(firmware_header_t* header) {
	extern uint32_t _start_flash;
	extern uint32_t _end_flash;
	extern uint32_t _data;
	extern uint32_t _edata;

	uint32_t check_sum = 0;
	uint32_t len_of_flash = (uint32_t)&_end_flash - (uint32_t)&_start_flash + ((uint32_t)&_edata - (uint32_t)&_data) + sizeof(uint32_t);

	for (uint32_t index = (uint32_t)&(_start_flash); index < ((uint32_t)&(_start_flash) + len_of_flash); index += sizeof(uint32_t)) {
		check_sum += *((uint32_t*)index);
	}

	header->psk = FIRMWARE_PSK;
	header->checksum = (check_sum & 0xFFFF);
	header->bin_len = len_of_flash;
}

uint32_t sys_ctrl_jump_to_app_req;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated"
void sys_ctrl_jump_to_app() {
	volatile uint32_t normal_stack_pointer	=	(uint32_t) *(volatile uint32_t*)(NORMAL_START_ADDRESS);
	volatile uint32_t normal_jump_address	=	(uint32_t) *(volatile uint32_t*)(NORMAL_START_ADDRESS + 4);

	p_jump_func jump_to_normal = (p_jump_func)normal_jump_address;

	/* Disable interrupt */
	DISABLE_INTERRUPTS();

	/* set stack pointer */
	__DMB();
	__asm volatile ("MSR msp, %0\n" : : "r" (normal_stack_pointer) : "sp");
	__DSB();

	/* jump to normal program */
	jump_to_normal();
}
#pragma GCC diagnostic pop
