/*
 * opos/loader/loader.S  (SYSTEM LOADER)
 *
 * Copyright (C) 2002 Vivek Mohan <opendev@phreaker.net>
 *
 * LICENSE
 * =======
 *
 * This program is free software; you can redistribute it  and/or modify  
 * it under the terms of the  GNU General Public License as published by
 * the Free Software Foundation ; either version  2  of the  License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope  that it  will be useful, but
 * WITHOUT  ANY  WARRANTY  ;   without  even  the  implied  warranty  of
 * MERCHANTABILITY   or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
 * GNU General Public License for more details.
 *
 * You should have received  a  copy  of  the GNU General Public License
 * along  with  this  program;  if  not,  write  to  the  Free  Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */


/* NOTES
 * 
 * * 1
 *
 * The routines switch_to_real_mode and switch_to_protected_mode are very 
 * important and useful. The switch_to_real_mode function is called when
 * ever a real mode interrupt is to be used. Note that the code following
 * the function switch_to_real_mode must in mode 16 until a switch to protected
 * mode is made.
 *
 * The sub routines..
 * 
 *	* _getc, waits for the user to press any key.
 *
 *	* _printc, prints the character passed as argument. 
 *
 *	* _prints, prints the null terminated string passed
 *	  as argument.
 *
 *	* _printi, prints the 32 bit integer passed as argument.
 *
 *	* switch_to_protected_mode, switches the cpu mode to protected
 *	  mode with all the selectors loaded. Note that registers eax
 *	  and edx are the only data registers affected. esp and ebp are
 *	  are obviously changed due to setting up of pmode stack.
 *
 *	* switch_to_real_mode, switches the cpu mode to real mode with all
 *	  the selector loaded with real mode selectors. In this case too
 *	  eax and edx are affected.
 */ 


/* HISTORY
 *
 * [Jun 20] 
 *   Dropped the idea of verbose levels. No echoing in assembly segment. 
 *   I also added some macros. The print functions are bug free (I guess 
 *   so). We have a new entity called DATATABLE which will be passed on 
 *   to the Kernel containing lots of vital information about the system, 
 *   defined in datatable.h
 *				-Vivek 
 * [Jun 21]
 *   Implemented the 5 step routine for determining the system memory. 
 *				-Vivek
 * [Jun 22]
 *   Implemented a routine that uses function 0xC0 of int 15 to determine
 *   the bus architecture.
 *				-Vivek
 * [Jun 24]
 *   Added code to detect the hard disks and int 13h extended services.
 *				-Vivek
 * [Jun 25]
 *   Added a routine for standard bios disk functions.
 */

#define __ASSEMBLY_SEGMENT__

#include <loader.h>
#include <datatable.h> 
  
  REALMODE()	/* Switch to real mode */

