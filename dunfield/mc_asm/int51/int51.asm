*
* This program sets up timer0, to generate regular interrupts, which are
* user to increment a counter in R7. The main program monitors that counter,
* and sends a derived (printable) value to the serial port any time that
* it changes.
*
* For testing under MONICA on our BD52, set CODE to $0800. For use
* under MON51, set CODE to the USERRAM address defined in MON51
*
CODE	EQU	$0800		Place code in memory here
*
COUNT	EQU	61440		Count value for timer
* Position to start of ROM, and code a jump to the main program
	ORG	CODE		Program in ROM
	SJMP	PROG		Jump to program
* Interrupt handler - Reset count & increment R7
	ORG	CODE+11		Timer0 interrupt handler
T0INT	MOV	TH0,#=-COUNT	Reset timer0 high count
	MOV	TL0,#-COUNT	Reset timer0 low count
	INC	R7		Advance count
	RETI
* Main program: Initialize the timer0 as 16 bit timer. For MON51, keep
* timer1 (baud rate generator) running as 8 bit auto-reload.
PROG	MOV	TMOD,#%00100001	T1=8 bit auto-reload, T0=16 bit cascade
	MOV	TH0,#=-COUNT	Set timer 0 high count
	MOV	TL0,#=COUNT	Set timer 0 low count
	MOV	TCON,#%01010000	Run 1, Run 0
	MOV	IE,#%10000010	Enable TIMER0 interrupt
* Main program, loop checking for change in state of counter
* Output character if count changes.
loop	MOV	A,R7		Get state
	CJNE	A,6,loop1	It has changed
	SJMP	LOOP		Keep looking
* Change detected, output character to serial port
loop1	MOV	R6,A		Save new state
	ANL	A,#%00001111	Save last 4 bits
	ADD	A,#'A'		Convert to ASCII
	JNB	SCON.1,*	Wait for TX ready
	CLR	SCON.1		Indicte we are sending
	MOV	SBUF,A		Write out character
	SJMP	loop		And continue to wait
