/*  MMURTL System Call library for gcc
 *
 *  
*/

int ClrScr()
{
	int rc = 0;
	asm volatile ("lcall $0x1F0, $0x00000000;"
	    : "=a" (rc) : );

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
