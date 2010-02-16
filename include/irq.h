#ifndef         _IRQ_
#define         _IRQ_


#define         ICRM            0x20
#define         ICRS            0xa0
#define         IMRM            0x21
#define         IMRS            0xa1

#define         ICW1            0x11
#define         ICW2M           0x20
#define         ICW2S           0x28
#define         ICW3M           0x04
#define         ICW3S           0x02
#define         ICW4            0x01
/*-----------------------------------------------------------------------------
 * OCW2, clear corresponding bit in ISR
 * avoid low-level INT high-level, so clear ISR manually
 *
 * see in /lib/interrupt.s
 *-----------------------------------------------------------------------------*/


#define         OCW1M           0xff
#define         OCW1S           0xff
#define         EOI             0x20                            /* OCW2 */


#define         TC0             0x40
#define         TMCR            0x43
#define         RATE_GENERATOR  0x34



#define		init_slave()	irq_enable(2)
/*-----------------------------------------------------------------------------
 * there ought to be 4 $IMRS
 * but my gcc says that "undefined reference to IMRS"
 * when I change one of them to 0xa1, the world converts to be silent again...
 * i don't know why
 * please show me if u know
 * thanks
 *-----------------------------------------------------------------------------*/

/* notice that it's AS, not C, so the consts above don't work */
#define		irq_disable(vector)     \
__asm__ __volatile__(".include  \"include/sconst.inc\" \n\t"    \
         "pushf \n\t"                                           \
         "cli \n\t"                                             \
         "mov  $1,     %%dl \n\t"                               \
         "rol  %%cl,   %%dl \n\t"                               \
                               "/* IRQ 0-7 or IRQ 8-15 */ \n\t" \
         "cmp   $8,     %%cl \n\t"                              \
         "jae   1f \n\t"                                        \
                                "/* IRQ 0-7 0x21, OCW1 */ \n\t" \
         "inb   $IMRM \n\t"                                     \
         "or   %%dl,   %%al \n\t"                               \
         "outb  $IMRM \n\t"                                     \
         "popf \n\t"                                            \
         "jmp   2f \n\t"                                        \
         "1: \n\t"                                              \
                                "/* IRQ 8-15 0xa1 OCW1 */ \n\t" \
         "inb   $IMRS \n\t"                                     \
         "or   %%dl,   %%al \n\t"                               \
         "outb   $IMRS \n\t"                                    \
         "popf \n\t"                                            \
         "2: \n\t"                                              \
         :                                                      \
         : "c"(vector)                                          \
         : "ax", "dx") /* can't use "i" to be the constraint, why?? */

#define         irq_enable(vector)                              \
__asm__ __volatile__(".include  \"include/sconst.inc\" \n\t"    \
         "pushf \n\t"                                           \
         "mov  $~1,     %%dl \n\t"                              \
         "rol  %%cl,   %%dl \n\t"                               \
                               "/* IRQ 0-7 or IRQ 8-15 */ \n\t" \
         "cmp   $8,     %%cl \n\t"                              \
         "jae   1f \n\t"                                        \
                                "/* IRQ 0-7 0x21, OCW1 */ \n\t" \
         "inb   $IMRM \n\t"                                     \
         "and   %%dl,   %%al \n\t"                              \
         "outb  $IMRM \n\t"                                     \
         "popf \n\t"                                            \
         "jmp   2f \n\t"                                        \
         "1: \n\t"                                              \
                                "/* IRQ 8-15 0xa1 OCW1 */ \n\t" \
         "inb   $IMRS \n\t"                                     \
         "and   %%dl,   %%al \n\t"                              \
         "outb   $IMRS \n\t"                                    \
         "popf \n\t"                                            \
         "2: \n\t"                                              \
         :                                                      \
         : "c"(vector)                                          \
	 : "ax", "dx")  /* can't use "i" to be the constraint, why?? */

#endif
