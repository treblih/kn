/*
 * =====================================================================================
 *
 *       Filename:  handler_int.c
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
#include "io.h"
#include "protect.h"
#include "proc.h"
#include "tty.h"
#include "hd.h"
#include "keyboard.h"
#include "proto.h"
#include "global.h"
#include "string.h"

KEYBOARD kb;

void handler_exception(int vec_no, int err_code, int eip, int cs, int eflags)
{
	int i;
	char *err_msg[] = { "#DE Divide Error",
		"#DB RESERVED",
		"—  NMI Interrupt",
		"#BP Breakpoint",
		"#OF Overflow",
		"#BR BOUND Range Exceeded",
		"#UD Invalid Opcode (Undefined Opcode)",
		"#NM Device Not Available (No Math Coprocessor)",
		"#DF Double Fault",
		"    Coprocessor Segment Overrun (reserved)",
		"#TS Invalid TSS",
		"#NP Segment Not Present",
		"#SS Stack-Segment Fault",
		"#GP General Protection",
		"#PF Page Fault",
		"—  (Intel reserved. Do not use.)",
		"#MF x87 FPU Floating-Point Error (Math Fault)",
		"#AC Alignment Check",
		"#MC Machine Check",
		"#XF SIMD Floating-Point Exception"
	};

	//disp_str(err_msg[vector]);
	disp_pos = 0;
	for (i = 0; i < 80 * 5; i++) {
		disp_str(" ");
	}
	disp_pos = 0;

	disp_str("Exception! --> ");
	disp_str(err_msg[vec_no]);
	disp_str("\n\n");
	disp_str("EFLAGS:");
	disp_int(eflags);
	disp_str("CS:");
	disp_int(cs);
	disp_str("EIP:");
	disp_int(eip);

	if (err_code != 0xFFFFFFFF) {
		disp_str("Error code:");
		disp_int(err_code);
	}
}

void irq_show_num()
{
	disp_str("irq--->");
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  handler_clock
 *  Description:  inc sys ticks;
 *  		  dec proc ticks;
 *  		  if int re-enter, return directly;
 *  		  else, get into schedule();
 * =====================================================================================
 */
void handler_clock()
{
	ticks++;                                                /* sys  ticks */
	proc_current->ticks--;                                  /* proc, consuption */

	if (int_counter != 0)                                   /* avoid int re-enter */
		return;

	/* if (proc_current->ticks > 0) */
	     /* return; */

	schedule();                                             /* proc switch */
}

void handler_keyboard()		// kb, written in RING0
{
	u8 scan_code = in_byte(KB_DATA);
	if (kb.count < KEY_BUF_SIZE) {	// no equals
		__asm__ __volatile__("cli");
		if (kb.head == kb.buf + KEY_BUF_SIZE) {	// means head has pointed out
			kb.head = kb.buf;
		}
		*kb.head = scan_code;
		kb.head++;
		kb.count++;
		__asm__ __volatile__("sti");
	}
}

void handler_hd()
{
	u8 hd_status = in_byte(HD_REG_CMD);
	send_hd_int(HD);
}
