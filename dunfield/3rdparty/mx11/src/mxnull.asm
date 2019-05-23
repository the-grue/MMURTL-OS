*************************************************************************
*                                                                       *
* 12/5/93 MX-11 Multitasking Executive for MICRO-C/11 V 1.0             *
*                                                                       *
* Nulltask (Wait for interrupt version)                                 *
*                                                                       *
* Kevin J White, kevin@novell.com                                       *
*                                                                       *
* Copyright 1993 Kevin White.  All rights reserved.                     *
*                                                                       *
*************************************************************************
nulltsk  wai                   wait for interrupt
         ldx     cycles        count the number of cycles
         inx                   through nulltask
         stx     cycles
         bra     nulltsk       do it again
*********************************************************************
$DD:     cycles    2
*********************************************************************

