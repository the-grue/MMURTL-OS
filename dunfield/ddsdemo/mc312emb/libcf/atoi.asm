*
* Get a decimal number from a string
*
atoi	LDI	2,S		Get argument pointer
* First, skip any whitespace preceeding the number
?1	LDB	I		Get char
	CMPB	#' '		Is it space?
	SJNZ	?2		Yes, go to next
	LDB	I		Get char back
	CMPB	#9		Tab?
	SJZ	?3		No, its legit
?2	LEAI	1,I		Skip to next
	SJMP	?1		Try next
* Keep track of '-' sign for negative numbers
?3	LDB	I		Get char
	CMPB	#'-'		Is it negative?
	SJZ	?4		No, its OK
	LEAI	1,I		Skip '-'
	CALL	?4		Get number
	NEG			Negate
	RET
* Add up the digits into a binary value
?4	CLR			Begin with zero
	ST	?temp		Set parm
?5	LDB	I		Get char
	LEAI	1,I		Skip to next
	SUBB	#'0'		Convert
	PUSHA			Save for later
	CMPB	#9		In range
	UGT			Test for OK
	SJNZ	?6		No, exit
	LD	?temp		Get old value
	MULB	#10		Multiply
	ADDB	S+		Include new number
	ST	?temp		Resave
	SJMP	?5		And proceed
?6	LD	S+		Fix stack
	LD	?temp		Get value
	RET
