/**********************************
 *
 * STRIPEXE
 * ========
 * Strips the header of an exe file. Creates two files
 * One the stripped exe and the other the header file.
 *
 * by Vivek Mohan
 * mail me : mailvivek27@sify.com
 * visit my home : http://www.geocities.com/cppresources/
 *
 **********************************/

# include <stdlib.h>
# include <conio.h>
# include <stdio.h>

void main(int argc,char *argv[])
{
	if(argc<=2)
	{ printf("Usage :.. <sourcefile(.exe)> <strippedfilename> <headerfile>");
	  exit(0);
	}

	/* The File Handles */

	FILE *srcFile , *dstFile , *hdrFile;

	printf("Opening source file %s",argv[1]);

	if((srcFile = fopen(argv[1],"rb"))==NULL)
	{ printf("\nfatal-error: file (%s) not found. exiting..",argv[1]);
	  exit(1);
	}

	printf("\nCreating destination files %s.bin and header.bin",argv[1]);

	if((dstFile = fopen(argv[2],"wb"))==NULL)
	{ printf("\nfatal-error: output file (%s) could not be created. exiting..",argv[1]);
	  exit(1);
	}

	if((hdrFile = fopen(argv[3],"wb"))==NULL)
	{ printf("\nfatal-error : header output could not be created.");
	  exit(1);
	}

	printf("\nStripping EXE header..");

	char* buffer = new char[512];

	fread (buffer,512,sizeof(char),srcFile);
	fwrite(buffer,sizeof(char),512,hdrFile);


	fclose(hdrFile);

	printf("\nWriting rest of the binary to %s",argv[2]);

	while(!feof(srcFile))
	{ fputc(fgetc(srcFile),dstFile); }

	printf("\nDone.");

	fclose(dstFile);
	fclose(srcFile);

	EXIT_SUCCESS;
}