/*
 * 8085 Cross Disassembler
 *
 * Copyright 1991-1993 Dave Dunfield
 * All rights reserved.
 */
#include c:\mc\stdio.h

#define	SYMBOLS	2500		/* Maximum number of symbols to allow */

#define	BIT8	0x01		/* Symbol is referenced as 8-bit */
#define	BIT16	0x02		/* Symbol is referenced as 16-bit */
#define	SYMDEF	0x80		/* Symbol is defined */
#define	SYMOLD	0x40		/* Symbol was loaded */

/*
 * Table of instruction opcodes: MASK, COMPARE, TYPE/LENGTH, TEXT
 */
char itable[] = {
	0xFF, 0xFE, 0x02, 'C', 'P', 'I', ' ', 0,
	0xFF, 0x3A, 0x03, 'L', 'D', 'A', ' ', 0,
	0xFF, 0x32, 0x03, 'S', 'T', 'A', ' ', 0,
	0xFF, 0x2A, 0x03, 'L', 'H', 'L', 'D', ' ', 0,
	0xFF, 0x22, 0x03, 'S', 'H', 'L', 'D', ' ', 0,
	0xFF, 0xF5, 0x01, 'P', 'U', 'S', 'H', ' ', 'P', 'S', 'W', 0,
	0xFF, 0xF1, 0x01, 'P', 'O', 'P', ' ', 'P', 'S', 'W', 0,
	0xFF, 0x27, 0x01, 'D', 'A', 'A', 0,
	0xFF, 0x76, 0x01, 'H', 'L', 'T', 0,
	0xFF, 0xFB, 0x01, 'E', 'I', 0,
	0xFF, 0xF3, 0x01, 'D', 'I', 0,
	0xFF, 0x37, 0x01, 'S', 'T', 'C', 0,
	0xFF, 0x3F, 0x01, 'C', 'M', 'C', 0,
	0xFF, 0x2F, 0x01, 'C', 'M', 'A', 0,
	0xFF, 0xEB, 0x01, 'X', 'C', 'H', 'G', 0,
	0xFF, 0xE3, 0x01, 'X', 'T', 'H', 'L', 0,
	0xFF, 0xF9, 0x01, 'S', 'P', 'H', 'L', 0,
	0xFF, 0xE9, 0x01, 'P', 'C', 'H', 'L', 0,
	0xFF, 0xDB, 0x02, 'I', 'N', ' ', 0,
	0xFF, 0xD3, 0x02, 'O', 'U', 'T', ' ', 0,
	0xFF, 0x07, 0x01, 'R', 'L', 'C', 0,
	0xFF, 0x0F, 0x01, 'R', 'R', 'C', 0,
	0xFF, 0x17, 0x01, 'R', 'A', 'L', 0,
	0xFF, 0x1F, 0x01, 'R', 'A', 'R', 0,
	0xFF, 0xC6, 0x02, 'A', 'D', 'I', ' ', 0,
	0xFF, 0xCE, 0x02, 'A', 'C', 'I', ' ', 0,
	0xFF, 0xD6, 0x02, 'S', 'U', 'I', ' ', 0,
	0xFF, 0xDE, 0x02, 'S', 'B', 'I', ' ', 0,
	0xFF, 0xE6, 0x02, 'A', 'N', 'I', ' ', 0,
	0xFF, 0xF6, 0x02, 'O', 'R', 'I', ' ', 0,
	0xFF, 0xEE, 0x02, 'X', 'R', 'I', ' ', 0,
	0xFF, 0x00, 0x01, 'N', 'O', 'P', 0,
	/*  8085 specific instructions */
	0xFF, 0x20, 0x01, 'R', 'I', 'M', 0,
	0xFF, 0x30, 0x01, 'S', 'I', 'M', 0,
	/*  Jumps, Calls & Returns */
	0xFF, 0xC3, 0x0B, 'J', 'M', 'P', ' ', 0,
	0xFF, 0xCA, 0x43, 'J', 'Z', ' ', 0,
	0xFF, 0xC2, 0x4B, 'J', 'N', 'Z', ' ', 0,
	0xFF, 0xDA, 0x13, 'J', 'C', ' ', 0,
	0xFF, 0xD2, 0x1B, 'J', 'N', 'C', ' ', 0,
	0xFF, 0xEA, 0x23, 'J', 'P', 'E', ' ', 0,
	0xFF, 0xE2, 0x2B, 'J', 'P', 'O', ' ', 0,
	0xFF, 0xFA, 0x83, 'J', 'M', ' ', 0,
	0xFF, 0xF2, 0x8B, 'J', 'P', ' ', 0,
	0xFF, 0xCD, 0x0B, 'C', 'A', 'L', 'L', ' ', 0,
	0xFF, 0xCC, 0x43, 'C', 'Z', ' ', 0,
	0xFF, 0xC4, 0x4B, 'C', 'N', 'Z', ' ', 0,
	0xFF, 0xDC, 0x13, 'C', 'C', ' ', 0,
	0xFF, 0xD4, 0x1B, 'C', 'N', 'C', ' ', 0,
	0xFF, 0xEC, 0x23, 'C', 'P', 'E', ' ', 0,
	0xFF, 0xE4, 0x2B, 'C', 'P', 'O', ' ', 0,
	0xFF, 0xFC, 0x83, 'C', 'M', ' ', 0,
	0xFF, 0xF4, 0x8B, 'C', 'P', ' ', 0,
	0xFF, 0xC9, 0x05, 'R', 'E', 'T', 0,
	0xFF, 0xC8, 0x45, 'R', 'Z', 0,
	0xFF, 0xC0, 0x4D, 'R', 'N', 'Z', 0,
	0xFF, 0xD8, 0x15, 'R', 'C', 0,
	0xFF, 0xD0, 0x1D, 'R', 'N', 'C', 0,
	0xFF, 0xE8, 0x25, 'R', 'P', 'E', 0,
	0xFF, 0xE0, 0x2D, 'R', 'P', 'O', 0,
	0xFF, 0xF8, 0x85, 'R', 'M', 0,
	0xFF, 0xF0, 0x8D, 'R', 'P', 0,
	/*  Register based instructions */
	0xC0, 0x40, 0x01, 'M', 'O', 'V', ' ', 'd', ',', 's', 0,
	0xC7, 0x06, 0x02, 'M', 'V', 'I', ' ', 'd', ',', 0,
	0xF8, 0x90, 0x01, 'S', 'U', 'B', ' ', 's', 0,
	0xF8, 0x98, 0x01, 'S', 'B', 'B', ' ', 's', 0,
	0xF8, 0x80, 0x01, 'A', 'D', 'D', ' ', 's', 0,
	0xF8, 0x88, 0x01, 'A', 'D', 'C', ' ', 's', 0,
	0xF8, 0xA0, 0x01, 'A', 'N', 'A', ' ', 's', 0,
	0xF8, 0xB0, 0x01, 'O', 'R', 'A', ' ', 's', 0,
	0xF8, 0xA8, 0x01, 'X', 'R', 'A', ' ', 's', 0,
	0xF8, 0xB8, 0x01, 'C', 'M', 'P', ' ', 's', 0,
	0xC7, 0x04, 0x01, 'I', 'N', 'R', ' ', 'd', 0,
	0xC7, 0x05, 0x01, 'D', 'C', 'R', ' ', 'd', 0,
	/*  Register pair instructions */
	0xCF, 0x01, 0x03, 'L', 'X', 'I', ' ', 'p', ',', 0,
	0xEF, 0x0A, 0x01, 'L', 'D', 'A', 'X', ' ', 'p', 0,
	0xEF, 0x02, 0x01, 'S', 'T', 'A', 'X', ' ', 'p', 0,
	0xCF, 0x03, 0x01, 'I', 'N', 'X', ' ', 'p', 0,
	0xCF, 0x0B, 0x01, 'D', 'C', 'X', ' ', 'p', 0,
	0xCF, 0x09, 0x01, 'D', 'A', 'D', ' ', 'p', 0,
	0xCF, 0xC5, 0x01, 'P', 'U', 'S', 'H', ' ', 'p', 0,
	0xCF, 0xC1, 0x01, 'P', 'O', 'P', ' ', 'p', 0,
	/*  Restart instruction */
	0xC7, 0xC7, 0x01, 'R', 'S', 'T', ' ', 'v', 0,
	/*  This entry always matches invalid opcodes */
	0x00, 0x00, 0x01, 'D', 'B', ' ', 0 }

