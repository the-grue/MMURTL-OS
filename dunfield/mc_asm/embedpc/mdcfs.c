/*
 * MDCFS: Minimal Dos Compatible File System
 *
 *   These routines provide the bare minimum needed to read and write
 * files on an MS-DOS format floppy disk. You could use them with a hard
 * disk as well, however since only 12 bit FAT's are supported, you are
 * limited to a total of 4096 clusters, and total drive space is limited
 * to 32MB due to 16 bit sector numbers (assuming 512 byte sectors).
 *
 *   The functions were written for use in embedded systems (where memory
 * is often limited), and therefore provide only the basic open, read/write,
 * close and delete operations. I have documented the functions which
 * manipulate the directory and FAT, and it should be fairly easy to add
 * other features if you need them (directory display etc.). Only access to
 * files in the ROOT directory is provided, subdirectories are NOT supported.
 *
 *   For simplicity and memory conservation, these functions buffer only 1
 * sector (512 bytes) in memory. This makes them run quite slowly, but is
 * adaquate for reading/writing setup information and occational data logging.
 * If you have lots of memory and need extra speed, you could modify the
 * functions to read/write multiple sectors (a cluster would be easy).
 * You can also experiment with different interleave factors, to obtain
 * optimim performance with the existing I/O functions.
 *
 *   As they stand, the functions really support access to only one drive
 * at a time. You can use multiple drives if you call "open_drive()" between
 * disk operations to the separate drives. This "switches" the active drive
 * to the specified one. Note however, that the selected drive will seek to
 * track zero each time this function is called, so performing many small
 * operations on more than one drive gets VERY inefficent. DO NOT read or
 * write to an open file located on any drive other than the currently
 * selected one! Call "open_drive()" first!
 *
 *   Concurrent access to multiple files (on the same drive) is supported,
 * however since only one "work" buffer is used for directory/FAT access,
 * the drive may have to perform extra read/write operation when switching
 * from one file to the other. For this reason, it is best to try and do
 * as many reads or writes as possible on one file before accessing others.
 * Avoid many small operations to multiple files.
 *
 *   At present, only the first copy of the disk File Allocation Table
 * (FAT) is used by these functions.
 *
 *   Functions read/write data in RAW (binary) form, without regard for
 * NEWLINE characters etc. If you want to read/write ASCII text, you will
 * have to write "wrapper" functions to drop RETURN (0x0D) characters on
 * reading, and to add them before NEWLINE (0x0A) when writing.
 *
 *   Due to the use of 'C' structures, Version 3.0 (or later) of MICRO-C
 * is REQUIRED! It should not be difficult to compile with a different
 * compiler, but I have not attempted to do so.
 *
 *   You can compile this DEMO program to run under DOS with the MICRO-C
 * DOS compiler (available from our BBS - 613-256-5820 - Dial '2' at voice
 * prompt). To use the functions without DOS, replace the I/O routines (at
 * the end of this file) with the from the file DK86IO.C, and compile it
 * with our stand-alone 8086 Developers Kit.
 *
 * Copyright 1993-1995 Dave Dunfield
 * All rights reserved.
 *
 * Permission granted for personal (non-commercial) use only.
 */


/* Required definitions from MICRO-C stdio.h (not part of MDCFS) */
extern register printf();
#define	FILE	unsigned


/* Misc fixed parameters */
#define	SECTOR_SIZE	512		/* Size of a disk sector */
#define	BPB_SIZE	17		/* Number of bytes in BIOS Parm Block */
#define	EMPTY		0xE5	/* Signals empty directory */
#define	EOF			-1		/* End of file indicator */
#define	ERROR		-2		/* Report error in file */
#define	READ		0		/* File opened for READ */
#define	WRITE		1		/* File opened for WRITE */


/*
 * Structure of MSDOS directory entry
 */
