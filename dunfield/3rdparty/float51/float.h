/**************************************************************************
*
* Floating point numbers conform to the IEEE binary floating point
* standard.An eight bit exponent and 24 bit mantissa are supported.
* Using four bytes the format is shown below:
*
* 31 30________23 22______________________________0
* s    exponent     mantissa
*
* The exponent is biased by 128. Above 128 are positive exponents and
* below are negative.  The sign bit is 0 for positive numbers and 1 for
* negative.The mantissa is stored in hidden bit normalized format so
* that 24 bits of precision can be obtained.  Since by definition A
* normalized number's most significant bit is 1, this position is used
* to store the exponent's least significant bit.  This allows storing
* the entire number in just four bytes.The following math routines
* require the sign, exponent, and mantissa to be separate.FPACC1 and
* FPACC2 are reserved for operations.
*
***************************************************************************/
#define	NUMENTER		1
#define	OVERFLOW		4
#define	POSTDP			8

register char flagreg;			/* Flag register */

register char fpacc1ex;			/*FLOATING POINT ACCUMULATOR #1*/
register char fpacc1mn[3];
register char mantsgn1;			/*MANTISSA SIGN FOR FPACC1 (0=+, FF=-)*/
register char fpacc2ex;			/*FLOATING POINT ACCUMULATOR #2*/
register char fpacc2mn[3];
register char mantsgn2;			/*MANTISSA SIGN FOR FPACC2 (0=+, FF=-)*/
register char fpaccB[4];		/*ANSWER FPACC STORAGE*/

#define fpacc1 &fpacc1ex
#define fpacc2 &fpacc2ex

char cons0[4]={0,0,0,0};		/*Constant 0*/

register char error;			/* 0=None, 1=Overflow, 2=Underflow */

asm {
num_char	equ		13			Number of characters to be displayed
$RL:4							Insure ?udiv is loaded
}

/* Floating Point to ASCII Routine
*
* This routine will convert the number in fpacc1 and convert
* it to ASCII.
*/
fltasc() asm
{
		mov		A,#fpacc1ex		Point to fpacc1
		lcall	chck0			Is it zero?
		jz		fltasc1			No, continue
		mov		R2,#num_char-2
fltas3a mov		A,#' '
		lcall	?putch
		djnz	R2,fltas3a
		mov		A,#'0'
		lcall	?putch
		mov		A,#'.'
		ljmp	?putch
fltasc1 lcall	pshf2
		mov		A,#maxdec		Get constant 100000.
		mov		B,#=maxdec
		lcall	getfpc2			Load fpacc2
		lcall	fltcmp			Is fpacc1 >= 100000?
		jc		fltasc2			No, continue
		mov		error,#1		Return with overflow status
		lcall	popf2
		ret
fltasc2 mov		A,#mindec		Get constant .00001
		mov		B,#=mindec
		lcall	getfpc2			Load fpacc2
		lcall	fltcmp			Is fpacc1 > .00001
		jnc		fltas25			Yes, continue
		jnz		fltas25			Fpacc1 = .00001, continue
		mov		error,#2		Return with underflow status
		lcall	popf2
		ret
fltas25 lcall	mov1to2			Save fpacc1 in fpacc2
		mov		R2,#0			Initialize digit counter
		mov		A,#'-'
		anl		A,mantsgn1
		orl		A,#$20			A is '-' if sign is negative, ' ' if positive
		push	A
		inc		R2
		mov		mantsgn1,#0		Make sign positive
*
* Move the radix point all the way to the right
*
		clr		C
		mov		A,fpacc1ex
		subb	A,#$81			Take out bias +1
		cpl		A
		inc		A				Twos complement exponent
		add		A,#24			Add in the number of mantissa bits
		mov		R3,A
		ljmp	fltas17			Check if shift is required
fltasc7 clr		C
		mov		A,fpacc1mn
		rrc		A				Shift mantissa to the right
		mov		fpacc1mn,A
		mov		A,fpacc1mn+1
		rrc		A
		mov		fpacc1mn+1,A
		mov		A,fpacc1mn+2
		rrc		A
		mov		fpacc1mn+2,A
fltas17 djnz	R3,fltasc7		Keep shifting
*
* Convert integer to ASCII
*
fltasc3 mov		R3,#$0A		Divide the integer by 10
		mov		R4,#0
		mov		A,fpacc1mn+1
		mov		B,fpacc1mn
		lcall	?udiv
		mov		fpacc1mn,B
		mov		fpacc1mn+1,A
		mov		B,R5
		mov		A,fpacc1mn+2
		lcall	?udiv
		mov		fpacc1mn+2,A
		mov		A,R5
		add		A,#$30			Make ASCII
		push	A				Save remainder
		inc		R2				Increment digit counter
		mov		A,fpacc1mn+2	Is quotient zero?
		jnz		fltasc3
		mov		A,fpacc1mn+1
		jnz		fltasc3
		mov		A,fpacc1mn
		jnz		fltasc3
*
* Convert fraction
*
fltas3f lcall	mov2to1			Get fpacc1
		clr		C
		mov		A,fpacc1ex		Calculate the exponent size of the fraction
		subb	A,#$80			Subtract bias 80
		mov		R3,A
		jc		fltas28			Exponent negative, shift right until exp=80
fltas32 mov		A,R3			Exponent positive, shift left until exp=80
		jz		fltas33
		clr		C
		mov		A,fpacc1mn+2	Shift mantissa to the left
		rlc		A
		mov		fpacc1mn+2,A
		mov		A,fpacc1mn+1
		rlc		A
		mov		fpacc1mn+1,A
		mov		A,fpacc1mn
		rlc		A
		mov		fpacc1mn,A
		dec		R3
		ljmp	fltas32
fltas28 mov		A,R3
		jz		fltas33
		clr		C
		mov		A,fpacc1mn		Shift mantissa to the right
		rrc		A
		mov		fpacc1mn,A
		mov		A,fpacc1mn+1
		rrc		A
		mov		fpacc1mn+1,A
		mov		A,fpacc1mn+2
		rrc		A
		mov		fpacc1mn+2,A
		inc		R3
		clr		A
		addc	A,fpacc1mn+2
		jz		fltas28			Dont round if overflow in NSB
		mov		fpacc1mn+2,A
		ljmp	fltas28
*
* Convert fraction to decimal
*
fltas33 mov		A,#'.'
		push	A				Push A '.'
		inc		R2
		cjne	R2,#8,fltas90	Only push eight digits
		ljmp	fltas70			Go to print routine
fltas90 lcall	mov1to2			Move fpacc1 because mulby10 uses fpacc2
		mov		R4,#0			Initialize trailing zero counter
fltas73 mov		R0,#fpacc2mn
		lcall	chck02
		jnz		fltas70			Mantissa is zero
		lcall	mulby10
		mov		A,B				Get product
		inc		R4				Assume A trailing zero, increment counter
		jz		fltas97
		mov		R4,#0			Found A nonzero digit, clear zero counter
fltas97 add		A,#$30			Make ASCII
		push	A				Push digit
		inc		R2
		cjne	R2,#8,fltas73	Only push eight digits
*
* Pop digits and print
*
fltas70 mov		?R7,R2			Move digit count to R7 because putch destroys R2 and R3
		clr		C
		mov		A,#num_char		Maximum number of digits
		subb	A,R7			Subtract digit count to calculate the number of leading SPaces
		add		A,R4			Print leading SPaces instead of trailing zeros
		mov		R1,A
fltas96 jz		fltas56			Dont print any SPaces
fltas55 mov		A,#' '
		lcall	?putch
		djnz	R1,fltas55
fltas56 clr		C
		mov		A,SP
		subb	A,R7
		mov		R5,A			Save return address
		inc		A
		mov		R1,A			Find beginning digit (- or A SPace)
		clr		C
		mov		A,R7
		subb	A,R4			Subtract the number of trailing SPaces from the number of digits to be printed
		mov		R7,A
fltas95 mov		A,[R1]			Get the digit
		inc		R1
		lcall	?putch			Print '-' or ' '
		dec		R7
		mov		R6,#0			Initialize integer digit counter
fltas89 mov		A,[R1]			Count the digits until the decimal point
		inc		R1
		inc		R6
		cjne	A,#'.',fltas89
		dec		R1
		dec		R1
		dec		R6
fltas81 mov		A,[R1]			Print remaining number in decimal
		dec		R1
		lcall	?putch
		dec		R7
		djnz	R6,fltas81		Continue printing until complete
fltas88 mov		A,[R1]			Now find the decimal point
		inc		R1
		cjne	A,#'.',fltas88
		dec		R1
fltas87 mov		A,[R1]
		inc		R1
		lcall	?putch
		djnz	R7,fltas87
		mov		SP,R5			Put return address in SP
		clr		A				Return good error status
		lcall	popf2
}

