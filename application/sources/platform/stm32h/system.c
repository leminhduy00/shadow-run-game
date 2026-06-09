/**
 ******************************************************************************
 * @author: GaoKong
 * @date:   05/09/2016
 ******************************************************************************
 **/
#include <stdint.h>
#include <stdbool.h>

#include "sys_cfg.h"
#include "system.h"
#include "platform.h"

#include "sys_dbg.h"
#include "sys_ctrl.h"
#include "sys_irq.h"
#include "sys_boot.h"

#include "ak.h"
#include "message.h"
#include "timer.h"

#include "app.h"

#include "sys_cfg.h"

#include "stm32h7xx_ll_tim.h"
#include "stm32h7xx_ll_usart.h"
// #pragma GCC optimize ("O3")

/*****************************************************************************/
/* linker variable                                                           */
/*****************************************************************************/
extern uint32_t _ldata;
extern uint32_t _data;
extern uint32_t _edata;
extern uint32_t _bss;
extern uint32_t _ebss;
extern uint32_t _sstack;
extern uint32_t _estack;

extern void (*__preinit_array_start[])();
extern void (*__preinit_array_end[])();
extern void (*__init_array_start[])();
extern void (*__init_array_end[])();
extern void _init();
extern void _fini();

/*****************************************************************************/
/* static function prototype                                                 */
/*****************************************************************************/
/*****************************/
/* system interrupt function */
/*****************************/
void default_handler();
void reset_handler();

/*****************************/
/* user interrupt function   */
/*****************************/
void timer7_irq();
void uart1_irq();
void buzzer_irq(void);

/* cortex-M processor fault exceptions */
void nmi_handler() __attribute__((weak));
void hard_fault_handler() __attribute__((weak));
void mem_manage_handler() __attribute__((weak));
void bus_fault_handler() __attribute__((weak));
void usage_fault_handler() __attribute__((weak));

/* cortex-M processor non-fault exceptions */
void dg_monitor_handler() __attribute__((weak, alias("default_handler")));
void pendsv_handler() __attribute__((weak, alias("default_handler")));
void svc_handler() __attribute__((weak, alias("default_handler")));
void systick_handler();

/* external interrupts */
void svc_exe(uint32_t *svc_args);

