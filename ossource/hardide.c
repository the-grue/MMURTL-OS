/* IDE ATA-2 Device Driver for MMURTL.
   This driver depends on data requested from ATA-2 devices
   for drive geometry.  This will not work for older IDE
   devices. This REPLACES the old HARDIDE.C driver.
   This requires the newer Fsys.c (Version 1.1).

  MMURTL Operating System Source Code
  Copyright 1991,1992,1993,1994,1995,1996 Richard A. Burgess
  ALL RIGHTS RESERVED   Version 1.1
*/

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


/* LOCAL PROTOTYPES */

U32  hdisk_setup(void);
static void interrupt hdisk_isr(void); 	/* The HD interrupt function */
static void interrupt hdisk_isr2(void); /* Second HD interrupt func  */
static U32  hd_format_track(U32 dLBA, U32 dnBlocks, U8 hdrive);
static void hd_reset(U8 controller);
static U32  send_command(U8  parm, U8 hdrive);
static U32  hd_wait (U8 controller);
static U32  check_busy(U8 controller);
static U32  hd_seek(U32 dLBA, U8 hdrive);
static U32  hd_recal(U8 drive);
static U32  hd_write(U32 dLBA, U32 dnBlocks, U8 *pDataOut, U8 hdrive);
static U32  hd_read(U32 dLBA, U32 dnBlocks, U8 *pDataIn, U8 hdrive);
static U32 hd_status(U8 LastCmd, U8 hdrive);
static U32  setupseek(U32 dLBA, U32 nBlks, U8 hdrive);
static U32  hd_init(U8 drive);
static U32 ReadSector(U32 Cylinder, U32 HdSect, U8 *pDataRet, U8 hdrive);

/* The following 3 calls are required in every MMURTL device driver */

static U32  hddev_op(U32  dDevice,
 		    U32  dOpNum,
		    U32  dLBA,
		    U32  dnBlocks,
		    U8  *pData);

static U32  hddev_stat(U32  dDevice,
			  S8 * pStatRet,
			  U32  dStatusMax,
			  U32  *pdSatusRet);

static U32  hddev_init(U32  dDevNum,
			  S8  *pInitData,
			  U32   sdInitData);

/* LOCAL DEFINITIONS */

#define ok 0

/* Error Codes to return */

#define ErcNoMsg		20
#define ErcNotInstalled 	504

#define ErcBadBlock		651
#define ErcAddrMark		652
#define ErcBadECC		653
#define ErcSectNotFound		654
#define ErcNoDrive0		655
#define ErcNotSupported 	656
#define ErcBadHDC		658
#define ErcBadSeek		659
#define ErcHDCTimeOut		660
#define ErcOverRun		661
#define ErcBadLBA		662
#define ErcInvalidDrive		663
#define ErcBadOp		664
#define ErcBadRecal		665
#define ErcSendHDC		666
#define ErcNotReady		667
#define ErcBadCmd		668
#define ErcNeedsInit		669
#define ErcTooManyBlks		670		/* The controller can only do 128 max */
#define ErcZeroBlks		671		/* 0 Blocks not allowed for this cmd */
#define ErcWriteFault		672		/* WriteFault bit set... bummer */

#define ErcMissHDDInt   	675

#define ErcHDDMsgBogus  	676
#define ErcHDDIntMsg		677
#define ErcHDDAlarmMsg  	678

/* Commands accepted by this HD driver */

#define CmdNull     0
#define CmdRead     1
#define CmdWrite    2
#define CmdVerify   3
#define CmdFmtBlk   4
#define CmdFmtTrk   5
#define CmdSeekTrk  6
#define CmdSetMedia 7   /* Not used unless mountable */
#define CmdResetHdw 8   /* Used to reset controller hardware */

/* CmdReadSect is the only device specific call in the IDE hard
   disk device driver.  This allows you to read ONE sector
   specified by Cylinder, head and Sector number.
   Cylinder is HiWord of dLBA in DeviceOp call,
   Head is LoWord of dLBA in DeviceOp call, and
   Sector number is LowWord in dnBlocks.
*/

#define CmdReadSect 256 /* only device specific call in HDD */

/* HDC port definitions */

#define HD_PORT 0x1f0

/* When writing to the port+X (where X =):
	 0 - write data       (1F0h - 16 bit)
	 1 - pre-comp         (1F1h)
	 2 - sector count     (1F2h)
	 3 - sector number    (1F3h)
	 4 - low cyl          (1F4h)
	 5 - high cyl         (1F5h)
	 6 - size/drive/head  (1F6h)
	 7 - command register (1F7h)

When reading from the port+X (where X =):
	 0 - read data       (1F0h - 16 bit)
	 1 - error register  (1F1h)
	 2 - sector count    (1F2h)
	 3 - sector number   (1F3h)
	 4 - low cyl         (1F4h)
	 5 - high cyl        (1F5h)
	 6 - size/drive/head (1F6h)
	 7 - status register (1F7h)
*/

#define HD_REG_PORT 0x3f6

/* This is a byte wide write only control port
   that allows reset and defines some special
   characteristics of the hard drives.
	Bit		Desc
	0		Not used
	1		Not used
	2		Reset Bit - Set, wait 50us, then Reset
	3		Mucho Heads Flag. Set = More than 8 heads
	4		Not used
	5		Not used
	6		Disable retries
	7		Disable retries (same as six, either one set)
*/

