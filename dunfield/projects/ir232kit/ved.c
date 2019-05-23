/*
 * Video Editor
 *
 * Copyright 1993-1995 Dave Dunfield
 * All rights reserved.
 *
 * Permission granted for personal (non-commercial) use only.
 *
 * Compile command: CC ved -fop
 */
#include <stdio.h>
#include <comm.h>
#include <window.h>

#define	NEDITS	19		/* Number of video edits */
#define	NCONFIG	6		/* Bytes of config storage to save */

/* DBOX output variables */
unsigned mode;
unsigned char status[3], counter[3] = 0;

/*
 * Linked list of video edits
 */
unsigned VEnum = 0, VEsel = 0;
unsigned char VEstart[NEDITS][3], VEend[NEDITS][3], VEname[NEDITS][41];

/*
 * Window variables
 */
char *mwin;
int attributes[] = {
	WSAVE|WBOX2|WCOPEN|REVERSE,			/* 0: Message window */
	WSAVE|WBOX1|WCOPEN|NORMAL,			/* 1: Main window */
	WSAVE|WBOX3|WCOPEN|REVERSE,			/* 2: Menus */
	WSAVE|WBOX3|WCOPEN|REVERSE,			/* 3: Entry forms */
	WSAVE|WBOX3|WCOPEN|REVERSE };		/* 4: Yes/No prompt */
/* Configuration variables */
unsigned comport = 0, vcr1 = 1, vcr2 = 2;

/* Names of screen menus */
char *menus[] = {
	"Message window",
	"Main screen",
	"Selection menus",
	"Entry forms",
	"Yes/No prompts" };
/* Form for editing clip name */
char *nform[] = {
	55<<8|3,
	"\x01\x00\x28Clip name:",
	0 };
char *fform[] = {
	65<<8|3,
	"\x01\x00\x32Filename:",
	0 };
char *vform[] = {
	25<<8|6,
	"\x01\x01\x81Playback  VCR #:",
	"\x01\x02\x81Recording VCR #:",
	0 };
/* Vcr selection menu */
char *vmenu[] = {
	"Playback  VCR",
	"Recording VCR",
	0 };
char *imenu[] = {
	"Insert New clip",
	"Paste last delete",
	0 };
char *pmenu[] = {
	"Selected clip",
	"All clips",
	0 };
char *cmenu[] = {
	"VCR assignments",
	"DBOX COM port",
	"Screen colors",
	"Save configuration",
	0 };
char *iomenu[] = {
	"Com 1",
	"Com 2",
	"Com 3",
	"Com 4",
	0 };
char *fmenu[] = {
	"Load",
	"Save",
	0 };

char filename[51] = { "VED.DAT" };

extern register message_box();

/*
 * Main program
 */
