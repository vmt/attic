
 /*
  * Linker Script for the boot strapper code.
  *
  */

 OUTPUT_FORMAT("binary");
 OUTPUT_ARCH("i386");
 SECTIONS 
 { bootsector 0x7C00:	/* This is where bios loads it */
   {	*(.text)	/* Put the code and.. */
	. += (510-.);	/* Move to offset 510 */
	SHORT(0x55AA);	/* Put the boot signature */
	*(.data)	/* the data section.  */
   }
 }