/* Ports for secondary ide controller  */
#define HD_PORT_2	0x170
#define HD_REG_PORT_2	0x376

/* HDC Status Register Bit Masks (1F7h) */

#define BUSY        0x80  /* busy.. can't talk now! */
#define READY       0x40  /* Drive Ready  */
#define WRITE_FAULT 0x20  /* Bad news */
#define SEEKOK      0x10  /* Seek Complete */
#define DATA_REQ    0x08  /* Sector buffer needs servicing */
#define CORRECTED   0x04  /* ECC corrected data was read */
#define REV_INDEX   0x02  /* Set once each disk revolution */
#define ERROR       0x01  /* data address mark not found */

/* HDC Error Register Bit Masks (1F1h) */

#define BAD_SECTOR  0x80  /* bad block */
#define BAD_ECC     0x40  /* bad data ecc */
#define BAD_IDMARK  0x10  /* id not found */
#define BAD_CMD     0x04  /* aborted command */
#define BAD_SEEK    0x02  /* trk 0 not found on recalibrate, or bad seek */
#define BAD_ADDRESS 0x01  /* data address mark not found */


/* HDC internal command bytes (HDC_Cmd[7]) */

#define HDC_RECAL      0x10	  /* 0001 0000 */
#define HDC_READ       0x20   /* 0010 0000 */
#define HDC_READ_LONG  0x22   /* 0010 0010 */
#define HDC_WRITE      0x30   /* 0011 0000 */
#define HDC_WRITE_LONG 0x32   /* 0011 0010 */
#define HDC_VERIFY     0x40   /* 0100 0000 */
#define HDC_FORMAT     0x50   /* 0101 0000 */
#define HDC_SEEK       0x70   /* 0111 0000 */
#define HDC_DIAG       0x90   /* 1001 0000 */
#define HDC_SET_PARAMS 0x91   /* 1001 0001 */
#define HDC_ID         0xEC   /* 1001 0001 */

/* L O C A L   D A T A  */
/* Made all of these arrays to support
 * multiple controllers!  */

static U8 hd_Cmd[2][8];		/* For all 8 command bytes, controllers 0-1 */

static U8 fDataReq[4];		/* Flag to indicate if fDataRequest is active, drives 0-3 */
static U8 statbyte[2];		/* From HDC status register last time it was read, controllers 0-1 */

static U8 hd_control[4];	/* Current control byte value, controller 0-1 */

static U8  hd_drive[4];		/* Current Physical Drive, 0-3 */
static U8  hd_head[4];		/* Calculated from LBA - which head, drives 0-3 */
static U8  hd_nsectors[4];	/* Calculated from LBA - n sectors to read/write, drives 0-3 */
static U8  hd_sector[4];	/* Calculated from LBA - Starting sector, drives 0-3 */

/* Current type drive 0-3 set by caller. */
/* Current number of heads, cylinders, and sectors set by caller */
static U8	hd_type[4];
static U8	hd_heads[4];
static U8	hd_secpertrk[4];
static U16	hd_cyls[4];

#define sIDEid 512

static struct idestruct
 {			/* 16 BIT WORD offset */
  U16 config;		/* 0  General IDE configuration */
  U16 ncyls;		/* 1 */
  U16 rsvd1;		/* 2 */
  U16 nheads;		/* 3 */
  U16 rsvd2;  		/* 4 */
  U16 rsvd3;  		/* 5 */
  U16 nsectpertrk;	/* 6 */
  U16 rsvd4[3];		/* 7-9    Vendor specific */
  U16 serial[10];	/* 10-19  Device serial # */
  U16 rsvd5[2];     	/* 20-21  Vendor specific */
  U16 bytesavail;	/* 22     # of vendor specific bytes avail on read LONG */
  U8 firmrev[8];	/* 23-26  Firmware revision - ASCII 8 */
  U8 model[40];		/* 27-46  Device Model # */
  U16 rdmulti;		/* 47     Info on Read Multiple commands */
  U16 rsvd6;		/* 48 */
  U16 capabil;		/* 49     Capabilties */
  U16 rsvd7;		/* 50 */
  U16 pioxfermode;	/* 51 */
  U16 dmaxfermode;	/* 52 */
  U16 validflds;	/* 53 */
  U16 ncylsx;		/* 54-56 current/xlated cyls, heads, sect per tracks */
  U16 nheadsx;
  U16 nsectpertrkx;
  U32 capsectors;	/* 57-58 long */
  U16 rdmultisect;	/* 59    More info in read multiple sectors command */
  U32 totalsectlba;	/* 60-61 Total sectors in LBA mode only */
  U16 singldmaxfer;	/* 62 */
  U16 multidmaxfer;	/* 63 */
  U16 advancedpio;	/* 64 */
  U16 dmatimemin;	/* 65 */
  U16 dmatimerec;	/* 66 */
  U16 piotimemin;	/* 67 */
  U16 piotimerec;	/* 68 */
  U16 piorsvd;		/* 69 */
  U16 piorsvd1;		/* 70 */
  U16 rsvd8[127-71+1]; 	/* 71-127 */
  U16 rsvd9[159-128+1];	/* 128-159 */
  U16 rsvd9[255-160+1];	/* 160-255 */
  };

