
#include <__defs.h>
#include <conio.h>

#define isalpha(a) ((a>='a'&&a<='z')||(a>='A'&&a<='Z'))

BYTE strcmp(STRING s1, STRING s2)
{
 while (*(s1++) && *(s2++))
   {
    if (*s1 != *s2)
      {
       break;
      }
   }
 if (*s1 == 0 && *s2 == 0)
   {
    return 0;
   }
 else
   {
    return 1;
   }
}

BYTE getdirective(BYTE *script, BYTE *dir)
{
 if (isalpha(*script))
   {
    while(isalpha(*script))
      {
       *(dir++) = *(script++);
      }
    *dir = 0;
    return 1;
   }
 return 0;
}

NORET execscript(BYTE *script, LONG len)
{
 BYTE directive[20];

 while (len)
   {
    if ( getdirective(script, directive) )
      {
       if ( strcmp(directive, "print") == 0)
         {
          printc('\n');
          printc('\r');
          while (*(script++) != '"');
          while (*(script++) != '"')
            printc(*(script-1));

          while (*(script++) != '\n');
         }
      }
   }
}
