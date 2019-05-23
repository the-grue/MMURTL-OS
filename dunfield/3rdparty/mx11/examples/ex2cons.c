/*
 * MX-11 Example
 * User and Display interface routines.
 */
/**********************************************************************/
/* Console Input Task */
unsigned char *optr;
int saveenv[2];
unsigned args[5], i;
char asciibuf[16];

char *Commands[]={"?",
                  "RUN",
                  "STOP",
                  "SS",
                  "SM",
                  "MM",
                  "SP",
                  "ST",
                  "Q",
                  "?",
                  "SET",
                   0 }

cons_in()
{
   unsigned v, j, k;
   char c, buffer[25], *ptr1, *inptr;

   Cputstr("MX Monitor\n");
   Cputstr("Enter '?' for help\n");

   switch(setjmp(saveenv))
   {  case 1 : Cputstr("Invalid command\n");            break;
      case 2 : Cputstr("Invalid parameter\n");          break;
      case 3 : Cputstr("Invalid number of arguments\n");break;
      case 4 : Cputstr("Aborted!\n");  }

   for(;;)
   {  Cputstr(">");
      if(!Cgetstr(optr = buffer, sizeof(buffer)-1))  continue;
      if(!(c = toupper(skip()))) continue;
/* Compare first argument with command list and put j = to command number,
 * set to 0 if command not recognized, to print help message.
 */
      j = k = 0;   /* 0 is help command */
      while ((ptr1=Commands[k]) && !j)/* do till end of commands we find a match */
      { inptr = optr;
         while (*ptr1) /* Repeat for length of Command word */
         {
          if (*ptr1++ != toupper(*inptr++)) {j=0; break;}/* no match */
          j = k;     /* characters match */
         }
/* See if we Have a match */
        if (!j) {k++; continue;}
        if (isalnum(*inptr)){j=0; k++; continue;}
        /* Argument continues try next command*/
   }
/*Cprintf("Processing Command %d\n",j);*/

      switch(j)
      {     case 0:
            case 9:                        /* Help */
                Cputstr("Run             - Start script\n");
                Cputstr("Stop            - Stop script\n");
                Cputstr("SS              - Show Script\n");
                Cputstr("SM <start> <end>- Show memory\n");
                Cputstr("MM <addr> <val> - Memory modify\n");
                Cputstr("SP              - Show Poll/Timer Queue\n");
                Cputstr("ST              - Show Tasks\n");
                Cputstr("Set <hh:mm>     - set time\n");
                Cputstr("Q               - Quit program\n");
                Cputstr("?               - Display this help text\n");
                break;

            case 1   :  run_flag=1; break;     /* Set run flag */
            case 2   :  run_flag=0; break;     /* Clear run flag */
            case 3   :  showmem();  break;     /* Show script */
            case 4   :  showmem();  break;     /* Show memory */
            case 5   :                         /* Modify Memory */
                        optr = *args;
                        if(i < 2) longjmp(saveenv, 3);
                        for(v = 1; v < i; ++v) *optr++ = args[v];
                                    break;
            case 6   :  showpoll(); break;     /* Show Timer List */
            case 7   :  showtasks();break;     /* Show Task list */
            case 8   :  disable(); 0xe000();   /* Quit, goto monitor */
            case 10  :  settime();  break;     /* Set time of day */
      } 
   }
}
/****************************************************************/
/* get next argument as hex integer */
int gethex()
{  int i,v;
   char d;
   while ((*optr) && (isalnum(*optr++)));
   i = 0;
   skip();
   v = 0;             /* Convert to ints */
   while((d = toupper(*optr)) && !isspace(d))
   {  ++optr;
      v <<= 4;        /* construct 16 bit int from Hex */
   if(isdigit(d)) v += d - '0';
    else if((d >= 'A') && (d <= 'F'))  v += d - ('A' - 10);
       else longjmp(saveenv, 2);
   }
return v;
}
/****************************************************************/
int getdec()
/* get next argument as decimal integer */
{
char d;
int v;
while ((*optr) && (isalnum(*optr++)));
skip();
v = 0;
while((d = toupper(*optr)) && isdigit(d))
   {
   ++optr;
   v *=10;
   v += d - '0';
   }
return v;
}
/****************************************************************/
showmem()    /* Dump memory in hex */
{
int i;
unsigned char *j,*k;
j=gethex();  k=gethex();
Cputstr("\n");
while(j <= k)
   {
   Cprintf("%04x:", j);
   for(i=0; i < 16; ++i)
      {
      asciibuf[i] = *j;
        Cprintf(" %02x", *j++);
      if (i == 7) Cprintf(" -"); /* Put space in middle */
      }
      Cputstr("  ");
   for (i=0; i < 16 ; i++)
      {   /* Print ascii */
      if (asciibuf[i] >31) Cputc(asciibuf[i]);
      else Cputc('.');
      }
   Cputstr("\n");
   if(Ctestc() == 0x1B) longjmp(saveenv, 4);
   }
}
/****************************************************************/
displaytime()
{
getTOD(TimeofDay);
Cprintf("Current Time: %2d:%02d:%02d\n",
TimeofDay[3],TimeofDay[2],TimeofDay[1]);
}
/* Set time of day "set hh:mm" */
settime()
{
getTOD(TimeofDay);
TimeofDay[3]=getdec(); /* Get Hours */
TimeofDay[2]=getdec(); /* Get Minutes */
TimeofDay[1]=0;        /* Zero Seconds */
setTOD(TimeofDay);
displaytime();
}
/****************************************************************/
/*
 * Skip to next non-blank
 */
skip()
{  while(isspace(*optr)) ++optr;
   return *optr;
}
/**********************************************************************/

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

showtasks()
{
/* Follow TCB's and display Tasks */
Cprintf("MX Variables start at %4x.  Run State %d\n", mxvar(), run_flag);
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
}

showpoll()
{
int   i;
/* Follow POLLQ's and timers */
pmxtimers=mxvar(); /* Get start of MX variables */
pmxtimers++; /* Point at timer variables TIMHEAD, POLL0..3 */
displaytime();
if (pTIMER=*pmxtimers) /* Display Timer List */
   do Cprintf("Task:%-15s  Value:%5d  Message:%5d  Mode:%s\n",
              pTIMER->tmTCB->tname,           /* Task name */
              pTIMER->tmval,                  /* Timer Value */
              pTIMER->tmmsg,                  /* Timer message */
              TMODES[pTIMER->tmmode]);        /* Timer mode */
   while (pTIMER=*pTIMER);
else Cprintf("No Timers defined\n");

pmxtimers++;                             /* Point to start of POLL queues */

for (i=0;i<4;i++) /* Do each of four POLL queues */
  {
   Cprintf("Poll List %d: ",i);
   if (*pmxtimers)  /* If pointer to poll list not null */
      {
      pPOLL=pmxtimers;
      while (pPOLL=pPOLL->plink)
         Cprintf(" Task:%-15s  Message:%5d  Pollqueue:%d",
                 pPOLL->ptask->tname, /* Task name */
                 pPOLL->pmesg,
                 i);
      Cputstr("\n");
      pmxtimers++;
      }
   else
      {
      Cputstr("No tasks defined\n");
      pmxtimers++;
      }
  }
}
/**********************************************************************/
/* Console Initialization */
cons_init()
{  Copen(8,80);
   consinh =deftask (&cons_in ,256,"Console Input");
   sendmsg(consinh,0); /* Start Tasks */
}
/**********************************************************************/

