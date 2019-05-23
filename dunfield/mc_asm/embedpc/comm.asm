*
* Low level ASYNC serial I/O driver for
* use with DDS MICRO-C compiler on IBM/PC.
*
* Copyright 1990-1995 Dave Dunfield
* All rights reserved.
*
* Misc constants.
?BUFMSK	EQU		$00FF			Buffer size mask
?WINDOW	EQU		256-30			Flow control assertion window
?RXRDY	EQU		%00000001		Uart Receiver ready flag
?TXRDY	EQU		%00100000		Uart Transmitter ready flag
?8259	EQU		$0020			8259 interrupt controller
* Bits in driver control flags
?RFLOW	EQU		%10000000		Received flow control
?TFLOW	EQU		%01000000		Transmit flow control
?TXOFF	EQU		%00100000		Transmit XOFF pending
?TXON	EQU		%00010000		Transmit XON  pending
?XPAREN	EQU		%00001000		Transparency enabled
*
$SE:1
*
* Initialized variables & tables
*
Cflags	DB		0				Control flags
?comvec	DW		0				Comm interrupt vector
*
$DD:?rdptr	2					Buffer read pointer
$DD:?wrptr	2					Buffer write pointer
$DD:?comadr	2					Com port address
$DD:?oldoff	2					Saved old interrupt offset
$DD:?oldseg	2					Saved old interrupt segment
$DD:?buffer	256					Receive buffer
*
$SE:0
*
* Open the com port: Copen(int port, int speed, char mode, char modem)
*
Copen	PUSH	BP				Save callers stack
		MOV		BP,SP			Address parameters
		CALL	Cclose			Insure its closed
		MOV		AX,10[BP]		Get com port
		MOV		DX,#$03FD		Comm 1 address
		MOV		BX,#$0030		Comm 1 interrupt vector
		MOV		CL,#$0EF		Comm 1 interrupt mask
		DEC		AX				Is it com1?
		JZ		?2				Yes, it is
		MOV		DX,#$02FD		Comm 2 address
		MOV		BX,#$002C		Comm 2 interrupt vector
		MOV		CL,#$0F7		Comm 2 interrupt mask
		DEC		AX				Is it com2?
		JZ		?2				Yes it is
* Report failure to open port
?1		STI						Insure interrupts enabled
		MOV		AX,#-1			Indicate failure
		POP		BP				Restore caller
		RET
* Proceed with opening the comm port
?2		MOV		?comadr,DX		Save address
		CLI						Inhibit interrupts
* Setup the uart
		DEC		DX				Backup ...
		DEC		DX				to line control register (FB)
		IN		AL,DX			Read current value
		OR		AL,#$80			Enable baud rate register
		OUT		DX,AL			Write it
		MOV		AX,8[BP]		Get baud rate
		SUB		DX,#3			Point to baud rate LSB (F8)
		OUT		DX,AL			Write it
		INC		DX				Advance to MSB (F9)
		MOV		AL,AH			Get MSB
		OUT		DX,AL			Write it
		DEC		DX				Backup to LSB (F8)
		IN		AL,DX			Re-read LSB
		MOV		AH,AL			Copy for later
		INC		DX				Back to MSB (F9)
		IN		AL,DX			Re-read MSB
		XCHG	AH,AL			Swap for multi
		CMP		AX,8[BP]		Does it match
		JNZ		?1				No, its dosn't
		MOV		AL,6[BP]		Get mode
		AND		AL,#$7F			Insure no baud rate
		INC		DX				Advance...
		INC		DX				to line control register (FB)
		OUT		DX,AL			Write it
		MOV		AL,#$01			Receive interrupt only
		DEC		DX				Backup ...
		DEC		DX				to Interrupt enable register (F9)
		OUT		DX,AL			Write it
		MOV		AL,4[BP]		Get modem control
		ADD		DX,#3			Point to modem control register (FC)
		OUT		DX,AL			Write it
