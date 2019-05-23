/*
 * Example of using a union to overlay int (16 bit) and char (8 bit)
 * data. In this program, a 16 bit int is written to memory, and then
 * the high and low nibbles are exchanged by reading them as chars
 * defined in a union with the int.
 */

#include cflea.h

main()
{
	union {
		int Idata;
		struct {
			char byte1;
			char byte2; } Bdata; } data;

	int i;
	char t;

	for(i=0; i < 10; ++i) {
		data.Idata = i;
		t = data.Bdata.byte1;
		data.Bdata.byte1 = data.Bdata.byte2;
		data.Bdata.byte2 = t;
		printf("%04x <-> %04x\n", i, data.Idata); }
}
