/* Simple test of WriteBytes and newlib exit
 *
 *
*/
#include <mmlib.h>
#include <stdlib.h>
#include <string.h>

/* Global variables for easy location with debugger vs stack  */
char *x = "This is a test of gcc and newlib on MMURTL\n\r";

int main(void)
{
	int *p;
/*	asm ("int $3");	*/	/* Force into debugger */
				/* Note that this can make MMURTL unstable
				   so use only for true debugging and with
				   a reboot for sanity once done  */

	WriteBytes(2, x, strlen(x), p);
	exit(0);
}
