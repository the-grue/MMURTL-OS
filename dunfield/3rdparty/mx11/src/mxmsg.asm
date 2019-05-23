*************************************************************************
*                                                                       *
* 12/5/93 MX-11 Multitasking Executive for MICRO-C/11 V 1.0             *
*                                                                       *
* C call Library - Message functions                                    *
*                                                                       *
* Kevin J White, kevin@novell.com                                       *
*                                                                       *
* Copyright 1993 Kevin White.  All rights reserved.                     *
*                                                                       *              *
*************************************************************************
* sendmsg(ptask,msg)           - send message to task
* sendmsgw(ptask,msg)          - send message to task, wait for reply (*)
* getmsg()                     - get pointer to next message          (*)
* chkmsg()                     - check if message available
* flushmsg()                   - flush all messages for task
*************************************************************************
* sendmsg(ptask,msg)           - send message to task
* get MCB, put message in it and link into task message queue
* exit via task switch.  Return 0 for error, 1 for no error
sendmsg  tsx
         tpa
         sei
         psha
*
         ldd     2,X           get message
         ldx     4,X           get receiving task
         jsr     mxpstvnt      send message to task
         pula
*
         bcs     sendmsg1      was there an error
         tap
         ldd     #TRUE
         jmp     mxswch        exit via dispatcher
*
sendmsg1 tap                   exit with error
         ldd     #FALSE
         rts
*************************************************************************
* getmsg()
* get msg if there is a message waiting, otherwise
* wait on message.  Pointer is bumped to point at value of message
*
getmsg   tpa                   critical section
         sei
         psha
*
         ldx     currTCB
getmsg1  jsr     mxgetmsg      get message if available
         bcc     getmsg2       yes there is a message
*
         ldaa    #trcv         No message yet, wait
         staa    tstat,X       set TCB to wait on receiving message
*
         jsr     mxswch        
         bra     getmsg1       get the message
*
getmsg2  xgdx                  save message in X
         pula
         tap
         xgdx                  restore message in D
         rts
************************************************************************
* chkmsg()                     - check if message available, get it if
*                                one available, NULL returned if none
chkmsg   ldx     currTCB
         ldd     tqhead,X      head pointer to first message
         beq     chkmsg1       no message yet
         addd    #message      point at message
chkmsg1  rts
************************************************************************
* flushmsg()                   - flush all messages for task
flushmsg tpa                   critical section
         sei
         psha

         ldx     currTCB
         ldy     tqhead,X      pointer to first message
         bne     flshmsg1      
         pula                  no messages, just leave
         tap
         rts

flshmsg1 ldd     MFREE
         ldy     tqtail,X
         std     mlink,Y
         ldd     tqhead,X
         std     MFREE

         ldd     #NULL         indicate no messages for task
         std     tqhead,X
         std     tqtail,X

         pula
         tap
         rts
************************************************************************