/* One struct per disk instead of a single shared one  */
static struct idestruct IDEid[4];

#define sStatus 64

static struct statstruct
 {
  U32 erc;
  U32 blocks_done;
  U32 BlocksMax;
  U8 fNewMedia;
  U8 type_now;		/* current fdisk_table for drive selected */
  U8 resvd0[2];		/* padding for DWord align  */
  U32 nCyl;		/* total physical cylinders */
  U32 nHead;		/* total heads on device    */
  U32 nSectors;		/* Sectors per track        */
  U32 nBPS;		/* Number of bytes per sect.  32 bytes out to here.*/

  U32 LastRecalErc0;
  U32 LastSeekErc0;
  U8  LastStatByte0;
  U8  LastErcByte0;
  U8  fIntOnReset;	/* Interrupt was received on HDC_RESET */
  U8  filler0;

  U32 LastRecalErc1;
  U32 LastSeekErc1;
  U8  LastStatByte1;
  U8  LastErcByte1;
  U8  ResetStatByte;	/* Status Byte immediately after RESET */
  U8  filler1;

  U32 resvd1[2];	/* out to 64 bytes */
  };

/* One struct per disk instead of a single shared one  */
static struct statstruct hdstatus[4];

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
	U8   fDevReent;
	U8   fSingleUser;
	S16  wJob;
	U32  OS1;
	U32  OS2;
	U32  OS3;
	U32  OS4;
	U32  OS5;
	U32  OS6;
	};

static struct dcbtype hdcb[4];	/* four HD device control blocks */

/* Exch and msgs space for HD ISR */

static U32 hd_exch[2]; 
static U32 hd_msg[2];

static long HDDInt[2]; 


/*======================================================*/
/*=================== START OF CODE ====================*/
/*======================================================*/

/*********************************************************
    This is called ONCE to initialize the HD Driver.
*********************************************************/

U32  hdisk_setup(void)
{
U32  erc;
U8 counter;

/* first we set up the 4 DCBs in anticipation of calling InitDevDr */

	hdcb[0].Name[0]  = 'H';
	hdcb[0].Name[1]  = 'D';
	hdcb[0].Name[2]  = '0';
	hdcb[0].sbName   = 3;
	hdcb[0].type     = 1;	 	/* Random */
	hdcb[0].nBPB     = 512;
	hdcb[0].nBlocks  = 524288;   	/* largest disk handled - 2Gb disks*/
	hdcb[0].pDevOp   = &hddev_op;
	hdcb[0].pDevInit = &hddev_init;
	hdcb[0].pDevSt   = &hddev_stat;

	hdcb[1].Name[0]  = 'H';
	hdcb[1].Name[1]  = 'D';
	hdcb[1].Name[2]  = '1';
	hdcb[1].sbName   = 3;
	hdcb[1].type     = 1;		/* Random */
	hdcb[1].nBPB     = 512;
	hdcb[1].nBlocks  = 524288;	/* largest device handled - 2Gb disks*/
	hdcb[1].pDevOp   = &hddev_op;
	hdcb[1].pDevInit = &hddev_init;
	hdcb[1].pDevSt   = &hddev_stat;

	hdcb[2].Name[0]  = 'H';
	hdcb[2].Name[1]  = 'D';
	hdcb[2].Name[2]  = '2';
	hdcb[2].sbName   = 3;
	hdcb[2].type     = 1;		/* Random */
	hdcb[2].nBPB     = 512;
	hdcb[2].nBlocks  = 524288;	/* largest device handled - 2Gb disks*/
	hdcb[2].pDevOp   = &hddev_op;
	hdcb[2].pDevInit = &hddev_init;
	hdcb[2].pDevSt   = &hddev_stat;

	hdcb[3].Name[0]  = 'H';
	hdcb[3].Name[1]  = 'D';
	hdcb[3].Name[2]  = '3';
	hdcb[3].sbName   = 3;
	hdcb[3].type     = 1;		/* Random */
	hdcb[3].nBPB     = 512;
	hdcb[3].nBlocks  = 524288;	/* largest device handled - 2Gb disks*/
	hdcb[3].pDevOp   = &hddev_op;
	hdcb[3].pDevInit = &hddev_init;
	hdcb[3].pDevSt   = &hddev_stat;

/* These are defaulted to non zero values to
   ensure we don't get a divide by zero during initial calculations
   on the first read.
*/

	hd_type[0] = 1;
	hd_type[1] = 1;
	hd_type[2] = 1;
	hd_type[3] = 1;

/* IRQ Vector for first disk controller at IRQ 14 */
	erc = AllocExch(&hd_exch[0]);	

	SetIRQVector(14, &hdisk_isr);
	UnMaskIRQ(14);

/* IRQ Vector for second disk controller at IRQ 15 */
 
  	erc = AllocExch(&hd_exch[1]);
 
	SetIRQVector(15, &hdisk_isr2);
	UnMaskIRQ(15);

/* Documentation lists the fixed disk types at CMOS 11h and 12h,
   and also shows them at 19h and 1Ah.  We don't actually read them
   because they are not dependable. They vary from BIOS to BIOS.
   We have to make this sucker work the hard way.
*/

/* Reset the HDCs - The reset is a hardware reset. */

	hd_reset(0);		/* no error is returned */
	hd_reset(1);		/* no error is returned */

/* Now we attempt to select and recal all drives.
   The driver MUST be able to recal the first physical drive
   or the Driver won't install.
*/
	for(counter = 0; counter < 4; counter++)
	{
		erc = hd_recal(counter);			/* Try to recalibrate */
		if (erc)					/* Try once again if error */
		{
			hd_reset((counter & 0x02) >> 1);	/* Reset... */
			erc = hd_recal(counter);		/* Recal... */
			if (erc)				/* Error again? */
			{
				hdcb[counter].last_erc = erc;	/* Log the error */
				hd_type[counter] = 0;		/* Not a valid drive */
				if (counter == 0)		/* If drive 0, don't install driver */
					return(ErcNoDrive0);
			}
		}
	}

	/* We must redo drive 0 cause some cheap controllers lockup
	   on us if drive 1 is not there.  They SHOULD simply return
	   a Bad Command bit set in the Error register, but they don't. */
	hd_reset(0);
	erc = hd_recal(0);
	hdcb[0].last_erc = erc;

	return(erc = InitDevDr(12, &hdcb, 4, 1));
}

