#ifndef         _TTY_
#define         _TTY_


#define         KEY_BUF_SIZE    0x20
#define         CONSOLE_SIZE    4

#define         FLAG_EXT	0x0100		/* Normal function keys		*/

#define	        CRTC_ADDR_REG	0x3D4	/* CRT Controller Registers - Addr Register */
#define	        CRTC_DATA_REG	0x3D5	/* CRT Controller Registers - Data Register */
#define	        START_ADDR_H	0xC	/* reg index of video mem start addr (MSB) */
#define	        START_ADDR_L	0xD	/* reg index of video mem start addr (LSB) */
#define	        CURSOR_H	0xE	/* reg index of cursor position (MSB) */
#define	        CURSOR_L	0xF	/* reg index of cursor position (LSB) */
#define	        VMEM_BASE	0xB8000	/* base of color video memory */
#define	        VMEM_SIZE	0x8000	/* 32K: B8000H -> BFFFFH */
#define         COLOR           0x0c

#define         SCREEN_WIDTH    80
#define         SCREEN_SIZE     80 * 25

#define         SCROLL_UP       1
#define         SCROLL_DOWN     0

typedef struct {
        int     current_addr;
        int     original_addr;
        int     vmem_limit;
        int     cursor;
} CONSOLE;

/* it's INT, not u8, 'cause keyval is INT, although scan_code is u8 */
typedef struct {
        u32*     head;
        u32*     tail;
        CONSOLE* p_console;
        int      count;
        u32      buf[KEY_BUF_SIZE];
} TTY;

#define         MAGIC_PANIC     '\002'
#define         MAGIC_ASSERT    '\003'

#ifndef         ASSERT
#define         ASSERT
void assert_declare(char*, char*, char*, int, const char*);
#define         assert(exp)     \
                if      (exp) ;        \
		/* __func__ */		\
                else    assert_declare(#exp, __FILE__, __BASE_FILE__, __LINE__, __func__)
#endif


#endif