fpcomp(arg1, arg2)		/*Compare arg1 with arg2, return one if they equal*/
int arg1, arg2;
{
	movfpac(arg1,fpacc1);
	movfpac(arg2,fpacc2);
	if(mantsgn1 != mantsgn2)
		return 0;
	else
		asm " LCALL fltcmp";
}

asm {
*
* Compare fpacc1 and fpacc2
* Set carry if fpacc1 < fpacc2
* Clear carry if fpacc1 >= fpacc2
* Set A = 1 if fpacc1 = fpacc2
*
fltcmp	mov		A,fpacc1ex
		cjne	A,fpacc2ex,fltcmp1		Exponents are not equal
fltcmpm mov		A,fpacc1mn
		cjne	A,fpacc2mn,fltcmp1		MSBs are not equal
		mov		A,fpacc1mn+1
		cjne	A,fpacc2mn+1,fltcmp1	NSBs are not equal
		mov		A,fpacc1mn+2
		cjne	A,fpacc2mn+2,fltcmp1	LSBs are not equal
		clr		C
		clr		A
		mov		B,A
		inc		A
		ret
fltcmp1 clr		A						Clear A for not equal
		mov		B,A						Clear B
		ret
}


fpgrtr(arg1,arg2)	/*Return one if the content of arg1 is greater than arg2*/
int arg1, arg2;
{
	movfpac(arg1,fpacc1);
	movfpac(arg2,fpacc2);
	if(!mantsgn1)
		return mantsgn2 ? 1 : absgrtr();
	else
		return mantsgn2 ? !absgrtr() : 0;
}

absgrtr() asm /*Return 1 if the absolute value of fpacc1 is greater than fpacc2*/
{
		lcall	fltcmp
		jc		fpgtr			Fpacc1<fpacc2
		jnz		fpgtr			Fpacc1=fpacc2
		mov		A,#1			Fpacc1>fpacc2
		ret
fpgtr	clr		A
}

asm {
maxdec	DB $48,$C3,$4F,$81		Constant 100000
mindec	DB $38,$27,$C5,$A0		Constant 0.00001
* Last digit of maxdec has been changed from '0' to '1' to allow 99999
* Last digit of mindec has been changed from 'D' to '0' to allow for rounding
}


fix(arg) /*Fix fpac arg and return int, msb!=0 is returned if > 127 or < 0 */
int arg;
{
	movfpac(arg,fpacc1);
	asm {
		clr		C
		mov		A,#$87
		subb	A,fpacc1ex
		mov		R0,A			Save number of shifts in R0
		jc		fixerr			Number greater than 255
		mov		A,mantsgn1
		jnz		fixerr			Number less than 0
		inc		R0
		mov		A,fpacc1mn
fix1	rr		A
		djnz	R0,fix1			Rotate right until exponent is zero
		mov		B,#0
		ret
fixerr	mov		B,#1			Return error
	}
}

clrfpac(arg)	/*Clear fpacc*/
int	arg;
{
	movfpac(cons0,arg);
}

/* ASCII to Floating Point Routine
 *
 * buildnum() will accept an ASCII number 0-9.	Overflow bit will be set
 * if the number exceeds the exponent.	Postdp bit must be set by main
 * program will A decimal point is entered.
 *
 * finflt() completes the conversion after all the numbers have been
 * entered.
 */
