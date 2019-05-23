?0 sei turn
 ldx currTCB
 ldaa #tdormant
 staa tstat,X
 jsr mxgetmsg
 bcs ?1
 jsr mxtwake
 bra ?1
mxswch pshy
 pshx
 psha
 pshb
 tpa
 psha
mxswchi sei disable
 ldx ?parm
 pshx
 ldx ?temp
 pshx
 ldx currTCB
 sts tsp,X
mxswch2 ldx #THEAD
?1 ldx tlink,X
 beq ?2
 ldab tstat,X
 bne ?1
 stx currTCB
 lds tsp,X
 pulx retrieve
 stx ?temp
 pulx
 stx ?parm
 rti goto
?2 ldaa #swcherr
 jsr fatalerr
 fccz 'Copyright 1993 Kevin J White'
?3 ldaa #OC1F
 staa TFLG1
 ldd TOC1
 addd #TM1CNT
 std TOC1
 dec TICKCNT
 beq ?5
?4 rti If
?5 ldaa #TOCK
 staa TICKCNT
 ldx TIMHEAD
 beq ?15
 ldd tmval,X
 subd #1
 std tmval,X
 bne ?15
?6 ldaa tmmode,X
 beq ?13
 bmi ?7
 ldd tmmsg,X
 pshx
 ldx tmTCB,X
 jsr mxpstvnt
 pulx
 bra ?14
?7 pshx save
 ldy tmTCB,X
 ldx tsema,Y
 cpy semhead,X
 beq ?9
 ldx semhead,X
?8 cpy tsemalnk,X
 beq ?11
 ldx tsemalnk,X
 bne ?8
 ldaa semerr1
 jsr fatalerr
?9 ldd tsemalnk,Y
 bne ?10
 std semtail,X
?10 std semhead,X
 bra ?12
?11 ldd tsemalnk,Y
 std tsemalnk,X
?12 ldx tsp,Y
 ldd #FALSE
 std 6,X
 pulx
?13 ldy tmTCB,X
 clr tstat,Y
?14 ldy tmlink,X
 sty TIMHEAD
 jsr retMCB
 ldx TIMHEAD
 beq ?15
 ldd tmval,X
 beq ?6
?15 clrb loop
?16 pshb save
 aslb
 ldx #poll
 abx
?17 ldx plink,X
 beq ?18
 ldd pmesg,X
 pshx save
 ldx ptask,X
 jsr mxpstvnt
 pulx
 bra ?17
?18 pulb restore
 ldx #TOD
 abx
 inc 0,X
 ldaa 0,X
 ldx #?20
 abx
 cmpa 0,X
 bne ?19
 ldx #TOD
 abx
 clr 0,X
 incb
 cmpb #4
 bne ?16
?19 jmp mxswchi

?20 FCB 20
 FCB 60
 FCB 60
 FCB 24
mxpstvnt xgdy *
 ldaa tstat,X
 cmpa #tdormant
 beq ?24
 cmpa #trcv
 bne ?21
 clr tstat,X
?21 xgdy
 pshx
 jsr getMCB
 beq ?23
 std message,X
 ldd #NULL
 std mlink,X
 puly
 ldd tqtail,Y
 beq ?22
 xgdx put
 std mlink,X
 std tqtail,Y
 clc
 rts
?22 stx tqtail,Y
 stx tqhead,Y
 clc
 rts
?23 pulx
 rts
mxtwake xgdy *
?24 ldd tspinit,X
 subd #17
 std tsp,X
 xgdy get
 std 16,Y
 ldd tstart,X
 std 12,Y
 clr 5,Y
 ldd #?0
 std 14,Y
 clr tstat,X
 clc *
 rts
getTCB ldx TFREE
 beq ?25
 psha
 tpa
 pshx
 ldx tlink,X
 stx TFREE
 pulx
 tap
 pula
?25 rts exit
retTCB psha
 pshb
 ldd TFREE
 std tlink,X
 stx TFREE
 pulb
 pula
 rts
getMCB ldx MFREE
 beq ?26
 psha
 tpa
 pshx
 ldx mlink,X
 stx MFREE
 pulx
 tap
 pula restore
?26 rts
retMCB psha
 pshb
 ldd MFREE
 std mlink,X
 stx MFREE
 pulb
 pula
 rts
mxgetmsg ldy tqhead,X
 beq ?28
 ldd mlink,Y
 bne ?27
 std tqtail,X
?27 std tqhead,X
 ldd MFREE
 std mlink,Y
 sty MFREE
 ldd message,Y
 clc indicate
 rts
?28 sec indicate
 rts
mxheap std ?parm
 ldx #?heap
?29 tst 0,X
 beq ?30
 pshx Block
 pula
 pulb
 addd #3
 addd 1,X
 xgdx
 bra ?29
?30 ldaa #2
 staa 0,X
 ldd ?parm
 std 1,X
 stx ?temp
 addd ?temp
 addd #3
 xgdx
 clr 0,X
 ldx ?temp
 inx bump
 inx
 inx
 ldd ?parm
 rts
