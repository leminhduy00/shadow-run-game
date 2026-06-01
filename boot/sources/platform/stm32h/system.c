/**
 ******************************************************************************
 * @Author: GaoKong
 * @Date:   05/09/2016
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

#include "app.h"


#include "usart.h"

/*****************************************************************************/
/* linker variable                                                           */
/*****************************************************************************/
extern uint32_t _ldata;
extern uint32_t _data;
extern uint32_t _edata;
extern uint32_t _bss;
extern uint32_t _ebss;
extern uint32_t _estack;

system_info_t system_info;
/*****************************************************************************/
/* static function prototype                                                 */
/*****************************************************************************/
/*****************************/
/* system interrupt function */
/*****************************/
void default_handler();
void reset_handler();

/* cortex-M processor fault exceptions */
void nmi_handler()          __attribute__ ((weak));
void hard_fault_handler()   __attribute__ ((weak));
void mem_manage_handler()   __attribute__ ((weak));
void bus_fault_handler()    __attribute__ ((weak));
void usage_fault_handler()  __attribute__ ((weak));

/* cortex-M processor non-fault exceptions */
void svc_handler()          __attribute__ ((weak, alias("default_handler")));
void dg_monitor_handler()   __attribute__ ((weak, alias("default_handler")));
void pendsv_handler()       __attribute__ ((weak, alias("default_handler")));
void systick_handler();

/* external interrupts */
static void shell_handler();

