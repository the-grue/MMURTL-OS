*
* Formatted print to the console device: printf(format, args ...)
*
printf	MULB	#2		Word entries
	LEAI	,S		Point to stack
	ADAI			Point to argument
	LD	#putch		Point to output func
	JMP	format		Format it
$EX:format
$EX:putch
