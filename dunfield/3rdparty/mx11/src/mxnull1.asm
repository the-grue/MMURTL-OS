*************************************************************************
*                                                                       *
* 12/5/93 MX-11 Multitasking Executive for MICRO-C/11 V 1.0             *
*                                                                       *
* Nulltask (CPU Utilization version)                                    *
*                                                                       *
* Kevin J White, kevin@novell.com                                       *
*                                                                       *
* Copyright 1993 Kevin White.  All rights reserved.                     *
*                                                                       *
*************************************************************************
* Utilization counter.  Increment variable CYCLES every 50 microseconds
* (100 cycles).
* code in twentieth second task will calculate CPU utilization from the
* count.
nulltsk  clra
         clrb
         std     cycles
null1    ldd     cycles        5 cycles   \
         addd    #1            4 cycles    }  17 cycles
         std     cycles        5 cycles    |
         ldx     #13           3 cycles   /
null2    dex                   3 cycles \
         bne     null2         3 cycles /     78 cycles
         nop                   2 cycles        2 cycles      
         bra     null1         3 cycles        3 cycles
*                                            ---
*                                            100 Cycles
*                                            ---
*********************************************************************
$DD:     cycles    2
*********************************************************************

