/*
 * Interrupt Serial I/O functions for the 8051:
 *
 * These functions perform buffered serial I/O via interrupts.
 *
 * The 8051 hardware generates an interrupt (if enabled) whenever
 * the RI or TI flags are set in the SCON register: INTERRUPT = TI | RI;
 *
 * The RI flag (and interrupt) occurs whwnever a character is detected
 * and received by the UART. The TI flag (and interrupt) occur whenever
 * the transmitter finishes sending a character and is ready for the
 * next. Both RI and TI must be cleared manually by the program in order
 * to "turn off" the interrupt.
 *
 * Receiving data:
 * ---------------
 * When the character is received, an interrupt is generated. The interrupt
 * handler checks the RI flag, and finding a character present, clears the
 * TI flag, and stores the character in a circular buffer.
 *
 * The Chkch() function when called by the application program checks the
 * Read/Write pointers to the receive buffer, and if it finds them to be
 * different, removes the oldest character from the buffer and returns it.
 * If the pointers are the same (buffer is empty), a -1 is returned.
 *
 * Transmitting data:
 * -----------------
 * The T8 bit (unused in MODE 1) is used to record the condition of the
 * transmitter. It is set to one when a character is sent, and set to
 * zero when no character is available. This is required so that we can
 * know when the transmitter has to be re-started. We cannot use the
 * TI bit directly, since it must be kept clear to avoid continuous
 * interrupts.
 *
 * When Putch() is called, it first checks to insure that there is
 * available space in the transmitter output buffer. If there is not,
 * it enters a loop, waiting for it. The next transmit interrupt will
 * remove a character from the buffer, making room for the new one.
 * (Note, a multi-tasking kernal etc. should "swap" at this point, to
 * allow another task to run while waiting for TX buffer space).
 * If space is available, the character is inserted into the queue.
 * The T8 flag is tested, and if clear, the TI bit is manually set to
 * cause an interrupt. This is required since TI was cleared and no
 * character loaded the last time. If T8 is NOT set, we assume that
 * a character is being transmitted, and will generate an interrupt
 * when it is finished.
 *
 * When the interrupt occurs, the interrupt handler notices the TI bit
 * set, clears it, loads the next character from the output buffer into
 * the UART, and sets T8 so we know another interrupt will occur. If no
 * data is available, the T8 bit is cleared, so that we know to "kick"
 * the transmitter when the next character is sent.
 *
 * Compile with the EXTENDED PRE-PROCESSOR ('-p' option), in any memory model.
 *
 * Dave Dunfield - Oct 3, 1994
 */
#include <8051io.h>		/* 8051 I/O definitions */
#include <8051reg.h>	/* 8051 SFR definitions */

#define	INTBASE	$0800	/* Base address for interrupt vector table */
#define	SRSIZE	8		/* Size of RX buffer */
#define	STSIZE	8		/* Size of TX buffer */

/*
 * Serial I/O buffers in internal memory
 */
register char SRbuf[SRSIZE], SRwp, SRrp;	/* RX buffer & pointers */
register char STbuf[STSIZE], STwp, STrp;	/* TX buffer & pointers */

/*
 * Main program... Display a "welcome" message, and echo input to output
 */
main()
{
	int a;

	SRwp = SRrp = SRbuf;		/* Initialize RX buffer to EMPTY */
	STwp = STrp = STbuf;		/* Initialize TX buffer to EMPTY */
	IE |= 0x90;					/* Enable serial interrupt */
	SCON &= 0xF7;				/* Clear T8 to force "kick" */

	Putstr("This is a test: ");

	for(;;)						/* Loop to echo characters */
		if((a = Chkch()) != -1)
			Putch(a);
}

/*
 * Write a string to the serial port
 */
Putstr(char *string)
{
	while(*string)
		Putch(*string++);
}

/*
 * Write a character to the serial port output buffer
 */
Putch() asm
{
		MOV		A,STwp			Get TX write pointer
		MOV		R1,A			Save for later
		INC		A				Advance pointer
		CJNE	A,#STbuf+STSIZE,?PU1 Buffer has wrapped
		MOV		A,#STbuf		Reset to start
?PU1	CJNE	A,STrp,?PU2		Buffer is full, wait
		SJMP	?PU1			Keep trying
?PU2	MOV		R2,A			Save
		MOV		R0,#-5			Offset to parameter
		LCALL	?auto0			Set up address
		MOV		A,[R0]			Get char to write
		MOV		[R1],A			Save in buffer
		MOV		STwp,R2			Set new pointer
		JB		SCON.3,?PU3		Transmitter is already running
		SETB	SCON.1			give it a kick-start
?PU3	EQU		*
}

/*
 * Test for a character received from the serial port
 */
Chkch() asm
{
		MOV		A,SRrp			Get RX read pointer
		CJNE	A,SRwp,?CH1		Any data pending?
		MOV		A,#-1			No, return -1
		MOV		B,A				Full 16 bit int
		RET
?CH1	MOV		R0,A			Set up pointer
		MOV		A,[R0]			Get character under pointer
		INC		R0				Advance to next
		CJNE	R0,#SRbuf+SRSIZE,?CH2 Buffer overflow
		MOV		R0,#SRbuf		Reset pointer
?CH2	MOV		SRrp,R0			Save new pointer
		MOV		B,#0			Zero high
}

/*
 * Serial interrupt handler
 */
asm {
* Take over interrupt vector, save registers & jump to handler
?SI1	EQU		*				Record code address
		ORG		INTBASE+$23		Position to serial I/O interrupt
		PUSH	PSW				Save flags
		PUSH	A				Save ACC
		LJMP	?SI1			Enter handler
		ORG		?SI1			Restore program counter
		PUSH	?R0				Save R0
* Test for character received & buffer if found
		JNB		SCON.0,?SI3		No data waiting
		MOV		R0,SRwp			Get RX write pointer
		CLR		SCON.0			Indicate data RXed
		MOV		[R0],SBUF		Write to buffer
		INC		R0				Advance
		CJNE	R0,#SRbuf+SRSIZE,?SI2 Buffer overflow
		MOV		R0,#SRbuf		Reset buffer pointer
?SI2	MOV		SRwp,R0			Resave RX write pointer
* Check for character waiting to transmit
?SI3	JNB		SCON.1,?SI3A	No data to transmit
		CLR		SCON.1			Terminate receiver
		MOV		A,STrp			Get TX read pointer
		CJNE	A,STwp,?SI4		Data to send?
		CLR		SCON.3			Shut down transmitter
* Restore registers & exit interrupt
?SI3A	POP		?R0				Restore R0
		POP		A				Restore ACC
		POP		PSW				Restore PSW
		RETI
* Character was found - remove from buffer & send
?SI4	MOV		R0,A			Set pointer
		MOV		SBUF,[R0]		Get data
		INC		R0				Advance pointer
		CJNE	R0,#STbuf+STSIZE,?SI5 Buffer overflow
		MOV		R0,#STbuf		Reset pointer
?SI5	MOV		STrp,R0			Resave TX write pointer
		SETB	SCON.3			Indicate transmitter in use
		SJMP	?SI3A			And exit
}
