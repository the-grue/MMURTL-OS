#  Makefile for static library of MMURTL System Calls

CC = gcc
AS = gcc
CFLAGS = -std=gnu99 -O0 -nostdlib -m32 -march=i386 -ffreestanding -fno-common -fno-asynchronous-unwind-tables -I../include

SRC = closefile.s clrscr.s createfile.s deletefile.s exitjob.s getcmdline.s getfilelfa.s \
      getfilesize.s openfile.s putvidchars.s readbytes.s renamefile.s setfilelfa.s \
      setfilesize.s setjobname.s sleep.s waitmsg.s writebytes.s sendmsg.s isendmsg.s \
      setpriority.s request.s respond.s checkmsg.s newtask.s allocexch.s deallocexch.s \
      alarm.s allocospage.s allocpage.s registersvc.s dmasetup.s readkbd.s addcallgate.s \
      addidtgate.s endofirq.s maskirq.s unmaskirq.s setirqvector.s getirqvector.s \
      initdevdr.s deviceinit.s deviceop.s devicestat.s beep.s tone.s killalarm.s \
	  microdelay.s spawntask.s getcmostime.s gettimertick.s outbyte.s outword.s  \
	  outdword.s inbyte.s inword.s indword.s readcmos.s copydata.s copydatar.s \
	  filldata.s comparencs.s compare.s setnormvid.s getnormvid.s chain.s getcmosdate.s \
	  createdir.s deletedir.s dealiasmem.s aliasmem.s getdmacount.s allocdmapage.s \
	  killjob.s getsystemdisk.s unregistersvc.s 

OBJ = $(SRC:.s=.o)

libmmlib.a: $(OBJ)
	@echo "[Linking MMURTL System Call Library]"
	@ar rcs $@ $^

stdentry.o: stdentry.c

.c.o:
	@echo "[Compiling]" $<
	@$(CC) -c $(CFLAGS) $< -o $@

.s.o:
	@echo "[Assembling]" $<
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJ) libmmlib.a stdentry.o

all: libmmlib.a stdentry.o
