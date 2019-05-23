*
* Format to "printf" output: A=Output Function, I = Argument List
*
format	PUSHA			Save output function pointer
	PUSHI			Save argument pointer
	ALLOC	10		Save  flag variable
* Local variable & argument offsets
?1	EQU	12		Pointer to output function
?2	EQU	10		Pointer to argument list
?3	EQU	4		Buffer for number conversions (6 bytes)
?4	EQU	2		Temporary locations
?5	EQU	1		Character count
?6	EQU	0		Formatting controls
*
	LDI	?2,S		Get arg pointer
	LDI	I		Get format string
?7	LDB	I		Get character from string
	SJZ	?8		Exit if end
	CMPB	#'%'		Format character
	SJNZ	?10		Yes, handle it
	LDB	I		Get char back
	LEAI	1,I		Advance pointer
	PUSHI			Save Index
	PUSHA			Pass character
	LD	4+?1,S		Get output function
	CALL	?9		Output the character
	LD	S+		Clean stack
	LDI	S+		Get Index back
	SJMP	?7		And proceed
?8	FREE	14		Release stack
	RET
* Call output function in A
?9	IJMP			Call function
* Format character located
?10	LEAI	1,I		Skip to next
	CLR			Begin with zero
	ST	?4,S		Save for later
* Check for leading '-' = Left justify
	STB	?6,S		Init. flag
	STB	?5,S		Init. count
	LDB	I		Get char
	CMPB	#'-'		Minus?
	SJZ	?11		No, try next
	LEAI	1,I		Advance pointer
	LDB	#%10000000	Set justify flag
	STB	?6,S		Resave
* Check for leading '0' = Zero fill
?11	LDB	I		Get char back
	CMPB	#'0'		Zero fill?
	SJZ	?12		No, try next
	LEAI	1,I		Advance
	LDB	?6,S		Get value
	ORB	#%01000000	Set zero fill flag
	STB	?6,S		Resave
* Get field width
?12	LDB	I		Get character
	SUBB	#'0'		Convert to binary
	PUSHA			Save
	CMPB	#10		In range?
	ULT			Must be less than
	SJZ	?13		Nope
	LEAI	1,I		Skip digit
	LD	2+?4,S		Get old value
	MULB	#10		x10
	ADD	S+		Include new
	ST	?4,S		Resave
	SJMP	?12		Do them all
?13	LD	S+		Clear stack
	LD	?4,S		Get value
	ORB	?6,S		Include previous flags
	STB	?6,S		Resave
* Lookup character & dispatch handler
	PUSHI			Save format pointer
	LD	2+?2,S		Get argptr
	DEC			Backup
	DEC			Backup
	ST	2+?2,S		Resave
	TAI			Address
	LD	I		Get value
	ST	2+?4,S		Resave
	LDB	[S]		Get next character
	LEAI	?14		Point to table
	SWITCH			Lookup & execute
?14	FDB	?15,'o'		Octal number
	FDB	?16,'d'		Decimal number
	FDB	?17,'u'		Unsigned number
	FDB	?18,'x'		Hexidecimal number
	FDB	?26,'c'		Character output
	FDB	?28,'s'		String output
	FDB	0,?27		Ignore it
* '%o' - Octal number
?15	LDB	#8		Base 8
	SJMP	?19		Do number
* '%d' - signed Decimal number
?16	LD	2+?4,S		Get value
	AND	#$8000		Negatve?
	SJZ	?17		Nothing special
	LDB	#'-'		Minus size
	PUSHA			Pass to function
	LD	4+?1,S		Get output function
	CALL	?9		Call function
	LD	S+		Clean stack
	LD	2+?4,S		Get value
	NEG			Negate
	ST	2+?4,S		And prcoeed
	LDB	2+?6,S		Get length
	ANDB	#%001111111	Free format?
	SJZ	?17		Yes, ignore
	LDB	2+?6,S		Get value back
	DEC			Adjust for '-'
	STB	2+?6,S		Resave
* '%u' - Unsigned decimal number
?17	LDB	#10		Base 10 output
	SJMP	?19		Do number
* '%x' - heXidecimal number
?18	LDB	#16		Base 16 output
* Output a number in the specified number base
?19	STB	2+?3,S		Save number base
	LEAI	6+2+?3,S	Point to END of buffer
?20	TIA			Get index
	DEC			Backup
	TAI			Set index
	LDB	2+?5,S		Get count
	INC			Advance
	STB	2+?5,S		Resave
	LD	2+?4,S		Get value
	DIVB	2+?3,S		Divide by 10
	PUSHA			Save quotient
	ALT			Get remainder
	ADDB	#'0'		Convert to ASCII
	STB	I		Write to stack
	CMPB	#'9'		Is it HEX?
	UGT			Complete test
	SJZ	?21		Yes it is
	LDB	I		Get char back
	ADDB	#7		Adjust
	STB	I		Resave char
?21	LD	S+		Get quotient
	ST	2+?4,S		Save new value
	SJNZ	?20		Do them all
* At this point, I = pointer, ?5=Length
?22	LDB	2+?5,S		Get count
	STB	3+?4,S		Set extra count
	LDB	2+?6,S		Get flag
	ANDB	#%10000000	Left justify?
	SJNZ	?23		Yes, don't fill
	CALL	?31		Do any pre-output
?23	LDB	3+?4,S		Get count
	SJZ	?24		All done, exit
	DEC			Reduce count
	STB	3+?4,S		Resave
	LDB	I		Get character from string
	LEAI	1,I		Advance
	PUSHI			Save it
	PUSHA			Pass character
	LD	6+?1,S		Get output function
	CALL	?9		Output character
	LD	S+		Clean stack
	LDI	S+		Restore I
	SJMP	?23		Go again
?24	LDB	2+?6,S		Get flag
	ANDB	#%10000000	Left justify
	SJZ	?25		No, skip it
	CALL	?31		Post justify
?25	LDI	S+		Restore format string
	LEAI	1,I		Advance pointer
	JMP	?7		And proceed
* '%c' - output character
?26	LDB	2+?4,S		Get character
?27	LEAI	2+?3,S		Point to buffer
	STB	I		Save it
	LDB	#1		Get 1
	STB	2+?5,S
	SJMP	?22
* '%s' - output string
?28	LDI	2+?4,S		Get string pointer
?29	LDB	I		Get character
	SJZ	?30		End of string
	LEAI	1,I		Skip to next
	LDB	2+?5,S		Get count
	INC			Advance
	STB	2+?5,S		Resave count
	SJMP	?29		And proceed
?30	LDI	2+?4,S		Get pointer back
	SJMP	?22		And output
* Perform pre-output justify
?31	LDB	4+?6,S		Get flag
	ANDB	#%00111111	Save length only
	SJZ	?35		No justify
	STB	4+?4,S		Save for later
	PUSHI			Save index
?32	LDB	6+?5,S		Get count
	CMPB	6+?4,S		Are we over
	UGE			Complete test
	SJNZ	?34		Yes, exit
	LDB	6+?6,S		Get flag
	ANDB	#%01000000	Zero fill?
	SJZ	?33		No, use ' '
	LDB	#'0'-' '	Get value
?33	ADDB	#' '		Convert to char
	PUSHA			Pass character
	LD	8+?1,S		Get output function
	CALL	?9		Output character
	LD	S+		Clean stack
	LDB	6+?4,S		Get remaining width
	DEC			Reduce
	STB	6+?4,S		Resave
	SJMP	?32		Do next
?34	LDI	S+		Restore index
?35	RET
