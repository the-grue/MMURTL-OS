/*
 * CAN - a program for providing "CANned" arguments to COM files.
 *
 * Copyright 1990 Dave Dunfield.
 *
 * May be freely distributed and used as long as
 * my copyright notices are retained.
 */
#include \mc\stdio.h
#include \mc\file.h

#define BUFFER_SIZE	1024

	unsigned size;
	char buffer[BUFFER_SIZE], save_code[3];
	FILE *fpr, *fpw;

	extern char CAN_S[], CAN_E[];	/* Embedded function pointers */

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
	char *ptr;

	if(argc < 4)
		abort("\nUse: CAN oldfile newfile args...\n\nCopyright 1990 Dave Dunfield\nFreely Distributable.\n");

	if(!(fpr = open(argv[1], F_READ)))
		abort("Cannot open INPUT file");

	if(!(fpw = open(argv[2], F_WRITE)))
		abort("Cannot open OUTPUT file");

/* Determine the size of the file & save the startup code */
	size = i = read(buffer, BUFFER_SIZE, fpr);
	memcpy(save_code, buffer, 3);
	while(i == BUFFER_SIZE)
		size += (i = read(buffer, BUFFER_SIZE, fpr));

/* Write out the new startup code that jumps to our embedded function */
	i = size - 3;
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

/* Append the embedded function */
	size += (i = CAN_E - CAN_S) + 0x103;
	write_byte(0xBE);	/* Load SI with address */
	write_byte(size & 255);
	write_byte(size / 256);
	write(CAN_S, i, fpw);

/* Append the argument extensions */
	for(i=3; i < argc; ++i) {
		write_byte(' ');
		ptr = argv[i];
		while(*ptr)
			write_byte(*ptr++); }
	write_byte('\r');

/* Append the original startup code so that can be restored */
	write(save_code, 3, fpw);

	close(fpr);
	close(fpw);
}