/*****************************************************************************/
/* interrupt vector table                                                    */
/*****************************************************************************/
__attribute__((section(".isr_vector")))
void (* const isr_vector[])() = {
		((void (*)())(uint32_t)&_estack),		//	The initial stack pointer
		reset_handler,							//	The reset handler
		nmi_handler,							//	The NMI handler
		hard_fault_handler,						//	The hard fault handler
		mem_manage_handler,						//	The MPU fault handler
		bus_fault_handler,						//	The bus fault handler
		usage_fault_handler,					//	The usage fault handler
		0,										//	Reserved
		0,										//	Reserved
		0,										//	Reserved
		0,										//	Reserved
		svc_handler,							//	SVCall handler
		dg_monitor_handler,						//	Debug monitor handler
		0,										//	Reserved
		pendsv_handler,							//	The PendSV handler
		systick_handler,						//	The SysTick handler

		//External Interrupts
		default_handler,                   /* Window WatchDog              */
		default_handler,                /* PVD/AVD through EXTI Line detection */
		default_handler,             /* Tamper and TimeStamps through the EXTI line */
		default_handler,               /* RTC Wakeup through the EXTI line */
		default_handler,                  /* FLASH                        */
		default_handler,                    /* RCC                          */
		default_handler,                  /* EXTI Line0                   */
		default_handler,                  /* EXTI Line1                   */
		default_handler,                  /* EXTI Line2                   */
		default_handler,                   /* EXTI Line3                   */
		default_handler,                   /* EXTI Line4                   */
		default_handler,                   /* DMA1 Stream 0                */
		default_handler,                   /* DMA1 Stream 1                */
		default_handler,                   /* DMA1 Stream 2                */
		default_handler,                   /* DMA1 Stream 3                */
		default_handler,                   /* DMA1 Stream 4                */
		default_handler,                  /* DMA1 Stream 5                */
		default_handler,                  /* DMA1 Stream 6                */
		default_handler,                  /* ADC1, ADC2 and ADC3s         */
		default_handler,                  /* FDCAN1 interrupt line 0      */
		default_handler,                  /* FDCAN2 interrupt line 0      */
		default_handler,                  /* FDCAN1 interrupt line 1      */
		default_handler,                  /* FDCAN2 interrupt line 1      */
		default_handler,                  /* External Line[9:5]s          */
		default_handler,                  /* TIM1 Break interrupt         */
		default_handler,                  /* TIM1 Update interrupt        */
		default_handler,                  /* TIM1 Trigger and Commutation interrupt */
		default_handler,                  /* TIM1 Capture Compare         */
		default_handler,                  /* TIM2                         */
		default_handler,                  /* TIM3                         */
		default_handler,                        /* TIM4                         */
		default_handler,                  /* I2C1 Event                   */
		default_handler,                  /* I2C1 Error                   */
		default_handler,                  /* I2C2 Event                   */
		default_handler,                  /* I2C2 Error                   */
		default_handler,                  /* SPI1                         */
		default_handler,                  /* SPI2                         */
		shell_handler,                         /* USART1                       */
		default_handler,                  /* USART2                       */
		default_handler,                  /* USART3                       */
		default_handler,                  /* External Line[15:10]s        */
		default_handler,                  /* RTC Alarm (A and B) through EXTI Line */
		0,                                 /* Reserved                     */
		default_handler,                  /* TIM8 Break and TIM12         */
		default_handler,                  /* TIM8 Update and TIM13        */
		default_handler,                  /* TIM8 Trigger and Commutation and TIM14 */
		default_handler,                  /* TIM8 Capture Compare         */
		default_handler,                  /* DMA1 Stream7                 */
		default_handler,                  /* FMC                          */
		default_handler,                  /* SDMMC1                       */
		default_handler,                  /* TIM5                         */
		default_handler,                  /* SPI3                         */
		default_handler,                  /* UART4                        */
		default_handler,                  /* UART5                        */
		default_handler,                  /* TIM6 and DAC1&2 underrun errors */
		default_handler,                  /* TIM7                         */
		default_handler,                  /* DMA2 Stream 0                */
		default_handler,                  /* DMA2 Stream 1                */
		default_handler,                  /* DMA2 Stream 2                */
		default_handler,                  /* DMA2 Stream 3                */
		default_handler,                  /* DMA2 Stream 4                */
		default_handler,                  /* Ethernet                     */
		default_handler,                  /* Ethernet Wakeup through EXTI line */
		default_handler,                  /* FDCAN calibration unit interrupt*/
		0,                                 /* Reserved                     */
		0,                                 /* Reserved                     */
		0,                                 /* Reserved                     */
		0,                                 /* Reserved                     */
		default_handler,                  /* DMA2 Stream 5                */
		default_handler,                  /* DMA2 Stream 6                */
		default_handler,                  /* DMA2 Stream 7                */
		default_handler,                  /* USART6                       */
		default_handler,                  /* I2C3 event                   */
		default_handler,                  /* I2C3 error                   */
		default_handler,                  /* USB OTG HS End Point 1 Out   */
		default_handler,                  /* USB OTG HS End Point 1 In    */
		default_handler,                  /* USB OTG HS Wakeup through EXTI */
		default_handler,                  /* USB OTG HS                   */
		default_handler,                  /* DCMI                         */
		0,                                 /* Reserved                     */
		default_handler,                  /* Rng                          */
		default_handler,                  /* FPU                          */
		default_handler,                  /* UART7                        */
		default_handler,                  /* UART8                        */
		default_handler,                  /* SPI4                         */
		default_handler,                  /* SPI5                         */
		default_handler,                  /* SPI6                         */
		default_handler,                  /* SAI1                         */
		default_handler,                  /* LTDC                         */
		default_handler,                  /* LTDC error                   */
		default_handler,                  /* DMA2D                        */
		default_handler,                  /* SAI2                         */
		default_handler,                  /* QUADSPI                      */
		default_handler,                  /* LPTIM1                       */
		default_handler,                  /* HDMI_CEC                     */
		default_handler,                  /* I2C4 Event                   */
		default_handler,                  /* I2C4 Error                   */
		default_handler,                  /* SPDIF_RX                     */
		default_handler,                  /* USB OTG FS End Point 1 Out   */
		default_handler,                  /* USB OTG FS End Point 1 In    */
		default_handler,                  /* USB OTG FS Wakeup through EXTI */
		default_handler,                  /* USB OTG FS                   */
		default_handler,                  /* DMAMUX1 Overrun interrupt    */
		default_handler,                  /* HRTIM Master Timer global Interrupt */
		default_handler,                  /* HRTIM Timer A global Interrupt */
		default_handler,                  /* HRTIM Timer B global Interrupt */
		default_handler,                  /* HRTIM Timer C global Interrupt */
		default_handler,                  /* HRTIM Timer D global Interrupt */
		default_handler,                  /* HRTIM Timer E global Interrupt */
		default_handler,                  /* HRTIM Fault global Interrupt   */
		default_handler,                  /* DFSDM Filter0 Interrupt        */
		default_handler,                  /* DFSDM Filter1 Interrupt        */
		default_handler,                  /* DFSDM Filter2 Interrupt        */
		default_handler,                  /* DFSDM Filter3 Interrupt        */
		default_handler,                  /* SAI3 global Interrupt          */
		default_handler,                  /* Serial Wire Interface 1 global interrupt */
		default_handler,                  /* TIM15 global Interrupt      */
		default_handler,                  /* TIM16 global Interrupt      */
		default_handler,                  /* TIM17 global Interrupt      */
		default_handler,                  /* MDIOS Wakeup  Interrupt     */
		default_handler,                  /* MDIOS global Interrupt      */
		default_handler,                  /* JPEG global Interrupt       */
		default_handler,                  /* MDMA global Interrupt       */
		0,                                 /* Reserved                    */
		default_handler,                  /* SDMMC2 global Interrupt     */
		default_handler,                  /* HSEM1 global Interrupt      */
		0,                                 /* Reserved                    */
		default_handler,                  /* ADC3 global Interrupt       */
		default_handler,                  /* DMAMUX Overrun interrupt    */
		default_handler,                  /* BDMA Channel 0 global Interrupt */
		default_handler,                  /* BDMA Channel 1 global Interrupt */
		default_handler,                  /* BDMA Channel 2 global Interrupt */
		default_handler,                  /* BDMA Channel 3 global Interrupt */
		default_handler,                  /* BDMA Channel 4 global Interrupt */
		default_handler,                  /* BDMA Channel 5 global Interrupt */
		default_handler,                  /* BDMA Channel 6 global Interrupt */
		default_handler,                  /* BDMA Channel 7 global Interrupt */
		default_handler,                  /* COMP1 global Interrupt     */
		default_handler,                  /* LP TIM2 global interrupt   */
		default_handler,                  /* LP TIM3 global interrupt   */
		default_handler,                  /* LP TIM4 global interrupt   */
		default_handler,                  /* LP TIM5 global interrupt   */
		default_handler,                  /* LP UART1 interrupt         */
		0,                                 /* Reserved                   */
		default_handler,                  /* Clock Recovery Global Interrupt */
		default_handler,                  /* ECC diagnostic Global Interrupt */
		default_handler,                  /* SAI4 global interrupt      */
		0,                                 /* Reserved                   */
		0,                                 /* Reserved                   */
		default_handler,                  /* Interrupt for all 6 wake-up pins */
		default_handler,								//	TIM7
};

