/* mouse driver for MMURTL v0.1 */
/* Written by James T. Sprinkle */

#define U32 unsigned long
#define S32 long
#define U16 unsigned int
#define S16 int
#define U8 unsigned char
#define S8 char

/* MMURTL OS PROTOTYPES */

extern far AllocExch(U32  *pExchRet);
extern far U32  InitDevDr(U32  dDevNum,
                                          S8  *pDCBs,
                                                  U32  nDevices,
                                                  U32  dfReplace);

extern far U32  UnMaskIRQ(U32  IRQNum);
extern far U32  MaskIRQ(U32  IRQNum);
extern far U32  SetIRQVector(U32  IRQNum, S8  *pIRQ);
extern far U32  EndOfIRQ(U32  IRQNum);
extern far U32  SendMsg(U32  Exch, U32  msg1, U32  msg2);
extern far U32  ISendMsg(U32  Exch, U32  msg1, U32  msg2);
extern far U32  WaitMsg(U32  Exch, U32  *pMsgRet);
extern far U32  CheckMsg(U32  Exch, U32  *pMsgRet);
extern far U32  Alarm(U32  Exch, U32  count);
extern far U32  KillAlarm(U32  Exch);
extern far U32  Sleep(U32  count);
extern far void MicroDelay(U32  us15count);
extern far void OutByte(U8 Byte, U16 wPort);
extern far void OutWord(U16 Word, U16 wPort);
extern far U8 InByte(U16 wPort);
extern far U16 InWord(U16 wPort);
extern far U8 ReadCMOS(U16 Address);
extern far void CopyData(U8 *pSource, U8 *pDestination, U32 dBytes);
extern far InWords(U32 dPort, U8 *pDataIn, U32 dBytes);
extern far OutWords(U32 dPort, U8 *pDataOut, U32 dBytes);

/* Near External for troubleshooting */

extern long xprintf(char *fmt, ...);

/* Local Prototypes */

static void interrupt mouse_isr(void);

/* The following 3 calls are required in every MMURTL device driver */

static U32  mousedev_op(U32  dDevice,
                    U32  dOpNum,
                    U32  dLBA,
                    U32  dnBlocks,
                    U8  *pData);

static U32  mousedev_stat(U32  dDevice,
                          S8 * pStatRet,
                          U32  dStatusMax,
                          U32  *pdSatusRet);

static U32  mousedev_init(U32  dDevNum,
                          S8  *pInitData,
                          U32   sdInitData);

static U32 mouse_exch;
static U32 mouse_msg;
static S8 mouse_buf[3];
static U8 mouse_buf_ptr;
static S16 mousex;
static S16 mousey;
static U8 buttons;

/* Ports and masks */

#define MOUSE_DATA	0x60	/* Read/Write */
#define MOUSE_STAT	0x64	/* Read Status Register */
#define MOUSE_CMD	0x64	/* Write Command Register */

#define OUT_BUF_STAT	0x01	/* Output Buffer Status 0=empty, 1=full */
#define IN_BUF_STAT	0x02	/* Input Buffer Status 0=empty, 1=full */
#define CMD_DATA	0x08	/* Command/Data 0=device, 1=controller */
#define TO_ERROR	0x40	/* Timeout Error 0=no, 1=yes */
#define PAR_ERROR	0x80	/* Parity Error 0=no, 1=yes */

/* Return Codes and Function Codes */
#define ErcBadOp	664
#define CmdNull		0

/* mousedcb type */
static struct dcbtype
{
        S8   Name[12];
        S8   sbName;
        S8   type;
        S16  nBPB;
        U32  last_erc;
        U32  nBlocks;
        S8  *pDevOp;
        S8  *pDevInit;
        S8  *pDevSt;
        S8   fDevReent;
        S8   fSingleUser;
        S16  wJob;
        U32  OS1;
        U32  OS2;
        U32  OS3;
        U32  OS4;
        U32  OS5;
        U32  OS6;
        };

static struct dcbtype mdcb;

U32 mouse_setup(void)
{
U32 erc;
U8 mstatus;

	mdcb.Name[0] = 'M';
	mdcb.Name[1] = 'O';
	mdcb.Name[2] = 'U';
	mdcb.Name[3] = 'S';
	mdcb.Name[4] = 'E';
	mdcb.sbName = 5;
	mdcb.type = 2;
	mdcb.nBPB = 1;
	mdcb.nBlocks = 0;
	mdcb.pDevOp = &mousedev_op;
	mdcb.pDevInit = &mousedev_init;
	mdcb.pDevSt = &mousedev_stat;

	mouse_buf_ptr = 0;
	mousex = 0;
	mousey = 0;
	buttons = 0;

	OutByte(0xA8, MOUSE_CMD);		/* Enable second PS2 port for mouse */
	OutByte(0x20, MOUSE_CMD);		/* Read controller configuration byte */
	mstatus = InByte(MOUSE_DATA) | 2;	/* Enable second PS2 interrupt bit */
	OutByte(0x60, MOUSE_CMD);		/* Write controller configuration byte */	
	OutByte(mstatus, MOUSE_DATA);		/* Send it */

	OutByte(0xD4, MOUSE_CMD);		/* Write controller configuration byte */
	OutByte(0xF4, MOUSE_DATA);		/* Enable reporting */
	InByte(MOUSE_DATA);			/* Eat status */

	erc = AllocExch(&mouse_exch);

	SetIRQVector(12, &mouse_isr);
	UnMaskIRQ(12);

	return(erc = InitDevDr(9, &mdcb, 1, 1));
}

static void interrupt mouse_isr(void)
{
U8 status;

	status = InByte(MOUSE_CMD);
	if(!(status & 0x20))
		EndOfIRQ(9);

	mouse_buf[mouse_buf_ptr] = InByte(MOUSE_DATA);

	mouse_buf_ptr = (mouse_buf_ptr + 1) % 3;

	if(mouse_buf_ptr == 0)
	{
		buttons = mouse_buf[0];
		mousex = mouse_buf[1];
		mousey = mouse_buf[2];
		xprintf("x=%d, y=%d, buttons=%d\r\n", mousex, mousey, buttons & 0x07);
	}
	EndOfIRQ(9);
}

static U32  mousedev_op(U32  dDevice,
                    U32  dOpNum,
                    U32  dLBA,
                    U32  dnBlocks,
                    U8  *pData)
{
U32 erc;
erc = 0;

switch(dOpNum)
{
	case(CmdNull):		/* NULL Command */
		erc = 0;
		break;

	default:
		erc = ErcBadOp;
		break;
}

mdcb.last_erc = erc;

return(erc);
}

static U32  mousedev_stat(U32  dDevice,
                          S8 * pStatRet,
                          U32  dStatusMax,
                          U32  *pdSatusRet)
{
return 0;
}

static U32  mousedev_init(U32  dDevNum,
                          S8  *pInitData,
                          U32   sdInitData)
{
mdcb.last_erc = 0;
return 0;
}