struct Dentry {
	unsigned char	Dname[11];		/* Filename + extension */
	unsigned char	Dattr;			/* File attributes */
	unsigned char	Dreserved[10];	/* Reserved area */
	unsigned		Dtime;			/* Time last modified */
	unsigned		Ddata;			/* Date last modified */
	unsigned		Dcluster;		/* First cluster number */
	unsigned		Dsizel;			/* File size (LOW) */
	unsigned		Dsizeh; } ;		/* File size (HIGH) */

/*
 * Structure of internal file control block
 */
struct Fblock {
	unsigned char 	Fattr;			/* Open attributes */
	unsigned char 	Fsector;		/* Sector within cluster */
	struct Dentry	*Fdirptr;		/* Pointer to directory entry */
	unsigned		Fdirsec;		/* Directory sector */
	unsigned		Ffirstcls;		/* First cluster in file */
	unsigned		Flastcls;		/* Last cluster read/written */
	unsigned		Fnextcls;		/* Next cluster to read/write */
	unsigned		Foffset;		/* Read/Write offset */
	unsigned		Fsizel;			/* File size (LOW) */
	unsigned		Fsizeh;			/* File size (HIGH) */
	unsigned char	Fbuffer[]; } ;	/* Data transfer buffer */


/* Internal "work" sector variables */
unsigned	wrkdrv = 0,				/* Current work drive number */
	 		wrksec = -1;			/* Current work sector number */
char		wrkchg = 0;				/* Indicates work sector changed */
unsigned char wrkbuff[SECTOR_SIZE];	/* Work sector buffer */

/* Active drive information (other than contained in BPB) */
char		active_drive = -1;		/* Open disk drive number */
unsigned	dirsec = 5,				/* First sector of directory */
			datasec = 12;			/* First sector of data area */

/* Disk information (from BIOS Parameter Block) */
unsigned int	bytsec	= 512;		/* Bytes / sector */
unsigned char	seccls	= 2;		/* Sectors / cluster */
unsigned int	ressec	= 1;		/* # reserved sectors */
unsigned char	numfat	= 2;		/* Number of FAT's */
unsigned int	dirent	= 112;		/* Number of directory entries */
unsigned int	sectors	= 720;		/* Sectors on disk */
unsigned char	mediaid	= 0xFD;		/* Media ID byte */
unsigned int	secfat	= 2;		/* Sectors / fat */
unsigned int	sectrk	= 9;		/* Sectors / track */
unsigned int	numhead	= 2;		/* Number of heads */


/*
 * Function Prototypes
 */
extern struct Dentry *lookup(), *create_file();


/*
 * File accessing functions:
 *
 *	open_drive(drive)			- Initialize a drive for file access
 *		drive	- Drive id (0=A, 1=B ...)
 *
 *	open_file(name, attrs)		- Open file for read or write
 *		name	- Name of file to open
 *		attrs	- Open attributes: READ or WRITE
 *		returns : Pointer to file structure, or 0 if failure
 *
 *	close_file(fp)				- Close an open file
 *		fp		- Pointer to open file structure
 *
 *	read_byte(fp)				- Read a byte from an open file
 *		fp		- Pointer to open file structure
 *		returns : Value read (0-255), -1 if EOF, -2 if not open for read
 *
 *	write_byte(byte, fp)		- Write a byte to a file
 *		byte	- Value to write to file (0-255)
 *		returns : Value written (0-255) or -2 if error
 *
 *	delete_file(name)			- Erases the named file
 *		name	- Name of file to erase
 *		returns : 0 if Success, -1 if failure (file not found)
 */

/*
 * Open a disk drive and set up control information
 *
 * THIS FUNCTION MUST BE CALLED BEFORE ACCESSING ANY FILES,
 * AND ANYTIME YOU SWITCH TO ACCESS A DIFFERENT DRIVE.
 */