/*
 * As the first step in hardware detection we'll try to detect the system
 * memory.
 *  * The base memory is stored in DATATABLE(BASE_MEM_SZ)
 *  * The extended memory in DATATABLE(EXT_MEM_SZ)
 *  * If supported, the system memory map buffer in DATATABLE(SYS_MEM_MAP)
 *    and its size in DATATABLE(SYS_MEM_MAP_SZ). If the size is 0 the buff
 *    is empty.	
 *  * The xtended memory is determined through int 15 ah = 0xe801 -> 0x88
 *    or direct probing.
 */

	/*
	 * Initialize the data table bits to 0
	 */

	movb	$0, 		%al
	movw	$__DATA_TABLE, 	%si
	movw	$DATA_TABLE_SZ, %cx
	rep 	stosb

	/*
	 * STEP 1
	 *
	 * Get system conventional memory size using the isr 0x12 
	 * which returns the size in AX (Kbs)
	 */

	movl	$0, %eax
	int	$0x12
	movw	%ax, DATATABLE(BASE_MEM_SZ)

	/* STEP 2
	 *
	 * Get system memory map using the isr 0x15 function 0xe820
	 */

	movl	$0, %ebx			/* start */
	movl	$0, DATATABLE(SYS_MEM_MAP_SZ)	 
  sys_mem_map_detect:				
	movl	$0x0000E820, %eax		/* The function 0xE820, some bioses */
						/* need the high word to be 0. */
	movl	$0x534D4150, %edx		/* 'SMAP' */
	movl	$20, %ecx			/* The buffer size (min 20) */
	movl	$SYS_MEM_MAP, %edi		/* Destination */
	int	$0x15				
        jc	end_sys_mem_map_detect		/* error or it could also mean end-of-map */
	cmpl	$0x534D4150, %eax		/* 'SMAP' check for installation */
	jne	end_sys_mem_map_detect		 
        cmpl	$0x00000000, %ebx		/* check for end-of-map */
        je	end_sys_mem_map_detect		
	addl  	$20, %edi			
	addl	$20, DATATABLE(SYS_MEM_MAP_SZ)
	jmp	sys_mem_map_detect		/* do it again */
  end_sys_mem_map_detect:

	/*
	 * STEP 3
	 *
	 * Detect the extended memory using function 0xE801.
	 */
	 
  ext_mem_detect_0xe801:			
        movl	$0xe801, %eax			/* function e801 */
        int	$0x15				
        jc	ext_mem_detect_0x88		/* If not supported try function 0x88 */
	andl	$0xFFFF, %ebx			/* 0 the high word or ebx */
	andl	$0xFFFF, %eax			/* and eax */
	shl	$6, %ebx			/* multiply by 64 */
	addl	%eax, %ebx			
	movl	%ebx, DATATABLE(EXT_MEM_SZ)	/* store it */
	jmp	end_mem_detect			/* end detection */

	/*
         * STEP 4
         *
         * If the function 0xE801 is not supported we might as well assume
	 * that the system has memory <= 64 Mb, so we use int 15, ah= 0x88
	 *
	 */

  ext_mem_detect_0x88:
	movw	$0x8800, %ax			/* ah = 0x88 */
	int	$0x15
	jc	ext_mem_probe 
	movw	%ax, DATATABLE(EXT_MEM_SZ)
	jmp	end_mem_detect			/* end detection */

	/*
         * STEP 5
         *
         * Well we are at the last stage of memory detection. The bios has to be
	 * one of a kind to not support the above functions. Well we'll just have
	 * to probe the memory. 
	 * Technique... any address which is writable is memory
	 */

  ext_mem_probe:
  PROTMODE()
	movl  $0x100000, %esi			/* start with 1 MB limit */
  probe_loop:					
	movl  $0xBABEFEDA, (%esi)		/* write something unique ;-) */
	cmpl  $0xBABEFEDA, (%esi)		/* was it written ? */
        jne   end_probe				/* nope, that has to be the end of memory. */
	addl  $1024, %esi			/* Probe on 1K boundaries */
	jmp   probe_loop
  end_probe:	
	subl  $0x100000, %esi
	movl  %esi, DATATABLE(EXT_MEM_SZ)
  REALMODE()					/* we have extended memory */
						/* with no knowledge of holes... */
  end_mem_detect:

  /*
   * Well we have detected the memory (hopefully)
   *
   * Next we determine a few more things such as the bus architecture,
   * rtc and pic installation details etc..
   */

	/*
	 * To do that we'll be using the bios interrupt 15 function
	 * 0xC0, which returns a far pointer to a buffer containing
	 * info about the hardware..
	 */

        movl	$0, %ebx
	movb	$0xC0, %ah 
	int 	$0x15 
	jc	end_rom_table_detection 	/* not supported */
	movl	%es, %eax 			/* address is returned in es:bx */
	shl 	$4, %eax 			/* The linear address is calculated */
	addl	%ebx, %eax 			/* as eax = (es << 4) | (ebx) */
	movl	%eax, DATATABLE(ROM_TABLE_PTR)/* save the address */
  end_rom_table_detection:

        /*
	 * determine the bus architecture
	 */

	testb	$1, %es:5(%bx)	
	jz	test_bus_mca_isa
	movb    $1, DATATABLE(BUS_ARCH)
	jmp	end_bus_detect
  test_bus_mca_isa:
	testb	$2, %es:5(%bx)
	jz	bus_is_isa
	movb	$2, DATATABLE(BUS_ARCH)
	jmp	end_bus_detect
  bus_is_isa:
	movb	$3, DATATABLE(BUS_ARCH)
  end_bus_detect:
	
	/*
	 * check if the real time clock has been
	 * installed.
	 */

	testb	$6, %es:5(%bx)
	jz	end_rtc_install_check
	movb	$1, DATATABLE(RTC_INSTALLED)
  end_rtc_install_check:

	/*
	 * check if the 2nd interrupt controller has been
	 * installed.
	 */

	testb	$7, %es:5(%bx)
	jz	end_pic2_install_check
	movb	$1, DATATABLE(PIC2_INSTALLED)
  end_pic2_install_check:
			

  /*
   * Detect mass storage types
   */

	movl	$0, DATATABLE(N_FLOPPY_DRVS)

	/*
	 * Determine the number of floppy drives using the bios data table
	 * number-of-floppies-installed bits located at 0x0000:0x410.
	 */

	movb	0x410, %al			/* BDA 0x400:10 */
	testb	$1, %al				/* test bit 0 */
	jz	no_floppies_detected		/* if 0 no floppies installed in system */
	shrb	$6, %al 
	addb	$1, %al 
	movb	%al,DATATABLE(N_FLOPPY_DRVS) 	/* save number of floppies */
	cmpb	$1, %al
	je	no_floppy_1			/* If number of floppies is 1 skip */
						/* floppy1 parameters detection */

	/*
	 * Determine the floppy disk type and its parameters..
	 */

	movb	$0x1, %dl			/* for floppy 1 */
	movb	$0x8, %ah			/* function 0x8 */
	pushw	%es				/* save es */
	int	$0x13				/* es gets changes in this function */
	popw	%es				/* restore es */
	movb	%bh, DATATABLE(FLOPPY1_TYPE)
	movb	%ch, DATATABLE(FLOPPY1_MAX_CYL)
	movb	%cl, DATATABLE(FLOPPY1_MAX_SECT)
	movb	%dh, DATATABLE(FLOPPY1_MAX_HD)

  no_floppy_1:

	movb	$0x0, %dl			/* for floppy 0 */
	movb	$0x8, %ah
	pushw	%es
	int	$0x13
	popw	%es
	movb	%bh, DATATABLE(FLOPPY0_TYPE)
	movb	%ch, DATATABLE(FLOPPY0_MAX_CYL)
	movb	%cl, DATATABLE(FLOPPY0_MAX_SECT)
	movb	%dh, DATATABLE(FLOPPY0_MAX_HD)

  no_floppies_detected:

