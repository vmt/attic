
#include <datatable.h>
#include <loader.h>
#include <diskio.h>
#include <conio.h>

int main()
{
        unsigned t = 0;
 PTR dev_fsp = malloc(60);
 BYTE* scriptbuff;
 file_t scriptf;

 lprintf (
          "OPOS32 ....\n"
          "Memory \n"
          "  Base     = %d KB\n"
          "  Extended = %d KB\n"
          , *(unsigned short*)(&_DATA_TABLE[BASE_MEM_SZ])
          , *(unsigned long *)(&_DATA_TABLE[EXT_MEM_SZ] ) );

  if ( mount_boot_dev(0, dev_fsp) == 0 )
    {
     lprintf("Mounted floppy device.\n");
     lprintf("Looking for loader SCRIPT in root .. ");

     /* open the file named script */

     if ( openfile("SCRIPT     ", &scriptf, dev_fsp) == 0 )
       {
        lprintf("FOUND.\nLoading script to memory...");

        /* allocate enough space for loading the whole script */
        /* into the memory */

        scriptbuff = malloc(scriptf.length);

        /* read the whole file in */

        readfile(&scriptf, dev_fsp, scriptbuff);

        lprintf("DONE.");

        execscript(scriptbuff, scriptf.length);

       }
     else
       {
        lprintf("SCRIPT file not installed. ABORTING.");
        for(;;);
       }
    }

  for(;;);
}
