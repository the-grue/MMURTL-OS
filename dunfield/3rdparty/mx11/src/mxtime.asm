*************************************************************************
*                                                                       *
* 12/5/93 MX-11 Multitasking Executive for MICRO-C/11 V 1.0             *
*                                                                       *
* C call Library - Timer functions                                      *
*                                                                       *
* Kevin J White, kevin@novell.com                                       *
*                                                                       *
* Copyright 1993 Kevin White.  All rights reserved.                     *
*                                                                       *              *
*************************************************************************
* sleep(timeout)               - delay task for timeout ticks
* sendmsgd(time,msg)           - send message to self after delay
* getTOD(*TOD)                 - get time of day
* setTOD(*TOD)                 - set time of day
* gettime()                    - get value of hardware timer
*************************************************************************
* Sleep(ticks): Put task to sleep for "ticks" timer ticks.
* mark current task as "paused" and set up timer.  Return FALSE if
* no timer available.  Uses Message Control Blocks.
sleep    tsx
         tpa
         psha
         sei
         ldd     2,X           get timer value
         jsr     instimr       insert timer into list
         bcc     sleep1
         pula                  return with error!
         tap
         ldd     #FALSE
         rts
*
* Set task to waiting on timer
*
sleep1   ldx     currTCB
         ldaa    #tsleep
         staa    tstat,X
         pula
         tap
         ldd     #TRUE
         jmp     mxswch        exit via dispatcher
************************************************************************
* sndmsgd(delay, msg): send message to self after delay.
* Set up timer to send msg to task on timeout.  Return FALSE if
* no timer available.  Uses Message Control Blocks.
sendmsgd tsx
         tpa
         psha
         sei
         ldd     4,X           get delay value
         jsr     instimr       insert timer into list
         bcc     sndmsgd1
         pula                  return with error!
         tap
         ldd     #FALSE
         rts
*
* Set timer to give message on expiration
*
sndmsgd1 ldd     2,X           get message value
         std     tmmsg,Y       put message in timer
         ldaa    #tmalarm      set timer mode
         staa    tmmode,Y
         pula
         tap
         ldd     #TRUE
         rts                   exit
************************************************************************
* subroutine to insert timer into timer list
* D = Timer value, Y returns handle to timer, Carry set if error
* X preserved
instimr  ldy     MFREE         get free MCB
         bne     instim1       check if timer available
         sec
         rts                   exit with error

instim1  pshx                  save x
         psha                  save timer
         pshb
         ldd     mlink,Y       unlink new MCB from free list
         std     MFREE
         pulb                  get timer value
         pula
         ldx     #TIMHEAD      initialize tmp
         stx     ?temp
         ldx     TIMHEAD       get head of timer list
         beq     instim4       no timers in chain, yet.

instim2  cpd     tmval,X       compare with timer
         blo     instim5       put timer in list
         subd    tmval,X       subtract from next timer
         stx     ?temp         save link to previous timer
         ldx     tmlink,X      get link
         beq     instim4       have reached end of list
         bra     instim2       try next item
*
* have reached end of list (or nothing in list yet)
* Y-> new timer, X=0, D=Incremental value
*
instim4  stx     tmlink,Y      set link
         ldx     ?temp
         sty     tmlink,X      set previous link or TIMHEAD
         std     tmval,Y
         ldd     currTCB       get current task
         std     tmTCB,Y
         bra     instim6
*
* Put timer in middle, Y> new timer, X=next timer, ?temp=previous, D=Value
*
instim5  stx     tmlink,Y      set link
         std     tmval,Y       set value
         ldd     currTCB       set task
         std     tmTCB,Y
*
* Link into list
*
         ldd     tmval,X       modify next timer value
         subd    tmval,Y
         std     tmval,X
         ldx     ?temp         modify previous link
         sty     tmlink,X

instim6  ldaa    #tmsleep      set timer mode
         staa    tmmode,Y
         clc                   return no error
         pulx                  restore x
         rts
***********************************************************************         
* remove timer from queue, Y ->timer
* Search for timer, unlink if found
* no error if not found - it may have just expired
*
rmtimer  ldd     tmlink,Y      get link to next timer
         xgdy
         ldx     #TIMHEAD      get head of timer queue

rmtimr1  cpd     tmlink,X      have we found timer
         beq     rmtimr2       found it     
         ldx     tmlink,X      get next timer
         bne     rmtimr1       loop
         bra     rmtimr3       timer not found

rmtimr2  sty     tmlink,X      unlink timer
         ldx     MFREE         link purged timer into free queue
         xgdx
         std     tmlink,X
         stx     MFREE         
rmtimr3  rts
***********************************************************************         
* getTOD                  - Pass pointer to 4 Byte buffer
*                         - TOD copied into it
getTOD   tsx
         tpa
         sei
         psha
         ldy     2,X           get pointer to buffer
         ldx     #TOD
         ldab    #4

getTOD1  ldaa    0,X    
         staa    0,Y
         inx
         iny
         decb
         bne     getTOD1       loop

         pula
         tap
         rts
************************************************************************
* setTOD                  - Pass pointer to 4 Byte buffer
*                         - Copy it into TOD
setTOD   tsx
         tpa
         sei
         psha
         ldy     2,X           get pointer to buffer
         ldx     #TOD
         ldab    #4

setTOD1  ldaa    0,Y    
         staa    0,X
         inx
         iny
         decb
         bne     setTOD1       loop
*
         pula
         tap
         rts
************************************************************************
*gettime() return value of hardware timer
gettime  ldd     TCNT
         rts
************************************************************************
$EX:     TCNT

