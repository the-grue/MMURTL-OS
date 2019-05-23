/*
 * Parks the system hard drives.
 *
 * Copyright 1991 Dave Dunfield
 * All rights reserved.
 */
#include c:\mc\stdio.h

	int parms[12] = 2;		/* Default, 2 drives park via BIOS */

	extern char PSP[];		/* To determine address of COM start */

/*
 * Main program - parse arguments, park drives
 */
main(argc, argv)
	int argc;
	int *argv[];
{
	int drive, cyl, flag;
	FILE *fp;

	flag = cyl = 0;
	for(drive = 1; drive < argc; ++drive) switch(*argv[drive]) {
		case '?-' :		/* Help request */
		case '?'  :
			abort("\nUse: park [-s cyl1 cyl2 ...]\n\nCopyright 1991 Dave Dunfield\nAll rights reserved.\n");
		case 's-' :		/* Setup command */
		case 'S-' :
			flag = -1;
			break;
		default:
			if(!isdigit(*(char*)argv[drive]))
				abort("Invalid operand.");
			parms[parms[0] = ++cyl] = atoi(argv[drive]); }

	if(flag) {				/* Setup mode */
		putc('\n', stdout);
		for(drive=1; drive <= *parms; ++drive) {
			printf("Park drive %u ", drive);
			if(flag = parms[drive])
				printf("at cylinder %u\n", flag);
			else
				printf("via BIOS\n"); }
		if(cyl) {			/* Set options */
			fp = fopen("PARK.COM", "bwvq");
			fwrite(0x100, PSP-0x102, fp);
			fclose(fp);
			printf("\nPARK.COM written!\n"); }
		return; }

	printf("System shutdown in progress...\n\n");

	asm {
		MOV		AH,0Dh		; Disk reset
		INT		21h			; Ask DOS
	}

	for(drive = 1; drive <= *parms; ++drive) {
		if(!test_disk(drive)) {
			printf("Drive %u ", drive);
			if(!(cyl = parms[drive]))
				cyl = max_cyl(drive);
			flag = seek_disk(drive, cyl);
			printf("parked at cylinder %u", cyl);
			if(flag)
				printf(" - *flag %02x*", flag);
			putc('\n', stdout); } }

	printf("\nTurn system power OFF now!!! ");

	while(getc(stdin) != '\n');
}

/*
 * Recalibrate the fixed disk (also determine its existance)
 */
test_disk(drive) asm
{
		MOV		DL,4[BP]	; Get drive id
		ADD		DL,7fh		; Offset to drive
		MOV		AH,11h		; Recalibrate
		INT		13h			; Ask BIOS
		MOV		AL,AH		; Set low value
		XOR		AH,AH		; Zero high
}

/*
 * Get the maximum cylinder number for drive
 */
max_cyl(drive) asm
{
		MOV		DL,4[BP]	; Get drive id
		ADD		DL,7fh		; Convert ID
		MOV		AH,08h		; Get parms function
		INT		13h			; Ask BIOS
		MOV		AL,CH		; Get LOW byte of result
		MOV		AH,CL		; Get HIGH two bits
		MOV		CL,6		; Shift 6 bits
		SHR		AH,CL		; Do the shift
}

/*
 * Seek to specified cylinder on fixed disk
 */
seek_disk(drive, cyl) asm
{
		MOV		DL,6[BP]	; Get drive id
		ADD		DL,7fh		; Convert to BIOS id
		MOV		AL,5[BP]	; Get high cylinder
		MOV		CL,6		; Shift 6 bits
		SHL		AL,CL		; Perform shift
		INC		AL			; Sector 1
		MOV		CL,AL		; Set high bits
		MOV		CH,4[BP]	; Get LOW cylinder
		XOR		DH,DH		; Head zero
		MOV		AH,0Ch		; Function 13 SEEK
		INT		13h			; Ask BIOS
		MOV		AL,AH		; Put RC in lower byte
		XOR		AH,AH		; Zero upper byte
}
