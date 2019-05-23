*************************************************************************
*                                                                       *
* 12/5/93 MX-11 Multitasking Executive for MICRO-C/11 V 1.0             *
*                                                                       *
* Interrupt Driven Serial I/O                                           *
*                                                                       *
* Kevin J White, kevin@novell.com                                       *
*                                                                       *
* Copyright 1993 Kevin White.  All rights reserved.                     *
*                                                                       *
*************************************************************************
* Copen, Ctestc, Cgetc, Cputc, Cgetstr
*
* void Copen(inbufsize, outbufsize)
*
Copen    tsy
         ldd     4,Y           get input buffer size
         jsr     mxheap        get some heap space for it
*
         stx     sinbuf        save pointer to beginning of buffer
         stx     sinput
         stx     singet  
*
         xgdx                  calculate buffer end +1
         addd    4,Y
         std     sinbufe
*
         ldd     2,Y           get output buffer size
         std     soutsiz       save buffer size for later
         jsr     mxheap        get some heap space for it
*
         stx     soutbf        save pointer to beginning of buffer
         stx     soutput
         stx     soutget  
*
         xgdx                  calculate buffer end+1
         addd    2,Y
         std     soutbfe
*
* Open semaphore each for receive and transmit.
* mark transmit semaphore as ready, receive as not ready
*
         ldx     #0
         pshx
         jsr     sopen         open locked semaphore for receive
         pulx                  fixup stack
         cpd     #0
         beq     Copenerr      error
         std     Crxsem        save address of receive semaphore
*
         ldx     soutsiz       open semaphore with size of buffer for transmit          
         pshx
         jsr     sopen
         pulx                  fixup stack
         cpd     #0
         beq     Copenerr      error
         std     Ctxsem        save address of transmit semaphore
*
         ldx     #1            Open semaphore console output with one resource
         pshx
         jsr     sopen
         pulx                  fixup stack
         cpd     #0
         beq     Copenerr      error
         std     Cconsem       save address of Console semaphore
*
         ldd     #sint
         std     SCIVECT       set up interrupt vector
         clr     SCCR1         set up SCI
         ldaa    #$2C          TX & RX enabled, RxInt enable,txint disabled, 
         staa    SCCR2
         ldd     #0            indicate success
         rts
*
Copenerr ldd     #-1           indicate error
         rts
*************************************************************************
* get here as a result of a serial interrupt
sint     ldaa    SCSR          get status
         anda    SCCR2         mask with interrupt enable
         anda    #$A0
         bmi     sxmit         transmit interrupt, go do it
         bne     srecv         is it receive
         ldaa    #SCIerr
         jsr     fatalerr
*
* Data received, put into buffer.  If buffer full bump get pointer as well
* as put pointer to discard oldest data.
*
srecv    ldx     sinput        pointer
         ldaa    SCDR          get data from UART
         staa    0,X           put in buffer
*
         inx                   bump put pointer, wrap if needed
         cpx     sinbufe
         bne     srecv1
         ldx     sinbuf
srecv1   stx     sinput
*
         cpx     singet        is buffer full?
         beq     srecv3        yes, bump pointer, don't signal
*
         ldx     Crxsem        signal task that data available
         pshx
         jsr     ssignal
         pulx
         rti                   exit ISR
*
* buffer full, bump get pointer to discard data
*
srecv3   inx
         cpx     sinbufe
         bne     srecv2        wrap if needed
         ldx     sinbuf
srecv2   stx     singet
         rti
*************************************************************************
* Transmit interrupt.  Get data from buffer and put into uart.
* update flag, if last data disable transmit interrupts.
*
sxmit    ldx     soutget
         ldaa    0,X
         staa    SCDR          give to uart
*
         inx                   increment pointer and wrap if needed
         cpx     soutbfe
         bne     sxmit1
         ldx     soutbf
sxmit1   stx     soutget
*
         cpx     soutput       is buffer now empty
         bne     sxmit2        
*
* buffer empty, disable interrupts
*
         ldaa    SCCR2         disable transmit interrupt
         anda    #$7F    
         staa    SCCR2
