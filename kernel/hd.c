/*
 * =====================================================================================
 *
 *       Filename:  hd.c
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
#include "hd.h"
#include "protect.h"
#include "proc.h"
#include "tty.h"
#include "proto.h"
#include "global.h"
#include "string.h"

void hd_ctrl_regs(HD_CMD_REGS *);

void hd()
{
	MESSAGE m;
	init_msg(&m);
	int pid;

	while (1) {
		send_rec(RECEIVE, &m, ANY);
		pid = m.source;
		printf("HD recv from %d\n", m.source);

		switch (m.type) {
		case DEV_OPEN:
			hd_identify(0);
			/* notice that the m.source below is needed when u wanna send a msg, just like u name */
			m.source = HD;
			send_rec(SEND, &m, pid);
			break;
		default:
			panic("hd_drive had got an unknown msg type\n");
			break;
		}
	}
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  init_hd
 *  Description:  called in init_i8259(), so it runs in RING-0
 * =====================================================================================
 */
void init_hd()
{
	/*-----------------------------------------------------------------------------
	 *  BIOS data area(BDA) => 0x400 - 0x500
	 *  got it when POST
	 *-----------------------------------------------------------------------------*/
	u8 how_many_hd = *(u8 *) 0x475;

	/*-----------------------------------------------------------------------------
	 *  can't use printf/assert here, 'cause this part of initialization,
	 *  no tty now
	 *-----------------------------------------------------------------------------*/
	disp_str("u have ");
	disp_int(how_many_hd);
	disp_str(" hard-disk(s)\n");

	init_irq(14, handler_hd);                         /* IRQ-14 */
}

void hd_identify(int is_slave)
{
	MESSAGE m;
	HD_CMD_REGS cmd_regs;
	char hd_buf[512] = { 0 };

	cmd_regs.device = SET_DEVICE(0, is_slave, 0);
	cmd_regs.command = HD_INFO;

	/* tell hd what i wanna do */
	hd_ctrl_regs(&cmd_regs);
	/*
	 * hd has got it, and preparing the answer,
	 * so we're right here waiting waiting'n waiting... waiting for just a interrupt. so poor
	 */
	send_rec(RECEIVE, &m, INT);
	hd_info_read(hd_buf, HD_REG_DATA, 512);
	hd_info_print((u16 *) hd_buf);
}

void hd_ctrl_regs(HD_CMD_REGS * cmd_regs)
{
	if (!waiting4u(HD_REG_CMD, MASK_BSY, 0, 10000)) {
		panic
		    ("waiting 4 u 2 long... 0x1f7 has consumed all my patience... \n");
	}

	out_byte(HD_REG_CRTL, 0);
	out_byte(HD_REG_FEAT, cmd_regs->feature);
	out_byte(HD_REG_CNT, cmd_regs->count);
	out_byte(HD_REG_LBAL, cmd_regs->lba_low);
	out_byte(HD_REG_LBAM, cmd_regs->lba_mid);
	out_byte(HD_REG_LBAH, cmd_regs->lba_high);
	out_byte(HD_REG_DEV, cmd_regs->device);
	out_byte(HD_REG_CMD, cmd_regs->command);
}

int waiting4u(u16 reg, u8 mask, int ideal_val, int timeout)
{
	int t = get_ticks();
	while ((get_ticks() - t) * 10 < timeout) {
		if ((in_byte(reg) & mask) == ideal_val) {
			return 1;
		}
	}
	return 0;
}

void hd_info_print(u16 * buf)
{
	int i, j;
	char num[64] = { 0 };
	struct s_info {
		int offset;
		int len;
		char *desc;
	} info[2] = { {
	10, 20, "HD SN"}, {
	27, 40, "HD Model"}};

	for (i = 0; i < 2; i++) {
		assert(64 > info[i].len);
		char *p = (char *)&buf[info[i].offset];
		for (j = 0; j < (info[i].len / 2); j++) {
			num[j * 2 + 1] = *p++;
			num[j * 2] = *p++;
		}
		/* so important */
		num[j * 2] = 0;
		printf("%s: %s\n", info[i].desc, num);
	}

	int capabilities = buf[49];
	printl("LBA supported: %s\n", (capabilities & 0x0200) ? "Yes" : "No");

	int cmd_set_supported = buf[83];
	printl("LBA48 supported: %s\n",
	       (cmd_set_supported & 0x0400) ? "Yes" : "No");

	int sectors = ((int)buf[61] << 16) + buf[60];
	printl("HD size: %dMB\n", sectors * 512 / 1000000);
}

void send_hd_int(int pid)
{
	PCB *p = pid + pcb_table;

	// if ((p->flag == RECEIVING) && (p->recvfrom == ANY || p->recvfrom == INT)) {
	if ((p->flag == RECEIVING) && (p->recvfrom == INT)) {
		assert(p->msg);
		p->msg->source = INT;
		p->msg->type = INT_HARD;

		p->is_int = 0;
		clean_elements(p, RECEIVING);
	} else {
		p->is_int = 1;
	}
}
