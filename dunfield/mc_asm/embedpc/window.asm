*
* Low level IBM/PC console keyboard/window interface routines
* for use with the DDS MICRO-C compiler.
*
* Copyright 1989-1995 Dave Dunfield
* All rights reserved.
*
* Permission granted for personal (non-commercial) use only.
*
?CBASE	EQU		$B800			; Color video screen segment
?MBASE	EQU		$B000			; Monochrome video screen segment
*
* Initialized variables & tables
*
W_BASE	DW		?CBASE			; Mono=B000, Cga=B800
W_PAGE	DB		0				; Current video page
W_OPEN	DW		0				; Last opened window
* Special key table - General keys
?W_KEYS	DW		$4800			; KUA	(Up arrow)
		DW		$5000			; KDA	(Down arrow)
		DW		$4B00			; KLA	(Left arrow)
		DW		$4D00			; KRA	(Right arrow)
		DW		$4900			; KPU	(PgUp)
		DW		$5100			; KPD	(PgDn)
		DW		$4700			; KHO	(Home)
		DW		$4F00			; KEN	(End)
		DW		$4E2B			; KKP	(Keypad '+')
		DW		$4A2D			; KKM	(Keypad '-')
		DW		$5200			; KIN 	(Ins)
		DW		$5300			; KDL	(Del)
		DW		$0E08			; KBS	(Backspace)
* Function keys
		DW		$3B00			; K1	(F1)
		DW		$3C00			; K2	(F2)
		DW		$3D00			; K3	(F3)
		DW		$3E00			; K4	(F4)
		DW		$3F00			; K5	(F5)
		DW		$4000			; K6	(F6)
		DW		$4100			; K7	(F7)
		DW		$4200			; K8	(F8)
		DW		$4300			; K9	(F9)
		DW		$4400			; K10	(F10)
* Special control keys
		DW		$8400			; CPU	(CTRL-PgUp)
		DW		$7600			; CPD	(CTRL-PgDn)
		DW		$7700			; CHO	(CTRL-Home)
		DW		$7500			; CEN	(CTRL-End)
		DW		0
* BOX character table
?BOXTAB	DB		$C4,$B3,$DA,$BF,$C0,$D9
		DB		$CD,$BA,$C9,$BB,$C8,$BC
		DB		$CD,$B3,$D5,$B8,$D4,$BE
*
* Open a window: w_open(px, py, sx, sy, flags)
*
wopen	PUSH	BP				; Save callers stack frame
		MOV		BP,SP			; Address parameters
		PUSH	ES				; Save ES
* Set up video mode
		MOV		AH,#$0F			; Get video mode
		INT		$10				; Call BIOS
		MOV		W_PAGE,BH		; Save video page
		MOV		BX,#?CBASE		; Assume COLOR address
		CMP		AL,#$07			; Monochrome?
		JNZ		?oinit1			; No, assumption correct
		MOV		BX,#?MBASE		; Get MONOCHROME address
?oinit1	MOV		W_BASE,BX		; Set video base address
		MOV		ES,BX			; Set up segment
* Allocate a window buffer
		XOR		AH,AH			; zero high
		MOV		AL,5[BP]		; Get video flags
		AND		AL,#$80			; Do we save screen
		JZ		?oinit2			; No, do not
		MOV		AL,6[BP]		; Get 'Y' size
		MOV		BL,8[BP]		; Get 'X' size
		MUL		BL				; Calculate total size
		SHL		AX,1			; X2 for attributes
?oinit2	ADD		AX,#14			; Include overhead
		PUSH	AX				; Pass as parameter
		CALL	malloc			; Allocate the buffer
		MOV		SI,AX			; Set up pointer
		POP		DX				; Clean up stack
		AND		SI,SI			; Did we get it?
		JNZ		?oinit3			; Yes, proceed
* Couldn't allocate, return with bad news
		POP		ES				; Restore extra set
		POP		BP				; Restore caller
		RET
