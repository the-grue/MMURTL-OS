*
* Concatinate one string to another: strcat(dest, source)
*
strcat	LDI	4,S		Get dest ptr
?1	LDB	I		Get byte from index
	JZ	?2		End of string, copy
	LEAI	1,I		Advance pointer
	SJMP	?1		And proceed
?2	STI	4,S		Resave pointer
*
* Copy strings: strcpy(dest, source)
*
strcpy	LDI	2,S		Get source ptr
	LDB	I		Get byte from source
	LEAI	1,I		Advance source ptr
	STI	2,S		Resave
	LDI	4,S		Get dest ptr
	STB	I		Write to dest
	LEAI	1,I		Advance dest ptr
	STI	4,S		Resave
	SJNZ	strcpy		Do them all
	RET
