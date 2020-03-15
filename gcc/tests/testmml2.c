/* Simple test of PutVidChars function and forcing debugging with int 3
 *
 *
*/
#include <mmlib.h>

/* Global variables for easy location with debugger vs stack  */
char *x = "This is a test of gcc on MMURTL";

int main(void)
{
	int *p;
/*	asm ("int $3");	*/	/* Force into debugger */
				/* Note that this can make MMURTL unstable
				   so use only for true debugging and with
				   a reboot for sanity once done  */

	PutVidChars(5, 5, x, 31, 0x01 | 0x70);
	ExitJob(0);
}
