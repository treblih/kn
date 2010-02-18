/*
 * =====================================================================================
 *
 *       Filename:  proc.c
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
#include "proto.h"
#include "global.h"

void transfer()
{
	MESSAGE m;
	init_msg(&m);
	int pid;

	while (1) {
		send_rec(RECEIVE, &m, ANY);
		pid = m.source;

		switch (m.type) {
		case GET_TICKS:
			m.u.m3.i1 = ticks;
			/*------------------------------------------------------------
			 * the m.source below is needed when u wanna send a msg,
			 * just like u name
			 *-----------------------------------------------------------*/
			m.source = TRANSFER;
			send_rec(SEND, &m, pid);
			break;
		default:
			panic("unknown msg type\n");
			break;
		}
		//         printf("transfer has done this job: %d\n", m.type);
	}
}

void TestA()
{
	while (1) {
		printf("<ticks: %x>", get_ticks());
		delay(200);
	}
}

void TestB()
{
	while (1) {
		printf("%d ", 53);
		delay(200);
	}
}

void TestC()
{
	while (1) {
		printf("%c", 'C');
		delay(200);
	}
}

void delay(int ms)
{
	int i = get_ticks();
	while ((get_ticks() - i) * 10 < ms) {
	}
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  schedule
 *  Description:  always runs in RING-0
 *  		  proc -> int -> RING-0 -> schedule() -> here
 * =====================================================================================
 */
void schedule()
{
	int max = 0;                                            /* just a lookout */
	PCB *p;

	while (!max) {
		for (p = pcb_table; p < pcb_table + PROC_SIZE; p++) {

			/*------------------------------------------------------------
			 *  make sure the blocked one wouldn't be scheduled
			 *-----------------------------------------------------------*/
			if (p->flag == 0) {
				/*----------------------------------------------------
				 *  choose the richest one in ticks
				 *---------------------------------------------------*/
				if (p->ticks > max) {
					max = p->ticks;
					proc_current = p;
				}
			}
		}

		/*--------------------------------------------------------------------
		 *  when all proc's ticks == 0, 
		 *  init them again using the respective default PRIORITY
		 *-------------------------------------------------------------------*/
		if (!max) {
			for (p = pcb_table; p < pcb_table + PROC_SIZE; p++) {

				/*----------------------------------------------------
				 *  ignore the blocked one
				 *---------------------------------------------------*/
				if (p->flag == 0) {
					p->ticks = p->priority;
				}
			}
		}
	}
}