void sys_delay_us(volatile uint32_t count) {
	volatile uint32_t delay_value = 0;
	delay_value = count * 4;
	while(delay_value--);
}

void sys_delay_ms(volatile uint32_t count) {
	volatile uint32_t delay = 1000 * count;
	sys_delay_us(delay);
}

/*****************************************************************************/
/* static function defination                                                */
/************************************************                                                     *****************************/
void default_handler() {
}

uint8_t power_on_reset __attribute__ ((section ("non_clear_ram")));

uint8_t sys_is_power_on_reset() {
	return (uint8_t)power_on_reset;
}

void reset_handler() {
	power_on_reset = SYS_NON_POWER_ON_RESET;
	uint32_t *pSrc	= &_ldata;
	uint32_t *pDest	= &_data;

	if  (LL_RCC_IsActiveFlag_PORRST()){
		power_on_reset = SYS_POWER_ON_RESET;
	}
	LL_RCC_ClearResetFlags();

	if (sys_ctrl_jump_to_app_req == SYS_CTRL_JUMP_TO_APP_REQ) {
		sys_ctrl_jump_to_app();
	}

	/* init system */
	SystemInit();
	
	/* copy init data from FLASH to SRAM */
	while(pDest < &_edata) {
		*pDest++ = *pSrc++;
	}

	/* zero bss */
	for (pDest = &_bss; pDest < &_ebss; pDest++) {
		*pDest = 0UL;
	}

	ENTRY_CRITICAL();
    
	HAL_Init();
	sys_cfg_clock(); /* init system clock */
	sys_cfg_tick();
	sys_cfg_console();

	/* entry app function */
	boot_main();
}

/***************************************/
/* cortex-M processor fault exceptions */
/***************************************/
void nmi_handler() {
}

void hard_fault_handler() {
}

void mem_manage_handler() {
}

void bus_fault_handler() {
}

void usage_fault_handler() {
}

/*******************************************/
/* cortex-M processor non-fault exceptions */
/*******************************************/
void systick_handler() {
	sys_irq_timer_10ms();
}

/************************/
/* external interrupts  */
/************************/
void shell_handler() {
    if ((LL_USART_IsEnabledIT_RXNE_RXFNE(USART_CONSOLE) == SET) &&
        (LL_USART_IsActiveFlag_RXNE_RXFNE(USART_CONSOLE) == SET))
    {
        if (LL_USART_IsActiveFlag_ORE(USART_CONSOLE) == SET)
        {
            LL_USART_ClearFlag_ORE(USART_CONSOLE);
        }
        sys_irq_shell();
    }
}
