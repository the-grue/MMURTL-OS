*
* Compare two strings: strcmp(string1, string2)
*
strcmp	LDI	4,S		Get string1 pointer
?1	LDB	I		Get character
	LDI	2,S		Get string2 pointer
	CMPB	I		Compare
	SJZ	?2		Not same
	LDB	I		Get char back
	SJZ	?3		Zero, strings match
	LEAI	1,I		Advance string2
	STI	2,S		Resave
	LDI	4,S		Get pointer back
	LEAI	1,I		Advance
	STI	4,S		Resave
	SJMP	?1		And proceed
?2	GT			Is string1 < string2
	JNZ	?3		Yes, its OK
	COM			Get -1
?3	RET
