*
* Test file to verify instructions and addresing modes of: 8051
*
EXT	EQU	$0123
DIR	EQU	$45
IMM8	EQU	$67
IMM16	EQU	$6789
*
	ACALL	EXT
*
	ADD	A,R5
	ADD	A,DIR
	ADD	A,[R1]
	ADD	A,#IMM8
*
	ADDC	A,R3
	ADDC	A,DIR
	ADDC	A,[R1]
	ADDC	A,#IMM8
*
	AJMP	EXT
*
	ANL	A,R6
	ANL	A,DIR
	ANL	A,[R1]
	ANL	A,#IMM8
	ANL	DIR,A
	ANL	C,A.5
	ANL	C,/A.5
*
	CJNE	A,DIR,*
	CJNE	A,#IMM8,*
	CJNE	R3,#IMM8,*
	CJNE	[R1],#IMM8,*
*
	CLR	A
	CLR	C
	CLR	A.5
*
	CPL	A
	CPL	C
	CPL	A.5
*
	DA	a
*
	DEC	A
	DEC	R5
	DEC	DIR
	DEC	[R1]
*
	DIV
*
	DJNZ	R5,*
	DJNZ	DIR,*
*
	INC	A
	INC	R5
	INC	DIR
	INC	[R1]
	INC	DPTR
*
	JB	A.5,*
	JBC	A.5,*
	JC	*
	JMP	[A+DPTR]
	JNB	A.5,*
	JNC	*
	JNZ	*
	JZ	*
*
	LCALL	EXT
	LJMP	EXT
*
	MOV	A,R5
	MOV	A,DIR
	MOV	A,[R1]
	MOV	A,#IMM8
	MOV	R5,A
	MOV	R5,DIR
	MOV	R5,#IMM8
	MOV	DIR,A
	MOV	DIR,R5
	MOV	DIR,DIR+1
	MOV	DIR,[R1]
	MOV	DIR,#IMM8
	MOV	[R1],A
	MOV	[R1],DIR
	MOV	[R1],#IMM8
	MOV	C,A.5
	MOV	A.5,C
	MOV	DPTR,#IMM16
*
	MOVC	A,[A+DPTR]
	MOVC	A,[A+PC]
*
	MOVX	A,[R1]
	MOVX	A,[DPTR]
	MOVX	[R1],A
	MOVX	[DPTR],A
*
	MUL
*
	NOP
*
	ORL	A,R5
	ORL	A,DIR
	ORL	A,[R1]
	ORL	A,#IMM8
	ORL	DIR,A
	ORL	DIR,#IMM8
	ORL	C,A.5
	ORL	C,/A.5
*
	POP	DIR
	PUSH	DIR
*
	RET
	RETI
*
	RL	A
	RLC	A
	RR	A
	RRC	A
*
	SETB	C
	SETB	A.5
*
	SJMP	*
*
	SUBB	A,R5
	SUBB	A,DIR
	SUBB	A,[R1]
	SUBB	A,#IMM8
*
	SWAP	A
*
	XCH	A,R5
	XCH	A,DIR
	XCH	A,[R1]
*
	XCHD	A,[R1]
*
	XRL	A,R5
	XRL	A,DIR
	XRL	A,[R1]
	XRL	A,#IMM8
	XRL	DIR,A
	XRL	DIR,#IMM8
