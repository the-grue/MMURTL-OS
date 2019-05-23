/*
 * COMCHK - A program to checksum .COM files
 *
 * Copyright 1990 Dave Dunfield.
 *
 * May be freely distributed and used as long as
 * my copyright notices are retained.
 */
#include c:\mc\stdio.h
#include c:\mc\file.h

#define BUFFER_SIZE	1024

	unsigned size, cksum = 0;
	char buffer[BUFFER_SIZE], save_code[3];
	FILE *fpr, *fpw;

	char errmsg[] =
		{ 'C','o','r','r','u','p','t',' ','i','m','a','g','e',10,13 };

	extern char CHK_S[], CHK_E[];	/* Embedded function pointers */

/*
 * Write a binary character to a file
 */
write_byte(c)
	char c;
{
	write(&c, 1, fpw);
}

/*
 * Main program - Patch the COM file to include the embedded function.
 */
main(argc, argv)
	int argc;
	char *argv[];
{
	unsigned i;

	if(argc < 3)
		abort("\nUse: comchk oldfile newfile\n\nCopyright 1990 Dave Dunfield\nFreely Distributable.\n");

	if(!(fpr = open(argv[1], F_READ)))
		abort("Cannot open INPUT file");

	if(!(fpw = open(argv[2], F_WRITE)))
		abort("Cannot open OUTPUT file");

/* Determine the size of the file & save the startup code */
	size = i = read(buffer, BUFFER_SIZE, fpr);
	memcpy(save_code, buffer, 3);
	addchk(buffer, i);
	while(i == BUFFER_SIZE) {
		size += (i = read(buffer, BUFFER_SIZE, fpr));
		addchk(buffer, i); }

/* Write out the new startup code that jumps to our embedded function */
	i = (size - 3) + (sizeof(errmsg)+7);
	write_byte(0xE9);
	write_byte(i & 255);
	write_byte(i / 256);

/* Write out the remainder of the original file */
	lrewind(fpr);
	i = read(buffer, BUFFER_SIZE, fpr);
	write(&buffer[3], i - 3, fpw);
	while(i == BUFFER_SIZE) {
		i = read(buffer, BUFFER_SIZE, fpr);
		write(buffer, i, fpw); }

/* Append various data needed */
	write(save_code, 3, fpw);				/* Original startup code */
	write_byte(cksum & 255);				/* Checksum (LOW) */
	write_byte(cksum / 256);				/* Checksum (HIGH) */
	write_byte(sizeof(errmsg) & 255);		/* Msg size (LOW) */
	write_byte(sizeof(errmsg) / 256);		/* Msg size (HIGH) */
	write(errmsg, sizeof(errmsg), fpw);		/* Error message */

/* Append the embedded function */
	size += 0x100;
	write_byte(0xBE);	/* Load SI with address */
	write_byte(size & 255);
	write_byte(size / 256);
	write(CHK_S, CHK_E - CHK_S, fpw);

	close(fpr);
	close(fpw);
}

/*
 * Add contents of buffer to checksum
 */
addchk(ptr, size)
	unsigned char *ptr;
	unsigned size;
{
	while(size--)
		cksum += *ptr++;
}