/*************************************************************
  This reads the 512 bytes of ID from the controller
  which determines drive geometry (nCyls, nHeads, nSectors, etc.).
****************************************************************/

static U32  hd_init(U8 drive)
{
U32 erc;
U16 hd_port;
U8 drive_base;
U8 controller;

	drive_base = drive;				/* Used to differentiate drives/controllers */
	if (drive_base == 0 || drive_base == 1)		/* This is controller 0 */
	{
		hd_port = HD_PORT;
		controller = 0;
	}
	else if (drive_base == 2 || drive_base == 3)	/* This is controller 1 */
	{
		hd_port = HD_PORT_2;
		drive_base = drive_base & 0x01;		/* Base must be 0 or 1 */
		controller = 1;
	}
	else
	{
		erc = ErcInvalidDrive;			/* Drive not in valid range */
		return(erc);
	}

	hd_Cmd[controller][1] = 0;
	hd_Cmd[controller][2] = 0;
	hd_Cmd[controller][3] = 0;
	hd_Cmd[controller][4] = 0;
	hd_Cmd[controller][5] = 0;
	hd_Cmd[controller][6] = (drive_base << 4);	/* drive */

	erc = send_command(HDC_ID, drive);

	erc = hd_wait(controller);			/* wait for interrupt */
	if (!erc)
		erc = hd_status(HDC_READ, drive);
	if (!erc)
		InWords(hd_port, &IDEid[drive], 512);

	return(erc);
}

/************************************************************
 Reset the HD controller.  This should only be called by
 DeviceInit or hdisk_setup.  This resets the controller
 and calls hd_init which IDs the ATA device which fills the
 IDEid structure.   This also attempts to recal them.
*************************************************************/

static void hd_reset(U8 controller)
{
U32 i;
U16 port;

	if (controller == 0)
	{
		port = HD_REG_PORT;
	}
 	else 
 	{
 		port = HD_REG_PORT_2;
 	}

 	UnMaskIRQ(14 + controller);		/* enable the IRQ */
 	OutByte(4, port);			/* reset controller */
 	MicroDelay(4);				/* Delay 60 us */

    	/* bit 3 of HD_REG must be 1 for access to heads 8-15 */
    	/* Clear "MUCHO" heads bit, and clear the reset bit */

	OutByte(hd_control[controller] & 0x0f, port);

	Sleep(20);		/* 200ms - seems some controllers are SLOW!! */

	i = CheckMsg(hd_exch[controller], &hd_msg[controller]);	/* Eat Int if one came back */

	hdstatus[controller].ResetStatByte = statbyte[controller];	/* The ISR gets statbyte */

	/* For my general knowledge */
	if (i)
		hdstatus[controller * 2].fIntOnReset = 1;
	else
		hdstatus[controller * 2].fIntOnReset = 0;
}

/*************************************************************
 The ISRs are VERY simple. They just wait for an interrupt, gets
 the single status byte from the controller (which clears the
 interrupt condition) then sends an empty message to the
 exchange where the HD Driver task will be waiting.
 This tells the HD task currently running that it's got
 some status to act on!
****************************************************************/
static void interrupt hdisk_isr(void)
{
	statbyte[0] = InByte(HD_PORT+7);
    	HDDInt[0] = 1;
	ISendMsg(hd_exch[0], 0xfffffff0, 0xfffffff0);
	EndOfIRQ(14);
}

/*  ISR for second disk controller */
static void interrupt hdisk_isr2(void)
{
	statbyte[1] = InByte(HD_PORT_2+7);
    	HDDInt[1] = 1;
	ISendMsg(hd_exch[1], 0xfffffff0, 0xfffffff0);
	EndOfIRQ(15);
}

/*************************************************************
 This checks the HDC controller to see if it's busy so we can
 send it commands or read the rest of the registers.
 We will wait up to 3 seconds then error out.
 The caller should call check_busy and check the error.
 If it's 0 then the controller became ready in less than
 3 seconds.  ErcNotReady will be returned otherwise.
 It leaves the status byte in the global statbyte.
****************************************************************/

