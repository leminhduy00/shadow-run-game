/**
 ******************************************************************************
 * @author: GaoKong
 * @date:   05/09/2016
 ******************************************************************************
 **/
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "sys_cfg.h"
#include "system.h"

#include "xprintf.h"
#include "ring_buffer.h"

#include "sys_dbg.h"
#include "sys_ctrl.h"
#include "sys_io.h"
#include "sys_dbg.h"
#include "ak.h"

#include "tim.h"
#include "iwdg.h"
#include "usart.h"

#include "stm32h7xx_ll_system.h"
#include "stm32h7xx_ll_pwr.h"
#include "stm32h7xx_ll_cortex.h"
#include "stm32h7xx_ll_utils.h"
#include "stm32h7xx_ll_rcc.h"
// #pragma GCC optimize ("O3")

#define RING_BUFFER_CHAR_SHELL_SEND_BUFFER_SIZE 512

/* Private define */
static volatile uint32_t delay_coeficient = 0;

static uint8_t ring_buffer_char_shell_send_buffer[RING_BUFFER_CHAR_SHELL_SEND_BUFFER_SIZE];
ring_buffer_char_t ring_buffer_char_shell_send;

/******************************************************************************
 * system configure function
 *******************************************************************************/
void sys_cfg_clock()
{
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
    while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_4)
    {
    }
    LL_PWR_ConfigSupply(LL_PWR_LDO_SUPPLY);
    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE3);
    while (LL_PWR_IsActiveFlag_VOS() == 0)
    {
    }
    LL_RCC_HSE_Enable();

    /* Wait till HSE is ready */
    while (LL_RCC_HSE_IsReady() != 1)
    {
    }
    LL_RCC_LSI_Enable();

    /* Wait till LSI is ready */
    while (LL_RCC_LSI_IsReady() != 1)
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
    while (LL_RCC_PLL1_IsReady() != 1)
    {
    }

    /* Intermediate AHB prescaler 2 when target frequency clock is higher than 80 MHz */
    LL_RCC_SetAHBPrescaler(LL_RCC_AHB_DIV_2);

    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1);

    /* Wait till System clock is ready */
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL1)
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
    if (HAL_InitTick(IRQ_PRIO_TIMER7_SOFT_WATCHDOG) != HAL_OK)
    {
        FATAL("CLK", 0x01);
    }
}

void sys_cfg_tick()
{
    volatile uint32_t ticks = SystemCoreClock / 1000;

    SysTick->LOAD = ticks - 1; /* set reload register */

    NVIC_SetPriority(SysTick_IRQn, IRQ_PRIO_SYS_SYSTEMSTICK);

    SysTick->VAL = 0;                                                                                /* Load the SysTick Counter Value */
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk; /* Enable SysTick IRQ and SysTick Timer */
}

void sys_cfg_console()
{
    ring_buffer_char_init(&ring_buffer_char_shell_send, ring_buffer_char_shell_send_buffer, RING_BUFFER_CHAR_SHELL_SEND_BUFFER_SIZE);

    MX_USART1_UART_Init();

    sys_ctrl_shell_sw_to_block();
}

void sys_cfg_svc()
{
    NVIC_SetPriority(SVCall_IRQn, IRQ_PRIO_SYS_SVC);
    NVIC_EnableIRQ(SVCall_IRQn);
}

void sys_cfg_pendsv()
{
    NVIC_SetPriority(PendSV_IRQn, IRQ_PRIO_SYS_PENDSV);
    NVIC_EnableIRQ(PendSV_IRQn);
}

