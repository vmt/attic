/*
 * opos/loader/diskio.c  (Disk i/o)
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

#include <__defs.h>
#include <loader.h>
#include <diskio.h>


/* file system parameter pointer to the current boot device */
/* from which the operating system is being loaded */

fat_fsp_t *boot_dev_fsp_ptr = 0;

/* this routine  verfies wether the boot parameter block */
/* passed as argument is a valid FAT bpb. This check is  */
/* not robust and more needs to be done.                 */

PRIVATE BYTE verify_fat(BYTE* bpb)
{
 if ( bpb[0] == 0xE9 );
 else
 if ( bpb[0] == 0xEB &&
      bpb[2] == 0x90 );
 else
   return 0;
 return 1;
}

/* Initializes boot device. In the case of a floppy disk */
/* it resets the controller. */

BYTE initdev(WORD dev_id)
{
 switch (dev_id)
   {
    case FDD0 :
    case FDD1 :
         {
          if (biosdisk (0,dev_id,0,0,0,0) == 6)
            {
             lprintf("\n Disk changed...");
             initdev(dev_id);
            }
         }
   }
 return 0xFF;
}

PRIVATE BYTE readsector(WORD dev_id, LONG sector, BYTE *buff_ptr)
{
 switch (dev_id)
   {
    case FDD0 :
    case FDD1 :
         {
          WORD ctr = 0;
          BYTE status = 0;

          status = biosdisk(2,                  /* read   */
                          dev_id,               /* disk   */
                          ((sector/18)/2),      /* track  */
                          ((sector/18)%2),      /* head   */
                          ((sector % 18) + 1),  /* sector */
                          1);                   /* sector count */
          
          if ( status )
            {
             lprintf("disk error {%d}, ABORTING. \n", status);
             for(;;);
            }
          for (ctr =0;ctr < 512; ++ctr)
            {
              buff_ptr[ctr] = _BIOS_DISK_BUFF[ctr];
            }
          return 0;
         }
    case HDD0 :
    case HDD1 :
         {
          return 0xff;
         }
   }
 return 0xFF;
}

/* this routine mounts a particular device usually a    */
/* floppy or a harddisk from which the operating system */
/* is to be loaded. The dev_fsp parameter is used by it */
/* to determine the type of file system implemented.    */

BYTE mount_boot_dev(BYTE dev_id, void* dev_fsp)
{
 BYTE bpb_buffer[512];
 
 switch (dev_id)
   {
    case FDD0 :
    case FDD1 :
         {
          initdev (dev_id);
          readsector (dev_id, 0, bpb_buffer);

          if (verify_fat (bpb_buffer))
            {
             #define FAT ((fat_fsp_t*)dev_fsp)

             FAT->nr_bytes_per_sector    = *(WORD*)(&bpb_buffer[11]);
             FAT->nr_sectors_per_cluster = *(BYTE*)(&bpb_buffer[13]);
             FAT->nr_reserved_sectors    = *(WORD*)(&bpb_buffer[14]);
             FAT->nr_fat                 = *(BYTE*)(&bpb_buffer[16]);
             FAT->nr_root_dir_entries    = *(WORD*)(&bpb_buffer[17]);
             FAT->nr_sectors             = *(WORD*)(&bpb_buffer[19]);
             FAT->nr_sectors_per_fat     = *(WORD*)(&bpb_buffer[22]);
             FAT->nr_sectors_per_track   = *(WORD*)(&bpb_buffer[24]);
             FAT->nr_heads               = *(WORD*)(&bpb_buffer[26]);

             if (FAT->nr_sectors == 0)
               FAT->nr_sectors  = *(LONG*)(&bpb_buffer[32]);

             if (FAT->nr_sectors_per_fat == 0)
               FAT->nr_sectors_per_fat = *(LONG*)(&bpb_buffer[36]);

             FAT->nr_root_dir_sectors = ((FAT->nr_root_dir_entries * 32) +
                  (FAT->nr_bytes_per_sector-1)) / FAT->nr_bytes_per_sector;
             FAT->nr_data_sectors = FAT->nr_sectors -
                 (FAT->nr_reserved_sectors +
                 (FAT->nr_fat * FAT->nr_sectors_per_fat)
                 + FAT->nr_root_dir_sectors );
             FAT->nr_clusters =
               (FAT->nr_data_sectors / FAT->nr_sectors_per_cluster);

             if (FAT->nr_clusters < 4085)
               FAT->type = FAT12;
             else
               {
                lprintf("Only FAT12 disks are supported, for now ;-)");
               }

               FAT->fat_buff = (BYTE*) malloc(FAT->nr_bytes_per_sector * 
                                        FAT->nr_sectors_per_fat*FAT->nr_fat);
               FAT->fat_root = (BYTE*)malloc(FAT->nr_root_dir_entries*32);

               {
                LONG sz  = FAT->nr_sectors_per_fat*FAT->nr_fat +
                FAT->nr_reserved_sectors;
                LONG scnt= 0;
                LONG acnt= 0;

                for ( scnt = FAT->nr_reserved_sectors ; scnt < sz ; scnt ++,
                      acnt ++)
                   readsector(dev_id,scnt,(&(FAT->fat_buff[acnt*512])));

                sz += (FAT->nr_root_dir_entries*32) /
                            FAT->nr_bytes_per_sector;

                for ( acnt = 0; scnt < sz ; scnt ++, acnt ++)
                   {
                    readsector(dev_id,scnt,(&(FAT->fat_root[acnt*512])));
                   }
               }
             #undef FAT
               return 0;
            }
          return 0xFF;
         }
    case HDD0 :
    case HDD1 :
         {
          lprintf("\nThis version does not support boot devices other "
                  "\nthan floppy disk drives");
          return 0;
         }
   }
 return 0xFF;
}

