/*
 * MICRO-Touch
 *
 * This utility sets the directory timestamp for the specified
 * files to the current or specified time/date.
 *
 * Copyright 1990,1992 Dave Dunfield
 * All rights reserved.
 */

#include \mc\stdio.h
#include \mc\file.h

#define	MAXFILE	25		/* Maximum number of file specification arguments */

	int argcount = 0, attrib = 0;
	char *arglist[MAXFILE];

/*
 * Main program, set time/date of file(s)
 */
main(argc, argv)
	int argc;
	char *argv[];
{
	char fname[80], *ptr;
	int hour, min, sec, day, month, year;
	int i, j, k, time, date;
	FILE *fp;

	get_date(&day, &month, &year);
	get_time(&hour, &min, &sec);

/* Process arguments */
	for(i=1; i < argc; ++i) {
		ptr = argv[i];
		switch((toupper(*ptr++) << 8) | toupper(*ptr++)) {
			case 'T=' :			/* Specify time */
				sscanf(ptr, "%u:%u:%u", &hour, &min, &sec);
				break;
			case 'D=' :			/* Specify date */
				sscanf(ptr,"%u/%u/%u", &day, &month, &year);
				if(year < 1900)
					year += 1900;
				break;
			case '-S' :			/* Touch SYSTEM files */
				attrib |= SYSTEM;
				break;
			case '-H' :			/* Touch HIDDEN files */
				attrib |= HIDDEN;
				break;
			default:			/* File specification */
				arglist[argcount++] = argv[i]; } }

	if(!argcount) {
		lputs("\nUse: touch <filespec*> [t=h:m:s d=d/m/y -h -s]\n\nCopyright 1990,1992 Dave Dunfield\nAll rights reserved.\n", L_stderr);
		exit(-1); }

/* Pack time & date into DOS directory format */
	time = (hour << 11) | (min  << 5) | (sec / 2);
	date = ((year-1980) << 9) | (month << 5) | day;

/* Step through & set date of specified files */
	for(i = 0; i < argcount; ++i) {
		/* Include full directory prefix */
		ptr = arglist[i];
		j = k = 0;
		do {
			switch(fname[k++] = *ptr) {
				case '\\' :
				case ':'  :
					j = k; } }
		while(*ptr++);

		if(find_first(arglist[i], attrib, &fname[j], &k, &k, &k, &k, &k)) {
			lputs("Not found: ", L_stderr);
			lputs(arglist[i], L_stderr);
			lputc('\n', L_stderr); }
		else {
			do {
				if(fp = open(fname, F_READ)) {	/* Open for read & touch */
					touch(fp, time, date);
					close(fp); }
				else {							/* Could not open file */
					lputs("Unable to access: '", L_stderr);
					lputs(fname, L_stderr);
					lputs("'\n", L_stderr); } }
			while(!find_next(&fname[j], &k, &k, &k, &k, &k)); } }
}

/*
 * Touch the file with encoded date
 */
touch(fp, time, date) asm
{
		MOV		DX,4[BP]	; Get date
		MOV		CX,6[BP]	; Get time
		MOV		BX,8[BP]	; Get file handle
		MOV		AX,5701h	; Set date & time function
		INT		21h			; Ask DOS
}