/*
 * NOTES
 *
 * *1 Detecting the number of hard disks
 *
 *    The number of hard disks is determined using the bios data segment. The
 *    byte located at offset 0x75 of the bda holds the number of hd installed
 *    on the system.
 *									- Vivek
 *
 * *2 Determining the drive parameters
 *
 *    The drive parameters for the first two hard disk drives, can be detected
 *    using two buffers pointed by int 41 and int 46. These interrupt numbers
 *    cannot be executed but the address points to the buffer.
 *
 *    For hard disk 0 the seg:off address stored at 0:(41*4) = 0:104h points to
 *    the table. Similarily for hard disk 1 the seg:off address stored at 0:46*
 *    4 = 0:118h points to its table.
 *									- Vivek
 *
 * *3 The bios int 13h extensions
 *
 *    Newer bioses support extended services at int 13h interface, which can be
 *    used to determine more aspects of the hard disks and to get extended soft
 *    ware support for conducting disk operations.
 *
 *    The int 13h extensions can be checked for installation using the function
 *    41h, which returns a great deal of information about wether or not it is
 *    supported and if supported the version and the api subset details.
 *
 *    Function 48h of the int 13h interface returns an extended version of disk 
 *    parameter table for the selected hard disk. If the version of the int 13h
 *    extensions is 2.0+ and the api subset supports the enchanced disk drive 
 *    functions, the offset 1A of the table holds the address to another buffer
 *    called the enhanced disk drive configuration table.
 *									- Vivek
 *
 * *4 Reference
 *
 *    For more details on the above mentioned topics, please refer to the pheonix
 *    website (www.pheonix.com) documents on edd specifications.
 */

	/*
	 * Save and load registers  ..
	 */

	/*
	 * initialize table
	 */

	movb	$0, DATATABLE(N_HARD_DRVS)
	movb	$0, DATATABLE(HD0_INT13_E_INSTALLED)
	movb	$0, DATATABLE(HD0_INT13_E_MAJ_VER)
	movw	$0, DATATABLE(HD0_INT13_E_API_SUBSET)
	movb	$0, DATATABLE(HD0_INT13_E_DPT_SUPP)
	movb	$1, DATATABLE(HD0_INT13_E_EDD_SUPP)
	movw	$0x30, DATATABLE(HD0_INT13_E_DPT)

	/*
	 * Determine the number of hard drives using the bios data table
	 * number-of-harddisks-installed byte located at 0x0000:0x475.
	 */

	movb	0x475, %al			 /* BDA 0x400:75 */
	movb	%al, DATATABLE(N_HARD_DRVS)	 /* save... */
	cmpb	$0, %al	
	je	no_hdds_detected		 /* No hard disks */
	cmpb	$1, %al 
	je	no_hdd_1			 /* Only hd0 availbl */

	/*
	 * Get the disk parameter tables for the drives
	 */

	pushw	%ds				 /* save ds */
	lds	(0x46 * 4), %si			 /* load ds:si -> address of dpt1 */
	movw	$HD1_DPT, %di			 /* load destination buffer index */
	movw	$0x10, %cx			 
	rep	movsb				
	popw	%ds				 /* restore ds */

 no_hdd_1:


	pushw	%ds				 /* save ds */
	lds	(0x41 * 4), %si			 /* load ds:si -> address of dpt0 */
	movw	$__DATA_TABLE + HD0_DPT, %di	 /* load destination buffer index */
	movw	$0x10, %cx
	cld
	rep	movsb
	popw	%ds				 /* restore ds */

	/*
	 * Check int 13h extension installation
	 */

	movb	$0x41, %ah			 /* The function */
	movw	$0x55AA, %bx			 /* The signature */
	movb	$0x80, %dl
	int	$0x13				
	jc	no_extensions			 /* carry = not supported */
	cmpw	$0xAA55, %bx			 /* compare the return signature */
	jne	no_extensions			 /* not equal = not supported */

	/*
	 * save values in data table
	 */

	movb	$1, DATATABLE(HD0_INT13_E_INSTALLED)
	movb	%ah,DATATABLE(HD0_INT13_E_MAJ_VER)
	movw	%cx,DATATABLE(HD0_INT13_E_API_SUBSET)	

	/*
	 * get the extended disk parameter table
	 */

	movb	$0x48, %ah			     /* load functions */
	mov	$__DATA_TABLE + HD0_INT13_E_DPT, %si /* source index into the result buffers */
	int	$0x13			
	jc	no_e_dpt			 /* carry => error */
	movb	$1, DATATABLE(HD0_INT13_E_DPT_SUPP)

	/*
	 * check if the version is >= 2.0
	 * If the major version of the extension support is less 
	 * than 2.0 the edd configuration parameters are not supported
	 */

	cmp	$0x20, DATATABLE(HD0_INT13_E_MAJ_VER)	 
	jb	no_edd_config
	movb	$1, DATATABLE(HD0_INT13_E_EDD_SUPP)

	/*
	 * If supported the dword at offset 0x1A of the edpt
	 * holds the seg:off address of the edd config table
	 * The table is 16 bytes long, we just copy it into
	 * the data table.
	 */

	pushw	%ds
	lds	DATATABLE(HD0_INT13_E_DPT + 0x1A), %si
	mov	$__DATA_TABLE+HD0_INT13_E_EDD, %di
	mov	$0x10, %cx
	rep	movsb
	popw	%ds

  no_e_dpt:
  no_edd_config:
  no_extensions:
  no_hdds_detected:

  PROTMODE();

  jmp _main   /* jump to main() */

