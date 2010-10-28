
 /*
  * Linker Script for the boot strapper code.
  *
  */

 OUTPUT_FORMAT("binary");
 OUTPUT_ARCH("i386");
 SECTIONS 
 { loader 0x8000:	
   {	loader.o(.text)	/* the text section */
	*(.text)
	*(.data) /* the data section.  */
   . += 32768 - .;
   }
 }