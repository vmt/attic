
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
# include "init.c"
# include "console.c"
# include "shell.c"

void main()
{	/* Just to make it impressive , for now ! ;-) */
	int i=0 , j=0 , k = 0;
	char buffer[255];
	clear(color(BLACK,WHITE)) ;
	print("OPOS ver0.01",1,1);
	print("Loading.........",1,2) ;
	for(i=0;i<0xfff;++i)	
		for(j=0;j<0xfff;++j)
			for(k=0;k<0x4;++k);
			
	welcome();
	clear(color(BLACK,WHITE));
	print("Loading And Stuff....",1,1);
	print("A lot of system functions here ..in the future",1,2);
	print("But for now , lets just load a stupid little shell.",1,3);
	print("Press Any Key .. ",25,12);
	getch();
	osh_init();
	osh_load();
	print("Shut down..",1,3	);
	print("You may shut down the system now.",1,4);
	for(;;);
}

