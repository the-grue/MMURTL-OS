*
* Long math functions
*
?LSIZE	EQU	4		4 bytes (32 bits)
$DD:Longreg 4
* Add two long numbers: longadd(num1, num2)
longadd	LDB	#?LSIZE		Get size
	PUSHA			Save for later
?1	LDI	4,S		Get num2 ptr
	LDB	I		Get byte
	LEAI	1,I		Skip to next
	STI	4,S		Resave
	LDI	6,S		Get num1 pointer
	ADDB	I		Add in low
	ADDB	1,S		Include carry
	STB	I		Resave
	LEAI	1,I		Skip to next
	STI	6,S		Resave
	SHR	#8		Keep carry out
	STB	1,S		Set new carry out
	LDB	,S		Get count
	DEC			Reduce
	STB	,S		Resave
	JNZ	?1		Do them all
	LD	S+		Restore value
	SHR	#8		Return carry out
	RET
* Subtract two long numbers: longsub(num1, num2)
longsub	LDB	#?LSIZE		Get size
	PUSHA			Save for later
?2	LDI	6,S		Get num1 pointer
	LDB	I		Get value
	LDI	4,S		Get num2 pointer
	SUBB	I		Subtract
	LEAI	1,I		Skip
	STI	4,S		Resave
	SUBB	1,S		Include borrow
	LDI	6,S		Get num1 back
	STB	I		Resave
	LEAI	1,I		Skip
	STI	6,S		Resave
	SHR	#8		Retain borrow
	NOT			Convert to 0/1
	NOT			Convert to 1/0
	STB	1,S		Set new borrow
	LDB	,S		Get count
	DEC			Reduce
	STB	,S		Resave
	JNZ	?2		Do them all
	LD	S+		Restore value
	SHR	#8		Return carry out
	RET
* Shift a long number RIGHT: longshr(num1)
longshr	LDB	#?LSIZE		Get long size
	PUSHA			Save for later
?3	DEC			Backup
	STB	,S		Resave count
	ADD	4,S		Get pointer to position
	TAI			Set index
	LDB	I		Get value
	PUSHA			Save old value
	SHR	#1		Shift one bit
	ORB	3,S		Include carry
	STB	I		Resave
	LD	S+		Get carry
	ANDB	#%00000001	Get old carry out
	SHL	#7		Convert to 80 : 0
	STB	1,S		Save new carry
	LDB	,S		Get count
	JNZ	?3		Do them all
	LD	S+		Restore value
	NOT			Convert to 0/1
	NOT			Convert to 1/0
	RET
* Shift a long number LEFT: longshl(num1)
longshl	LDI	2,S		Get pointer to num1
	LDB	#?LSIZE		Get long size
	PUSHA			Save for later
?4	LDB	I		Get old value
	SHL	#1		Shift left
	ORB	1,S		Include old shift out
	STB	I		Resave
	SHR	#8		Keep carry out
	STB	1,S		Save new carry out
	LEAI	1,I		Advance to next
	LDB	,S		Get count
	DEC			Reduce
	STB	,S		Resave
	JNZ	?4		Do them all
	LD	S+		Restore value
	SHR	#8		Return carry out
	RET
* Test a long number for zero: longtst(num1)
longtst	LDI	2,S		Get pointer to num1
?50	LDB	#?LSIZE		Get count
?5	PUSHA			Save count
	LDB	I		Get value
	SJNZ	?6		Not-z, exit
	LEAI	1,I		Skip to next
	LD	S+		Get value
	DEC			Backup
	SJNZ	?5		Do them all
	RET
?6	LDI	S+		Clean stack
	RET
* Compare two long numbers
longcmp	LDB	#?LSIZE		Get count
?7	PUSHA			Save for later
	DEC			Backup
	STB	,S		Resave
	LDI	6,S		Get num1 pointer
	ADAI			Offset to char
	ADD	4,S		Offset to num2
	PUSHA			Save for later
	LDB	I		Get num1 byte
	CMPB	[S+]		Compare with num2
	SJZ	?9		Not same, exit
	LD	S+		Get count
	SJNZ	?7		Do them all
