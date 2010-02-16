.include        "include/sconst.inc"

.globl  _start
.globl  _restart
.globl  _re_restart
.globl  _stack_kernel

        .bss
        .align  32
.fill   2048
_stack_kernel:

        .text
        .align  32
_start:
        mov     $_stack_kernel,  %esp
        sgdt    (gdt_ptr)

        call    cstart
        lgdt    (gdt_ptr)
        lidt    (idt_ptr)

        jmp     $SELECTOR_FLAT_C, $flush      # flush CS's cache
flush:
        xor     %eax,           %eax
        movw    $SELECTOR_FLAT_RW,      %ax     # flush other sregs except GS
        mov     %ax,    %ds
        mov     %ax,    %es
        mov     %ax,    %fs
        mov     %ax,    %ss
        movw    $SELECTOR_TSS,  %ax
        ltr     %ax

_restart:              # no need to sti explicitly, 'cause IF in PCB has been evaluated by 1
#call    _asm_debug
        mov     (proc_current),      %esp       # leave from stack_kernel for PCB-start, always here
        lldt    PCB_SEL_LDT(%esp)
        lea     PCB_SS(%esp),     %eax
        mov     %eax,   (tss + TSS_ESP0)

_re_restart:            # change sregs' value from GDT to LDT, except SS
        pop     %gs
        pop     %fs
        pop     %es
        pop     %ds
        popa

        add     $4,     %esp
        decb    (int_counter)
        iret
        # then get into Process, meanwhile the IOPL'n IF = 1
