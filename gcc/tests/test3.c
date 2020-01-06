int first;
int second;
int third;

extern int exit(int x);
extern int add(int x, int y);

int main(void)
{
	first = 3;
	second = 2;
	third = add(first, second);
	exit(third);
}

asm ("call main\n");