open_drive(drive)
	char drive;
{
	read_work(active_drive = drive, 0);
	memcpy(&bytsec, wrkbuff+11, BPB_SIZE);
	dirsec	= (numfat * secfat) + ressec;
	datasec	= ((((dirent * sizeof(struct Dentry)) + bytsec) - 1) / bytsec) + dirsec;
}

/*
 * Open a file & return a pointer to an allocated file structure
 */
struct Fblock *open_file(name, attrs)
	char *name;
	int attrs;
{
	struct Dentry *dirptr;
	struct Fblock *fp;

	if(dirptr = lookup(name)) {					/* File already exists */
		if(attrs == WRITE)						/* Zero size on write */
			dirptr->Dsizel = dirptr->Dsizeh = 0; }
	else {
		if(attrs != WRITE)						/* Not writing file */
			return 0;
		if(!(dirptr = create_file(name, 0)))	/* Unable to create */
			return 0; }

	/* Allocate buffer for file control block */
	if(!(fp = malloc(bytsec+sizeof(struct Fblock))))
		return 0;

	/* Fill in file control block from directory information */
	fp->Fdirsec		= wrksec;
	fp->Fdirptr		= dirptr;
	fp->Fattr		= attrs;
	fp->Fnextcls	= fp->Ffirstcls = dirptr->Dcluster;
	fp->Fsizel		= dirptr->Dsizel;
	fp->Fsizeh		= dirptr->Dsizeh;
	fp->Fsector		= fp->Foffset = fp->Flastcls = 0;

	return fp;

}

/*
 * Close an open file
 */
close_file(fp)
	struct Fblock *fp;
{
	struct Dentry *dirptr;
	unsigned sizeh, sizel;

	/* Special actions to be taken when writing */
	if(fp->Fattr == WRITE) {
		sizel = fp->Fsizel;
		sizeh = fp->Fsizeh;
		/* If there is a partial last sector, fill it with DOS EOF */
		/* characters, which also causes it to be written out */
		while(fp->Foffset)
			write_byte(0x1A, fp);
		/* Release remaining clusters in file. If any sectors in the */
		/* current cluster are used, begin with the next one. */
		release(fp->Fsector ? get_fat(fp->Fnextcls) : fp->Fnextcls);
		/* Update size entry in file directory */
		read_work(active_drive, fp->Fdirsec);
		dirptr = fp->Fdirptr;
		dirptr->Dcluster= fp->Ffirstcls;
		dirptr->Dsizel	= sizel;
		dirptr->Dsizeh	= sizeh;
		/* Set directory date of modification here - if you wish */
		wrkchg = -1; }

	free(fp);

	update_work();		/* Insure disk is in sync */
}

/*
 * Delete a file from the disk
 */
delete_file(name)
	char *name;
{
	struct Dentry *dirptr;

	if(dirptr = lookup(name)) {
		*dirptr->Dname = EMPTY;
		wrkchg = -1;
		release(dirptr->Dcluster);
		update_work();
		return 0; }
	return -1;
}

/*
 * Read next byte from open file
 */
int read_byte(fp)
	struct Fblock *fp;
{
	unsigned cluster, sector;

	/* If all data read, return EOF */
	if(!(fp->Fsizel || fp->Fsizeh)) {
		return EOF; }

	/* Decrement file size (32 bit) */
	if((--fp->Fsizel) == -1)
		--fp->Fsizeh;

	/* If no data buffered ... read next sector */
	if((fp->Foffset >= bytsec) || !fp->Foffset) {
		/* If not open for READ, return ERROR */
		if(fp->Fattr != READ)
			return ERROR;

		/* If past last cluster, return EOF */
		if(((cluster = fp->Fnextcls) >= 0xFF8) || !cluster)
			return EOF;

		/* Read the sector into memory */
		read_sector(active_drive,
			(sector = fp->Fsector) + ((cluster-2)*seccls) + datasec,
			fp->Fbuffer);

		/* Advance sector in cluster, if past end, get next cluster number */
		if(++sector >= seccls) {
			fp->Fnextcls = get_fat(cluster);
			sector = 0; }

		/* Update file control block information */
		fp->Flastcls	= cluster;
		fp->Fsector		= sector;
		fp->Foffset		= 0; }

