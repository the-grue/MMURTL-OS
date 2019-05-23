Copen tsy
 ldd 4,Y
 jsr mxheap
 stx sinbuf
 stx sinput
 stx singet
 xgdx calculate
 addd 4,Y
 std sinbufe
 ldd 2,Y
 std soutsiz
 jsr mxheap
 stx soutbf
 stx soutput
 stx soutget
 xgdx calculate
 addd 2,Y
 std soutbfe
 ldx #0
 pshx
 jsr sopen
 pulx fixup
 cpd #0
 beq ?0
 std Crxsem
 ldx soutsiz
 pshx
 jsr sopen
 pulx fixup
 cpd #0
 beq ?0
 std Ctxsem
 ldx #1
 pshx
 jsr sopen
 pulx fixup
 cpd #0
 beq ?0
 std Cconsem
 ldd #?1
 std SCIVECT
 clr SCCR1
 ldaa #$2C
 staa SCCR2
 ldd #0
 rts
?0 ldd #-1
 rts
?1 ldaa SCSR
 anda SCCR2
 anda #$A0
 bmi ?6
 bne ?2
 ldaa #SCIerr
 jsr fatalerr
?2 ldx sinput
 ldaa SCDR
 staa 0,X
 inx bump
 cpx sinbufe
 bne ?3
 ldx sinbuf
?3 stx sinput
 cpx singet
 beq ?4
 ldx Crxsem
 pshx
 jsr ssignal
 pulx
 rti exit
?4 inx
 cpx sinbufe
 bne ?5
 ldx sinbuf
?5 stx singet
 rti
?6 ldx soutget
 ldaa 0,X
 staa SCDR
 inx increment
 cpx soutbfe
 bne ?7
 ldx soutbf
?7 stx soutget
 cpx soutput
 bne ?8
 ldaa SCCR2
 anda #$7F
 staa SCCR2
?8 ldx Ctxsem
 pshx
 jsr ssignal
 pulx
 rti exit
Cputc tsy
 ldab 3,Y
 bsr Cputca
 cmpb #$0A
 beq ?9
 rts
?9 ldab #$0D
Cputca pshy Preserve
 pshx
 psha
 pshb
 tpa critical
 psha
 sei
 pshb save
 ldaa SCCR2
 oraa #$80
 staa SCCR2
 ldx Ctxsem
 jsr ?11
 pulb retrieve
 ldx soutput
 stab 0,X
 inx Update
 cpx soutbfe
 bne ?10
 ldx soutbf
?10 stx soutput
 pula restore
 tap
 pulb restore
 pula
 pulx
 puly
 rts
?11 ldd semsema,X
 beq ?12
 subd #1
 std semsema,X
 rts
?12 ldy currTCB
 ldaa #twait
 staa tstat,Y
 ldd #NULL
 std tsemalnk,Y
 xgdy put
 ldy semtail,X
 beq ?13
 std semtail,X
 std tsemalnk,Y
 bra ?14
?13 std semhead,X
 std semtail,X
?14 jmp mxswch
Cgetc tpa critical
 psha
 sei
 ldx Crxsem
 jsr ?11
 ldx singet
 ldab 0,X
 cmpb #$0D
 bne ?15
 ldab #$0A
?15 inx bump
 cpx sinbufe
 bne ?16
 ldx sinbuf
?16 stx singet
 pula
 tap
 clra clear
 rts exit
?17 pshx
 pshy
 jsr Cgetc
 puly
 pulx
 rts
Ctestc tpa critical
 psha
 sei
 ldx Crxsem
 ldd semsema,X
 beq ?18
 jsr ?17
 XGDX
 bra ?19
?18 ldx #-1
?19 pula
 tap
 XGDX get
 rts exit
Cgetstr TSX Address
 LDY 4,X
 LDX #1
?20 JSR ?17
 CMPB #$7F
 BEQ ?21
 CMPB #$08
 BEQ ?21
 CMPB #$0A
 BEQ ?22
 PSHY Save
 TSY Address
 CPX 4,Y
 PULY Restore
 BHS ?20
 STAB ,Y
 INX Advance
 INY Advance
 JSR Cputca
 BRA ?20
?21 CPX #1
 BEQ ?20
 LDAB #$08
 JSR Cputca
 LDAB #' '
 JSR Cputca
 LDAB #$08
 JSR Cputca
 DEX Reduce
 DEY Reduce
 BRA ?20
?22 CLR ,Y
 JSR Cputca
 DEX adjust
 XGDX Return
 RTS
Cputstr ldx Cconsem
 pshx
 jsr swait
 pulx clean
 TSX Point
 LDX 2,X
?23 LDAB ,X
 BEQ ?24
 INX Advance
 pshx preserve
 pshb Put
 pshb
 jsr Cputc
 pulx clean
 pulx restore
 BRA ?23
?24 ldx Cconsem
 pshx
 jsr ssignal
 pulx
 RTS exit
Cprintf TSX Address
 ABX Offset
 ABX Twice
 TSY Y
 XGDY D
 SUBD #100
 XGDY Y
 TYS S
 PSHY Save
 JSR _format_
 JSR Cputstr
 TSX X
 XGDX D
 ADDD #102
 XGDX X
 TXS S
 RTS
$DD: sinput 2
$DD: singet 2
$DD: soutput 2
$DD: soutget 2
$DD: sinbuf 2
$DD: sinbufe 2
$DD: soutbf 2
$DD: soutbfe 2
$DD: soutsiz 2
$DD: Ctxsem 2
$DD: Crxsem 2
$DD: Cconsem 2
$EX: _format_
$EX: ssignal
$EX: swait
$EX: sopen
$EX: SCDR
$EX: SCCR2
$EX: SCSR
