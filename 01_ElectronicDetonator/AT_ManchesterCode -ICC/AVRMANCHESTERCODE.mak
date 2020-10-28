CC = iccavr
LIB = ilibw
CFLAGS =  -e -D__ICC_VERSION=722 -D_EE_EXTIO -DATMega1280  -l -g -MLongJump -MHasMul -MEnhanced -Wf-use_elpm 
ASFLAGS = $(CFLAGS) 
LFLAGS =  -g -e:0x20000 -ucrtatmega.o -bfunc_lit:0xe4.0x20000 -dram_end:0x21ff -bdata:0x200.0x21ff -dhwstk_size:30 -beeprom:0.4096 -fihx_coff -S2
FILES = main.o ManchesterCode.o ManchesterInit.o 

AVRMANCHESTERCODE:	$(FILES)
	$(CC) -o AVRMANCHESTERCODE $(LFLAGS) @AVRMANCHESTERCODE.lk   -lcatm128
main.o: .\..\AT_MAN~2\src\ManchesterInit.h C:\iccv7avr\include\iot45v.h C:\iccv7avr\include\_iotX5v.h C:\iccv7avr\include\macros.h C:\iccv7avr\include\AVRdef.h .\..\AT_MAN~2\src\ManchesterCode.h
main.o:	..\AT_MAN~2\src\main.c
	$(CC) -c $(CFLAGS) ..\AT_MAN~2\src\main.c
ManchesterCode.o: .\..\AT_MAN~2\src\ManchesterCode.h .\..\AT_MAN~2\src\ManchesterInit.h C:\iccv7avr\include\iot45v.h C:\iccv7avr\include\_iotX5v.h C:\iccv7avr\include\macros.h C:\iccv7avr\include\AVRdef.h
ManchesterCode.o:	..\AT_MAN~2\src\ManchesterCode.c
	$(CC) -c $(CFLAGS) ..\AT_MAN~2\src\ManchesterCode.c
ManchesterInit.o: .\..\AT_MAN~2\src\ManchesterInit.h C:\iccv7avr\include\iot45v.h C:\iccv7avr\include\_iotX5v.h C:\iccv7avr\include\macros.h C:\iccv7avr\include\AVRdef.h .\..\AT_MAN~2\src\ManchesterCode.h
ManchesterInit.o:	..\AT_MAN~2\src\ManchesterInit.c
	$(CC) -c $(CFLAGS) ..\AT_MAN~2\src\ManchesterInit.c