	/* Read character, and advance circular buffer pointer */
	return fp->Fbuffer[fp->Foffset++];
}

/*
 * Write a byte to an open file
 */
int write_byte(c, fp)
	unsigned c;
	struct Fblock *fp;
{
	unsigned cluster, sector;

	/* Test for writable file */
	if(fp->Fattr != WRITE)
		return ERROR;

	/* Advance file size */
	if(!++fp->Fsizel)
		++fp->Fsizeh;

	/* Write character to buffer */
	fp->Fbuffer[fp->Foffset++] = c;

	/* If buffer is full, write it */
	if(fp->Foffset >= bytsec) {
		/* If no sector allocated, allocate one */
		if(!fp->Fnextcls)
			fp->Ffirstcls = fp->Fnextcls = allocate(0);
		else if(fp->Fnextcls >= 0xFF8)
			fp->Fnextcls = allocate(fp->Flastcls);

		if(!(cluster = fp->Fnextcls))
			return ERROR;

		/* Write the data to the drive */	
		write_sector(active_drive,
			(sector = fp->Fsector) + ((cluster-2)*seccls) + datasec,
			fp->Fbuffer);

		/* Advance to next sector in cluster */
		if(++sector >= seccls) {
			fp->Fnextcls = get_fat(cluster);
			sector = 0; }

		/* Update file control block information */
		fp->Flastcls	= cluster;
		fp->Fsector		= sector;
		fp->Foffset		= 0; }

	return c;
}


/*
 * FAT manipulation functions:
 *
 *	allocate(cluster)			- Allocate a free cluster
 *		cluster	- Cluster to link this one to (o if none).
 *		returns : Cluster number allocated, 0 if failure
 *
 *	release(cluster)			- Release a cluster chain (if allocated)
 *		cluster	- Beginnig cluster to release
 *
 *	get_fat(cluster)			- Get FAT entry for cluster
 *		cluster	- Cluster number to obtain entry for
 *		returns : Cluster number linked (0 if free, 0xFF8+ for EOF)
 *
 *	set_fat(cluster, link)		- Set the FAT entry for a cluster
 *		cluster	- Cluster number to set link for
 *		link	- Cluster number to link to (0=free, 0xFFF = EOF)
 */

/*
 * Allocate a free cluster on disk, and cross connect FAT if necessary
 * mark cluster as used & end of file.
 */
int allocate(cluster)
	int cluster;
{
	int begin, end, i;

	/* Calculate start and end clusters for search */
    /* If we have a "FAT" sector loaded, begin searching from there, */
    /* To attempt to keep allocated sectors in same "FAT" sector */

	begin = ((i = wrksec-1) && (i <= secfat)) ?
		((i * (SECTOR_SIZE*2)) / 3) + 1 : 2;
	end = (sectors - datasec) / seccls;

	do {
#ifdef	DEBUG
		printf("Allocate(%u) : Wrk=%u Begin=%u End=%u\n", cluster, wrksec, begin, end);
#endif
		for(i = begin; i < end; ++i)
			if(!get_fat(i)) {
#ifdef	DEBUG
			printf("Allocated %u\n", i);
#endif
				set_fat(i, 0xFFF);
				if(cluster)
					set_fat(cluster, i);
				return i; }
		/* Didn't find, reset to start cluster, and try again */
		end = begin;
		begin = 2; }
	while(end != 2);

#ifdef	DEBUG
	printf("Failed!\n");
#endif
	return 0;
}

/*
 * Release a chain of allocated clusters
 */