* Fill in window parameter block
?oinit3	MOV		DL,12[BP]		; Get 'X' position
		MOV		DH,10[BP]		; Get 'Y' position
		MOV		2[SI],DX		; Save it
		MOV		DL,8[BP]		; Get 'X' size
		MOV		DH,6[BP]		; Get 'Y' size
		MOV		4[SI],DX		; Save size
		MOV		BX,4[BP]		; Get window flags
		MOV		[SI],BX			; Save attributes & flags
		MOV		DX,W_OPEN		; Get last opened window
		MOV		8[SI],DX		; Save in buffer
		XOR		DX,DX			; Reset cursor position
		MOV		6[SI],DX		; Save initial cursor position
* Save window contents if required
		TEST	BH,#$80			; Do we save it?
		JZ		?oinit5			; No we don't
* ... Set up pointers to screen & save area
		CALL	?xyaddr			; Get base address
		MOV		DX,4[SI]		; Get 'X' and 'Y' sizes
		LEA		DI,14[SI]		; Offset to data area
		PUSH	SI				; Save SI
		MOV		SI,BX			; Get base window address
* ... Save one line at a time
		PUSH	ES				; Stack ES
		PUSH	DS				; And DS
		POP		ES				; For swap
		POP		DS				; ES <> DS
?oinit4	PUSH	SI				; Save source
		MOV		CL,DL			; Get 'X' size
		XOR		CH,CH			; Zero high size
		REP
		MOVSW					; Move one line
		POP		SI				; Restore it
		ADD		SI,#160			; Offset to next line
		DEC		DH				; Reduce count
		JNZ		?oinit4			; And proceed
* ... Restore registers and continue
		PUSH	ES				; Stack ES
		PUSH	DS				; And DS
		POP		ES				; For swap
		POP		DS				; ES <> DS
		POP		SI				; Restore SI
		XOR		DX,DX			; Reset cursor position
		MOV		BH,5[BP]		; Get flags back
* Draw box if required
?oinit5	MOV		BP,#?BOXTAB		; Point to BOX table
		MOV		CX,4[SI]		; Get size of box
		SUB		CH,#2			; Adjust for bar
		DEC		CL				; Adjust for bar
		AND		BH,#$60			; Draw BOX?
		JZ		?oinit8			; No we don't
* ... Select box type
		CMP		BH,#$40			; Box1?
		JZ		?oinit6			; Use this one
		ADD		BP,#6			; Offset to next
		CMP		BH,#$20			; Box2?
		JZ		?oinit6			; Use this one
		ADD		BP,#6			; Offset to next
?oinit6	PUSH	CX				; Save size
		CALL	?xyaddr			; Get address
* ... Draw the top (With corners)
		MOV		AH,[SI]			; Get attribute
		MOV		AL,2[BP]		; Top left corner
		MOV		ES:[BX],AX		; Write top corner
		LEA		DI,2[BX]		; Get value
		XOR		CH,CH			; Zero high
		DEC		CL				; Adjust for bar
		MOV		AL,0[BP]		; Horizontal line
		REP
		STOSW					; Draw the line
		MOV		AL,3[BP]		; Top right hand corner
		MOV		ES:[DI],AX		; Write it
		POP		CX				; Restore position
* ... Draw the box sides
?oinit7	INC		DH				; Advance
		MOV		AL,1[BP]		; Box side
		CALL	?xyaddr			; Get position
		MOV		ES:[BX],AX		; Write it
		ADD		DL,CL			; Offset
		CALL	?xyaddr			; Get position
		MOV		ES:[BX],AX		; Write it
		SUB		DL,CL			; Backup
		DEC		CH				; Reduce count
		JNZ		?oinit7			; Do them all
* ... Draw the bottom (With corners)
		INC		DH				; Advance to bottom
		CALL	?xyaddr			; get address
		MOV		AL,4[BP]		; Lower left corner
		MOV		ES:[BX],AX		; Write it
		LEA		DI,2[BX]		; Position to line
		DEC		CL				; Adjust for bar
		MOV		AL,0[BP]		; Horizontal line
		REP
		STOSW					; Write it out
		MOV		AL,5[BP]		; Lower right corner
		MOV		ES:[DI],AX		; Write it out
