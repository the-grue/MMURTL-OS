/*
 * 8051 Timer/Interrupt demonstration program
 *
 * This program sets up timer0, to generate regular interrupts, which are
 * user to increment a counter in memory. The main program monitors that
 * counter, and sends a derived (printable) value to the serial port any
 * time that it changes.
 *
 * It is assumed that this program will be run beneath the MON51 or MONICA
 * debuggers, and therefore the serial port is assumed to be already
 * initialized.
 *
 * Make sure that the INTBASE constant in the 8051INT.H file is set
 * correctly for your development environment. See the comments in that
 * file for details.
 *
 * Compile with the EXTENDED PRE-PROCESSOR ('-p' option), in any memory model.
 */
#include <8051io.h>			/* 8051 I/O definitions */
#include <8051reg.h>		/* 8051 SFR definitions */
#include <8051int.h>		/* 8051 interrupt macro */

#define	COUNT	61440		/* Count value for timer0 */

register count, old_count;	/* Counter & previous value */

/*
 * Timer0 (vector 11) Interrupt handler.
 * Reset the timer, and increment a variable.
 */
INTERRUPT(_TF0_) myfunc()
{
	TH0 = -COUNT >> 8;		/* Reset high count */
	TL0 = -COUNT;			/* Reset low count */
	++count;
}

/*
 * Main program.
 * Set up timer, enable interrupts, and monitor counter value.
 *
 * Note that we keep timer1 running in 8 bit auto-reload mode,
 * since it provides the baud-rate generator clock for the serial
 * port used by MON51 or the MONICA kernal.
 */
main()
{
	TMOD = 0x21;		/* T1=8 bit auto-reload, T0=16 bit cascade */
	TH0 = -COUNT >> 8;	/* Initial T0 high count */
	TL0 = -COUNT;		/* Initial T0 low count */
	TCON = 0x50;		/* Run T1, Run T0 */
	IE = 0x82;			/* Enable T0 interrupt */

	for(;;) {
		if(count != old_count) {
			old_count = count;
			putchr((count & 0x0f) + 'A'); } }
}
