*
* Search for character in string: strchr(string, chr)
*
strchr	LDI	4,S		Get pointer to string
?1	LDB	I		Get character from string
	JZ	?2		End of string
	CMP	2,S		Does it match?
	JNZ	?3		Yes, we found it
	LEAI	1,I		Skip to next
	SJMP	?1		No, keep looking
?2	CMP	2,S		Are we looking for zero
	JZ	?4		No, return 0
?3	TIA			Get pointer
?4	RET
