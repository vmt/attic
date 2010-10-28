/*

 boot/fat12.S

 Copyright (c) 2002, Vivek Mohan <vivek@sig9.com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation ; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 NOTES

 This is a FAT12 boot loader for 1.44MB Floppy Diskettes

 The boot sector code does the following..
   - sets up the stack at RM_STCK_SEG:RM_STCK_OFF
   - looks for stage2 in disk
   - loads to memory and passes control to it

 FIXME

   - Disk routine needs an exception handler.

 FAT FACTS

       RootDirSectors = sectorBytes / (rootDirEntries * 32)
       FatSectors     = fatCount * sectorsPerFat
       DataStart      = reservedSectors + fatSectors + rootDirSectors
 */

#define RM_STACK_SEG		0x0000
#define RM_STACK_OFF		0xFFFF
#define STAGE2_BUFFER_SEGMENT	0x0000
#define STAGE2_BUFFER_OFFSET	0xA000

#define STAGE2_FILE_NAME 	"STAGE2     "
#define NR_BYTES_PER_SECTOR 	512
#define NR_SECTORS_PER_CLUSTER	1
#define NR_RESERVED_SECTORS 	1
#define NR_FATS 		2
#define NR_ROOT_DIR_ENTRIES 	224
#define NR_SECTORS 		2880
#define NR_SECTORS_PER_TRACK 	18
#define NR_HEADS_PER_CYLINDER 	2
#define NR_SECTORS_PER_FAT 	9
#define NR_ROOT_DIR_SECTORS  	((NR_ROOT_DIR_ENTRIES * 32) / NR_BYTES_PER_SECTOR)
#define NR_FAT_SECTORS 		(NR_FATS * NR_SECTORS_PER_FAT)
#define DATA_START_SECTOR 	(NR_RESERVED_SECTORS + NR_FAT_SECTORS + NR_ROOT_DIR_SECTORS)
#define ROOT_DIR_SECTOR 	(1 + NR_FAT_SECTORS)
#define D_SEGMENT 		0
#define FAT_OFFSET 		0x8000
#define ROOT_DIR_OFFSET 	(FAT_OFFSET + (NR_FATS * NR_SECTORS_PER_FAT * NR_BYTES_PER_SECTOR))
#define FAT_FILENAME_LENGTH 	11
#define FAT_CLUSTER_OFFSET 	0x1A
#define PRINTS(stroff)		mov stroff, %si; call prints;

#include <boot.h>

.TEXT           /* code segment */
.CODE16         /* 16 bit mode  */

	/* the FAT-12 Boot Parameter Block */
	 jmp     begin_boot
         nop

	 /* OEM name */
	.ascii "OPOS    "
	.word  NR_BYTES_PER_SECTOR
	.byte  NR_SECTORS_PER_CLUSTER
	.word  NR_RESERVED_SECTORS
	.byte  NR_FATS
	.word  NR_ROOT_DIR_ENTRIES
	.word  NR_SECTORS
	.byte  0xF0   /* media */
	.word  NR_SECTORS_PER_FAT
 	.word  NR_SECTORS_PER_TRACK
	.word  NR_HEADS_PER_CYLINDER
	.long  0
	.long  0
	.word  0
	.byte  0x29
	.long  0xFFFFFFFF
	.ascii "OPOSBOOTDSK"
#ifdef FAT12
	.ascii "FAT12   "
#endif

begin_boot:

	/*
	 Set up the real mode stack at RM_STACK_SEG:RM_STACK_OFF
	 We don't need external hardware interrupts while
	 doing so..
	 */

	cli
	movw    $RM_STACK_SEG, %ax
	movw    %ax, %ss
	movw    $RM_STACK_OFF, %sp
	sti

	/* load valules into other segment registers */
	movw    $0,  %ax
	movw    %ax, %ds
	movw    %ax, %es

	PRINTS($msg_welcome)

	/* Next step, load the FAT into memory. */
	mov     $FAT_OFFSET, %bx
	mov     $1, %ax	 /* start from sector 1 */
	mov	$NR_FAT_SECTORS, %di
	call    read_sectors

	/* load root directory */
	mov	$ROOT_DIR_OFFSET, %bx
	mov	$ROOT_DIR_SECTOR, %ax
	mov	$NR_ROOT_DIR_SECTORS, %di
	call	read_sectors

	PRINTS($msg_searching_stage2)

	/* search for second stage */
	cld
	mov	$ROOT_DIR_OFFSET, %di
