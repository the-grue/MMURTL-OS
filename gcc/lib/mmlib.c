/*  MMURTL System Call library for gcc
 *
 *  
*/

int CloseFile(int dHandle)
{
	int rc = 0;
	asm volatile ("pushl %%ebx;"
	    "lcall $0x230, $0x00000000;"
	    : "=a" (rc) : "b" (dHandle) );

	return rc;
}

int ClrScr()
{
	int rc = 0;
	asm volatile ("lcall $0x1F0, $0x00000000;"
	    : "=a" (rc) : );

	return rc;
}

int DeleteFile(int dHandle)
{
	int rc = 0;
	asm volatile ("pushl %%ebx;"
	    "lcall $0x280, $0x00000000;"
	    : "=a" (rc) : "b" (dHandle) );

	return rc;
}

int GetCmdLine(char *pCmdLineRet, int *pdcbCmdLineRet)
{
	int rc = 0;
	asm volatile ("pushl %%ebx;"
	    "pushl %%ecx;"
	    "lcall $0x2D8, $0x00000000;"
	    : "=a" (rc) : "c" (pdcbCmdLineRet), "b" (pCmdLineRet) ); 

	return rc;
}

int GetFileLFA(int dHandle, int *pdLFARet)
{
	int rc = 0;
	asm volatile ("pushl %%ebx;"
	    "pushl %%ecx;"
	    "lcall $0x258, $0x00000000;"
	    : "=a" (rc) : "c" (pdLFARet), "b" (dHandle) ); 

	return rc;
}

int GetFileSize(int dHandle, int *pdSizeRet)
{
	int rc = 0;
	asm volatile ("pushl %%ebx;"
	    "pushl %%ecx;"
	    "lcall $0x268, $0x00000000;"
	    : "=a" (rc) : "c" (pdSizeRet), "b" (dHandle) ); 

	return rc;
}

int SetFileLFA(int dHandle, int dNewLFA)
{
	int rc = 0;
	asm volatile ("pushl %%ebx;"
	    "pushl %%ecx;"
	    "lcall $0x260, $0x00000000;"
	    : "=a" (rc) : "c" (dNewLFA), "b" (dHandle) ); 

	return rc;
}

int SetFileSize(int dHandle, int dSize)
{
	int rc = 0;
	asm volatile ("pushl %%ebx;"
	    "pushl %%ecx;"
	    "lcall $0x348, $0x00000000;"
	    : "=a" (rc) : "c" (dSize), "b" (dHandle) ); 

	return rc;
}

int SetJobName(char *pname, int dcbName)
{
	int rc = 0;
	asm volatile ("pushl %%ebx;"
	    "pushl %%ecx;"
	    "lcall $0x388, $0x00000000;"
	    : "=a" (rc) : "c" (dcbName), "b" (pname) ); 

	return rc;
}

int Sleep(int dnTicks)
{
	int rc = 0;
	asm volatile ("pushl %%ebx;"
	    "lcall $0x90, $0x00000000;"
	    : "=a" (rc) : "b" (dnTicks) );

	return rc;
}

int ExitJob(int dExitError)
{
	int rc = 0;
	asm volatile ("pushl %%ebx;"
	    "lcall $0x2B8, $0x00000000;"
	    : "=a" (rc) : "b" (dExitError) );

	return rc;
}