* Setup the interrupt vector
		MOV		?comvec,BX		Save interrupt vector
		MOV		CS:?dsval,DS	Save data segment for int handler
		XOR		AX,AX			Get a zero
		MOV		?rdptr,AX		Zero read pointer
		MOV		?wrptr,AX		Zero write pointer
		MOV		AX,ES:[BX]		Get old offset
		MOV		?oldoff,AX		Save old offset
		MOV		AX,ES:2[BX]		Get old segmemt
		MOV		?oldseg,AX		Save segment
		MOV		AX,#?COMINT		Point to routine
		MOV		ES:[BX],AX		Write new offset
		MOV		ES:2[BX],CS		Write new segment
* Clear out any pending characters
		SUB		DX,#4			Point to data register (F8)
		IN		AL,DX			Read to clear interrupt
		IN		AL,DX			Read to clear interrupt
* Setup the interrupt controller
		IN		AL,?8259+1		Read interrupt mask
		AND		AL,CL			Enable serial port
		OUT		?8259+1,AL		Write interrupt controller
		STI						Re-enable interrupts
		XOR		AX,AX			Indicate success
		POP		BP				Restore caller
		RET
*
* Close the comm port: Cclose()
*
Cclose	XOR		AX,AX			Get zero
		MOV		ES,AX			Point to interrupt vectors
		MOV		BX,?comvec		Get old vector
		AND		BX,BX			Is it set?
		JZ		?3				No, its not
		MOV		?comvec,AX		Indicate not set
		CLI						Disable interrupts
* Restore the old comm interrupt vector
		MOV		DX,?oldoff		Get old offset
		MOV		ES:[BX],DX		Restore old offset
		MOV		DX,?oldseg		Get old segment
		MOV		ES:2[BX],DX		Restore old segment
* Disable interrupts on the uart
		MOV		DX,?comadr		Get uart address
		SUB		DX,#4			Point to interrupt enable register
		OUT		DX,AL			Write zero
		IN		AL,?8259+1		Read interrupt mask
		OR		AL,#$18			Disable comm interrupts
		OUT		?8259+1,AL		Write interrupt mask
		STI						Re-enable interrupts
?3		RET
*
* Test for char from com port: int Ctestc()
*
Ctestc	MOV		BX,?rdptr		Get read pointer
		CMP		BX,?wrptr		Test for data in buffer
		JNZ		?4				Yes, we have some
		MOV		AX,#-1			Report no data available
		RET
*
* Read a character from the comport: int Cgetc()
*
Cgetc	MOV		BX,?rdptr		Get read pointer
		CMP		BX,?wrptr		Test for data in buffer
		JZ		Cgetc			No characters, wait for them
* Read character from com port
?4		MOV		DI,#?buffer		Get I/O buffer address
		MOV		AL,[BX+DI]		Get character from buffer
		XOR		AH,AH			Zero high
		INC		BX				Advance read pointer
		AND		BX,#?BUFMSK		Mask for buffer wrap
		MOV		?rdptr,BX		Resave read pointer
		CMP		BX,?wrptr		Did we just empty buffer?
		JNZ		?3				No, its ok
		PUSH	AX				Save for later
* Buffer is empty, send XON if necessary
		MOV		DX,?comadr		Point to com port
		CLI						No interrupts
		MOV		AH,Cflags		Get control flags
		TEST	AH,#?TFLOW		Flow controlled?
		JZ		?7				No, its not
		AND		AH,#~(?TFLOW|?TXOFF|?TXON) Clear flags
		IN		AL,DX			Get status
		TEST	AL,#?TXRDY		Ok to send?
		JZ		?5				No, set pending
		SUB		DX,#5			Backup to data port
		MOV		AL,#'Q'-$40		Get XON character
		OUT		DX,AL			Send the XON
		JMP		<?6				And continue
* Cannot send not, set pending flag
?5		OR		AH,#?TXON		Set XON pending flag
?6		MOV		Cflags,AH		Resave the flags
?7		STI						Re-enable interrupts
		POP		AX				Restore character
		RET
*
* Write a character to the com port: Cputc(char c)
*
Cputc	PUSH	BP				Save callers stack frame
		MOV		BP,SP			Address parameters
?8		MOV		DX,?comadr		Get address of uart
		IN		AL,DX			Read uart status
		TEST	AL,#?TXRDY		Ok to transmit
		JZ		?8				No, wait for it
		SUB		DX,#5			Position to data address
		CLI						Disallow interrupts
		MOV		AH,Cflags		Get control flags
