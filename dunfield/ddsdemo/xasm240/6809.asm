*
* Test file to verify instructions and addressing modes of: 6809
*
IMM	EQU	$12
DIR	EQU	$34
EXT	EQU	$5678
	SETDP	0
*
	ABX
*
	ADCA	#IMM
	ADCA	DIR
	ADCA	EXT
	ADCA	[EXT]
	ADCA	,X
	ADCA	,Y++
	ADCA	[,--U]
	ADCB	#IMM
	ADCB	DIR
	ADCB	EXT
	ADCB	[EXT]
	ADCB	,X
	ADCB	,Y++
	ADCB	[,--U]
*
	ADDA	#IMM
	ADDA	DIR
	ADDA	EXT
	ADDA	[EXT]
	ADDA	,X
	ADDA	,Y++
	ADDA	[,--U]
	ADDB	#IMM
	ADDB	DIR
	ADDB	EXT
	ADDB	[EXT]
	ADDB	,X
	ADDB	,Y++
	ADDB	[,--U]
	ADDD	#IMM
	ADDD	DIR
	ADDD	EXT
	ADDD	[EXT]
	ADDD	,X
	ADDD	,Y++
	ADDD	[,--U]
*
	ANDA	#IMM
	ANDA	DIR
	ANDA	EXT
	ANDA	[EXT]
	ANDA	,X
	ANDA	,Y++
	ANDA	[,--U]
	ANDB	#IMM
	ANDB	DIR
	ANDB	EXT
	ANDB	[EXT]
	ANDB	,X
	ANDB	,Y++
	ANDB	[,--U]
	ANDCC	#IMM
*
	ASLA
	ASLB
	ASL	DIR
	ASL	EXT
	ASL	[EXT]
	ASL	,X
	ASL	,Y++
	ASL	[,--U]
*
	ASRA
	ASRB
	ASR	DIR
	ASR	EXT
	ASR	[EXT]
	ASR	,X
	ASR	,Y++
	ASR	[,--U]
*
	BCC	*
	BCS	*
	BEQ	*
	BGE	*
	BGT	*
	BHI	*
	BHS	*
	BLE	*
	BLO	*
	BLS	*
	BLT	*
	BMI	*
	BNE	*
	BPL	*
	BRA	*
	BRN	*
	BVC	*
	BVS	*
	BSR	*
*
	BITA	#IMM
	BITA	DIR
	BITA	EXT
	BITA	[EXT]
	BITA	,X
	BITA	,Y++
	BITA	[,--U]
	BITB	#IMM
	BITB	DIR
	BITB	EXT
	BITB	[EXT]
	BITB	,X
	BITB	,Y++
	BITB	[,--U]
*
	CLRA
	CLRB
	CLR	DIR
	CLR	EXT
	CLR	[EXT]
	CLR	,X
	CLR	,Y++
	CLR	[,--U]
*
	CMPA	#IMM
	CMPA	DIR
	CMPA	EXT
	CMPA	[EXT]
	CMPA	,X
	CMPA	,Y++
	CMPA	[,--U]
	CMPB	#IMM
	CMPB	DIR
	CMPB	EXT
	CMPB	[EXT]
	CMPB	,X
	CMPB	,Y++
	CMPB	[,--U]
	CMPD	#IMM
	CMPD	DIR
	CMPD	EXT
	CMPD	[EXT]
	CMPD	,X
	CMPD	,Y++
	CMPD	[,--U]
*
	COMA
	COMB
	COM	DIR
	COM	EXT
	COM	[EXT]
	COM	,X
	COM	,Y++
	COM	[,--U]
*
	CWAI	#IMM
	DAA
*
	DECA
	DECB
	DEC	DIR
	DEC	EXT
	DEC	[EXT]
	DEC	,X
	DEC	,Y++
	DEC	[,--U]
*
	EORA	#IMM
	EORA	DIR
	EORA	EXT
	EORA	[EXT]
	EORA	,X
	EORA	,Y++
	EORA	[,--U]
	EORB	#IMM
	EORB	DIR
	EORB	EXT
	EORB	[EXT]
	EORB	,X
	EORB	,Y++
	EORB	[,--U]
*
	EXG	A,B
	EXG	X,Y
*
	INCA
	INCB
	INC	DIR
	INC	EXT
	INC	[EXT]
	INC	,X
	INC	,Y++
	INC	[,--U]
