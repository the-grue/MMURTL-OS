/*
 * Test program to display passed arguments.
 */
#include \mc\stdio.h

main(argc, argv)
	int argc;
	char *argv[];
{
	int i;
	for(i=0; i < argc; ++i)
		printf("argv[%u] = '%s'\n", i, argv[i]);
}