main(argc, argv)
	int argc;
	char *argv[];
{
	int c, i, j, k;
	FILE *fp;
	unsigned char VBstart[3], VBend[3], VBname[41], temp[50];

	printf("Video EDitor v1.0\nCopyright 1993-1995 Dave Dunfield\nAll rights reserved.\n\n");

	/* Load configuration if present */
	if(fp = fopen("CONFIG.VED", "rvb")) {
		fread(attributes, sizeof(attributes)+NCONFIG, fp);
		fclose(fp); }

	/* Enter configuration if desired */
	if((argc > 1) && (toupper(*argv[1]) == 'C')) {
		printf("Entering Configuration...\n");
		configuration(); }

	if(vcr1 || vcr2) {
		printf("Initializing DBOX");
		if(Copen(comport+1, _9600, PAR_NO|DATA_8|STOP_1, SET_RTS|SET_DTR|OUTPUT_2))
			abort("Cannot access COM port");
		i = 0;
		do {
			if(++i > 5)
				abort("Failed");
			putc('.', stdout);
			Cputc('\r');
			delay(2);
			while((j = Ctestc()) != -1)
				k = j; }
		while(k != '>');

		printf("\nResetting VCR's...\n");
		dbox(vcr2, "FR");
		dbox(vcr1, "RZ");
		dbox(vcr2, "RZ"); }

	memset(VBstart, 0, sizeof(VBstart));
	memset(VBend, 0, sizeof(VBend));
	memset(VBname, 0, sizeof(VBname));

reopen:
	mwin = wopen(0, 0, 80, 3, attributes[0]);
	wopen(0, 3, 80, 22, attributes[1]);
	wcursor_off();
redraw:
	title("Main Editor Panel");
	j = *W_OPEN;
	k = ((j << 4) & 0xF0) | ((j >> 4) & 0x0f);
	for(i=0; i < VEnum; ++i) {
		*W_OPEN = (i == VEsel) ? k : j;
		wgotoxy(0, i);
		wprintf("%u - %s -> ", i+1, show_counter(VEstart[i]));
		wprintf("%s - %s", show_counter(VEend[i]), VEname[i]);
		wcleol(); }
	wgotoxy(0, i);
	if(VEsel == VEnum) {
		*W_OPEN = k;
		wcleol();
		wgotoxy(0, i+1); }
	*W_OPEN = j;
	wcleow();
	wgotoxy(1, 19);
	wprintf("A)ssemble C)onfigure D)elete E)dit F)ile G)oto I)nsert P)lay Z)ero");
recmd:
	switch(c = toupper(wgetc())) {
		case _KDA :		/* Move down */
			if(++VEsel > VEnum)
				VEsel = 0;
			goto redraw;
		case _KUA :		/* Move up */
			if(--VEsel & 0xFF00)
				VEsel = VEnum ? VEnum - 1 : 0;
			goto redraw;
		case _KHO :		/* Home key */
			VEsel = 0;
			goto redraw;
		case _KEN :
			VEsel = VEnum;
			goto redraw;
		case 'D' :	/* Delete */
			if(VEsel < VEnum) {
				title("Delete video clip");
				sprintf(temp,"Delete clip #%u", VEsel+1);
				if(yesno(temp)) {
					memcpy(VBstart, VEstart[VEsel], sizeof(VBstart));
					memcpy(VBend, VEend[VEsel], sizeof(VBend));
					memcpy(VBname, VEname[VEsel], sizeof(VBname));
					for(i = VEsel; i < VEnum; ++i) {
						memcpy(VEstart[i], VEstart[i+1], sizeof(VEstart[0]));
						memcpy(VEend[i], VEend[i+1], sizeof(VEend[0]));
						memcpy(VEname[i], VEname[i+1], sizeof(VEname[0])); }
					if(VEsel >= --VEnum)
						VEsel = VEnum ? VEnum - 1 : 0; } }
			goto redraw;
		case 'I' :		/* Insert clip */
			j = 0;
			if(wmenu(10, 7, attributes[2], imenu, &j))
				goto recmd;
			if(!j) {
				memset(VBstart, 0, sizeof(VBstart));
				memset(VBend, 0, sizeof(VBend));
				memset(VBname, 0, sizeof(VBname)); }
			if(VEnum >= NEDITS) {
				wgotoxy(1, 18);
				wputs("\x007You cannot define any more clips!");
				goto recmd; }
			for(i = VEnum; i > VEsel; --i) {
				memcpy(VEstart[i], VEstart[i-1], sizeof(VEstart[0]));
				memcpy(VEend[i], VEend[i-1], sizeof(VEend[0]));
				memcpy(VEname[i], VEname[i-1], sizeof(VEname[0])); }
			memcpy(VEstart[VEsel], VBstart, sizeof(VBstart));
			memcpy(VEend[VEsel], VBend, sizeof(VBend));
			memcpy(VEname[VEsel], VBname, sizeof(VBname));
			++VEnum;
			if(j) goto redraw;
		case 'E' :
			if(VEsel < VEnum) {
				title("Edit video clip");
				wform(8, 10, attributes[3], nform, VEname[VEsel]);
				if(j)
					dbox_count(vcr1, "GO", VEstart[VEsel]);
				position_counter("Starting", VEstart[VEsel], vcr1);
				if(j)
					dbox_count(vcr1, "GO", VEend[VEsel]);
				position_counter("Ending", VEend[VEsel], vcr1); }
			goto redraw;
		case 'Z' :		/* Reset VCR */
			i = 0;
			title("Reset VCR counter");
			if(!wmenu(10, 7, attributes[2],vmenu, &i))
				dbox(i ? vcr2 : vcr1, "RZ");
			goto redraw;
		case 'G' :		/* Goto counter position */
			i = 0;
			title("Position VCR");
			if(!wmenu(10, 7, attributes[2], vmenu, &i))
				position_counter(vmenu[i], temp, i ? vcr2 : vcr1);
			goto redraw;
		case 'P' :		/* Play command */
			i = 0;
			if(wmenu(10, 7, attributes[2], pmenu, &i))
				goto recmd;
			j = i ? VEnum : VEsel+1;
			i = i ? 0 : VEsel;
			while(i < j) {
				sprintf(temp,"Playing clip %u", i+1);
				title(temp);
				dbox_count(vcr1, "GO", VEstart[i]);
				dbox(vcr1, "PL");
				dbox_count(vcr1, "GS", VEend[i]);
				++i; }
			goto redraw;
		case 'A' :
			if(yesno("Record all clips"))
				assemble();
			goto redraw;
		case 'C' :
			title("Configuration");
			configuration();
			wclose();
			wclose();
			goto reopen;
		case 'F' :		/* File commands */
			i = 0;
			if(!wmenu(10, 7, attributes[2], fmenu, &i)) {
				wform(5, 8, attributes[3], fform, filename);
				if(i) {
					if(fp = open_file(filename, "wb")) {
						fwrite(&VEnum, sizeof(VEnum), fp);
						for(i=0; i < VEnum; ++i) {
							fwrite(VEstart[i], sizeof(VEstart[0]), fp);
							fwrite(VEend[1], sizeof(VEend[0]), fp);
							fputs(VEname[i], fp);
							putc('\n', fp); }
						fclose(fp); } }
				else {
					if(fp = open_file(filename, "rb")) {
						fread(&VEnum, sizeof(VEnum), fp);
						for(i=0; i < VEnum; ++i) {
							fread(VEstart[i], sizeof(VEstart[0]), fp);
							fread(VEend[i], sizeof(VEend[0]), fp);
							fgets(VEname[i], sizeof(VEname[0]), fp); }
						fclose(fp); } } }
			goto redraw;
		case 0x1B:
			if(yesno("Exit Video EDitor"))
				break;
		default :
			goto recmd; }
			

	wclose();
	wclose();
}


