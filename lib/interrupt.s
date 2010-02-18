	.include        "include/sconst.inc"

	.globl	_divide_error
	.globl	_single_step_exception
	.globl	_nmi
	.globl	_breakpoint_exception
	.globl	_overflow
	.globl	_bounds_check
	.globl	_inval_opcode
	.globl	_copr_not_available
	.globl	_double_fault
	.globl	_copr_seg_overrun
	.globl	_inval_tss
	.globl	_segment_not_present
	.globl	_stack_exception
	.globl	_general_protection
	.globl	_page_fault
	.globl	_fpu_fault
	.globl	_align_fault
	.globl	_machine_abort
	.globl	_simd_fault
	.globl	_ignore

	.globl  _hwint00
	.globl  _hwint01
	.globl  _hwint02
	.globl  _hwint03
	.globl  _hwint04
	.globl  _hwint05
	.globl  _hwint06
	.globl  _hwint07
	.globl  _hwint08
	.globl  _hwint09
	.globl  _hwint10
	.globl  _hwint11
	.globl  _hwint12
	.globl  _hwint13
	.globl  _hwint14
	.globl  _hwint15

	.globl  _asm_debug
	.globl  _sys_call

#--------------------------------------------------------------------------------------------

_divide_error:
	push	$0xffffffff	# no err code
	push	$0x00		# vec_no	= 0
	jmp	exception
_single_step_exception:
	push	$0xffffffff	# no err code
	push	$0x01		# vec_no	= 1
	jmp	exception
_nmi:
	push	$0xffffffff	# no err code
	push	$0x02		# vec_no	= 2
	jmp	exception
_breakpoint_exception:
	push	$0xffffffff	# no err code
	push	$0x03		# vec_no	= 3
	jmp	exception
_overflow:
	push	$0xffffffff	# no err code
	push	$0x04		# vec_no	= 4
	jmp	exception
_bounds_check:
	push	$0xffffffff	# no err code
	push	$0x05		# vec_no	= 5
	jmp	exception
_inval_opcode:
	push	$0xffffffff	# no err code
	push	$0x06		# vec_no	= 6
	jmp	exception
_copr_not_available:
	push	$0xffffffff	# no err code
	push	$0x07		# vec_no	= 7
	jmp	exception
_double_fault:
	push	$0x08		# vec_no	= 8
	jmp	exception
_copr_seg_overrun:
	push	$0xffffffff	# no err code
	push	$0x09		# vec_no	= 9
	jmp	exception
_inval_tss:
	push	$0x0a		# vec_no	= a
	jmp	exception
_segment_not_present:
	push	$0x0b		# vec_no	= b
	jmp	exception
_stack_exception:
	push	$0x0c		# vec_no	= c
	jmp	exception
_general_protection:
	push	$0x0d		# vec_no	= d
	jmp	exception
_page_fault:
	push	$0x0e		# vec_no	= e
	jmp	exception
_fpu_fault:                     # yes, it's right. 'cause INTEL has reserved 0xf
	push	$0xffffffff	# no err code
	push	$0x10		# vec_no	= 0x10
	jmp	exception
_align_fault:
	push	$0x11		# vec_no	= 9
	jmp	exception
_machine_abort:
	push	$0x12
	jmp	exception
_simd_fault:
	push	$0xffffffff
	push	$0x13
	jmp	exception
_ignore:
	push	$0xffffffff
	push	$0x14
	jmp	exception

exception:
	call	handler_exception
	add	$8,    %esp	# skip vec_no & error_code to eip - cs -eflags
        hlt


.macro  hwint   vector
        call    save

	/* OCW1, close itself, so can't be re-entered by itself */
        inb     $IMRM
        orb     $1 << \vector,    %al
        outb    $IMRM

	/*-----------------------------------------------------------------------------
	 * OCW2, clears corresponding bit in ISR
	 * avoid low-level one interrupts high-level one,
	 * so set ICW4 == 0x01(manu), and clears ISR by ourself
	 * see in kernel/init.c -- init_i8259a()
	 * 
	 * only reset master
	 *-----------------------------------------------------------------------------*/
        mov     $EOI,   %eax
        outb    $ICRM

	/*-----------------------------------------------------------------------------
	 * from int-gate(excluding trap-gate), 
	 * cpu will "cli" - exe - "sti" automatically
	 * but here we need int-reenter, so we "sti" - exe - "cli" manually
	 * 
	 * whole progress: 
	 * int - cli - exe - (sti exe cli) - exe - sti - iret
	 *
	 * int re-enter from here
	 *-----------------------------------------------------------------------------*/
        sti
        call    *(irq_handler_table + \vector * 4)
        cli

	/* OCW1, open again */
        inb     $IMRM
        andb    $~1 << \vector,    %al
        outb    $IMRM
        ret