static U32 check_busy(U8 controller)
{
S16 count;
U16 port;
 
	if (controller == 0)
  		port = HD_PORT;
	else
  		port = HD_PORT_2;

	count = 0;
	while (count++ < 60)
	{
		statbyte[controller] = InByte(port+7);
		if ((statbyte[controller] & BUSY) == 0) 
			return(ok);
		Sleep(5);	/* 50ms shots */
	}
	return(ErcNotReady);	/* controller out to lunch! */
}

/******************************************
Wait for the hardware interrupt to occur.
Time-out and return if no interrupt.
********************************************/

static U32 hd_wait(U8 controller)
{
U32  erc;

	/* Set alarm for 3 seconds */

	HDDInt[controller] = 0;
	KillAlarm(hd_exch[controller]);		/* kill any pending alarm */

	erc = Alarm(hd_exch[controller], 300);	/* Set it up again */
	if (erc)
		return(erc);			/* bad problem */

	erc = WaitMsg(hd_exch[controller], &hd_msg[controller]);

	KillAlarm(hd_exch[controller]);

	if (hd_msg[controller] != 0xfffffff0)
	{					/* HD interrupt sends fffffff0 */
		if (HDDInt[controller])
			return(ErcMissHDDInt);
		else				/* Alarm sends 0xffffffff */
			return(ErcHDCTimeOut);
	}
	else
	{
		KillAlarm(hd_exch[controller]);
		return(ok);
	}
}

/********************************************
    Recalibrate the drive.
*********************************************/

static U32  hd_recal(U8 drive)
{
U32 erc;
U8 controller;
U8 drive_base;

	
	controller = (drive & 0x02) >> 1;	/* select appropriate controller */
	drive_base = drive & 0x01;		/* drive must be 0 or 1 */

	hd_Cmd[controller][6] = (drive_base << 4) | (hd_head[drive] & 0x0f) | 0xa0;
	erc = send_command(HDC_RECAL, drive);
	if (!erc)
		erc = hd_wait(controller);	/* wait for interrupt */
	if (!erc)
		erc = hd_status(HDC_RECAL, drive);
	hdstatus[drive].LastRecalErc0 = erc;

	return(erc);
}


/********************************************
    Send the command to the controller.
    Clear the Echange of any left over
    alarm or int messages before we
    send a command.
*********************************************/

static U32 send_command(U8 Cmd, U8 hdrive)
{
U32 erc, msg[2];
U16 port, regport;
U8 controller;

	controller = (hdrive & 0x02) >> 1;	/* select appropriate controller */
	if (controller == 0)
	{
		port = HD_PORT;
		regport = HD_REG_PORT;
	}
	else
	{
		port = HD_PORT_2;
		regport = HD_REG_PORT_2;
	}

	while (CheckMsg(hd_exch[controller], &msg) == 0);	/* empty it */

	/* bit 3 of HD_REG must be 1 for access to heads 8-15 */
	if (hd_head[hdrive] > 7)
	{
		hd_control[controller] |= 0x08;
		OutByte(hd_control[controller], regport);	/* set bit for head < 7 */
		hd_control[controller] &= 0xf7;
	}
	erc = check_busy(controller);

    	if (!erc) 
		OutByte(hd_Cmd[controller][1], port+1);
    	if (!erc) 
		erc = check_busy(controller);
    	if (!erc) 
		OutByte(hd_Cmd[controller][2], port+2);
   	if (!erc) 
		erc = check_busy(controller);
    	if (!erc) 
		OutByte(hd_Cmd[controller][3], port+3);
    	if (!erc) 
		erc = check_busy(controller);
    	if (!erc) 
		OutByte(hd_Cmd[controller][4], port+4);
   	if (!erc) 
		erc = check_busy(controller);
    	if (!erc) 
		OutByte(hd_Cmd[controller][5], port+5);
   	if (!erc) 
		erc = check_busy(controller);
    	if (!erc) 
		OutByte(hd_Cmd[controller][6], port+6);
    	if (!erc) 
		erc = check_busy(controller);
    	if (!erc) 
		OutByte(Cmd, port+7);

    	return(erc);
}

/*************************************************************
 This sets up the cylinder, head and sector variables for all
 commands that require them (read, write, verify, format, seek).
 nBlks can NOT be greater than the hardware can handle. For
 IDE/MFM controllers this is 128 sectors.
 The caculated values are placed in the proper command byte
 in anticipation of the command being sent.
*************************************************************/

