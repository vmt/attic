
 /* Author : Vivek Mohan 
  *	 	(Home: http://www.geocities.com/cppresources/)
  *		(Mail : mailvivek27@sify.com || opendev@phreaker.net)
  * OPOS HOME : http://opos.t35.com/
  */
  
 /* COPYRIGHT ? DO what ever you want , just don't call it your own. If possible
  * please link me (http://www.geocities.com/cppresources/) to your homepage. Thanx !
  */


 # ifndef _SYSTEM_SERVICES_
 # define _SYSTEM_SERVICES_

 # include "defs.h"


 u8b attrib = 15;
 u8b page = 0;

 /* BIOS video IO operations 	*/
 /* ======================== 	*/
 /* Uses the int 0x10		*/

 /* prints a character at the current position of the cursor 	*/
 /* Uses the bios interrupt 0x10.a				*/
 
 void _printc(u8b c)
 {    	asm mov ah , 0x09
	asm mov al , c
	asm mov bh , page
	asm mov bl , attrib
	asm mov cx , 1
	asm int 0x10
 }

 /* sets the vide mode 			*/
 /* USes BIOS Int 0x10 and function 0x0 */

 void _setvideomode(u8b m) /* m = the mode */
 {    	asm	mov ah , 0
	asm	mov al , m
	asm	int 0x10	
 }

 /* sets the cursor position on the screen 	*/
 /* Positons ranging from 0,0 to 79,24		*/

 void _setcurpos(int x,int y)
 {    	asm	mov ah , 0x02	;
	asm	mov bh , 0x0	;
	asm	mov dh , y	;
	asm	mov dl , x	;
	asm	int 0x10	;
	
 }

 /* Gets the cursor's position on the screen */

 void _getcurpos(int *x,int *y)
 {	asm	mov ah , 0x03
	asm	mov bh , 0x00
	asm	int 0x10
	asm	mov [x], dl
	asm	mov [y], dh
 }

 /* scroll a part of the screen up */

 void _scrollup(int nol,int x1,int y1,int x2,int y2)
 {	asm	mov ah , 0x06
	asm	mov al , nol
	asm	mov cl , x1
	asm	mov ch , y1
	asm	mov dl , x2
	asm	mov dh , y2
	asm	mov bh , attrib
	asm	int 0x10
 }

 /* scroll a part of the screen down */

 void _scrolldown(int nol,int x1,int y1,int x2,int y2)
 {	asm	mov ah , 0x07
	asm	mov al , nol
	asm	mov cl , x1
	asm	mov ch , y1
	asm	mov dl , x2
	asm	mov dh , y2
	asm	mov bh , attrib
	asm	int 0x10
 }

 /* waits for a key to be pressed and stores the key pressed and its scan code */

 void _getkey(u8b *key,u8b *scancode)
 {	u8b a,b;
	asm	mov ah , 0
	asm	int 0x16
	asm	mov a, al
	asm	mov b, ah
	*key = a ;
	*scancode = b;
 } 

 /* reboots the comp */

 void _reboot()
 {	asm int 0x19 
 }

 /* PORT Functions */
 /* ============== */

 u8b inportb(u16b port_no)
 {	u8b rbyte;
	asm mov dx , port_no
	asm in  al , dx 
	asm mov rbyte , al
 }
 
 void outportb(u16b port_no,u8b value)
 {	asm mov dx , port_no
	asm mov al , value
	asm out dx , al
 }

 void updatekbdleds(void)
 {
 	outportb(0x60, 0xED);
	while(inportb(0x64) & 2) ;
	outportb(0x60, 0);	
	while(inportb(0x64) & 2) ;
 }

 # endif