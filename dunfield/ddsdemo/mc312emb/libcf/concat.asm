*
* Concatinate several strings: register concat(dest, source, ...)
*
concat	ST	?temp		Save argument count
	MULB	#2		Word entries
	LEAI	,S		Point to stack
	ADAI			Adjust
	PUSHI			Save argument pointer
	LDI	I		Get output pointer
	PUSHI			Save output pointer
?1	LD	?temp		Get arg count
	DEC			Reduce count
	ST	?temp		And resave
	SJZ	?3		End of count
	LD	2,S		Get argument pointer
	DEC			Backup
	DEC			Backup
	ST	2,S		Resave argument pointer
	TAI			Point to it
	LDI	I		Get source pointer
?2	LDB	I		Get byte from source
	SJZ	?1		Zero, get next
	PUSHI			Save input pointer
	LDI	2,S		Get output pointer
	STB	I		Write to output
	LEAI	1,I		Advance
	STI	2,S		Resave
	LDI	S+		Restore input pointer
	LEAI	1,I		Advance to next
	SJMP	?2		And proceed
?3	FREE	4		Release stack
	RET
