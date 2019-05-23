*
* Read a byte from memory: peek(addr)
*
peek	LDI	2,S		Get address
	LDB	I		Get value
	RET
*
* Read a word from memory: peekw(addr)
*
peekw	LDI	2,S		Get address
	LD	I		Get value
	RET
*
* Write a byte to memory: poke(addr, value)
*
poke	LDI	4,S		Get address
	LD	2,S		Get value
	STB	I		Write byte
	RET
*
* Write a word to memory: pokew(addr, value)
*
pokew	LDI	4,S		Get address
	LD	2,S		Get value
	ST	I		Write byte
	RET