* ... Reduce size of active region
		MOV		AX,2[SI]		; Get position
		INC		AL				; Advance 'X'
		INC		AH				; Advance 'Y'
		MOV		2[SI],AX		; Resave
		MOV		AX,4[SI]		; Get size
		SUB		AH,#2			; Chop out bars
		SUB		AL,#2			; Chop out bars
		MOV		4[SI],AX		; And resave
		XOR		DX,DX			; Reset cursor position
* Clear screen if required
?oinit8	MOV		BH,1[SI]		; Get flags back
		TEST	BH,#$10			; Clear screen?
		JZ		?oinit9			; No, its ok
		CALL	?cleos			; Clear one line
* Save cursor information
?oinit9	MOV		BH,W_PAGE		; Get video page
		MOV		AH,#$03			; Get cursor info
		INT		$10				; Get cursor
		MOV		10[SI],CX		; Save shape
		MOV		12[SI],DX		; Save position
		MOV		AX,SI			; Get buffer address
		MOV		W_OPEN,AX		; Save this window pointer
		POP		ES				; Restore ES
		POP		BP				; Restore callers stack frame
		RET
*
* Close current window: wclose()
*
wclose	CALL	?xsetup			; Get last window
		JMP		<?wclos0		; And proceed
*
* Close a window: w_close(window)
*
w_close CALL	?wsetup			; Get parameters
?wclos0	XOR		DX,DX			; Zero position
		MOV		BH,1[SI]		; Get open flags
* If window was BOXed, adjust sizes first
		TEST	BH,#$60			; Was it BOXed
		JZ		?wclos1			; No, don't adjust
		MOV		AX,2[SI]		; Get position
		DEC		AL				; Expand 'X'
		DEC		AH				; Expand 'Y'
		MOV		2[SI],AX		; Resave
		MOV		AX,4[SI]		; Get size
		ADD		AH,#2			; Adjust for bars
		ADD		AL,#2			; Adjust for bars
		MOV		4[SI],AX		; Resave
* Clear window if requested
?wclos1	TEST	BH,#$08			; Clear window?
		JZ		?wclos2			; No, try next
		CALL	?cleos			; Clear the window
		MOV		BH,1[SI]		; Get flags back
* Restore previous contents if saved
?wclos2	TEST	BH,#$80			; Do we restore it
		JZ		?wclos4			; No, just clear
* ... Set up pointers to screen & save area
		CALL	?xyaddr			; Calculate base address
		MOV		DX,4[SI]		; Get 'X' and 'Y' sizes
		PUSH	SI				; Save SI
		LEA		SI,14[SI]		; Offset to data area
		MOV		DI,BX			; Get base window address
* ... Restore one line at a time
?wclos3	PUSH	DI				; Save source
		MOV		CL,DL			; Get 'X' size
		XOR		CH,CH			; Zero high size
		REP
		MOVSW					; Move one line
		POP		DI				; Restore it
		ADD		DI,#160			; Offset to next line
		DEC		DH				; Reduce count
		JNZ		?wclos3			; And proceed
		POP		SI				; Restore SI
		MOV		BH,1[SI]		; Get flags back
* Restore the cursor
?wclos4	MOV		CX,10[SI]		; Get cursor shape
		MOV		AH,#$01			; Set shape
		INT		$10				; Call BIOS
		MOV		DX,12[SI]		; Get position
		MOV		BH,W_PAGE		; Get display page
		MOV		AH,#$02			; Set position
		INT		$10				; Call BIOS
* If this is the active window, switch to previously open
		CMP		SI,W_OPEN		; Is this it?
		JNZ		?wclos5			; No, don't reset
		MOV		AX,8[SI]		; Get last free
		MOV		W_OPEN,AX		; Save it
* Release the buffer contents
?wclos5	PUSH	SI				; Save address of buffer
		CALL	free			; Release it
		POP		SI				; Fixup stack
		POP		ES				; Restore ES
		POP		BP				; Restore BP
		RET
*
* Write character into current window: wputc(int c)
*
wputc	CALL	?xsetup			; Get last open window
		MOV		AX,4[BP]		; Get character
		JMP		<?tstbel		; And proceed
*
* Write a character to the video display: w_putc(int c, window)
*
w_putc	CALL	?wsetup			; Set up video addresses
		MOV		AX,6[BP]		; Get character to display
