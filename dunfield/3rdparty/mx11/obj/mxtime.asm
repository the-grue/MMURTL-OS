sleep tsx
 tpa
 psha
 sei
 ldd 2,X
 jsr instimr
 bcc ?0
 pula return
 tap
 ldd #FALSE
 rts
?0 ldx currTCB
 ldaa #tsleep
 staa tstat,X
 pula
 tap
 ldd #TRUE
 jmp mxswch
sendmsgd tsx
 tpa
 psha
 sei
 ldd 4,X
 jsr instimr
 bcc ?1
 pula return
 tap
 ldd #FALSE
 rts
?1 ldd 2,X
 std tmmsg,Y
 ldaa #tmalarm
 staa tmmode,Y
 pula
 tap
 ldd #TRUE
 rts exit
instimr ldy MFREE
 bne ?2
 sec
 rts exit
?2 pshx save
 psha save
 pshb
 ldd mlink,Y
 std MFREE
 pulb get
 pula
 ldx #TIMHEAD
 stx ?temp
 ldx TIMHEAD
 beq ?4
?3 cpd tmval,X
 blo ?5
 subd tmval,X
 stx ?temp
 ldx tmlink,X
 beq ?4
 bra ?3
?4 stx tmlink,Y
 ldx ?temp
 sty tmlink,X
 std tmval,Y
 ldd currTCB
 std tmTCB,Y
 bra ?6
?5 stx tmlink,Y
 std tmval,Y
 ldd currTCB
 std tmTCB,Y
 ldd tmval,X
 subd tmval,Y
 std tmval,X
 ldx ?temp
 sty tmlink,X
?6 ldaa #tmsleep
 staa tmmode,Y
 clc return
 pulx restore
 rts
rmtimer ldd tmlink,Y
 xgdy
 ldx #TIMHEAD
?7 cpd tmlink,X
 beq ?8
 ldx tmlink,X
 bne ?7
 bra ?9
?8 sty tmlink,X
 ldx MFREE
 xgdx
 std tmlink,X
 stx MFREE
?9 rts
getTOD tsx
 tpa
 sei
 psha
 ldy 2,X
 ldx #TOD
 ldab #4
?10 ldaa 0,X
 staa 0,Y
 inx
 iny
 decb
 bne ?10
 pula
 tap
 rts
setTOD tsx
 tpa
 sei
 psha
 ldy 2,X
 ldx #TOD
 ldab #4
?11 ldaa 0,Y
 staa 0,X
 inx
 iny
 decb
 bne ?11
 pula
 tap
 rts
gettime ldd TCNT
 rts
$EX: TCNT
