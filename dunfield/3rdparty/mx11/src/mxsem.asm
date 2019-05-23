*************************************************************************
*                                                                       *
* 12/5/93 MX-11 Multitasking Executive for MICRO-C/11 V 1.0             *
*                                                                       *
* C call Library - Semaphore Routines                                   *
*                                                                       *
* Kevin J White, kevin@novell.com                                       *
*                                                                       *
* Copyright 1993 Kevin White.  All rights reserved.                     *
*                                                                       *
*************************************************************************
* sopen(initial_value)         - Get handle to semaphore
* swait(&sema)                 - lock semaphore, block if already locked
* swaittim(&sema, timeout)     - same as swait but with timeout
* ssignal(&sema)               - Signal semaphore
* sset(&sema,value)            - Set semaphore value
* sclose(&sema)                - Close semaphore and return MCB
***********************************************************************
* sopen(initial_value).  Get MCB, initialize sempahore to value defined
* by argument. Put task pointer to null
sopen    tsy
         tpa
         sei
         psha
         jsr     getMCB        get message control block
         ldd     2,Y           get starting value of semaphore
         std     semsema,X     initialize semaphore
         ldd     #NULL
         std     semhead,X
         std     semtail,X
         pula
         tap
         xgdx                  return address of MCB (Zero if error)
         rts
***********************************************************************
* sclose().  Return MCB to free list. No checking for semaphore
* still in use.
sclose   tsx
         tpa
         sei
         psha
         ldx     2,X
         jsr     retMCB        return message control block
         pula
         tap
         rts
***********************************************************************
* swait(&sema)               - Lock semaphore,
* decrement semaphore, if already zero then block
swait    tsx
         tpa
         sei
         psha
*
         ldx     2,X           get address of semaphore
         ldd     semsema,X     test semaphore
         beq     swait1        is it already zero?
         subd    #1
         std     semsema,X     decrement semaphore
*
         pula
         tap
         rts                   exit
*
* Semaphore is already locked.  Set task waiting on semaphore.
* Add task to tail of semaphore task list
*
swait1   ldy     currTCB
         ldaa    #twait        mark task as waiting on semaphore
         staa    tstat,Y          
         ldd     #NULL         mark as end of list
         std     tsemalnk,Y
*
         xgdy                  put task in D
         ldy     semtail,X     get tail pointer into y
         beq     swait2        are we first task waiting?
*
* No other tasks waiting
*
         std     semtail,X     set tail pointer
         std     tsemalnk,Y    link from previous end of list
         bra     swait3        exit via task switch
*
* Yes first task waiting
*
swait2   std     semhead,X     set up first task in list
         std     semtail,X
*
swait3   pula
         tap
         jmp     mxswch        exit and wait on semaphore
***********************************************************************
* swaittim(&sema, timeout)      - Wait for semaphore with timeout
* Setup semaphore as in swait, but also setup timer as in sleep.
* The TCB contains a pointer to each.  The semaphore and Timer contain
* a pointer to the TCB.
* Whichever occurs first removes the other.
* Return value is one for semaphore or 0 for timeout, the value is placed
* in the context on the stack. If the semaphore is in a chain
* follow the chain and remove it.
*
swaittim tsx
         tpa
         sei
         psha
*
         ldx     4,X           get address of semaphore
         ldd     semsema,X     test semaphore
         beq     swaittm1      is it already locked
         subd    #1
         std     semsema,X     not locked, lock it.
*
         pula
         tap
         ldd     #TRUE         indicate semaphore unlocked
         rts                   exit
*
* Semaphore is already locked.  Set task waiting on semaphore.
* Add task to tail of semaphore task list.  Setup timer
*
swaittm1 ldy     currTCB
         ldaa    #twtim        mark task as waiting with timeout
         staa    tstat,Y          
         ldd     #NULL         mark as end of list
         std     tsemalnk,Y
         stx     tsema,Y       put address of semaphore in TCB
*
         xgdy                  put task in D
         ldy     semtail,X     get tail pointer into y
         beq     swaittm2      are we first task waiting?
*
* There are other tasks waiting, put at end of list
*
         std     semtail,X     set tail pointer
         std     tsemalnk,Y    link from previous end of list
         bra     swaittm3      exit via task switch
*
* first task waiting
*
swaittm2 std     semhead,X     set up first task in list
         std     semtail,X
*
* Setup timer
* X-> Semaphore, D->Task
*
swaittm3 tsy
         ldd     3,Y           get timeout value
         jsr     instimr       insert timer
         bcs     swaittm4      error
*
* Set timer mode
*
         ldaa    #tmsema       semaphore timeout mode
         staa    tmmode,Y
*
         ldx     currTCB       get pointer to current task
         sty     ttimer,X      put link to timer in TCB                 
*
         pula
         tap
         jmp     mxswch        exit and wait on semaphore
*
swaittm4 ldaa    semerr
         jsr     fatalerr
***********************************************************************
* ssignal(&sema)               - unlock semaphore
* Increment semaphore.  If there are any tasks on list, unblock them
* but leave semaphore at zero.  If task has timeout, remove from
* timer list and set return value
ssignal  tsx
         tpa
         psha
         sei
*
         ldx     2,X           get address of semaphore
         ldy     semhead,X     get first task (If exists)
         bne     ssignal1      still at least one task on list
         ldd     semsema,X     get semaphore
         addd    #1            increment semaphore if no task to run
         std     semsema,X
*
         pula
         tap
         rts                   * Exit to calling task
*
* mark first task on list as ready and unlink from list
* Check if task has timeout, if so remove timer and set return value
*
ssignal1 ldaa    tstat,Y       get status
         cmpa    #twtim
         bne     ssignal2      no timeout, just make ready
*
* Remove timer  set return as 1
*
         pshx
         pshy                  save Y
*
         ldx     tsp,Y         get task's stack
         ldd     #256          need to reverse A&B because of stack order
         std     6,X           put return value
*
         ldy     ttimer,Y      get timer address
         jsr     rmtimer       remove timer
*
         puly                  restore TCB
         pulx
*
ssignal2 clr     tstat,Y       mark as ready
         ldd     tsemalnk,Y    get next one on list
         bne     ssignal3      is this the last one
         std     semtail,X     yes, mark tail as well
*
ssignal3 std     semhead,X
*
         pula
         tap
         jmp     mxswch        allow task to run (If high enough priority)
***********************************************************************
* sset(&sema, value)               - lock semaphore, set to value
* Mark semaphore as locked.  Don't change task status
*
sset     tsx
         tpa
         psha
         sei
*
         ldx     4,X           get address of semaphore
         ldd     2,X
         std     semsema,X     set semaphore
*
         pula
         tap
         rts                   * Exit to calling task
***********************************************************************
$EX:     rmtimer
$EX:     instimr
***********************************************************************

