/*
 * Program to walk a bit through an I/O port
 */
main()
{
	int i;

loop:
	outp1(0x01);				/* Initialize to only bit0 */

	for(i=0; i < 8; ++i) {		/* Perform 8 steps */
		wait(50);				/* Wait a bit */
		outp1(inp1() << 1);	}	/* Move bit over */

	goto loop;					/* Infinite loop */
}

/*
 * Wait a short delay
 */
wait(delay)
	unsigned delay;
{
	while(delay)
		--delay;
}
