/*
 * =====================================================================================
 *
 *       Filename:  keyboard.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  15.02.10
 *       Revision:  
 *       Compiler:  gcc
 *
 *         Author:  Yang Zhang (ZY), imyeyeslove@163.com
 *        Company:  
 *
 * =====================================================================================
 */

#include "io.h"
#include "type.h"
#include "protect.h"
#include "proc.h"
#include "tty.h"
#include "proto.h"
#include "global.h"
#include "keyboard.h"
#include "keymap.h"

extern KEYBOARD kb;

static u8 read_keyboard_buf();
static void init_led();
static void clear_8042();
static void get_ack();

static int alt_l;
static int alt_r;
static int ctrl_l;		/* l ctrl state  */
static int ctrl_r;		/* l ctrl state  */
static int shift_l;
static int shift_r;
static int caps_lock;
static int num_lock;
static int scroll_lock;

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  init_keyboard
 *  Description:  called in init_i8259(), so it runs in RING-0
 * =====================================================================================
 */
void init_keyboard()
{
	kb.head = kb.tail = kb.buf;
	kb.count = 0;

	num_lock = 1;                                           /* default open */
	init_led();                                             /* keyboard LEDs */

	init_irq(1, handler_keyboard);                          /* IRQ-1 */
}


/*-----------------------------------------------------------------------------
 * 
 * 		all below run in RING-1, 'cause proc tty runs in RING-1
 *
 *-----------------------------------------------------------------------------*/
void read_keyboard(TTY * p_tty)	// kb, read in RING1
{
	u8 scan_code;		// u8  scan_code -> key_col -> key_value
	u32 keyval = 0;		// int key_value
	int code_with_E0 = 0;
	int column = 0;
	int make;
	u32 *keycol;

	/*
	 * if there is anything in buf, we do next.
	 * so the read_keyboard_buf() below need no judge count, diff with write_tty()
	 */
	if (kb.count) {		// no equals
		scan_code = read_keyboard_buf();

		if (scan_code == 0xe0) {
			scan_code = read_keyboard_buf();

			/* PrintScreen 被按下 */
			if (scan_code == 0x2A) {
				if (read_keyboard_buf() == 0xE0) {
					if (read_keyboard_buf() == 0x37) {
						keyval = PRINTSCREEN;
						make = 1;
					}
				}
			}
			/* PrintScreen 被释放 */
			if (scan_code == 0xB7) {
				if (read_keyboard_buf() == 0xE0) {
					if (read_keyboard_buf() == 0xAA) {
						keyval = PRINTSCREEN;
						make = 0;
					}
				}
			}
			/* 不是PrintScreen, 此时scan_code为0xE0紧跟的那个值. */
			if (keyval == 0) {
				code_with_E0 = 1;
			}
		} else if (scan_code == 0xe1) {
			int i;
			u8 pausebrk_scode[] =
			    { 0xE1, 0x1D, 0x45, 0xE1, 0x9D, 0xC5 };
			int is_pausebreak = 1;
			for (i = 1; i < 6; i++) {
				if (read_keyboard_buf() != pausebrk_scode[i]) {
					is_pausebreak = 0;
					break;
				}
			}
			if (is_pausebreak) {
				keyval = PAUSEBREAK;
			}
		}
		/* there's no else */

		if ((keyval != PAUSEBREAK) && (keyval != PRINTSCREEN)) {
			make = (scan_code & 0x80) ? 0 : 1;	// make's destiny is for Ctl_break_code's judge
			keycol = &keymap[(scan_code & 0x7f) * 3];	// be sure of the including line num

			if (shift_l || shift_r) {
				column = 1;
			}
			if (code_with_E0) {
				column = 2;
				code_with_E0 = 0;
			}
			if ((scan_code >= 0x47 && scan_code <= 0x53)
			    && ((column == 0) && num_lock)) {
				column = 1;
			}

			keyval = keycol[column];

			/* just judge for ctrl_key */
			switch (keyval) {
			case SHIFT_L:
				shift_l = make;
				break;
			case SHIFT_R:
				shift_r = make;
				break;
			case CTRL_L:
				ctrl_l = make;
				break;
			case CTRL_R:
				ctrl_r = make;
				break;
			case ALT_L:
				alt_l = make;
				break;
			case ALT_R:
				alt_r = make;
				break;

				/* if it's make_code, we change it */
			case CAPS_LOCK:
				if (make) {
					caps_lock = !caps_lock;
					init_led();
				}
				break;
			case NUM_LOCK:
				if (make) {
					num_lock = !num_lock;
					init_led();
				}
				break;
			case SCROLL_LOCK:
				if (make) {
					scroll_lock = !scroll_lock;
					init_led();
				}
				break;
			case UP:
				scroll_screen(p_tty->p_console, SCROLL_UP);
				break;
			case DOWN:
				scroll_screen(p_tty->p_console, SCROLL_DOWN);
				break;
			case F1:
			case F2:
			case F3:
			case F4:
				//  if (alt_l || alt_r) {
				select_tty(tty_table + (keyval - F1));
				break;
				// }
			case 'l':
				if (ctrl_l || ctrl_r) {
					clean_screen(p_tty->p_console);
				}
				break;
			default:
				if (make) {	/* 忽略 Break Code */
					kbuf_to_tbuf(keyval, p_tty);
				}
				break;
			}
		}
	}
}

static u8 read_keyboard_buf()
{
	/* diff with write_tty(), 'cause it has judged above */
	u8 scan_code;		// scan_code again, doesn't matter

	/*
	 * very very very significant
	 * it only has influence on keys which has E0 in front
	 * just have a look at read_keyboard() u will know
	 * key with E0, namely it has 2 make_code
	 */
	while (kb.count <= 0) {
	}

	__asm__ __volatile__("cli");
	if (kb.tail == kb.buf + KEY_BUF_SIZE) {	// means tail has pointed out
		kb.tail = kb.buf;
	}
	scan_code = *kb.tail;
	kb.tail++;
	kb.count--;
	__asm__ __volatile__("sti");

	return scan_code;
}

static void init_led()
{
	u8 led = (caps_lock << 2) | (num_lock << 1) | scroll_lock;

	clear_8042();
	out_byte(KB_DATA, LED_MODE);
	get_ack();

	out_byte(KB_DATA, led);
}

static void clear_8042()
{
	u8 i;

	do {
		i = in_byte(KB_CMD);
	} while (i & 0x2);
}

static void get_ack()
{
	u8 i;

	do {
		i = in_byte(KB_DATA);
	} while (i != ACK);
}