/*
 * 32 bit functions
 *
 */ 

 .CODE32

/****
 *  unsigned char biosdisk(function, disk, cyl
 *             head, sect, nsect)
 *  unsigned short cyl;
 *  unsigned char function, disk, cyl, head, sect, nsect;
 *
 ****/

C_PROC(biosdisk)
	pushl	%ebp
	movl	%esp, %ebp
	movb	8(%ebp), %ah	
	movb	12(%ebp), %dl
	movw	16(%ebp), %cx
	xchg	%ch, %cl
	shlb	$6, %cl
	movb	20(%ebp), %dh
	orb	24(%ebp), %cl
	movb	28(%ebp), %al
  REALMODE()
	pushw	%ds
	popw	%es
	movw	$__BIOS_DISK_BUFF, %bx
	int	$0x13
  PROTMODE()
	xchg	%ah,    %al
	movl	%ebp,	%esp
	popl	%ebp	
	ret	

/****
 *  unsigned char getc()
 * 
 *	waits for the user to press a key and returns the code..
 *
 ****/

C_PROC(getc)
  REALMODE()
	movl	$0x0, %eax
	int	$0x16
	movl	%eax, %ecx
  PROTMODE()
        movl	%ecx, %eax	
	ret 

/****
 *  void print_char(char c)
 *
 *	prints the character passed as the argument.
 *
 ****/

