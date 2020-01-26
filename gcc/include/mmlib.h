/* MMURTL System Call library for gcc
 *
 *
*/

#ifndef __MMLIB_H
#define __MMLIB_H

int ClrScr();
int GetCmdLine(char *pCmdLineRet, int *pdcbCmdLineRet);
int SetJobName(char *pname, int dcbName);
int Sleep(int dnTicks);
int ExitJob(int dExitError);

#endif