release(cluster)
	int cluster;
{
	unsigned i;
	while(cluster && (cluster < 0xFF8)) {
		i = get_fat(cluster);
		set_fat(cluster, 0);
		cluster = i; }
}

/*
 * Get a FAT entry for a given cluster number
 */
int get_fat(cluster)
	unsigned cluster;
{
	unsigned sec, fat;

#ifdef	DEBUG
	printf("Get FAT for %u - ", cluster);
#endif

	sec = (cluster *= 3)/2;
	read_work(active_drive, (sec / bytsec) + 1);
	fat = wrkbuff[sec % bytsec];
	read_work(active_drive, (++sec / bytsec) + 1);
	fat |= wrkbuff[sec % bytsec] << 8;
	if(cluster & 1)
		fat >>= 4;

#ifdef	DEBUG
	printf("%u\n", fat & 0xFFF);
#endif

	return fat & 0xfff;
}

/*
 * Set a FAT entry for a given cluster number
 */
set_fat(cluster, value)
	unsigned cluster, value;
{
	unsigned sec, x;

#ifdef	DEBUG
	printf("Set FAT for %u - %u\n", cluster, value);
#endif

	sec = (cluster *= 3)/2;

	/* Set LOW byte of FAT */
	read_work(active_drive, (sec / bytsec) + 1);
	x = sec % bytsec;
	if(cluster & 1)
		wrkbuff[x] = (wrkbuff[x] & 0x0F) | (value << 4);
	else
		wrkbuff[x] = value;
	wrkchg = -1;

	/* Set high byte of FAT */
	read_work(active_drive, (++sec / bytsec) + 1);
	x = sec % bytsec;
	if(cluster & 1)
		wrkbuff[x] = value >> 4;
	else
		wrkbuff[x] = (wrkbuff[x] & 0xF0) | ((value >> 8) & 0x0F);
	wrkchg = -1;
}


/*
 * Directory manipulation functions:
 *
 *	lookup(file)				- Locate a files directory entry
 *		file	- Name of file to locate
 *		returns : Pointer to directory entry, 0 if failure.
 *
 *	create_file(file,attrs) 	- Create a mew file
 *		file	- Name of file to create
 *		attrs	- Attributes for file (0 = normal)
 *		returns : Pointer to directory entry, 0 if failure.
 *
 *	parse_filename(buffer,name)	- Get filename in directory format
 *		buffer	- Buffer for directory format filename (13 bytes)
 *		name	- ASCII filename to convert
 *
 *	memcmp(ptr1, ptr2, size)	- Block memory compare
 *		ptr1	- Pointer to first block
 *		ptr2	- Pointer to second block
 *		size	- Number of bytes to compare
 *		returns	: -1 of match, 0 if different.
 */

/*
 * Lookup a directory entry & return pointer to it
 */
struct Dentry *lookup(file)
	char *file;
{
	unsigned i, j, k;
	char fbuffer[12];

	parse_filename(fbuffer, file);

#ifdef	DEBUG
	printf("Lookup: '%s' ", fbuffer);
#endif

	j = bytsec;
	k = dirsec;
	for(i=0; i < dirent; ++i) {
		if(j >= bytsec) {
			read_work(active_drive, k++);
			j = 0; }
		if(memcmp(wrkbuff+j, fbuffer, 11)) {
#ifdef	DEBUG
	printf("Found - Cluster: %u\n", *(int*)(wrkbuff+j+0x1A));
#endif
			return wrkbuff+j; }
		j += sizeof(struct Dentry); }

#ifdef	DEBUG
	printf("Not found\n");
#endif
	return 0;
}

/*
 * Create a file with the specified name
 * NOTE: Does NOT check for duplicates. It is assumed that "lookup()"
 * has been called first, to verify that the file does not already
 * exist on the disk.
 */
