gcc -std=gnu99 -O0 -nostdlib -m32 -march=i386 -ffreestanding -fno-common -fno-asynchronous-unwind-tables -I../include -c -o mmlib.o mmlib.c
gcc -std=gnu99 -O0 -nostdlib -m32 -march=i386 -ffreestanding -fno-common -fno-asynchronous-unwind-tables -I../include -c -o stdlib.o stdlib.c
gcc -std=gnu99 -O0 -nostdlib -m32 -march=i386 -ffreestanding -fno-common -fno-asynchronous-unwind-tables -I../include -c -o ctype.o ctype.c
