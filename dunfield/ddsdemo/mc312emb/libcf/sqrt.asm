*
* Return SQUARE ROOT of argument: int sqrt(int arg)
*
sqrt	LD	2,S		Get argument
	CMP	#255*255	Test for MAX
	ULT			Is it less?
	SJNZ	?1		Yes, its OK
	LD	#256		Assume 256*256
	RET
?1	LDB	#1		Start with 1
?2	PUSHA			Save value
	MUL	,S		Multiply
	CMP	4,S		Compare with arg
	UGE			Is it over?
	SJNZ	?3		Yes, exit
	LD	S+		Get value back
	INC			Advance
	SJMP	?2		And proceed
?3	LD	S+		Get value back
	RET
