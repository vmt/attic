/*********************
 * BOOT SECTOR WRITER
 * ------------------
 * by Vivek Mohan
 * http://www.geocities.com/cppresources/
 * If you are new to this stuff I'd strongly recommend you
 * experiment with floppies only.
 *********************/

 #include <bios.h>
 #include <stdio.h>

 /* Different Drives */

 #define _floppy1	0x0
 #define _floppy2	0x1

 #define _hd1		0x80
 #define _hd2		0x81
 #define _hd3		0x82

 /* Writes the boot sector */

 void writebootsector(char *buffer,int drive = _floppy1)
 {  printf("\nWriting....");
    int result;
    if((result=biosdisk(3,drive,0,0,1,1,buffer))==0)
    {   printf("1 sector copied successfully !");  }
    else
    {	printf("Error %d",result); }
 }

 int main(int argc,char* argv[])
 {	if(argc>1)
	{       FILE *fin = fopen(argv[1],"rb");
		if(fin)
		{       /* Buffer which stores the data 	*/
			char buffer[512];

			/* Read 512 bytes into the buffer 	*/
			fread(&buffer,512,1,fin);

			/* Write the 512 bytes */
			writebootsector(buffer,_floppy1);
			fclose(fin);
			return 1;
		}
	}
	printf("\ncopyboot.exe <filename>");
	return 0;
 }