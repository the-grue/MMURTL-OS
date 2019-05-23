/*
 * Example MX-11 program 12/7/93
 */

#include \mc\6811io.h
#include \mc\lib11\mx.h
#include ex2.h
#include ex2cons.c
#include ex2time.c
main()
{
mainh=mxinit(256,8,16);
mxstkchk();    /* Enable Stack Checking */
/* Initialize Globals */
run_flag=0;

cons_init();
time_init();

for(;;)sleep(20);

}

