/*
 * =====================================================================================
 *
 *       Filename:  clock.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  15.02.10
 *       Revision:  
 *       Compiler:  gcc
 *
 *         Author:  Yang Zhang (ZY), imyeyeslove@163.com
 *        Company:  
 *
 * =====================================================================================
 */

#include "irq.h"
#include "io.h"
#include "type.h"
#include "protect.h"
#include "proc.h"
#include "tty.h"
#include "proto.h"
#include "global.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  init_clock
 *  Description:  called in init_i8259(), so it runs in RING-0
 * =====================================================================================
 */
void init_clock()
{
	out_byte(TMCR, RATE_GENERATOR);
	out_byte(TC0, (u8) (1193182L / 100));
	out_byte(TC0, (u8) ((1193182L / 100) >> 8));

	init_irq(0, handler_clock);                             /* IRQ-0 */
}
