/* please notice the sequence of the including list
 * we put the one who needs no other *.h at the beginning
 * follow the principle, const.h is 1st
 */

#include "io.h"
#include "type.h"
#include "protect.h"
#include "proc.h"
#include "tty.h"
#include "proto.h"
#include "global.h"


void
init_console(CONSOLE* p_console, int i)
{
        /* memory == cursor * 2, so here we need (VMEM_SIZE >> 1) */
        int     vmem_size_avr   =       (VMEM_SIZE >> 1) / CONSOLE_SIZE;

        p_console->current_addr = p_console->original_addr = i * vmem_size_avr;
        p_console->vmem_limit = vmem_size_avr;

        if (i == 0) {
        //         p_console->cursor        =       p_console->original_addr;
                p_console->cursor        =       disp_pos / 2;
        }
        else {
                p_console->cursor        =       p_console->original_addr;
                key_output(p_console, i + '0');
                key_output(p_console, '#');
        }
}

void
/* notice here can't use INT cursor, it's just a copy. so what we typed will always at the same position */
key_output(CONSOLE* p_console, char ch)
{
        /* memory == cursor * 2 */
        char*     p_video =       (char*)(VMEM_BASE + p_console->cursor * 2);

        /*
         * 'cause p_console->cursor gets started from 0, so
         * <  the last char in a line
         * =  the 1st  char in a line
         * >  the 2nd  char in a line
         */

        switch (ch) {
        case '\n':
                if (p_console->cursor + SCREEN_WIDTH < p_console->original_addr + p_console->vmem_limit) {

                        /*
                         * (0x8000 >> 1 / 3) != (n * SCREEN_WIDTH)
                         * if it's ==, we could use
                         * cursor = (cursor / width + 1) * width
                         */
                        p_console->cursor = ((p_console->cursor - p_console->original_addr) / SCREEN_WIDTH + 1) \
                                            * SCREEN_WIDTH + p_console->original_addr;
                }
                break;
        case '\b':
                /* >, not >= */
                if (p_console->cursor > p_console->original_addr) {

                        /*
                         * ASCII SPACE == 0x20
                         * if u press ENTER, sys will use 0x20 to fill the blanks
                         * jump the SPACE-sequence when it's very in the tail of last line
                         * if it's in the current line, delete SPACE 1 by 1
                         */
                        if ((!((p_console->cursor - p_console->original_addr) % SCREEN_SIZE)) && \
                               (*(p_video - 2) == 0x20)) {
                                do {
                                        p_video -= 2;
                                        p_console->cursor--;
                                } while (*(p_video - 2) == 0x20);
                        }
                        else {
                                *(p_video - 2) = ' ';
                                *(p_video - 1) = 7;
                                p_console->cursor--;
                        }

                        if (p_console->cursor < p_console->current_addr) {
                                scroll_screen(p_console, SCROLL_UP);
                        }
                }
                break;
        default:
                if (p_console->cursor < p_console->original_addr + p_console->vmem_limit) {
                        *p_video++      =       ch;
                        *p_video++      =       COLOR;
                        p_console->cursor++;
                        break;
                }
        }

        /* >=, not > */
        if (p_console->cursor >= p_console->current_addr + SCREEN_SIZE) {
                scroll_screen(p_console, SCROLL_DOWN);
        }

        /*
         * this judge is so significant.
         * else if TestA.B.C run in TTY2, we type in TTY0,
	 * we would't see the cursor any more
         */
        if (p_console == tty_current->p_console) {
                crt_ctrl_reg(CURSOR_H, CURSOR_L, p_console->cursor);
        }
}

void
crt_ctrl_reg(u8 reg_h, u8 reg_l, int data)
{
        __asm__ __volatile__("cli");
        out_byte(CRTC_ADDR_REG, reg_h);
        out_byte(CRTC_DATA_REG, data >> 8);
        out_byte(CRTC_ADDR_REG, reg_l);                      /* here ports is 32bits, not 8bits */
        out_byte(CRTC_DATA_REG, data);
        __asm__ __volatile__("sti");
}

void
scroll_screen(CONSOLE* p_console, int up)
{
        if (up) {
                if (p_console->current_addr > p_console->original_addr) {
                        p_console->current_addr -= SCREEN_WIDTH;
                }
        }
        else {
                if (p_console->current_addr + SCREEN_SIZE < p_console->original_addr + p_console->vmem_limit) {
                        p_console->current_addr += SCREEN_WIDTH;
                }
        }

        /* also important judge for avoid auto-switching between multi TTYs */
        if (p_console == tty_current->p_console) {
                crt_ctrl_reg(START_ADDR_H, START_ADDR_L, p_console->current_addr);
        }
}

void
clean_screen(CONSOLE* p_console)
{
        char*     p_video =       (char*)(VMEM_BASE + p_console->cursor * 2);

        /* no equals */
        while (p_console->cursor > p_console->current_addr) {
                *--p_video      =       7;
                *--p_video      =       ' ';
                p_console->cursor--;
        }

        if (p_console == tty_current->p_console) {
                crt_ctrl_reg(CURSOR_H, CURSOR_L, p_console->cursor);
        }
}
