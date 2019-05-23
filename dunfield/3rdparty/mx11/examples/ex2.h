/* Example program for MX-11
 */
#define UNLOCKED     1            /* Initial value for unlocked sema    */
#define LOCKED       0            /* Initial value for locked sema      */
#define TICK_RATE    0            /* Value for def_poll for 1/20 th sec */
#define ONE_SECOND   1            /*                        1 second    */
#define ONE_MINUTE   2            /*                        1 minute    */

int     mainh,consinh,consouth;/* Handles to tasks */
int     run_flag, ymin, ymax;
char    input_line[81];       /* Buffer for input line from serial port */
char    TimeofDay[4];
int     *pmxtimers;
int     *pTIMERS;

struct TCB {
      int   *tlink;
      char   tstat;
      char   tpriority;
      int    tsp;
      int    tstart;
      int    tspinit;
      int   *tqhead;
      int   *tqtail;
      int   *tsema;
      int   *tsemalnk;
      char  *tname;
      int    tspend;
      int    tspmin;
      int   *ttimer
      } *pTCB;


struct TIMER {
      int   tmlink;
      char  tmmode;
      int   tmval;
      int   *tmTCB;
      int   tmmsg;
      } *pTIMER;
      
struct POLLQ {
      int   *plink;
      int   *ptask;  /* Pointer to TCB of task */
      int   *pmesg;  /* Pointer to MCB */
      } *pPOLL;

char *DayofWeek[] ={"Sunday",
                    "Monday",
                    "Tuesday",
                    "Wednesday",
                    "Thursday",
                    "Friday",
                    "Saturday"}
      
