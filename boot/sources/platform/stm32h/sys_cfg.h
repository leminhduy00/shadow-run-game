/**
 ******************************************************************************
 * @Author: GaoKong
 * @Date:   05/09/2016
 ******************************************************************************
**/
#ifndef __SYS_CFG_H__
#define __SYS_CFG_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "stm32h7xx.h"

/* USART Communication boards Interface */
#define USART_CONSOLE                           USART1


extern void sys_cfg_clock();
extern void sys_cfg_tick();
extern void sys_cfg_svc();
extern void sys_cfg_console();
extern void sys_cfg_update_info();


#ifdef __cplusplus
}
#endif

#endif //__SYS_CFG_H__
