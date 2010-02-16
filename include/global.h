#ifndef         _GLOBAL_
#define         _GLOBAL_

#define         SYS_CALL_SIZE   2

extern          char            int_counter;
extern          int             ticks;
extern          int             disp_pos;

extern          void*           sys_call_table[SYS_CALL_SIZE];
extern		PCB*            proc_current;
extern          TTY*            tty_current;

extern          char            stack_table[STACK_SIZE_ALL];
extern          DESCRIPTOR      gdt[GDT_SIZE];
extern          u8              gdt_ptr[6];
extern          GATE            idt[IDT_SIZE];
extern          u8              idt_ptr[6];

extern          TSS             tss;

extern          PCB             pcb_table[PROC_SIZE];
extern          PROC            task_table[TASK_SIZE];
extern          PROC            user_table[USER_SIZE];

extern          HANDLER         irq_handler_table[16];

extern          CONSOLE         console_table[CONSOLE_SIZE];
extern          TTY             tty_table[CONSOLE_SIZE];


#endif
