/* MMURTL System Call library for gcc
 *
 *
*/

#ifndef __MMLIB_H
#define __MMLIB_H

int AddCallGate();
int AddIDTGate();
int Alarm(int dExchRet, int count);
int AliasMem(char *pMem, int dcbMem, int dJobNum, char **ppAliasRet);
int AllocDMAPage(int nPages, int **ppMemRet, int *pPhyMemRet);
int AllocExch(int *pExchRet);
int AllocOSPage(int nPages, char **ppMemRet);
int AllocPage(int nPages, char **ppMemRet);
int Beep();
int Chain(char *pFileName, int cbFileName, int dExitError);
int CheckMsg(int dExch, char *pMsgsRet);
int CloseFile(int dHandle);
int ClrScr();
int Compare(char *pS1, char *pS2, int dSize);
int CompareNCS(char *pS1, char *pS2, int dSize);
int CopyData(char *pSource, char *pDestination, int dBytes);
int CopyDataR(char *pSource, char *pDestination, int dBytes);
int CreateDir(char *pName, int cbName);
int CreateFile(char *pFilename, int cbFilename, int Attribute);
int DeAliasMem(char *pAliasMem, int dcbAliasBytes, int JobNum);
int DeAllocExch(int dExch);
int DeAllocPage(char *pMem, int dnPages);
int DeleteDir(char *pName, int cbName);
int DeleteFile(int dHandle);
int DeviceInit(int dDevNum, char *pInitData, int sdInitData);
int DeviceOp(int dDevice, int dOpNum, int dLBA, int dnBlocks, char *pData);
int DeviceStat(int dDevice, char *pStatRet, int dStatusMax, int *pdStatusRet);
int DMASetUp(int dPhyMem, int sdMem, int dChannel, int fdRead, int dMode);
int EditLine(char *pStr, int dCrntLen, int dMaxLen, int *pdLenRet, 
             char *pbExitChar, int dEditAttr);
int EndOfIRQ(int dIRQnum);
int ExitJob(int dExitError);
int FillData(char *pDest, int cBytes, char bFill);
int GetCmdLine(char *pCmdLineRet, int *pdcbCmdLineRet);
int GetCMOSDate(int *pDateRet);
int GetCMOSTime(int *pTimeRet);
int GetDirSector(char *pPathSpec, int cbPathSpec, char *pSectorRet,
                 int sSectorRet, int SectorNum);
int GetDMACount(int dChannel, int *pwCountRet);
int GetExitJob(char *pRunRet, int *pdcbRunRet);
int GetFileLFA(int dHandle, int *pdLFARet);
int GetFileSize(int dHandle, int *pdSizeRet);
int GetIRQVector(int IRQnum, int *pVectorRet);
int GetJobNum(int *pJobNumRet);
int GetNormVid(int *pNormVidRet);
int GetPath(int JobNum, char *pPathRet, int *pdcbPathRet);
int GetPhyAdd(int JobNum, char *LinAdd, int *pPhyRet);
int GetpJCB(int dJobNum, char *pJCBRet);
int GetSysIn(char *pFileRet, int *pdcbFileRet);
int GetSysOut(char *pFileRet, int *pdcbFileRet);
int GetSystemDisk(char *pDiskRet);
int GetTimerTick(int *pTickRet);
int GetTSSExch(int *pExchRet);
int GetUserName(char *pUserRet, int *pdcbUserRet);
int GetVidChar(int ddCol, int ddLine, char *pCharRet, char *pAttrRet);
int GetVidOwner(int *pdJobNumRet);
int GetXY(int *pXRet, int *pYRet);
int InByte(int dPort);
int InDWord(int dPort);
int InitDevDr(int dDevNum, char *pDCBs, int nDevices, int dfReplace);
int InWord(int dPort);
int InWords(int dPort, char *pDataIn, int dBytes);
int ISendMsg(int dExch, int dMsgHi, int dMsgLo);
int KillAlarm(int dAlarmExch);
int KillJob(int dJobNum);
int LoadNewJob(char *pFileName, int cbFileName, int *pJobNumRet);
int MaskIRQ(int dIRQnum);
int MicroDelay(int dn15us);
int MoveRequest(int dRqBlkHndl, int DestExch);
int NewTask(int JobNum, int CodeSeg, int Priority, int fDebug, int Exch,
            unsigned int ESP, unsigned int EIP);
int OpenFile(char *pFilename, int dcbFilename, int Mode, int Type, int *pdHandleRet);
int OutByte(char Byte, int wPort);
int OutDWord(int DWord, int wPort);
int OutWord(int Word, int wPort);
int OutWords(int dPort, char *pDataOut, int dBytes);
int PutVidChars(int ddCol, int ddLine, char *pChars, int sChars, int ddAttrib);
int PutVidAttrs(int ddCol, int ddLine, int sChars, int dAttr);
int QueryPages(int *pdPagesLeft);
int ReadBlock(int dHandle, char *pBytesRet, int nBytes,
              int dLFA, int *pdnBytesRet);
int ReadBytes(int dHandle, char *pDataRet, int nBytes, int *pdnBytesRet);
char ReadCMOS(int Address);
int ReadKBD(int *pdKeyCodeRet, int fWait);
int RegisterSvc(char *pSvcName, int dExch);
int RenameFile(char *pCrntName, int cbCrntName, char *pNewName, int cbNewName);
int Request(char *pSvcName, int wSvcCode, int dRespExch, int *pRqHndlRet, int dnpSend,
            char *pData1, int dcbData1, char *pData2, int dcbData2, int dData0,
            int dData1, int dData2);
int Respond(int dRqHndl, int dStatRet);
int ScrollVid(int ddULCol, int ddULline, int nddCols,
              int nddLines, int ddfUp);
int SendMsg(int dExch, int dMsgHi, int dMsgLo);
int SetCmdLine(char *pCmd, int dcbCmd);
int SetExitJob(char *pRunFile, int dcbRunFile);
int SetFileLFA(int dHandle, int dNewLFA);
int SetFileSize(int dHandle, int dSize);
int SetIRQVector(int IRQnum, int *pVectorRet);
int SetJobName(char *pname, int dcbName);
int SetNormVid(int dAttr);
int SetPath(char *pPath, int dcbPath);
int SetPriority(int bPriority);
int SetSysIn(char *pFile, int dcbFile);
int SetSysOut(char *pFile, int dcbFile);
int SetUserName(char *pUser, int dcbUser);
int SetVidOwner(int dJobNum);
int SetXY(int dNewX, int dNewY);
int Sleep(int dnTicks);
int SpawnTask(char *pEntry, int dPriority, int fDebug, char *pStack, int fOSCode);
int Tone(int dHz, int dTicks10ms);
int TTYOut(char *pTextOut, int ddTextOut, int ddAttrib);
int UnMaskIRQ(int dIRQnum);
int UnRegisterSvc(char *pName);
int WaitMsg(int dExchg, char *pqMsgRet);
int WriteBlock(int dHandle, char *pData, int nBytes,
               int dLFA, int *pdnBytesRet);
int WriteBytes(int dHandle, char *pData, int nBytes, int *pdnBytesRet);

#endif
