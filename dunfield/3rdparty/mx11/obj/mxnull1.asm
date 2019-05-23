nulltsk clra
 clrb
 std cycles
?0 ldd cycles
 addd #1
 std cycles
 ldx #13
?1 dex 3
 bne ?1
 nop 2
 bra ?0
$DD: cycles 2
