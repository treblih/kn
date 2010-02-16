#ifndef         _HD_
#define         _HD_


#define         DEV_OPEN        1001

#define         HD_INFO         0xec

#define         HD_REG_DATA     0x1f0
#define         HD_REG_FEAT     0x1f1
#define         HD_REG_CNT      0x1f2
#define         HD_REG_LBAL     0x1f3
#define         HD_REG_LBAM     0x1f4
#define         HD_REG_LBAH     0x1f5
#define         HD_REG_DEV      0x1f6
#define         HD_REG_CMD      0x1f7
#define         HD_REG_CRTL     0x3f6

#define         MASK_BSY        0x80

#define         SET_DEVICE(is_lba, is_slave, hs)        \
                ((is_lba << 6) | (is_slave << 4) | (hs & 0x0f) | 0xa0)

#define         hd_info_read(buf, reg, size)    \
        __asm__ __volatile__("cli \n\t" \
                         "shr   $2,     %2 \n\t"        \
                         "rep   insl\n\t"     \
                         :      \
                         : "D"(buf), "d"(reg), "c"(size))
typedef struct {
        u8      feature;
        u8      count;
        u8      lba_low;
        u8      lba_mid;
        u8      lba_high;
        u8      device;
        u8      command;
} HD_CMD_REGS;


#endif
