mxstkchk tpa
 sei
 psha
 ldaa #$BD
 staa mxswch2
 ldd #?0
 std mxswch2+1
 pula
 tap
 rts
?0 ldx THEAD
?1 ldd tsp,X
 cpd tspmin,X
 bgt ?2
 std tspmin,X
?2 subd #STKMRGN
 cpd tspend,X
 ble ?3
 ldy tspend,X
 ldaa 0,Y
 cmpa #MXSTKSIG
 bne ?5
 ldx tlink,X
 bne ?1
 ldx #THEAD
 rts
?3 pshx save
 ldx #?6
 jsr mxpmesg
?4 pulx
 pshx save
 ldx tname,X
 jsr mxpmesg
 jsr mxcrlf
 pulx retrieve
 swi exit
 rts allow
?5 pshx save
 ldx #?7
 jsr mxpmesg
 bra ?4
?6 fcb $0D,$0A
 fccz 'Stack Overflow:  Task name - '
?7 fcb $0A,$0D
 fccz 'Stack Signature Corrupted:  Task name - '
