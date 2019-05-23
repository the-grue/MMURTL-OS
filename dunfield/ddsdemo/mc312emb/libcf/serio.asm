*
* Write a string to the console: putstr(string)
*
putstr	LDI	2,S		Get argument
?putstr	LDB	I		Get char
	SJZ	?2		Exit
	LEAI	1,I		Advance
	PUSHA			Pass parm
	CALL	?putch		Output it
	LD	S+		Clean stack
	SJMP	?putstr		And proceed
*
* Output character to console with newline translation: putch(c)
*
putch	LD	2,S		Get character
?putch	CMP	#$0A		Newline?
	SJZ	putchr		No, don't process
	LDB	#$0D		Get RETURN
	CALL	?putchr		Output it
*
* Output 'raw' character to console: putchr(c)
*
putchr	LD	2,S		Get character
?putchr	OUT	0		Write to console
	RET
*
* Read a character from console with newline translation: getch()
*
getch	CALL	chkch		Test for character ready
	SJZ	getch		Wait for it
	RET
*
* Read 'raw' character from console: getchr()
*
getchr	IN	0		Read console port
	INC			Test for character
	SJZ	getchr		Wait for character
	DEC			Restore proper character
	RET
*
* Check for character from the console with translation: chkch()
*
chkch	IN	0		Read port 0
	INC			Test for char
	SJZ	?2		None, exit
	DEC			Restore proper char
	PUSHA			Save for later
	CMPB	#$0D		RETURN?
	SJZ	?1		No, its OK
	LD	S+		Clean stack
	LDB	#$0A		Get NEWLINE
	RET
?1	LD	S+		Get char back
?2	RET
*
* Check for 'raw' character from the console: chkchr()
*
chkchr	IN	0		Read port 0
	RET
*
* Get string from console (with editing): getstr(string, size)
*
getstr	CLR			Zero length
	PUSHA			Save on stack
?3	CALL	getchr		Get a character
	LDI	#?4		Point to table
	SWITCH			Dispatch
?4	FDB	?6,$08		Backspace
	FDB	?6,$7F		Delete
	FDB	?7,$0D		Return
	FDB	0,?5		Default
* Normal character
?5	STB	?temp		Save for later
	LD	,S		Get current position
	CMP	4,S		Are we OK
	UGE			Test for over
	SJNZ	?3		Ignore
	LDI	6,S		Get pointer
	LD	S+		Get size
	ADAI			Adjust pointer
	INC			Advance count
	PUSHA			Resave
	LDB	?temp		Get char back
	STB	I		Save in buffer
	CALL	?putchr		Echo it
	SJMP	?3		And proceed
* Backspace & delete
?6	LD	,S		Get count
	SJZ	?3		At zero, ignore
	DEC			Backup
	ST	,S		Resave
	LDI	#?8		Point to message
	CALL	?putstr		Output
	SJMP	?3		And proceed
* RETURN, terminate entry
?7	LDI	6,S		Get buffer pointer
	LD	S+		Get count
	ADAI			Adjust pointer
	CLR			Get zero
	STB	I		Terminate buffer
	LDB	#$0A		Newline
	SJMP	?putchr		Output
?8	FCB	$08,' ',$08,0