/*
 * Display a title in the status window
 */
title(name)
	char *name;
{
	w_clwin(mwin);
	w_gotoxy(38 - strlen(name)/2, 0, mwin);
	w_puts(name, mwin);
}

/*
 * Issue a command to the DBOX, and parse the status
 */
dbox(vcr, command)
	char vcr, *command;
{
	int i, c, d;

	if(!vcr)
		return;

	Cputc(vcr + ('0'-1));
	while(*command)
		Cputc(*command++);
	Cputc(0x0D);

	while((c = Cgetc()) != '=') {
		if(c == '?') {
			printf("DBOX: error ?%c", Cgetc());
			exit(-1); } }

	mode = Cgeth();
	if(Cgetc() != ' ')
		abort("DBOX: no space after mode");

	for(i=0; i < 3; ++i) {
		if((c = Cgeth()) < 0)
			abort("DBOX: Invalid hex digit in status");
		status[i] = c; }

	if(Cgetc() != ' ')
		abort("DBOX: no space after status");

	d = Cgetc();
	if((d != '+') && (d != '-'))
		abort("DBOX: no '+' or '-' on counter");

	for(i=0; i < 3; ++i) {
		if((c = Cgeth()) < 0)
			abort("DBOX: Invalid digit in counter");
		counter[i] = c; }
	if(d == '-')
		counter[0] = counter[1] = counter[2] = 0;

	if(Cgetc() != 0x0A)
		abort("DBOX: No Line-feed");
	if(Cgetc() != 0x0D)
		abort("DBOX: No Carriage-return");

#ifdef DEBUG
	printf("DBOX: mode=%02x, status=%02x%02x%02x counter=%02x%02x%02x\n",
		mode, status[0], status[1], status[2], counter[0], counter[1], counter[2]);
#endif
}

/*
 * Get a HEX value from the serial port... -1 = invalid
 */
Cgeth()
{
	int a, b;
	a = toupper(Cgetc());
	if(isdigit(a))
		a -= '0';
	else if(isalpha(a))
		a -= ('A' - 10);
	else
		return -1;

	b = toupper(Cgetc());
	if(isdigit(b))
		b -= '0';
	else if(isalpha(b))
		b -= ('A' - 10);
	else
		return -1;

	return (a<<4) | b;
}

/*
 * Prompt for YES/NO response
 */
yesno(ptr)
	char *ptr;
{
	int i;

	i = strlen(ptr)+6;
	wopen((80-i)/2, 10, i, 3, attributes[4]);
	wprintf("%s ? Y", ptr);
	for(;;) switch(toupper(wgetc())) {
		case 'N' :
		case 0x1B :
			wclose();
			return 0;
		case '\n' :
		case 'Y' :
			wclose();
			return 1; }
}

/*
 * Construct a displayable counter value
 */
show_counter(ptr)
	unsigned char ptr[];
{
	static char ostring[25];
	sprintf(ostring,"%02x:%02x:%02x", ptr[0], ptr[1], ptr[2]);
	return ostring;
}

/*
 * Execute a command with a counter operand
 */