* Test for pending XOFF to send
		TEST	AH,#?TXOFF		Transmit XOFF?
		JZ		?9				No, try next
		MOV		AL,#'S'-$40		Get XOFF
		AND		AH,#~?TXOFF		Clear the bit
		JMP		<?10			Write to comm port
* Test for pending XON to send
?9		TEST	AH,#?TXON		Transmit XON?
		JZ		?11				No, output character
		MOV		AL,#'Q'-$40		Get XON
		AND		AH,#~?TXON		Clear the bit
* Resave the control flags & proceed
?10		MOV		Cflags,AH		Re-save control flags
		STI						Re-enable interrupts
		OUT		DX,AL			Write character
		JMP		<?8				And proceed
* No pending flow control, output data
?11		STI						Re-enable interrupts
		TEST	AH,#?RFLOW		Output inhibited?
		JNZ		?8				Yes, wait for it
		MOV		AL,4[BP]		Get character
		OUT		DX,AL			Write to comm port
		POP		BP				Restore caller
		RET
*
* Read the com port signals: int Csignals()
*
Csignals MOV	DX,?comadr		Get the com port address
		INC		DX				Advance to modem status register
		IN		AL,DX			Read modem status
		XOR		AH,AH			Zero high bits
		RET
*
* Comms Interrupt handler
*
?COMINT	PUSH	AX				Save AX
		PUSH	BX				Save BX
		PUSH	DX				Save DX
		PUSH	DI				Save DI
		PUSH	DS				Save DS
		MOV		DS,CS:?dsval		Get data segment
		MOV		DX,?comadr		Get com port I/O address
		IN		AL,DX			Read uart status register
		TEST	AL,#?RXRDY		Receiver ready?
		JZ		?15				No, Spurious interrupt
		SUB		DX,#5			Backup to data port
		MOV		AH,Cflags		Get comm flags
		IN		AL,DX			Read data character
		TEST	AH,#?XPAREN		Are we transparent?
		JNZ		?13				Yes, do not interpret flow control
* Test for XOFF, inhibit output
		CMP		AL,#'S'-$40		Is it XOFF?
		JNZ		?12				No, try next
		OR		AH,#?RFLOW		Set flow control bit
		JMP		<?14			and continue
* Test for XON, enable output
?12		CMP		AL,#'Q'-$40		Is it XON
		JNZ		?13				No, its not
		AND		AH,#~?RFLOW		Reset flow control bit
		JMP		<?14			and continue
* Normal character, stuff in buffer
?13		MOV		DI,#?buffer		Get I/O buffer address
		MOV		BX,?wrptr		Get write pointer
		MOV		[BX+DI],AL		Write into buffer
		INC		BX				Advance
		AND		BX,#?BUFMSK		Mask for buffer wrap
		MOV		?wrptr,BX		Resave pointer
* Test for nearing end of buffer
		SUB		BX,?rdptr		Calculate size of buffer
		AND		BX,#?BUFMSK		Mask for buffer wrap
		CMP		BX,#?WINDOW		Are we nearing end
		JB		?15				No, its ok
		TEST	AH,#?XPAREN		Are we transparent?
		JNZ		?15				Don't send flow ctrl
* Send XOFF, flow control dest
		OR		AH,(?TFLOW|?TXOFF) Indicate flow control asserted
		AND		AH,#~?TXON		Insure no XON pending
		ADD		DX,#5			Offset to status register
		IN		AL,DX			Read status
		TEST	AL,#?TXRDY		Test for transmitter ready
		JZ		?14				Not ready
		SUB		DX,#5			Backup to data port
		MOV		AL,#'S'-$40		Get XOFF character
		OUT		DX,AL			Write to port
		AND		AH,#~?TXOFF		No pending XOFF needed
* Resave status flags to record changes
?14		MOV		Cflags,AH		Resave flags
* Reset 8259, Restore registers & return from interrupt
?15		MOV		AL,#$20			End of Interrupt command
		OUT		?8259,AL		Write to interrupt controller
		POP		DS				Restore DS
		POP		DI				Restore DI
		POP		DX				Restore DX
		POP		BX				Restore BX
		POP		AX				Restore AX
		IRET
* Saved data segment incase we are running in small model
?dsval	DW		0				Stored code segment value
