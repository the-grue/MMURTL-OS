
int exit(int x)
{
/*      ExitJob(x);*/

        asm("movl %0, %%eax;"
            "pushl %%eax;"
            "ljmp $0x2B8, $0x00000000;"
            : : "r" (x) : "%eax");

        return 0;       /* Never happens */
}
