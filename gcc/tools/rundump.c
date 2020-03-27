/* Runfile checker - validates your runfile to ensure it is safe to run
 *
 * Note - this is made to be run on Linux, not MMURTL
 *
 * Usage:
 * ./rundump runfile.run
 *
*/
#include <stdio.h>

int main(int argc, char** argv)
{
	FILE *fp;
	unsigned char code;
	unsigned int len;
	unsigned char cval;
	unsigned int val;
	unsigned int stacksz;
	unsigned int codesz;
	unsigned int datasz;
	unsigned int counter;

	if(argc < 2)
	{
		printf("Must provide filename as parameter!\n");
		return 1;
	}
	if((fp = fopen(argv[1], "rb")) == NULL)
	{
		printf("Filename %s not found!\n", argv[1]);
		return 2;
	}

	while(fread(&code, 1, 1, fp) != 0)
	{
		switch(code)
		{
			case 0x80:
				fread(&len, 4, 1, fp);
				fread(&cval, 1, 1, fp);
				printf("Code %X with len %X, and cval %X\n", code, len, cval);
				break;

			case 0x90:
				fread(&len, 4, 1, fp);
				fread(&stacksz, 4, 1, fp);
				fread(&codesz, 4, 1, fp);
				fread(&datasz, 4, 1, fp);
				if(len != 0x0C)
				{
					printf("Expecting Initial Segment Sizes Code 0x90 with size 0x0C and got %X, %X\n", code, len);
					fclose(fp);
					return 2;
				}
				printf("Code %X\n", code);
				printf("\tStack Size %X\n", stacksz);
				printf("\tCode Size %X\n", codesz);
				printf("\tData Size %X\n", datasz);
				break;

			case 0x92:
			case 0x94:
			case 0x96:
				fread(&len, 4, 1, fp);
				fread(&val, 4, 1, fp);
				printf("Found code %X with length %X and val %X\n", code, len, val);
				break;

			case 0xB0:
				fread(&len, 4, 1, fp);
				printf("Code Segment found with length 0x%X\n", len);
				printf("\tscanning...\n");

				for(counter = 0; counter < len; counter++)
				{
					fread(&cval, 1, 1, fp);
					printf("%X ", cval);
				}
				printf("\n");
				break;
			
			case 0xB2:
				fread(&len, 4, 1, fp);
				printf("Data Segment found with length 0x%X\n", len);
				printf("\tscanning...\n");

				for(counter = 0; counter < len; counter++)
				{
					fread(&cval, 1, 1, fp);
					printf("%X ", cval);
				}
				printf("\n");
				break;

			case 0xC0:
			case 0xC1:
			case 0xC2:
			case 0xC3:
				fread(&len, 4, 1, fp);
				printf("Fixup Segment %X found with length 0x%X\n",code , len);
				printf("\tscanning...\n");

				for(counter = 0; counter < len; counter++)
				{
					fread(&cval, 1, 1, fp);
					printf("%X ", cval);
				}
				printf("\n");
				break;

			case 0xFF:
				fread(&len, 4, 1, fp);
				fread(&val, 4, 1, fp);
				printf("File End Segment reached with %X and len %x\n", code, len);
				break;

			default:
				printf("Expecting File End Segment 0xFF and got %X\n", code);
				fclose(fp);
				return 2;
				break;
		}
	}

	printf("This is a good run file!\n");

	fclose(fp);
}