/*
 * Tables to convert register index into actual names
 */
char regtab[]	= { 'B','C','D','E','H','L','M','A' };
char rptab[]	= { 'B','D','H','S' };

/*
 * Symbol output flags
 */
char *flags[] = { "?", "B", "W", "B&W" };

/*
 * Symbol table variables
 */
char sname[SYMBOLS][9], sflags[SYMBOLS];
unsigned svalue[SYMBOLS], scount = 0;

/*
 * Buffer for reading file, and recording opcode data
 */
unsigned char memory[64];

/*
 * Command option flags
 */
unsigned int begin = 0, size = -1, addr = 0;
char aflag = -1, bflag = 0, dflag = 0, eflag = -1;
FILE *dfp = 0;

/*
 * Perform a disassembly
 */
main(argc, argv)
	int argc;
	char *argv[];
{
	FILE *fp;
	unsigned a, i, j, length, s;
	unsigned char *ptr, opcode, c;

	if(argc < 2)
abort("\n8085 Cross Disassembler\n\n\
Use: dis85 <filename> [options ...]\n\n\
Options: -a     - Assembly output\n\
         -b     - include Byte constants (in -d)\n\
         -d     - pre-Define symbols for addresses\n\
         -e     - Exclude out of range addresses (in d=)\n\
         a=addr - specify memory Address\n\
         b=size - specify Beginning offset (in file)\n\
         d=file - Dump symbols to this file\n\
         n=size - specify Number of instructions to disassemble\n\
         s=file - specify a Symbol table file\n\n\
Copyright 1991-1993 Dave Dunfield\nAll rights reserved.\n");

/* parse for command line options. */
	for(i=2; i < argc; ++i) {
		if(*(ptr = argv[i]) == '-') {		/* Enable switch */
			while(*++ptr) switch(toupper(*ptr)) {
				case 'A' : aflag = -1;		break;
				case 'B' : bflag = -1;
				case 'D' : dflag = -1;		break;
				case 'E' : eflag = -1;		break;
				default: goto badopt; }
			continue; }
		if(*++ptr == '=') switch(toupper(*(ptr++ - 1))) {
			case 'A' : sscanf(ptr, "%x", &addr);	continue;
			case 'B' : sscanf(ptr, "%u", &begin);	continue;
			case 'N' : sscanf(ptr, "%u", &size);	continue;
			case 'S' :		/* Symbol table */
				fp = fopen(ptr, "rvq");
				while(fgets(memory, sizeof(memory), fp)) {
					sscanf(memory, "%s %x", sname[scount], &svalue[scount]);
					sflags[scount++] = SYMOLD; }
				fclose(fp);
				continue;
			case 'D' :
				dfp = fopen(ptr, "wvq");
				dflag = -1;
				continue; }
	badopt:
		fprintf(stderr,"Invalid option: %s\n", argv[i]);
		exit(-1); }


	fp = fopen(argv[1], "rvqb");

top:
	for(i=0; i < begin; ++i)
		getc(fp);
	a = addr;
	s = size;
	while(s-- && !((*memory = opcode = getc(fp)) & 0xff00)) {

	/* lookup opcode in instruction table */
		ptr = itable;
		while((*ptr++ & opcode) != *ptr++) {
			++ptr;
			while(*ptr++); }
		length = *ptr++ & 3;

	/* Read in all the operand bytes */
		for(i=1; i < length; ++i)
			memory[i] = getc(fp);

	/* If dumping symbols - Process */
		if(dflag & 0xF0) {
			a += length;
			if((length > 2) || ((length > 1) && bflag)) {
				j = memory[1];
				if(length > 2)
					j += (memory[2] << 8);
				for(i=0; i < scount; ++i)
					if(svalue[i] == j)
						goto skip1;
				sprintf(sname[scount],"H%04x", j);
				svalue[scount] = j;
				sflags[scount++] = (length == 3) ? BIT16 : BIT8; }
		skip1:
			continue; }

	/* If enabled, output disassembly format display options */
		if(aflag) {
			printf("%04x ", a);
			for(i=0; i < 3; ++i)
				printf(i < length ? "%02x " : "   ", memory[i]);
			putc(' ', stdout);
			for(i=0; i < 3; ++i) {
				c = (i < length) ? memory[i] : ' ';
				putc(((c >= ' ') && (c < 0x7f)) ? c : '.', stdout); }
			putc(' ', stdout); }

/* Display any symbol occuring at this address */
		for(i=0; i < scount; ++i)
			if(svalue[i] == a) {
				printf("%-9s", sname[i]);
				sflags[i] |= SYMDEF;
				goto skip2; }
		printf("         ");
skip2:	a += length;

	/* Display instruction with register substitutions etc. */
		i = 0;
		while(*ptr) switch(c = *ptr++) {
			case 'v' :		/* Interrupt vector */
				putc(((opcode >> 3) & 7) + '0', stdout);
				break;
			case 'p' :		/* Register PAIR */
				putc(rptab[(opcode >> 4) & 3], stdout);
				break;
			case 'd' :		/* Destination register */
				putc(regtab[(opcode >> 3) & 7], stdout);
				break;
			case 's' :		/* Source register */
				putc(regtab[opcode & 7], stdout);
				break;
			case ' ' :		/* Separator */
				while(i++ < 6)
					putc(' ', stdout);
				break;
			default:
				++i;
				putc(c, stdout); }

		switch(length) {
			case 2 :		/* Single byte operand */
				printv(memory[1], BIT8);
				break;
			case 3 :		/* Double byte operand */
				printv((memory[2] << 8) + memory[1], BIT16); }
		putc('\n', stdout); }

	if(dflag & 0xF0) {
		dflag = 0x0F;
		rewind(fp);
		goto top; }

	fclose(fp);

	if(dflag) {
		for(i=0; i < scount; ++i) {
			if((j = sflags[i] & SYMDEF) || eflag) {
				if(dfp)
					fprintf(dfp, "%s %04x %s\n", sname[i], svalue[i], flags[sflags[i] & 3]); }
			if(!j) {
				if(aflag)
					printf("%04x%15s", svalue[i], "");
				printf("%-9sEQU   $%04x %s\n", sname[i], svalue[i], flags[sflags[i]&3]); } }
		if(dfp)
			fclose(dfp); }
}

/*
 * Print a value using either a format, or a name from the symbol table
 */
printv(value, size)
	unsigned value;
	char size;
{
	int i;
	for(i=0; i < scount; ++i)
		if(value == svalue[i]) {
			fputs(sname[i], stdout);
			sflags[i] |= size;
			return; }

	printf(size == BIT16 ? "$%04x" : "$%02x" , value);
}
