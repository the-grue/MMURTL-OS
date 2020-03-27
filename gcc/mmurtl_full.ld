OUTPUT_FORMAT(binary)
SECTIONS
{
    . = 0x0000;
    .text :
    {
	BYTE(0x80);
	LONG(0x00000001);
	BYTE(0x01);
	BYTE(0x90);
	LONG(0x0000000C);
	LONG(0x00001000);
	LONG(__CEND__ - __CBEG__);
	LONG(__DEND__ - __DBEG__);
	BYTE(0x92);
	LONG(0x00000004);
	LONG(0x00000000);
	BYTE(0x94);
	LONG(0x00000004);
	LONG(0x00000000);
	BYTE(0x96);
	LONG(0x00000004);
	LONG((__CEND__ - __CBEG__) - 0x05);
	BYTE(0xB0);
	LONG(__CEND__ - __CBEG__);
	__CBEG__ = .;
        *(.text);
	__CEND__ = .;
	BYTE(0xB2);
	LONG(__DEND__ - __DBEG__);
    }
    .data 0x40002000 : AT ( ADDR (.text) + SIZEOF (.text) )
    {
	__DBEG__ = .;
        *(.data);
        *(.bss);
        *(.rodata);
	__DEND__ = .;
	BYTE(0xFF);
	LONG(0x00000004);
	LONG(0x00000000);
    }
    _heap = ALIGN(4);
}