dbox_count(vcr, cmd, count)
	unsigned char vcr, *cmd, count[];
{
	char command[50];
	sprintf(command,"%s%02x%02x%02x", cmd, count[0], count[1], count[2]);
	dbox(vcr, command);
}

/*
 * Generate a short delay
 */
delay(c)
	int c;
{
	int j, s, s1;
	s1 = -1;
	do {
		do
			get_time(&j, &j, &s);
		while(s == s1);
		s1 = s; }
	while(c--);
}

/*
 * Tweak a counter value
 */
tweak_counter(name, ctr)
	unsigned char *name, ctr[3];
{
	int i, c;
	static char *tnames[] = { "HOUR", "MINITE", "SECOND" };

	i = 0;
	wopen(20, 8, 30, 8, attributes[3]);
	wcursor_off();
	wprintf("%s counter:", name);
	wgotoxy(0, 4);
	wputs("Use ARROW keys to set.\nPress ESCAPE when finished");
	for(;;) {
		wgotoxy(0, 2);
		wprintf("Setting %-6s : %s", tnames[i], show_counter(ctr));
		switch(wgetc()) {
	case _KRA : i = (i > 1) ? 0 : i+1;	break;
	case _KLA : i = (i) ? i-1 : 2;		break;
	case _KUA :
		if(((c = ctr[i] + 1) & 0x0f) > 9)
			c += 6;
		if(c > 0x59)
			c = 0;
		ctr[i] = c;
		break;
	case _KDA :
		if(c = ctr[i]) {
			if((--c & 0x0f) > 9)
		c = (c & 0xf0) | 9; }
		else
			c = 0x59;
		ctr[i] = c;
		break;
	case 0x1B :
		wclose();
		return; } }
}

/*
 * Position the VCR counter
 */
position_counter(name, ctr, vcr)
	unsigned char *name, ctr[3];
	int vcr;
{
	char play;

	play = 0;
	wopen(15, 7, 40, 10, attributes[3]);
	wcursor_off();
	wprintf("\nSet %s counter:\n\n", name);
	wprintf("Use controls on VCR or:\n\n");
	wprintf("Play = Up/Enter     Stop = Down/Space\n");
	wprintf("FFwd = Right        Rew  = Left\n");
	wprintf("Man  = '.'          Exit = Esc");
	ctr[2] = -1;
	for(;;) {
		if((counter[2] != ctr[2]) || (counter[1] != ctr[1]) || (counter[0] != ctr[0])) {
			memcpy(ctr, counter, sizeof(ctr));
			wgotoxy(30, 1);
			printf("%s", show_counter(counter)); }
		switch(wtstc()) {
			case 0 :
				dbox(vcr, "QU");
				break;
			case _KRA :
				dbox(vcr, play ? "CF" : "FF");
				break;
			case _KLA :
				dbox(vcr, play ? "CR" : "FR");
				break;
			case _KDA :
			case ' ' :
				dbox(vcr, "ST");
				play = 0;
				break;
			case _KUA :
			case '\n' :
				play = -1;
				dbox(vcr, "PL");
				break;
			case '.' :
				dbox(vcr, "ST");
				tweak_counter(name, counter);
				dbox_count(vcr, "GO", counter);
				ctr[2] = -1;
				break;
			case 0x1B :		/* Exit */
				wclose();
				return; } }
}

/*
 * Assemble edit
 */
assemble()
{
	unsigned i;
	char temp[50];

	title("Assemble Recorder");
	/* Place VCR2 into recording pause mode */
	dbox(vcr2, "RE");
	dbox(vcr2, "PU");

	/* Play & record the various tape clips */
	for(i=0; i < VEnum; ++i) {
		dbox_count(vcr1, "GO", VEstart[i]);
		dbox(vcr1, "PL");
		sprintf(temp, "Recording clip %u", i+1);
		title(temp);
		dbox(vcr1, "QU");	/* Short pause */
		dbox(vcr1, "QU");
		dbox(vcr1, "QU");
		dbox(vcr2, "PU");	/* Release pause */
		dbox_count(vcr1, "GT", VEend[i]);
		dbox(vcr2, "PU");
		dbox(vcr1, "QU");	/* Short pause */
		dbox(vcr1, "QU");
		dbox(vcr1, "QU");
		dbox(vcr1, "ST");
		dbox(vcr1, "QU"); }

	dbox(vcr2, "ST");
}

/*
 * Configure the video editor options
 */
