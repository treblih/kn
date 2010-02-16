/*
 * =====================================================================================
 *
 *       Filename:  ipc.c
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
#include "syscall.h"
#include "global.h"
#include "string.h"

/* PCB* is pushed by sendrec(a macro) */
int send_rec(int mode, MESSAGE * m, int pid)
{
	assert((mode == SEND || mode == RECEIVE) || mode == BOTH);
	assert(m);
	assert((pid >= 0 && pid < PROC_SIZE) || pid == ANY || pid == INT);

	int ret = 0;

	switch (mode) {
	case BOTH:
		ret = sendrec(SEND, m, pid);
		if (ret == 0) {
			ret = sendrec(RECEIVE, m, pid);
		}
		break;
	case SEND:
	case RECEIVE:
		ret = sendrec(mode, m, pid);
		break;
	default:
		assert(mode == BOTH || mode == SEND || mode == RECEIVE);
		break;
	}

	return ret;
}

/*********************************************************************************************************
 *                              all below executed after INT 0x90
 *
 *                                      so run in RING-0
 *
 *                              they need arg PCB*, except auxiliary
 *                              PCB* was pushed by sendrec(a macro)
 *********************************************************************************************************/
int sys_sendrec(PCB * proc_current, int mode, MESSAGE * m, int pid)
{
	assert(proc_current >= 0 && proc_current < pcb_table + PROC_SIZE);

	/* so here not including BOTH */
	assert(mode == SEND || mode == RECEIVE);
	assert(m);
	assert((pid >= 0 && pid < PROC_SIZE) || pid == ANY || pid == INT);
	assert(int_counter == 0);

	/* every proc should do this itself */
	//m->source       =       proc2pid(proc_current);

	if (mode == SEND) {
		return msg_send(proc_current, m, pid);
	} else if (mode == RECEIVE) {
		return msg_receive(proc_current, m, pid);
	} else {
		panic("\ninvalid mode\nUSAGE: SEND->%d, RECEIVE->%d\n", SEND,
		      RECEIVE);
	}

	/* can't arrive here */
	return 0;
}

/*
 * ANY/INT can just appear in receiving target, here we send, can only need a certain target
 * namely, the arg receiver should b a num except ANY/INT
 * u should b clear that we r not able to send to ANY, 'cause there's no queue of send-to, that's sound so funny!
 * nevertheless we can recerive from ANY
 */
int msg_send(PCB * proc_current, MESSAGE * m, int receiver)
{
	assert(proc_current >= 0 && proc_current < pcb_table + PROC_SIZE);
	assert(m);
	/* we can't send INT to a certain proc */
	assert((receiver >= 0 && receiver < PROC_SIZE) || receiver == ANY);

#if 0
	printf("proc: %d ", proc2pid(proc_current));
	printf("send to: %d ", receiver);
#endif
	PCB *dst = pcb_table + receiver;

	/* including self to self */
	//assert(!dead_lock(proc_current, dst));

	if (dst->flag == RECEIVING
	    && (dst->recvfrom == ANY
		|| dst->recvfrom == proc2pid(proc_current))) {
		assert(dst->msg);
		memcpy(dst->msg, m, MESSAGE_SIZE);

		/* clean dst's dirty elements */
		clean_elements(dst, RECEIVING);

#if 0
		assert_ipc_clear(proc_current);
		assert_ipc_clear(dst);
#endif
	} else {
		/* make proc_current's elements dirty */
		dirty_elements(proc_current, SENDING, m, receiver);

		/* add proc_current to dst's queue_send */
		if (!dst->queue_send) {
			dst->queue_send = proc_current;
		} else {
			PCB *p = dst->queue_send;
			while (p->queue_next) {
				p = p->queue_next;
			}
			p->queue_next = proc_current;
		}
		/* make sure no one followed */
		proc_current->queue_next = 0;

		block(proc_current, m);
	}

	return 0;
}