C_PROC(printc)
	pushl	%ebp
	movl	%esp,	%ebp
	movl	8(%ebp),%ebx
  REALMODE()
	movb	%bl,  %al
	movb	$0xE, %ah
	int	$0x10
  PROTMODE()
	movl	%ebp,	%esp
	popl	%ebp
	ret 

/****
 *  void print_string(char *string)
 *
 *	pretty basic, prints the null terminated string passed as 
 *	argument.
 *
 ****/

C_PROC(prints)
	pushl	%ebp
	movl	%esp,	%ebp
	movl	8(%ebp), %esi
  REALMODE()
	mov	$0xE,	%ah
  LOOP_S_PRINT:
	cmpb	$0,	(%si)
	je	END_S_PRINT
	lodsb
	int	$0x10
	jmp	LOOP_S_PRINT
  END_S_PRINT:
  PROTMODE()
	movl	%ebp, %esp
	popl	%ebp
	ret

/****
 *
 *  void print_int(unsigned long integer)
 *	prints the integer passed as argument
 *
 *       [jun'02] Just a quick hack.
 *                                - Vivek
 ****/

C_PROC(printi)
	pushl	%ebp
	movl	%esp,	%ebp
        movb    $0, PRINT_INT_LV1
	movl	8(%ebp), %ebx
  REALMODE()
	movl	$1000000000, %ecx
	cmpl	$0,	%ebx
	jne	divisor_loop
	movb	$'0',	%al
	movb	$0xe,	%ah
	int	$0x10
	jmp	end_divisor_loop
  divisor_loop:
	cmpl	$0,	%ecx
	je	end_divisor_loop
	movl	%ebx,	%eax
	movl	$0,	%edx
	divl	%ecx
	addl	$'0',	%eax
	cmpl	$'0',	%eax
	je	p_i_continue
	movb	$1,	PRINT_INT_LV1
  p_i_continue:
	cmpb	$1,	PRINT_INT_LV1
	jne	no_echo
        movb    $1,     PRINT_INT_LV1
	movb	$0xe,	%ah
	int	$0x10
  no_echo:        
	movl	$0,	%edx
	movl	%ebx,	%eax
	divl	%ecx
	movl	%edx,	%ebx
	movl	$0,	%edx
	movl	%ecx,	%eax	
	movl	$10,	%esi
	divl	%esi
	movl	%eax,	%ecx
	jmp	divisor_loop	
  end_divisor_loop:	
  PROTMODE()
	movl	%ebp, %esp
	popl	%ebp
	ret

/****
 *
 * void switch_to_protected_mode()
 *
 *	Function should be called, when the cpu is in real mode and 
 *	to switch it to protected mode. This function is meant for 
 *	use by routines which used switch_to_real_mode and want to 
 *	switch back to protected mode.
 *
 * Notes
 *
 *	The initial part of the code assumes that the processor is
 *	in 16 bit real mode and so the code is generated in 16 bit
 *	mode.
 *
 *	The edx register stores the 16 bit near address of the caller
 *	which is pushed into the stack as a 32 bit value just before 
 *	returning.
 *
 *	The selector FLAT_CODE_SEL and FLAT_DATA_SEL are loaded into
 *	the cs register and the es,ss,ds,fs,gs register respectively
 *
 *	The protected mode stack offset is assumed to be saved in the
 *	variable PM_STCK_OFF which is loaded once in prot.. mode.
 * 
 ****/

