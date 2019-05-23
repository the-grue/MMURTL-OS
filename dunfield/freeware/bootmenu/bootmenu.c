/*
 * BOOTMENU: a DOS Startup menu that automatically reloads with
 * different AUTOEXEC.BAT and CONFIG.SYS if necessary.
 *
 * Copyright 1992 Dave Dunfield
 * All rights reserved.
 *
 * Permission granted to use for personal (non-commercial) use only.
 */
#include <stdio.h>		/* Standard I/O definitions */
#include <window.h>		/* Windowing library definitions */

#define	MAXCFG	10		/* Maximum number of menu entries supported */

char description[MAXCFG][71], autoexec[MAXCFG][51], config[MAXCFG][51],
	old_autoexec[51], old_config[51], *parse_ptr, reboot = 0, *boot_dir;

unsigned cfg_count = 0, max_length = 0, select = 0, timer = 10;

/*
 * Parse a string delimited by '|' from the input line
 */
parse(dest)
	char *dest;
{
	while(isspace(*parse_ptr))
		++parse_ptr;
	while(*parse_ptr) {
		if((*dest = *parse_ptr++) == '|')
			break;
		++dest; }
	*dest = 0;
}

/*
 * Main program - check if in "reboot phase", and if so, terminate with
 * proper error code. Otherwise, give menu of available options.
 */
main(argc, argv)
	int argc;
	char *argv[];
{
	unsigned i, j, s, s1;
	char buffer[150], buffer1[50];
	FILE *fp;

	if(argc > 1)		/* Directory is specified */
		boot_dir = argv[1];
	else {				/* Use directory from program name */
		boot_dir = argv[i=j=0];
		while(s = boot_dir[i]) {
			if(s == '\\')
				j = i;
			++i; }
		boot_dir[j] = 0; }

	/* If re-booting to establish configuration */
	sprintf(buffer, "%s\\BOOTMENU.CTL", boot_dir);
	if(fp = fopen(buffer, "rv")) {
		fgets(buffer, sizeof(buffer)-1, fp);
		fgets(old_autoexec, sizeof(old_autoexec)-1, fp);
		fgets(old_config, sizeof(old_config)-1, fp);
		fclose(fp);
		select = atoi(buffer+1);
		if(*buffer == '*')
			goto xboot; }

	/* Read in configuration directory file */
	sprintf(buffer,"%s\\BOOTMENU.CFG", boot_dir);
	fp = fopen(buffer, "rvq");
	parse_ptr = fgets(buffer, sizeof(buffer)-1, fp);
	parse(buffer1);
	if(*buffer1)
		timer = atoi(buffer1);
	parse(buffer1);
	if(*buffer1)
		select = atoi(buffer1);
	while(parse_ptr = fgets(buffer, sizeof(buffer)-1, fp)) {
		parse(description[cfg_count]);
		parse(autoexec[cfg_count]);
		parse(config[cfg_count]);
		max_length = max(max_length, strlen(description[cfg_count]));
		++cfg_count; }
	fclose(fp);

	/* Open window & indicate available options */
	wopen(40-(max_length+8)/2, 12-(cfg_count+3)/2, max_length+8, cfg_count+3,
		WSAVE|WBOX1|WCOPEN|0x70);
	wcursor_off();
	for(i=0; i < cfg_count; ++i) {
		wprintf("%c%u - %s\n", (i == select) ? '*' : ' ', i, description[i]); }

	wprintf("Select: %u", timer);

	/* Wait for user to choose selection, or timeout */
	get_time(&j, &j, &s);
	do {
		if(i = wtstc()) {
			if((i == '\n') || (i == ' ') || (i == 0x1B))
				break;
			if((i - '0') < cfg_count) {
				select = i - '0';
				break; } }
		get_time(&j, &j, &s1);
		if(s != s1) {
			s = s1;
			wprintf("\rSelect: %u ", --timer); } }
	while(timer && (i != ' ') && (i != '\n'));
	wclose();

	/* If new config uses a different AUTOEXEC, copy it over */
	parse_ptr = autoexec[select];
	if(*parse_ptr && strcmp(parse_ptr, old_autoexec)) {
		strcpy(old_autoexec, parse_ptr);
		copy_files("\\AUTOEXEC.BAT", parse_ptr); }

	/* If new config uses a different CONFIG, copy it over */
	parse_ptr = config[select];
	if(*parse_ptr && strcmp(parse_ptr, old_config)) {
		strcpy(old_config, parse_ptr);
		copy_files("\\CONFIG.SYS", parse_ptr); }

xboot:

	/* Rewrite control file to reflect current state */
	sprintf(buffer, "%s\\BOOTMENU.CTL", boot_dir);
	fp = fopen(buffer, "wvq");
	fprintf(fp,"%c%u\n%s\n%s", reboot ? '*' : '-', select, old_autoexec, old_config);
	fclose(fp);

	/* If files have changed - REBOOT! */
	if(reboot) asm {
		MOV		AH,0Dh					; Disk RESET function
		INT		21h						; Ask DOS
		MOV		AX,40h					; BIOS data area
		MOV		ES,AX					; Set ES
		MOV		WORD PTR ES:72h,1234h	; Indicate warm start
		DB		0EAh,0,0,0FFh,0FFh		; JMP FAR FFFF:0
	}

	/* Pass back selection number as errorlevel */
	exit(select);
}

/*
 * Copy one or more files from our directory to the destination
 */
copy_files(dest, source)
	char *dest, *source;
{
	char buffer[150], name[15], *ptr, data_flag;
	FILE *fpd, *fps;

	fpd = fopen(dest, "wvq");

	/* Copy files(s) into output file */
	while(*source) {
		data_flag = 0;
		/* Concatinate directory and source file */
		dest = buffer;
		ptr = boot_dir;
		while(*dest = *ptr++)
			++dest;
		*dest++ = '\\';
		if(*source == '!') {			/* From general file */
			strcpy(dest, "BOOTMENU.DAT");
			dest = name;
			data_flag = -1; }
		while(*source) {				/* Copy in file name */
			if((*dest = *source++) == ',')
				break;
			++dest; }
		*dest = 0;
		/* Copy data into new file */
		fps = fopen(buffer, "rvq");
		if(data_flag) {					/* Copy from general file */
			while(fgets(buffer, sizeof(buffer)-1, fps)) {
				if(*buffer == '!') {
					if(!data_flag)
						break;
					if(!strcmp(buffer, name))
						data_flag = 0; }
				else if(!data_flag) {
					fputs(buffer, fpd);
					putc('\n', fpd); } } }
		else {							/* Copy from separate file */
			while(fgets(buffer, sizeof(buffer)-1, fps)) {
				fputs(buffer, fpd);
				putc('\n', fpd); } }
		fclose(fps); }

	fclose(fpd);
	reboot = -1;
}