buildnum(arg)
int	arg;
{
	if(arg == '.') {
		flagreg |= POSTDP;
		return; }
	flagreg |= NUMENTER;
	if(flagreg & OVERFLOW) {
		if(!(flagreg & POSTDP))
			++fpacc2ex; }	/*Increment exponent if pre decimal point*/
	else {
		if(addnxtd(arg)) {
			if(flagreg & POSTDP)
				flagreg |= OVERFLOW;
			else
				++fpacc2ex; }	/*Increment exponent if pre decimal point*/
		else if(flagreg & POSTDP)
				--fpacc2ex; }		/*Decrement exponent if post decimal point*/
}

asm {
*
* Multiply fpacc1 by 10
*
mulby10 mov		A,fpacc2mn		Get MSB
		mov		fpacc1mn,A		Copy into fpacc1
		mov		A,fpacc2mn+1	Get NSB
		mov		fpacc1mn+1,A	Copy into fpacc1
		mov		A,fpacc2mn+2	Get LSB
		mov		fpacc1mn+2,A	Copy into fpacc1
		clr		C				Multiply fpacc2 by 2
		mov		A,fpacc2mn+2
		rlc		A
		mov		fpacc2mn+2,A
		mov		A,fpacc2mn+1
		rlc		A
		mov		fpacc2mn+1,A
		mov		A,fpacc2mn
		rlc		A
		mov		fpacc2mn,A
		clr		A
		rlc		A
		mov		B,A
		mov		A,fpacc2mn+2	Multiply fpacc2 by 4
		rlc		A
		mov		fpacc2mn+2,A
		mov		A,fpacc2mn+1
		rlc		A
		mov		fpacc2mn+1,A
		mov		A,fpacc2mn
		rlc		A
		mov		fpacc2mn,A
		mov		A,B
		rlc		A
		mov		B,A
		mov		A,fpacc2mn+2	Multiply fpacc2 by 5
		add		A,fpacc1mn+2
		mov		fpacc2mn+2,A
		mov		A,fpacc2mn+1
		addc	A,fpacc1mn+1
		mov		fpacc2mn+1,A
		mov		A,fpacc2mn
		addc	A,fpacc1mn
		mov		fpacc2mn,A
		clr		A
		addc	A,B
		mov		B,A
		mov		A,fpacc2mn+2	Multiply fpacc2 by 10
		rlc		A
		mov		fpacc2mn+2,A
		mov		A,fpacc2mn+1
		rlc		A
		mov		fpacc2mn+1,A
		mov		A,fpacc2mn
		rlc		A
		mov		fpacc2mn,A
		mov		A,B
		rlc		A
		mov		B,A
		ret
}

addnxtd() asm
{
*
* Add digit to fpacc1, set carry if overflow
*
		mov		R0,#-5
		lcall	?auto0
		mov		?R1,[R0]		Get keystroke and save in R1
		lcall	pshf1			Save fpacc1
		lcall	mulby10			Multiply fpacc1 by 10
		jnz		addnxt1			Jump and restore if overflow
		mov		A,R1			Restore character
		subb	A,#$30			Remove ascii
		add		A,fpacc2mn+2	Add digit to fpacc2
		mov		fpacc2mn+2,A
		clr		A
		addc	A,fpacc2mn+1
		mov		fpacc2mn+1,A
		clr		A
		addc	A,fpacc2mn
		mov		fpacc2mn,A
		jc		addnxt1			Jump and restore if overflow
		lcall	popf1			Restore fpacc1
		clr		A
		mov		B,A				Normal status
		ret
* Restore fpacc2 because of overflow
addnxt1 mov		A,fpacc1mn		Get upper byte
		mov		fpacc2mn,A		Restore into fpacc2
		mov		A,fpacc1mn+1	Get middle byte
		mov		fpacc2mn+1,A	Restore into fpacc2
		mov		A,fpacc1mn+2	Get lower byte
		mov		fpacc2mn+2,A	Restore into fpacc2
		lcall	popf1			Restore fpacc1
		clr		A
		mov		B,A
		inc		A				Overflow status
}

finflt()
{
	asm {
*
* Now finish conversion by multiplying the resultant mantissa
* by 10 for each positive power of exponent received or by .1
* for each negative power of exponent received
*
finish	lcall	pshf1			Save fpacc1
		lcall	mov2to1
		mov		A,#fpacc1ex		Check if fpacc1 is zero
		lcall	chck0
		jnz		finis3			Quit if it is
		mov		A,fpacc1ex
		mov		R2,A			Save power 10 exponent
		mov		A,#$80+24		Set up initial exponent (bias + # of bits)
		mov		fpacc1ex,A
		lcall	fpnorm			Go normalize mantissa
		mov		A,R2			Get power 10 exponent
		jz		finis3			Done if exponent is zero
		jnb		A.7,finis1		Exponent positive, multiply by 10
		mov		A,#constp1		Exponent negative, get constant .1
		mov		B,#=constp1
		lcall	getfpc2			Load fpacc2 with constant
		mov		A,R2
		cpl		A
		add		A,#1			Make exponent positive
		mov		R2,A
		ljmp	finis2
finis1	mov		A,#const10		Get constant 10
		mov		B,#=const10
		lcall	getfpc2			Load fpacc2 with constant
finis2	lcall	fltmul			Multiply fpacc1 by fpacc2, result in fpacc1
		dec		R2				Decrement exponent
		mov		A,R2
		jnz		finis2			Not done, continue
finis3	lcall	mov1to2
		lcall	popf1			Restore fpacc1
		clr		A				Initialize flags for next conversion
	}
	flagreg = 0;
	movfpac(fpacc2,fpaccB);
}

fpneg()			/*Check fpacc1 and return 1 if negative*/
{
	return mantsgn1 ? 1 : 0;
}

fpzero(arg)		/*Check arg and return 1 if zero*/
int arg;
{
	movfpac(arg,fpacc1);
	asm {
*
* Set A = 1 if fpacc pointed by A is zero
*
chck0	mov		R0,A			Move fpacc address into R0
chck03	mov		B,#0			Clear b for return
		mov		A,[R0]			Get exp
		jnz		chck01
		inc		R0
chck02	mov		A,[R0]			Get mantissa +0
		jnz		chck01
		inc		R0
		mov		A,[R0]			Get mantissa +1
		jnz		chck01
		inc		R0
		mov		A,[R0]			Get mantissa +2
		jnz		chck01
		inc		A				Zero, return 1
		ret
chck01	clr		A				Non-zero, return zero
	}
}

