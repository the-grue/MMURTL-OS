/*
 * Telephone "distinctive ringing" detector.
 *
 * This program may be compiled with the DDS 8051 MICRO-C Compiler.
 *
 * Compile command: CC51 ringswit -fop m=t
 *
 * Permission granted for personal (non commercial) use only.
 *
 * Copyright 1993-1995 Dave Dunfield
 * All rights reserved.
 */
#include <8051io.h>				/* 8051 I/O definitions */
#include <8051reg.h>			/* 8051 register definitions */
#include <8051bit.h>			/* 8051 bit manipulation macros */

/*
 * If you remove the following define, the code will output progress
 * message to the 8051 serial port, which are useful when troubleshooting.
 */
#define	printf()				/* disable printf unless debugging */

/*
 * 8051 internal RAM locations
 */
register unsigned char P, LP1;	/* Active port mask & status */
register unsigned bflag;		/* Busy tone cadence counter */

/*
 * Main program....
 *
 * Sit in idle loop, waiting for ringing, or any set off-hook.
 */
main()
{
#ifndef printf
	serinit(28800);		/* For compatibility with DDS MONICA debugger */
#endif

	bflag = 0;
	for(;;) {
		P1 = 0x0F;			/* Set all lines idle */
		pause();
		printf("Idle\n");
		P = (~readp1() & 0x0F);
		if(P & 0x01)		/* Ringing - incoming call */
			incomming();
		else if(P & 0x0E)	/* Set off-hook - originate */
			outgoing(); }
}

/*
 * Incomming call... count rings, and route to proper set
 */
incomming()
{
	unsigned i, n;

	printf("Incomming... ");

	/* First, count number of rings */
	n = 0;
	for(;;) {
		++n;
		while(~readp1() & 0x01);		/* Wait for ring to stop */
		i = 0;
		while(readp1() & 0x01) {		/* Check for no-ring */
			if(++i > 12)
				goto doring; } }

doring:
	printf("%u rings\n", n);

	if((n < 1) || (n > 3))				/* Invalid... try again */
		return;

	P = 1 << n;			/* Set line mask */

	/* Select line (activate relay) */
	P1 = (P << 3) | 0x0F;

	/* Wait for ringing to stop */
	for(i=0; i < 100; ++i) {
		busy();
		if(~LP1 & 0x01) {
			i = 0;
			printf("Ring\n"); } }

	printf("Connected\n");

	/* Wait for line to go on-hook */
	for(i=0; i < 25; ++i) {
		busy();
		if(~LP1 & P)
			i = 0; }
}

/*
 * Local set has gone off-hook, connect to line, and indicate busy
 * to other sets until the call is completed.
 */
outgoing()
{
	unsigned i;

	printf("Outgoing P=%02x\n", P);

	P1 = (P << 3) | 0x0F;		/* Select line (activate relay) */
	pause();
	for(i=0; i < 25; ++i) {
		busy();
		if(~LP1 & P)
			i = 0; }
}

/*
 * Generate a short pause to allow relays to settle etc.
 */
pause()
{
	unsigned i;

	for(i=0; i < 1000; ++i);
}

/*
 * Generate "busy" tone on local loop if any sets other than
 * the one involved in the current call are off-hook.
 */
busy()
{
	unsigned i, j;

	if((~readp1() & ~P) & 0x0E) {
		printf("Busy\n");
		if(++bflag >= 8) {
			bflag = 0;
			for(i=0; i < 150; ++i) {
				setbit(P1.7);
				for(j=0; j < 15; ++j);
				clrbit(P1.7);
				for(j=0; j < 15; ++j); } } }
}

/*
 * Read and de-bounce the input port P1
 */
readp1()
{
	unsigned i, p, q;

	p = q = P1;
	for(i=0; i < 600; ++i) {
		if(p != P1) {
			q &= (p = P1);
			i = 0; } }

	return LP1 = p & (q | 0xFE);
}
