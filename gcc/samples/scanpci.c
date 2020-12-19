/* scanpci.c - test job to exercise some MMURTL system calls
 *             from a gcc compiled runfile.
 *             This program will scan the PCI bus and identify
 *             any devices it finds.
 *
 * 
*/
#include <mmlib.h>

unsigned int counter;
char pcidev[10];
char hexchars[] = "0123456789ABCDEF";

void hexpcidev(unsigned short int VID, unsigned short int DID);
unsigned short int getpciconfig16(unsigned short int counter, unsigned char position);

int main(void)
{
	unsigned short int VID, DID;	/* Vendor and Device ID */
	pcidev[9] = 0;			/* String for VID:DID */
	int *p;				/* Some functions require this */

	SetJobName("SCANPCI", 7);	/* F2 in Monitor to see */
	ClrScr();			/* Clear the screen */
	WriteBytes(2, "\n\r", 2, p);	/* To avoid header clobbering first line */

	/* Performs a brute-force scan of all possible PCI devices and
	 * outputs their bus number and device number.  
	 * counter increments by 8 to only scan the first function
	 * for each device. */

	for(counter = 0; counter <= 0xFFFF; counter += 8)
	{
		VID = DID = 0;
		if((VID = getpciconfig16((unsigned short int) counter, 0)) != 0xFFFF)
		{
			DID = getpciconfig16((unsigned short int) counter, 2);
			hexpcidev(VID, DID);
			WriteBytes(2, "Found Device ", 13, p);
			WriteBytes(2, pcidev, 9, p);
			WriteBytes(2, " at BUS:DEVICE ", 16, p);
			hexpcidev((unsigned short int) (counter >> 8), 
				  (unsigned short int) ((counter & 0xFF) >> 3));
			WriteBytes(2, pcidev, 9, p);
			WriteBytes(2, "\n\r", 2, p);

			/*Sleep(10);*/	/* If you have a lot of devices this
					   will help keep the screen from scrolling
					   too fast */
		}
	}

	ExitJob(0);			/* Exit and return to CLI */

	return 0;			/* Never happens! */
}

/* This takes two 16 bit values and converts them to XXXX:YYYY format
 * suitable for printing */

void hexpcidev(unsigned short int VID, unsigned short int DID)
{
	unsigned short int value;

	value = VID;
	pcidev[3] = hexchars[value & 0x000F];
	pcidev[2] = hexchars[(value>>4) & 0x000F];
	pcidev[1] = hexchars[(value>>8) & 0x000F];
	pcidev[0] = hexchars[(value>>12) & 0x000F];

	value = DID;
	pcidev[8] = hexchars[value & 0x000F];
	pcidev[7] = hexchars[(value>>4) & 0x000F];
	pcidev[6] = hexchars[(value>>8) & 0x000F];
	pcidev[5] = hexchars[(value>>12) & 0x000F];

	pcidev[4] = ':';
	pcidev[9] = 0;

}

/* This gets a 16 bit entry from the PCI configuration based on position.
 * The actual port read returns 32 bits so this makes sure it is returning
 * the appropriate word.  
 *
 * counter holds the bus and device identifiers in a single 16 bit value
 * which is bit shifted into place and OR'd with the enable bit and with
 * the position low 2 bits masked off.
 *
 * 0xCF8 is the 32 bit configuration address port
 * 0xCFC is the 32 bit configuration data port
 *
 * */

unsigned short int getpciconfig16(unsigned short int counter, unsigned char position)
{
	unsigned int device;

	device = (unsigned int) counter << 8 | 0x80000000 | (position & 0xfc);

	OutDWord(device, 0xCF8);
	return ((unsigned short int) ((InDWord(0xCFC) >> ((position & 2) * 8)) & 0xFFFF));
}