asm {
*
* Normalize fpacc1
*
fpnorm	mov		A,#fpacc1ex		Check if fpacc1 is zero
		lcall	chck0
		jnz		fpnorm3			Yes, just return
		mov		A,fpacc1mn
		jb		A.7,fpnorm3		Is it already normalized, just return
		clr		C
fpnorm1 dec		fpacc1ex		Decrement exponent with each shift
		mov		A,fpacc1ex
		jz		fpnorm4			Exponent went to 0, underflow
		mov		A,fpacc1mn+2	Shift mantissa
		rlc		A
		mov		fpacc1mn+2,A
		mov		A,fpacc1mn+1
		rlc		A
		mov		fpacc1mn+1,A
		mov		A,fpacc1mn
		rlc		A
		mov		fpacc1mn,A
		jnb		A.7,fpnorm1		Normalized?, if not keep shifting
fpnorm3 clr		C
		ret						No errors, return
fpnorm4 setb	C
		ret						Errors, return
*
constp1 DB $3E,$CC,$CC,$CE		0.1 decimal
const10 DB $42,$20,$00,$00		10.0 decimal
*
}

/* FPMULT: Floating Point Multiply
 *
 * This routine multiplies FPACC1 by FPACC2 and places the result
 * in FPACC1.	FPACC2 is not modified.
 *
 * R4-7 are used.
 */

fpmult(arg1,arg2)
int arg1, arg2;
{
	movfpac(arg1,fpacc1);
	movfpac(arg2,fpacc2);
	asm " LCALL fltmul";
	movfpac(fpacc1,fpaccB);
}


asm {
fltmul	mov		A,fpacc1ex
		jz		fpmult3			Fpacc1 is zero, answer is zero
		mov		A,fpacc2ex
		jnz		fpmult8			Fpacc2 is not zero, go multiply
fpmult3 clr		A				Fpacc2 is zero, answer is zero
		mov		mantsgn1,A
		mov		fpacc1ex,A
		mov		fpacc1mn,A
		mov		fpacc1mn+1,A
		mov		fpacc1mn+2,A
		ret
fpmult8 mov		A,mantsgn1		Get fpacc1 exponent
		xrl		A,mantsgn2		Compute the sign of result
		mov		mantsgn1,A		Save sign of result
		mov		A,fpacc1ex		Get fpacc1 exponent
		add		A,fpacc2ex		Compute the exponent
		jnb		A.7,fpmult1		Positive exponent
		jnc		fpmult2			Continue multiply
rtnmax	mov		error,#1		Overflow has occurred
		ljmp	rtnz			Fill fpacc and return
fpmult1 jc		fpmult2			If carry is set, then continue
rtnzero mov		error,#2		Underflow has occurred
rtnz	clr		A
		mov		mantsgn1,A
fpmult7 mov		fpacc1ex,A
		mov		fpacc1mn,A
		mov		fpacc1mn+1,A
		mov		fpacc1mn+2,A
		ret
fpmult2 add		A,#$80			Add 128 bias back
		mov		fpacc1ex,A		Save new exponent
		lcall	umult			Multiply the mantissas
		mov		A,R5			Get product MSB
		jb		A.7,fpround		Result normalized, go round result
		mov		A,R4			Result un-normalized
		rlc		A				Rotate mantissa left one bit
		mov		R4,A
		mov		A,R7
		rlc		A
		mov		R7,A
		mov		A,R6			Get NSB
		rlc		A
		mov		R6,A
		mov		A,R5			Get MSB
		rlc		A
		mov		R5,A
		dec		fpacc1ex		Decrement exponent
fpround mov		A,R4			Get byte 4
		rlc		A
		jnc		fpmult4			No rounding is required
		mov		A,#1
		add		A,R7			Get LSB
		mov		R7,A
		jnz		fpmult4			No carry propagate
		mov		A,#1
		add		A,R6			Get NSB
		mov		R6,A
		jnz		fpmult4			No carry propagate
		clr		A
		addc	A,R5			Get MSB
		mov		R5,A
		jnc		fpmult4			Overflow condition
		rrc		A				Change MSB to $80
		mov		R5,A
		inc		fpacc1ex		Increment exponent
fpmult4 mov		A,R7
		mov		fpacc1mn+2,A	Store LSB into fpacc1mn+2
		mov		A,R6
		mov		fpacc1mn+1,A	Store NSB into fpacc1mn+1
		mov		A,R5
		mov		fpacc1mn,A		Store MSB into fpacc1mn
		mov		A,fpacc1ex
		jz		rtnzero			Underflow, return error
		clr		A				No errors
		ret
*
* Multiply mantissas with
*	R7 = LSB
*	R6 = NSB
*	R5 = MSB
*	R4 = byte 4
*
umult	mov		A,fpacc2mn+2	Get multiplier LSB
		mov		B,fpacc1mn+2	Get multiplicand LSB
		mul
		mov		R7,B			Save byte 5, discard byte 6
		mov		A,fpacc2mn+1	Get multiplier NSB
		mov		B,fpacc1mn+2	Get multiplicand LSB
		mul
		add		A,R7			Add previous byte 5 result
		mov		R7,A			Save byte 5
		clr		A
		mov		A,B				Get byte 4
		addc	A,#0			Carry propogate from byte 5
		mov		R6,A			Save byte 4
		mov		A,fpacc2mn+2	Get multiplier LSB
		mov		B,fpacc1mn+1	Get multiplicand NSB
		mul
		add		A,R7			Add previous byte 5 result and discard
		mov		A,B				Get byte 4
		addc	A,R6			Add previous byte 4 result and carry from byte 5
		mov		R7,A			Save byte 4
		clr		A
		addc	A,#0			Carry propogate from byte 4
		mov		R6,A			Save byte 3
		mov		A,fpacc2mn		Get multiplier MSB
		mov		B,fpacc1mn+2	Get multiplicand LSB
		mul
		add		A,R7			Add previous byte 4 result
		mov		R7,A			Save byte 4
		mov		A,B				Get byte 3
		addc	A,R6			Add previous byte 3 result and carry from byte 4
		mov		R6,A			Save byte 3
		clr		A
		addc	A,#0			Carry propogate from byte 3
		mov		R5,A			Save byte 2
		mov		A,fpacc2mn+1	Get multiplier NSB
		mov		B,fpacc1mn+1	Get multiplicand NSM
		mul
		add		A,R7			Add previous byte 4 result
		mov		R7,A			Save byte 4
		mov		A,B				Get byte 3
		addc	A,R6			Add previous byte 3 result and carry from byte 4
		mov		R6,A			Save byte 3
		mov		A,R5			Get byte 2
		addc	A,#0			Carry propogate from byte 3
		mov		R5,A			Save byte 2
		mov		A,fpacc2mn+2	Get multiplier LSM
		mov		B,fpacc1mn		Get multiplicand MSB
		mul
		add		A,R7			Add previous byte 4 result
		mov		R4,A			Save byte 4 in R3
		mov		A,B				Get byte 3
		addc	A,R6			Add previous byte 3 result and carry from byte 3
		mov		R7,A			Save byte 3
		mov		A,R5			Get byte 2
		addc	A,#0			Carry propogate from byte 3
		mov		R6,A			Save byte 2
		mov		A,fpacc2mn		Get multiplier MSB
		mov		B,fpacc1mn+1	Get multiplicand NSB
		mul
		add		A,R7			Add previous byte 3 result
		mov		R7,A			Save byte 3
		mov		A,B				Get byte 2
		addc	A,R6			Add previous byte 2 result and carry from byte 3
		mov		R6,A			Save byte 2
		clr		A
		addc	A,#0			Carry propogate from byte 2
		mov		R5,A			Save byte 1
		mov		A,fpacc2mn+1	Get multiplier NSB
		mov		B,fpacc1mn		Get multiplicand MSB
		mul
		add		A,R7			Add previous byte 3 result
		mov		R7,A			Save byte 3
		mov		A,B				Get byte 2
		addc	A,R6			Add previous byte 2 result and carry from byte 3
		mov		R6,A			Save byte 2
		mov		A,R5			Get byte 1
		addc	A,#0			Carry propogate from byte 2
		mov		R5,A			Save byte 1
		mov		A,fpacc2mn		Get multiplier MSB
		mov		B,fpacc1mn		Get multiplicand MSB
		mul
		add		A,R6			Add previous byte 2 result
		mov		R6,A			Save byte 2
		mov		A,B				Get byte 1
		addc	A,R5			Add previous byte 1 result and carry propogate from byte 2
		mov		R5,A			Save byte 1
		ret
}

