/* please notice the sequence of the including list
 * we put the one who needs no other *.h at the beginning
 * follow the principle, const.h is 1st
 */

#include "type.h"
#include "protect.h"
#include "proc.h"
#include "tty.h"
#include "proto.h"
#include "global.h"


static void write_tty(TTY*);
static void init_tty(TTY*, int);


void
tty()
{
        printl("aaa\n");
        // printf("proc_current in tty: %d\n", proc2pid(proc_current));
        int     i       =       0;
        TTY*    p_tty   =       tty_table;

        /* init tty_table */
        for ( ; p_tty < tty_table + CONSOLE_SIZE || i < 3; p_tty++, i++) {
                init_tty(p_tty, i);
        }
        /* no need to set cursor and start_addr */

        /* 
         * here we set cursor and start_addr with the elements in TTY 
         * not p_tty, 'cause it has been out of the tty_table
         */
        select_tty(tty_table);
	/* assert(0); */

        while (1) {
                for (p_tty = tty_table; p_tty < tty_table + CONSOLE_SIZE; p_tty++) {

                        /* judge tty_current, we have evaluated it in select_tty() */
                        if (tty_current == p_tty) {
                                read_keyboard(p_tty);
                                write_tty(p_tty);
                        }
                }
        }
}

void
kbuf_to_tbuf(u32 keyval, TTY* p_tty)
{
        if (!(keyval & FLAG_EXT)) {
                if (p_tty->count < KEY_BUF_SIZE) {                           // no equals
                        if (p_tty->head == p_tty->buf + KEY_BUF_SIZE) {           // means head has pointed out
                                p_tty->head         =       p_tty->buf;
                        }
                        *p_tty->head        =       keyval;
                        p_tty->head++;
                        p_tty->count++;
                }
        }
}

static void
init_tty (TTY* p_tty, int i)
{
        p_tty->head = p_tty->tail = p_tty->buf;

        /*
         * if this sentence below were in init_console(CONSOLE* ...), it should be init_console(TTY*)
         * the reason we all know, just be careful
         */
        p_tty->p_console = console_table + i;

        init_console(p_tty->p_console, i);
}

static void
write_tty(TTY* p_tty)
{
        /* diff with read_keyboard_buf(), must have the count judge */
        if (p_tty->count) {                                     // diff with read_keyboard_buf(), must have the count judge
                if (p_tty->tail == p_tty->buf + KEY_BUF_SIZE) {           // means tail has pointed out
                        p_tty->tail         =       p_tty->buf;
                }

                char ch       =       (char)*p_tty->tail;
                p_tty->tail++;
                p_tty->count--;

                key_output(p_tty->p_console, ch);
        }
}

void
select_tty(TTY* p_tty)
{
        if (p_tty >= tty_table && p_tty < tty_table + CONSOLE_SIZE) {

                /* for the judge of tty_current below */
                tty_current = p_tty;

                crt_ctrl_reg(CURSOR_H, CURSOR_L, p_tty->p_console->cursor);

                /* use current_addr, not original_addr
                 * 'cause current_addr is variable, but original_addr is static
                 */
                crt_ctrl_reg(START_ADDR_H, START_ADDR_L, p_tty->p_console->current_addr);
        }
}

void
sys_printx(PCB* proc_current, char* buf)
{
        char*   p       =       buf;

        for ( ; *p; p++) {
                if ((*p == MAGIC_ASSERT) || (*p == MAGIC_PANIC)) {
                        continue;
                }
                key_output(tty_table[proc_current->tty].p_console, *p);
        }

        if ((*buf == MAGIC_PANIC) || ((*buf == MAGIC_ASSERT) && (proc2pid(proc_current) < TASK_SIZE))) {
                /* hlt only can halt cpu until next interupt occurs */
                __asm__ __volatile__("cli");
                __asm__ __volatile__("hlt");
        }
}
