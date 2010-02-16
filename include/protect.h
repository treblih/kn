#ifndef         _PROTECT_
#define         _PROTECT_


#define         SELECTOR_DUMMY      0
#define         SELECTOR_FLAT_C     0x08
#define         SELECTOR_FLAT_RW    0x10
#define         SELECTOR_VIDEO      0x1b        // 0x1b = 0x18 + 3, RPL = 3
#define         SELECTOR_CGATE      0x20        // 0x1b = 0x18 + 3, RPL = 3
#define         SELECTOR_TSS        0x28
#define         SELECTOR_LDT_1ST    0x30

#define         GDT_SIZE        128
#define         IDT_SIZE        256
#define         LDT_SIZE        2


#define         DA_386IGATE     0x8e
#define         DA_386CGATE     0x8c
#define         DA_LDT          0x82
#define         DA_386TSS       0x89
#define         DA_C            0x98
#define         DA_DRW          0x92

#define         RPL_KERL        0
#define         RPL_TASK        1
#define         RPL_SERV        2
#define         RPL_USER        3

#define         DPL_KERL        0
#define         DPL_TASK        1
#define         DPL_SERV        2
#define         DPL_USER        3

#define         TI_L            1

typedef struct {          //we may use the highest bit of every element, so unsigned needed
        u16     limit_low;
        u16     base_low;
        u8      base_mid;
        u8      attr_low;
        u8      limit_attr_high;
        u8      base_high;
} DESCRIPTOR;

typedef struct {
         u16    offset_low;
         u16    selector;
         u8     dcount;
         u8     attr;
         u16    offset_high;
} GATE;

typedef struct {
	u32	backlink;
	u32	esp0;		/* stack pointer to use during interrupt */
	u32	ss0;		/*   "   segment  "  "    "        "     */
	u32	esp1;
	u32	ss1;
	u32	esp2;
	u32	ss2;
	u32	cr3;
	u32	eip;
	u32	flags;
	u32	eax;
	u32	ecx;
	u32	edx;
	u32	ebx;
	u32	esp;
	u32	ebp;
	u32	esi;
	u32	edi;
	u32	es;
	u32	cs;
	u32	ss;
	u32	ds;
	u32	fs;
	u32	gs;
	u32	ldt;
	u16	trap;
	u16	iobase;	/* I/O位图基址大于或等于TSS段界限，就表示没有I/O许可位图 */
	/*u8	iomap[2];*/
} TSS;
#endif
