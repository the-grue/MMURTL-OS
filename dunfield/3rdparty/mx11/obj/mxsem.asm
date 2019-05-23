sopen tsy
 tpa
 sei
 psha
 jsr getMCB
 ldd 2,Y
 std semsema,X
 ldd #NULL
 std semhead,X
 std semtail,X
 pula
 tap
 xgdx return
 rts
sclose tsx
 tpa
 sei
 psha
 ldx 2,X
 jsr retMCB
 pula
 tap
 rts
swait tsx
 tpa
 sei
 psha
 ldx 2,X
 ldd semsema,X
 beq ?0
 subd #1
 std semsema,X
 pula
 tap
 rts exit
?0 ldy currTCB
 ldaa #twait
 staa tstat,Y
 ldd #NULL
 std tsemalnk,Y
 xgdy put
 ldy semtail,X
 beq ?1
 std semtail,X
 std tsemalnk,Y
 bra ?2
?1 std semhead,X
 std semtail,X
?2 pula
 tap
 jmp mxswch
swaittim tsx
 tpa
 sei
 psha
 ldx 4,X
 ldd semsema,X
 beq ?3
 subd #1
 std semsema,X
 pula
 tap
 ldd #TRUE
 rts exit
?3 ldy currTCB
 ldaa #twtim
 staa tstat,Y
 ldd #NULL
 std tsemalnk,Y
 stx tsema,Y
 xgdy put
 ldy semtail,X
 beq ?4
 std semtail,X
 std tsemalnk,Y
 bra ?5
?4 std semhead,X
 std semtail,X
?5 tsy
 ldd 3,Y
 jsr instimr
 bcs ?6
 ldaa #tmsema
 staa tmmode,Y
 ldx currTCB
 sty ttimer,X
 pula
 tap
 jmp mxswch
?6 ldaa semerr
 jsr fatalerr
ssignal tsx
 tpa
 psha
 sei
 ldx 2,X
 ldy semhead,X
 bne ?7
 ldd semsema,X
 addd #1
 std semsema,X
 pula
 tap
 rts *
?7 ldaa tstat,Y
 cmpa #twtim
 bne ?8
 pshx
 pshy save
 ldx tsp,Y
 ldd #256
 std 6,X
 ldy ttimer,Y
 jsr rmtimer
 puly restore
 pulx
?8 clr tstat,Y
 ldd tsemalnk,Y
 bne ?9
 std semtail,X
?9 std semhead,X
 pula
 tap
 jmp mxswch
sset tsx
 tpa
 psha
 sei
 ldx 4,X
 ldd 2,X
 std semsema,X
 pula
 tap
 rts *
$EX: rmtimer
$EX: instimr
