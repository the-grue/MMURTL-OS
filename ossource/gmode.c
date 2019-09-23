#include "vga8x8.h"	/*for IBM_VGA_8x8 font*/
#include "vga8x16.h"	/*for IBM_VGA_8x16 font*/
#include "\OSSource\MJob.h"
#include "\OSSource\gmode.h"

#define ErcBadVidMode		303

#define	ACPort1		0x3C0
#define	ACPort2		0x3C1
#define	MISCPort2		0x3C2
#define SEQPort1		0x3C4
#define SEQPort2		0x3C5
#define	DACPort2	0x3C7
#define	DACPort1	0x3C8
#define	DACPort3		0x3C9
#define	MISCPort1		0x3CC
#define GCPort1 		0x3CE
#define GCPort2 		0x3CF
/*			COLOR emulation		MONO emulation */
#define CRTCPort1		0x3D4		/* 0x3B4 */
#define CRTCPort2		0x3D5		/* 0x3B5 */
#define	GCISPort2		0x3DA

#define	VGA_NUM_SEQ_REGS	5
#define	VGA_NUM_CRTC_REGS	25
#define	VGA_NUM_GC_REGS		9
#define	VGA_NUM_AC_REGS		21
#define	VGA_NUM_REGS		64

static struct JCBRec *pTmpJCB;
long JobNum;
/*****************************************************************************
 * Video register settings for various modes
*****************************************************************************/

unsigned char g_80x25_text[] =
{
	0x67, 0x03, 0x00, 0x03, 0x00, 0x02, 0x5F, 0x4F,
       	0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F, 0x00, 0x4F,
       	0x0D, 0x0E, 0x00, 0x00, 0x00, 0x50, 0x9C, 0x0E,
       	0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3, 0xFF, 0x00,
       	0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00, 0xFF,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x0C, 0x00, 0x0F, 0x08, 0x00
};

unsigned char g_80x50_text[] =
{
	0x67, 0x03, 0x00, 0x03, 0x00, 0x02, 0x5F, 0x4F,
       	0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F, 0x00, 0x47,
       	0x06, 0x07, 0x00, 0x00, 0x01, 0x40, 0x9C, 0x8E,
       	0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3, 0xFF, 0x00,
       	0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00, 0xFF, 
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x0C, 0x00, 0x0F, 0x08, 0x00,
};

unsigned char g_640x480x16[] =
{
	0xE3, 0x03, 0x01, 0x08, 0x00, 0x06, 0x5F, 0x4F,
       	0x50, 0x82, 0x54, 0x80, 0x0B, 0x3E, 0x00, 0x40,
       	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEA, 0x0C,
       	0xDF, 0x28, 0x00, 0xE7, 0x04, 0xE3, 0xFF, 0x00,
       	0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x0F, 0xFF,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x01, 0x00, 0x0F, 0x00, 0x00
};

unsigned char g_320x200x256[] =
{
	0x63, 0x03, 0x01, 0x0F, 0x00, 0x0E, 0x5F, 0x4F,
       	0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F, 0x00, 0x41,
       	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9C, 0x0E,
       	0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3, 0xFF, 0x00,
       	0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F, 0xFF,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x41, 0x00, 0x0F, 0x00,	0x00
};

/*****************************************************************************
 * Write a byte to a 16 bit port
*****************************************************************************/
void outportb(unsigned int port, unsigned char value)
{
;
#asm
	XOR EAX,EAX
	MOV DX, [EBP+12]
	MOV AL, [EBP+8]
	OUT DX, AL
#endasm

}

/*****************************************************************************
 * Read a byte from a 16 bit port
*****************************************************************************/
unsigned char inportb(unsigned int port)
{
;
#asm
	MOV DX, [EBP+8]
	XOR EAX,EAX
	IN AL, DX
#endasm

}
/*****************************************************************************
 * Write out register settings for selected graphics mode
*****************************************************************************/
void write_regs(unsigned char *regs)
{
	unsigned i;

/* write MISCELLANEOUS reg */
	outportb(MISCPort2, *regs);
	regs++;
/* write SEQUENCER regs */
	for(i = 0; i < VGA_NUM_SEQ_REGS; i++)
	{
		outportb(SEQPort1, i);
		outportb(SEQPort2, *regs);
		regs++;
	}
/* unlock CRTC registers */
	outportb(CRTCPort1, 0x03);
	outportb(CRTCPort2, inportb(CRTCPort2) | 0x80);
	outportb(CRTCPort1, 0x11);
	outportb(CRTCPort2, inportb(CRTCPort2) & ~0x80);
/* make sure they remain unlocked */
	regs[0x03] |= 0x80;
	regs[0x11] &= ~0x80;
/* write CRTC regs */
	for(i = 0; i < VGA_NUM_CRTC_REGS; i++)
	{
		outportb(CRTCPort1, i);
		outportb(CRTCPort2, *regs);
		regs++;
	}
/* write GRAPHICS CONTROLLER regs */
	for(i = 0; i < VGA_NUM_GC_REGS; i++)
	{
		outportb(GCPort1, i);
		outportb(GCPort2, *regs);
		regs++;
	}
/* write ATTRIBUTE CONTROLLER regs */
	for(i = 0; i < VGA_NUM_AC_REGS; i++)
	{
		inportb(GCISPort2);
		outportb(ACPort1, i);
		outportb(ACPort1, *regs);
		regs++;
	}
/* lock 16-color palette and unblank display */
	inportb(GCISPort2);
	outportb(ACPort1, 0x20);
}
/*****************************************************************************
 * Set graphics plane
 * *****************************************************************************/
