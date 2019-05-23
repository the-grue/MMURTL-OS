/*
 * This program implements a simple "calculator" using LONG numbers.
 * It demonstrates the "long" math functions from the library.
 */

#include cflea.h

/*
 * Default LONG number size is 4 bytes (32 bits). If you wish to change
 * this, look at the LONGMATH.ASM file in the library. You also have to
 * adjust the sizes of all array declarations which are to contain LONG
 * numbers. For this demo, just adjust the constant defined below.
 */
#define	LSIZE	4		/* 32 bit numbers */

/*
 * Long number registers
 */
char reg[LSIZE], temp1[LSIZE],	temp2[LSIZE];

/*
 * Input/Output Digit buffer. Note that we use an approximation to
 * compute the number of digits required based on the size of the
 * LONG numbers.
 */
char buffer[(LSIZE*25)/10+1];

/*
 * This temporary variable used by the LONGMATH functions is useful,
 * because it will contain the remainder after a division.
 */
extern char Longreg[];

/*
 * Convert a LONG number into a printable string
 */
long_to_string(string, n1, base)
	 unsigned char *string, *n1, base;
{
	unsigned sp;
	unsigned char c, stack[(LSIZE*25)/10+1];

	longcpy(temp2, n1);
	longset(temp1, base);
	
	/* Stack up digits in reverse order */
	sp = 0;
	do {
		longdiv(temp2, temp1);
		stack[sp++] = ((c = *Longreg) > 9) ? c + '7' : c + '0'; }
	while(longtst(temp2));

	/* Unstack digits into output buffer */
	do
		*string++ = stack[--sp];
	while(sp);
	*string = 0;
}

/*
 * Convert a string into a LONG number
 * Returns character terminating conversion.
 */
string_to_long(string, n1, base)
	 unsigned char *string, *n1, base;
{
	unsigned char c;

	longset(n1, 0);

	while(c = *string++) {
		if(isdigit(c))
			c -= '0';
		else if(c >= 'a')
			c -= ('a' - 10);
		else if(c >= 'A')
			c -= ('A' - 10);
		else
			break;
		if(c >= base)
			break;
		longset(temp1, base);
		longmul(n1, temp1);
		longset(temp1, c);
		longadd(n1, temp1); }
	return c;
}

/*
 * Main calculator program
 */
main()
{
	unsigned char c, op;
	 char *ptr;

clear:			/* Clear the calculator by forcing a copy operation */
	putstr("\nReady\n");
	op = '=';

next:			/* Begin processing a number */
	ptr = buffer;
next1:			/* Continue processing a number */
	while(isdigit(c = getch())) {		/* Collect digits into string */
		putch(c);
		*ptr++ = c; }
	*ptr = 0;							/* Zero terminate */
	string_to_long(buffer, temp2, 10);	/* Get LONG value for operation */

	switch(op) {						/* Perform pending operation */
		case '=' :	longcpy(reg, temp2);	break;
		case '+' :	longadd(reg, temp2);	break;
		case '-' :	longsub(reg, temp2);	break;
		case '*' :	longmul(reg, temp2);	break;
		case '/' :	longdiv(reg, temp2);	break;
		case '%' :	longdiv(reg, temp2); longcpy(reg, Longreg); }

operate:								/* Process next operation */
	putch(c);							/* Echo next command */
	switch(c) {
		default:
			putstr(" Use: + - * / % = C");
		case 'c' :						/* Clear */
		case 'C' :
			goto clear;
		case '=' :						/* Display results */
			long_to_string(buffer, reg, 10);
			putstr(buffer);
			if(isdigit(c = getch())) {
				op = '=';
				putch('\n');
				putch(c);
				ptr = buffer;
				*ptr++ = c;
				goto next1; }
			goto operate;
		case '+' :						/* Addition */
		case '-' :						/* Subtraction */
		case '*' :						/* Multiplication */
		case '/' :						/* Division */
		case '%' :						/* Modulus */
			op = c; }
	goto next;
}
