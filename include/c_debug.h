#ifndef         _C_DEBUG_
#define         _C_DEBUG_



#define c_debug()    \
        __asm__ __volatile__("c_debug: \n\t" \
                         "push      %eax \n\t"      \
                         "push  %esi \n\t"      \
                         "movl       $0x90000,       %esi \n\t"       \
                         "movl  %ds : (%esi),   %eax \n\t"       \
                         "pushl %eax \n\t"       \
                         "movl  $0xcb,  %ds : (%esi) \n\t"      \
                         "pushl  %cs \n\t"       \
                         "pushl $back \n\t"     \
                         ".byte 0xea \n\t"     \
                         ".long 0x90000 \n\t"        \
                         ".word 0x8 \n\t"     \
                         "back: \n\t"   \
                         "popl  %eax \n\t"      \
                         "movl  %eax,   %ds : (%esi) \n\t"      \
                         "pop   %esi \n\t"      \
                         "pop   %eax \n\t")
#endif
