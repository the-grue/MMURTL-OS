sendmsg tsx
 tpa
 sei
 psha
 ldd 2,X
 ldx 4,X
 jsr mxpstvnt
 pula
 bcs ?0
 tap
 ldd #TRUE
 jmp mxswch
?0 tap exit
 ldd #FALSE
 rts
getmsg tpa critical
 sei
 psha
 ldx currTCB
?1 jsr mxgetmsg
 bcc ?2
 ldaa #trcv
 staa tstat,X
 jsr mxswch
 bra ?1
?2 xgdx save
 pula
 tap
 xgdx restore
 rts
chkmsg ldx currTCB
 ldd tqhead,X
 beq ?3
 addd #message
?3 rts
flushmsg tpa critical
 sei
 psha
 ldx currTCB
 ldy tqhead,X
 bne ?4
 pula no
 tap
 rts
?4 ldd MFREE
 ldy tqtail,X
 std mlink,Y
 ldd tqhead,X
 std MFREE
 ldd #NULL
 std tqhead,X
 std tqtail,X
 pula
 tap
 rts
