*
* CUBIX Command (batch) file processor
*
* END OF FILE ON COMMAND FILE
CMDEOF	PULS	A,B
	STD	>CMDSTK
	LDA	>CMDRC
	RTS
*
* COMMAND FILE PROCESSOR
*
COMFIL	LEAY	1,Y		SKIP '@'
	STA	>TEMP1		INDICATE NOT IMPLIED
CMDEX	JSR	GETDIR		GET DIRECTORY
	JSR	NAMGET		GET NAME
	LDD	#$4000		'@'
	STD	,X++		SET IT
	CLR	,X+		ZERO LAST
	JSR	LOCERR		LOOK IT UP
* START UP COMMAND FILE DIR ENT(X)
COMGO	LDA	DATTR,X		GET PROTS
	BITA	#EPERM		CAN WE EXECUTE?
	LBEQ	PROERR		NO, ERROR
	LDA	>FDRIVE		GET COMMAND FILE DRIVE
	STA	>CMDDRV		SAVE IT
	LDD	DDADR,X		GET ADDRESS
	STD	>CMDSEC		SAVE COMMAND SECTOR
	CLRA
	CLRB
	STD	>CMDOFF		SAVE COMMAND FILE OFFSET
	LDD	>CMDSTK		GET COMMAND STACK
	PSHS	A,B		SAVE OLD STACK
	CLR	>CMDRC		CLEAR RETURN CODE
	LDY	>TEMP2		RESTORE COMMAND LINE POINTER
	JSR	SAVPRM		SAVE PARAMETERS
NEWLIN	LDX	#INBUFF		PT TO BUFFER
REDCHR	JSR	RCCHR		READ CHAR FROM CMD FILE
	BEQ	CMDEOF		END OF FILE
	CMPA	#'?'		PARM?
	BNE	NOMAC		NOPE
	JSR	RCCHR		GET CHAR
	SUBA	#'0'		CONVERT
	CMPA	#9		OVER
	BLS	PAROK		OK
	LDB	#'?'
	STB	,X+
	ADDA	#'0'
	BRA	NOMAC		NOT VAR
* PARAMETER SUB TO PERFORM
PAROK	TFR	A,B		COPY
	LDY	#CMDBUF		POINT TO BUFFER
PAR1	JSR	SKIP		ADVANCE
	DECB			REDUCE COUNT
	BMI	PAR3		WE HAVE IT
PAR2	JSR	TSTERM		KEEP LOOKING
	BNE	PAR2		KEEP LOOKING
	BRA	PAR1		CONTINUE
PAR3	JSR	TSTERM		IS THIS
	BEQ	REDCHR		YES, QUIT
	STA	,X+		SAVE
	BRA	PAR3
* EXECUTE COMMAND
NOMAC	STA	,X+		CLEAR IT
	CMPA	#$0D		END OF LINE?
	BNE	REDCHR
	LDY	#INBUFF		POINT TO BUFFER
	TST	>TRCFLG		TEST FOR TRACE ON
	BEQ	GOCMD		NO, DON'T TRACE
	LDA	#'*'
	JSR	PUTCHR		FAKE DOS PROMPT
	TFR	Y,X		COPY
TRAC1	LDA	,X+
	BEQ	TRAC2
	CMPA	#$0D
	BEQ	TRAC2
	JSR	PUTCHR
	BRA	TRAC1
TRAC2	JSR	LFCR
GOCMD	JSR	SKIP		ADVANCE
	CMPA	#'?'		COMMAND CODE
	BNE	EXDCOM		NO, SKIP IT
	LDD	,Y++		GET OPERAND
	JSR	SKIP
* '?O' OUTPUT LINE
	CMPB	#'O'		OUTPUT?
	BEQ	CMPR
* '?P' ONTPUT LINE - CR
	CMPB	#'P'		PRINT?
	BNE	RCOND		CONDITIONAL
CMPR	LDA	,Y+		GET LINE
	BEQ	CMPR1
	CMPA	#$0D
	BEQ	CMPR1
	JSR	PUTCHR
	BRA	CMPR
CMPR1	CMPB	#'O'		OUTPUT?
	BEQ	CMDRT1		NO LFCR
	JSR	LFCR
	BRA	CMDRT1		BACK
* '?F' EXECUTE ON LAST COMMAND FAILED
RCOND	CMPB	#'F'		?F?
	BNE	RSUCC		NO
	LDA	>CMDRC		GET RC
	BEQ	CMDRT1		RETURN
	BRA	GOCMD		EXECUTE THIS
* '?S' EXECUTE ON LAST COMMAND SUCESSFUL
RSUCC	CMPB	#'S'		?S?
	BNE	CSRC		NO, TRY EXIT
	CLRB
	BRA	TSTRET		PEFORM TEST
* '?R' EXECUTE ON SPECIFIC RETURN CODE
CSRC	CMPB	#'R'		?R?
	BNE	CEXIT		TRY EXIT
	JSR	GETNUM		GET VALUE
	TFR	X,D		COPY
TSTRET	CMPB	>CMDRC		WUZ THIS IT?
	BNE	CMDRT1		NO, DON'T EXECUTE
	BRA	GOCMD		DO IT
* '?E' EXIT COMMAND FILE
CEXIT	CMPB	#'E'		EXIT
	BNE	CMDIN		NO, TRY SET VARIABLES
	JSR	SKIP		ANY OPERANDS
	BEQ	CEXI		NO
	JSR	GETNUM		GT VALUE
	TFR	X,D		COPY
	STB	>CMDRC		SET IT
