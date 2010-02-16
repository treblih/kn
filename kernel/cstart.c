/*
 * =====================================================================================
 *
 *       Filename:  cstart.c
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

#include "type.h"
#include "protect.h"
#include "proc.h"
#include "tty.h"
#include "global.h"
#include "string.h"
#include "proto.h"

/* GCC has held it BUILT-IN, so no need define it ourselves */
/* #define		__GNUC__ */

void cstart()
{
	/* here can't use PRINTF, 'cause we haven't init tty'n console */
	disp_str("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n--init start--\n");

	init_gdt();

	init_idt();

	init_i8259();

	init_pcb();

	init_tss();

	disp_str("--all init done--\n");
}
