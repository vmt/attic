###############################################################
##
##
##   THE OPOS SYSTEM LOADER - MAKEFILE
##
##
## HISTORY
##
## [Jun 22]
##   Restructured the file. Added a new rule for making opos
##   floppy image file for bochs (i386 emulator)
##
###############################################################

####
##
## Variables
##
####

AS = gcc -c -I includes
CC = gcc -c -I includes
LD = ld
RM = del
CF = -Wall -fno-builtin -fomit-frame-pointer -D __C_SEGMENT__ -D _DISKIO_DEBUG_
RM = del
DISKUTIL = ../dosutils/partcopy.exe

msg:
	@echo OS LOADER MAKEFILE
	@echo ==================
	@echo make fat12-boot-disk - MAKE A FAT12 (FLOPPY) BOOT DISK
	@echo make fat12-bochs-img - MAKE THE LOADER BINARY IMAGE
	@echo make clean           - CLEAN UP OBJECT AND BINARY FILES

fat12-boot-disk: loader.bin fat12-boot.bin ../dosutils/partcopy.exe
	@echo ## INSERT A CLEAN FORMATTED 1.44 MEG FLOPPY DISK INTO A:
	@echo ## WARNING, ALL DATA IN THE DISK WILL BE LOST.
	@pause
	$(DISKUTIL) boot.bin   0 4200 -f0 0
	$(DISKUTIL) loader.bin 0 8000 -f0 4400
	@echo ## DONE.
	@echo ## TO USE THIS DISK, YOU MUST INSTALL A SCRIPT FILE.
	@echo ## PLEASE READ THE MANUAL FOR MORE DETAILS.

fat12-boot.bin: disk-fat12/boot.S disk-fat12/boot.ld
	@echo ## MAKING FAT12 BOOT SECTOR and FAT TABLE IMAGE
	cd disk-fat12
	$(MAKE)
	cd ..
	@echo ## DONE.

loader.bin: 	loader.o main.o	conio.o	diskio.o malloc.o script.o loader.ld
	@echo ## MAKING THE LOADER IMAGE
	$(LD)   loader.o main.o conio.o diskio.o malloc.o script.o -o loader.bin -T loader.ld 
	@echo ## DONE

fat12-bochs-img:
	@echo ## MAKING FAT12 BOCHS IMAGE.
	@echo ## NOTE: this is only required for win 9x systems as Unix
	@echo ## and win 2k, can directly use the floppy device.
	@echo ## MAKE SURE YOU HAVE MADE THE OPOS FAT12 BOOT DISK.
	@echo ## PLEASE INSERT THE OPOS FAT12 BOOT DISK INTO A:
	@pause
	$(DISKUTIL) -f0 0 16C800 fdd.img 
	@echo ## USE FDD.IMG AS TEHF FLOPPY DISK IMAGE
	
####
##
## Implicit Rules
##
####

.S.o:
	$(AS) $< -o $(*F).o
.c.o:
	$(CC) $(CF) $< -o $(*F).o

####
##
## Cleaner
##
####

clean:
	$(RM) *.o
	$(RM) *.bin
	cd disk-fat12
	$(RM) *.o
	cd ..

####
##
## Dependencies
## 
####

boot.o:		boot.S \
		includes/loader.h \
		includes/datatable.h
loader.o:	loader.S \
		includes/loader.h \
		includes/datatable.h
diskio.o:	diskio.c \
		includes/loader.h \
		includes/datatable.h \
		includes/diskio.h
main.o:		main.c \
		includes/loader.h \
		includes/datatable.h \
		includes/diskio.h
conio.o:	conio.c \
		includes/conio.h
malloc.o:	malloc.c \
		includes/datatable.h
script.o:	script.c \
		includes/conio.h