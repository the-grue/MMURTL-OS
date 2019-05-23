*
* Set a long jump: setjmp(int jaddr[2])
*
setjmp	LDI	S+		Get return address
	TSA			Get stack pointer
	PUSHI			Resave return
	LDI	2,S		Get buffer
	ST	I		Write SP
	LD	,S		Get return address
	ST	2,I		Write to buffer
	CLR			Return zero
	RET
*
* Perform a long jump: longjmp(int jaddr[2], int rc)
*
longjmp	LD	2,S		Get value
	ST	?temp		Save for later
	LDI	4,S		Get buffer
	LD	I		Get SP
	LDI	2,I		Get RETADDR
	TAS			Set SP
	PUSHI			Save ret address
	LD	?temp		Restore value
	RET