fatalerr psha
 ldx #?34
 jsr mxpmesg
 pula
 jsr ?31
 ldx #?35
 jsr mxpmesg
 tsx
 ldd 0,X
 jsr ?31
 tba
 jsr ?31
 jsr mxcrlf
 jmp monitor
?31 psha
 lsra
 lsra
 lsra
 lsra
 bsr ?32
 pula
?32 anda #$0F
 cmpa #9
 bgt ?33
 adda #'0'
 bra ?37
?33 adda #'A'-10
 bra ?37
?34 fccz "Fatal Error - "
?35 fccz " at "
?36 bsr ?37
 inx
mxpmesg ldaa 0,X
 bne ?36
 rts
mxcrlf ldaa #$0D
 bsr ?37
 ldaa #$0A
?37 tst SCSR
 bpl ?37
 staa SCDR
 rts
mxinit sei
 tsx
 ldd 4,X
 ldaa #TCBSIZE
 mul
 jsr mxheap
 stx TFREE
 ldd TFREE
 tsx
 ldy 4,X
 ldx TFREE
?38 dey
 beq ?39
 addd #TCBSIZE
 std 0,X
 ldx 0,X
 bra ?38
?39 sty 0,X
 tsx
 ldd 2,X
 ldaa #MCBSIZE
 mul calculate
 jsr mxheap
 stx MFREE
 tsy
 ldy 2,Y
 ldd MFREE
?40 dey
 beq ?41
 addd #MCBSIZE
 std 0,X
 ldx 0,X
 bra ?40
?41 sty 0,X
 jsr getTCB
 stx THEAD
 ldaa #tready
 staa tstat,X
 tsy
 xgdy
 tsy
 subd 6,Y
 xgdy
 ldaa #MXSTKSIG
 staa 1,Y
 xgdy
 std ?temp
 subd #17
 std tsp,X
 std tspmin,X
 subd #nullstk
 std nextstk
 addd #1
 std tspend,X
 xgdy
 ldaa #MXSTKSIG
 staa 0,Y
 ldy tsp,X
 ldd #nulltsk
 std tstart,X
 std 12,Y
 clr 5,Y
 ldd #NULL
 std tlink,X
 ldd #?42
 std tname,X
 clr tpri,X
 jsr getTCB
 ldd THEAD
 std tlink,X
 stx THEAD
 ldd #NULL
 std tqhead,X
 std tqtail,X
 std tsema,X
 std tsemalnk,X
 ldd #?43
 std tname,X
 ldd #main
 std tstart,X
 clr tstat,X
 stx currTCB
 ldaa #1
 staa tpri,X
 staa mxpri
 tsy
 xgdy
 std tsp,X
 std tspmin,X
 ldd ?temp
 addd #1
 std tspend,X
 ldd #?3
 std TOC1VECT
 LDAA #OC1I
 STAA TMSK1
 ldaa #TOCK
 staa TICKCNT
 clra
 clrb Clear
 std TOD
 std TOD+2
 staa TOD+4
 std TIMHEAD
 ldd #NULL
 std poll
 std poll+2
 std poll+4
 std poll+6
 cli enable
 ldd THEAD
 rts
?42 fccz 'NullTask'
?43 fccz 'Main'
deftask tpa critical
 sei
 tsy put
 psha save
 jsr getTCB
 beq ?44
 ldd THEAD
 std tlink,X
 stx THEAD
 ldd 2,Y
 std tname,X
 ldd 6,Y
 std tstart,X
 ldab #tdormant
 stab tstat,X
 ldd #NULL
 std tqhead,X
 std tqtail,X
 std tsema,X
 ldd nextstk
 std tspinit,X
 std tspmin,X
 std tsp,X
 subd 4,Y
 std nextstk
 addd #1
 std tspend,X
 xgdy
 ldaa #MXSTKSIG
 staa 0,Y
 inc mxpri
 ldaa mxpri
 staa tpri,X
?44 pula
 tap restore
 xgdx put
 rts
defpoll tpa
 sei
 tsy
 psha
 jsr getMCB
 bne ?45
 pula return
 tap
 ldd #FALSE
 rts
?45 ldd 4,Y
 asld create
 addd #poll
 pshy
 xgdy Y
 ldd 0,Y
 std plink,X
 stx 0,Y
 puly restore
 ldd 6,Y
 std ptask,X
 ldd 2,Y
 std pmesg,X
 pula
 tap
 ldd #TRUE
 rts
mxvar ldd #THEAD
 rts
$DD: THEAD 2
$DD: TIMHEAD 2
$DD: poll 8
$DD: TFREE 2
$DD: MFREE 2
$DD: currTCB 2
$DD: mxpri 2
$DD: nextstk 2
$DD: TICKCNT 1
$DD: prevTCS 2
$DD: TOD 4
$EX: TOC1
$EX: TFLG1
$EX: TMSK1
$EX: nulltsk
