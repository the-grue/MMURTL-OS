/*
 * Video Answering Machine
 *
 * Copyright 1993-1995 Dave Dunfield
 * All rights reserved.
 *
 * Permission granted for personal (non-commercial) use only.
 *
 * Compile command: CC vam -fop
 */
#include <stdio.h>
#include <comm.h>
#include <window.h>

#define	NMSGS	25			/* Max. number of messages supported */
#define	NCONFIG	4			/* Number of config varibles to save */

/* DBOX output variables */
unsigned mode;
unsigned char status[3], counter[3], outlen[3] = 0;

/* Saved memory */
int	nmsgs = 0,		/* Number of messages recorded */
	cardet = 0,		/* Type of carrier detect selected */
	dual = 1,		/* Dual VCR mode */
	comport = 0;	/* Comm port selected */

unsigned char mcounter[NMSGS][3];
int mday[NMSGS], mmonth[NMSGS], mhour[NMSGS], mmin[NMSGS];

/* Window variables & attributes */
char *mwin;
int attributes[] = {
	WSAVE|WBOX2|WCOPEN|REVERSE,			/* 0: Message window */
	WSAVE|WBOX1|WCOPEN|NORMAL,			/* 1: Main window */
	WSAVE|WBOX3|WCOPEN|REVERSE,			/* 2: Menus */
	WSAVE|WBOX3|WCOPEN|REVERSE };		/* 3: yes/no prompts */
char *menus[] = {
	"Message window",
	"Main TTY screen",
	"Selection menu",
	"Yes/No prompts" };

/* Command menus */
int menu1 = 0;
char *Menu1[] = {
	"Go into AUTO-ANSWER mode",
	"Playback your messages",
	"Reset message counter",
	"Record a new outgoing message",
	"Play outgoing message",
	"Configuration menu",
	"Exit Video Answering Machine",
	0 };
char *Menu2[] = {
	"Number of VCRS",
	"Carrier detection",
	"Screen Colors",
	"Outgoing message length",
	"Communication port",
	0 };
char *Menu3[] = {
	"DSR",
	"CTS",
	"DSR or CTS",
	"DSR and CTS",
	0 };
char *Menu4[] = {
	"ONE VCR   (Prompt is caller mirror)",
	"TWO VCR's (Prompt is record source)",
	"TWO VCR's (Prompt is outgoing tape)",
	0 };
char *Menu5[] = {
	"Com 1",
	"Com 2",
	"Com 3",
	"Com 4",
	0 };

