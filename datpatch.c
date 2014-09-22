#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <io.h>



  char        filename[80];
  char        inbuf[28];
  char        outstr[] = "COMPILED ON %02d/%02d/%02d - %02d:%02d";
  char        teststr[] = "COMPILED ON MM/DD/YY - HH:MM";
  FILE       *infile;
  long        offset = 0L;
  time_t      t;
  struct      tm       *now;



main(argc,argv)
    int       argc;
    char     *argv[];
  {
    char      loopsw = 0;
    int       x;
    long      l;

                                       /*  check argument count:     */
                                       /*  must be 2!                */
    if (argc != 2)
       {
       printf("USAGE: DATPATCH filename\n");
       return(0);
       }

                                       /*  open the .EXE for update  */
    strcpy(filename,argv[1]);
    infile = fopen(filename,"rb+");
    if (infile == NULL)
       {
       printf("ERROR: Unable to open %s\n",filename);
       fclose(infile);
       return(0);
       }

                                       /*  begin scanning for the    */
                                       /*  target string             */
    printf("Scanning %s...\n",filename);
    while(loopsw == 0)
      {
      memset(inbuf,0x00,sizeof(inbuf));
      fseek(infile,offset,SEEK_SET);
      fread(inbuf,sizeof(inbuf),1,infile);
      if (memcmp(inbuf,teststr,sizeof(inbuf)) != 0)
         {
         l = sizeof(inbuf);
         for(x = 1; x < sizeof(inbuf); x++)
           {
           if ((inbuf[x] == 'C') && (inbuf[x + 1] == 'O'))
              l = x;
           }
         offset += l;
         }
        else
         loopsw = 2;

      if (feof(infile) != 0)
         loopsw = 1;
      }

                                       /*  if the target was found,  */
                                       /*  patch in the new string   */
    if (loopsw == 2)
       {
       printf("Patching %s at offset %lx\n",filename,offset);
       time(&t);
       now = localtime(&t);
       sprintf(inbuf,outstr,(now->tm_mon + 1),now->tm_mday,now->tm_year,
               now->tm_hour,now->tm_min);
       fseek(infile,offset,SEEK_SET);
       fwrite(inbuf,sizeof(inbuf),1,infile);
       }
      else
       printf("Could not find target in %s\n",filename);

    fclose(infile);

    return(0);
  }

