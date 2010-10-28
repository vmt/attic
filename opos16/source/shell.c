 /* THE $tupid $hell for OPOS */
 /* ========================= */

 /* Author : Vivek Mohan 
  *	 	(Home: http://www.geocities.com/cppresources/)
  *		(Mail : mailvivek27@sify.com || opendev@phreaker.net)
  * OPOS HOME : http://opos.t35.com/
  */
  
 /* COPYRIGHT ? Who needs a copyright for this stupid shell , just do what ever
  * you want expect for calling it your own .
  */

 # ifndef _SHELL_
 # define _SHELL_
 
 # include "defs.h"
 # include "syssrv.c"
 # include "string.h"
 # include "console.c"
 
 /* $hell 		*/
 /* =================== */
 /* The OPOS Basic Shell*/

 u8b 	osh_x = 1	, 
	osh_y = 3	,
 	osh_cmd_buff[300],
 	osh_header_message[] = "'$tupid $hell' v0.01",
	osh_bg = BLACK	,
	osh_fg = WHITE	,
	osh_prompt_msg[10] = "$hell/:" ;
	osh_cmd_parser_buff[300] ;

 # define kill_opos 	1
 # define reboot_opos 	2

 u8b osh_prompt();
 void osh_init();
 void osh_load();
 void osh_help();
 void osh_putmsg(char*);

 /* Loads or reloads the shell */
 void osh_init()
 {	
	osh_x = 1	, 
	osh_y = 3	;

	osh_cmd_buff[0] = 0;

	/* Clear screen 	  */
	clear(color(BLACK,WHITE));
	
	/* print the header 	  */
	print(osh_header_message,1,1);
	drawhline(1,2,78,color(BLACK,YELLOW));

	/* set shell color attrib */
	setattrib(color(osh_bg,osh_fg));

	/* Initialize the parser  */
	osh_cmd_parser_buff[0] = 0;

 }

 void osh_load()
 {	u8b message = 0;
	while((message=osh_prompt())!=0)
	{	if(message==kill_opos)
		{	clear(color(BLACK,WHITE));
			print("Terminating system ",1,1);
			print("Breaking to main()..",1,2);
			break;			
		}
		else if(message==reboot_opos)
		{	clear(color(BLACK,WHITE));
			print("Terminating system ",1,1);
			print("Rebooting..",1,2);
			_reboot();	
		}
		else if(message==100)
		{	osh_putmsg("Not A Valid OSH-Command. Type `help'.");

		}
	}
 }
 
 u8b osh_prompt()
 {	u8b  	c	= 0 , 
		       *cmdptr 	= osh_cmd_buff ;
		u16b 	counter = 0 ;
		
		print(osh_prompt_msg,osh_x=1,osh_y);
		osh_x+=strlen(osh_prompt_msg);

		gotoxy(++osh_x,osh_y);

		while(c!=13) /* EOL */
		{	c = getch();
			if(c==8) /* Back space */
			{	if(counter>0&&(osh_x)>1)
				{	cmdptr[--counter]=0;
					gotoxy(--osh_x,osh_y);
					_printc(0);
				}
				else
				if(osh_x==0)
				{	gotoxy((osh_x=79),--osh_y);
					_printc(0);
				}
			}
			else if(c==13) /* ENTER */
			{
				break;
			}
			else
			{	if(osh_x<79)
				{	_printc(c);
					gotoxy(++osh_x,osh_y);
					cmdptr[counter++] = c;
				}
				else
				{	if(osh_y<24)
					{	osh_x = 0;
						osh_y+= 1;
						gotoxy(++osh_x,osh_y);
						_printc(c);
						cmdptr[counter++] = c;
						gotoxy(++osh_x,osh_y);
					}
					else 
					{
						osh_x = 0;
						_scrollup(1,0,3,79,24);
						gotoxy(++osh_x,osh_y=24);
						
					}				
				}		
			}
		}
		if(osh_y>=24)
		{	_scrollup(1,0,3,79,24);
			osh_y = 24;
		}
		else
		{	osh_y++;

		}		
		cmdptr[counter] = 0;
	return osh_command();
 }
 
 int osh_command()
 {	int i=0;
	char command[100];
	for(;i<100&&osh_cmd_buff[i]!=' '&&osh_cmd_buff[i]!=13&&osh_cmd_buff[i];++i)
	{	command[i] = osh_cmd_buff[i];	}
	command[i] = 0;
	
	if(streq(command,"clear")==1)
	{
		osh_init();
		
		return 10;
	}
	
	if(streq(command,"help")==1)
	{

		osh_help();

		return 11;

	}

	if(streq(command,"shutdown")==1)
	{
		return 1;

	}

	if(streq(command,"reboot")==1)
	{
		return 2;
	}

	if(streq(command,"joindevteam")==1)
	{
		osh_putmsg("Hi ! First of all thanks for downloading this. If");
		osh_putmsg("you'd like to join the team , all you have to do ");
		osh_putmsg("is send an email to opendev@phreaker.net or to   ");
		osh_putmsg("mailvivek27@sify.com , with your email , home page ");
		osh_putmsg("and a little info about your self. Thats it , you  ");
		osh_putmsg("are in the team no matter what your skill level is.");		
		
		return 12;
	}

	return 100; 
 }

 void osh_help()
 {
	osh_init();
	
	osh_putmsg("clear       - clears and refreshes the console.");
	osh_putmsg("help        - shows this screen");
	osh_putmsg("shutdown    - shuts down the system");
	osh_putmsg("reboot      - reboot the system");
	osh_putmsg("joindevteam - Info On How To Join The Development Team ");
	osh_putmsg(" ");
 }

 void osh_putmsg(char *s)
 {
	if(osh_y<24)
		print(s,1,osh_y++);
	else 
	{	print(s,1,osh_y=24);
		_scrollup(1,0,3,79,24);			
	}
 }

 # endif