/* Example MX-11 program
 */

int clockh;
clock_task()
{ if (run_flag)
   {  getTOD(TimeofDay);
      Cprintf("%02d:%02d:%02d\n",TimeofDay[3],TimeofDay[2],TimeofDay[1]);
   }
}

time_init()
{  clockh=deftask(&clock_task,256,"One Second Task");
   defpoll(clockh,ONE_SECOND,0); /* Put clock task on second poll queue */
}
