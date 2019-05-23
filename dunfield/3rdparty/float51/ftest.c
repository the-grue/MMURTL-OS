/*
 * A primitive floating point calculator to demonstrate
 * the 8051 floating point library.
 */
#include c:\mc\8051io.h
#include c:\mc\8051reg.h
#include float.h

register char key, operation;

/*
 * Main calculator program
 */
main()
{
	putstr("\nFloating point calculator\n");
	flagreg = error = operation = 0;
	clrfpac(fpacc2);
	clrfpac(fpacc1);
	for(;;) switch(key = getch()) {
		case '+':
		case '-':
		case '*':
		case '/':
			putch(key);
			if(flagreg & NUMENTER) {
				finflt();
				putstr("\n");
				movfpac(fpaccB,fpacc2);
				gocalc(operation);
				movfpac(fpaccB,fpacc1);
				clrfpac(fpacc2); }
			operation=key;
			break;
		case '=':
			if(flagreg & NUMENTER)
				finflt();
			putstr("\n");
			gocalc(operation);
			movfpac(fpaccB,fpacc1);
			operation=0;
			break;
		case 'c':
			flagreg = error = operation = 0;
			clrfpac(fpacc2);
			clrfpac(fpacc1);
			fltasc();
			putstr("\n");
			break;
		default:
			putch(key);
			buildnum(key); }
}

/*
 * Perform a floating point operation
 */
gocalc(arg)
int arg;
{
	switch(arg) {
		case '+':
			fpadd(fpacc1,fpacc2);
			break;
		case '-':
			fpsub(fpacc1,fpacc2);
			break;
		case '*':
			fpmult(fpacc1,fpacc2);
			break;
		case '/':
			fpdiv(fpacc1,fpacc2);
			break;
		case 0:
			movfpac(fpaccB,fpacc1); }

	clrfpac(fpacc2);
	if(operation) {
		movfpac(fpaccB,fpacc1);
		putch('=');
		fltasc();
		putstr("\n"); }
}

/*
 * Print the contents of a floating point accumulator
 */
putfpac(fpacc)
asm {
		mov		A,#' '
		lcall	?putch
		mov		R0,#-5
		lcall	?auto0
		mov		A,[R0]			Get LSB address of source
		cjne	A,#fpacc1ex,prn3 Address is not fpacc1
		sjmp	prn2			Address is fpacc1
prn3	cjne	A,#fpacc2ex,prn4 Address is not fpacc2
		sjmp	prn2			Address is fpacc2
prn4	mov		R1,A			Print fpreg
		mov		R0,#4
prn6	mov		A,[R1]
		lcall	printb
		inc		R1
		djnz	R0,prn6			Print 4 bytes
		sjmp	prn5
prn2	mov		R2,A			Print fpacc
		add		A,#4
		mov		R1,A
		mov		A,[R1]			Get sign
		rlc		A				Rotate sign into carry
		mov		?R1,R2
		mov		A,[R1]			Get exponent
		rrc		A				Shift carry into exponent, exponent lsb into carry
		lcall	printb			Print exponent
		mov		A,[R1]			Get exponent lsb again because carry has been distroyed
		rrc		A				Rotate sign into carry
		inc		R1
		mov		A,[R1]			Get MSB
		rl		A				Removed inplied mantissa bit 24
		rrc		A				Shift carry into mantissa bit 24
		lcall	printb			Print MSB
		inc		R1
		mov		A,[R1]			Get NSB
		lcall	printb			Print NSB
		inc		R1
		mov		A,[R1]			Get LSB
		lcall	printb			Print LSB
prn5	ret
printb	mov		R7,A			Print byte, save byte in R7
		swap	A
		lcall	printd
		mov		A,R7
		lcall	printd
		ret
printd	anl		A,#$0F
		orl		A,#$30
		mov		B,A
		clr		C
		subb	A,#$3A
		mov		A,B
		jc		pri1
		add		A,#7
pri1	lcall	?putch
		ret
}
