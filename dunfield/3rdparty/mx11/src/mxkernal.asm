*************************************************************************
*                                                                       *
* 12/5/93 MX-11 Multitasking Executive for MICRO-C/11 V 1.0             *
*                                                                       *
* Multitasking Core                                                     *
*                                                                       *
* Kevin J White, kevin@novell.com                                       *
*                                                                       *
* Copyright 1993 Kevin White.  All rights reserved.                     *
*                                                                       *
*************************************************************************
* mxexit.  If task falls through to here we check if there are any
* messages pending.  If so we set up to run again
* else we just mark as dormant and then go to taskswitch
*
mxexit   sei                   turn off interrupts (Don't need to save)
         ldx     currTCB
         ldaa    #tdormant     mark as dormant
         staa    tstat,X
*
         jsr     mxgetmsg      get message if available
         bcs     mxswch3       no message, go straight to taskswitch
         jsr     mxtwake       wake up task
         bra     mxswch3       go to taskswitch
*************************************************************************
* mxswch, saves registers on stack same as hardware interrupt, then
* falls through to switch.  The return adress is already on the stack, so
* need Y,X,A,B and CCR to be pushed in that order fall through to switch
mxswch   pshy
         pshx
         psha
         pshb
         tpa
         psha
*
* save current task
*
mxswchi  sei                   disable interrupts, RTI will re-enable
         ldx     ?parm         save run time library temps
         pshx
         ldx     ?temp
         pshx
         ldx     currTCB       save stack in TCB
         sts     tsp,X
*
mxswch2  ldx     #THEAD        start with highest priority task
*
mxswch3  ldx     tlink,X       get next task
         beq     mxswch4       fatal error
         ldab    tstat,X       get status
         bne     mxswch3       not runnable try next
*
* We have a ready task
* set up for new task: X points to new task's TCB.
* Setup stack to allow RTI to start new task
*
         stx     currTCB       set to current task
         lds     tsp,X         get new SP
         pulx                  retrieve old parm and temp
         stx     ?temp
         pulx
         stx     ?parm
         rti                   goto new task
*
mxswch4  ldaa    #swcherr
         jsr     fatalerr
*
         fccz    'Copyright 1993 Kevin J White'
**********************************************************************
* Timer Interrupt routine
* Count two ticks to get 1/20 second timer, decrement first timer in
* timer queue on every tock (if it exists).  If timer expires set
* task to ready.  if next timer is zero then that must be done
* immediately also, and so on.
* 
OC1INT   ldaa  #OC1F           Reset the interrupt flag
         staa  TFLG1
         ldd   TOC1            get previous compare value
         addd  #TM1CNT         create next value
         std   TOC1            put it in compare register
         dec   TICKCNT         Decrement TICK count to see if TOCK
         beq   TOCKDON
dotimx   rti                   If not a TOCK, exit quickly.

* When a tock, reset tickcnt and process timer queue
* if task is set to ready then goto tasksw to see if we need to
* switch tasks
TOCKDON  ldaa    #TOCK
         staa    TICKCNT
*
* Process timer queue
         ldx     TIMHEAD
         beq     dotimq4       link is zero, no timers active, do polling
         ldd     tmval,X
         subd    #1            decrement it
         std     tmval,X       restore it
         bne     dotimq4       timer hasn't expired, check poll list

*
* Timer could be in sleep mode, alarm mode or semaphore timeout
*
dotimq1  ldaa    tmmode,X      get timer mode
         beq     dotimq9       sleep mode
         bmi     dotimq11      semaphore timoeut
*
* Alarm mode
*
         ldd     tmmsg,X       get message
         pshx
         ldx     tmTCB,X       get task
         jsr     mxpstvnt      post message
         pulx
         bra     dotmq10       unlink timer
*
* Semaphore timeout
* remove task from semaphore queue, set return value to zero
* Task may not be first one on queue!
* X->Timer
dotimq11 pshx                  save timer
         ldy     tmTCB,X       get Task
         ldx     tsema,Y       get semaphore
         cpy     semhead,X     Is task first on queue?
         beq     dotimq12      yes
         ldx     semhead,X     no, follow chain
dotimq14 cpy     tsemalnk,X    have we found pointer to task
         beq     dotimq13      yes
         ldx     tsemalnk,X    follow chain
         bne     dotimq14      try again
         ldaa    semerr1
         jsr     fatalerr      reached end of chain
*
* Task first on semaphore queue, may not be only task,
* unlink task, if only task put null in semhead and semtail
* Set return value to zero
* X->semaphore, Y->TCB, stack ->timer, exit with X->timer
dotimq12 ldd     tsemalnk,Y    get next task
         bne     dotimq15      is it last on queue?
         std     semtail,X     set tail
dotimq15 std     semhead,X     set head
         bra     dotimq16      set return value
*
* Task not first on queue, unlink from semaphore Q
* X->prior TCB, Y->TCB, stack ->timer, exit with X->timer
*
dotimq13 ldd     tsemalnk,Y    get next task
         std     tsemalnk,X    link to previous one (don't care if null)
*
* finished with semaphore, now set return value
*
dotimq16 ldx     tsp,Y         get stack of task
         ldd     #FALSE
         std     6,X           set return value
         pulx
*
* Unblock task
*
dotimq9  ldy     tmTCB,X       get TCB of task
         clr     tstat,Y       unblock task
*
* Link expired Timer into FREE MCB queue
*
dotmq10  ldy     tmlink,X      get link to next TCS
         sty     TIMHEAD       put at head of queue
         jsr     retMCB        put on free queue
*
* check next timer
*        
         ldx     TIMHEAD
         beq     dotimq4       end of line
         ldd     tmval,X       get timer value
         beq     dotimq1       process if it is coterminous
*
* Process poll Queues and increment time of day
*
dotimq4  clrb                  loop counter
dotimq5  pshb                  save B
         aslb
         ldx     #poll
         abx

dotimq6  ldx     plink,X
         beq     dotimq7       no task
         ldd     pmesg,X       get message
         pshx                  save X
         ldx     ptask,X       get task TCB
         jsr     mxpstvnt
         pulx
         bra     dotimq6       do next task on poll list

dotimq7  pulb                  restore B

* Increment TOD
         ldx     #TOD
         abx
         inc     0,X           increment counter
         ldaa    0,X           see if we have overflowed
         ldx     #TODDIV
         abx
         cmpa    0,X
         bne     dotimq8       no, exit

         ldx     #TOD
         abx
         clr     0,X           reset counter
         incb                  
         cmpb    #4            are we done
         bne     dotimq5       loop to do next poll queue and TOD counter
dotimq8  jmp     mxswchi       done
    
TODDIV   FCB     20            dividing ratios for TOD
         FCB     60            seconds in minute
         FCB     60            minutes in hour
         FCB     24            hours in day
*********************************************************************
* postevnt: D=Event, X=taskhandle, post message for task
* If task is dormant set up task to run and mark as ready
* else add event to task's message queue.  Carry set if error
* assume interrupts disabled. X,Y,D destroyed
mxpstvnt xgdy                  * free up D
         ldaa    tstat,X
         cmpa    #tdormant     is task dormant
         beq     mxtwake1      yes, wake it up.
*
         cmpa    #trcv         is the task already waiting on a message?
         bne     post1         no, just add event to queue
         clr     tstat,X       mark task as ready  
*
post1    xgdy
         pshx
         jsr     getMCB
         beq     post3         error, exit with carry set
         std     message,X     put event into message (frees up D)
         ldd     #NULL         put null in last message
         std     mlink,X
         puly

         ldd     tqtail,Y      get tail of message queue
         beq     post2         this must be first message!

         xgdx                  put tail ptr in x
         std     mlink,X       link to new message
         std     tqtail,Y
         clc
         rts                                    

post2    stx     tqtail,Y       put MCB in both head and tail
         stx     tqhead,Y
         clc
         rts

post3    pulx
         rts
*********************************************************************
* mxtwake - wake dormant task.  X->task.  D=event
* Assume interrupts disabled.  D,Y destroyed, X preserved
mxtwake  xgdy                  * Free up D
mxtwake1 ldd     tspinit,X     get starting SP
         subd    #17           adjust for context
         std     tsp,X         set SP
         xgdy                  get message in D, new stack in Y
         std     16,Y          put message on stack
         ldd     tstart,X
         std     12,Y          setup starting address
         clr     5,Y           clear CCR
         ldd     #mxexit       set up exit address
         std     14,Y
         clr     tstat,X       mark task as ready
         clc                   * exit with no error
         rts
*********************************************************************
* Support Routines
* get next Task TCB address from free list, return 0 if none available
* Exit with X -> TCB, A,B,Y untouched
* Assumes interrupts disabled
getTCB   ldx     TFREE
         beq     getTCB1       exit if none available
         psha
         tpa
         pshx
         ldx     tlink,X       get next free
         stx     TFREE         save address
         pulx
         tap
         pula
getTCB1  rts                   exit
*
* Return TCB.  Put TCB back into free list
* On entry X -> TCB, A,B,Y untouched
retTCB   psha
         pshb
         ldd     TFREE
         std     tlink,X
         stx     TFREE
         pulb
         pula
         rts
*********************************************************************
* get next Message Control Block address from free list, return 0
* if none available Exit with X -> MCB, A,B,Y untouched
getMCB   ldx     MFREE
         beq     getMCB1       exit if none available
         psha
         tpa
         pshx
         ldx     mlink,X       get next free
         stx     MFREE         save address
         pulx
         tap
         pula                  restore condition codes
getMCB1  rts

* Return MCB.  Put MCB back into free list
* On entry X -> TCB, A,B,Y untouched
retMCB   psha
         pshb
         ldd     MFREE
         std     mlink,X
         stx     MFREE
         pulb
         pula
         rts
*********************************************************************
* Get message - get next message into D, unlink MCB
* Entry X->task, exit D = message pointer, Carry set if no message
* Assumes Interrupts disabled. Y & D destroyed
mxgetmsg ldy     tqhead,X
         beq     mxgetms2      no message available
         ldd     mlink,Y       get pointer to message
         bne     mxgetms1      last message need to set tqtail
         std     tqtail,X
mxgetms1 std     tqhead,X
         ldd     MFREE
         std     mlink,Y
         sty     MFREE
         ldd     message,Y     get message
         clc                   indicate message available
         rts

mxgetms2 sec                   indicate no message available
         rts
*********************************************************************
* Allocate space on heap at end of heap
* D = space required, assume its available
* X returns pointer to block (after header)
* A,B,Y Untouched
mxheap   std     ?parm
         ldx     #?heap
mxheap1  tst     0,X
         beq     mxheap2

         pshx                  Block in use, try next one
         pula
         pulb

         addd    #3
         addd    1,X
         xgdx
         bra     mxheap1       try next block

* X now points to end of heap         
mxheap2  ldaa    #2
         staa    0,X           mark as in use
         ldd     ?parm
         std     1,X           save size in header
*
         stx     ?temp
         addd    ?temp         calculate address of next block 
         addd    #3
         xgdx
*
         clr     0,X           mark end of heap
         ldx     ?temp
         inx                   bump X to point to memory
         inx
         inx
         ldd     ?parm         retrieve size
         rts
*************************************************************************
* Print "Fatal Error at" address of caller "Code -" and A value
fatalerr psha
         ldx     #mxmsg1
         jsr     mxpmesg
         pula
         jsr     mxhex2
         ldx     #mxmsg2
         jsr     mxpmesg
         tsx
         ldd     0,X
         jsr     mxhex2
         tba
         jsr     mxhex2
         jsr     mxcrlf
         jmp     monitor
*
* Print character in A, destroys A
mxhex2   psha
         lsra
         lsra
         lsra
         lsra
         bsr     mxhex1
         pula
*
* Print hex character in A
mxhex1   anda    #$0F
         cmpa    #9
         bgt     mxhex1a
         adda    #'0'
         bra     mxpchar

mxhex1a  adda    #'A'-10
         bra     mxpchar

mxmsg1   fccz    "Fatal Error - "        
mxmsg2   fccz    " at "
*************************************************************************
* Print zero terminated message without using interrupts
* X points to message
mxpmesg1 bsr     mxpchar       print character in A
         inx
mxpmesg  ldaa    0,X
         bne     mxpmesg1
         rts
*
mxcrlf   ldaa    #$0D
         bsr     mxpchar
         ldaa    #$0A
*
mxpchar  tst      SCSR
         bpl     mxpchar
         staa    SCDR
         rts
*************************************************************************
* mxinit(stacksize, numtcb, nummcb) - Initialize MX-11
* deftask(&task, stack_size, name)  - define task
* defpoll(taskh,pollq,msg)          - add task to poll list
*************************************************************************
* int mxinit(stacksize, nummTCB, numMCB)
* Initialize executive, create TCBs, MCBs from heap memory.
* Initialize timer OC1 use vector.
* Main is set as second to lowest priority task.  nulltsk is set as
* lowest priority task. All TCBs free list. Parameter sets size of stack
* to leave for main. Returns handle of main task
mxinit   sei
*
* Calculate space needed on heap and then get it.  Assume we are first
* usr so don't bother to search on heap for space.  Assume also that
* no more than 255 TCB's or MCB's are requested.
         tsx
         ldd     4,X           get number of TCBs
         ldaa    #TCBSIZE
         mul
         jsr     mxheap        allocate space on heap for TCB
*
* Address of memory is in X
*
         stx     TFREE         initialize TCB free list
         ldd     TFREE
         tsx
         ldy     4,X           get number of TCB's to make
         ldx     TFREE
*
* set up TCB's
*
mxinit1  dey
         beq     mxinit2
         addd    #TCBSIZE
         std     0,X
         ldx     0,X
         bra     mxinit1

mxinit2  sty     0,X           zero last link
*
* set up MCB's
*
         tsx
         ldd     2,X           get number of MCB's
         ldaa    #MCBSIZE
         mul                   calculate size needed
         jsr     mxheap
* X now has start of MCB's
         stx     MFREE
         tsy
         ldy     2,Y           get number of MCB's to make
         ldd     MFREE
*
mxinit3  dey
         beq     mxinit4
         addd    #MCBSIZE
         std     0,X
         ldx     0,X
         bra     mxinit3
mxinit4  sty     0,X           zero last link         
*
* Setup null task
*
         jsr     getTCB
         stx     THEAD         THEAD points to head of task chain
         ldaa    #tready       set task as ready
         staa    tstat,X       set nulltask as ready
*
         tsy
         xgdy
         tsy
         subd    6,Y           leave room for main stack
*
* setup main stack signature
*
         xgdy
         ldaa    #MXSTKSIG     set up stack signature of main
         staa    1,Y
         xgdy
         std     ?temp         save main stack for later
*
         subd    #17           leave room for initial context
         std     tsp,X
         std     tspmin,X      initialize minstack position
         subd    #nullstk      leave room for null task stack
         std     nextstk
*
         addd    #1
         std     tspend,X
         xgdy
         ldaa    #MXSTKSIG     set up stack signature
         staa    0,Y
*
         ldy     tsp,X         retrieve null task stack
         ldd     #nulltsk
         std     tstart,X      setup start address in TCB
         std     12,Y          setup PC
         clr     5,Y           setup CCR
         ldd     #NULL
         std     tlink,X       zero link
         ldd     #mxname1      get task name
         std     tname,X 
         clr     tpri,X        set priority to zero
*
* Now setup main
         jsr     getTCB        get next TCB
         ldd     THEAD         link task into chain highest priority first
         std     tlink,X
         stx     THEAD

         ldd     #NULL         setup queue pointers
         std     tqhead,X
         std     tqtail,X
         std     tsema,X
         std     tsemalnk,X

         ldd     #mxname2      set task name
         std     tname,X
         ldd     #main         setup starting address
         std     tstart,X
         clr     tstat,X       set main as running (Called from main)
         stx     currTCB       set current TCB
         ldaa    #1            set priority
         staa    tpri,X
         staa    mxpri
*
         tsy
         xgdy
         std     tsp,X         set up stack parameters in TCB
         std     tspmin,X
         ldd     ?temp         get stack end
         addd    #1
         std     tspend,X      set up end point
*                       
* Initialize Timer system
         ldd     #OC1INT       Setup interrupt vectors
         std     TOC1VECT
         LDAA    #OC1I
         STAA    TMSK1         Enable OC1 interrupt
*
         ldaa    #TOCK         set up timer
         staa    TICKCNT
         clra
         clrb                  Clear TOD counter
         std     TOD
         std     TOD+2
         staa    TOD+4
         std     TIMHEAD       set timer queue as empty

         ldd     #NULL
         std     poll          clear poll list
         std     poll+2
         std     poll+4
         std     poll+6

         cli                   enable interrupts

         ldd     THEAD         return handle to main
         rts

mxname1  fccz    'NullTask'
mxname2  fccz    'Main'
**********************************************************************
* task_handle deftask (taskadd, stack_size, name)                             *
*************************************************************************
* Stack frame data structure, when task suspended                       *
*  Offset from SP                                                       *
*      ----------------                                                 *
*      | 17  |   ARGL |  <- Argument low                                *
*      | 16  |   ARGH |  <- Argument High                               *
*      | 15  |   EXL  |  <- Exit function address                       *
*      | 14  |   EXH  |                                                 *
*      | 13  |   PCL  |  <- Program Counter                             *
*      | 12  |   PCH  |                                                 *
*      | 11  |   IYL  |                                                 *
*      | 10  |   IYH  |                                                 *
*      |  9  |   IXL  |                                                 *
*      |  8  |   IXH  |                                                 *
*      |  7  |   ACCA |                                                 *
*      |  6  |   ACCB |                                                 *
*      |  5  |   CCR  |  <- Condition code register                     *
*      |  4  |   PARML|  <- Runtime lib Parm                            *
*      |  3  |   PARMH|                                                 *
*      |  2  |   TEMPL|  <- Runtime lib Temp                            *
*      |  1  |   TEMPH|                                                 *
*      |     |        |  <- SP points here                              *
*      ----------------                                                 *
*************************************************************************
* Get next free TCB (exit with D=0 if none available)
* Set up TCB with start add, stack etc.  Returns TCB address
* Does not run task, sendmsg will cause task to run
* Setup stack limits, and put end of stack signature into memory
*
deftask  tpa                   critical section while we mess with TCB
         sei
         tsy                   put SP into Y to get parameters
         psha                  save CCR
*
*        Set up TCB
*
         jsr     getTCB
         beq     deftsk1       exit with error, no TCB available

         ldd     THEAD         link into task queue
         std     tlink,X
         stx     THEAD

         ldd     2,Y           set name
         std     tname,X
         ldd     6,Y           set task startadd
         std     tstart,X
         ldab    #tdormant     set task status to dormant
         stab    tstat,X

         ldd     #NULL         set other elements of TCB to null
         std     tqhead,X      don't allocate Message q until used
         std     tqtail,X      set to Null so we know it isn't active
         std     tsema,X       same for semaphore
         ldd     nextstk       set task Stack into TCB
         std     tspinit,X     starting SP
         std     tspmin,X      initialize min stack marker
         std     tsp,X         initialize SP to keep stack checker happy
         subd    4,Y           leave stack space for task (arg 2)
         std     nextstk
         addd    #1            calulate end of stack
         std     tspend,X      put in TCB
         xgdy
         ldaa    #MXSTKSIG
         staa    0,Y           put signature in memory         
*
         inc     mxpri         set priority of task
         ldaa    mxpri
         staa    tpri,X
*
deftsk1  pula
         tap                   restore CCR
         xgdx                  put taskhandle in D
         rts
*************************************************************************
* defpoll(task,pollq,msg)     - add task to poll list
* Message is sent to task at poll interval
* returns 1 if no error, 0 if error.
defpoll  tpa
         sei
         tsy
         psha
*
         jsr     getMCB        get control block to use for poll queue
         bne     defpol1       no error

         pula                  return error
         tap
         ldd     #FALSE
         rts
*
defpol1  ldd     4,Y           get poll queue number
         asld                  create address of poll head
         addd    #poll
*
* D->pollqhead,X->new MCB, Y->task stack
*
         pshy
         xgdy                  Y points to pollhead
         ldd     0,Y           get first poll item
         std     plink,X       put address in MCB
         stx     0,Y           link in new MCB
         puly                  restore Y
*
         ldd     6,Y           get task handle
         std     ptask,X
         ldd     2,Y           get message
         std     pmesg,X       put in MCB
*
         pula
         tap
         ldd     #TRUE         exit without error
         rts
************************************************************************
* Get pointer to base of MX variables
mxvar    ldd     #THEAD
         rts
*********************************************************************
*   Kernel Variables                                                *
*********************************************************************
* list pointers
$DD:     THEAD     2             Head of TCB list
$DD:     TIMHEAD   2             head of TIMER list
$DD:     poll      8             poll queue list pointers
$DD:     TFREE     2             head of TCB free list
$DD:     MFREE     2             Head of MCB free list
$DD:     currTCB   2             Address of current task's TCB
$DD:     mxpri     2             Priority of next allocated task
$DD:     nextstk   2             Next available stack space
$DD:     TICKCNT   1             fortieth of second counter
$DD:     prevTCS   2
$DD:     TOD       4             space for twntieths,sec,min,hours
**********************************************************************
$EX:     TOC1
$EX:     TFLG1
$EX:     TMSK1
$EX:     nulltsk
*********************************************************************


