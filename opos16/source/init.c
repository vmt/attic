
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
 # include "console.c"

 void init();
 void welcome();

 void init()
 {

 /* No Initializations for now */

 }

 /* The Welcoming Screen */
 /* NOTE : Inspired by SuSE Linux ;-) */
 
 void welcome()
 {	clear(color(BLUE,WHITE));
	boxoutset(10,2,70,8,color(BLUE,WHITE),color(BLUE,BLACK));
	boxinset (18,3,62,7,color(BLUE,WHITE),color(BLUE,BLACK));

	setattrib(color(BLUE,WHITE));
	print("OPOS - the OPen source Operating System",21,5);

	boxinset (10,10,70,22,color(BLUE,WHITE),color(BLUE,BLACK));

	print("* Project      : OPOS ",12,12);
	print("* Initiated By : Vivek Mohan (opendev@phreaker.net) ",12,13);
	print("* Dev Team     : Vivek Mohan (mail to join)",12,14);
	print("* Project Home : http://opos.t35.com/",12,15);

	print("* Current Project Status..",12,17);
	print("         * Planning",12,18);
	print("         * Initial Coding",12,19);
	print("* TO DO...",12,20);
	print("         * >o) Uhmmmmmmm....... ",12,21);

	print(" Press Any Key To Continue... ",23,24);
	getch();
	clear(color(BLACK,WHITE));
 }