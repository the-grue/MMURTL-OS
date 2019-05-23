*
* Test file to verify instructions and addressing modes of: 8085
*
IMM	EQU	$12
EXT	EQU	$3445
*
	MOV	A,B
	MOV	D,M
*
	MVI	A,IMM
	MVI	M,IMM
*
	LXI	B,EXT
	LXI	D,EXT
	LXI	H,EXT
	LXI	SP,EXT
*
	LDA	EXT
	STA	EXT
	LHLD	EXT
	LHLD	EXT
*
	LDAX	B
	LDAX	D
	STAX	B
	STAX	D
*
	SUB	A
	SUB	B
	SUB	M
	SUI	IMM
	SBB	A
	SBB	B
	SBB	M
	SBI	IMM
*
	ADD	A
	ADD	B
	ADD	M
	ADI	IMM
	ADC	A
	ADC	B
	ADC	M
	ACI	IMM
*
	INR	A
	INR	B
	INR	M
	DCR	A
	DCR	B
	DCR	M
	INX	B
	INX	D
	INX	H
	INX	SP
	DCX	B
	DCX	D
	DCX	H
	DCX	SP
*
	DAD	B
	DAD	D
	DAD	H
	DAD	SP
*
	DAA
*
	ANA	A
	ANA	B
	ANA	M
	ANI	IMM
	ORA	A
	ORA	B
	ORA	M
	ORI	IMM
	XRA	A
	XRA	B
	XRA	M
	XRI	IMM
*
	CMP	A
	CMP	B
	CMP	M
	CPI	IMM
*
	RLC
	RRC
	RAL
	RAR
*
	CMA
	CMC
	STC
*
	JMP	EXT
	JZ	EXT
	JNZ	EXT
	JC	EXT
	JNC	EXT
	JPE	EXT
	JPO	EXT
	JM	EXT
	JP	EXT
*
	CALL	EXT
	CZ	EXT
	CNZ	EXT
	CC	EXT
	CNC	EXT
	CPE	EXT
	CPO	EXT
	CM	EXT
	CP	EXT
*
	RET
	RZ
	RNZ
	RC
	RNC
	RPE
	RPO
	RM
	RP
*
	RST	0
	RST	1
	RST	2
	RST	3
	RST	4
	RST	5
	RST	6
	RST	7
*
	PUSH	PSW
	PUSH	B
	PUSH	D
	PUSH	H
	POP	PSW
	POP	B
	POP	D
	POP	H
*
	XCHG
	XTHL
	SPHL
	PCHL
*
	IN	IMM
	OUT	IMM
*
	EI
	DI
	HLT
	NOP
* RIM & SIM are only available on the 8085 (not the 8080)
	RIM
	SIM