PROC(switch_to_protected_mode)
  .CODE16	

	/* save eax and edx
	 */

	movl	%eax, LV1
	movl	%edx, LV2

	
	/* save address of caller 
	 */

	movw	%sp,		%bp
	xorl	%edx,		%edx
	movw	(%bp),		%dx

	/* load the gdt and switch to pmode
	 */

	cli
	lgdt	GDTR
	movl	%cr0,	%eax
	orb	$1,	%al
	movl	%eax,	%cr0

	/* load the flat code selector into cs
	 * flush the prefetch ques and enter 
	 * protected mode.
	 */

	jmp	$FLAT_CODE_SEL,	$P_MODE
  P_MODE:
  .CODE32

	/* load flat data selector into the rest
	 * of the segment registers.
	 */

	movl	$FLAT_DATA_SEL,	%eax
	movl	%eax, %ds
	movl	%eax, %es
	movl	%eax, %ss
	movl	%eax, %fs
	movl	%eax, %gs

	/* restore the protected mode stack
	 */

	movl	PM_ESP,	%esp
	movl	PM_EBP,	%ebp

	/* push the return address into the stack
	 */

	pushl	%edx

	/* restore the register values
	 */

	movl	LV1, %eax
	movl	LV2, %edx

	ret

/****
 *
 * void switch_to_real_mode()
 *
 *	Function should be called, when the cpu is in protected mode
 *	and to switch it to real mode. 
 *
 * Notes
 *
 *	The initial part of the code assumes that the processor is
 *	in 32 bit protected mode and so the code is generated in 32 bit
 *	mode.
 *
 *	The edx register stores the 32 bit near address of the caller
 *	which is pushed into the stack as a 32 bit value just before 
 *	returning.
 *
 *	The selector REAL_CODE_SEL and REAL_DATA_SEL are loaded into
 *	the cs register and the es,ss,ds,fs,gs register respectively
 *
 *	The protected mode stack offset ESP and EBp are saved in the
 *	variables PM_ESP and EBP respectively.
 * 
 ****/


PROC(switch_to_real_mode)
  .CODE32

	/* save eax and edx
	 */

	movl	%eax, LV1
	movl	%edx, LV2
	
	/* save the address of the caller 
	 */

	movl	%esp, PM_ESP
        addl    $4 , PM_ESP
	movl	%ebp, PM_EBP
	movl	(%esp), %edx

	/* load the real mode stack offset into 
	 * esp and ebp.
	 */

	movl	$RM_STCK_OFF, %esp
	pushl	$RM_STCK_OFF 
        popl    %ebp

	/* load the real code selector into cs
	 */

	jmp	$REAL_CODE_SEL,	$REAL_SEG	

  REAL_SEG:
  .CODE16

	/* switch to real mode 
	 */

 	movl	%cr0, %eax
	xorb	$1, %al
	movl	%eax, %cr0

	/* load real data selector values into the segment
	 * registers so that a 0xFFFF limit is loaded into 
	 * descriptor caches.
	 */

 	movw	$REAL_DATA_SEL,	%ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	movw	%ax, %fs
	movw	%ax, %gs

	/* long jump to really switch to real mode
	 */

 	ljmp	$RM_LDR_SEG, $REAL_MODE

  REAL_MODE: 

	movw	$0, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %gs
	movw	$RM_STCK_SEG, %ax
	movw	%ax, %ss
	
	/* activate interrupts
	 */

	 sti

	/* push caller's address
	 */

	 pushl %edx

	/* restore the register values
	 */

	movl	LV1, %eax
	movl	LV2, %edx

	/* do a 32 bit return 
	 */

	 O32 ret
 

 .DATA	/* data section */

 .global __DATA_TABLE, __BIOS_DISK_BUFF


 GDTR:;	gdt_limit:	.word GDT_LIMIT
	gdt_phyad:	.long GDT_PHY_ADDR

 PM_ESP: .long	0	/* holders for protected mode */
 PM_EBP: .long	0	/* esp and ebp */

 PRINT_INT_LV1: .byte 0	 /* Just a few local variable for 
			     the print_int routine */
 LV1: .long 0	/* local */
 LV2: .long 0   /* variables */
 
 __DATA_TABLE: .space 512, 0
 __BIOS_DISK_BUFF: .space 512, 0
