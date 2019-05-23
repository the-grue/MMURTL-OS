* Current window printf: register wprintf(format, args ...)
wprintf	MOV	BX,SP		Address stack
	ADD	BX,AX		BX = operand pointer
	ADD	BX,AX		(word entries)
	SUB	SP,#100		Allocate space
	MOV	DI,SP		DI = output string
	PUSH	DI		Argument to wputs
	CALL	_format_	Do the formatting
	CALL	wputs		Write to window
	ADD	SP,#102		Fix stack
	RET
* Any window printf: register w_printf(window, format, args ...)
w_printf MOV	BX,SP		Address stack
	ADD	BX,AX		BX = operand pointer
	ADD	BX,AX		(word entries)
	MOV	AX,[BX]		Get output window
	DEC	BX		Backup
	DEC	BX		Backup
	SUB	SP,#100		Allocate space
	MOV	DI,SP		DI = output string
	PUSH	DI		Argument for w_puts
	PUSH	AX		Argument to w_puts
	CALL	_format_	Process it
	CALL	w_puts		Output in window
	ADD	SP,#104		Fix stack
	RET
$EX:_format_
$EX:wputs
$EX:w_puts
