* Parallel I/O functions
inp1	IN	1		Read port 1
	RET
inp2	IN	2		Read port 2
	RET
inp3	IN	3		Read port 3
	RET
inp4	IN	4		Read port 4
	RET
outp1	LD	2,S		Write port 1
	OUT	1
	RET
outp2	LD	2,S		Write port 2
	OUT	2
	RET
outp3	LD	2,S		Write port 3
	OUT	3
	RET
outp4	LD	2,S		Write port 4
	OUT	4
	RET