configuration()
{
	int m, c, i, f, b;
	FILE *fp;

	m = 0;
	while(!wmenu(10, 7, attributes[2], cmenu, &m)) {
		switch(m) {
			case 0 :		/* VCR id's */
				wform(11, 8, attributes[3], vform, &vcr1, &vcr2);
				break;
			case 1 :		/* Comm port */
				wmenu(11, 8, attributes[2], iomenu, &comport);
				break;
			case 2 :		/* Set screen colors */
				i = 0;
				do {
					f = attributes[i] & 0x0F;
					b = (attributes[i] >> 4) & 0x0F;
					wopen(11, 8, 40, 9, WSAVE|WCOPEN|WBOX2|(b<<4)|f);
					wcursor_off();
					*W_OPEN = (f << 4) + b;
					wprintf("%-38s\n\n", menus[i]);
					*W_OPEN = (b << 4) + f;
					wputs("Left/Right = Foreground\nUp/Down    = Background\n");
					wputs("PgUp/PgDn  = Select window\n\nPress ESCAPE to exit.");
					c = wgetc();
					wclose();
					switch(c) {
						case _KRA : f = (f+1) & 0x0F; goto newattr;
						case _KLA : f = (f-1) & 0x0F; goto newattr;
						case _KUA : b = (b+1) & 0x0F; goto newattr;
						case _KDA : b = (b-1) & 0x0F;
						newattr:
							attributes[i] = (attributes[i] & 0xff00) | (b << 4) | f;
							break;
						case _KPD : if(++i >= (sizeof(menus)/2)) i = 0;	break;
						case _KPU : if(--i < 0) i = (sizeof(menus)/2)-1; } }
				while(c != 0x1B);
				break;
			case 3 :		/* Save configuration */
				if(fp = fopen("CONFIG.VED", "wb")) {
					fwrite(attributes, sizeof(attributes)+NCONFIG, fp);
					fclose(fp); }
				else
					message_box("Unable to open config file!"); } }
}

/*
 * Open a file with options
 */
open_file(filename, options)
	char *filename, *options;
{
	int n, p, c, i;
	char *names[100], namebuf[1000], *nptr, *ptr, *ptr1;
	FILE *fp;

	for(ptr = filename; i = *ptr; ++ptr)
		if((i == '*') || (i == '?'))
			break;
	if(i) {
		if(find_first(filename, n = 0, nptr = namebuf, &i, &i, &i, &i, &i)) {
			message_box("No files found matching: '%s'", filename);
			return 0; }
		wopen(13, 7, 53, 12, attributes[3]);
		wcursor_off();
		do {
			for(i=0; i < n; ++i)
				if(strcmp(names[i], nptr) == 1)
					break;
			for(c = ++n; c > i; --c)
				names[c] = names[c-1];
			names[i] = nptr;
			while(*nptr++); }
		while((n < (sizeof(names)/2)) && !find_next(nptr, &i, &i, &i, &i, &i));
		for(i=n; i < (sizeof(names)/2); ++i)
			names[i] = "";

		c = p = 0;
		for(;;) {
			for(i=0; i < 40; ++i) {
				wgotoxy((i%4)*13, i/4);
				*W_OPEN = (i == c) ? ((*W_OPEN >> 4)|(*W_OPEN << 4)) : attributes[3];
				wputf(names[i+p], 12); }
			switch(wgetc()) {
				case 0x1B :
					wclose();
					return 0;
				case '\n' :
					ptr = ptr1 = filename;
					while(i = *ptr++) {
						if((i == '\\') || (i == ':'))
							ptr1 = ptr; }
					strcpy(ptr1, names[p+c]);
					wclose();
					goto open_file;
				case _KRA : i = c + 1; goto position;
				case _KLA : i = c - 1; goto position;
				case _KUA : i = c - 4; goto position;
				case _KDA : i = c + 4;
				position:
					if(i < 0) {
						c = 0;
						if((p - 4) >= 0) {
							c = i + 4;
							p -= 4; }
						break; }
					if(i >= 40) {
						if((p + 40) < n) {
							i -= 4;
							p += 4; } }
					c = ((p + i) < n) ? i : (n - p) - 1; } } }

open_file:
	if(!options)		/* Solve wildcards only ... don't open */
		return -1;
	if(!(fp = fopen(filename, options)))
		message_box("Unable to access: '%s'", filename);
	return fp;
}

/*
 * Display a message in a box
 */
register message_box(args)
	unsigned args;
{
	unsigned *ptr, l;
	char buffer[80];

	ptr = nargs() * 2 + &args;
	_format_(buffer, ptr);

	l = strlen(buffer)+16;
	wopen((80-l)/2, 10, l+2, 3, attributes[4]);
	wcursor_off();
	wprintf(" %s (Press ENTER) ", buffer);
	while(wgetc() != '\n');
	wclose();
}