static U32 setupseek(U32 dLBA, U32 nBlks, U8 hdrive)
{
U32  j;
U16  cyl, port;
U8 controller, drive_base;
 
	controller = (hdrive & 0x02) >> 1;	/* select appropriate controller */
	drive_base = (hdrive & 0x01);		/* drive must be 0 or 1 */

	if (controller == 0)
		port = HD_PORT;
	else
		port = HD_PORT_2;

  	if (nBlks > 256) 
		return ErcTooManyBlks;
  	if (nBlks == 0) 
		return ErcZeroBlks;

	hd_nsectors[hdrive] = nBlks;
  	if (hd_nsectors[hdrive] == 256) 
		hd_nsectors[hdrive] = 0;   /* 0==256 for controller */

	cyl = dLBA / (hd_heads[hdrive] * hd_secpertrk[hdrive]);
	j = dLBA % (hd_heads[hdrive] * hd_secpertrk[hdrive]);	/* remainder */

	/* we now know what cylinder, calculate head and sector */

	hd_head[hdrive] = j / hd_secpertrk[hdrive];
	hd_sector[hdrive] = j % hd_secpertrk[hdrive] + 1;	/* sector number start at 1 !!! */

	hd_Cmd[controller][2] = nBlks;				/* How many sectors */
	hd_Cmd[controller][3] = hd_sector[hdrive];		/* Which sector to start on */
	hd_Cmd[controller][4] = cyl & 0xff;			/* cylinder lobyte */
	hd_Cmd[controller][5] = (cyl >> 8) & 0xff;		/* cylinder hibyte */
	hd_Cmd[controller][6] = (drive_base << 4) | (hd_head[hdrive] & 0x0f) | 0xa0;

	return ok;
}


/*******************************************************
  Move the head to the selected track (cylinder).
*********************************************************/

static U32 hd_seek(U32 dLBA, U8 hdrive)
{
U32 erc;
U8 controller;

	controller = (hdrive & 0x02) >> 1;	/* select appropriate controller */

	erc = setupseek(dLBA, 1, hdrive);	/* sets up for implied seek */
	if (!erc)
		erc = send_command(HDC_SEEK, hdrive);	/* Not implied anymore... */
	if (!erc)
		erc = hd_wait(controller);	/* wait for interrupt */
	if (!erc)
		erc = hd_status(HDC_SEEK, hdrive);

	hdstatus[hdrive].LastSeekErc0 = erc;

	return(erc);
}


/*******************************************************
 Called to read status and errors from the controller
 after an interrupt generated by a command we sent.
 The error checking is based on the command that we sent.
 This is done because certain bits in the status and error
 registers are actually not errors, but simply indicate
 status or indicate an action we must take next.
 ZERO returned indicates no errors for the command status
 we are checking.
*********************************************************/

static U32 hd_status(U8 LastCmd, U8 hdrive)
{
U32 erc;
U16 port;
U8 statbyte, errbyte;
U8 controller;

  /* We shouldn't see the controller busy. After all,
     he interrupted us with status.
  */

	controller = (hdrive & 0x02) >> 1;	/* select appropriate controller */

	if (controller == 0)
		port = HD_PORT;
	else
		port = HD_PORT_2;

	erc = check_busy(controller);		/* puts status byte into global statbyte */
	if (!erc)
		statbyte = InByte(port+7);
	else
		return(erc);

	hdstatus[hdrive].LastStatByte0 = statbyte;

	if((statbyte & ERROR) == 0)
	{	/* Error bit not set in status reg */
    		erc = ok;  			/* default */

		switch (LastCmd)
		{
	  		case HDC_READ:
	  		case HDC_READ_LONG:
	  		case HDC_WRITE:
	  		case HDC_WRITE_LONG:
	  		case HDC_SEEK:
	  		case HDC_RECAL:
				if (statbyte & WRITE_FAULT)
					erc = ErcWriteFault;
				else if ((statbyte & SEEKOK) == 0) 
					erc =	ErcBadSeek;
				break;
	  		case HDC_SET_PARAMS:
	  		case HDC_VERIFY:
	  		case HDC_FORMAT:
	  		case HDC_DIAG:
				break;
	  		default:
				break;
		}
    		return(erc);
	}
	else
	{
		erc = check_busy(controller);
		if (!erc)
	  		errbyte = InByte(port+1);
		else 
			return(erc);

	    	hdstatus[hdrive].LastErcByte0 = errbyte;

	if (errbyte & BAD_ADDRESS) 
		erc = ErcAddrMark;
	else if (errbyte & BAD_SEEK) 
		erc = ErcBadSeek;
	else if (errbyte & BAD_CMD) 
		erc = ErcBadCmd;
	else if (errbyte & BAD_IDMARK) 
		erc = ErcSectNotFound;
	else if (errbyte & BAD_ECC) 
		erc = ErcBadECC;
	else if (errbyte & BAD_SECTOR) 
		erc = ErcBadBlock;
	else erc = ErcBadHDC;	/* no error bits found but should have been! */
  }
 return erc;
}

/*************************************************************
 This is called for the DeviceOp code Read.
 This reads 1 or more whole sectors from the calculated values
 in hd_head, hd_sector, and hd_cyl
*************************************************************/

static U32 hd_read(U32 dLBA, U32 dnBlocks, U8 *pDataRet, U8 hdrive)
{
U32 erc, nleft, nBPS;
U16 port;
U8 controller;

	controller = (hdrive & 0x02) >> 1;	/* select appropriate controller */

	if (controller == 0)
		port = HD_PORT;
	else
		port = HD_PORT_2;

	nBPS = hdcb[hdrive].nBPB;		/* From nBytesPerBlock in DCB */
	nleft = dnBlocks;
	erc = setupseek(dLBA, dnBlocks, hdrive);/* sets up for implied seek */
	if (!erc)
		erc = send_command(HDC_READ, hdrive);

	while ((nleft) && (!erc))
	{
		erc = hd_wait(controller);	/* wait for interrupt */
		if (!erc)
			erc = hd_status(HDC_READ, hdrive);
		if (!erc)
		{
			InWords(port, pDataRet, nBPS);
			pDataRet+=nBPS;
			--nleft;
		}
	}
	return(erc);
}

