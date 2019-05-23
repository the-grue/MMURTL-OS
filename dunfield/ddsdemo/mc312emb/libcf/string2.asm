*
* Determine length of string: strlen(string)
*
strlen	LDI	2,S		; Get string ptr
?1	LDB	I		; Get byte from string
	JZ	?2		; We are at end
	LEAI	1,I		; Skip to next
	SJMP	?1		; And proceed
?2	TIA			; Get pointer
	SUB	2,S		; Compute actual length
	RET
