/* Example program3 for MX-11
 */

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