/*************************************************************
 This is called for the DeviceOp code Write.
 This writes 1 or more whole sectors from the calculated values
 in hd_head, hd_sector, and hd_cyl
*************************************************************/

static U32 hd_write(U32 dLBA, U32 dnBlocks, U8 *pDataOut, U8 hdrive)
{
U32 erc, nSoFar, nBPS;
U16 port;
U8 controller;

	controller = (hdrive & 0x02) >> 1;	/* select appropriate controller */

	if (controller == 0)
		port = HD_PORT;
	else
		port = HD_PORT_2;
	
	nBPS = hdcb[hdrive].nBPB;		/* From nBytesPerBlock in DCB */

	nSoFar = 0;

	erc = setupseek(dLBA, dnBlocks, hdrive);/* sets up for implied seek */
	erc = send_command(HDC_WRITE, hdrive);
	erc = check_busy(controller);		/* No INT occurs for first sector of write */

	if((!erc) && (statbyte[controller] & DATA_REQ))
	{
		OutWords(port, pDataOut, nBPS);
		pDataOut+=nBPS;
		nSoFar++;
	}

	while ((nSoFar < dnBlocks) && (erc==ok))
	{
		erc = hd_wait(controller);	/* wait for interrupt */
		if (erc==ok)
			erc = hd_status(HDC_WRITE, hdrive);
		if ((erc == ok) && (statbyte[controller] & DATA_REQ))
		{
			OutWords(port, pDataOut, nBPS);
			pDataOut+=nBPS;
			nSoFar++;
		}
	}

	if (!erc)
		erc = hd_wait(controller);	/* wait for final interrupt */
	if (!erc)
		erc = hd_status(HDC_WRITE, hdrive);

	return(erc);
}


/*************************************************************
 This formats the track beginning at the block address given
 in dLBA. dLBA must always be a multiple of the number of
 sectors per track minus 1 for the disk type.
*************************************************************/

static U32 hd_format_track(U32 dLBA, U32 dnBlocks, U8 hdrive)
{
U32  erc;
U8 controller;

	controller = (hdrive & 0x02) >> 1;	/* select appropriate controller */
		
	erc = setupseek(dLBA, dnBlocks, hdrive);/* sets up for implied seek */
	erc = send_command(HDC_FORMAT, hdrive);
	erc = hd_wait(controller);		/* wait for interrupt */

	if (erc == ok)
		erc = hd_status(HDC_FORMAT, hdrive);
	return(erc);
}

/******************************************************************
   ReadSector is the only device specific call in the IDE hard
   disk device driver.  This allows you to read ONE sector
   specified by Cylinder, head and Sector number.
   Cylinder is LoWord of dLBA in DeviceOp call,
   Head is LoWord of dnBlocks in DeviceOp call, and
   Sector number is HiWord in dnBlocks.
*******************************************************************/

static U32 ReadSector(U32 Cylinder, U32 HdSect, U8 *pDataRet, U8 hdrive)
{
U32 erc;
U16  cyl;
U16 hd_port;
U8 drive_base;
U8 controller;

	controller = ((hdrive & 0x02) >> 1);	/* select appropriate controller */

	if(controller == 0)
		hd_port = HD_PORT;
	else
	{
		hd_port = HD_PORT_2;
		drive_base = hdrive & 0x01;
	}

	cyl = Cylinder;
	hd_head[hdrive] = HdSect & 0xffff;
	hd_sector[hdrive] = (HdSect >> 16) & 0xffff;

/*	For testing
 	xprintf("\r\nCYL %d, HD %d, SEC %d\r\n", cyl, hd_head[hdrive], hd_sector[hdrive]);
*/

	hd_Cmd[controller][2] = 1;			/* How many sectors */
	hd_Cmd[controller][3] = hd_sector[hdrive];	/* Which sector to start on */
	hd_Cmd[controller][4] = cyl & 0xff;		/* cylinder lobyte */
	hd_Cmd[controller][5] = (cyl >> 8) & 0xff;	/* cylinder hibyte */
	hd_Cmd[controller][6] = (drive_base << 4) | (hd_head[hdrive] & 0x0f) | 0xa0;

	erc = send_command(HDC_READ, hdrive);

	erc = hd_wait(controller);		/* wait for interrupt */
	if (!erc)
		erc = hd_status(HDC_READ, hdrive);
	if (!erc)
		InWords(hd_port, pDataRet, 512);

	return(erc);
}

/***************************************************************************
Now begins the PUBLIC routines that are used for all DEVICE DRIVERS
*/

/******************************************
Called for all device operations.  This
assigns physical device from logical number
that outside callers use. For Hard disk,
12=0 and 13=1. This will check to make sure a
drive type is assigned and check to see if
they are going to exceed max logical blocks.
*******************************************/