CEXI	JMP	CMDEOF		END OF FILE
* EXECUTE DOS COMMAND
EXDCOM	LDX	>CMDOFF		GET OFFSET
	LDD	>CMDSEC		GET SECTOR
	PSHS	A,B,X		SAVE
	LDA	>CMDDRV		GET DRIVE
	PSHS	A		SAVE
	STS	>CMDSTK		SET STACK
	JSR	EXECMD		EXECUTE
CMDRET	BNE	CMDRT0
	CLRA
CMDRT0	STA	>CMDRC		SET RC
	LDS	>CMDSTK		GET STACK
	PULS	A		RESTORE DRIVE
	STA	>CMDDRV		RESAVE
	PULS	A,B,X		RESTORE VARS
	STD	>CMDSEC		RESET SECTOR
	STX	>CMDOFF		RESET OFFSET
CMDRT1	JMP	NEWLIN		LFCR & RETURN
* '?=' SET COMMAND FILE VARIABLES
CMDIN	CMPB	#'='
	BEQ	SETVAR		SET THEM
* '?I' INPUT NEW PARMS
	CMPB	#'I'		NEW PARMS?
	BNE	CHAIN		NO, TRY CHAIN
	JSR	GLINE		GET LINE
SETVAR	BSR	SAVPRM		SAVE THEM
	BRA	CMDRT1		CONTINUE
* SAVE COMMAND LINE PARAMETERS IN BUFFER
SAVPRM	PSHS	X		SAVE REGS
	LDX	#CMDBUF		POINT TO IT
SAVP1	LDA	,Y+		GET IT
	STA	,X+		WRITE IT
	BEQ	SAVP2		END
	CMPA	#$0D		OK?
	BNE	SAVP1		KEEP GOING
SAVP2	PULS	X,PC		GO HOME
* '?@' CHAIN TO NEW CMD FILE
CHAIN	CMPB	#'@'
	BNE	CMDSKP
	PULS	A,B		RESTORE STACK
	STD	>CMDSTK		RESET SAVED STACK
	JMP	CMDEX		EXECUTE IT
* '?A' ADVANCE LINES
CMDSKP	CMPB	#'A'		?A?
	BNE	CMDKEY		CONTINUE
	JSR	GETNUM		GET # TO SKIP
	LEAX	1,X		ADVANCE
LOCLP	LEAX	-1,X		REDUCE COUNT
	BEQ	CMDRT1		NEW LINE
LOCCR	BSR	RCCHR		READ A CHAR
	BEQ	CEXI		END OF FILE
	CMPA	#$0D		END OF LINE?
	BNE	LOCCR		NO, KEEP LOOKING
	BRA	LOCLP		DO THEM ALL
* '?K' READ SINGLE KEY
CMDKEY	CMPB	#'K'		?K?
	BNE	CMDCMP		NO, TRY COMPARE
	JSR	GETCHR		GET SINGLE KEY
	STA	>CMDRC		SAVE RETURN CODE
	BRA	CMDRT2		EXIT
* '??' COMPARE STRINGS
CMDCMP	CMPB	#'?'		???
	BNE	CMDRT2		IGNORE
	JSR	SKIP		TEST FOR EOL
	BEQ	CMDC3		FAIL
	TFR	Y,X		SAVE PTR
CMDC1	LDA	,Y+		GET CHAR
	CMPA	#$0D		CR?
	BEQ	CMDC3		FAIL
	CMPA	#' '		SPACE?
	BNE	CMDC1		KEEP LOOKING
CMDC2	LDA	,X+		GET CHAR
	CMPA	#' '		SPACE?
	BEQ	CMDC4		END
	CMPA	,Y+		ARE THEY SAME?
	BEQ	CMDC2		YES, A-OK
CMDC3	LDA	#255		RETURN CODE
	BRA	CMDC6		QUIT
CMDC4	LDA	,Y+		GET CHAR FROM SOURCE
	CMPA	#$0D		PASS?
	BEQ	CMDC5		YES
	CMPA	#' '		SPACE
	BNE	CMDC3		NO, FAIL
CMDC5	CLRA			ZERO RETURN CODE
CMDC6	STA	>CMDRC		SAVE RETURN CODE
CMDRT2	JMP	NEWLIN		NEXT LINE
* READ A CHARACTER FROM THE COMMAND FILE
RCCHR	PSHS	X		SAVE X
	LDA	>CMDDRV		GET CMD FILE DRIVE
	STA	>SDRIVE		SET DRIVE
	LDD	>CMDOFF		GET OFFSET
RCC1	CMPD	#512		OVER?
	BLO	RCC2		ITS OK
	LDD	>CMDSEC		GET SECTOR
	JSR	FNDLNK		LOOK UP LINK
	BEQ	RCC3		END
	STD	>CMDSEC		RESAVE
	CLRA			ZERO OFFSET
	CLRB			FOR READ
RCC2	LDX	#WRKSPC		POINT TO IT
	LEAX	D,X		OFFSET
	ADDD	#1		ADVANCE
	STD	>CMDOFF		RESAVE
	PSHS	X		SAVE REGS
	LDD	>CMDSEC		GET SECTOR
	JSR	RDWRK		READ IT
	PULS	X		RESTORE
	LDA	,X+		GET CHAR
	CMPA	#$FF		END OF FILE
RCC3	PULS	X,PC
