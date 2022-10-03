# MMURTL-OS
MMURTL Operating System (Message based MUltitasking, Real-Time kerneL)

Original source code written by Richard Burgess and released to the public
domain on the web site http://www.ipdatacorp.com/mmurtl/
(Note that the web site has been down since hurricane Ian)

This repository includes an .iso image of the original CD included with
the book, the PDF version of the book from the above website, as well as
all of the source code.

Major updates/changes:

* Now supports two disk controllers and up to 4 drives each with 4 partitions.
* FAT32 support 
* Simple mouse driver
* gcc support including
	- custom linker script to generate run files
	- runfile dumper and disassembler script to validate runfiles
	- some simple tests
	- compliation script to create executables
	- MMURTL System Call library and makefile

Bootable floppy disk images now available in the images directory.

    mmurtl.img   - Boots to PC-MOS/386 then use mmloader.exe to launch
                   or use mmlaunch.bat from the root of the drive.
    mmurtlsa.img - Standalone MMURTL bootable floppy.  Custom boot
                   loader launches MMURTL natively.
