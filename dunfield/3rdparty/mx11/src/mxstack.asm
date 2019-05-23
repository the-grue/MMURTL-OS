*************************************************************************
*                                                                       *
* 12/5/93 MX-11 Multitasking Executive for MICRO-C/11 V 1.0             *
*                                                                       *
* Stack Checking Routines                                               *
*                                                                       *
* Kevin J White, kevin@novell.com                                       *
*                                                                       *
* Copyright 1993 Kevin White.  All rights reserved.                     *
*                                                                       *
*************************************************************************
mxstkchk tpa
         sei
         psha
*
* Patch mxswch
*
         ldaa    #$BD          JSR instruction
         staa    mxswch2
         ldd     #mxstchk1
         std     mxswch2+1
         pula
         tap
         rts
**********************************************************************
* Stack check: Runs through each task and verifies whether there
* is at least STKMARGIN space left in its stack space.
* It puts the minimum space found in the TCB location MINSTK.
* Also it checks that the stack signature is uncorrupted if either
* case is true a message is printed through the SCI and then a jump
* is made to "fatalerr".
* Exits with X set to #THEAD to make up for the instruction we patched
* out.
mxstchk1 ldx     THEAD         start with highest priority task
*
mxstchk2 ldd     tsp,X         get task's stack
         cpd     tspmin,X      is it lower than previous minstack
         bgt     mxstchk3      no, carry on
         std     tspmin,X      yes, save it
*
mxstchk3 subd    #STKMRGN      allow margin
         cpd     tspend,X      compare with allowed bottom of stack
         ble     mxstchk4      we have a problem
*
         ldy     tspend,X      get end of stack
         ldaa    0,Y           get signature
         cmpa    #MXSTKSIG     is it still intact?
         bne     mxstchk5      there is an error
*
* Do next task
*
         ldx     tlink,X       get next task
         bne     mxstchk2      do next task
*
* Done go back to task switch
*
         ldx     #THEAD        set up for return to mxswch
         rts
**********************************************************************
* Stack check error routines, print message then go to debugger.
* X points to TCB of task with problem.
*
* Insufficient room on stack
mxstchk4 pshx                  save TCB
         ldx     #mxoflom
         jsr     mxpmesg
mxstchk6 pulx
         pshx                  save TCB for display by debugger
         ldx     tname,X       get pointer to name
         jsr     mxpmesg       display name
         jsr     mxcrlf
         pulx                  retrieve TCB
         swi                   exit to debugger with breakpoint
         rts                   allow return to caller

* Stack signature corrupted
mxstchk5 pshx                  save TCB
         ldx     #mxsigm
         jsr     mxpmesg
         bra     mxstchk6

mxoflom  fcb     $0D,$0A
         fccz    'Stack Overflow:  Task name - '

mxsigm   fcb     $0A,$0D
         fccz    'Stack Signature Corrupted:  Task name - '
**********************************************************************