/* FPDIV: Floating Point Divide
 *
 * This routine divides FPACC1 by FPACC2 and places the result
 * in FPACC1. FPACC2 is not modified.
 */

fpdiv(arg1,arg2)
int arg1, arg2;
{
	movfpac(arg1,fpacc1);
	movfpac(arg2,fpacc2);
	asm " LCALL fltdiv";
	movfpac(fpacc1,fpaccB);
}

asm {
fltdiv	mov		A,mantsgn2		Get fpacc2 sign
		xrl		A,mantsgn1		Compute the sign of the result
		mov		mantsgn1,A		Save sign
		mov		A,fpacc2ex
		jnz		fltdiv1			The divisor is not zero
fltdiv5 ljmp	rtnmax			Return overflow error
fltdiv1 mov		A,fpacc1ex
		jnz		fltdiv2			The dividend is not zero
		ljmp	rtnz			Return zero
fltdiv2 clr		C
		mov		A,fpacc1ex
		subb	A,fpacc2ex		Compute exponent
		jnc		fltdiv8			Carry clear, result is positive
		jb		A.7,fltdiv6		Result in range
		ljmp	rtnzero			Return zero
fltdiv8 jb		A.7,fltdiv5		Overflow, return
fltdiv6 add		A,#$80			Put back exponent bias
		mov		fpacc1ex,A		Store quotient exponent
		lcall	fltcmpm			Compare mantissas
		jnc		fltdiv7			Fpacc1 > or = fpacc2, make fpacc1 smaller
		ajmp	fltdiv4			Fpacc1 < fpacc2
fltdiv7 mov		A,fpacc1mn		Divide fpacc1 by 2 to make smaller than fpacc2
		rrc		A
		mov		fpacc1mn,A
		mov		A,fpacc1mn+1
		rrc		A
		mov		fpacc1mn+1,A
		mov		A,fpacc1mn+2
		rrc		A
		mov		fpacc1mn+2,A
		inc		fpacc1ex		Increment exponent because of shift
		mov		A,fpacc1ex
		jnz		fltdiv4
		ljmp	rtnmax			Exponent overflow
fltdiv4 lcall	quocalc			Calculate MSB
		push	A				Save MSB
		lcall	quocalc			Calculate NSB
		push	A				Save NSB
		lcall	quocalc			Calculate LSB
		push	A				Save LSB
		lcall	quocalc			Calculate Round byte
		anl		A,#$F0			Mask off bottom byte
		clr		C
		jz		fltdiv9			Top nibble zero, dont round
		setb	C				Set carry to increment
fltdiv9 clr		A
		pop		A				Get LSB
		addc	A,#0
		mov		fpacc1mn+2,A
		pop		A				Get NSB
		addc	A,#0
		mov		fpacc1mn+1,A
		pop		A				Get MSB
		addc	A,#0
		mov		fpacc1mn,A
		ret
quocalc mov		R2,fpacc1mn		Save dividend
		mov		R1,fpacc1mn+1
		mov		R0,fpacc1mn+2
		mov		B,fpacc1mn
		mov		A,fpacc1mn+1
		mov		R4,#0
		mov		R3,fpacc2mn
		lcall	?udiv
		mov		R3,A			Save quotient byte
		mov		fpacc1mn,A		MSB of the quotient
		mov		fpacc1mn+1,#0
		mov		fpacc1mn+2,#0
		lcall	umult			Multiply quotient byte * divisor
		mov		fpacc1mn,?R2	Get dividend
		mov		fpacc1mn+1,?R1
		mov		fpacc1mn+2,?R0
		clr		C				Subtract product from dividend for the remainder
		mov		A,fpacc1mn+2
		subb	A,R7
		mov		R7,A
		mov		A,fpacc1mn+1
		subb	A,R6
		mov		R6,A
		mov		A,fpacc1mn
		subb	A,R5
		mov		R5,A
		jnc		fltdv12			Check for overflow, add the dividend to the remainder
		dec		R3				Decrement quotient
		mov		A,R7			Add .01h * dividend from remainder to correct
		add		A,fpacc2mn+1
		mov		R7,A
		mov		A,R6
		addc	A,fpacc2mn
		mov		R6,A
		mov		A,R5
		addc	A,#0
		mov		R5,A
fltdv12 mov		fpacc1mn+2,#0
		mov		fpacc1mn+1,R7
		mov		fpacc1mn,R6
		mov		A,R3
		ret
}

