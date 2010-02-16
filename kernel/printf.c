#include "type.h"
#include "protect.h"
#include "proc.h"
#include "tty.h"
#include "proto.h"
#include "syscall.h"



int
printf (const char* fmt, ...)
{
        char buf[256] = {0};
        va_list argv    =       (va_list)(&fmt + 1);
        int buf_len     =       vsprintf(buf, fmt, argv);
        //disp_int(buf_len);

        //write(buf, buf_len);

        if (buf_len)
                printx(buf);

        return buf_len;
}

int
vsprintf(char* buf, const char* fmt, va_list argv)
{
        char    fill;
        char    count   =       0;
        char    align   =       0;
        char*   p       =       buf;
        va_list string  =       argv;

        /* in the WHILE below, we must use p instead of buf */
        while (*fmt) {
                if (*fmt != '%') {
                        *p++ = *fmt++;
                        continue;
                }

        /* 
         *                      \n \t \b... 
         * \ and the char followed are one, so no need to case '\'
         */
                ++fmt;
                if (*fmt == '%') {
                        *p++ = *fmt++;
                        continue;
                }
                else if (*fmt == '0') {
                        fill = '0';
                        fmt++;
                }
                else {
                        fill = ' ';
                }

                while (*fmt >= '0' && *fmt <= '9') {
                        align *= 10;
                        align += (*fmt - '0');
                        fmt++;
                }

                switch (*fmt) {
                case 'x':
                        count = i2a(&p, *(int*)argv, 16);

                        __asm__ __volatile__("jmp 1f");
#if 0
                        if (align) {
                                if (align < count)
                                        return -1;
                                char i = count;
                                char j = align - count;
                                for ( ; i; i--) {
                                        p--;
                                        *(p + j) = *p;
                                }
                                for ( ; j; j--) {
                                        *p++ = fill;
                                }
                                p += count;
                                *p = 0;
                                align = 0;
                        }
                        break;
#endif
                case 'd':
                        count = i2a(&p, *(int*)argv, 10);

                        __asm__ __volatile__("1:");
                        if (align) {
                                if (align < count)
                                        return -1;
                                char i = count;
                                char j = align - count;
                                for ( ; i; i--) {
                                        p--;
                                        *(p + j) = *p;
                                }
                                for ( ; j; j--) {
                                        *p++ = fill;
                                }
                                p += count;
                                *p = 0;
                                align = 0;
                        }
                        break;
                case 'c':
                        *p++ = *argv;
                        break;
                case 's':
                        string = (char*)*(int*)argv;
                        while (*string) {
                                *p++ = *string++;
                        }
                        break;
                default:
                        break;
                }
                fmt++;
                argv += 4;
        }

        return (p - buf);
}
