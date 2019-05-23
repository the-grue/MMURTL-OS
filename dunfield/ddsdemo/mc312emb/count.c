#include cflea.h
/*
 * Count to 1000, and display numbers as ASCII
 */
main()
{
	int i;
	for(i=0; i < 1000; ++i)
		printf("%u\n", i);
	asm " FCB $FF";		/* Illegal opcode to stop simulator */
}
