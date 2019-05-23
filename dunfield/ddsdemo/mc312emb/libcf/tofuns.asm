*
* Convert character to UPPER case: toupper(c)
*
toupper	LD	2,S		Get operand
	CMPB	#'a'		Lower case?
	ULT			Compete compare
	SJNZ	?1		No, ignore
	LD	2,S		Get operand
	CMPB	#'z'		Lower case?
	UGT			Complere compare
	SJNZ	?1		No, ignore
	LD	2,S		Get operand
	ANDB	#%01011111	Convert to upper
	RET
*
* Convert character to LOWER case: tolower(c)
*
tolower	LD	2,S		Get operand
	CMPB	#'A'		Lower case?
	ULT			Compete compare
	SJNZ	?1		No, ignore
	LD	2,S		Get operand
	CMPB	#'Z'		Lower case?
	UGT			Complere compare
	SJNZ	?1		No, ignore
	LD	2,S		Get operand
	ORB	#%00100000	Convert to upper
	RET
?1	LD	2,S		Get parameter
	RET