?8	RET
?9	UGT			Test for less
	SJNZ	?91		Greater, return 1
	COM			Get -1
?91	LDI	S+		Clean stack
	RET
* Zero temporary register
?10	LDI	#Longreg	Point to it
?11	LDB	#?LSIZE		Get length
?12	PUSHA			Save it
	CLR			Get zero
	STB	I		Save it
	LEAI	1,I		Advance
	LD	S+		Get value
	DEC			Backup
	SJNZ	?12		Do em all
	RET
* Set a LONG number to value: longset(num1, value)
longset	LDI	4,S		Get pointer
	CALL	?11		Zero it
	LDI	4,S		Get pointer
	LD	2,S		Get value
	ST	I		Write it
	RET
* Copy a LONG number: longcpy(dest, source)
longcpy	LDB	#?LSIZE		Get size
?13	PUSHA			Save it
	LDI	4,S		Get source
	LDB	I		Get byte
	LEAI	1,I		Advance
	STI	4,S		Resave
	LDI	6,S		Get dest
	STB	I		Write value
	LEAI	1,I		Advance
	STI	6,S		Resave
	LD	S+		Get count
	DEC			Reduce
	SJNZ	?13		Do all
	RET
* Multiply two LONG numbers: longmul(num1, num2)
longmul	ALLOC	?LSIZE		Reserve space
	CALL	?10		Zero temp reg
	TSA			Get stack address
	LDI	?LSIZE+2,S	Get source
	PUSHA			Pass dest
	PUSHI			Pass source
	CALL	longcpy		Copy number
	FREE	4		Release args
?14	LD	?LSIZE+4,S	Get num1
	PUSHA			Save it
	CALL	longshr		Do the shift
	LDI	S+		Clean stack
	SJZ	?15		No add
	LDI	#Longreg	Point to temp
	TSA			Get local
	PUSHI			Pass dest
	PUSHA			Pass source
	CALL	longadd		Perform add
	FREE	4		Release args
?15	LDI	?LSIZE+4,S	Point to temp
	CALL	?50		Test for zero
	SJZ	?16		End, exit
	TSA			Get local
	PUSHA			Save it
	CALL	longshl		Shift left
	LD	S+		Clean stack
	LEAI	0,S		Get local ptr
	CALL	?50		Zero?
	SJNZ	?14		Nope, keep going
?16	LD	?LSIZE+4,S	Get num1 ptr
	LDI	#Longreg	Point to dest
	PUSHA			Pass dest
	PUSHI			Pass source
	CALL	longcpy		Copy it
	FREE	?LSIZE+4	Release args
	RET
* Divide two LONG numbers: longdiv(num1, num2)
longdiv	CALL	?10		Zero temp
	LDB	#(?LSIZE*8)+1	Get count
	PUSHA			Save for later
?17	CLR			Get zero
?18	STB	1,S		No carry in
	LD	6,S		Get num1 ptr
	PUSHA			Pass as parm
	CALL	longshl		Shift it
	LDI	S+		Get value back
	PUSHA			Save new carry
	LDB	I		Get low byte
	OR	3,S		Include carry
	STB	I		Resave
	LDB	2,S		Get count
	DEC			Reduce
	STB	2,S		Resave
	SJZ	?19		End, exit
	LD	#Longreg	Point to register
	PUSHA			Pass as parm
	CALL	longshl		Shift it
	LDI	S+		Get address
	LDB	I		Get Low byte
	OR	S+		Include new carry
	STB	I		Resave
	LD	4,S		Get num2 pointer
	PUSHI			Pass num1
	PUSHA			Pass num2
	CALL	longcmp		Do the compare
	FREE	4		Release
	SHR	#8		Test for > 1
	SJNZ	?17		Carry in zero
	LD	#Longreg	Point to reg
	LDI	4,S		Get num2 pointer
	PUSHA			Pass longreg
	PUSHI			Pass num2
	CALL	longsub		Subtract
	FREE	4		Release
	LDB	#1		Get 1
	SJMP	?18		Carry in 1
?19	FREE	4		Clean stack
	RET
