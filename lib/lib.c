/*
 * =====================================================================================
 *
 *       Filename:  lib.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  18.02.10
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
#include "string.h"

void disp_int(int input)
{
	char output[16];
	itoa(output, input);
	disp_str(output);
}

char *itoa(char *str, int num)
{				/* 数字前面的 0 不被显示出来, 比如 0000B800 被显示成 B800 */
	char *p = str;
	char ch;
	int i;
	int flag = 0;

	if (num == 0) {
		*p++ = '0';
	} else {
		for (i = 28; i >= 0; i -= 4) {
			ch = (num >> i) & 0xF;
			if (flag || (ch > 0)) {
				flag = 1;
				ch += '0';
				if (ch > '9') {
					ch += 7;
				}
				*p++ = ch;
			}
		}
	}

	*p = 0;

	return str;
}

/* return the bit of a given num */
int i2a(char **p, int num, int dividend)
{
	int bit = 1;
	int quotient = num / dividend;
	int remainder = num % dividend;

	if (quotient) {
		bit += i2a(p, quotient, dividend);
	}
	*(*p)++ = (remainder < 10) ? (remainder + '0') : (remainder - 10 + 'A');

	return bit;
}

int strlen(char *buf)
{
	int i = 0;
	while (*buf++) {
		i++;
	}

	return i;
}

void
/* __func__ needs const char *, diff with __FILE__ __LINE__ */
assert_declare(char *exp, char *file, char *base_file, int line,
	       const char *func)
{
	/* the 1st can't b '\n', must b '\003' */
	printl
	    ("%c assert(%s)\nfailed-> file: %s, base_file: %s, line: %d, func: %s\n",
	     MAGIC_ASSERT, exp, file, base_file, line, func);

	/* below we pause the user-process(r2-r3) which has assert occured */
	spin("assert");
}

void spin(char *string)
{
	printl("%s will always spin here\n", string);
	__asm__ __volatile__("jmp .");
}

/* PANIC is diff from ASSERT, it's also para-varience, like printf */
void panic(const char *fmt, ...)
{
	char buf[256] = { 0 };
	va_list argv = (va_list) (&fmt + 1);
	vsprintf(buf, fmt, argv);

	/* the 1st can't b '\n', must b '\002' */
	printl("%c !! PANIC !!\n%s\n", MAGIC_PANIC, buf);
}

int get_ticks()
{
	MESSAGE m;
	init_msg(&m);
	m.source = proc2pid(proc_current);
	m.type = GET_TICKS;
	send_rec(BOTH, &m, TRANSFER);
	return m.u.m3.i1;
}