void sys_cfg_update_info()
{
    extern uint32_t _start_flash;
    extern uint32_t _end_flash;
    extern uint32_t _start_ram;
    extern uint32_t _end_ram;
    extern uint32_t _data;
    extern uint32_t _edata;
    extern uint32_t _bss;
    extern uint32_t _ebss;
    extern uint32_t __heap_start__;
    extern uint32_t __heap_end__;
    extern uint32_t _estack;

    LL_RCC_ClocksTypeDef RCC_Clocks;

    LL_RCC_GetSystemClocksFreq(&RCC_Clocks);

    system_info.cpu_clock = RCC_Clocks.HCLK_Frequency;
    system_info.tick = 1;
    system_info.console_baudrate = SYS_CONSOLE_BAUDRATE;
    system_info.flash_used = ((uint32_t)&_end_flash - (uint32_t)&_start_flash) + ((uint32_t)&_edata - (uint32_t)&_data);
    system_info.ram_used = (uint32_t)&_estack - (uint32_t)&_start_ram;
    system_info.data_init_size = (uint32_t)&_edata - (uint32_t)&_data;
    system_info.data_non_init_size = (uint32_t)&_ebss - (uint32_t)&_bss;
    system_info.stack_avail = (uint32_t)&_estack - (uint32_t)&_end_ram;
    system_info.heap_avail = (uint32_t)&__heap_end__ - (uint32_t)&__heap_start__;
    system_info.ram_other = system_info.ram_used - (system_info.heap_avail + system_info.stack_avail + system_info.data_non_init_size + system_info.data_init_size);

    delay_coeficient = system_info.cpu_clock / 1000000;

    /* kernel banner */
    SYS_PRINT("\n");
    SYS_PRINT("   __    _  _ \n");
    SYS_PRINT("  /__\\  ( )/ )\n");
    SYS_PRINT(" /(__)\\ (   ( \n");
    SYS_PRINT("(__)(__)(_)\\_)\n");
    SYS_PRINT("Wellcome to Active Kernel %s\n", AK_VERSION);
    SYS_PRINT("\n");

#if 0
	/* system banner */
	SYS_PRINT("system information:\n");
	SYS_PRINT("\tFLASH used:\t%d bytes\n", system_info.flash_used);
	SYS_PRINT("\tSRAM used:\t%d bytes\n", system_info.ram_used);
	SYS_PRINT("\t\tdata init size:\t\t%d bytes\n", system_info.data_init_size);
	SYS_PRINT("\t\tdata non_init size:\t%d bytes\n", system_info.data_non_init_size);
	SYS_PRINT("\t\tstack avail:\t\t%d bytes\n", system_info.stack_avail);
	SYS_PRINT("\t\theap avail:\t\t%d bytes\n", system_info.heap_avail);
	SYS_PRINT("\t\tother:\t\t\t%d bytes\n", system_info.ram_other);
	SYS_PRINT("\n");
	SYS_PRINT("\tcpu clock:\t%d Hz\n", system_info.cpu_clock);
	SYS_PRINT("\ttime tick:\t%d ms\n", system_info.tick);
	SYS_PRINT("\tconsole:\t%d bps\n", system_info.console_baudrate);
	SYS_PRINT("\n\n");
#endif
}

/******************************************************************************
 * system utilities function
 *******************************************************************************/
void sys_ctrl_shell_put_char(uint8_t c)
{
    bool _flag_trigger = false;

    ENTRY_CRITICAL();

    if (ring_buffer_char_is_empty(&ring_buffer_char_shell_send))
    {
        _flag_trigger = true;
    }

    ring_buffer_char_put(&ring_buffer_char_shell_send, c);

    EXIT_CRITICAL();

    if (_flag_trigger)
    {
        LL_USART_EnableIT_TXE_TXFNF(USART_CONSOLE);
    }
}

void sys_ctrl_shell_put_char_block(uint8_t c)
{
	while (LL_USART_IsActiveFlag_TXE(USART_CONSOLE) == RESET);

	/* put transnission data */
	LL_USART_TransmitData8(USART_CONSOLE, (uint8_t)c);

    /* wait transmission completed */
	while (LL_USART_IsActiveFlag_TC(USART_CONSOLE) == RESET);
}

void sys_ctrl_shell_sw_to_block()
{
    ENTRY_CRITICAL();

    xfunc_output = (void (*)(int))sys_ctrl_shell_put_char_block;

    LL_USART_DisableIT_TXE_TXFNF(USART_CONSOLE);

    EXIT_CRITICAL();
}

void sys_ctrl_shell_sw_to_nonblock()
{
    ENTRY_CRITICAL();

    xfunc_output = (void (*)(int))sys_ctrl_shell_put_char;

    LL_USART_EnableIT_TXE_TXFNF(USART_CONSOLE);

    EXIT_CRITICAL();
}