/* Array of months */
char *months[] = {
	"---", "Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
/* Array of time names */
char *tnames[] = { "HOUR", "MINITE", "SECOND" };

char vamfile[] = { "CONFIG.VAM" };
int vcr1 = 0, vcr2 = 1;

/*
 * Main program
 */
main(argc, argv)
	int argc;
	char *argv[];
{
	int i, j, k;
	FILE *fp;

	printf("Video Answering Machine v1.0\nCopyright 1993-1995 Dave Dunfield\nAll rights reserved.\n\n");

	mcounter[0][0] = mcounter[0][1] = mcounter[0][2] = 0;
	if(fp = fopen(vamfile, "rbv")) {
		fread(attributes, sizeof(attributes), fp);
		fread(&nmsgs, sizeof(int) * NCONFIG, fp);
		fread(outlen, sizeof(outlen), fp);
		fread(mcounter, (nmsgs+1) * 3, fp);
		fread(mmonth, nmsgs*sizeof(int), fp);
		fread(mday, nmsgs*sizeof(int), fp);
		fread(mhour, nmsgs*sizeof(int), fp);
		fread(mmin, nmsgs*sizeof(int), fp);
		fclose(fp); }

	if((argc > 1) && (toupper(*argv[1]) == 'C')) {
		printf("Entering Configuration...\n");
		configuration(); }

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
	vcr2 = dual ? 1 : 0;
	if(dual)
		dbox(vcr2, "FR");
	dbox(vcr1,"RZ");
	if(dual)
		dbox(vcr2, "RZ");

redraw:
	mwin = wopen(0, 0, 80, 3, attributes[0]);
	wopen(0, 3, 80, 22, attributes[1]);
	wcursor_off();
	for(;;) {
		title("Main Control Panel");
		wclwin();
		wgotoxy(1, 19);
		wprintf("You have %u message%s available for viewing.", nmsgs, &"s"[nmsgs == 1]);
		while(wmenu(10, 7, attributes[2], Menu1, &menu1));
		switch(menu1) {
			case 0 :		/* AUTO-ANSWER mode */
				auto_answer();
				break;
			case 1 :		/* Playback messages */
				play_messages();
				break;
			case 2 :		/* Reset message counter */
				title(Menu1[2]);
				if(yesno("Reset message counter"))
					nmsgs = 0;
				break;
			case 3 :		/* Record a new message */
				record_outgoing();
				break;
			case 4 :		/* Play outgoing message */
				wclwin();
				dbox(vcr1, "RZ");
				play_outgoing();
				if(dual >= 2) {
					wprintf("\nPrompt message is playing.. Press SPACE to stop:");
					while(wgetc() != ' ');
					dbox(vcr1, "ST"); }
				break;
			case 5 :		/* Configuration */
				title(Menu1[5]);
				wclwin();
				configuration();
				vcr2 = dual ? 1 : 0;
				standby();
				if(dual)
					dbox(vcr2, "FR");
				dbox(vcr1,"RZ");
				if(dual)
					dbox(vcr2, "RZ");
				wclose();
				wclose();
				goto redraw;
			case 6 :		/* Exit VAM */
				wclose();
				wclose();
				Cclose();
				write_data();
				printf("Ending...\n");
				return; } }
}

/*
 * Write the VAM data to the configuration file
 */
write_data()
{
	FILE *fp;

	fp = fopen(vamfile, "wbvq");
	fwrite(attributes, sizeof(attributes), fp);
	fwrite(&nmsgs, sizeof(int) * NCONFIG, fp);
	fwrite(outlen, sizeof(outlen), fp);
	fwrite(mcounter, (nmsgs+1)*3, fp);
	fwrite(mmonth, nmsgs*sizeof(int), fp);
	fwrite(mday, nmsgs*sizeof(int), fp);
	fwrite(mhour, nmsgs*sizeof(int), fp);
	fwrite(mmin, nmsgs*sizeof(int), fp);
	fclose(fp);
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

	Cputc(vcr + '0');
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
	wopen((80-i)/2, 10, i, 3, attributes[3]);
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
char ostring[25];
show_counter(ptr)
	unsigned char ptr[];
{
	sprintf(ostring,"%02x:%02x:%02x", ptr[0], ptr[1], ptr[2]);
	return ostring;
}
show_time(index)
	int index;
{
	sprintf(ostring,"%s %u, %2u:%02u", months[mmonth[index]], mday[index],
		mhour[index], mmin[index]);
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
 * Record an outgoing message
 */
record_outgoing()
{
	title(Menu1[3]);
	wclwin();
	if(!yesno("Record a new outgoing message"))
		return;

	dbox(vcr1, "RZ");
	wprintf("\nBegin recording your message... Press SPACE to stop:");
	dbox(vcr1, "RE");
	while(wgetc() != ' ');
	dbox(vcr1, (dual < 2) ? "ST" : "PU");
	outlen[0] = counter[0];
	outlen[1] = counter[1];
	outlen[2] = counter[2];
	if(!dual) {
		nmsgs = 0;
		mcounter[0][0] = counter[0];
		mcounter[0][1] = counter[1];
		mcounter[0][2] = counter[2]; }
	wprintf("\n\nOutgoing message length = %s\n", show_counter(counter));

	if(dual >= 2) {
		wprintf("\n\nReady prompt message... press RETURN when ready:");
		while(wgetc() != '\n');
		wprintf("\nRecording prompt...Press SPACE to stop:");
		dbox(vcr1, "RE");
		while(wgetc() != ' ');
		dbox(vcr1, "ST"); }

	standby();
	dbox(vcr1, "RZ");
	wprintf("\nPress RETURN for main menu...");
	while(wgetc() != '\n');
}

/*
 * Play the outgoing message
 */
play_outgoing()
{
	title(Menu1[4]);
	wprintf("\nPlaying tape from 00:00:00 to %s\n", show_counter(outlen));
	dbox(vcr1, "PL");
	if(dual < 2)
		dbox_count(vcr1, "GS", outlen);
	else
		dbox_count(vcr1, "GT", outlen);
}

/*
 * Answer the telephone
 */
auto_answer()
{
	int j;
	wclwin();

	/* Position answer tape in dual mode */
	if(dual) {
		standby();
		if(nmsgs)
			dbox_count(vcr2, "GO", mcounter[nmsgs]);
		else
			dbox(vcr2, "RZ"); }

	wprintf("Auto-answer mode... Press ESCAPE to exit.\n");
	for(;;) {
		title("AUTO-ANSWER Mode");
		dbox(vcr1, "RZ");
		wprintf("Waiting for call... ");
		while(!carrier()) {
			if(wtstc() == 0x1B)
				return; }

		/* Incomming call */
		get_date(&mday[nmsgs], &mmonth[nmsgs], &j);
		get_time(&mhour[nmsgs], &mmin[nmsgs], &j);
		wprintf("Received %s", show_time(nmsgs));
		play_outgoing();

		if(!dual) {
			title("Skip to free...");
			dbox_count(vcr2, "GO", mcounter[nmsgs]); }

		/* Record users message */
		if(carrier()) {
			title("Record Incoming Message");
			wprintf("Record message %u from %s", nmsgs+1, show_counter(mcounter[nmsgs]));
			++nmsgs;
			dbox(vcr2, "RE");
			while(carrier());
			dbox(vcr2, "ST");
			mcounter[nmsgs][0] = counter[0];
			mcounter[nmsgs][1] = counter[1];
			mcounter[nmsgs][2] = counter[2];
			wprintf(" to %s\n", show_counter(mcounter[nmsgs]));
			write_data(); }

		/* If playing prompt - stop before rewind */
		if(dual >= 2)
			dbox(vcr1, "ST"); }
}

/*
 * Play messages
 */
play_messages()
{
	int i, mnum;
	char saveattr;

	title(Menu1[1]);
	wclwin();
	if(!nmsgs) {
		wprintf("\nThere are no messages!\n\nPress ENTER for main menu...");
		while(wgetc() != '\n');
		return; }
	standby();
	mnum = 0;
	saveattr = *W_OPEN;
	dbox(vcr2, "RZ");
	wclwin();
	for(;;) {
		for(i=0; i < nmsgs; ++i) {
			wgotoxy(1, i);
			if(i == mnum)
				*W_OPEN = (saveattr >> 4) + (saveattr << 4);
			wprintf("#%2u - %s - ", i+1, show_time(i));
			wprintf("%s ", show_counter(mcounter[i]));
			wprintf("to %s\n", show_counter(mcounter[i+1]));
			*W_OPEN = saveattr; }

		wgotoxy(1, 19);
		wprintf("Use arrows to select, ENTER to play, or ESCAPE to exit:");
		switch(wgetc()) {
			case _KUA :		/* Previous one */
				mnum = mnum ? mnum - 1 : nmsgs - 1;
				break;
			case '\n' :		/* Play next */
				wgotoxy(1, 19);
				wprintf("Playing...");
				wcleol();
				dbox_count(vcr2, "GO", mcounter[mnum]);
				dbox(vcr2, "PL");
				dbox_count(vcr2, "GS", mcounter[mnum+1]);
				wclwin();
			case _KDA :		/* Select next */
				mnum = (mnum < (nmsgs - 1)) ? mnum + 1 : 0;
				break;
			case 0x1B :
				return; } }
}

/* Print a standby message */
standby()
{
	wprintf("One moment please... ");
}

/*
 * Set configuration
 */
configuration()
{
	int m, c, i, f, b;
	m = 0;
	while(!wmenu(10, 7, attributes[2], Menu2, &m)) {
		switch(m) {
			case 0 :	/* Number of VCR's */
				if(!wmenu(10, 7, attributes[2], Menu4, &dual)) {
					nmsgs = vcr1 = 0;
					vcr2 = c ? 1 : 0;
					mcounter[0][0] = outlen[0];
					mcounter[0][1] = outlen[1];
					mcounter[0][2] = outlen[2]; }
				break;
			case 1 :		/* Carrier detect */
				wmenu(10, 7, attributes[2], Menu3, &cardet);
				break;
			case 2 :		/* Set screen colors */
				i = 0;
				do {
					f = attributes[i] & 0x0F;
					b = (attributes[i] >> 4) & 0x0F;
					wopen(10, 7, 40, 9, WSAVE|WCOPEN|WBOX2|(b<<4)|f);
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
			hmsexit:
				break;
			case 3 :		/* Set outgoing message length */
				i = 0;
				wopen(10, 7, 30, 8, WSAVE|WCOPEN|WBOX2|attributes[2]);
				wcursor_off();
				wputs("Outgoing message length:");
				wgotoxy(0, 4);
				wputs("Use ARROW keys to set.\nPress ESCAPE when finished");
				for(;;) {
					wgotoxy(0, 2);
					wprintf("Setting %-6s : %s", tnames[i], show_counter(outlen));
					switch(wgetc()) {
						case _KRA : i = (i > 1) ? 0 : i+1;	break;
						case _KLA : i = (i) ? i-1 : 2;		break;
						case _KUA :
							if(((c = outlen[i] + 1) & 0x0f) > 9)
								c += 6;
							if(c > 0x59)
								c = 0;
							outlen[i] = c;
							break;
						case _KDA :
							if(c = outlen[i]) {
								if((--c & 0x0f) > 9)
									c = (c & 0xf0) | 9; }
							else
								c = 0x59;
							outlen[i] = c;
							break;
						case 0x1B :
							wclose();
							goto hmsexit; } }
			case 4 :		/* Comm port */
				wmenu(10, 7, attributes[2], Menu5, &comport); } }

	write_data();
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
 * Check for carrier present from the codec modems
 */
carrier()
{
	switch(cardet) {
		case 0 :	return Csignals() & DSR;
		case 1 :	return Csignals() & CTS;
		case 2 :	return Csignals() & (DSR | CTS); }
	return (Csignals() & (DSR | CTS)) == (DSR | CTS);
}
