#ifndef         _PROTO_
#define         _PROTO_


/*
 * for portability
 * GCC has built-in __GNUC__ 
 */
#ifndef		__GNUC__
#define 	__attribute__(x)  /* nop */
#endif

void	_divide_error();
void	_single_step_exception();
void	_nmi();
void	_breakpoint_exception();
void	_overflow();
void	_bounds_check();
void	_inval_opcode();
void	_copr_not_available();
void	_double_fault();
void	_copr_seg_overrun();
void	_inval_tss();
void	_segment_not_present();
void	_stack_exception();
void	_general_protection();
void	_page_fault();
void	_fpu_fault();
void	_align_fault();
void	_machine_abort();
void	_simd_fault();
void	_ignore();

void	 _hwint00();
void	 _hwint01();
void	 _hwint02();
void	 _hwint03();
void	 _hwint04();
void	 _hwint05();
void	 _hwint06();
void	 _hwint07();
void	 _hwint08();
void	 _hwint09();
void	 _hwint10();
void	 _hwint11();
void	 _hwint12();
void	 _hwint13();
void	 _hwint14();
void	 _hwint15();
void	 _sys_call();

void    init_gdt();
void    init_idt();
void    init_i8259();
void    init_pcb();
void    init_tss();
void    init_idt_desc(int, HANDLER, u8, u8);
void    init_gdt_gate(int, HANDLER, u8, u8);
void    init_gdt_desc(int, u32, u32, u16);
void    init_irq(int, HANDLER);
void    init_irq_slave(int, HANDLER);
void    init_clock();
void    init_keyboard();
void    init_hd();
void    init_console(CONSOLE*, int);

void    _asm_debug();

void    disp_int(int);
char*   itoa(char*, int);/* 数字前面的 0 不被显示出来, 比如 0000B800 被显示成 B800 */
int     i2a(char** p, int num, int dividend);

void    TestA();
void    TestB();
void    TestC();
void    tty();
void    transfer();

void    sys_printx(PCB*, char*);
int     sys_sendrec(PCB*, int, MESSAGE*, int);

void    delay(int);
void    schedule();
void    handler_clock();
void    handler_keyboard();
void    handler_hd();
void    handler_exception(int, int, int, int, int);
void    irq_show_num();

void    read_keyboard(TTY*);
void    kbuf_to_tbuf(u32, TTY*);
void    key_output(CONSOLE*, char);
void    crt_ctrl_reg(u8, u8, int);
void    scroll_screen(CONSOLE*, int);
void    select_tty(TTY*);
void    clean_screen(CONSOLE*);


int     strlen(char*);

/* in reality no need __attribute__ here, 'cause PRINTF PANIC has the
 * same name with ones in lib, so GCC may check them itself */
int     printf(const char*, ...)			/* here's no ';' */
	__attribute__((__format__(__printf__, 1, 2)));	/* ';' is here */
int     vsprintf(char*, const char*, va_list);
void    panic(const char*, ...)
	__attribute__((__format__(__printf__, 1, 2)));

void    spin(char*);
#define         printl          printf

int     get_ticks();

int     send_rec(int, MESSAGE*, int);
int     msg_send(PCB*, MESSAGE*, int);
int     msg_receive(PCB*, MESSAGE*, int);
void    block(PCB*, MESSAGE*);
void    unblock(PCB*);
int     dead_lock(PCB*, PCB*);
void    assert_ipc_clean(PCB*);
void    clean_elements(PCB*, int);
void    dirty_elements(PCB*, int, MESSAGE*, int);
void    init_msg(MESSAGE*);

void    hd();
void    init_hd();
void    send_hd_int(int);
void    hd_identify(int);
int     waiting4u(u16, u8, int, int);
void    hd_info_print(u16*);

void    fs();

#endif