*
sxmit2   ldx     Ctxsem        signal output task
         pshx
         jsr     ssignal
         pulx
         rti                   exit ISR (ssignal has already done mxswch)
*************************************************************************
* Cputc(char c) Transmit character to serial port.  If character
* is a LF then also transmit a CR.  If room in buffer then will
* transmit immediately, otherwise will block
* Ensure interrupts on SCI are enabled.
*
Cputc    tsy
         ldab    3,Y           get character
         bsr     Cputca        transmit it
         cmpb    #$0A          is it a line feed?
         beq     Cputc1        yes send CR
         rts
Cputc1   ldab    #$0D          send carriage return, fall into Cputca
*
* Print raw character to serial port with assembly syntax
* preserves A,B,X,Y
*
Cputca   pshy                  Preserve registers
         pshx
         psha
         pshb
*
         tpa                   critical section
         psha
         sei
         pshb                  save character
*
         ldaa    SCCR2         Ensure SCI transmit interrupt enabled
         oraa    #$80
         staa    SCCR2
*
         ldx     Ctxsem        is there room in buffer?
         jsr     Cwait         Wait for semaphore
*
         pulb                  retrieve character
         ldx     soutput
         stab    0,X           put data in buffer
*
         inx                   Update put pointer
         cpx     soutbfe
         bne     Cputca1
         ldx     soutbf        wrap pointer
Cputca1  stx     soutput       update pointer
*
         pula                  restore status
         tap
*
         pulb                  restore registers
         pula
         pulx
         puly
         rts
*************************************************************************
* Test semaphore and wait if locked
* decrement semaphore, if already zero then block
* Enter with X pointing to semaphore, Y,D destroyed
Cwait    ldd     semsema,X     test semaphore
         beq     Cwait1        is it already zero?
         subd    #1            no, 
         std     semsema,X     decrement semaphore
         rts
*
* Semaphore is already locked.  Set task waiting on semaphore.
* Add task to tail of semaphore task list
*
Cwait1   ldy     currTCB
         ldaa    #twait        mark task as waiting on semaphore
         staa    tstat,Y          
         ldd     #NULL         mark as end of list
         std     tsemalnk,Y
*
         xgdy                  put task in D
         ldy     semtail,X     get tail pointer into y
         beq     Cwait2        are we first task waiting?
*
* No, there are other tasks waiting
*
         std     semtail,X     set tail pointer
         std     tsemalnk,Y    link from previous end of list
         bra     Cwait3         exit via task switch
*
* Yes first task waiting
*
Cwait2   std     semhead,X     set up first task in list
         std     semtail,X
Cwait3   jmp     mxswch        wait on semaphore
*************************************************************************
* Cgetc().  Receive data. If data in buffer put in D and return, else
* put task into blocked state, waiting on receive data.
* Translate CR to newline
Cgetc    tpa                   critical section
         psha
         sei
*
         ldx     Crxsem
         jsr     Cwait         wait for semaphore
*
         ldx     singet
         ldab    0,X           get data
*
         cmpb    #$0D          is it a CR
         bne     Cgetc3
         ldab    #$0A          Convert to newline         
*
Cgetc3   inx                   bump pointer and wrap if needed
         cpx     sinbufe
         bne     Cgetc2
         ldx     sinbuf
Cgetc2   stx     singet
*
         pula
         tap
         clra                  clear upper half
         rts                   exit
*
* assembly interface to Cgetc, preserve X and Y, put character in B
Cgetca   pshx
         pshy
         jsr     Cgetc
         puly
         pulx
         rts   
*************************************************************************
* Ctestc() Test for a character available in buffer.  if none
* available returns -1 else returns character from buffer
Ctestc   tpa                   critical section
         psha
         sei
*
         ldx     Crxsem
         ldd     semsema,X     test semaphore
         beq     Ctestc1       is it already zero?
         jsr     Cgetca
         XGDX
         bra     Ctestc2
*
Ctestc1  ldx     #-1           return no data available
Ctestc2  pula
         tap
         XGDX                  get return value
         rts                   exit
