/*
 * CHAINSAW.... Chop down an entire directory tree.
 *
 * This program will remove an entire directory tree or portion thereof
 * (outward from any branch), including all sub-directories...
 * USE WITH EXTREME CAUTION!!! Careless use of this program could result
 * in the loss of LARGE amounts of data.
 *
 * To minimize the chances of disaster, the program does error checking
 * on every critical operation, and aborts if a failure occurs. If this
 * happens fix the problem, and run CHAINSAW again.
 *
 * Copyright 1992 Dave Dunfield
 * All rights reserved.
 *
 * Permission granted to use for personal (non-commercial) use only.
 */
#include <stdio.h>
#include <file.h>

#define	MAXDIRS	500		/* Maximum number of directories to stack */

/* Stack of directory names... + top of stack marker */
char names[MAXDIRS][13];
unsigned ndir = 0, fcount = 0, dcount = 0, rc;

/* Command line switches */
char message = -1, yesno = -1, system = 0;

/* File control block for delete file function */
char fcb[37] = { "\000???????????" };

/* Register holders for int86() function */
extern int _AX_, _DX_;

/*
 * Main program... parse arguments, ask permission, and KILL! KILL! KILL!
 */
main(argc, argv)
	int argc;
	char *argv[];
{
	int i;
	char buffer[25], directory[68], old_dir[68], *ptr, flag;

/* Record our current position */
	old_dir[0] = get_drive() + 'A';
	old_dir[1] = ':';
	old_dir[2] = '\\';
	getdir(&old_dir[3]);

/* If argument is passed, parse directory */
	for(flag=i=1; i < argc; ++i) {
		ptr = argv[i];
		switch((toupper(*ptr) << 8) | toupper(*(ptr+1))) {
			case '/Q' :
			case '-Q' :
				message = 0;
				break;
			case '/Y' :
			case '-Y' :
				yesno = 0;
				break;
			case '/S' :
			case '-S' :
				system = -1;
				break;
			default:
				if(flag) {
					if(*((ptr = argv[1])+1) == ':') {
						set_drive(toupper(*ptr) - 'A');
						ptr += 2; }
					if(cd(ptr))
						abort("Unable to access directory");
					break; }
			case '/?' :
			case '?' << 8 :
				abort("Use: CHAINSAW [directory] [/Quiet /System /Yes]"); } }

	if(message)
		printf("CHAINSAW by Dave Dunfield\n");

/* Prompt with selected directory, and make sure he really wants to do it */
	directory[0] = get_drive() + 'A';
	directory[1] = ':';
	directory[2] = '\\';
	getdir(&directory[3]);
	if(yesno) {
		printf("Delete tree below %s (ARE YOU SURE)?", directory);
		fgets(buffer, sizeof(buffer)-1, stdin); }

/* If he really wants to.... kill off everything */
	if((!yesno) || (toupper(*buffer) == 'Y')) {
		kill_tree();						/* Kill files & subdirs */
		if(!cd(".."))						/* Up to parent */
			dcount += !rmdir(directory); }	/* Kill base directory */

/* Try and restore our former directory position */
	set_drive(old_dir[0] - 'A');
	cd(old_dir);

	if(message)
		printf("\nRemoved %u files and %u directories.\n", fcount, dcount);
}

/*
 * Kill all files in current directory, and then recursively call
 * self to exterminate any sub-directories.
 */
kill_tree()
{
	int j, attr, n;
	char name[65];

/* If no files or sub-directories... do nothing */
	if(find_first("*.*", HIDDEN|SYSTEM|DIRECTORY, name, &j, &j, &attr, &j, &j))
		return;

/* Search and record sub-directories, insure all files delete-able */
	n = ndir;
	do {
		if(attr & DIRECTORY) {
			if(strcmp(name,".") && strcmp(name,".."))
				strcpy(names[ndir++], name); }
		else {
			if((attr & (SYSTEM|READONLY)) && system) {
				if(rc = set_attr(name, 0)) {
					printf("Can't set attributes of file: %s (%d)\n", name, rc);
					exit(-1); } }
			++fcount; } }
	while(!find_next(name, &j, &j, &attr, &j, &j));

/* Using FCB's, delete all files in this directory */
	_AX_ = 0x1300;
	_DX_ = fcb;
	int86(0x21);

/* Kill off sub-directories */
	while(ndir > n) {
		if(rc = cd(names[ndir-1])) {
			printf("Can't enter subdirectory: %s (%d)\n", names[ndir-1], rc);
			exit(-1); }
		kill_tree();
		if(rc = cd("..")) {
			printf("Can't access next higher directory. (%d)\n", rc);
			exit(-1); }
		if(rc = rmdir(names[--ndir])) {
			printf("Can't remove subdirectory: %s (%d)\n", names[ndir], rc);
			exit(-1); }
		++dcount; }
}
