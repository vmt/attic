; A Simple Hello World Operating System - Package
; ===============================================
; By Vivek Mohan
; Mail me : mailvivek27@sify.com
; My Home : http://www.geocities.com/cppresources/
;
; The Package Includes
;	1. copyboot.cpp - boot sector copier
;	2. stripexe.cpp - exe header stripper
;	3. BOOT.ASM 	- the boot sector code
;	4. Readme	- This file
;
; Minimum Requirements
; 	1. TCC or DJGPP Dos Compilers
;	2. TASM for assembling BOOT.ASM , I am not sure wether it will
;	   assemble with other assemblers too.	
;	3. A Floppy disk , only a fool would copy the boot sector into the hard
;	   disk.
;
; The Boot Sector
; ===============
; The boot sector of any disk is the first sector on the first head of the disk 
; specifically (cylinder:0,head:0). When ever you switch on (or reset) the computer ,
; the BIOS performs the power-on self  test and then initializes its data. 
; After this it checks the first sector of each disk usually in the order: 
; floppy drives then primary disks (depending on the BIOS settings).
; When it finds a valid boot signature it loads the sector which 
; consists of 512 bytes , to the memory at location 0:7C00. After this the boot sector
; code gets control.
; 
; What does the boot sector code do ?
; ===================================
; The boot sector code can do anything. Usually boot sector code, loads the OS kernel
; and gives control to it. Here for example , it does nothing but print a message.
;
; What is the boot signature ?
; ============================
; When the bios checks the first sector of all the disks it looks for a specific
; signature denoting a valid boot sector. This signature is 0AA55h located at the
; offset 510. Which means that the total size of the boot sector code is 512bytes (or
; 1 sector).
;
; So to make a valid boot sector , we must make sure that it is 512 bytes long
; and 0AA55h is present at the offset 510 (or 1FEh) and thats what is being done
; in BOOT.asm
;
; How do I build the Hello World OS ?
; ===================================
;
; Follow the below instructions carefully and you'll be running the Hello World OS
; in no time.
;
; 1. Compile and link the file copyboot.cpp to copyboot.exe
; 2. Compile and link the file stripexe.cpp to stripexe.exe
; 3. Assemble BOOT.ASM using TASM
;	tasm  boot.asm
; 4. Link the object file thus generated boot.obj
;	tlink /x /3 boot.obj
; 5. Strip the header off the file boot.exe
;	stripexe boot.exe boot.bin hdr.bin
; 6. Copy the boot sector code into a floppy
;	copyboot boot.bin
; 7. Restart The Computer and see what happens...
;
; If you have any kind of problems please email me : mailvivek27@sify.com