*************************************************************************
* Read a string (with editing) from the console: Cgetstr(buffer, length)
* Returns length of string, CR translated to Line feed
Cgetstr  TSX                   Address stack
         LDY	4,X           Get buffer ptr
         LDX	#1            Starting count is one
*
Cgetstr1 JSR     Cgetca        Get a character
         CMPB	#$7F          Delete?
         BEQ	Cgetstr2      Yes, handle it
         CMPB	#$08          Backspace?
         BEQ	Cgetstr2      Yes, handle it
         CMPB	#$0A          Enter?
         BEQ	Cgetstr3      Yes, handle it
         PSHY                  Save buffer ptr
         TSY                   Address stack
         CPX	4,Y           Are we within length
         PULY                  Restore buffer ptr
         BHS	Cgetstr1      No, ignore
         STAB	,Y            Write to buffer
         INX                   Advance length
         INY                   Advance buffer ptr
         JSR	Cputca        Echo character to console
         BRA	Cgetstr1      And proceed
*
Cgetstr2 CPX     #1            Any data in buffer?
         BEQ	Cgetstr1      No, ignore
         LDAB	#$08          Backspace
         JSR	Cputca        Echo
         LDAB	#' '          Overwrite with space
         JSR	Cputca        Echo
         LDAB	#$08          Backspace
         JSR	Cputca        Echo
         DEX                   Reduce length
         DEY                   Reduce buffer ptr
         BRA	Cgetstr1      And proceed
*
Cgetstr3 CLR     ,Y            Zero end of buffer
         JSR	Cputca        New line on console
         DEX                   adjust length
         XGDX                  Return buffer length
         RTS
*************************************************************************
*
* Write a string to the serial port: Cputstr(char *s), use Cconsem
* to ensure synchronized printing
*
Cputstr  ldx     Cconsem       get address of console semaphore
         pshx
         jsr     swait         wait for semaphore
         pulx                  clean up stack
*
         TSX		Point to parameter
         LDX	2,X	Get pointer
Cputstr1 LDAB    ,X	Get char
         BEQ     Cputstr2      End of string, exit
         INX                   Advance pointer
         pshx                  preserve X
         pshb                  Put character on stack
         pshb
         jsr     Cputc         write character
         pulx                  clean up stack
         pulx                  restore X
         BRA     Cputstr1      And proceed
*
Cputstr2 ldx     Cconsem       signal console semaphore
         pshx
         jsr     ssignal
         pulx
         RTS                   exit
*
*************************************************************************
* Serial port printf: register Cprintf(format, args ...)
Cprintf  TSX                   Address stack
         ABX                   Offset for first parm
         ABX                   Twice for word entries
         TSY                   Y = stack
         XGDY                  D = stack
         SUBD    #100          Output buffer
         XGDY                  Y = output buffer
         TYS                   S = output buffer
         PSHY                  Save output pointer
         JSR     _format_      Call formatter
         JSR     Cputstr       Output the string
         TSX                   X = stack
         XGDX                  D = stack
         ADDD    #102          Clean args + buffer
         XGDX                  X = stack
         TXS                   S = stack
         RTS
*************************************************************************
$DD:     sinput   2            receive buffer put pointer
$DD:     singet   2            receive buffer get pointer
$DD:     soutput  2            transmit buffer put pointer
$DD:     soutget  2            transmit buffer get pointer
$DD:     sinbuf   2            receive buffer start
$DD:     sinbufe  2            receive buffer end
$DD:     soutbf   2            transmit buffer start
$DD:     soutbfe  2            transmit buffer end
$DD:     soutsiz  2            transmit buffer size
$DD:     Ctxsem   2            transmit semaphore address
$DD:     Crxsem   2            receive semaphore address
$DD:     Cconsem  2            Console semaphore address
*************************************************************************
$EX:     _format_
$EX:     ssignal
$EX:     swait
$EX:     sopen
$EX:     SCDR
$EX:     SCCR2
$EX:     SCSR
*************************************************************************

