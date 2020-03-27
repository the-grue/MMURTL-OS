/* clear.c - test job to exercise some MMURTL system calls
 *           from a gcc compiled runfile.
 *
 * 
*/
#include <mmlib.h>

int rando;				/* Never used but MMURTL requires a data segment */

int main(void)
{
	SetJobName("CLEAR", 5);		/* F2 in Monitor to see */
	ClrScr();			/* Clear the screen */
	Sleep(1000);			/* Sleep 10 secs so you can see job name in monitor */
	ExitJob(0);			/* Exit and return to CLI */

	return 0;			/* Never happens! */
}