* Handle BELL
?tstbel	CMP		AX,#'G'-$40		; BELL code?
		JNZ		?tstcr			; No, try next
		MOV		AX,#$0E07		; Write BELL code
		XOR		BX,BX			; Write to page 0
		INT		$10				; Call BIOS
		JMP		<?vedit			; and exit
* Handle CARRIAGE RETURN
?tstcr	CMP		AX,#'M'-$40		; Is it carriage return
		JNZ		?tstbs			; No, try next
		XOR		DL,DL			; Reset 'X' position
		JMP 	<?vedit			; and proceed
* Handle BACKSPACE
?tstbs	CMP		AX,#'H'-$40		; Is it backspace
		JNZ		?tstnl			; No, try line-feed
		AND		DL,DL			; Already at first col?
		JZ		?vedit			; Yes, don't backup
		DEC		DL				; Reduce 'X' position
		JMP		<?vedit			; And exit
* Handle NEWLINE
?tstnl	CMP		AX,#'J'-$40		; Is it newline?
		JNZ		?norchr			; Yes, advance line
		MOV		AL,1[SI]		; Get flags
		TEST	AL,#$04			; Special case?
		JNZ		?advy			; Yes, don't reset 'X'
		XOR		DL,DL			; Reset cursor
		JMP		<?advy			; And goto a new line
* Normal Character, output it
?norchr	CALL	?xyaddr			; Calculate address
		MOV		AH,[SI]			; Get video attributes
		MOV		ES:[BX],AX		; Write to video display
* Advance 'X' position
?advx	INC		DL				; Advance 'X'
		CMP		DL,4[SI]		; Are we over?
		JB		?vedit			; No, its ok
		DEC		DL				; Restore it
		MOV		AL,1[SI]		; Get flags
		TEST	AL,#$01			; Line wrap enabled?
		JZ		?vedit			; No, skip it
		XOR		DL,DL			; Reset 'X' position
* Advance 'Y' position
?advy	INC		DH				; Advance 'Y' position
		CMP		DH,5[SI]		; are we over?
		JB		?vedit			; No, Its OK
		DEC		DH				; Reset it
		MOV		AL,1[SI]		; Get flags
		TEST	AL,#$02			; Screen wrap enabled?
		JZ		?vedit			; No, skip it
		CALL	?scroll			; Scroll window
* Restore callers environment & exit
?vedit	MOV		6[SI],DX		; Resave 'X' and 'Y'
		POP		ES				; Restore callers ES
		POP		BP				; Restore callers stack frame
		RET
*
* CLEAR current window: wclwin()
*
wclwin	CALL	?xsetup			; Get current window
		JMP		<?clscr1		; And proceed
*
* CLEAR entire window: w_clwin(window)
*
w_clwin	CALL ?wsetup				; Setup video
?clscr1	XOR		DX,DX			; Reset to top of screen
		MOV		6[SI],DX		; Reset cursor
		CALL	?updt2			; Position cursor
		XOR		DX,DX			; Reset again
		JMP		<?clscr2		; And proceed
*
* CLEAR to end of current window: wcleow()
*
wcleow	CALL	?xsetup			; Get current window
		JMP		<?clscr2		; And proceed
*
* CLEAR to END OF WINDOW: w_cleow(window)
*
w_cleow	CALL	?wsetup			; Setup video
?clscr2	CALL	?cleos			; Clear to end
		POP		ES				; Restore ES
		POP		BP				; Restore caller
		RET
*
* CLEAR to end of line in current: wcleol()
*
wcleol	CALL	?xsetup			; Get current window
		JMP 	<?cleol1		; And proceed
*
* CLEAR to END OF LINE: w_cleol(window)
*
w_cleol	CALL	?wsetup			; Setup video
?cleol1	CALL	?xyaddr			; Get address
		MOV		DI,BX			; Set up address
		MOV		CL,4[SI]		; Get size of line
		SUB		CL,DL			; Calculate remaining
		XOR		CH,CH			; Zero high
		MOV		AH,[SI]			; Get attribute
		MOV		AL,#' '			; Clear to space
		REP
		STOSW					; Clear a line
		POP		ES				; REstore ES
?cleol2	POP		BP				; Restore caller
		RET