*
	JMP	DIR
	JMP	EXT
	JMP	[EXT]
	JMP	,X
	JMP	,X++
	JMP	[,Y--]
	JSR	DIR
	JSR	EXT
	JSR	[EXT]
	JSR	,X
	JSR	,X++
	JSR	[,Y--]
*
	LDA	#IMM
	LDA	DIR
	LDA	EXT
	LDA	[EXT]
	LDA	,X
	LDA	,Y++
	LDA	[,--U]
	LDB	#IMM
	LDB	DIR
	LDB	EXT
	LDB	[EXT]
	LDB	,X
	LDB	,Y++
	LDB	[,--U]
	LDD	#IMM
	LDD	DIR
	LDD	EXT
	LDD	[EXT]
	LDD	,X
	LDD	,Y++
	LDD	[,--U]
*
	LEAX	5,X
	LEAX	,Y++
	LEAX	,--U
	LEAX	[,S++]
	LEAY	5,X
	LEAY	,Y++
	LEAY	,--U
	LEAY	[,S++]
	LEAU	5,X
	LEAU	,Y++
	LEAU	,--U
	LEAU	[,S++]
	LEAS	5,X
	LEAS	,Y++
	LEAS	,--U
	LEAS	[,S++]
*
	LSLA
	LSLB
	LSL	DIR
	LSL	EXT
	LSL	[EXT]
	LSL	,X
	LSL	,Y++
	LSL	[,--U]
*
	LSRA
	LSRB
	LSR	DIR
	LSR	EXT
	LSR	[EXT]
	LSR	,X
	LSR	,Y++
	LSR	[,--U]
*
	MUL
*
	NEGA
	NEGB
	NEG	DIR
	NEG	EXT
	NEG	[EXT]
	NEG	,X
	NEG	,Y++
	NEG	[,--U]
*
	NOP
*
	ORA	#IMM
	ORA	DIR
	ORA	EXT
	ORA	[EXT]
	ORA	,X
	ORA	,Y++
	ORA	[,--U]
	ORB	#IMM
	ORB	DIR
	ORB	EXT
	ORB	[EXT]
	ORB	,X
	ORB	,Y++
	ORB	[,--U]
	ORCC	#IMM
*
	PSHS	A,B,CC,DP,X,Y,U,PC
	PSHU	A,B,CC,DP,X,Y,S,PC
	PULS	A,B,CC,DP,X,Y,U,PC
	PULU	A,B,CC,DP,X,Y,S,PC
*
	ROLA
	ROLB
	ROL	DIR
	ROL	EXT
	ROL	[EXT]
	ROL	,X
	ROL	,Y++
	ROL	[,--U]
*
	RORA
	RORB
	ROR	DIR
	ROR	EXT
	ROR	[EXT]
	ROR	,X
	ROR	,Y++
	ROR	[,--U]
*
	RTI
	RTS
*
	SBCA	#IMM
	SBCA	DIR
	SBCA	EXT
	SBCA	[EXT]
	SBCA	,X
	SBCA	,Y++
	SBCA	[,--U]
	SBCB	#IMM
	SBCB	DIR
	SBCB	EXT
	SBCB	[EXT]
	SBCB	,X
	SBCB	,Y++
	SBCB	[,--U]
*
	SEX
*
	STA	DIR
	STA	EXT
	STA	[EXT]
	STA	,X
	STA	,Y++
	STA	[,--U]
	STB	DIR
	STB	EXT
	STB	[EXT]
	STB	,X
	STB	,Y++
	STB	[,--U]
	STD	DIR
	STD	EXT
	STD	[EXT]
	STD	,X
	STD	,Y++
	STD	[,--U]
*
	SUBA	#IMM
	SUBA	DIR
	SUBA	EXT
	SUBA	[EXT]
	SUBA	,X
	SUBA	,Y++
	SUBA	[,--U]
	SUBB	#IMM
	SUBB	DIR
	SUBB	EXT
	SUBB	[EXT]
	SUBB	,X
	SUBB	,Y++
	SUBB	[,--U]
	SUBD	#IMM
	SUBD	DIR
	SUBD	EXT
	SUBD	[EXT]
	SUBD	,X
	SUBD	,Y++
	SUBD	[,--U]
*
	SWI
	SWI2
	SWI3
	SYNC
*
	TFR	A,B
	TFR	X,Y
*
	TSTA
	TSTB
	TST	DIR
	TST	EXT
	TST	[EXT]
	TST	,X
	TST	,Y++
	TST	[,--U]