static U32  hddev_op(U32  dDevice,
 		      U32  dOpNum,
		      U32  dLBA,
		      U32  dnBlocks,
		      U8  *pData)
{
U32  erc;
U8 hdrive;

	erc = 0;
	hdrive = dDevice - 12;

	hdstatus[hdrive].blocks_done = 0;	/* resets value in status record */

 	/* Based on drive number */
	/* Check to see if we have a leftover interrupt message from last
    	   command.  If so then we eat it (and do nothing) */

	switch(hdrive)
	{
		case 0:
		case 1:
			CheckMsg(hd_exch[0], &hd_msg[0]);
			break;

		case 2:
		case 3:
			CheckMsg(hd_exch[1], &hd_msg[1]);
			break;

		default:
			erc = ErcInvalidDrive;
			break;
	}

	if(hd_type[hdrive] == 0)
		erc = ErcInvalidDrive;

 	/* make sure they don't exceed max blocks */

	if (!erc)
		if (dLBA > hdcb[hdrive].nBlocks)
			erc = ErcBadLBA;

	if (!erc)
	{
		switch(dOpNum)
		{
			case(CmdNull):		/* NULL command */
				erc = ok;
				break;

			case(CmdRead):		/* Read command */
				erc = hd_read(dLBA, dnBlocks, pData, hdrive);
				break;

			case(CmdWrite):		/* Write command */
				erc = hd_write(dLBA, dnBlocks, pData, hdrive);
				break;

			case(CmdVerify):	/* Verify command */
				erc = ErcNotSupported;

				/** hd_verify is not supported in this version
				 * of the driver.
				 * erc = hd_verify(dLBA, dnBlocks, pData,
				 *                 hdrive);
				 */
				break;

			case(CmdSeekTrk):	/* Seek track command */
				erc = hd_seek(dLBA, hdrive);
				break;

			case(CmdFmtTrk):	/* Format track command */
				erc = hd_format_track(dLBA, dnBlocks, hdrive);
				break;

			case(CmdResetHdw):	/* Reset controller */
				hd_reset((hdrive & 0x02) >> 1);
				erc = 0;
				break;

			case(CmdReadSect):	/* Read sector(s) command */
				erc = ReadSector(dLBA, dnBlocks, pData, hdrive);
				break;

			default:		/* Invalid command */
				erc = ErcBadOp;
				break;
		}
	}
	hdcb[hdrive].last_erc = erc;		/* Update DCB erc */

	return(erc);
}

/******************************************
Called for indepth status report on ctrlr
and drive specified. Returns 64 byte block
of data including current drive geometery.
This is called by the PUBLIC call DeviceStat!
*******************************************/

static U32 hddev_stat(U32  dDevice,
			   S8 * pStatRet,
			   U32  dStatusMax,
			   U32  *pdStatusRet)
{
U32 i;
U8 hdrive;

	hdrive = dDevice - 12;

	hdstatus[hdrive].erc = hdcb[hdrive].last_erc;
	hdstatus[hdrive].type_now = hd_type[hdrive];
	hdstatus[hdrive].nCyl = hd_cyls[hdrive];
	hdstatus[hdrive].nHead = hd_heads[hdrive];
	hdstatus[hdrive].nSectors = hd_secpertrk[hdrive];
	hdstatus[hdrive].nBPS = hdcb[hdrive].nBPB;

	/* Calculate size of status to return. Return no more than asked for! */
	if (dStatusMax <= sStatus)
		i = dStatusMax;
	else
		i = sStatus;

	CopyData(&hdstatus[hdrive], pStatRet, i);	/* copy to their status block */

	*pdStatusRet = i;				/* tell emhow much it was */

	return ok;
}

/******************************************
Called to reset the hard disk controller
and query drive parameters.  ATA specs are
used.  The Initdata block is ignored with this
ATA spec driver.
The DCB values are updated if this is successful.
This is called by the PUBLIC call DeviceInit.
*******************************************/

static U32  hddev_init(U32  dDevice,
			  S8  *pInitData,
			  U32   sdInitData)

{
U32 erc, i;
U8 hdrive;

	/* Set internal drive number */
 	hdrive = dDevice - 12;
	erc = 0;

	/* Read the init status block in */
	erc = hd_init(hdrive);
	if (!erc)
	{
		hd_cyls[hdrive] = IDEid[hdrive].ncyls;
		hd_heads[hdrive] = IDEid[hdrive].nheads;
		hd_secpertrk[hdrive] = IDEid[hdrive].nsectpertrk;

		xprintf("IDE Drive %d stats:\r\n", hdrive);
		xprintf("IDEid.ncyls:       %d\r\n", IDEid[hdrive].ncyls);
		xprintf("IDEid.nheads:      %d\r\n", IDEid[hdrive].nheads);
		xprintf("IDEid.nsectpertrk: %d\r\n", IDEid[hdrive].nsectpertrk);
	}
	else
	{
		hd_type[hdrive] = 0;
		xprintf("IDE Drive %d Invalid\r\n", hdrive);
		erc = ErcInvalidDrive;
	}

	/* If no error, update corresponding DCB values */
	if (!erc)
	{
		hdcb[hdrive].nBlocks =	IDEid[hdrive].ncyls *
					IDEid[hdrive].nheads *
					IDEid[hdrive].nsectpertrk;
	}

	hdcb[hdrive].last_erc = 0;	/* update DCB erc */

	return(erc);
}

/*===========  THE END  =========================================*/
