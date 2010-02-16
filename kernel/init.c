/*
 * =====================================================================================
 *
 *       Filename:  init.c
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
#include "string.h"

void init_gdt()
{
	memcpy(gdt, *(u32 *) (&gdt_ptr[2]), (*(u16 *) (&gdt_ptr[0]) + 1));

	*(u16 *) (&gdt_ptr[0]) = GDT_SIZE * sizeof(DESCRIPTOR) - 1;
	*(u32 *) (&gdt_ptr[2]) = (u32) gdt;

	/*
	 * just for call-gate
	 * here we could make one with the 2nd args changed
	 */
	init_gdt_gate(4, 0, DA_386CGATE, DPL_USER);
}

void init_i8259()
{
	/*-----------------------------------------------------------------------------
	 *  init, follow the very sequence
	 *-----------------------------------------------------------------------------*/
	out_byte(ICRM, ICW1);
	out_byte(ICRS, ICW1);
	out_byte(IMRM, ICW2M);
	out_byte(IMRS, ICW2S);
	out_byte(IMRM, ICW3M);
	out_byte(IMRS, ICW3S);
	out_byte(IMRM, ICW4);
	out_byte(IMRS, ICW4);

	/*-----------------------------------------------------------------------------
	 *  disable all
	 *-----------------------------------------------------------------------------*/
	out_byte(IMRM, OCW1M);
	out_byte(IMRS, OCW1S);

	/*-----------------------------------------------------------------------------
	 *  default handler, just print the index
	 *-----------------------------------------------------------------------------*/
	for (int i = 0; i < 16; i++) {
		irq_handler_table[i] = irq_show_num;
	}

	init_clock();                                           /* IRQ-0 */
	init_keyboard();                                        /* IRQ-1 */

	init_slave();                                           /* IRQ-2 */
	init_hd();                                              /* IRQ-14 */
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  init_idt
 *  Description:  copy the empty idt new home, set every interrupt entry in pmode
 * =====================================================================================
 */
void init_idt()
{
	*(u16 *) (&idt_ptr[0]) = IDT_SIZE * sizeof(GATE) - 1;   /* 16 limit */
	*(u32 *) (&idt_ptr[2]) = (u32) idt;                     /* 32 addr */

	/*-----------------------------------------------------------------------------
	 *  should have follow the sequence
	 *  fuck INTEL, occupied 0x08 - 0x0f, in real mode they're reserved for IRQ
	 *-----------------------------------------------------------------------------*/
	init_idt_desc(0x00, _divide_error,		DA_386IGATE, DPL_KERL);
	init_idt_desc(0x01, _single_step_exception,	DA_386IGATE, DPL_KERL);
	init_idt_desc(0x02, _nmi,			DA_386IGATE, DPL_KERL);
	init_idt_desc(0x03, _breakpoint_exception,	DA_386IGATE, DPL_USER);	/* user */
	init_idt_desc(0x04, _overflow,			DA_386IGATE, DPL_USER);	/* user unique ins -- INTO */
	init_idt_desc(0x05, _bounds_check,		DA_386IGATE, DPL_KERL);
	init_idt_desc(0x06, _inval_opcode,		DA_386IGATE, DPL_KERL);
	init_idt_desc(0x07, _copr_not_available,	DA_386IGATE, DPL_KERL);
	init_idt_desc(0x08, _double_fault,		DA_386IGATE, DPL_KERL);
	init_idt_desc(0x09, _copr_seg_overrun,		DA_386IGATE, DPL_KERL);
	init_idt_desc(0x0a, _inval_tss,			DA_386IGATE, DPL_KERL);
	init_idt_desc(0x0b, _segment_not_present,	DA_386IGATE, DPL_KERL);
	init_idt_desc(0x0c, _stack_exception,		DA_386IGATE, DPL_KERL);
	init_idt_desc(0x0d, _general_protection,	DA_386IGATE, DPL_KERL);
	init_idt_desc(0x0e, _page_fault,		DA_386IGATE, DPL_KERL);
	init_idt_desc(0x0f, _copr_error,		DA_386IGATE, DPL_KERL);

	/*-----------------------------------------------------------------------------
	 *  0x10 - 0x1f	reserved
	 *
	 *  from now on, set vectors casually
	 *  0x20 - 0x2f	IRQ 0-15
	 *-----------------------------------------------------------------------------*/
	init_idt_desc(0x20, _hwint00, DA_386IGATE, DPL_KERL);
	init_idt_desc(0x21, _hwint01, DA_386IGATE, DPL_KERL);
	init_idt_desc(0x22, _hwint02, DA_386IGATE, DPL_KERL);
	init_idt_desc(0x23, _hwint03, DA_386IGATE, DPL_KERL);
	init_idt_desc(0x24, _hwint04, DA_386IGATE, DPL_KERL);
	init_idt_desc(0x25, _hwint05, DA_386IGATE, DPL_KERL);
	init_idt_desc(0x26, _hwint06, DA_386IGATE, DPL_KERL);
	init_idt_desc(0x27, _hwint07, DA_386IGATE, DPL_KERL);
	init_idt_desc(0x28, _hwint08, DA_386IGATE, DPL_KERL);
	init_idt_desc(0x29, _hwint09, DA_386IGATE, DPL_KERL);
	init_idt_desc(0x2a, _hwint10, DA_386IGATE, DPL_KERL);
	init_idt_desc(0x2b, _hwint11, DA_386IGATE, DPL_KERL);
	init_idt_desc(0x2c, _hwint12, DA_386IGATE, DPL_KERL);
	init_idt_desc(0x2d, _hwint13, DA_386IGATE, DPL_KERL);
	init_idt_desc(0x2e, _hwint14, DA_386IGATE, DPL_KERL);
	init_idt_desc(0x2f, _hwint15, DA_386IGATE, DPL_KERL);

	/*-----------------------------------------------------------------------------
	 *  system call, DPL_USER -> RING-3
	 *-----------------------------------------------------------------------------*/
	init_idt_desc(0x90, _sys_call, DA_386IGATE, DPL_USER);
}

void init_idt_desc(int vector, HANDLER handler, u8 type, u8 privilege)
{
	GATE *p = &idt[vector];
	u32 offset = (u32) handler;

	p->offset_low = (u16) offset;
	p->selector = SELECTOR_FLAT_C;
	p->dcount = 0;
	p->attr = type | (privilege << 5);
	p->offset_high = (u16) (offset >> 16);
}

void init_gdt_gate(int vector, HANDLER handler, u8 type, u8 privilege)
{
	GATE *p = &idt[vector];
	u32 offset = (u32) handler;

	p->offset_low = (u16) offset;
	p->selector = SELECTOR_FLAT_C;
	p->dcount = 0;
	p->attr = type | (privilege << 5);
	p->offset_high = (u16) (offset >> 16);
}

void init_gdt_desc(int vector, u32 base, u32 limit, u16 type)
{
	DESCRIPTOR *p = &gdt[vector];

	p->limit_low = (u16) limit;
	p->base_low = (u16) base;
	p->base_mid = (u8) (base >> 16);
	p->attr_low = (u8) type;
	p->limit_attr_high = (u8) (limit >> 16) | (u8) (type >> 8);
	p->base_high = (u8) (base >> 24);
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  init_pcb
 *  Description:  
 * =====================================================================================
 */
void init_pcb()
{
	int i;
	int stack_top = (u32) stack_table + STACK_SIZE_ALL;
	PCB *p = pcb_table;

	u8 rpl = RPL_TASK;
	u8 dpl = DPL_TASK;
        u32 eflags = 0x1202;                                    /* IF = 1, IOPL = 1 */
	PROC *t = task_table;

	for (i = 0; i < PROC_SIZE;) {
		memcpy(&(p->ldt[0]), &gdt[1], 8);
		memcpy(&(p->ldt[1]), &gdt[2], 8);

		p->gs = SELECTOR_VIDEO | rpl;
		// these sregs all point to LDT, not GDT, so there's 0 and 8, not 8 and 0x10
		p->fs = 8 | TI_L << 2 | rpl;
		p->es = 8 | TI_L << 2 | rpl;
		p->ds = 8 | TI_L << 2 | rpl;
		p->edi = 0;
		p->esi = 0;
		p->ebp = 0;
		p->kernel_esp = 0;	//popad ignore this
		p->ebx = 0;
		p->edx = 0;
		p->ecx = 0;
		p->eax = 0;
		p->ret_addr = 0;
		p->eip = (u32) t->handler;
		p->cs = 0 | TI_L << 2 | rpl;
		p->eflags = eflags;
		p->user_esp = stack_top;
		p->ss = 8 | TI_L << 2 | rpl;

		p->sel_ldt = SELECTOR_LDT_1ST + (i << 3);	// = 0x28, TI = RPL = 0, in GDT
		p->ldt[0].attr_low = DA_C | dpl << 5;
		p->ldt[1].attr_low = DA_DRW | dpl << 5;
		p->pid = i;
		memcpy(p->name, t->name, 16);

		p->flag = 0;
		p->msg = 0;
		p->sendto = NO_PROC;
		p->recvfrom = NO_PROC;
		p->is_int = 0;
		p->queue_send = 0;
		p->queue_next = 0;

		init_gdt_desc((SELECTOR_LDT_1ST >> 3) + i, (u32) p->ldt,
			      LDT_SIZE * 8 - 1, DA_LDT);

		stack_top -= t->stack_size;
		p++;
		t++;
		i++;

		if (i == TASK_SIZE) {	// notice, just ==, so it's only 1 time to change them
			rpl = RPL_USER;
			dpl = DPL_USER;
			eflags = 0x202;	// users' IOPL = 0, IF = 1
			t = user_table;
		}
	}

	/* here's TASK's */
	/* 
	 * no matter how many the proc-0's ticks will b(including 0), proc-0 will always b the 1st 
	 * 'cause we have initialized  proc_current = 0
	 */
	pcb_table[0].ticks = pcb_table[0].priority = 15;
	pcb_table[1].ticks = pcb_table[1].priority = 15;
	pcb_table[2].ticks = pcb_table[2].priority = 15;
	pcb_table[3].ticks = pcb_table[3].priority = 15;
	/* here's USER's */
	pcb_table[4].ticks = pcb_table[4].priority = 5;
	pcb_table[5].ticks = pcb_table[5].priority = 5;
	pcb_table[6].ticks = pcb_table[6].priority = 5;

	pcb_table[0].tty = 0;
	pcb_table[1].tty = 0;
	pcb_table[2].tty = 0;
	pcb_table[3].tty = 0;
	pcb_table[4].tty = 1;
	pcb_table[5].tty = 2;
	pcb_table[6].tty = 3;
}

void init_tss()
{
	TSS *p = &tss;

	p->ss0 = SELECTOR_FLAT_RW;	// 0x0c works also, 'cause one int GDT, the other int LDT
	p->iobase = sizeof(tss);	// means no iobase

	// struct name doesn't like array name, it's on behalf of itself, not it's addr
	init_gdt_desc(SELECTOR_TSS >> 3, (u32) & tss, sizeof(tss) - 1,
		      DA_386TSS);
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  init_irq
 *  Description:  place corresponding func pointer in irq_handler_table
 * =====================================================================================
 */
void init_irq(int vector, HANDLER handler)
{
	/* irq_disable(vector); */
	irq_handler_table[vector] = handler;
	irq_enable(vector);
}
