#include <stdio.h>
#include "\OSSource\Mjob.h"
#include "\OSSource\Mvid.h"
#include "\OSSource\gmode.h"

#define VIDBASE 0xA0000

extern far unsigned long Sleep(unsigned long count);

int main()
{
	unsigned char *scrbuf;
	unsigned int count;
	unsigned int x, y;

	scrbuf=VIDBASE;


	SetJobName("VidTest", 7);
	SetVidMode(0);
	ClrScr();
	printf("Should be in mode 0 with 80x25\r\n");
	printf("Sleeping for 10\r\n");
	Sleep(1000);
	SetVidMode(1);
	ClrScr();
	printf("Should be in mode 1 with 80x50\r\n");
	printf("Sleeping for 10\r\n");
	Sleep(1000);
	SetVidMode(0);
	ClrScr();
	printf("Should be back in mode 0 with 80x25\r\n");
	printf("Let's try a video mode\r\n");
	printf("And paint it blue with a red box and a white X!\r\n");
	Sleep(1000);
	SetVidMode(3);
	for(count = 0; count < 64000; count++)
		*scrbuf++ = 0x09;
	scrbuf=VIDBASE;
	for(x = 50; x <= 80; x++)
		for(y = 50; y <= 80; y++)
		{
			scrbuf=VIDBASE + (y * 320) + x;
			if(x == y)
				*scrbuf = 0x0f;
			else
				*scrbuf = 0x0c;
		}

	scrbuf=VIDBASE;
	y = 50;
	for(x = 80; x >= 50; x--)
		{
			scrbuf=VIDBASE + (y++ * 320) + x;
			*scrbuf = 0x0f;
		}
		
	Sleep(1000);
	SetVidMode(0);
	ClrScr();
	printf("Should be back in mode 0 with 80x25\r\n");
	printf("End of test\r\n");
	ExitJob(0);
}
