#ifndef         _IO_
#define         _IO_


#define in_byte(port)       \
({      \
        u8      _i;     \
        __asm__ __volatile__("inb %%dx, %%al \n\t" : "=a"(_i) : "d"(port));   \
        _i;    \
 })

#define out_byte(port, val)      \
        __asm__ __volatile__("outb %%al, %%dx" : : "a"(val), "d"(port));

#endif
