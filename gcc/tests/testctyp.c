/* Simple test of ctype function and forcing debugging with int 3
 *
 *
*/
#include <ctype.h>
#include <stdlib.h>

/* Global variables for easy location with debugger vs stack  */
char x;
char X;

int main(void)
{
	asm ("int $3");		/* Force into debugger */
				/* Note that this can make MMURTL unstable
				   so use only for true debugging and with
				   a reboot for sanity once done  */
	x='x';
	X=toupper(x);

	exit(0);
}

asm ("call main\n");
