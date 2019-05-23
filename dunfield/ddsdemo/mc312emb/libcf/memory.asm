*
* Set memory to a specified value: memset(block, value, size)
*
memset	LDI	6,S		Get pointer
?1	LD	2,S		Get size
	SJZ	?2		Zero, exit
	DEC			Backup
	ST	2,S		Resave
	LD	4,S		Get value
	STB	I		Write to memory
	LEAI	1,I		Advance
	SJMP	?1		And proceed
*
* Copy memory: memcpy(dest, source, size)
*
memcpy	LD	2,S		Get size
	SJZ	?2		Zero, exit
	DEC			Reduce count
	ST	2,S		Resave
	LDI	4,S		Get source
	LDB	I		Get byte
	LEAI	1,I		Advance
	STI	4,S		Resave
	LDI	6,S		Get dest
	STB	I		Write byte
	LEAI	1,I		Advance
	STI	6,S		Resave
	SJMP	memcpy		And proceed
?2	RET