/* FPSUB: Floating Point Subtraction
 *
 * This routine subtracts FPACC1 and FPACC2 and places the result
 * in FPACC1.	FPACC2 is not modified
 *
 * R0-1 and R3-7 are used
 */

fpsub(arg1,arg2)
int arg1, arg2;
{
	movfpac(arg1,fpacc1);
	movfpac(arg2,fpacc2);
	asm {
fltsub	lcall	fltsub1			Invert sign
		lcall	fltadd			Do A floating add
fltsub1 mov		A,mantsgn2		Get fpacc2 sign
		cpl		A				Invert
		mov		mantsgn2,A		Store fpacc2 sign
	}
	movfpac(fpacc1,fpaccB);
}

/* FPADD: Floating Point Addition
 *
 * This routine adds FPACC1 and FPACC2 and places the result
 * in FPACC1.  FPACC2 is not modified
 *
 * R0-1 and R3-7 are used
 */

fpadd(arg1,arg2)
int arg1, arg2;
{
	movfpac(arg1,fpacc1);
	movfpac(arg2,fpacc2);
	asm " LCALL fltadd";
	movfpac(fpacc1,fpaccB);
}

asm {
fltadd	mov		A,mantsgn1		Load sign from fpacc1
		xrl		A,mantsgn2		0 = add, $FF = subtract
		mov		B,A				Add/subt flag in b
		clr		C
		mov		A,fpacc1ex
		subb	A,fpacc2ex		Compare exponents
		mov		R1,A			Save shift count in R1
		jc		fltadd2			Fpacc2 > Fpacc1
		subb	A,#24
		jnc		fltaddr			Exponent difference >= 24
		mov		R0,#fpacc2mn	Mantissa to be normalized
		mov		A,#fpacc1mn		Mantissa already normalized
		add		A,#2
		mov		R4,A			Point at LSB
		ljmp	fltadd3
fltadd1 mov		A,fpacc2ex		Move fpacc2 to fpacc1
		mov		fpacc1ex,A
		mov		A,fpacc2mn
		mov		fpacc1mn,A
		mov		A,fpacc2mn+1
		mov		fpacc1mn+1,A
		mov		A,fpacc2mn+2
		mov		fpacc1mn+2,A
		mov		A,mantsgn2
		mov		mantsgn1,A
fltaddr clr		A				Return normal status
		ret
fltadd2 cpl		A				Twos complement of shift count
		inc		A
		mov		R1,A			Save shift count in R1
		clr		C
		subb	A,#24
		jnc		fltadd1			Exponent difference >= 24
		mov		A,fpacc2ex		Move fpacc2ex to fpacc1ex
		mov		fpacc1ex,A
		mov		A,mantsgn2		Move sign2 to sign1
		mov		mantsgn1,A
		mov		R0,#fpacc1mn	Mantissa to be normalized
		mov		A,#fpacc2mn		Mantissa already normalized
		add		A,#2
		mov		R4,A			Point at LSB
fltadd3 mov		A,[R0]			Load MSB
		xrl		A,B				Complement to subtract
		mov		R5,A
		inc		R0
		mov		A,[R0]			Load NSB
		xrl		A,B				Complement
		mov		R6,A
		inc		R0
		mov		A,[R0]			Load LSB
		xrl		A,B				Complement
		mov		R7,A
		mov		A,R1			Get exponent difference
		anl		A,#$07			Remove bit shift
		mov		R3,A			Save in R3
		mov		A,R1			Get exponent difference
		anl		A,#$18			Remove byte shift
		rr		A				Right justify byte shift count
		rr		A
		rr		A
		mov		R1,A			Save byte shift count
fltadd4 mov		A,R1			Get byte shift count
		jz		fltadd5			Done, now bit shift
		mov		R7,?R6			Byte shift mantissa
		mov		R6,?R5
		mov		R5,B			Sign extend
		dec		R1				Decrement byte shift count
		ljmp	fltadd4
fltadd5 mov		A,R3			Get bit shift
		jz		fltadd6
		mov		A,B				Get sign
		rrc		A
		mov		A,R5			Get MSB
		rrc		A
		mov		R5,A
		mov		A,R6			Get NSB
		rrc		A
		mov		R6,A
		mov		A,R7			Get LSB
		rrc		A
		mov		R7,A
		dec		R3				Decrement exponent because of shift
		ljmp	fltadd5
fltadd6 mov		A,B				Get sign
		rrc		A
		mov		R0,?R4
		mov		A,R7			Get LSB
		addc	A,[R0]
		dec		R0
		mov		fpacc1mn+2,A
		mov		A,R6			Get NSB
		addc	A,[R0]
		dec		R0
		mov		fpacc1mn+1,A
		mov		A,R5			Get MSB
		addc	A,[R0]
		mov		fpacc1mn,A
		mov		A,B				Get sign
		jnz		fltadd7			Operation was subtraction
		jnc		fltadd8			Operation was addition with no carry
		mov		A,fpacc1mn		Get MSB
		rrc		A				Correct for overflow
		mov		fpacc1mn,A
		mov		A,fpacc1mn+1	Get NSB
		rrc		A
		mov		fpacc1mn+1,A
		mov		A,fpacc1mn+2	Get LSB
		rrc		A
		mov		fpacc1mn+2,A
		inc		fpacc1ex		Increment exponent because of shift
		mov		A,fpacc1ex
		jnz		fltadd8			No exponent overflow
		ljmp	rtnmax			Overflow, return
fltadd7 jc		fltadd9			Subtraction with overflow
		mov		A,mantsgn1		Twos complement the mantissa
		cpl		A				Complement sign
		mov		mantsgn1,A
		mov		A,fpacc1mn+2
		cpl		A
		inc		A
		mov		fpacc1mn+2,A	Negate LSB
		clr		A
		subb	A,fpacc1mn+1	Negate NSB
		mov		fpacc1mn+1,A
		clr		A
		subb	A,fpacc1mn		Negate MSB
		mov		fpacc1mn,A
fltadd9 mov		A,fpacc1mn
		jb		A.7,fltadd8		Already normalized
		jnz		fltad10			Not normalized, do bit shift
		setb	C				Not normalized, MSB=0, do byte shift
		mov		A,fpacc1ex
		subb	A,#8			Decrement exponent by 8 for byte shift
		jc		fltad12			Underflow *** may be zero result, return zero
		inc		A				Complete the subtract
		mov		fpacc1ex,A
		mov		A,fpacc1mn+1
		mov		fpacc1mn,A		Shift NSB to MSB
		mov		A,fpacc1mn+2
		mov		fpacc1mn+1,A	Shift LSB to NSB
		jnz		fltad15			Check for zero mantissa
		mov		A,fpacc1mn
		jz		fltad12			Zero mantissA, return
fltad15 clr		A
		mov		fpacc1mn+2,A	Clear LSB
		ljmp	fltadd9
fltadd8 clr		A				Return with no errors
		ret
fltad10 mov		A,fpacc1ex
		dec		A
		mov		fpacc1ex,A		Decrement exponent to accomodate shift
		jz		fltad11			Underflow, return
		clr		C
		mov		A,fpacc1mn+2
		rlc		A
		mov		fpacc1mn+2,A	Shift LSB one bit
		mov		A,fpacc1mn+1
		rlc		A
		mov		fpacc1mn+1,A	Shift NSB one bit
		mov		A,fpacc1mn
		rlc		A
		mov		fpacc1mn,A		Shift MSB one bit
		jnb		A.7,fltad10		Continue until normalized
		ljmp	fltadd8			Done, return
fltad12 ljmp	rtnz			Zero, return
fltad11 ljmp	rtnzero			Underflow, return
}

