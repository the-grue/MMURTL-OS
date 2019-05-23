*
* "Bit-bang" serial I/O functions for the 8051.
*
* These routines transmit and receive serial data using two general
* I/O pins, in 8 bit, No parity, 1 stop bit format. They are useful
* for performing serial I/O on 8051 derivatives not having an
* internal UART, or for implementing a second serial channel.
*
* Dave Dunfield - May 17, 1994
*
* NOTE that R0 and R1 are used by the functions. You may wish to
* add PUSH/POP instructions to save/restore these registers.
*
TXD	EQU	P1.0		Transmit on this pin
RXD	EQU	P1.1		Receive on this pin
* The serial baud rate is determined by the processor crystal, and
* this constant which is calculated as: (((crystal/baud)/12) - 5) / 2
BITTIM	EQU	45		(((11059200/9600)/12) - 5) / 2
*
* Transmit character in A via TXD line
*
putc	CLR	TXD		Drop line for start bit
	MOV	R0,#BITTIM	Wait full bit-time
	DJNZ	R0,*		For START bit
	MOV	R1,#8		Send 8 bits
putc1	RRC	A		Move next bit into carry
	MOV	TXD,C		Write next bit
	MOV	R0,#BITTIM	Wait full bit-time
	DJNZ	R0,*		For DATA bit
	DJNZ	R1,putc1	write 8 bits
	SETB	TXD		Set line high
	RRC	A		Restore ACC contents
	MOV	R0,#BITTIM	Wait full bit-time
	DJNZ	R0,*		For STOP bit
	RET
*
* Receive a character from the RXD line and return in A
*
getc	JB	RXD,*		Wait for start bit
	MOV	R0,#BITTIM/2	Wait 1/2 bit-time
	DJNZ	R0,*		To sample in middle
	JB	RXD,getc	Insure valid
	MOV	R1,#8		Read 8 bits
getc1	MOV	R0,#BITTIM	Wait full bit-time
	DJNZ	R0,*		For DATA bit
	MOV	C,RXD		Read bit
	RRC	A		Shift it into ACC
	DJNZ	R1,getc1	read 8 bits
	RET			go home