stage2_search:
	mov	$stage2_file_name, %si
	mov	$FAT_FILENAME_LENGTH, %cx
	push	%di
	rep	cmpsb
	pop	%di
	je	stage2_found
	add	$(32), %di
	cmp	%di, %bx
	je	stage2_not_found
	jmp	stage2_search
stage2_not_found:
	PRINTS($msg_stage2_not_found)
	jmp	.
stage2_found: /*  stage 2  found */
	PRINTS($msg_stage2_found)

/*
 STAGE2 Loading
 ax: stores the cluster
 bx: used as offset to buffer es:bx and as offset to FAT
 dx: used to store new cluster value
*/
	PRINTS($msg_stage2_loading)

	/* initialize storage buffer */
	pushw	$STAGE2_BUFFER_SEGMENT
	popw	%es
	mov	$STAGE2_BUFFER_OFFSET, %bx

	/* get starting cluster at offset 0x1a*/
	mov	FAT_CLUSTER_OFFSET(%di), %ax

	/* loader loop - start */
stage2_load:
	push	%ax			/* save ax		*/
	call	read_cluster		/* read_cluster(ax,es:bx)	*/
	pop	%ax			/* restore		*/
	add	$NR_BYTES_PER_SECTOR, %bx	/* bx += 512	*/
	push	%bx			/* save bx		*/
	mov	%ax, %bx		/* bx = cluster  	*/
	shr	$1,  %bx		/* bx = cluster/2 	*/
	add	%ax, %bx		/* bx = 3/2.cluster 	*/
	movw	FAT_OFFSET(%bx), %dx	/* dx = fat entry 	*/
	pop	%bx			/* restore bx		*/
	test	$0x01, %ax		/* is bit 1 set ?	*/
	jnz	odd_cluster		/* an odd cluster	*/
even_cluster:
	and	$0x0FFF, %dx		/* keep lower 12bits	*/
	jmp	test_cluster
odd_cluster:
	shr	$4, %dx			/* keep upper 12bits	*/
test_cluster:
	cmp	$0xFF0, %dx		/* is dx < 0xFF0 ?	*/
	mov	%dx, %ax		/* ax = dx		*/
	jb	stage2_load
	/* load	loop - end */

	PRINTS($msg_stage2_loading_done)

	/* pass control to stage 2 */

	jmp	$STAGE2_BUFFER_SEGMENT, $STAGE2_BUFFER_OFFSET


read_sectors:
	cmp	$0, %di
	je	end_read_sectors
	push	%ax
	call	read_sector
	pop	%ax
	add	$NR_BYTES_PER_SECTOR, %bx
	inc	%ax
	dec	%di
	jmp	read_sectors
end_read_sectors:
	ret


	/* reads a logical sector in ax off the disk to es:bx */
read_sector:
        mov     %sp, %bp
	sub	$8, %sp
        mov     $NR_SECTORS_PER_TRACK, %si
        xor	%edx, %edx
        div     %si
        inc     %dl
        mov     %dl, -1(%bp) /* 15 */
	mov	$NR_HEADS_PER_CYLINDER, %si
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

/* a simple string output routine */
prints:
	mov	$0xE, %ah
prints_loop:
	lodsb
	cmp	$0, %al
	je	prints_exit
	int	$0x10
	jmp	prints_loop
prints_exit:
	ret

/*
 function: read_cluster(ax => cluster, es:bx => buffer)

 Cluster to Sector 'xlation -

	 Sector  = DataStart + ((AnyClusterNumber-2) * sectorsPerCluster)
 */

read_cluster:
	sub     $0x02, %ax		/* ax = ax-2			*/
	xor     %cx, %cx		/* cx = 0 			*/
	mov     $NR_SECTORS_PER_CLUSTER, %cl	/* cl = sectors/cluster */
	mul     %cx			/* dx:ax = (ax * cx) 		*/
	add     $DATA_START_SECTOR, %ax /* ax = ax + data start 	*/
					/* now, ax = sector (LBA) 	*/
	call	read_sector		/* read_sector(ax, es:bx) 	*/
	ret

msg_welcome: 		.asciz "Booting ..\n\r"
msg_stage2_not_found: 	.asciz " not found. (fatal error)\n\r"
msg_searching_stage2: 	.asciz " Looking for Stage2 ..."
msg_stage2_found: 	.asciz " found.\n\r"
msg_stage2_loading:	.asciz " Loading ..."
msg_stage2_loading_done:.asciz " done.\n\r"
stage2_file_name: 	.ascii STAGE2_FILE_NAME
