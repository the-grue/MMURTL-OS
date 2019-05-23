*
* Test file to verify additional instructions and addressing modes of: 6801/03
* This file should be used in ADDITION to TEST00.ASM
*
IMM	EQU	$12
DIR	EQU	$34
EXT	EQU	$5678
*
	ABX
	ADDD	#IMM
	ADDD	DIR
	ADDD	EXT
	ADDD	10,X
	SUBD	#IMM
	SUBD	DIR
	SUBD	EXT
	SUBD	10,X
	LDD	#IMM
	LDD	DIR
	LDD	EXT
	LDD	10,X
	STD	DIR
	STD	EXT
	STD	10,X
	LSLD
	ASLD
	LSRD
	BRN	*
	JSR	DIR
	MUL
	PSHX
	PULX
