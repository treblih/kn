#ifndef         _PROC_
#define         _PROC_



#define         USER_SIZE       3
#define         TASK_SIZE       4
#define         PROC_SIZE       USER_SIZE + TASK_SIZE           /* notice sequence */
#define         STACK_USER_SIZE 0x1000
#define         STACK_TASK_SIZE 0x1000
#define         STACK_SIZE_ALL  \
	STACK_USER_SIZE * USER_SIZE + STACK_TASK_SIZE * TASK_SIZE /* same as above */

#define         MESSAGE_SIZE    sizeof(MESSAGE)

#define         SENDING         0x02
#define         RECEIVING       0x04

#define         SEND            1
#define         RECEIVE         2
#define         BOTH            3

#define         ANY             PROC_SIZE + 10 
#define         NO_PROC         -1
#define         INT             -2

#define         INT_HARD        0
#define         GET_TICKS       1

#define         TRANSFER        1
#define         HD              2
#define         FS              3

#define         proc2pid(p)     (p - pcb_table)


struct mess1 {
	int i1;
	int i2;
	int i3;
	int i4;
};
struct mess2 {
	void* p1;
	void* p2;
	void* p3;
	void* p4;
};
struct mess3 {
	int	i1;
	int	i2;
	int	i3;
	int	i4;
	u64	l1;
	u64	l2;
	void*	p1;
	void*	p2;
};
typedef struct {
	int source;
	int type;
	union {
		struct mess1 m1;
		struct mess2 m2;
		struct mess3 m3;
	} u;
} MESSAGE;

typedef struct s_pcb {
	/*-----------------------------------------------------------------------------
	 *  these sregs all point to LDT, not GDT, NOTICE!!!
	 *-----------------------------------------------------------------------------*/
        u32     gs;
        u32     fs;
        u32     es;
        u32     ds;
        u32     edi;
        u32     esi;
        u32     ebp;
        u32     kernel_esp;                                     /* popad ignore this */
        u32     ebx;
        u32     edx;
        u32     ecx;
        u32     eax;
        u32     ret_addr;
        u32     eip;
        u32     cs;
        u32     eflags;
        u32     user_esp;
        u32     ss;

        u32     sel_ldt;
        DESCRIPTOR      ldt[LDT_SIZE];
        u32     pid;
        char    name[16];

        int     ticks;
        int     priority;
        int     tty;

        int     flag;
        MESSAGE* msg;
        int     sendto;
        int     recvfrom;
        int     is_int;
        struct s_pcb*    queue_send;
        struct s_pcb*    queue_next;
} PCB;

typedef struct
{
        HANDLER handler;
        int     stack_size;
        char    name[16];
} PROC;
#endif
