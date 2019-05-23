;
; Embedded function for the CAN utility.
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
		PUBLIC	_CAN_S, _CAN_E
;
; Embedded CAN function, appends arguments to the PSP,
; and then restores & jumps to the original code.
;
_CAN_S:	MOV		BX,0080h			; Get offset of Command Tail
		ADD		BL,[BX]				; Get length
		INC		BX					; Skip length byte
; Copy in the "permanent" operands
can1:	MOV		AL,[SI]				; Get value
		MOV		[BX],AL				; Write it out
		INC		SI					; Skip to next source
		INC		BX					; Skip to next dest
		CMP		AL,0Dh				; End of line?
		JNZ		can1				; No, keep going
		SUB		BL,82h				; Convert to actual length
		MOV		DS:0080h,BL			; Set new length
; Now... restore the original startup code, and execute it
		MOV		BX,0100h			; Point to start of code
		MOV		AX,[SI]				; Get first two bytes
		MOV		[BX],AX				; Replace it
		MOV		AL,2[SI]			; Get last byte
		MOV		2[BX],AL			; Replace it
		JMP		BX					; And execute
_CAN_E:
;
CSEG	ENDS
		END
