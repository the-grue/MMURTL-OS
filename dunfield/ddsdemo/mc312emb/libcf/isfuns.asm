*
* Test for an ASCII character
*
isascii	LD	#$007F		Range to test
	SJMP	?4		Perform test
*
* Test for alphanumeric character (0-9, A-Z, a-z)
*
isalnum	LDI	S+		Save return address
	CALL	isdigit		Is it a number?
	PUSHI			Replace
	SJNZ	?1		Yes, its OK
*
* Test for alphabetic character (A-Z, a-z)
*
isalpha	LDI	S+		Save ret address
	CALL	isupper		Test uppercase
	PUSHI			Replace ret address
	SJNZ	?1		Yes, its OK
*
* Test for lowercase alphabetic character (a-z)
*
islower	LD	#'az'		Range to test
	SJMP	?4		Perform test
*
* Test for uppercase alphabetic character (A-Z)
*
isupper	LD	#'AZ'		Range to test
	SJMP	?4		Perform test
*
* Test for graphic (printable) character
*
isgraph	LD	#$217E		Range to test
	SJMP	?4		And proceed
*
* Test for hexidecimal digit
*
isxdigit LDI	S+		Save RET address
	LD	#'AF'		Range to test
	CALL	?4		Test it
	PUSHI			Resave
	SJNZ	?1		Its OK
	LDI	S+		Get RET address
	LD	#'af'		Range to test
	CALL	?4		Test ot
	PUSHI			Fix stack
	SJNZ	?1		Found it
*
* Test for an ASCII digit (0-9)
*
isdigit	LD	#'09'		Range to test
* Test argument against range in ACC
?4	PUSHA			Save ACC
	LD	4,S		Get argument
	CMPB	1,S		Too LOW?
	ULT			Complete compare
	SJNZ	?2		Yes, its invalid
	LD	4,S		Get argument
	CMPB	,S		Too HIGH?
	UGT			Complete compare
	SJNZ	?2		Too high
	LD	S+		Restore stack
* Complete test by insuring that high bits are zero
?1	LDB	#1		Return TRUE
	RET
* Return with FALSE value
?2	LD	S+		Clean stack
?3	CLR			Get a FALSE
	RET
*
* Test for CONTROL character
*
iscntrl	LD	2,S		Get argument
	CMPB	#$7F		Test for special case
	SJNZ	?1		Its control
	LD	#$001F		Control code range
	SJMP	?4		Test for it
*
* Test for a punctuation character
*
ispunct	LD	2,S		Get argument
	PUSHA			Save value
	CALL	isalnum		Alphanumeric?
	LDI	S+		Clean up stack
	SJNZ	?3		Yes, false
isprint	LD	#$207E		Printable range
	SJMP	?4		And test for it
*
* Test for a "space" character
*
isspace	LD	2,S		Get argument
	LDI	#?5		Switch table
	SWITCH
?5	FDB	?1,' '		Space
	FDB	?1,9		Tab
	FDB	?1,10		Newline
	FDB	0,?3		Default - Fail