asm {
*
* Subroutine to get A floating point number stored in memory
* and put it into fpacc2
*
* A contains the memory pointer high address byte
* B contains the memory pointer low address byte
*
getfpc2 mov		R1,DPH
		mov		R0,DPL
		mov		DPL,A
		mov		DPH,B
		clr		A
		mov		mantsgn2,A		Set up for positive number
		movc	A,[A+DPTR]		Get exponent from memory
		rlc		A				Shift sign into carry
		rr		A				Shift back
		jnc		getfp21			Sign is positive
		mov		mantsgn2,#$FF	Set sign negative
getfp21 mov		B,A
		mov		A,#1
		movc	A,[A+DPTR]		Get MSB
		rlc		A				Shift bit 24 into carry
		xch		A,B				Get exponent
		rlc		A				Shift carry into exponent
		mov		fpacc2ex,A
		jz		getfp22			Exponent zero, dont set normalization bit
		setb	C
getfp22 mov		A,B				Get MSB
		rrc		A				Shift in normalization bit
		mov		fpacc2mn,A		Store MSB
		mov		A,#2
		movc	A,[A+DPTR]		Get NSB
		mov		fpacc2mn+1,A
		mov		A,#3
		movc	A,[A+DPTR]		Get LSB
		mov		fpacc2mn+2,A
		mov		DPH,R1			Restore DPTR
		mov		DPL,R0
		ret
}


pushfpac1()				/*Push fpacc1 on the stack*/
asm {
pshf1	mov		A,SP
		add		A,#4			Put return address above fpacc
		mov		R0,A
		pop		A
		mov		[R0],A			Get high byte return address
		pop		A
		dec		R0
		mov		[R0],A			Get low byte return address
		mov		A,mantsgn1
		rlc		A				Move sign bit into carry
		mov		A,fpacc1ex		Get exponent
		rrc		A				Shift carry into exponent, exponent lsb into carry
		push	A				Save exponent
		mov		A,fpacc1mn		Get MSB
		rl		A				Remove implied mantissa bit 23
		rrc		A				Shift carry into mantissa bit 23
		push	A				Save MSB
		mov		A,fpacc1mn+1	Get NSB
		push	A				Save NSB
		mov		A,fpacc1mn+2	Get LSB
		push	A				Save LSB
		inc		SP				Increment SP to return address
		inc		SP
}

pushfpac2() asm			/*Push fpacc2 on the stack*/
{
pshf2	mov		A,SP
		add		A,#4			Put return address above fpacc
		mov		R0,A
		pop		A
		mov		[R0],A			Get high byte return address
		pop		A
		dec		R0
		mov		[R0],A			Get low byte return address
		mov		A,mantsgn2
		rlc		A				Move sign bit into carry
		mov		A,fpacc2ex		Get exponent
		rrc		A				Shift carry into exponent, exponent lsb into carry
		push	A				Save exponent
		mov		A,fpacc2mn		Get MSB
		rl		A				Remove implied mantissa bit 23
		rrc		A				Shift carry into mantissa bit 23
		push	A				Save MSB
		mov		A,fpacc2mn+1	Get NSB
		push	A				Save NSB
		mov		A,fpacc2mn+2	Get LSB
		push	A				Save LSB
		inc		SP				Increment SP to return address
		inc		SP
}

pop_fpac1() asm			/*Pop fpacc1 from the stack*/
{
popf1	pop		A				Get return address
		mov		DPH,A
		pop		A
		mov		DPL,A
		mov		mantsgn1,#0		Set sign positive
		pop		A				Get LSB
		mov		fpacc1mn+2,A	Store LSB
		pop		A				Get NSB
		mov		fpacc1mn+1,A	Store NSB
		pop		A				Get MSB
		rlc		A				Shift bit 23 into carry
		mov		B,A				Save MSB
		pop		A				Get exponent
		rlc		A				Shift bit 23 into exponent and sign into carry
		mov		fpacc1ex,A		Save exponent
		jnc		popf_3			Sign is positive
		mov		mantsgn1,#$FF	Set sign negative
popf_3	clr		C
		jz		popf_4			Exponent zero, dont set normalization bit
		setb	C
popf_4	mov		A,B				Get MSB
		rrc		A
		mov		fpacc1mn,A		Save MSB
		clr		A
		jmp		[A+DPTR]		Jump to return address
}