static void set_plane(unsigned p)
{
        unsigned char pmask;

        p &= 3;
        pmask = 1 << p;
/* set read plane */
        outportb(GCPort1, 4);
        outportb(GCPort2, p);
/* set write plane */
        outportb(SEQPort1, 2);
        outportb(SEQPort2, pmask);
}
/*****************************************************************************
 * VGA framebuffer is at A000:0000, B000:0000, or B800:0000
 * depending on bits in GC 6
 * *****************************************************************************/
static unsigned get_fb_seg(void)
{
        unsigned seg;

        outportb(GCPort1, 6);
        seg = inportb(GCPort2);
        seg >>= 2;
        seg &= 3;
        switch(seg)
        {
        case 0:
        case 1:
                seg = 0xA000;
                break;
        case 2:
                seg = 0xB000;
                break;
        case 3:
                seg = 0xB800;
                break;
        }
        return seg;
}
/*****************************************************************************
 * Video memory write - used by character generator
 * *****************************************************************************/
static void _vmemwr(unsigned dst_seg, unsigned dst_off, unsigned char *src, unsigned count)
{
        unsigned char *dest;
	unsigned int pos;

	for(pos = 0; pos < count; pos++)
	{
		dest=(dst_seg * 16) + dst_off + pos;
		*dest=*(src + pos);
	}
}
/*****************************************************************************
 * Wrapper for Video memory write
 * *****************************************************************************/
static void vmemwr(unsigned dst_off, unsigned char *src, unsigned count)
{
        _vmemwr(get_fb_seg(), dst_off, src, count);
}

/*****************************************************************************
 * write font to plane P4 (assuming planes are named P1, P2, P4, P8)
 * *****************************************************************************/
static void write_font(unsigned char *buf, unsigned font_height)
{
        unsigned char seq2, seq4, gc4, gc5, gc6;
        unsigned i;

/* save registers
 * set_plane() modifies GC 4 and SEQ 2, so save them as well */
        outportb(SEQPort1, 2);
        seq2 = inportb(SEQPort2);

        outportb(SEQPort1, 4);
        seq4 = inportb(SEQPort2);
/* turn off even-odd addressing (set flat addressing)
 * assume: chain-4 addressing already off */
        outportb(SEQPort2, seq4 | 0x04);

        outportb(GCPort1, 4);
        gc4 = inportb(GCPort2);

        outportb(GCPort1, 5);
        gc5 = inportb(GCPort2);
/* turn off even-odd addressing */
        outportb(GCPort2, gc5 & ~0x10);

        outportb(GCPort1, 6);
        gc6 = inportb(GCPort2);
/* turn off even-odd addressing */
        outportb(GCPort2, gc6 & ~0x02);
/* write font to plane P4 */
        set_plane(2);
/* write font 0 */ 
        for(i = 0; i < 256; i++)
        {
                vmemwr(16384u * 0 + i * 32, buf, font_height);
                buf += font_height;
        }
/* restore registers */
        outportb(SEQPort1, 2);
        outportb(SEQPort2, seq2);
        outportb(SEQPort1, 4);
        outportb(SEQPort2, seq4);
        outportb(GCPort1, 4);
        outportb(GCPort2, gc4);
        outportb(GCPort1, 5);
        outportb(GCPort2, gc5);
        outportb(GCPort1, 6);
        outportb(GCPort2, gc6);
}
/*****************************************************************************
 * MMURTL SetVidMode function
 * Allows for the following modes:
 * 0 - 80x25 text mode
 * 1 - 80x50 text mode
 * 2 - 640x480x16 graphics mode
 * 3 - 320x200x256 graphics mode
 *
 * Returns ErcBadVidMode on error
*****************************************************************************/
long far _SetVidMode(unsigned long vmode)
{
	long erc;
	erc = 0;

	switch(vmode) {
		/* Set Text modes */
		case 0:
			write_regs(g_80x25_text);
			write_font(IBM_VGA_8x16, 16);
			GetJobNum(&JobNum);
			GetpJCB(JobNum, *pTmpJCB);
			pTmpJCB->VidMode = 0;
			pTmpJCB->nLines = 25;
			break;

		case 1:
			write_regs(g_80x50_text);
			write_font(IBM_VGA_8x8, 8);
			GetJobNum(&JobNum);
			GetpJCB(JobNum, *pTmpJCB);
			pTmpJCB->VidMode = 1;
			pTmpJCB->nLines = 50;
			break;
		/* Set Graphics modes */
		case 2:
			write_regs(g_640x480x16);
			write_font(IBM_VGA_8x8, 8);
			GetJobNum(&JobNum);
			GetpJCB(JobNum, *pTmpJCB);
			pTmpJCB->VidMode = 2;
			break;

		case 3:
			write_regs(g_320x200x256);
			write_font(IBM_VGA_8x8, 8);
			GetJobNum(&JobNum);
			GetpJCB(JobNum, *pTmpJCB);
			pTmpJCB->VidMode = 3;
			break;

		default:
			erc = ErcBadVidMode;
			break;
	}
	return erc;
}