/* FILE OPENER  */
/*              */
/* [9 sept 02] - The routine only supports the fat12 file */
/* system and can open files only from the root directory */
/* By opening, what I mean is it searches for the file,   */
/* stores its starting cluster and its length in the file */
/* descriptor.                                            */

BYTE openfile(BYTE* f_name, file_t *fdesc, void* disk_fsp)
{
 switch (*(BYTE*)(disk_fsp))
   {

    /* FAT-12 */

    case FAT12 :
         {
          #define FAT ((fat_fsp_t*)(disk_fsp))

          LONG cntr = 0;

          /* This loop takes us through the root directory */
          /* Each entry is 32 bytes in length. */

          for (; cntr < (FAT->nr_root_dir_entries * 32); cntr += 32)
             {
              BYTE f_name_cnt = 0;

              /* compare root directory entry with our */
              /* file name */

              while (f_name_cnt < 11)
                {

                 if (f_name[f_name_cnt] != FAT->fat_root[cntr+f_name_cnt])
                   break;

                 f_name_cnt ++;
                }

              if ( f_name_cnt == 11 )
                break;
             }

          /* If we found the file, get the starting cluster */
          /* and the length at offsets 26 and 28 respectvly */

          if ( cntr < (FAT->nr_root_dir_entries * 32))
            {
             fdesc->cluster  = *(WORD*)(&(FAT->fat_root[cntr+26]));
             fdesc->length   = *(LONG*)(&(FAT->fat_root[cntr+28]));
             return 0;
            }

          /* else return a file not found error code */

          else
            {
             return 0xFF;
            }
          #undef  FAT
         }
   }
 return 0xFF;
}

/* READ FILE    */
/*              */
/* [9 sept 02] - Reads the whole file into the buffer pointed */
/* by file_buff. The passed file descriptor should be of an   */
/* open file opended using openfile(...)                      */

NORET readfile(file_t *fdesc, void* disk_fsp, BYTE* file_buff)
{
 switch (*(BYTE*)(disk_fsp))
   {
    /* FAT-12 */

    case FAT12 :
         {
          #define FAT ((fat_fsp_t*)(disk_fsp))

          /* get all parameters we need to read the file data */

          LONG data_start =  (FAT->nr_reserved_sectors) +
                             (FAT->nr_fat * FAT->nr_sectors_per_fat) +
                             (FAT->nr_root_dir_sectors) ;
          WORD cluster    =  (fdesc->cluster);
          WORD sector     =   data_start + ((cluster - 2) *
                              FAT->nr_sectors_per_cluster);

          /* loop till the end of the cluster chain */
          /* denoted by values 0xFF8 - 0xFFF        */

          while (1)
            {
             WORD sctr       =  0;

             /* read all the sectors in the cluster */

             for (; sctr < FAT->nr_sectors_per_cluster; ++sctr)

                { lprintf("read %d, ",sector+sctr);
                  readsector(0,sector+sctr,&file_buff[sctr * 512]); }

             /* get the next cluster in the chain */

             cluster  = *(WORD*)&(FAT->fat_buff[(cluster * 3) / 2]);
             cluster  =  (cluster & 0x01) ?
                          cluster & 0x0FFF : cluster >> 4 ;

             /* if its the end of the chain return */

             if (cluster >= 0xFF8 && cluster <= 0xFFF)

                return;

             /* derive sector number from the cluster */

             sector = data_start + ((cluster - 2) *
                      FAT->nr_sectors_per_cluster);
            }

          #undef FAT
         }
   }
}


