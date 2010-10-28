/*
 * opos/loader/includes/LOADER.H  (SYSTEM LOADER)
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
 * HISTORY
 *
 * [Jun 20]
 *   Added lots of macros. - Vivek
 * 
 */

#ifndef __LOADER_H_
#define __LOADER_H_

 /* This is the real mode stack segment value; whenever a 
  * switch from protected mode to real mode is made using
  * the function _switch_to_protected_mode, the stack seg
  * and offset are set to RM_STCK_SEG:RM_STCK_OFF
  */ 

 #define        RM_STCK_SEG     0x0000
 #define        RM_STCK_OFF     0xFFFF

 /* These two define the real mode segment and offset of
  * the loader. These definitions are VERY IMPORTANT as
  * they are used at lots of real mode code sections.
  */

 #define        RM_LDR_SEG      0x0000
 #define        RM_LDR_OFF      0x8000

 /* The next two define the real mode segment and offset
  * values of the global descriptor table of the loader.
  */

 #define        RM_GDT_SEG      0x0000
 #define        RM_GDT_OFF      0x0500

 /* The below variables define the physical addresses of
  * of the above mentioned real mode segment:offset vals
  */

 #define        LDR_PHY_ADDR    0x8000
 #define        GDT_PHY_ADDR    0x0500

 /* The protected mode stack is used when the cpu is in
  * protected mode.. ie once we switch from real mode 
  * to protected mode using switch_to_protected_mode.
  */

 #define        PM_STCK_ADDR    0xEFFF

 /* Selector definitions
  */

 #define        FLAT_DATA_SEL   0x08
 #define        FLAT_CODE_SEL   0x10
 #define        REAL_DATA_SEL   0x18
 #define        REAL_CODE_SEL   0x20

 /* GDT limit
  */

 #define        GDT_LIMIT       0xFE


 #if defined(__ASSEMBLY_SEGMENT__)

   #define PROC(name)    .global name;    ##name##:
   #define C_PROC(name)  .global _##name; _##name##:
   #define CALL(name)    call _##name 
   #define O32           .byte 0x66;
   #define A32           .byte 0x67;
   #define REALMODE() call switch_to_real_mode ; .CODE16 ;
   #define PROTMODE() call switch_to_protected_mode ; .CODE32 ;
   #define ES(v) pushw v ; popw %es ;
   #define DS(v) pushw v ; popw %ds ;
   #define SS(v) pushw v ; popw %ss ;
   #define FS(v) pushw v ; popw %fs ;
   #define GS(v) pushw v ; popw %gs ;
 
 #endif
 
#endif
