int first;
int second;
int third;

/*extern void ExitJob(int x);*/

static volatile int exit(int x)
{
/*	ExitJob(x);*/

	asm("movl %0, %%eax;"
	    "pushl %%eax;"
	    "ljmp $0x2B8, $0x00000000;"
	    : : "r" (x) : "%eax");

	return 0;	/* Never happens */
}

static volatile int add(int x, int y)
{
	return x + y;
}

int main(void)
{
	first = 3;
	second = 2;
	third = add(first, second);
	while(1) ;
	exit(third);
}

