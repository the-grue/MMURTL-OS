/*
 * MX 68HC11 Multitasking Executive Example Program
 */
#include \mc\stdio.h
#include \mc\lib11\mx.h
#include ex3.h

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
char *task1h, *task2h, *task3h, *task4h, *task5h, *sema, c, i;

task1()
{
int i, run; i=0; run=0;
Cputstr("Task 1 started\n");
defpoll(task1h,TICK,0);
for (;;)
   switch(getmsg()){
   case 0:  if (run) i++; break;
   case 1:  if (run) run=0; else run=1;
   case 2:  Cprintf("Task 1: Counter = %5d Run state is %d\n",i,run); 
            break;
   case 3:  Cprintf("Task 1: Resetting Counter\n"); i=0;
   }
}

task2()
{
int i, run; i=0; run=0;
Cputstr("Task 2 started\n");
defpoll(task2h,TICK,0);
for (;;)
   switch(getmsg()){
   case 0:  if (run) i++; break;
   case 1:  if (run) run=0; else run=1;
   case 2:  Cprintf("Task 2: Counter = %5d Run state is %d\n",i,run); 
            break;
   case 3:  Cprintf("Task 2: Resetting Counter\n"); i=0;
   }
}

task3()
{
int i, run; i=0; run=0;
Cputstr("Task 3 started\n");
defpoll(task3h,TICK,0);
for (;;)
   switch(getmsg()){
   case 0:  if (run) i++; break;
   case 1:  if (run) run=0; else run=1;
   case 2:  Cprintf("Task 3: Counter = %5d Run state is %d\n",i,run); 
            break;
   case 3:  Cprintf("Task 3: Resetting Counter\n"); i=0;
   }
}

helptext()
{
Cputstr("MX-11 Example program\n");

Cputstr("This example has three tasks that independently count\n");
Cputstr("clock ticks.  The tasks start up disabled and\n");
Cputstr("counting can enabled/disabled by sending the task\n");
Cputstr("a message by pressing the 1,2 or 3 keys.\n");
Cputstr("The task status can be obtained by pressing S and\n");
Cputstr("the counters can be reset by pressing the R key.\n\n");

Cputstr("Press 1-3 to enable\disable counting\n");
Cputstr("Press S   to get status\n");
Cputstr("Press R   to reset tasks\n");
Cputstr("Press <ESC> to exit\n\n");
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
mxinit(256,7,32);
mxstkchk();
i=0; /* Initialize counter */
/*
 * Open serial port with 64 byte buffer on transmit and receive,
 * Print result code
 */
error=Copen(64,64);
if (error) printf("Port opened error = %d\n",error);

helptext();

task1h=deftask( &task1,256,"Task 1"); /* define task1 */ 
task2h=deftask( &task2,256,"Task 2"); /* define task2 */ 
task3h=deftask( &task3,256,"Task 3"); /* define task3 */ 

/* Start up all three tasks */
sendmsg(task1h,1);
sendmsg(task2h,1);
sendmsg(task3h,1);

/* Main now waits for Commands until escape pressed */

do {
   switch (c=Cgetc()) {
      case '1':sendmsg(task1h,1); break;
      case '2':sendmsg(task2h,1); break;
      case '3':sendmsg(task3h,1); break;

      case 'S':
      case 's':sendmsg(task1h,2);
               sendmsg(task2h,2);
               sendmsg(task3h,2);
               Cputstr("\n");
               break;

      case 'R':
      case 'r':sendmsg(task1h,3);
               sendmsg(task2h,3);
               sendmsg(task3h,3);
               Cputstr("\n");
               break;

      case 'T':
      case 't':showtasks(); break;
      case 27 :break; 
      default :helptext();
      }
}
while (c !=27); /* Repeat until escape key pressed */
Cputstr("Program finished\n");
sleep(20);
disable();
}
