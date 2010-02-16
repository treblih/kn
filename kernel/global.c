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


char            int_counter;     
int             disp_pos;
int             ticks;

void*           sys_call_table[SYS_CALL_SIZE] = {sys_printx,
                                                 sys_sendrec};

/* make sure that       proc_current = tty'pid */
PCB*            proc_current    =       pcb_table;
TTY*            tty_current     =       tty_table;

DESCRIPTOR      gdt[GDT_SIZE];
u8              gdt_ptr[6];
GATE            idt[IDT_SIZE];
u8              idt_ptr[6];

TSS             tss;

PCB             pcb_table[PROC_SIZE];
PROC            task_table[TASK_SIZE] = {{tty,          STACK_TASK_SIZE,        "TTY"},
                                         {transfer,     STACK_TASK_SIZE,        "TRANSFER"},
                                         {hd,           STACK_TASK_SIZE,        "HD"},
                                         {fs,           STACK_TASK_SIZE,        "FS"}};
PROC            user_table[USER_SIZE] = {{TestA,        STACK_USER_SIZE,        "TestA"},
                                         {TestB,        STACK_USER_SIZE,        "TestB"},
                                         {TestC,        STACK_USER_SIZE,        "TestC"}};

HANDLER         irq_handler_table[16];

char            stack_table[STACK_SIZE_ALL];

CONSOLE         console_table[CONSOLE_SIZE];
TTY             tty_table[CONSOLE_SIZE];
