/*
 * opos/loader/includes/DATATABLE.H  (SYSTEM DATA TABLE)
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

/*
 *
 * HISTORY
 * 
 */

#ifndef __DATA_TABLE_H
#define __DATA_TABLE_H

#include <__defs.h>

#define DATA_TABLE_SZ           0x0200  // The data table size

/* 
 * Data Table Entries           OFFSET
 * ==================           ======
 */

 #define BASE_MEM_SZ            0x0000  // base memory size holder
 #define EXT_MEM_SZ             0x0004  // extnd memory size holder
 #define ROM_TABLE_PTR          0x0008  // holder for a pointer to the rom table
 #define BUS_ARCH               0x000C  // Bus architecture type holder
 #define RTC_INSTALLED          0x0010  // RTC installation flag
 #define PIC2_INSTALLED         0x0011  // PIC-2 installation flag
 #define N_FLOPPY_DRVS          0x0012  // Number of floppy drives
 #define N_HARD_DRVS            0x0013  // Number of hard drives
 #define FLOPPY0_TYPE           0x0014  // Floppy drive 0 params
 #define FLOPPY0_MAX_CYL        0x0015  //
 #define FLOPPY0_MAX_SECT       0x0016  //
 #define FLOPPY0_MAX_HD         0x0017  //
 #define FLOPPY1_TYPE           0x0018  // Floppy drive 1 params
 #define FLOPPY1_MAX_CYL        0x0019  //
 #define FLOPPY1_MAX_SECT       0x001a  //
 #define FLOPPY1_MAX_HD         0x001b  //
 #define HD0_DPT                0x0020  // Disk parameter table for hd0
 #define HD1_DPT                0x0030  // Disk parameter table for hd1
 #define HD0_INT13_E_INSTALLED  0x0040  // int 13 extensions install flag
 #define HD0_INT13_E_MAJ_VER    0x0041  // int 13 extensions major version
 #define HD0_INT13_E_API_SUBSET 0x0042  // int 13 extensions api bit map (16bit)
 #define HD0_INT13_E_DPT_SUPP   0x0044  // int 13 extensions dpt supported flag
 #define HD0_INT13_E_EDD_SUPP   0x0045  // int 13 extensions edd supported flag
 #define HD0_INT13_E_DPT        0x0050  // int 13 extensions dpt buffer
 #define HD0_INT13_E_EDD        0x00A0  // int 13 extensions edd buffer
 #define SYS_MEM_MAP_SZ         0x0100  // system memory map size holder
 #define SYS_MEM_MAP            0x0104  // the system memory map buffers

 #if defined(__ASSEMBLY_SEGMENT__)      // macros for asm code
   #define DATATABLE(entry) %gs:__DATA_TABLE +  entry
 #else
   IMPORT BYTE _DATA_TABLE[512];
 #endif

#endif
