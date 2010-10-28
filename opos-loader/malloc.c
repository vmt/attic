#include <__defs.h>
#include <datatable.h>

PRIVATE LONG mm_ctr = 0;

/* a very basic memory allocation routine which allocates */
/* units starting from the end of the extended memory.    */
/* All units are dword aligned.                           */

PTR malloc(WORD sz)
{
 if ( mm_ctr == 0)
   {
    mm_ctr = ((*(WORD*)(&_DATA_TABLE[BASE_MEM_SZ])) +
             (*(LONG*)(&_DATA_TABLE[EXT_MEM_SZ]))) * 1024  ;
   }
 return (PTR)(mm_ctr -= (sz));
}


