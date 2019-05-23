*
* Abort program with error message: abort(message)
*
abort	LD	S+		Remove return address
	CALL	putstr		Output string
	JMP	exit		Exit program
$EX:putstr
$EX:exit
