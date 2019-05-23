/*
 * DDS MICRO-C Command Coordinator
 *
 * This program integrates the various commands required to compile a
 * program into a single command. It is currently set up for the PC/8086
 * compiler (using MASM and LINK):
 *
 * The following assemblers and linkers have been reported to work OK with
 * MICRO-C, using the indicated command line options:
 *
 * Vendor    Name  Versions(s)     Options (meaning)			  #define
 * ----------------------------------------------------------------------
 * Microsoft MASM  3.0				/ml		Case sensitive			MASM3
 * Microsoft MASM  4.0, 5.1			/t/ml	Terse, Case sensitive	MASM4
 * Borland   TASM  1.0, 2.01, 3.0	/t/ml	Terse, Case sensitive	TASM
 * Arrowsoft ASM   1.00d			/ml		Case sensitive			ARROW1
 * Arrowsoft ASM   2.00c			/s/ml	Silent, Case sensitive	ARROW2
 * Microsoft LINK  3.x, 5.x			/noi	Case sensitive			LINK
 * Borland   TLINK 2.0, 3.01		/c		Case sensitive			TLINK
 * freeware  VAL   ??				/nci 	Case sensitive *		VAL
 **VAL also requires '/COM' for TINY model. Original version did not
 * work correctly for TINY model. Fixed VAL is available from our BBS.
 *
 * To use a particular assembler or linker, #define the indicated symbol
 * (Make sure that only ONE assembler and ONE linker is defined)
 *
 * If your assembler and linker are not compatible with the command lines
 * hard coded in this program, do the following:
 * - Modify CC.C and LC.BAT (if necessary) to use acceptable commands
 * - Compile CC.C to CC.ASM using: CC CC -afop
 * - Assemble with your assembler
 * - Link to a new CC.COM with: LC CC
 *
 * If either your assembler OR linker does not support case sensitive
 * operation, run BOTH of them in case insensitive mode, and be sure
 * not to use any global symbols which differ only in case.
 *
 * Copyright 1990-1995 Dave Dunfield
 * All rights reserved.
 *
 * Compile command: cc cc -fop
 */
#include <stdio.h>

#define	MASM4			/* Use Microsoft MASM4+ assembler */
#define	LINK			/* Use Microsoft Linker */

#define	NOFILE	2		/* EXEC return code for file not found */
#define	NOPATH	3		/* EXEC return code for path not found */

static char mcdir[65], temp[65], ofile[65], tail[150], mcparm[80];
static char oasm = -1, opt = 0, pre = 0, tiny = -1, link = -1, verb = -1,
	del = -1, com = 0, fold = 0, *fnptr, *mptr = &mcparm;

/*
 * Main program, process options & invoke appropriate commands
 */