pop_fpac2() asm				/*Pop fpacc2 from the stack*/
{
popf2	pop		A				Get return address
		mov		DPH,A
		pop		A
		mov		DPL,A
		mov		mantsgn2,#0		Set sign positive
		pop		A				Get LSB
		mov		fpacc2mn+2,A	Store LSB
		pop		A				Get NSB
		mov		fpacc2mn+1,A	Store NSB
		pop		A				Get MSB
		rlc		A				Shift bit 23 into carry
		mov		B,A				Save MSB
		pop		A				Get exponent
		rlc		A				Shift bit 23 into exponent and sign into carry
		mov		fpacc2ex,A		Save exponent
		jnc		popf_1			Sign is positive
		mov		mantsgn2,#$FF	Set sign negative
popf_1	clr		C
		jz		popf_2			Exponent zero, dont set normalization bit
		setb	C
popf_2	mov		A,B				Get MSB
		rrc		A
		mov		fpacc2mn,A		Save MSB
		clr		A
		jmp		[A+DPTR]		Jump to return address
}

asm {
*
* Move fpacc1 to fpacc2
*
mov1to2 mov		R0,#fpacc1ex
		mov		R1,#fpacc2ex
		sjmp	fpacmov
*
* Move fpacc2 to fpacc1
*
mov2to1 mov		R0,#fpacc2ex
		mov		R1,#fpacc1ex
fpacmov mov		R7,#5
fpacmo	mov		A,[R0]
		mov		[R1],A
		inc		R0
		inc		R1
		djnz	R7,fpacmo
		ret
}

movfpac(srcfpac,dstfpac) asm	/*Move source fpacc, fpreg or fpconst to*/
{								/*destination fpacc or fpreg*/
movfp	mov		R0,#-7
		lcall	?auto0
		mov		A,DPH			Save DPTR
		push	A
		mov		A,DPL
		push	A
		mov		?R7,[R0]		Get LSB address of source
		inc		R0
		mov		?R6,[R0]		Get MSB address of source
		mov		A,R6
		jnz		mov1			Source is A constant in external memory
		mov		A,R7
		cjne	A,#fpacc1ex,mov2 Address is not fpacc1
		ljmp	mov3
mov2	cjne	A,#fpacc2ex,mov4 Address is not fpacc2
mov3	mov		?R1,R7			Address is A fpacc
		mov		R5,#5
mov5	mov		A,[R1]			Move fpacc to stack
		inc		R1
		push	A
		djnz	R5,mov5
		ljmp	mov6			Jump to destination load
mov4	mov		?R5,#0			Set up R5 as the mantissa sign
		mov		?R1,R7			Address is A fp register
		inc		R1
		mov		A,[R1]			Get MSB
		rlc		A				Shift bit 23 into carry
		mov		B,A				Save MSB
		dec		R1
		mov		A,[R1]			Get exponent
		rlc		A				Shift bit 23 into exponent and sign into carry
		push	A				Push exponent
		jnc		mov7			Sign is positive
		mov		?R5,#$FF		Set sign negative
mov7	clr		C
		jz		mov8			Exponent zero, dont set normalization bit
		setb	C
mov8	mov		A,B				Get MSB
		rrc		A				Shift in normalization bit
		push	A				Push MSB
		inc		R1
		inc		R1
		mov		A,[R1]			Get NSB
		push	A
		inc		R1
		mov		A,[R1]			Get LSB
		push	A
		mov		A,R5			Get sign
		push	A
		ljmp	mov6			Jump to destination load
mov1	mov		?R5,#0			Set up R5 as the mantissa sign
		mov		DPH,R6			Address is in external memory
		mov		DPL,R7
		mov		A,#1
		movc	A,[A+DPTR]		Get MSB
		rlc		A				Shift bit 23 into carry
		mov		B,A				Save MSB
		clr		A
		movc	A,[A+DPTR]		Get exponent
		rlc		A				Shift bit 23 into exponent and sign into carry
		push	A				Push exponent
		jnc		mov12			Sign is positive
		mov		?R5,#$FF		Set sign negative
mov12	clr		C
		jz		mov13			Exponent zero, dont set normalization bit
		setb	C
mov13	mov		A,B				Get MSB
		rrc		A				Shift in normalization bit
		push	A				Push MSB
		mov		A,#2
		movc	A,[A+DPTR]		Get NSB
		push	A
		mov		A,#3
		movc	A,[A+DPTR]		Get LSB
		push	A
		mov		A,R5			Get sign
		push	A
mov6	inc		R0				Destination load
		mov		?R7,[R0]		Get LSB of destination
		mov		A,R7
		cjne	A,#fpacc1ex,mov9 Address is not fpacc1
		ljmp	mov10
mov9	cjne	A,#fpacc2ex,mov11 Address is not fpacc2
mov10	mov		A,R7			Address is A fpacc
		add		A,#4
		mov		R1,A
		pop		A
		mov		[R1],A			Store sign
		pop		A
		dec		R1
		mov		[R1],A			Store LSB
		pop		A
		dec		R1
		mov		[R1],A			Store NSB
		pop		A
		dec		R1
		mov		[R1],A			Store MSB
		pop		A
		dec		R1
		mov		[R1],A			Store exponent
		ljmp	mov14
mov11	mov		A,R7			Address is A fp register
		add		A,#3			Compute address of LSB
		mov		R1,A
		pop		A				Get sign
		mov		B,A				Save sign in b
		pop		A
		mov		[R1],A			Store LSB
		pop		A
		dec		R1
		mov		[R1],A			Store NSB
		pop		A				Get MSB
		xch		A,B				Get sign, save MSB
		rlc		A				Move sign bit into carry
		pop		A				Get exponent
		rrc		A				Shift sign into exponent, lsb into carry
		xch		A,B				Get MSB, save exponent
		rl		A				Remove implied mantissa bit 23
		rrc		A				Shift carry into bit 23
		dec		R1
		mov		[R1],A			Store MSB
		mov		A,B				Get exponent
		dec		R1
		mov		[R1],A			Store exponent
mov14	pop		A				Restore DPTR
		mov		DPL,A
		pop		A
		mov		DPH,A
		mov		B,#0
		mov		A,R7			Return destination fpacc address
}
