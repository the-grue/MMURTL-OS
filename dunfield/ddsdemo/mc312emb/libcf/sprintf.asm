*
* Formatter print to a string: sprintf(string, format, args ...)
*
sprintf	MULB	#2		Word entries
	LEAI	,S		Point to stack
	ADAI			Point to argument
	LD	I		Get output pointer
	ST	?temp		Save for later
	TIA			Point to it
	DEC			Backup
	DEC			Backup
	TAI			Back to index
	LD	#?1		Output functino
	CALL	format		Format it
	CLR			Get zero
	SJMP	?2		And write it
?1	LD	2,S		Get character
?2	LDI	?temp		Get output pointer
	STB	I		Write to memory
	LEAI	1,I		Advance
	STI	?temp		Resave
	RET
$EX:format