/*****************************************************************************/
/* system variable                                                           */
/*****************************************************************************/
system_info_t system_info;
/*****************************************************************************/
/* interrupt vector table                                                    */
/*****************************************************************************/
__attribute__((section(".isr_vector"))) void (*const isr_vector[])() = {
    ((void (*)())(uint32_t)&_estack),   //	The initial stack pointer
    reset_handler,                      //	The reset handler
    nmi_handler,                        //	The NMI handler
    hard_fault_handler,                 //	The hard fault handler
    mem_manage_handler,                 //	The MPU fault handler
    bus_fault_handler,                  //	The bus fault handler
    usage_fault_handler,                //	The usage fault handler
    0,                                  //	Reserved
    0,                                  //	Reserved
    0,                                  //	Reserved
    0,                                  //	Reserved
    svc_handler,                        //	SVCall handler
    dg_monitor_handler,                 //	Debug monitor handler
    0,                                  //	Reserved
    pendsv_handler,                     //	The PendSV handler
    systick_handler,                    //	The SysTick handler

    // External Interrupts
    default_handler, /* Window WatchDog              */
    default_handler, /* PVD/AVD through EXTI Line detection */
    default_handler, /* Tamper and TimeStamps through the EXTI line */
    default_handler, /* RTC Wakeup through the EXTI line */
    default_handler, /* FLASH                        */
    default_handler, /* RCC                          */
    default_handler, /* EXTI Line0                   */
    default_handler, /* EXTI Line1                   */
    default_handler, /* EXTI Line2                   */
    default_handler, /* EXTI Line3                   */
    default_handler, /* EXTI Line4                   */
    default_handler, /* DMA1 Stream 0                */
    default_handler, /* DMA1 Stream 1                */
    default_handler, /* DMA1 Stream 2                */
    default_handler, /* DMA1 Stream 3                */
    default_handler, /* DMA1 Stream 4                */
    default_handler, /* DMA1 Stream 5                */
    default_handler, /* DMA1 Stream 6                */
    default_handler, /* ADC1, ADC2 and ADC3s         */
    default_handler, /* FDCAN1 interrupt line 0      */
    default_handler, /* FDCAN2 interrupt line 0      */
    default_handler, /* FDCAN1 interrupt line 1      */
    default_handler, /* FDCAN2 interrupt line 1      */
    default_handler, /* External Line[9:5]s          */
    default_handler, /* TIM1 Break interrupt         */
    default_handler, /* TIM1 Update interrupt        */
    default_handler, /* TIM1 Trigger and Commutation interrupt */
    default_handler, /* TIM1 Capture Compare         */
    default_handler, /* TIM2                         */
    buzzer_irq,      /* TIM3                         */
    default_handler, /* TIM4                         */
    default_handler, /* I2C1 Event                   */
    default_handler, /* I2C1 Error                   */
    default_handler, /* I2C2 Event                   */
    default_handler, /* I2C2 Error                   */
    default_handler, /* SPI1                         */
    default_handler, /* SPI2                         */
    uart1_irq,       /* USART1                       */
#if defined(TASK_MBMASTER_EN) && defined(TASK_ZIGBEE_EN)
    default_handler,   //	USART2
#elif defined(TASK_MBMASTER_EN)
    vMBPUSART2ISR,   //	USART2
#elif defined(TASK_ZIGBEE_EN)
    uart2_irq,   //	USART2
#else
    default_handler,   //	USART2
#endif
    default_handler, /* USART3                       */
    default_handler, /* External Line[15:10]s        */
    default_handler, /* RTC Alarm (A and B) through EXTI Line */
    0,               /* Reserved                     */
    default_handler, /* TIM8 Break and TIM12         */
    default_handler, /* TIM8 Update and TIM13        */
    default_handler, /* TIM8 Trigger and Commutation and TIM14 */
    default_handler, /* TIM8 Capture Compare         */
    default_handler, /* DMA1 Stream7                 */
    default_handler, /* FMC                          */
    default_handler, /* SDMMC1                       */
    default_handler, /* TIM5                         */
    default_handler, /* SPI3                         */
    default_handler, /* UART4                        */
    default_handler, /* UART5                        */
    default_handler, /* TIM6 and DAC1&2 underrun errors */
    timer7_irq,      /* TIM7                         */
    default_handler, /* DMA2 Stream 0                */
    default_handler, /* DMA2 Stream 1                */
    default_handler, /* DMA2 Stream 2                */
    default_handler, /* DMA2 Stream 3                */
    default_handler, /* DMA2 Stream 4                */
    default_handler, /* Ethernet                     */
    default_handler, /* Ethernet Wakeup through EXTI line */
    default_handler, /* FDCAN calibration unit interrupt*/
    0,               /* Reserved                     */
    0,               /* Reserved                     */
    0,               /* Reserved                     */
    0,               /* Reserved                     */
    default_handler, /* DMA2 Stream 5                */
    default_handler, /* DMA2 Stream 6                */
    default_handler, /* DMA2 Stream 7                */
    default_handler, /* USART6                       */
    default_handler, /* I2C3 event                   */
    default_handler, /* I2C3 error                   */
    default_handler, /* USB OTG HS End Point 1 Out   */
    default_handler, /* USB OTG HS End Point 1 In    */
    default_handler, /* USB OTG HS Wakeup through EXTI */
    default_handler, /* USB OTG HS                   */
    default_handler, /* DCMI                         */
    0,               /* Reserved                     */
    default_handler, /* Rng                          */
    default_handler, /* FPU                          */
    default_handler, /* UART7                        */
    default_handler, /* UART8                        */
    default_handler, /* SPI4                         */
    default_handler, /* SPI5                         */
    default_handler, /* SPI6                         */
    default_handler, /* SAI1                         */
    default_handler, /* LTDC                         */
    default_handler, /* LTDC error                   */
    default_handler, /* DMA2D                        */
    default_handler, /* SAI2                         */
    default_handler, /* QUADSPI                      */
    default_handler, /* LPTIM1                       */
    default_handler, /* HDMI_CEC                     */
    default_handler, /* I2C4 Event                   */
    default_handler, /* I2C4 Error                   */
    default_handler, /* SPDIF_RX                     */
    default_handler, /* USB OTG FS End Point 1 Out   */
    default_handler, /* USB OTG FS End Point 1 In    */
    default_handler, /* USB OTG FS Wakeup through EXTI */
    default_handler, /* USB OTG FS                   */
    default_handler, /* DMAMUX1 Overrun interrupt    */
    default_handler, /* HRTIM Master Timer global Interrupt */
    default_handler, /* HRTIM Timer A global Interrupt */
    default_handler, /* HRTIM Timer B global Interrupt */
    default_handler, /* HRTIM Timer C global Interrupt */
    default_handler, /* HRTIM Timer D global Interrupt */
    default_handler, /* HRTIM Timer E global Interrupt */
    default_handler, /* HRTIM Fault global Interrupt   */
    default_handler, /* DFSDM Filter0 Interrupt        */
    default_handler, /* DFSDM Filter1 Interrupt        */
    default_handler, /* DFSDM Filter2 Interrupt        */
    default_handler, /* DFSDM Filter3 Interrupt        */
    default_handler, /* SAI3 global Interrupt          */
    default_handler, /* Serial Wire Interface 1 global interrupt */
    default_handler, /* TIM15 global Interrupt      */
    default_handler, /* TIM16 global Interrupt      */
    default_handler, /* TIM17 global Interrupt      */
    default_handler, /* MDIOS Wakeup  Interrupt     */
    default_handler, /* MDIOS global Interrupt      */
    default_handler, /* JPEG global Interrupt       */
    default_handler, /* MDMA global Interrupt       */
    0,               /* Reserved                    */
    default_handler, /* SDMMC2 global Interrupt     */
    default_handler, /* HSEM1 global Interrupt      */
    0,               /* Reserved                    */
    default_handler, /* ADC3 global Interrupt       */
    default_handler, /* DMAMUX Overrun interrupt    */
    default_handler, /* BDMA Channel 0 global Interrupt */
    default_handler, /* BDMA Channel 1 global Interrupt */
    default_handler, /* BDMA Channel 2 global Interrupt */
    default_handler, /* BDMA Channel 3 global Interrupt */
    default_handler, /* BDMA Channel 4 global Interrupt */
    default_handler, /* BDMA Channel 5 global Interrupt */
    default_handler, /* BDMA Channel 6 global Interrupt */
    default_handler, /* BDMA Channel 7 global Interrupt */
    default_handler, /* COMP1 global Interrupt     */
    default_handler, /* LP TIM2 global interrupt   */
    default_handler, /* LP TIM3 global interrupt   */
    default_handler, /* LP TIM4 global interrupt   */
    default_handler, /* LP TIM5 global interrupt   */
    default_handler, /* LP UART1 interrupt         */
    0,               /* Reserved                   */
    default_handler, /* Clock Recovery Global Interrupt */
    default_handler, /* ECC diagnostic Global Interrupt */
    default_handler, /* SAI4 global interrupt      */
    0,               /* Reserved                   */
    0,               /* Reserved                   */
    default_handler, /* Interrupt for all 6 wake-up pins */
};

