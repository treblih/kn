#ifndef         _SYSCALL_
#define         _SYSCALL_


#define         SYS_CALL_SIZE   2


#define printx(buf)     \
        __asm__ __volatile__("mov       $0,     %%eax \n\t"     \
                         "int   $0x90 \n\t"     \
                         :      \
                         : "b"(buf)       \
                         : "ax")


#define sendrec(mode, m, pid)   \
({      \
        int  _i = 1;      \
        __asm__ __volatile__("int   $0x90 \n\t"     \
                         : "+a"(_i)     \
                         : "b"(mode), "c"(m), "d"(pid));        \
        _i;     \
})
#endif