int msg_receive(PCB * proc_current, MESSAGE * m, int sender)
{
	assert(proc_current >= 0 && proc_current < pcb_table + PROC_SIZE);
	assert(m);
	/* but a proc can receive an INT */
	assert((sender >= 0 && sender < PROC_SIZE) || sender == ANY
	       || sender == INT);

	/* arg sender may not b a certain num, so here can't evaluate the PCB-pointer src */
	PCB *src = 0;
	PCB *previous = 0;

	/* copy is a lookout */
	int copy = 0;

	/* not self to self */
	assert(sender != proc2pid(proc_current));

	// if (proc_current->is_int && (sender == INT)) {
	if (proc_current->is_int) {
		// assert(sender == ANY || sender == INT);
		assert(sender == INT);
		m->source = INT;
		m->type = INT_HARD;

		proc_current->is_int = 0;
		return 0;
	}

	switch (sender) {
		/* not a certain pid, we can't check it's flag, just check our queue_send */
	case ANY:
		if (proc_current->queue_send) {
			src = proc_current->queue_send;
			assert(src->msg);
			copy = 1;
		}
		break;
	case INT:
		break;
	default:
		src = pcb_table + sender;
		// assert(src->msg);
		if (src->flag == SENDING
		    && src->sendto == proc2pid(proc_current)) {
			PCB *p = proc_current->queue_send;

			/*
			 * chech whether src has already been in proc_current's queue_send
			 * and make previous b a node in the link-queue before src
			 */
			while (p) {
				if (p == src) {
					copy = 1;
					break;
				}
				previous = p;
				p = p->queue_next;
			}
		}
		break;
	}

	if (copy) {
		memcpy(m, src->msg, MESSAGE_SIZE);

		/* remove the src-node from the link-queue */
		if (previous) {	/* means src is not the 1st in the queue */
			previous->queue_next = src->queue_next;
		}
		/* src is the 1st */
		else {
			proc_current->queue_send = src->queue_next;
		}
		src->queue_next = 0;

		/* then clean dirty elements */
		clean_elements(src, SENDING);
	} else {
		dirty_elements(proc_current, RECEIVING, m, sender);
	}

	return 0;
}

void block(PCB * p, MESSAGE * m)
{
	assert(p->flag);
	schedule();
}

void unblock(PCB * p)
{
	assert(!p->flag);
}

int dead_lock(PCB * p, PCB * dst)
{
	int i;
	PCB *dst2 = dst;

	while (dst->sendto) {
		if ((dst->sendto == proc2pid(p))) {
			i = 1;
			break;
		}
		dst = pcb_table + dst->sendto;
	}

	if (i) {
		//printf("\nloop: %s-> ", p->name);
		while (p != dst2) {
			//printf("%s-> ", dst2->name);
			dst2 = pcb_table + dst->sendto;
		}
		//printf("%s\n", p->name);

		return -1;
	}

	return 0;
}

#if 0
void assert_ipc_clear(PCB * p, int flag_type, MESSAGE * m, int pid)
{
	flag_type == SENDING ?
	    assert(p->flag = 0),
	    assert(p->msg = 0),
	    assert(p->sendto = 0),
	    assert(p->recvfrom = 0),
	    assert(p->int_msg = 0),
	    assert(p->queue_send = 0), assert(p->queue_next = 0), : assert(1);
}
#endif

void clean_elements(PCB * my_target, int flag_type)
{
	my_target->flag &= ~flag_type;
	my_target->msg = 0;
	(flag_type == SENDING) ? (my_target->sendto =
				  NO_PROC) : (my_target->recvfrom = NO_PROC);

	unblock(my_target);
}

void dirty_elements(PCB * myself, int flag_type, MESSAGE * m, int pid)
{
	myself->flag |= flag_type;
	myself->msg = m;
	(flag_type == SENDING) ? (myself->sendto = pid) : (myself->recvfrom =
							   pid);

	block(myself, m);
}

void init_msg(MESSAGE * m)
{
	memset(m, 0, MESSAGE_SIZE);
	m->source = proc2pid(proc_current);
}
