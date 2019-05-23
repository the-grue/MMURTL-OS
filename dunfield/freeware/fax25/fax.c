/*
 * Test program to perform basic functions on the CANNON FAXPHONE-25
 *
 * Copyright 1991,1992 Dave Dunfield
 * All rights reserved.
 */
#include c:\mc\stdio.h
#include c:\mc\comm.h

#define	ACK		0x06
#define	NAK		0x15
#define	MPS		0x02
#define	EOP		0x04

char DEF_font[] = "MEDRES.FNT";		/* Default font file */

char *help[] = {
	"",
	"Use: FAX  SC <image file>                     - Scan image",
	"          PT <text  file> [font file]         - Print TEXT",
	"          PI <image file>                     - Print IMAGE",
	"          ST <text  file> <tel #> [font file] - Send TEXT fax",
	"          SI <image file> <tel #>             - Send IMAGE fax",
	"\nCopyright 1991,1992 Dave Dunfield\nAll rights reserved.",
	0 };

/*
 * Main program
 */
main(argc, argv)
	int argc;
	char *argv[];
{
	int i;

	if(argc < 3) {
		for(i=0; help[i]; ++i)
			printf("%s\n", help[i]);
		exit(0); }

	if(Copen(1, _9600, DATA_8|PAR_NO|STOP_1, SET_DTR|SET_RTS|OUTPUT_2))
		abort("Cannot open COMM port");
	Cflags |= TRANSPARENT;

	switch((toupper(argv[1][0])<<8) | toupper(argv[1][1])) {
		case ('S'<<8)|'C' :		/* Scan an image */
			command("CC=010104/CD=03/FN=15//");
			status();
			load_image(argv[2]);
			break;
		case ('P'<<8)|'T' :		/* Print TEXT */
			send_font(argc < 4 ? DEF_font : argv[3]);
			send_text(argv[2]);
			command("CC=010402/CD=00/FN=10//");
			break;
		case ('P'<<8)|'I' :		/* Print an image */
			send_image(argv[2]);
			command("CC=010402/CD=03/FN=15//");
			break;
		case ('S'<<8)|'T' :		/* Send TEXT file */
			if(argc < 4)
				abort("Telephone number required");
			send_dest(argv[3]);
			send_font(argc < 5 ? DEF_font : argv[4]);
			send_text(argv[2]);
			command("CC=010405/FN=10//");
			break;
		case ('S'<<8)|'I' :		/* Send image */
			if(argc < 4)
				abort("Telephone number required");
			send_dest(argv[3]);
			send_image(argv[2]);
			command("CC=010405/FN=15//");
			break;
		default:
			terminate("Unknown FAX command"); }

	Cclose();
}

/*
 * Read a character from the FAX (with timeout in seconds)
 */
Cinput(limit)
	unsigned limit;
{
	int j, s, s1;

	get_time(&j, &j, &s);
	for(;;) {
		if((j = Ctestc()) == -1) {
			get_time(&j, &j, &s1);
			if(s == s1)
				continue;
			s = s1;
			if(--limit)
				continue;
			j = -1; }
		return j; }
}

/*
 * Send a string to the serial port
 */
Cputs(string)
	char *string;
{
	while(*string)
		Cputc(*string++);
}

/*
 * Send a command to the FAX (& wait for ACK)
 */
command(string)
	char *string;
{
	printf("Command='%s' ", string);
	Cputs(string);
	return wait_ack();
}

/*
 * Wait for an ACK code from the FAX, and report any
 * other inputs.
 */
wait_ack()
{
	int j;

	for(;;) switch(j = Cinput(5)) {
		case -1 :
			terminate("Timeout\n");
		case ACK :
			printf("ACK received\n");
			return 0;
		case NAK :
			printf("%02x \n", j); }
}

/*
 * Wait for status from the FAX
 */
status()
{
	printf("Waiting for STATUS... ");

	for(;;) switch(Cinput(99)) {
		case '=' :		/* Status message is "ID=nn//" */
			putc(Cgetc(), stdout);
			putc(Cgetc(), stdout);
			Cgetc();
			Cgetc();
			putc('\n', stdout);
			return 0;
		case -1 :		/* Timeout */
			terminate("Timeout\n"); }
}

