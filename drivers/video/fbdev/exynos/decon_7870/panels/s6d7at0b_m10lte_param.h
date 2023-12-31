#ifndef __S6D7AT0B_PARAM_H__
#define __S6D7AT0B_PARAM_H__
#include <linux/types.h>
#include <linux/kernel.h>

#define EXTEND_BRIGHTNESS	306
#define UI_MAX_BRIGHTNESS	255
#define UI_DEFAULT_BRIGHTNESS	128

struct lcd_seq_info {
	unsigned char	*cmd;
	unsigned int	len;
	unsigned int	sleep;
};

struct i2c_rom_data {
	u8 addr;
	u8 val;
};

static u8 LM3632_INIT[] = {
	0x09, 0x41,
	0x02, 0x50,
	0x03, 0x8D,
	0x04, 0x07,
	0x05, 0xFF,
	0x0A, 0x19,
	0x0D, 0x1C,
	0x0E, 0x24,
	0x0F, 0x24,
	0x0C, 0x1F,
	0x11, 0x0D,
};

/*
 * The applied command [0x9F] level key
 * 01h, 10h, 12h, 13h, 18h, 19h, 20h, 21h, 22h, 23h, 28h,
 * 29h, 30h, 34h, 35h, 36h, 38h, 39h, 3Ah, 44h, 4Fh, 51h.
 */

static const unsigned char SEQ_TEST_KEY_ON_9F[] = {
	0x9F,
	0x5A, 0x5A,
};

static const unsigned char SEQ_TEST_KEY_ON_F0[] = {
	0xF0,
	0x5A, 0x5A,
};

static const unsigned char SEQ_TEST_KEY_ON_FC[] = {
	0xFC,
	0x5A, 0x5A,
};

static const unsigned char SEQ_S6D7AT0B_51[] = {
	0x51,
	0xFF, 0xF1,
};

static const unsigned char SEQ_S6D7AT0B_53[] = {
	0x53,
	0x24,
};

static const unsigned char SEQ_S6D7AT0B_55[] = {
	0x55,
	0x00,
};

static const unsigned char SEQ_S6D7AT0B_71[] = {
	0x71,
	0x0A
};

static const unsigned char SEQ_S6D7AT0B_73[] = {
	0x73,
	0xB0, 0x32, 0x10, 0x01, 0x53, 0x00,
};

static const unsigned char SEQ_S6D7AT0B_B1[] = {
	0xB1,
	0x11, 0x32, 0x11, 0x00,
};

static const unsigned char SEQ_S6D7AT0B_B3[] = {
	0xB3,
	0x01, 0xBE, 0x07, 0x70, 0x18, 0x68, 0x08, 0x08, 0x07, 0x80,
	0x00, 0xE2, 0x00, 0xE2, 0x00, 0xE2,
};