*
* Position the cursor in current window: wgotoxy(int x, int y)
*
wgotoxy	PUSH	BP				; Save callers stack frame
		MOV		BP,SP			; Address parameters
		MOV		AL,6[BP]		; Get 'X' value
		MOV		AH,4[BP]		; Get 'Y' value
		MOV		SI,W_OPEN	; Get open window
		JMP		<?goto1			; And proceed
*
* Position the cursor in window: w_gotoxy(int x, int y, window)
*
w_gotoxy PUSH	BP				; Save callers stack frame
		MOV		BP,SP			; Address parameters
		MOV		AL,8[BP]		; Get 'X' value
		MOV		AH,6[BP]		; Get 'Y' value
		MOV		SI,4[BP]		; Get window buffer
?goto1	CMP		AL,4[SI]		; In range?
		JAE		?cleol2			; No, error
		CMP		AH,5[SI]		; In range
		JAE		?cleol2			; No, error
		MOV		6[SI],AX		; Save values
		JMP		<?updt1			; And proceed
*
* Update the cursor in current window: wgotoxy()
*
wupdatexy MOV	SI,W_OPEN		; Get open window
		JMP		<?updt2			; And proceed
*
* Update the cursor position: w_updatexy(window)
*
w_updatexy PUSH BP				; Save callers stack frame
		MOV		BP,SP			; Address parameters
		MOV		SI,4[BP]		; Get caller
?updt1	POP		BP				; Resture caller
?updt2	MOV		DX,2[SI]		; Get starting address
		ADD		DX,6[SI]		; Offset into window
		MOV		BH,W_PAGE		; Get video page
		MOV		AH,#$02			; Set cursor function
		INT		$10				; Call DOS
		RET
*
* Check for a key from the keyboard: c = w_tstc(window)
*
w_tstc	MOV		AH,#$01			; Check for key
		INT		$16				; Do we have key?
		JNZ		w_getc			; Yes, read it
?wtst1	XOR		AX,AX			; No key today
		RET
*
* Check for key with cursor in current window: c = wtstc()
*
wtstc	MOV 	AH,#$01			; Check for key
		INT		$16				; Do we have key?
		JZ		?wtst1			; No, return zero
*
* Get in cursor in current window: c = wgetc()
*
wgetc	MOV		SI,W_OPEN		; Get active window
		JMP 	<?getc1			; And proceed
*
* Get a key from the keyboard with translations: c = w_getc(window)
*
w_getc	MOV		BX,SP			; Address parameters
		MOV		SI,2[BX]		; Get window
?getc1	CALL	?updt2			; Update the cursor position
* Call BIOS to read key
		XOR		AH,AH			; Function code 0 - read key
		INT		$16				; Call bios
* Lookup key for special entries
		MOV		CL,#$80			; Beginning function code
		MOV		BX,#?W_KEYS		; Address of keys array
?lokkey	MOV		DX,[BX]			; Get key entry
		CMP		AX,DX			; Does it match?
		JZ		?fndkey			; We found it
		ADD		BX,#2			; Skip ahead
		INC		CL				; Advance key code
		OR		DH,DL			; End of table?
		JNZ		?lokkey			; Keep looking
* Translate ENTER key to newline
		CMP		AX,#$1C0D		; ENTER key?
		JNZ		?norkey			; Yes, we have it
		MOV		CL,#'J'-$40		; Newline is LINE-FEED
* Translate special keys
?fndkey	MOV		AL,CL			; Set return value
?norkey	CBW						; Set high bits
		RET
*
* Set the CURSOR OFF: wcursor_off()
*
wcursor_off	EQU	*
		MOV		CX,#$2020		; Value for cursor off
		JMP		<?setc2			; And set it
*
* Set the CURSOR to a block: wcursor_block()
*
wcursor_block EQU *
		MOV		CX,#$0006		; Color block cursor
		MOV		DX,#$000B		; Monochrome block cursor
		JMP		<?setc1			; and proceed
*
* Set the CURSOR to a LINE: wcursor_line()
*
wcursor_line EQU *
		MOV		CX,#$0607		; Color line cursor
		MOV		DX,#$0B0C		; Monochrome line cursor
