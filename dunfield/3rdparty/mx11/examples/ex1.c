/*
 * MX 68HC11 Multitasking Executive Example Program
 */
#include \mc\stdio.h
#include \mc\lib11\mx.h
#include ex1.h

char *Status[]={  "Running  ",
                  "I/O Wait ",
                  "Semaphore",
                  "Sema/time",
                  "Rcv mesg ",
                  "Snd mesg ",
                  "Sleeping ",
                  "Blocked  ",
                  "Dormant  "}

char *TMODES[] ={ "Sleep",
                  "Alarm",
                  "Sema "}

int error;
char *task1h, *task2h, *task3h, *mainh, *sema, c, i;

task1(message)
int message;
{
Cprintf("Task 1: Message received value %d\n",message);
}

task2()
{
/* Signal sema after 3 seconds */
sleep(60);
Cputstr("Task 2: Signalling sema\n");
ssignal(sema);
}

task3()
{
}

demosleep()
{
Cputstr("\nGoing to sleep for 5 seconds\n");
sleep(100);
Cputstr("Task woken up\n");
}

demogettime()
{
char time[4];
getTOD(time);
Cprintf("\nThe time is %2d:%02d:%02d\n",time[3], time[2], time[1]);
}

demosendmsg() 
{
Cputstr("\nSending message (93) to Task 1\n");
sendmsg(task1h,93);
}

demosendmsgd()
{
Cputstr("\nSending message(25) to self with 3 second delay\n");
sendmsgd(60,25);
Cprintf("Message received value %d\n",getmsg());
}

demochkmsg()
{
Cprintf("\nchkmsg() with no message waiting... returns %04x\n",chkmsg());
sendmsg(mainh,25);
Cprintf(  "chkmsg() with message waiting   ... returns %04x\n",chkmsg());
flushmsg();
}

demoopensem() 
{
sema=sopen();
Cprintf("\nOpening semaphore, handle returned is %04x\n",sopen());
/* Close semaphore */
sclose(sema);
}

demoswait()
{
/* open locked semaphore.  Send message to task 2 to send a signal
 * after 2 seconds.  Wait on semapore */
Cputstr("\nWaiting on semaphore that will be signalled in 3 seconds\n");
sema=sopen(0);
sendmsg(task2h,0);
swait(sema);
Cputstr("Semaphore received\n");
sclose(sema);
}

demoswaittim()
{
/* Open semaphore, wait on it with timeout.  First do it without
 * signalling. Then signal it and repeat.
 */
sema=sopen(0);/* Open locked seapore */
Cprintf("\nDoing swaittim() with timeout occurring\n"); 
Cprintf("Return value was %d\n",swaittim(sema,40));
/* Doing swaittim() without timeout */
Cputstr("\nDoing swaittim() without timeout occurring\n"); 
ssignal(sema);
Cprintf("Return value was %d\n",swaittim(sema,40));
}

showmenu()
{
Cputstr("\n\n  MX-11 Example program 1\n\n");
Cputstr("  1            sleep()\n");
Cputstr("  2            gettime()\n");
Cputstr("  3            sendmsg()/getmsg()\n");
Cputstr("  4            sendmsgd()/getmsg()\n");
Cputstr("  5            chkmsg()\n");
Cputstr("  6            opensem()\n");
Cputstr("  7            swait()\ssignal()\n");
Cputstr("  8            swaittim()\n");
Cputstr("  T            Show tasks\n\n");
Cputstr("   Press selection,  press <ESC> to exit...");
}

showtasks()
{
/* Follow TCB's and display Tasks */
Cprintf("MX Variables start at %4x\n", mxvar());
pTCB=mxvar();
Cprintf(
"Task                TCB   Start Stack Status    Pri SPend SPmin Space\n");
Cprintf(
"----                ---   ----- ----- ------    --- ----- ----- -----\n");

while (pTCB=pTCB->tlink)
   {
   Cprintf("%-18s  %4x  %4x  %4x  %s  %d  %4x  %4x   %d\n",
   pTCB->tname, pTCB, pTCB->tstart, pTCB->tsp, Status[pTCB->tstat],
   pTCB->tpriority,pTCB->tspend,pTCB->tspmin,(pTCB->tspmin-pTCB->tspend));
   }
Cputc('\n');
}

main()
{
/* Initialize MX for 7 tasks, 32 MCB's and 256 byte stack space */
mainh=mxinit(256,7,32);
mxstkchk();
/*
 * Open serial port with 64 byte buffer on transmit and receive,
 * Print result code
 */
error=Copen(64,64);
if (error) printf("Port opened error = %d\n",error);

task1h=deftask( &task1,256,"Task 1"); /* define task1 */ 
task2h=deftask( &task2,256,"Task 2"); /* define task2 */ 
task3h=deftask( &task3,256,"Task 3"); /* define task3 */ 
/* Main now waits for Commands until escape pressed */

do {
   showmenu();
   switch (c=Cgetc()) {
      case '1':demosleep(); break;
      case '2':demogettime(); break;
      case '3':demosendmsg(); break;
      case '4':demosendmsgd(); break;
      case '5':demochkmsg(); break;
      case '6':demoopensem(); break;
      case '7':demoswait(); break;
      case '8':demoswaittim(); break;

      case 'T':
      case 't':showtasks(); break;
      }
if (c!=27) {Cputstr("\nPress any key to continue..."); Cgetc();}
}
while (c !=27); /* Repeat until escape key pressed */
Cputstr("\nProgram finished\n");
sleep(20);
disable();
}
