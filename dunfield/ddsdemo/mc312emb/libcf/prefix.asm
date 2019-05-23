*
* DDS MICRO-C C-FLEA Runtime Library - Prefix file
*
* Copyright 1991-1995 Dave Dunfield
* All rights reserved.
*
	ORG	$1000		Place code in memory here
* Begin execution here... Setup stack, zero heap & call main()
	LD	#*		Stack below here
	TAS			Set up stack
	CLR			Get zero
	STB	?heap		Zero heap
	CALL	main		Run main program
* Most embedded programs will never return or exit, however if they
* do, this section of code should do something appropriate, either
* restart the program, or stop the machine. For this system, we will
* simply freeze by entering an infinite loop.
exit	SJMP	*		Loop forever
