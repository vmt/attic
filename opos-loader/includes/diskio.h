/*
 * opos/loader/includes/DISKIO.H  (DISK I/O)
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

#ifndef __DISKIO_H_
#define __DISKIO_H_

#include <__defs.h>
#include <datatable.h>
#include <loader.h>
#include <conio.h>

#define NR_FDD            _DATA_TABLE[N_FLOPPY_DRVS]
#define FDD0_TYPE         _DATA_TABLE[FLOPPY0_TYPE]
#define FDD0_MAX_CYL      _DATA_TABLE[FLOPPY0_MAX_CYL]
#define FDD0_MAX_SECT     _DATA_TABLE[FLOPPY0_MAX_SECT]
#define FDD0_MAX_HD       _DATA_TABLE[FLOPPY0_MAX_HD]
#define bd_buffer         (&(_BIOS_DISK_BUFF[0]))

/* device ids */

#define FDD0    0x0
#define FDD1    0x1
#define HDD0    0x80
#define HDD1    0x81

/* file system types */

enum{ FAT12, FAT16, FAT32 };

/* FAT file system parameter table */
/* structure */

typedef struct
{

 BYTE  type;
 WORD  nr_bytes_per_sector;
 WORD  nr_sectors_per_cluster;
 WORD  nr_reserved_sectors;
 WORD  nr_fat;
 WORD  nr_clusters;
 WORD  nr_root_dir_sectors;
 WORD  nr_root_dir_entries;
 WORD  nr_sectors_per_track;
 WORD  nr_heads;
 WORD  nr_hidden_sectors;
 LONG  nr_sectors;
 LONG  nr_sectors_per_fat;
 LONG  nr_data_sectors;
 BYTE *fat_buff;
 BYTE *fat_root;

} fat_fsp_t ;

typedef struct
{

 LONG cluster ;
 LONG length  ;

} file_t ;

IMPORT BYTE  _BIOS_DISK_BUFF[512];
IMPORT BYTE  biosdisk (BYTE fnc, BYTE dsk, WORD cyl,
                      BYTE hd, BYTE sect, BYTE nsect );
GLOBAL BYTE  mount_boot_dev(BYTE dev_id, PTR dev_fsp );\
GLOBAL BYTE  openfile(BYTE* f_name, file_t *fdesc, PTR disk_fsp);
GLOBAL NORET readfile(file_t *fdesc, PTR disk_fsp, BYTE* file_buff);


#endif
