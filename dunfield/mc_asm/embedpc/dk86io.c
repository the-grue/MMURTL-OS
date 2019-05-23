/*
 * Low level disk I/O functions for MDCFS:
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
 * with Dunfield Development Systems ASM86 assembler. Use these if
 * you are compiling MDCFS for stand-along (NON-DOS) applications
 * with our DDS MICRO-C 8086 Developers Kit.
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
		PUSH	DS			Save DS
		POP		ES			Set ES
		MOV		BX,4[BP]	Get buffer
		MOV		AX,6[BP]	Get sector
		XOR		DX,DX		Zero high
		DIV		>sectrk		Calculate track
		MOV		CL,DL		CL = sector
		INC		CL			1-
		XOR		DX,DX		Zero high
		DIV		>numhead	Compute head
		MOV		CH,AL		CH = cylinder
		MOV		DH,DL		DH = head
		MOV		DL,8[BP]	DL = drive
		MOV		DI,3		Try three times
read1	MOV		AX,$0201	Read 1 sector
		INT		$13			Call BIOS
		JNC		read2		Success
		DEC		DI			Reduce count
		JNZ		read1		Keep trying
		PUSH	AX			Pass parameter
		CALL	disk_error	Report an error
		POP		AX			Clean stack
read2	XOR		AX,AX		Zero return
}

/*
 * Write a sector to the disk drive
 */
write_sector(drive, sector, buffer) asm
{
		PUSH	DS			Save DS
		POP		ES			Set ES
		MOV		BX,4[BP]	Get buffer
		MOV		AX,6[BP]	Get sector
		XOR		DX,DX		Zero high
		DIV		>sectrk		Calculate track
		MOV		CL,DL		CL = sector
		INC		CL			1-
		XOR		DX,DX		Zero high
		DIV		>numhead	Compute head
		MOV		CH,AL		CH = cylinder
		MOV		DH,DL		DH = head
		MOV		DL,8[BP]	DL = drive
		MOV		DI,3		Try three times
write1	MOV		AX,$0301	Write 1 sector
		INT		$13			Call BIOS
		JNC		write2		Success
		DEC		DI			Reduce count
		JNZ		write1		Keep trying
		PUSH	AX			Pass parameter
		CALL	disk_error	Report an error
		POP		AX			Clean stack
write2	XOR		AX,AX		Zero return
}
