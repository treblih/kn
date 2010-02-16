#ifndef         _STRING_
#define         _STRING_

#define memcpy(dst, src, cnt)    \
        __asm__ __volatile__("rep \n\t"      \
                         "movsb \n\t"   \
                         :      \
                         : "D"(dst), "S"(src), "c"(cnt))

#define memset(dst, val, cnt)    \
        __asm__ __volatile__("cld \n\t" \
                         "rep \n\t"      \
                         "stosb \n\t"   \
                         :      \
                         : "D"(dst), "a"(val), "c"(cnt))

#define disp_str(str)   \
        __asm__ __volatile__("mov       (disp_pos),    %%edi \n\t"     \
                         "1:    \n\t"     \
                         "lodsb \n\t"   \
                         "test  %%al,   %%al \n\t"      \
                         "jz    3f \n\t" \
                         "cmp   $0xa, %%al \n\t"       \
                         "jnz   2f \n\t" \
                         "mov   %%edi,     %%eax \n\t"     \
                         "mov   $160,   %%bl \n\t"      \
                         "div   %%bl \n\t"      \
                         "andl  $0xff,  %%eax \n\t"     \
                         "inc   %%eax \n\t"     \
                         "mov   $160,   %%bl \n\t"      \
                         "mul   %%bl \n\t"      \
                         "mov   %%eax,  %%edi \n\t"        \
                         "jmp   1b \n\t" \
                         "2: \n\t"      \
                         "mov     $0xc,   %%ah \n\t"      \
                         "mov   %%ax,   %%gs : (%%edi) \n\t"       \
                         "add   $2,     %%edi \n\t"        \
                         "jmp   1b \n\t" \
                         "3: \n\t"      \
                         "mov   %%edi,  (disp_pos) \n\t"        \
                         : \
                         : "S"(str)    \
                         : "ax", "bx")
#if 0
#define strlen(buf)     \
({      \
        int     _i = 0;     \
        __asm__ __volatile__("1: \n\t"  \
                         "cmp   $0,     (%1) \n\t"     \
                         "jz    2f \n\t"        \
                         "xor   %0,  %0 \n\t"     \
                         "inc   %0 \n\t"     \
                         "inc   %1 \n\t"     \
                         "jmp   1b \n\t"        \
                         "2: \n\t"      \
                         : "=c"(_i)     \
                         : "S"(buf));       \
        _i;     \
})
#endif


#endif