main(int argc, char *argv[])
{
	int i;
	char ifile[65], *ptr, c;

	/* Get default directories from environment */
	if(!getenv("MCDIR", mcdir))		/* Get MICRO-C directory */
		strcpy(mcdir,"\\MC");
	if(!getenv("MCTMP", temp))		/* Get temporary directory */
		strcpy(temp,"$");

/* parse for command line options. */
	for(i=2; i < argc; ++i) {
		if(*(ptr = argv[i]) == '-') {		/* Enable switch */
			while(*++ptr) {
				switch(toupper(*ptr)) {
					case 'A' : oasm = 0;	continue;
					case 'C' : com = -1;	continue;
					case 'F' : fold = -1;	continue;
					case 'K' : del = 0;		continue;
					case 'L' : link = 0;	continue;
					case 'O' : opt = -1;	continue;
					case 'P' : pre = -1;	continue;
					case 'Q' : verb = 0;	continue;
					case 'S' : tiny = 0;	continue; }
				goto badopt; }
			continue; }

		if(*(ptr+1) == '=') switch(toupper(*ptr)) {
			case 'H' : strcpy(mcdir, ptr+2);	continue;
			case 'T' : strcpy(temp, ptr+2);		continue; }

		*mptr++ = ' ';
		c = 0;
		while(*mptr++ = *ptr++) {
			if(*ptr == '=')
				c = pre; }
		if(c)
			continue;

	badopt:
		fprintf(stderr,"Invalid option: %s\n", argv[i]);
		exit(-1); }

	message("DDS MICRO-C PC86 Compiler v3.12\n");

	if(argc < 2)
		abort("\nUse: CC <name> [-acfklopqs h=homedir t=tmpdir] [symbol=value]\n\nCopyright 1990-1995 Dave Dunfield\nAll rights reserved.\n");

	/* Parse filename & extension from passed path etc. */
	fnptr = ptr = argv[1];
	while(c = *ptr) {
		if(c == '.')
			goto noext;
		++ptr;
		if((c == ':') || (c == '\\'))
			fnptr = ptr; }
	strcpy(ptr, ".C");
noext:
	strcpy(ifile, argv[1]);
	message(fnptr);
	message(": ");
	*mptr = *ptr = 0;

	/* Pre-process to source file */
	if(pre) {
		message("Preprocess... ");
		sprintf(ofile,"%s%s.CP", temp, fnptr);
		sprintf(tail,"%s %s l=%s -q -l%s", ifile, ofile, mcdir, mcparm);
		docmd("MCP");
		strcpy(ifile, ofile); }

	/* Compile to assembly language */
	message("Compile... ");
	sprintf(ofile,"%s%s.%s", (oasm||opt) ? temp : "", fnptr,
		opt ? "CO" : "ASM");
	sprintf(tail,"%s %s -q%s%s%s", ifile, ofile, pre ? " -l" : "",
		com ? " -c" : "", fold ? " -f" : "");
	docmd("MCC");
	if(pre)
		erase(ifile);
	strcpy(ifile, ofile);

	/* Optimize the assembly language */
	if(opt) {
		message("Optimize... ");
		sprintf(ofile,"%s%s.ASM", oasm ? temp : "", fnptr);
		sprintf(tail, "%s %s -q", ifile, ofile);
		docmd("MCO");
		erase(ifile);
		strcpy(ifile, ofile); }

	/* Assemble into object module */
	if(oasm) {
		sprintf(ofile,"%s%s.OBJ", link ? temp : "", fnptr);

/*
 * If necessary, Modify one of the following to suit your assembler
 */
#ifdef	MASM3
		message("Assemble...\n");
		sprintf(tail,"/ml %s,%s;", ifile, ofile);
		docmd("MASM");
#endif
#ifdef	MASM4
		message("Assemble... ");
		sprintf(tail,"/t/ml %s,%s;", ifile, ofile);
		docmd("MASM");
#endif
#ifdef	TASM
		message("Assemble... ");
		sprintf(tail,"/t/ml %s,%s;", ifile, ofile);
		docmd("TASM");
#endif
#ifdef	ARROW1
		sprintf(tail,"/ml %s,%s;", ifile, ofile);
		docmd("ASM");
#endif
#ifdef	ARROW2
		message("Assemble... ");
		sprintf(tail,"/s/ml %s,%s;", ifile, ofile);
		docmd("ASM");
#endif

		erase(ifile);
		strcpy(ifile, ofile);

	/* Link into executable program */
		if(link) {
			message(tiny ? "Link TINY...\n" : "Link SMALL...\n");

/*
 * If necessary, Modify one of the following to suit your linker
 */
#ifdef	LINK
			sprintf(ofile,"%s.EXE", fnptr);
			sprintf(tail,"/noi %s\\PC86RL_%c %s,%s,NUL,%s\\MCLIB;",
				mcdir, tiny ? 'T' : 'S', ifile, ofile, mcdir);
			docmd("LINK");
#endif
#ifdef	TLINK
			sprintf(ofile,"%s.EXE", fnptr);
			sprintf(tail,"/c %s\\PC86RL_%c %s,%s,NUL,%s\\MCLIB;",
				mcdir, tiny ? 'T' : 'S', ifile, ofile, mcdir);
			docmd("TLINK");
#endif
#ifdef	VAL
			sprintf(ofile,"%s.%s", fnptr, tiny ? "COM" : "EXE");
			sprintf(tail,"/NCI%s %s\\PC86RL_%c %s,%s,NUL,%s\\MCLIB;",
				tiny ? "/COM" : "",
				mcdir, tiny ? 'T' : 'S', ifile, ofile, mcdir);
			docmd("VAL");
#endif
			erase(ifile);
#ifndef	VAL
			if(tiny) {
				message("Convert... ");
				sprintf(tail,"%s %s.COM", ofile, fnptr);
				docmd("EXE2BIN");
				erase(ofile); }
#endif
			} }

	message("All done.\n");
}

/*
 * Execute a command, looking for it in the MICRO-C directory,
 * and also in any directories found in the PATH environment
 * variable. Operands to the command have been previously
 * defined in the global variable 'tail'.
 */
docmd(char *cmd)
{
	int rc;
	char command[65], path[300], *ptr, *ptr1, *delim, c;

	ptr = mcdir;						/* First try MC home dir */
	if(!getenv("PATH", ptr1 = path))	/* And then search  PATH */
		ptr1 = "";

	do {	/* Search MCDIR & PATH for commands */
		delim = "\\"+(ptr[strlen(ptr)-1] == '\\');
		sprintf(command,"%s%s%s.COM", ptr, delim, cmd);
		if((rc = exec(command, tail)) == NOFILE) {
			sprintf(command,"%s%s%s.EXE", ptr, delim, cmd);
			rc = exec(command, tail); }
		ptr = ptr1;						/* Point to next directory */
		while(c = *ptr1) {				/* Advance to end of entry */
			++ptr1;
			if(c == ';') {
				*(ptr1 - 1) = 0;		/* Zero terminate */
				break; } } }
	while(((rc == NOFILE) || (rc == NOPATH)) && *ptr);
	if(rc) {
		fprintf(stderr,"%s failed (%d)\n", cmd, rc);
		exit(-1); }
}

/*
 * Output an informational message (verbose mode only)
 */
message(char *ptr)
{
	if(verb)
		fputs(ptr, stderr);
}

/*
 * Erase temporary file (if enabled)
 */
erase(char *file)
{
	if(del)
		delete(file);
}
