/*
 * opos/loader/disk-FAT12/boot.S  (FAT-12 BOOT STRAP LOADER)
 *
 * Copyright (c) 2002 Vivek Mohan <opendev@phreaker.net>
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
   
/*
 * NOTES
 * =====
 *
 * * 1
 *
 * So far the boot sector code does the following..
 *
 *      * sets up the stack at RM_STCK_SEG:RM_STCK_OFF
 *      * enables the a20 line
 *      * copies the GDT to location RM_GDT_SEG:RM_GDT_OFF
 *      * load the set up module from the floppy disk to
 *        RM_LDR_SEG:RM_LDR_OFF.
 *      * switch to protected mode
 *      * setup the protected mode stack
 *      * jump to the loader
 *
 * * 2 (FIXME)
 *
 * The a20 line is enabled using the keyboard controller only. The routine
 * is not so robust and assumes that the a20 is enabled at one go. 
 *
 * * 3 (FIXME)
 *
 * Disk routines need an exception handler.
 *
 * * 4 (FIXME)
 *
 * The code assumes the processor to be an i386+ processor. No check for
 * cpu has been implemented.
 *
 */

#include <loader.h>

 .TEXT           /* code segment */
 .CODE16         /* 16 bit mode  */

        jmp     begin_boot
        nop

/* the FAT-12 Boot Parameter Block
 **/

        .ascii "OPOS    "      /* OEM name */
        .word  512             /* bytes per sect */
        .byte  1               /* sects per clust*/
        .word  1               /* rsvd sects */
        .byte  2               /* nr fats */
        .word  224             /* root ent cnt */
        .word  2880            /* total sect */
        .byte  0xF0            /* media */
        .word  9               /* szof FAT */
 SECTORS_PER_TRACK:
        .word  18              /* sects p track */
 HEADS_PER_CYLINDER:
        .word  2
        .long  0
        .long  0
        .word  0
        .byte  0x29
        .long  0
        .ascii "OPOSBOOTDSK"
        .ascii "FAT12   "

 begin_boot:

/*
 * Set up the real mode stack at RM_STACK_SEG:RM_STACK_OFF
 * We don't need no external hardware interrupts while 
 * doing so..
 */

        cli
        movw    $RM_STCK_SEG, %ax
        movw    %ax, %ss                /* Load the stack segment */
        movw    $RM_STCK_OFF, %sp       /* Load the stack pointer */
        sti

        movw    $0,  %ax                /* Load values into other */
        movw    %ax, %ds                /* segment registers      */
        movw    %ax, %es

/*
 * Now to enable the a20 line using the 8042  keyboard controller. 
 * This method was stolen from linux 0.0.1 but we'll probably have 
 * to modify this sooner or later. This is just temporary.
 */
        call    flush_8042
        mov     $0xD1, %al
        out     %al,   $0x64
        call    flush_8042
        mov     $0xDF, %al
        out     %al,   $0x60
        call    flush_8042

/*
 * Next step, load the os loader into memory...
 */

        mov     $RM_LDR_SEG, %ax
	mov     %ax, %es
        mov     $RM_LDR_OFF, %bx
        mov     $18, %cx        /* loader 9 K */
        mov     $34, %ax	/* starting sector => 34 */
        
 load_loader:

        push    %ax
        push    %cx
        call    read_sector
        addw    $512, %bx
        pop     %cx
        pop     %ax
        inc     %ax
        loop    load_loader

/*
 * copy the GDT to RM_GDT_SEG:RM_GDT_OFF..
 */
        movw    $gdt_start, %si
        movw    $gdt_end-gdt_start, %cx
        movw    $RM_GDT_SEG, %ax
        movw    %ax, %es
        movw    $RM_GDT_OFF, %di
        cld
        rep     movsb

/*
 * Now to switch to protected mode
 */
 
        cli             /* no interrupts */
        lgdt  gdtr      /* load the gdtr register */

/*
 * Setting the PE bit of control register 0 will tell the
 * cpu to switch to protected mode.
 */

        movl    %cr0, %eax
        orb     $1,   %al
        movl    %eax, %cr0

/*
 * But thats not enough, we have to do a short jump and load valid 
 * selectors into the segment selectors. The following  instruction 
 * loads the a FLAT code selector into the cs  register and does a 
 * short jump to the next instruction.
 */

        jmp     $FLAT_CODE_SEL, $P_MODE

/*
 * Since cs has been loaded with a selector which sets the
 * default operand size to 32 bit...
 */

 .CODE32

 P_MODE:

/*
 * Load FLAT data selectors into the rest segment selectors
 */
        movl    $FLAT_DATA_SEL, %eax
        movl    %eax, %ds
        movl    %eax, %ss
        movl    %eax, %es
        movl    %eax, %fs
        movl    %eax, %gs

/*
 * setup the protected mode stack
 */
        movl    $PM_STCK_ADDR, %esp

/*
 * jump to the loader
 */
        jmp     LDR_PHY_ADDR

 .CODE16

/*
 * flush_8042
 * This wil flush the 8042 keyboard controller command queue.
 */

 flush_8042:

        jmp  .+2
        jmp  .+2
        inb  $0x64, %al
        test $2,    %al
        jnz  flush_8042
        ret

 read_sector:
        mov     %sp, %bp
	sub	$8, %sp
        mov     SECTORS_PER_TRACK, %si
        xor	%dx, %dx
        div     %si
        inc     %dl
        mov     %dl, -1(%bp)
	mov	HEADS_PER_CYLINDER, %si
        xor	%dx, %dx
        div     %si
        mov     %dl, -2(%bp)
        mov     %ax, -4(%bp)
        mov     $1, %al            /* read one sector     */
        mov     -4(%bp), %ch       /* load cylinder count */
        mov     -2(%bp), %dh       /* load head count     */
        mov     -1(%bp), %cl       /* load sector count   */
	mov	$0, %dl
        mov     $2, %ah            /* read function       */
        int     $0x13              /* int 13              */
        mov     %bp, %sp
	ret

/*
 * The GDT 
 */
 
  gdt_start:

        /*
         * The null descriptor
         */

        .long   0x0000 , 0x0000

  flat_data_desc:

 
        /*
         * The flat descriptor for data segment
         * Granularity = 4K, readable, writable
         * 32 bits default operand size
         */

        .short  0xFFFF , 0
        .byte   0 , 0x92 , 0xCF, 0

  flat_code_desc:

        /*
         * The flat descriptor for code segment
         * Granularity = 4K, readable, non-conforming
         * 32 bits default operand size
         */

        .short  0xFFFF , 0
        .byte   0, 0x9A, 0xCF, 0

 real_data_desc:

        /*
         * The real descriptor for data segment
         * Granularity = 1 B, r/w, 16 bits
         */

        .short  0xFFFF , 0
        .byte   0 , 0x92 , 0, 0

 real_code_desc:

        /*
         * The real descriptor for code segment
         * Granularity = 1 B, r, 16 bits
         */

        .short  0xFFFF , 0
        .byte   0, 0x9A, 0, 0

 gdt_end:

 gdtr:; gdt_limit: .word (gdt_end-gdt_start)-1
        gdt_phyad: .long GDT_PHY_ADDR

/*
 * The data segment of the boot module starts here
 */

 .DATA


 .byte 0xF0, 0x0F, 0xFF
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 .byte 0xF0, 0x0F, 0xFF      /* 2 clusters */
 
 .space (NR_FATS * NR_SECTORS_PER_FAT * NR_BYTES_PER_SECTOR - (33 *  3)), 0
 .space (ROOT_DIR_ENTRIES * ENTRY_SIZE), 0
