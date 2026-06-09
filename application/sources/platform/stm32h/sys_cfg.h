/**
 ******************************************************************************
 * @author: GaoKong
 * @date:   05/09/2016
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
#include "ring_buffer.h"

/* USART Communication boards Interface */
#define USART_CONSOLE                           USART1

extern ring_buffer_char_t ring_buffer_char_shell_send;

extern void sys_cfg_clock();
extern void sys_cfg_tick();
extern void sys_cfg_console();
extern void sys_cfg_svc();
extern void sys_cfg_pendsv();
extern void sys_cfg_update_info();

extern void sys_cfg_restore_app();

#ifdef __cplusplus
}
#endif

#endif //__SYS_CFG_H__
