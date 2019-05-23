/*
 * This program implements a "soft" die using the C-FLEA
 *
 * A LED display is driven by ports P0-P3 in the following configuration
 *
 * P1: 1100110011
 * P2: 0000110000
 * P3: 1100110011
 * 
 * Multiple ports/bits are used to make the die more visible from the
 * EMCF simulator display. If physical LEDS were used, only a single
 * port would be required, with a simpler translate table.
 *
 * Rolling of the die is triggered by the reception of any character
 * from the keyboard. In a physical hardware implementation, a momentary
 * contact switch could be connected to an unused port input.
 *
 * The display is assumed to be ACTIVE HIGH (writeing 1 turns on the LED)
 * You can change this to ACTIVE LOW by inverting the bits in the translate
 */
#include cflea.h

#define	DELAY	20000		/* Adjust this value to vary ROLL delay */

char *translate_table[6][3] = {
	0x00, 0x18, 0x00,		/* 1 */
	0xC0, 0x00, 0x03,		/* 2 */
	0x03, 0x18, 0xC0,		/* 3 */
	0xC3, 0x00, 0xC3,		/* 4 */
	0xC3, 0x18, 0xC3,		/* 5 */
	0xDB, 0x00, 0xDB };		/* 6 */

main()
{
	unsigned char old, new;
	unsigned i, j;

	old = 5;				/* First wraps to 1 */
	outp1(0);
	outp2(0);
	outp3(0);

	putstr("\nPress any key to roll die:\n");

	while(!chkch())			/* Wait for first keypress */
		rand();				/* Cycle random number generator */

	/* Roll dice & cycle to new value */
	for(;;) {
		putstr("Rolling ");
		new = rand(6);
		for(i=0; (i < 6) || (old != new); ++i) {
			old = (old + 1) % 6;
			outp1(translate_table[old][0]);
			outp2(translate_table[old][1]);
			outp3(translate_table[old][2]);
			for(j=0; j < DELAY; ++j); }
		putch('\n');
		getch(); }	/* Wait for next button pressed */
}