struct Dentry *create_file(file, attrs)
	char *file;
	int attrs;
{
	int i, j, k;
	char fbuffer[12];
	struct Dentry *dirptr;

	parse_filename(fbuffer, file);

#ifdef	DEBUG
	printf("Create '%s' ", fbuffer);
#endif

	j = bytsec;
	k = dirsec;
	for(i=0; i < dirent; ++i) {
		if(j >= bytsec) {
			read_work(active_drive, k++);
			j = 0; }
		if((*(dirptr = wrkbuff+j)->Dname == EMPTY) || !*dirptr->Dname) {
#ifdef	DEBUG
	printf("Dir entry #%u\n", i);
#endif
			memset(dirptr, 0, sizeof(struct Dentry));
			strcpy(dirptr, fbuffer);
			dirptr->Dattr = attrs;
			/* Set directory date of creation here - if you wish */
			wrkchg = -1;
			return wrkbuff+j; }
	j += sizeof(struct Dentry); }

#ifdef	DEBUG
	printf("No directory!\n");
#endif

	return 0;
}

/*
 * Parse a filename into directory format (12 characters, space filled)
 */
parse_filename(buffer, name)
	char buffer[], *name;
{
	int i;

	i = 0;
	while(i < 8)
		buffer[i++] = (*name && (*name != '.')) ? toupper(*name++) : ' ';
	if(*name == '.')
		++name;
	while(i < 11)
		buffer[i++] = *name ? toupper(*name++) : ' ';
	buffer[i] = 0;
}

/*
 * Memory to memory compare
 */
int memcmp(ptr1, ptr2, size)
	char *ptr1, *ptr2;
	unsigned size;
{
	while(size--)
		if(*ptr1++ != *ptr2++)
			return 0;
	return -1;
}


/*
 * Internal "work" sector manipulation functions:
 *
 *	read_work(sector)		- Read sector into work buffer (if necessary)
 *		sector	- Sector number to read.
 *
 *	update_work()			- Write work sector back to disk if changed
 */

/*
 * Read a work sector into memory if it is not already in memory.
 */
read_work(drive, sector)
	char drive;
	int sector;
{
	if((wrkdrv != drive) || (wrksec != sector)) {
		update_work();
		read_sector(wrkdrv = drive, wrksec = sector, wrkbuff); }
}

/*
 * Write the work sector if it has been marked as modified
 */
update_work()
{
	if(wrkchg) {
		write_sector(wrkdrv, wrksec, wrkbuff);
		wrkchg = 0; }
}


/*
 * Low level disk I/O functions:
 *
 *	disk_error(code)		- Called if unrecoverable disk error
 *		code	- Disk failure code (system depandant)
 *
 *	read_sector(d, s, b)	- Read a sector from the disk
 *		d		- Drive to read (0=A, 1=B ...)
 *		s		- Sector number to read (1-n)
 *		b		- Pointer to buffer to receive data
 *
 *	write_sector(d, s, b)	- Write a sector to the disk
 *		d		- Drive to write (0=A, 1=B ...)
 *		s		- Sector number to write (1-n)
 *		b		- Pointer to buffer containing the data
 *
 * These functions make use of the IBM PC BIOS, and are compatible
 * with Microsoft MASM assembler. Use these if you are compiling the
 * demo program with the MICRO-C DOS compiler.
 */

/*
 * Report a disk error
 */
disk_error(code)
	int code;
{
	printf("Disk error - Code: %04x\n", code);
	exit(-1);
}

/*
 * Read a sector from the disk drive
 */