static const unsigned char SEQ_S6D7AT0B_BA[] = {
	0xBA,
	0x01, 0x13, 0x02, 0x13, 0x02, 0x13, 0x02, 0x88, 0x02, 0x58,
	0x00, 0x84, 0x08, 0x43, 0x03, 0x34, 0xC1, 0x00, 0x98, 0x00,
	0x14, 0x00, 0x01, 0x1A, 0x01, 0x10, 0xC2, 0x89, 0x50, 0x2C,
	0x99, 0x46, 0x46, 0x00, 0x01, 0xF1, 0x01, 0x01, 0x28, 0x32,
	0x38, 0x3F, 0x48, 0x50, 0x54, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0x2F, 0x48, 0x64, 0x98, 0xBF, 0xDF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
	0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char SEQ_S6D7AT0B_BB[] = {
	0xBB,
	0x21, 0x68, 0x00, 0x00,
};

static const unsigned char SEQ_S6D7AT0B_EC[] = {
	0xEC,
	0x08, 0x02, 0x07, 0x02, 0x15, 0x14, 0x02, 0x0F, 0x0D, 0x0C,
	0x0E, 0x05, 0x09, 0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x02,
	0x03, 0x03, 0x08, 0x02, 0x07, 0x02, 0x15, 0x14, 0x02, 0x0E,
	0x0C, 0x0F, 0x0D, 0x04, 0x09, 0x02, 0x02, 0x02, 0x02, 0x00,
	0x00, 0x02, 0x03, 0x03,
};

static const unsigned char SEQ_S6D7AT0B_ED[] = {
	0xED,
	0x12, 0x40, 0x05, 0x15, 0x11, 0x20, 0x0C, 0x13, 0x40, 0x05,
	0x15, 0x11, 0x00, 0x0C, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x10, 0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x21, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x00, 0x40, 0x01, 0x72, 0x61, 0x00, 0xE0, 0x20,
	0x41, 0x01, 0x72, 0x61, 0x00, 0xC0, 0x40, 0x08, 0x92, 0x4C,
	0x40, 0x00, 0x80, 0x28, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char SEQ_S6D7AT0B_EE[] = {
	0xEE,
	0x12, 0xE4, 0x00, 0x19, 0x05, 0x06, 0x00, 0x00, 0x68, 0x12,
	0xE5, 0x00, 0x19, 0x05, 0x06, 0x00, 0x00, 0x48, 0x12, 0xE6,
	0x00, 0x19, 0x05, 0x06, 0x00, 0x00, 0x2C, 0x12, 0xE7, 0x00,
	0x19, 0x05, 0x06, 0x00, 0x00, 0x0C,
};

static const unsigned char SEQ_S6D7AT0B_EF[] = {
	0xEF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x40, 0x76, 0x76, 0x10,
	0x18, 0x19, 0x80, 0x00, 0x08, 0x50, 0x10, 0x00, 0x00, 0x48,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x11, 0x77, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char SEQ_S6D7AT0B_F2[] = {
	0xF2,
	0x48, 0x23, 0x33, 0x33, 0xE3, 0x44, 0x43, 0x00, 0x01, 0x00,
	0x00, 0x0D, 0x84, 0x00, 0x80, 0x00, 0xA6, 0x1E, 0x08, 0x03,
	0x34, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char SEQ_S6D7AT0B_B0[] = {
	0xB0,
	0x02,
};

static const unsigned char SEQ_S6D7AT0B_F4[] = {
	0xF4,
	0x13, 0x17, 0x8A, 0xD0, 0xD0, 0x05, 0x0A, 0x16, 0x95, 0x55,
	0x59, 0x3B, 0x10, 0x55, 0x15, 0x26, 0xFA, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
};

static const unsigned char SEQ_S6D7AT0B_F9[] = {
	0xF9,
	0x88, 0x88, 0x00, 0xC0, 0xC0, 0x04, 0x7A, 0x00, 0x0D, 0xC0,
	0x94, 0x0C, 0x4B, 0xA0, 0x1F, 0xA0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x0C, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x0C, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x0C, 0x00, 0x11, 0x00, 0x00, 0x77, 0xFF, 0x00,
};

static const unsigned char SEQ_S6D7AT0B_FE[] = {
	0xFE,
	0x02, 0x00, 0x01, 0x4C, 0x00, 0x00, 0x00, 0x60, 0x00, 0x03,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char SEQ_S6D7AT0B_C0[] = {
	0xC0,
	0x03, 0x08, 0x29, 0x01, 0x00, 0x80, 0x01, 0x24, 0x0F, 0xFF,
	0x30, 0x40, 0x86, 0xDA, 0x8E,
};

static const unsigned char SEQ_S6D7AT0B_C1[] = {
	0xC1,
	0xE5, 0xA6, 0x80, 0x25, 0x66, 0xA7, 0xA7, 0xAC, 0xB3, 0xB3,
	0xB3, 0xB3, 0xBA, 0xBD, 0xBF, 0xBF, 0xC4, 0xD6, 0xE8, 0xFF,
	0x88, 0x96, 0xA5, 0xB4, 0xBE, 0xC8, 0xCD, 0xD0, 0xD3, 0xD7,
	0xDF, 0xE6, 0xEA, 0xF0, 0xF5, 0xFF,
};

static const unsigned char SEQ_S6D7AT0B_C8[] = {
	0xC8,
	0x00, 0x24, 0x00, 0x41, 0x3A, 0x00, 0x00, 0x00, 0xB0, 0x02,
	0x00, 0x00, 0x00, 0x85, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x40, 0x4B,
};

static const unsigned char SEQ_S6D7AT0B_E8[] = {
	0xE8,
	0x00, 0x11, 0x1D, 0x1B, 0x22, 0x28, 0x28, 0x2A, 0x30, 0x34,
	0x33, 0x30, 0x2E, 0x2C, 0x29, 0x2E, 0x30, 0x00, 0x11, 0x1D,
	0x1B, 0x22, 0x28, 0x28, 0x2A, 0x30, 0x30, 0x2D, 0x30, 0x2E,
	0x2C, 0x29, 0x2E, 0x34,
};

// Rev 00
static const unsigned char SEQ_S6D7AT0B_w00_71[] = {
	0x71,
	0x0A
};

static const unsigned char SEQ_S6D7AT0B_w00_73[] = {
	0x73,
	0xB0, 0x32, 0x10, 0x01, 0x48, 0x00,
};

static const unsigned char SEQ_S6D7AT0B_w00_B1[] = {
	0xB1,
	0x11, 0x32, 0x11, 0x00,
};

static const unsigned char SEQ_S6D7AT0B_w00_B3[] = {
	0xB3,
	0x01, 0xB9, 0x07, 0x70, 0x18, 0x68, 0x08, 0x08, 0x07, 0x80,
	0x00, 0xE7, 0x00, 0xE7, 0x00, 0xE7,
};

static const unsigned char SEQ_S6D7AT0B_w00_BA[] = {
	0xBA,
	0x01, 0x13, 0x02, 0x13, 0x02, 0x13, 0x02, 0x88, 0x02, 0x58,
	0x00, 0x84, 0x08, 0x43, 0x03, 0x45, 0xC1, 0x00, 0x94, 0x00,
	0x14, 0x00, 0x01, 0x20, 0x01, 0x10, 0xC2, 0x89, 0x50, 0x2C,
	0x99, 0x46, 0x46, 0x00, 0x01, 0xF1, 0x01, 0x01, 0x28, 0x32,
	0x38, 0x3F, 0x48, 0x50, 0x54, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0x2F, 0x48, 0x64, 0x98, 0xBF, 0xDF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
	0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char SEQ_S6D7AT0B_w00_BB[] = {
	0xBB,
	0x21, 0x68, 0x00, 0x00,
};

static const unsigned char SEQ_S6D7AT0B_w00_EC[] = {
	0xEC,
	0x00, 0x08, 0x00, 0x07, 0x00, 0x15, 0x14, 0x02, 0x0F, 0x0D,
	0x0C, 0x0E, 0x05, 0x09, 0x06, 0x02, 0x02, 0x00, 0x00, 0x02,
	0x03, 0x03, 0x02, 0x08, 0x02, 0x07, 0x02, 0x15, 0x14, 0x02,
	0x0E, 0x0C, 0x0F, 0x0D, 0x04, 0x09, 0x06, 0x02, 0x02, 0x00,
	0x00, 0x02, 0x03, 0x03,
};

static const unsigned char SEQ_S6D7AT0B_w00_ED[] = {
	0xED,
	0x12, 0x40, 0x00, 0x15, 0x11, 0x00, 0x0C, 0x13, 0x40, 0x00,
	0x15, 0x11, 0x00, 0x0C, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x10, 0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x21, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x00, 0x40, 0x01, 0x72, 0x61, 0x00, 0xE0, 0x20,
	0x41, 0x01, 0x72, 0x61, 0x00, 0xC0, 0x40, 0x08, 0x92, 0x4C,
	0x40, 0x00, 0x80, 0x28, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char SEQ_S6D7AT0B_w00_EE[] = {
	0xEE,
	0x12, 0xC4, 0x80, 0x19, 0x05, 0x06, 0x00, 0x00, 0x68, 0x12,
	0xC5, 0x80, 0x19, 0x05, 0x06, 0x00, 0x00, 0x68, 0x12, 0xC6,
	0x80, 0x19, 0x05, 0x06, 0x00, 0x00, 0x6A, 0x12, 0xC7, 0x80,
	0x19, 0x05, 0x06, 0x00, 0x00, 0x6A,
};

static const unsigned char SEQ_S6D7AT0B_w00_EF[] = {
	0xEF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x40, 0x76, 0x76, 0x10,
	0x18, 0x09, 0x80, 0x00, 0x08, 0x50, 0x10, 0x00, 0x00, 0x48,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x11, 0x77, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char SEQ_S6D7AT0B_w00_F2[] = {
	0xF2,
	0x48, 0x23, 0x33, 0x33, 0xE3, 0x44, 0x43, 0x00, 0x01, 0x00,
	0x00, 0x0D, 0x84, 0x00, 0x80, 0x00, 0xA6, 0x1E, 0x08, 0x03,
	0x34, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char SEQ_S6D7AT0B_w00_B0[] = {
	0xB0,
	0x02,
};

static const unsigned char SEQ_S6D7AT0B_w00_F4[] = {
	0xF4,
	0x00, 0x17, 0x8A, 0xDE, 0xDE, 0x05, 0x0A, 0x16, 0x95, 0x55,
	0x59, 0x3B, 0x10, 0x55, 0x15, 0x26, 0xFA, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
};

static const unsigned char SEQ_S6D7AT0B_w00_F9[] = {
	0xF9,
	0x88, 0x88, 0x00, 0xC0, 0xC0, 0x04, 0x7A, 0x00, 0x0D, 0xC0,
	0x94, 0x0C, 0x4B, 0xA0, 0x1F, 0xA0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x0C, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x0C, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x0C, 0x00, 0x11, 0x00, 0x00, 0x77, 0xFF, 0x00,
};

static const unsigned char SEQ_S6D7AT0B_w00_FE[] = {
	0xFE,
	0x02, 0x00, 0x01, 0x4C, 0x00, 0x00, 0x00, 0x60, 0x00, 0x03,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char SEQ_S6D7AT0B_w00_C0[] = {
	0xC0,
	0x03, 0x08, 0x29, 0x01, 0x00, 0x80, 0x01, 0x24, 0x0F, 0xFF,
	0x30, 0x40, 0x86, 0xDA, 0x8E,
};

static const unsigned char SEQ_S6D7AT0B_w00_C1[] = {
	0xC1,
	0xE5, 0xA6, 0x80, 0x25, 0x66, 0xA7, 0xA7, 0xAC, 0xB3, 0xB3,
	0xB3, 0xB3, 0xBA, 0xBD, 0xBF, 0xBF, 0xC4, 0xD6, 0xE8, 0xFF,
	0x88, 0x96, 0xA5, 0xB4, 0xBE, 0xC8, 0xCD, 0xD0, 0xD3, 0xD7,
	0xDF, 0xE6, 0xEA, 0xF0, 0xF5, 0xFF,
};

static const unsigned char SEQ_S6D7AT0B_w00_C8[] = {
	0xC8,
	0x00, 0x24, 0x00, 0x41, 0x3A, 0x00, 0x00, 0x00, 0xB0, 0x02,
	0x00, 0x00, 0x00, 0x85, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x40, 0x4B,
};

static const unsigned char SEQ_S6D7AT0B_w00_E8[] = {
	0xE8,
	0x02, 0x0D, 0x17, 0x13, 0x1A, 0x20, 0x1F, 0x21, 0x27, 0x2D,
	0x2D, 0x2A, 0x29, 0x27, 0x27, 0x2E, 0x30, 0x02, 0x0D, 0x17,
	0x13, 0x1A, 0x20, 0x1F, 0x21, 0x27, 0x27, 0x27, 0x2A, 0x29,
	0x27, 0x27, 0x2E, 0x34,
};

static const unsigned char SEQ_TE_ON[] = {
	0x35,
	0x00,
};

static const unsigned char SEQ_TEST_KEY_OFF_9F[] = {
	0x9F,
	0xA5, 0xA5,
};

static const unsigned char SEQ_TEST_KEY_OFF_F0[] = {
	0xF0,
	0xA5, 0xA5,
};

static const unsigned char SEQ_TEST_KEY_OFF_FC[] = {
	0xFC,
	0xA5, 0xA5,
};

static const unsigned char SEQ_SLEEP_OUT[] = {
	0x11,
	0x00, 0x00
};

static const unsigned char SEQ_SLEEP_IN[] = {
	0x10,
	0x00, 0x00
};

static const unsigned char SEQ_DISPLAY_OFF[] = {
	0x28,
	0x00, 0x00
};

static const unsigned char SEQ_DISPLAY_ON[] = {
	0x29,
	0x00, 0x00
};

/* platform brightness <-> bl reg */
static unsigned int brightness_table[EXTEND_BRIGHTNESS + 1] = {
	0,
	12, 19, 26, 33, 40, 47, 54, 62, 69, 76,
	83, 90, 97, 104, 112, 119, 126, 133, 140, 147,
	154, 162, 169, 176, 183, 190, 197, 205, 212, 219,
	226, 233, 240, 247, 255, 262, 269, 276, 283, 290,
	297, 305, 312, 319, 326, 333, 340, 348, 355, 362,
	369, 376, 383, 390, 398, 405, 412, 419, 426, 433,
	440, 448, 455, 462, 469, 476, 483, 491, 498, 505,
	512, 519, 526, 533, 541, 548, 555, 562, 569, 576,
	583, 591, 598, 605, 612, 619, 626, 634, 641, 648,
	655, 662, 669, 676, 684, 691, 698, 705, 712, 719,
	726, 734, 741, 748, 755, 762, 769, 777, 784, 791,
	798, 805, 812, 819, 827, 834, 841, 848, 855, 862,
	869, 877, 884, 891, 898, 905, 912, 920, 932, 944,
	956, 968, 980, 992, 1005, 1017, 1029, 1041, 1053, 1065,
	1078, 1090, 1102, 1114, 1126, 1138, 1151, 1163, 1175, 1187,
	1199, 1211, 1224, 1236, 1248, 1260, 1272, 1284, 1297, 1309,
	1321, 1333, 1345, 1357, 1370, 1382, 1394, 1406, 1418, 1430,
	1443, 1455, 1467, 1479, 1491, 1503, 1516, 1528, 1540, 1552,
	1564, 1576, 1589, 1601, 1613, 1625, 1637, 1649, 1662, 1674,
	1686, 1698, 1710, 1722, 1735, 1747, 1759, 1771, 1783, 1795,
	1808, 1820, 1832, 1844, 1856, 1868, 1881, 1893, 1905, 1917,
	1929, 1941, 1954, 1966, 1978, 1990, 2002, 2014, 2027, 2039,
	2051, 2063, 2075, 2087, 2100, 2112, 2124, 2136, 2148, 2160,
	2173, 2185, 2197, 2209, 2221, 2233, 2246, 2258, 2270, 2282,
	2294, 2306, 2319, 2331, 2343, 2355, 2367, 2379, 2392, 2404,
	2416, 2428, 2440, 2452, 2465, 2465, 2465, 2465, 2465, 2465,
	2465, 2465, 2465, 2465, 2465, 2465, 2465, 2465, 2465, 2465,
	2465, 2465, 2465, 2465, 2465, 2465, 2465, 2465, 2465, 2465,
	2465, 2465, 2465, 2465, 2465, 2465, 2465, 2465, 2465, 2465,
	2465, 2465, 2465, 2465, 2465, 2465, 2465, 2465, 2465, 2465,
	2465, 2465, 2465, 2465, 2465, 2899,
};

#endif /* __S6D7AT0B_PARAM_H__ */