uint8_t sys_ctrl_shell_get_char()
{
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

void sys_ctrl_reset()
{
    NVIC_SystemReset();
}

void sys_ctrl_delay_us(volatile uint32_t count)
{
#if 0
	volatile uint32_t delay_value = 0;
	delay_value = count*delay_coeficient / 8;
	while(delay_value--);
#else
    __IO uint32_t currentTicks = SysTick->VAL;
    /* Number of ticks per millisecond */
    const uint32_t tickPerMs = SysTick->LOAD + 1;
    /* Number of ticks to count */
    const uint32_t nbTicks = ((count - ((count > 0) ? 1 : 0)) * tickPerMs) / 1000;
    /* Number of elapsed ticks */
    uint32_t elapsedTicks = 0;
    __IO uint32_t oldTicks = currentTicks;
    do
    {
        currentTicks = SysTick->VAL;
        elapsedTicks += (oldTicks < currentTicks) ? tickPerMs + oldTicks - currentTicks : oldTicks - currentTicks;
        oldTicks = currentTicks;
    } while (nbTicks > elapsedTicks);
#endif
}

void sys_ctrl_delay_ms(volatile uint32_t count)
{
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
    if (count != 0)
    {
        uint32_t start = sys_ctrl_millis();
        do
        {
            /* yield(); */
        } while (sys_ctrl_millis() - start < count);
    }
#endif
}

void sys_ctr_sleep_wait_for_irq()
{
    LL_LPM_EnableSleep();
    __WFI();
}

void sys_ctr_stop_mcu()
{
    LL_APB4_GRP1_EnableClock(LL_APB4_GRP1_PERIPH_SYSCFG);

	/* Configure all GPIO as analog to reduce current consumption on non used IOs */
	/* Enable GPIOs clock */
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOA | LL_AHB4_GRP1_PERIPH_GPIOB | LL_AHB4_GRP1_PERIPH_GPIOC |
                             LL_AHB4_GRP1_PERIPH_GPIOD | LL_AHB4_GRP1_PERIPH_GPIOE);

    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_ALL, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_ALL, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_ALL, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode(GPIOD, LL_GPIO_PIN_ALL, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinMode(GPIOE, LL_GPIO_PIN_ALL, LL_GPIO_MODE_ANALOG);

    /* Disable GPIOs clock */
    LL_AHB4_GRP1_DisableClock(LL_AHB4_GRP1_PERIPH_GPIOA | LL_AHB4_GRP1_PERIPH_GPIOB | LL_AHB4_GRP1_PERIPH_GPIOC |
                              LL_AHB4_GRP1_PERIPH_GPIOD | LL_AHB4_GRP1_PERIPH_GPIOE);

    /* Enter Stop Mode */
    LL_LPM_EnableDeepSleep();
    __WFI();

    while (1)
    {
    }
}

uint32_t sys_ctr_get_exception_number()
{
    volatile uint32_t exception_number = (uint32_t)__get_IPSR();
    return exception_number;
}

void sys_ctr_restart_app()
{
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated"
void sys_cfg_restore_app()
{
    extern uint32_t _isr_vector;
    volatile uint32_t normal_stack_pointer = (uint32_t) * (volatile uint32_t *)(APP_START_ADDR);
    volatile uint32_t normal_jump_address = (uint32_t) * (volatile uint32_t *)(APP_START_ADDR + 4);

    p_jump_func jump_to_normal = (p_jump_func)normal_jump_address;

    /* update interrupt vertor table */
    SCB->VTOR = (uint32_t)&_isr_vector; /* Vector Table Relocation in Internal FLASH. */

    /* set stack pointer */
    __DSB();
    __asm volatile("MSR msp, %0\n" : : "r"(normal_stack_pointer) : "sp");
    __DSB();

    /* jump to normal program */
    jump_to_normal();

    while (1)
        ;
}
#pragma GCC diagnostic pop

void sys_ctrl_independent_watchdog_init()
{
    MX_IWDG1_Init();
}

void sys_ctrl_independent_watchdog_reset()
{
    ENTRY_CRITICAL();
    IWDG_ReloadCounter();
    EXIT_CRITICAL();
}

static uint32_t sys_ctrl_soft_counter = 0;
static uint32_t sys_ctrl_soft_time_out;

void sys_ctrl_soft_watchdog_init(uint32_t time_out)
{
   sys_ctrl_soft_time_out = time_out;
}

void sys_ctrl_soft_watchdog_reset()
{
    ENTRY_CRITICAL();
    sys_ctrl_soft_counter = 0;
    EXIT_CRITICAL();
}

void sys_ctrl_soft_watchdog_enable()
{
   /*    UNUSED    */
}

void sys_ctrl_soft_watchdog_disable()
{
   /*    UNUSED    */
}

void sys_ctrl_soft_watchdog_increase_counter()
{
    sys_ctrl_soft_counter++;
    if (sys_ctrl_soft_counter >= sys_ctrl_soft_time_out)
{
        LL_TIM_DisableIT_UPDATE(TIM7);
        FATAL("SWDG", 0x01);
    }
}

void sys_ctrl_get_firmware_info(firmware_header_t *header)
{
    extern uint32_t _start_flash;
    extern uint32_t _end_flash;
    extern uint32_t _data;
    extern uint32_t _edata;

    uint32_t check_sum = 0;
    uint32_t len_of_flash = (uint32_t)&_end_flash - (uint32_t)&_start_flash + ((uint32_t)&_edata - (uint32_t)&_data) + sizeof(uint32_t);

    for (uint32_t index = (uint32_t) & (_start_flash); index < ((uint32_t) & (_start_flash) + len_of_flash); index += sizeof(uint32_t))
    {
        check_sum += *((uint32_t *)index);
    }

    header->psk = FIRMWARE_PSK;
    header->checksum = (check_sum & 0xFFFF);
    header->bin_len = len_of_flash;
}

void task_irq_io_entry_trigger()
{
#if defined(AK_IO_IRQ_ANALYZER)
    GPIO_ResetBits(LED_LIFE_IO_PORT, LED_LIFE_IO_PIN);
#endif
}

void task_irq_io_exit_trigger()
{
#if defined(AK_IO_IRQ_ANALYZER)
    GPIO_SetBits(LED_LIFE_IO_PORT, LED_LIFE_IO_PIN);
#endif
}
