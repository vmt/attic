
 /* Author : Vivek Mohan 
  *	 	(Home: http://www.geocities.com/cppresources/)
  *		(Mail : mailvivek27@sify.com || opendev@phreaker.net)
  * OPOS HOME : http://opos.t35.com/
  */
  
 /* COPYRIGHT ? DO what ever you want , just don't call it your own. If possible
  * please link me (http://www.geocities.com/cppresources/) to your homepage. Thanx !
  */


 # include "defs.h"
 # include "syssrv.c"

 # ifndef _CONSOLE_
 # define _CONSOLE_
/*********************************************
 *                                           *
 *  The OPOS Console Programming Inteface    *
 *					     *
 *********************************************/
 #ifndef  OPOS_CPI
 # define OPOS_CPI
 #endif


 u8b OPOS_CPI getch();

 /* NOTE 1 : OPOS-Console API supports only 80 x 25
    textmode. Anybody , who can modify it for other
    modes , please mail it to <create a mail id>   */

 /* NOTE 2 : The OPOS Console API has , as for now only
    a few conosle manip functions. Have to add more */

 /* A few global variables which store the
    properties of a console */

 const u8b Height = 25; /* Screen Height */
 const u8b Width  = 80; /* Screen Width  */

 const u8b TAB_Width = 4;
 const u8b TAB='\t';
 const u8b EOL='\n';

 const u8b horiz_line = 'Ä';
 const u8b vert_line  = '³';
 const u8b ul_corner  = 'Ú';
 const u8b ur_corner  = '¿';
 const u8b ll_corner  = 'À';
 const u8b lr_corner  = 'Ù';

 /* Color Constants */

 # define BLACK		0
 # define BLUE  	1
 # define GREEN		2
 # define CYAN		3
 # define RED		4
 # define MAGENTA	5
 # define BROWN 	6
 # define LIGHTGRAY	7
 # define DARKGRAY	8
 # define LIGHTBLUE 	9
 # define LIGHTGREEN	10
 # define LIGHTCYAN	11
 # define PINK		12
 # define LIGHTMAGENTA	13
 # define YELLOW	14
 # define WHITE		15
  
 /*...................................*/
 /* Sets the screen attribute	      */
 /*...................................*/

 void setattrib(color)
 { attrib = color; }

 /*.............................................*/
 /* Gotoxy , positions the cursor at a point	*/
 /* on the screen				*/
 /* ...........................................	*/

 void OPOS_CPI gotoxy(u8b x,u8b y)
 {    /* check if the co-ords are correct */
      /* if they are not , reset them to  */
      /* (1,1) */
      x = (x>=1&&x<=80) ? x : 1 ;
      y = (y>=1&&y<=25) ? y : 1 ;
      /* BIOS int 0x10 */
      _setcurpos(x,y);
 }

 /* Returns the color code for a particular background
  * and forground */
  u8b color(u8b bg,u8b fg)
  {  return (bg*16) + fg;  }
  

 /*.............................................*/
 /* clear , positions the cursor at a point	*/
 /* on the screen				*/
 /* ...........................................	*/

 void OPOS_CPI clear(int color)
 {	/* use the scroll function to clear the screen */
	setattrib(color);
	_scrollup(0,0,0,79,24);
 }

 /*.............................................   */
 /* print , prints a string on the screen	   */
 /* on the screen				   */
 /* ...........................................	   */
 /* NOTE: This print function is an ad hoc version */
 /* of what I actually planned to make. For now it */
 /* has no formatting and stuff. Just understands  */
 /* \n \t. 					   */
 /*----------------------------------------------- */
 /* NOTE: For printing integers use printi , not   */
 /* yet implemented */

 void OPOS_CPI print(u8b *s,u8b x,u8b y)
 {      while(*s)
	{       if(*s=='\n')
		{	_printc('\n');
			_printc('\r');
			++s;
			gotoxy((x=0),++y);
		}
		else{
			gotoxy(x++,y);
			_printc(*s);
			s++;			
		}		
	}
 }

 /*............................................. */
 /* read , reads characters from the keyword and */
 /* returns the buffer				 */
 /* ............................................ */
 /* NOTE: This function is also a very ad hoc    */
 /* implementation of what was required.	 */
 /* ............................................ */

 void  OPOS_CPI read(u8b *buffer,
	u16b bufferlength)
 {	u8b c = 'k';
	u8b j ;
	u8b x = 1 ; u8b y = 1;
	while(c!=13)
	{	c = getch();
		*buffer = c;
		gotoxy(x++,y);
		if(c!=13)
			_printc(c);
		++buffer;
	}	
 }
 
 /*..............................................*/
 /* Draws a line on the screen of specified len  */
 /* at the specified point			 */
 /*..............................................*/

 void OPOS_CPI drawhline(u8b x,u8b y,u8b l,u8b color)
 {	int i=0;
	gotoxy(x,y);
	setattrib(color);
	for(;i<l;++i){
		gotoxy(x++,y);
		_printc((u8b)horiz_line);
	}
 }

 void OPOS_CPI drawvline(u8b x,u8b y,u8b l,u8b color)
 {	int i=0;
	gotoxy(x,y++);
	setattrib(color);
	for(;i<l;++i){
		_printc((u8b)vert_line);
		gotoxy(x,y++);
	}
 }


 void OPOS_CPI boxoutset(u8b x1,u8b y1,u8b x2,u8b y2,u8b lb,u8b db)
 {		drawhline(x1+1,y1  ,x2-x1-1,lb);
		drawvline(x1  ,y1+1,y2-y1-1,lb);
		drawhline(x1+1,y2  ,x2-x1-1,db);
		drawvline(x2  ,y1+1,y2-y1-1,db);
		setattrib(lb);
		gotoxy(x1,y1);
		_printc(ul_corner);
		gotoxy(x1,y2);
		_printc(ll_corner); 
		setattrib(db);
		gotoxy(x2,y1);
		_printc(ur_corner);
		gotoxy(x2,y2);
		_printc(lr_corner); 
 }

 void OPOS_CPI boxinset(u8b x1,u8b y1,u8b x2,u8b y2,u8b lb,u8b db)
 {		drawhline(x1+1,y1  ,x2-x1-1,db);
		drawvline(x1  ,y1+1,y2-y1-1,db);
		drawhline(x1+1,y2  ,x2-x1-1,lb);
		drawvline(x2  ,y1+1,y2-y1-1,lb);
		setattrib(db);
		gotoxy(x1,y1);
		_printc(ul_corner);
		gotoxy(x1,y2);
		_printc(ll_corner); 
		setattrib(lb);
		gotoxy(x2,y1);
		_printc(ur_corner);
		gotoxy(x2,y2);
		_printc(lr_corner); 
 }

 u8b scancode;

 u8b OPOS_CPI getch()
 {	u8b k ; 
	_getkey(&k,&scancode);
	return k;	
 }

 # endif