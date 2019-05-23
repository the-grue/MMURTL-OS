;
; Embedded function for the COMCHK utility.
;
; Copyright 1990 Dave Dunfield.
;
; May be freely distributed and used as long as
; my copyright notices are retained.
;
DGRP	GROUP	DSEG
DSEG	SEGMENT	BYTE PUBLIC 'IDATA'
DSEG	ENDS
CSEG	SEGMENT	BYTE PUBLIC 'CODE'
		ASSUME	CS:CSEG, DS:DGRP, SS:DGRP
		PUBLIC	_CHK_S, _CHK_E
;
; Embedded COMCHK function, checksums the program image, and aborts to DOS
; if it does not match the previously calculated value.
;
_CHK_S:	MOV		BX,0100h			; Get offset of Command Tail
		MOV		CX,BX				; Save for later
		MOV		AX,[SI]				; Get first two bytes
		MOV		[BX],AX				; Replace it
		MOV		AL,2[SI]			; Get last byte
		MOV		2[BX],AL			; Replace it
		XOR		AX,AX				; Zero initial
chk1:	ADD		AL,[BX]				; Include low
		ADC		AH,0				; Include high
		INC		BX					; Advance to next
		CMP		BX,SI				; At end?
		JB		chk1				; Do them all
		CMP		AX,3[SI]			; Does it match?
		JNE		chk2				; No, report error
; Passed... restore the original startup code, and execute it
		JMP		CX
; Failed... Output error message & terminate
chk2:	MOV		AH,40h				; Write to file
		MOV		BX,2				; STDERR handle
		MOV		CX,5[SI]			; Get length
		LEA		DX,7[SI]			; Get message
		INT		21h					; Output message
		XOR		AH,AH				; Terminate
		INT		21h					; Goodbye
_CHK_E:
;
CSEG	ENDS
		END