/*
 * Load an image from the FAX
 */
load_image(fname)
	char *fname;
{
	int c;
	FILE *fp;

	if(!(fp = fopen(fname, "wbv")))
		terminate("Cannot WRITE image file");

	command("CC=010407/CD=03/FN=15//");
	while((c = Cinput(5)) >= 0)
		putc(c, fp);
	Cputc(ACK);
	fclose(fp);
}

/*
 * Send an image to the FAX
 */
send_image(fname)
	char *fname;
{
	int c;
	FILE *fp;

	if(!(fp = fopen(fname, "rbv")))
		terminate("Cannot READ image file");

	command("CC=010704/CD=03/FN=15//");
	while((c = getc(fp)) >= 0)
		Cputc(c);
	wait_ack();
	fclose(fp);
}

/*
 * Download a font into the FAX
 */
send_font(fname)
	char *fname;
{
	int c;
	FILE *fp;

	if(!(fp = fopen(fname, "rvb")))
		terminate("Cannot READ font file");

	command("CC=010704/CD=01/FN=01//");
	Cputs("DT=");
	while((c = getc(fp)) >= 0)
		Cputc(c);
	Cputc(EOP);
	wait_ack();
	fclose(fp);
}

/*
 * Send a text image to the FAX
 */
send_text(fname)
	char *fname;
{
	int c;
	FILE *fp;

	if(!(fp = fopen(fname, "rvq")))
		terminate("Cannot READ text file");

	command("CC=010704/CD=00/FN=10//");
	Cputs("DT=");
	while((c = getc(fp)) >= 0) {
		if(c == '\n') {
			Cputc('\r');
			Cputc('\n'); }
		else
			Cputc(c); }
	Cputc('\f');
	Cputc(EOP);
	wait_ack();
	fclose(fp);
}

/*
 * Send FAX telephone number.
 *
 * This routine uses the "special" coded speed dial numbers which are
 * reserved for use by the RS-232 mode only (51-66). Although all 16
 * numbers must be loaded, only the first one is used, after which
 * the parser "sticks" at the end of the number string, and loads the
 * remaining entries with 0x0C - Not used indications.
 */
send_dest(number)
	char *number;
{
	int b, c, i, j;

/*
First, send DESTINATION data... If you convert some of the 'CC' (unused)
entries, you can send to up to 16 destinations at the same time.
*/
	command("CC=010704/CD=00/FN=40//");
	Cputs("DT=");
	Cputc(0x01);	/* 00=One touch SPD, 01=Coded SPD, CC=Not used */
	Cputc(0x51);    /* First RS-232 only coded speed dial (51-66)  */
	for(i=0; i < 31; ++i)	/* Remaining 15 entries are not used */
		Cputc(0xCC);
	Cputc(EOP);
	wait_ack();

/*
Now, send telephone numbers to be placed in the "special" coded speed
dial entries (51-66). - We have to send 16 entries, however only the
first one will contain digits. The remainder will look like null numbers,
and will be send as '0x0C' which means 'Not used'
*/
	command("CC=010704/CD=00/FN=50//");
	Cputs("DT=");
	b = 0;
	for(i=0; i < 16; ++i) {		/* Send 16 entries */
		Cputc(0x00);			/* Indicate start of number */
		for(j=0; j < 30; ++j) {
			switch(c = *number) {
				case '0' :
				case '1' :
				case '2' :
				case '3' :
				case '4' :
				case '5' :
				case '6' :
				case '7' :
				case '8' :
				case '9' :
					c -= '0';
					++number;
					break;
				case 0 :		/* End of number - not used */
					c = 0x0C;
					break;
				case ',' :		/* Pause in number */
				case 'P' :
				case 'p' :
					c = 0x0E;
					++number;
					break;
				default:		/* All else, assume space */
					c = 0x0F;
					++number; }
			b = (b << 4) | c;
			if(j & 1) {
				Cputc(b);
				b = 0; } } }
	Cputc(EOP);
	wait_ack();
}

/*
 * Close the serial port (to insure no interrupts hanging)
 * and terminate with an error message
 */
terminate(msg)
	char *msg;
{
	Cclose();
	abort(msg);
}
