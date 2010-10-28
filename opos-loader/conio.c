/*
 * opos/loader/conio.c  (Console i/o)
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

/* This file includes implementation of console input and output */
/* routines.*/

#include <conio.h>

/* lprintf - can accept two format specifiers %d and %s */
/*      %d - prints an integer                          */
/*      %s - prints a string                            */

NORET lprintf(STRING f_str, ...)
{ 
 LONG arg_ctr = 0;
 LONG arg_ptr = (LONG)(&f_str);

 while(*f_str)
   {
    if (*f_str == '%' && *(f_str+1) == 'd')
      {
       printi(*(LONG*)(arg_ptr+((arg_ctr+1)*4)));
       arg_ctr++;
       f_str += 2;
      }
    else
    if (*f_str == '%' && *(f_str+1) == 's')
      {
       prints((BYTE*)(arg_ptr+((arg_ctr+1)*4)));
       arg_ctr++;
       f_str += 2;
      }
    else
    if (*f_str == '\n')
      {
       prints("\n\r");
       f_str ++;
      }
    else
      printc(*(f_str++));
   }
}
