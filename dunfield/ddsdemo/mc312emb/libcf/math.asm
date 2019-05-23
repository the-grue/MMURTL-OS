*
* Return ABSOLUTE VALUE of argument: int abs(int arg)
*
abs	LD	2,S		Get argument
	AND	#$8000		Is it negative?
	SJNZ	?1		Yes, handle it
	LD	2,S		Get argument back
	RET
?1	LD	2,S		Get argument back
	NEG			Negate
	RET
*
* Return MAXIMUM of two arguments: int max(int arg1, int arg2)
*
max	LD	2,S		Get arg1
	CMP	4,S		Compare with arg2
	LT			Is it less
	SJNZ	?3		Yes, return arg2
?2	LD	2,S		Get arg1
	RET
*
* Return MINIMUM of two arguments: int min(int arg1, int arg2)
*
min	LD	2,S		Get arg1
	CMP	4,S		Compare with arg2
	LT			Is it less?
	SJNZ	?2		Yes, return arg1
?3	LD	4,S		Get arg2
	RET