.endm
_hwint00:
        hwint   0
_hwint01:
        hwint   1
_hwint02:
        hwint   2
_hwint03:
        hwint   3
_hwint04:
        hwint   4
_hwint05:
        hwint   5
_hwint06:
        hwint   6
_hwint07:
        hwint   7


.macro  hwint8  vector
        call    save

	/* OCW1, close itself, so can't be re-entered by itself */
        inb     $IMRS
        orb     $1 << (\vector - 8),    %al
        outb    $IMRS

	/*-----------------------------------------------------------------------------
	 * OCW2, clears corresponding bit in ISR
	 * avoid low-level one interrupts high-level one,
	 * so set ICW4 == 0x01(manu), and clears ISR by ourself
	 * see in kernel/init.c -- init_i8259a()
	 * 
	 * reset master'n slave
	 *-----------------------------------------------------------------------------*/
        mov     $EOI,   %eax
        outb    $ICRM
        outb    $ICRS

	/*-----------------------------------------------------------------------------
	 * from int-gate(excluding trap-gate), 
	 * cpu will "cli" - exe - "sti" automatically
	 * but here we need int-reenter, so we "sti" - exe - "cli" manually
	 * 
	 * whole progress: 
	 * int - cli - exe - (sti exe cli) - exe - sti - iret
	 *
	 * int re-enter from here
	 *-----------------------------------------------------------------------------*/
        sti
        call    *(irq_handler_table + \vector * 4)
        cli

	/* OCW1, open again */
        inb     $IMRS
        andb    $~1 << (\vector - 8),    %al
        outb    $IMRS
        ret
.endm
_hwint08:
        hwint8   8
_hwint09:
        hwint8   9
_hwint10:
        hwint8   10
_hwint11:
        hwint8   11
_hwint12:
        hwint8   12
_hwint13:
        hwint8   13
_hwint14:
        hwint8   14
_hwint15:
        hwint8   15

_sys_call:
        call    save
        push    %edx
        push    %ecx
        push    %ebx
        pushl   (proc_current)
        sti
        call    *sys_call_table( , %eax, 4)
        add     $16,    %esp
        mov     %eax,   PCB_EAX(%esi)
        cli
        ret

#-----------------------------------------------------------------------------------------------------



#-----------------------------------------------------------------------------------------------------

save:                                   # here CS runs with IT = 1, RPL = 3
        pusha
        push    %ds
        push    %es
        push    %fs
        push    %gs

        mov     %edx,   %esi     # save edx
        mov     %ss,    %dx      # ss == SELECTOR_FLAT_RW(tss.ss0), so here mov ds'n es from LDT to GDT
        mov     %dx,    %ds      # to make preparation for RING0
        mov     %dx,    %es
        mov     %esi,   %edx     # restore edx

        mov     %esp,   %esi
        incb    (int_counter)
        cmpb    $0,     (int_counter)
        jne     1f

        mov     $_stack_kernel,   %esp  # get into stack kernel

        pushl   $_restart
        jmp     *PCB_RET_ADDR(%esi)
#         jmp     *(proc_current + PCB_RET_ADDR)                doesn't work, why?? result is JMP 0....
1:      pushl   $_re_restart
        jmp     *PCB_RET_ADDR(%esi)
#         jmp     *(proc_current + PCB_RET_ADDR)



_asm_debug:
        push    %eax                            # reserve %eax, %esi
        push    %esi
        movl       $0x90000,       %esi
        movl  %ds : (%esi),   %eax
        pushl %eax                              # reserve the data in 0x90000(%ds)
        movl  $0xcb,  %ds : (%esi)              # put ins "retf" in 0x90000(%ds)
        pushl %cs                               # push cs/ip, make preparation for "retf"
        pushl $back
        .byte 0xea                              # 0xea = jmp far
        .long 0x90000
        .word 0x8
        back:
        popl  %eax
        movl  %eax,   %ds : (%esi)
        pop     %esi
        pop     %eax
        ret             # now _bochs_debug is a func, so ret needed


#         inb     $IMRM
#         mov     $1,     %dl
#         mov     \vector,        %cl
#         rol     %cl,    %dl
#         orb     %dl,    %al
#         outb    $IMRM
#
#         inb     $IMRM
#         mov     $~1,     %dl
#         mov     \vector,        %cl
#         rol     %cl,    %dl
#         andb    %dl,    %al
#         outb    $IMRM