read_sector(drive, sector, buffer) asm
{
		PUSH	DS			; Save DS
		POP		ES			; Set ES
		MOV		BX,4[BP]	; Get buffer
		MOV		AX,6[BP]	; Get sector
		XOR		DX,DX		; Zero high
		DIV		WORD PTR _sectrk; Calculate track
		MOV		CL,DL		; CL = sector
		INC		CL			; 1-
		XOR		DX,DX		; Zero high
		DIV		WORD PTR _numhead; Compute head
		MOV		CH,AL		; CH = cylinder
		MOV		DH,DL		; DH = head
		MOV		DL,8[BP]	; DL = drive
		MOV		DI,3		; Try three times
read1:	MOV		AX,0201h	; Read 1 sector
		INT		13h			; Call BIOS
		JNC		read2		; Success
		DEC		DI			; Reduce count
		JNZ		read1		; Keep trying
		PUSH	AX			; Pass parameter
		CALL	_disk_error	; Report an error
		POP		AX			; Clean stack
read2:	XOR		AX,AX		; Zero return
}

/*
 * Write a sector to the disk drive
 */
write_sector(drive, sector, buffer) asm
{
		PUSH	DS			; Save DS
		POP		ES			; Set ES
		MOV		BX,4[BP]	; Get buffer
		MOV		AX,6[BP]	; Get sector
		XOR		DX,DX		; Zero high
		DIV		WORD PTR _sectrk; Calculate track
		MOV		CL,DL		; CL = sector
		INC		CL			; 1-
		XOR		DX,DX		; Zero high
		DIV		WORD PTR _numhead; Compute head
		MOV		CH,AL		; CH = cylinder
		MOV		DH,DL		; DH = head
		MOV		DL,8[BP]	; DL = drive
		MOV		DI,3		; Try three times
write1:	MOV		AX,0301h	; Write 1 sector
		INT		13h			; Call BIOS
		JNC		write2		; Success
		DEC		DI			; Reduce count
		JNZ		write1		; Keep trying
		PUSH	AX			; Pass parameter
		CALL	_disk_error	; Report an error
		POP		AX			; Clean stack
write2:	XOR		AX,AX		; Zero return
}


/*
 * Sample Main program (Not part of MDCFS) to demonstrate reading,
 * writing and deleteing files. This also uses the standard file
 * I/O of the MICRO-C DOS compiler, and will not run stand-alone.
 *
 * Demo command syntax:
 *   MCDFS W <file>		- Copy file from current DOS dir to floppy
 *   MCDFS R <file>     - Copy file from floppy to current DOS dir
 *   MCDFS D <file>     - Delete file from floppy.
 *
 * All accesss to the floppy are performed via MCDFS.
 */
main(argc, argv)
	int argc;
	char *argv[];
{
	int c;
	struct Fblock *fp;
	FILE *xfp;

	open_drive(0);		/* Change to (1) for drive B: */
	switch((argc > 2) ? toupper(*argv[1]) : 0) {

		case 'R' :		/* Read file from floppy drive */
			if(!(fp = open_file(argv[2], READ)))
				abort("Couldn't read MDCFS file");
			xfp = fopen(argv[2], "wbvq");
			while((c = read_byte(fp)) >= 0)
				putc(c, xfp);
			close_file(fp);
			fclose(xfp);
			c = 0;		/* Disk has not been modified */
			break;

		case 'W' :		/* Write file to floppy drive */
			xfp = fopen(argv[2], "rvbq");
			if(!(fp = open_file(argv[2], WRITE)))
				abort("Couldn't write MDCFS file");
			while((c = getc(xfp)) >= 0)
				write_byte(c, fp);
			fclose(xfp);
			close_file(fp);
			c = -1;		/* Note that disk may have been written */
			break;

		case 'D' :		/* Delete file from floppy drive */
			delete_file(argv[2]);
			c = -1;		/* Note that disk may have been written */
			break;

		default:
			abort("Use: MDCFS (Read/Write/Delete) <filename>"); }

	if(c) {
		printf("\nNote: Since MDCFS bypasses DOS completely, DOS will be unaware\n");
		printf("of any changes to the disk... Due to DOS buffering, files written\n");
		printf("may not appear in DOS 'dir' command, or may have their sizes\n");
		printf("reported incorrectly... Press CONTROL-C to force DOS to flush\n");
		printf("its buffers before accessing the floppy disk"); }
}
