/* MMURTL System Call library for gcc
 *
 *
*/

#ifndef __MMLIB_H
#define __MMLIB_H

int CloseFile(int dHandle);
int ClrScr();
int DeleteFile(long dHandle);
int GetCmdLine(char *pCmdLineRet, int *pdcbCmdLineRet);
int GetFileLFA(int dHandle, int *pdLFARet);
int GetFileSize(int dHandle, int *pdSizeRet);
int SetFileLFA(int dHandle, int dNewLFA);
int SetFileSize(int dHandle, int dSize);
int SetJobName(char *pname, int dcbName);
int Sleep(int dnTicks);
int ExitJob(int dExitError);

/*
 *	WriteBytes	250
 *	ReadBytes	248
 *	OpenFile	228
 *	RenameFile	278
 *	CreateFile	270
 */

#endif