void __attribute__((naked))
sys_ctrl_delay(volatile uint32_t count)
{
    (void)count;
    __asm("    subs    r0, #1\n"
          "    bne     sys_ctrl_delay\n"
          "    bx      lr");
}

static uint32_t millis_current;

uint32_t sys_ctrl_micros()
{
    uint32_t m = millis_current;
    const uint32_t tms = SysTick->LOAD + 1;
    __IO uint32_t u = tms - SysTick->VAL;

    /* checks if the Systick counter flag is active or not */
    if (((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == (SysTick_CTRL_COUNTFLAG_Msk)))
    {
        m = millis_current;
        u = tms - SysTick->VAL;
    }

    return (m * 1000 + (u * 1000) / tms);
}

uint32_t sys_ctrl_millis()
{
    volatile uint32_t ret;
    ENTRY_CRITICAL();
    ret = millis_current;
    EXIT_CRITICAL();
    return ret;
}

void _init()
{
    /* dummy */
}

void _fini()
{
    /* dummy */
}

/*****************************************************************************/
/* static function defination                                                */
/*****************************************************************************/
void default_handler()
{
    FATAL("SY", 0xEE);
}

void reset_handler()
{
    /* MUST BE disable interrupt */
    __disable_irq();

    volatile uint32_t *pSrc = &_ldata;
    volatile uint32_t *pDest = &_data;
    volatile unsigned i, cnt;

    /* init system */
    SystemInit();

    /* copy init data from FLASH to SRAM */
    while (pDest < &_edata)
    {
        *pDest++ = *pSrc++;
    }

    /* zero bss */
    for (pDest = &_bss; pDest < &_ebss; pDest++)
    {
        *pDest = 0UL;
    }

    sys_stack_count_init();

    ENTRY_CRITICAL();

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    sys_cfg_clock();   /* init system clock */
    sys_cfg_svc();     /* setting svc exception priority */
    sys_cfg_pendsv();  /* setting psv exception priority */
    sys_cfg_tick();    /* system tick 1ms */
    sys_cfg_console(); /* system console */

    /* invoke all static constructors */
    cnt = __preinit_array_end - __preinit_array_start;
    for (i = 0; i < cnt; i++)
        __preinit_array_start[i]();

    _init();

    cnt = __init_array_end - __init_array_start;
    for (i = 0; i < cnt; i++)
        __init_array_start[i]();

    /* wait configuration stable */
    sys_ctrl_delay(100); /* wait 300 cycles clock */

    /* update system information */
    sys_cfg_update_info();

    /* entry app function */
    main_app();
}

/****************************************
 * system memory function.				*
 ****************************************/
static uint32_t sys_stack_size;
uint32_t sys_stack_count_init()
{
    volatile uint32_t *start_stack = &_sstack;
    uint32_t *current_stack = (uint32_t *)((uint32_t)__get_MSP());

    sys_stack_size = ((uint32_t)&_estack - (uint32_t)&_sstack);

    ENTRY_CRITICAL();
    while (start_stack < current_stack)
    {
        *start_stack++ = 0xDEADC0DEu;
    }
    EXIT_CRITICAL();

    return sys_stack_size;
}

uint32_t sys_stack_usage()
{
    volatile uint32_t *cal_stack_free = &_sstack;
    while (*cal_stack_free == 0xDEADC0DEu)
    {
        cal_stack_free++;
    }
    return ((uint32_t)&_estack - (uint32_t)cal_stack_free);
}

uint32_t sys_stack_get_size()
{
    return sys_stack_size;
}

/***************************************/
/* cortex-M processor fault exceptions */
/***************************************/
void nmi_handler()
{
    FATAL("SY", 0x01);
}

void hard_fault_handler()
{
    FATAL("SY", 0x02);
}

void mem_manage_handler()
{
    FATAL("SY", 0x03);
}

void bus_fault_handler()
{
    FATAL("SY", 0x04);
}

void usage_fault_handler()
{
    FATAL("SY", 0x05);
}

/*******************************************/
/* cortex-M processor non-fault exceptions */
/*******************************************/
void systick_handler()
{
    static uint32_t div_counter = 0;

    task_entry_interrupt();

    /* increasing millis counter */
    millis_current++;

    timer_tick(1);

    if (div_counter == 10)
    {
        div_counter = 0;
    }

    switch (div_counter)
    {
        case 0:
            sys_irq_timer_10ms();
            break;

        default:
            break;
    }

    div_counter++;

    task_exit_interrupt();
}

void svc_exe(uint32_t *svc_args)
{
    volatile uint8_t svc_number;

    sys_dbg_func_stack_dump(svc_args);
    sys_dbg_cpu_dump();

    svc_number = ((uint8_t *)svc_args[6])[-2];

    switch (svc_number)
    {
        case 0x01:
        {
        }
        break;

        case 0x02:
        {
        }
        break;

        default:
            break;
    }
}

/************************/
/* external interrupts  */
/************************/
void uart1_irq()
{
    task_entry_interrupt();

    if ((LL_USART_IsEnabledIT_RXNE_RXFNE(USART_CONSOLE) == SET) &&
        (LL_USART_IsActiveFlag_RXNE_RXFNE(USART_CONSOLE) == SET))
    {
        if (LL_USART_IsActiveFlag_ORE(USART_CONSOLE) == SET)
        {
            LL_USART_ClearFlag_ORE(USART_CONSOLE);
        }
        sys_irq_shell();
    }

    if ((LL_USART_IsEnabledIT_TXE_TXFNF(USART_CONSOLE) == SET) &&
        (LL_USART_IsActiveFlag_TXE_TXFNF(USART_CONSOLE) == SET))
    {
        if (ring_buffer_char_is_empty(&ring_buffer_char_shell_send))
        {
            LL_USART_DisableIT_TXE_TXFNF(USART_CONSOLE);
        }
        else
        {
            LL_USART_TransmitData8(USART_CONSOLE, ring_buffer_char_get(&ring_buffer_char_shell_send));
        }
    }

    task_exit_interrupt();
}

void timer7_irq()
{
    if (LL_TIM_IsActiveFlag_UPDATE(TIM7))
    {
        if (LL_TIM_IsEnabledIT_UPDATE(TIM7))
        {
            HAL_IncTick();
            sys_ctrl_soft_watchdog_increase_counter();
            LL_TIM_ClearFlag_UPDATE(TIM7);
        }
    }

}