?setc1	MOV		AX,W_BASE		; Get video base
		CMP		AX,#?MBASE		; Monochrome?
		JNZ		?setc2			; No, do it
		MOV		CX,DX			; Copy for later
* Set cursor to value in CX
?setc2	MOV		AH,#1			; Set cursor type
		INT		$10				; Call BIOS
		RET
*
* Set up addressability to video display & stack registers
* Exit:	ES = video base address
*		SI = window buffer address
*		DX = cursor address (X/Y)
*
?wsetup	POP		AX				; Get return address
		PUSH	BP				; Save callers stack frame
		MOV		BP,SP			; Address parameters
		MOV		SI,4[BP]		; Get window buffer
?wset1	PUSH	ES				; Save callers Extra segment
		PUSH	AX				; Replace return address
		MOV		ES,W_BASE	; Point to video base
		MOV		DX,6[SI]		; Get 'X' and 'Y' position
		RET
*
* Set up addressability to currently open window
*
?xsetup	POP		AX				; Get return address
		PUSH	BP				; Save callers stack frame
		MOV		BP,SP			; Address parameters
		MOV		SI,W_OPEN	; Get open window
		JMP		<?wset1			; And proceed
*
* Scroll window forward one line
*
?scroll	PUSH	DS				; Save data segment
		PUSH	DX				; Save cursor
		PUSH	SI				; Save SI
* First, calculate base address of window
		MOV		AL,3[SI]		; Get 'Y' position
		MOV		BL,#160			; Size of line
		MUL		BL				; Calculate 'Y' offset
		MOV		BL,2[SI]		; Get 'X' position
		XOR		BH,BH			; Zero high
		SHL		BX,1			; * 2 for char & attribute bytes		
		ADD		BX,AX			; BX = character position
		MOV		DI,BX			; Get base window address
* Scroll one line at a time
		MOV		DX,4[SI]		; Get 'X' and 'Y' sizes
		DEC		DH				; Don't copy to last
		PUSH	ES				; Stack ES
		POP		DS				; To place in DS
?scrol1	PUSH	DI				; Save DI
		MOV		SI,DI			; Begin with same address
		ADD		SI,#160			; Offset to next line
		MOV		CL,DL			; Get 'X' size
		XOR		CH,CH			; Zero high size
		REP
		MOVSW					; Move one line
		POP		DI				; Restore it
		ADD		DI,#160			; Offset to next line
		DEC		DH				; Reduce count
		JNZ		?scrol1			; And proceed
* Clear bottom line of screen
		MOV		CL,DL			; Get 'X' size
		POP		SI				; Restore SI
		POP		DX				; Restore cursor
		POP		DS				; Restore data segment
		MOV		AH,[SI]			; Get attribute
		MOV		AL,#' '			; And space
		REP
		STOSW					; Clear it
		RET
*
* Clear from cursor(DX) to end of line
*
?cleos	PUSH	DX				; Save cursor
?cleos1	CALL	?xyaddr			; Get address
		MOV		DI,BX			; Set up address
		MOV		CL,4[SI]		; Get size of line
		SUB		CL,DL			; Calculate remaining
		XOR		CH,CH			; Zero high
		MOV		AH,[SI]			; Get attribute
		MOV		AL,#' '			; Clear to space
		REP
		STOSW					; Clear a line
		XOR		DL,DL			; Zero 'X'
		INC		DH				; Advance 'Y'
		CMP		DH,5[SI]		; Are we finished
		JB		?cleos1			; Keep going
		POP		DX				; Restore cursor
		RET
*
* Calculate screen address from X/Y position (in DX)
* On exit: BX = address
*
?xyaddr	PUSH	AX				; Save AX
		MOV		AL,DH			; Get 'Y' position
		ADD		AL,3[SI]		; Offset from start of screen
		MOV		BL,#160			; Size of physical screen
		MUL		BL				; Calculate 'Y' offset
		MOV		BL,DL			; Get 'X' position
		ADD		BL,2[SI]		; Offset from start of screen
		XOR		BH,BH			; Zero high byte
		SHL		BX,1			; * 2 for char & attribute bytes
		ADD		BX,AX			; BX = character position
		POP		AX				; Restore it
		RET
$EX:malloc
$EX:free
