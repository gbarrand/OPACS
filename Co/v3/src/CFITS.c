#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <CMemory.h>
#include <CPrinter.h>
#include <CError.h>

#include <CFITS.h>

typedef unsigned char        Uchar;
typedef unsigned long        Ulong;

#ifdef __cplusplus
extern "C"{
#endif
static void   StripString  (char*,char,char);
static int    FindPosition (char*,char);
static void   ReportError  (char*,int);
#ifdef __cplusplus
}
#endif

#define BUFF_SIZE 4096
#define HEAD_SIZE 2880
/******************************************************************************/
short* CFITS_GetImageFromStream (
 FILE* a_file
,int a_x_org   
,int a_y_org          
,unsigned int a_width         
,unsigned int a_height  
,int a_swap    
)
/******************************************************************************/
/*   Fonction  READ_FITS  :  Lecture d'un fichier au format fits ds une       */
/*   a_fname     Nom du fichier image                                         */
/*   a_x_org     x Offset par rapport a l'origine du fichier                  */
/*   a_y_org     y Offset par rapport a l'origine du fichier                  */
/*   a_width                                                                  */
/*   a_height                                                                 */
/*   a_swap      Swap flag =0 No swap                                         */
/*        Fonctions de Lecture/Ecriture...  d'images au format  FITS          */
/*                                                    R.Ansari  08/90         */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE* imfip;
  short* picmap;
  char *buff,*cpt,*mcpt,*argpt,c;
  short* sipt;
  int i,j,k,m;
  int select;
  int x_axe,y_axe,xmx,ymx;
  long  int lo,lrd,totr;
  int   fini;
/*............................................................................*/
/*      Ouverture du fichier :            */
  if(a_file==NULL)                    return NULL;
  if( (a_width==0) || (a_height==0) ) return NULL;
  imfip      = a_file;
  rewind     (imfip);
  if ( (a_x_org<0)||(a_y_org<0) )
    {
      ReportError  ("CFITS_GetImageFromStream",CErrorValue);
      return NULL;
    }
  picmap     = (short*) CMemoryAllocateBlock( (a_width * a_height)*sizeof(short));
  if(picmap==NULL) return NULL;
  buff       = (char*)CMemoryAllocateBlock(BUFF_SIZE*sizeof(char));
  if(buff==NULL)  {CMemoryFreeBlock(picmap);return NULL;}
/*     Lecture entete  :                   */
  x_axe=0;
  y_axe=0;
  for (fini=0;fini==0;)
    {
    fread(buff,(size_t)(1),(size_t)(HEAD_SIZE),imfip);
    /*	    Entete formatte sous forme de groupes de 80 caracteres  :  */
    for(k=0;k<36;k++)
      {
      cpt = &(buff[k*80]);
      c=(*(cpt+80));
      *(cpt+80)='\0';
      StripString(cpt,'L',' ');
      i=FindPosition(cpt,' ');
      *(cpt+i)='\0';
      mcpt=cpt;
      argpt=cpt+i+1;
      i=FindPosition(argpt,'=');
      argpt+=(i+1);
      StripString((argpt),'L',' ');
      j=FindPosition((argpt),' ');
      *(argpt+j)='\0';
      *(cpt+80)=c;
      
    /*	   Decodage des longueurs des axes  :  */
      select=0;
      if (strcmp(mcpt,"NAXIS1") == 0)  select=1; /* Axes X : Nb Pixel/ligne  */
      if (strcmp(mcpt,"NAXIS2") == 0)  select=2; /* Axes Y : Nb de lignes*/
      if (strcmp(mcpt,"END")    == 0)  fini=1;
       
      if (select>0) 
	{
	m=atoi((argpt));
	if (select == 1)  x_axe=m;
	if (select == 2)  y_axe=m;
	}
      }
    }
  if (x_axe == 0)
    {
      x_axe = 1024;
      CWarn ("Axe X prise par defaut = 1024.\n");
    }
  if (y_axe == 0)
    {
      y_axe = 1024;
      CWarn ("Axe Y prise par defaut = 1024.\n");
    }

/*CInfoF("Axes decodes : X=%d, Y=%d.\n",x_axe,y_axe);*/

  xmx=x_axe-a_x_org+1;
  ymx=y_axe-a_y_org+1;
  if (xmx>(int)a_width)   xmx=a_width;
  if (ymx>(int)a_height)  ymx=a_height;

  if ( (xmx<0)||(ymx<0) )
    {
      ReportError    ("CFITS_GetImageFromStream",CErrorValue);
      CMemoryFreeBlock (buff);
      CMemoryFreeBlock (picmap);
      return   NULL;
    }

  totr=0;
/*    On se positionne au debut de la portion a lire :   */
  lo=((a_y_org-1)*x_axe+a_x_org-1)*2;
  fseek(imfip,lo,SEEK_CUR);
  lo=(x_axe-xmx)*2;

  if(a_swap == 0) 
    {
      for(j=0;j<ymx;j++)     /*  On lit SANS byteSwapper : */
	{
	  sipt=(short*)((picmap)+j * a_width);
	  lrd=fread(sipt,(size_t)sizeof(short),(size_t)(xmx),imfip);
	  if(lrd != xmx) 
	    {
	      ReportError      ("CFITS_GetImageFromStream",CErrorIO);
	      CMemoryFreeBlock (buff);
	      CMemoryFreeBlock (picmap);
	      return           NULL;
	    }
	  fseek(imfip,lo,SEEK_CUR);    /*  On se positionne au debut de la ligne suivante  */
	  totr=totr+2*lrd;
	}
    }
  else 
    {
      for(j=0;j<ymx;j++)       /*  On byteSwappe : */
	{
	  lrd=fread(buff,(size_t)(2),(size_t)(xmx),imfip);
	  if(lrd != xmx) 
	    {
	      ReportError      ("CFITS_GetImageFromStream",CErrorIO);
	      CMemoryFreeBlock (buff);
	      CMemoryFreeBlock (picmap);
	      return           NULL;
	    }
	  /*    On bytea_swapp  */
	  cpt=(char *)((picmap)+j * a_width);
	  for(i=0;i<2*xmx;i+=2)
	    {
	      *(cpt+i)=buff[i+1];
	      *(cpt+i+1)=buff[i];
	    }
	  fseek(imfip,lo,SEEK_CUR);    /*  On se positionne au debut de la ligne suivante  */
	  totr=totr+2*lrd;
	}
    }

/*CInfoF("Fichier lu (total %d bytes).\n",totr);*/

/* Si la portion de l'image lue est plus petite que la taille de la structure */
/*  On met a zero la partie non utilisee .                                    */
  if (ymx<(int)a_height)
    {
      for(j=ymx;j<(int)a_height;j++)
        {
          for(i=0;i<(int)a_width;i++)  *(picmap+(j * a_width+i))='\0';
        }
     }
  if (xmx<(int)a_width)
    {
      for(j=0;j<ymx;j++)
        {
          for(i=xmx;i<(int)a_width;i++)  *(picmap+(j * a_width+i))='\0';
        }
    }
/*
imp->org_x = a_x_org;
imp->org_y = a_y_org;
*/
  CMemoryFreeBlock (buff);
  return picmap;
}
/******************************************************************************/
void CFITS_GetHeaderFromStream (
 FILE* a_file
,unsigned int* a_xSize    
,unsigned int* a_ySize    
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE* imfip;
  char *buff,*cpt,*mcpt,*argpt,c;
  int i,j,k,m;
  int select;
  int x_axe,y_axe;
  int fini;
/*............................................................................*/
  if(a_xSize!=NULL) *a_xSize = 0;
  if(a_ySize!=NULL) *a_ySize = 0;
  if(a_file==NULL) return;
  imfip      = a_file;
  rewind     (imfip);

  buff       = (char*)CMemoryAllocateBlock(BUFF_SIZE*sizeof(char));
  if(buff==NULL) return;

/*     Lecture entete  :                   */
  x_axe=0;
  y_axe=0;

  for (fini=0;fini==0;)
    {
    fread(buff,(size_t)(1),(size_t)(HEAD_SIZE),imfip);
    /*	    Entete formatte sous forme de groupes de 80 caracteres  :  */
    for(k=0;k<35;k++)
      {
      cpt = &(buff[k*80]);
      c=(*(cpt+80));
      *(cpt+80)='\0';
      StripString(cpt,'L',' ');
      i=FindPosition(cpt,' ');
      *(cpt+i)='\0';
      mcpt=cpt;
      argpt=cpt+i+1;
      i=FindPosition(argpt,'=');
      argpt+=(i+1);
      StripString((argpt),'L',' ');
      j=FindPosition((argpt),' ');
      *(argpt+j)='\0';
      *(cpt+80)=c;
      
    /*	   Decodage des longueurs des axes  :  */
      select=0;
      if (strcmp(mcpt,"NAXIS1") == 0)  select=1; /* Axes X : Nb Pixel/ligne  */
      if (strcmp(mcpt,"NAXIS2") == 0)  select=2; /* Axes Y : Nb de lignes*/
      if (strcmp(mcpt,"END")    == 0)  fini=1;
       
      if (select>0) 
	{
	m=atoi((argpt));
	if (select == 1)  x_axe=m;
	if (select == 2)  y_axe=m;
	}
      }
    }
  if (x_axe == 0)
    {
      x_axe = 1024;
      CWarn ("Axe X prise par defaut = 1024.\n");
    }
  if (y_axe == 0)
    {
      y_axe = 1024;
      CWarn ("Axe Y prise par defaut = 1024.\n");
    }

/*CInfoF("Axes decodes : X=%d ,Y=%d.\n",x_axe,y_axe);*/

  if(a_xSize!=NULL) *a_xSize = x_axe;
  if(a_ySize!=NULL) *a_ySize = y_axe;
  CMemoryFreeBlock (buff);
}
/******************************************************************************/
void CFITS_DumpHeaderInStream (
 FILE* a_file
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE* imfip;
  char *buff,*cpt,*mcpt,*argpt,c;
  int i,j,k,m;
  int select;
  int x_axe,y_axe;
  int fini;
/*............................................................................*/
  if(a_file==NULL) return;
  imfip      = a_file;
  rewind     (imfip);

  buff       = (char*)CMemoryAllocateBlock(BUFF_SIZE*sizeof(char));
  if(buff==NULL) return;

/*     Lecture entete  :                   */
  x_axe=0;
  y_axe=0;

  for (fini=0;fini==0;)
    {
    fread(buff,(size_t)(1),(size_t)(HEAD_SIZE),imfip);
    /*	    Entete formatte sous forme de groupes de 80 caracteres  :  */
    for(k=0;k<35;k++)
      {
      cpt = &(buff[k*80]);
      c=(*(cpt+80));
      *(cpt+80)='\0';
      StripString(cpt,'L',' ');
      i=FindPosition(cpt,' ');
      *(cpt+i)='\0';
      mcpt=cpt;
      argpt=cpt+i+1;
      i=FindPosition(argpt,'=');
      argpt+=(i+1);
      StripString((argpt),'L',' ');
      j=FindPosition((argpt),' ');
      *(argpt+j)='\0';
      *(cpt+80)=c;
      
    /*	   Decodage des longueurs des axes  :  */
      CDumpF ("%s %s\n",mcpt,argpt);

      select=0;
      if (strcmp(mcpt,"NAXIS1") == 0)  select=1; /* Axes X : Nb Pixel/ligne  */
      if (strcmp(mcpt,"NAXIS2") == 0)  select=2; /* Axes Y : Nb de lignes*/
      if (strcmp(mcpt,"END")    == 0)  fini=1;
       
      if (select>0) 
	{
	m=atoi((argpt));
	if (select == 1)  x_axe=m;
	if (select == 2)  y_axe=m;
	}
      }
    }
  if (x_axe == 0)
    {
      x_axe = 1024;
      CWarn ("Axe X prise par defaut = 1024.\n");
    }
  if (y_axe == 0)
    {
      y_axe = 1024;
      CWarn ("Axe Y prise par defaut = 1024.\n");
    }

/*CInfoF("Axes decodes : X=%d ,Y=%d.\n",x_axe,y_axe);*/

  CMemoryFreeBlock (buff);
}
/******************************************************************************/
void CFITS_PutImageInStream (
 FILE* a_file
,short* a_data
,unsigned int a_width
,unsigned int a_height
,int a_swap        
)
/******************************************************************************/
/*   Fonction  WRITE_FITS  :  Ecritue d'un fichier au format fits depuis une  */
/*   structure image.                                                         */
/*   a_swap Swap flag =0 Noswap                                               */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE* imfip;
  short* picmap;
  char *buff,*cpt;
  long int i,j,imx,TotNbPix;
/*............................................................................*/
  if(a_file==NULL) return;
  if(a_data==NULL) return;
  if( (a_width<=0) || (a_height<=0) ) return;
  picmap     = a_data;
  imfip      = a_file;
  buff       = (char*)CMemoryAllocateBlock(BUFF_SIZE*sizeof(char));
  if(buff==NULL)  return;

/*     Ecriture entete  :                   */
  sprintf(buff,     "SIMPLE  =                    T   / Format standard %40s"," ");
  sprintf(buff+80,  "BITPIX  =                   16   / 2 Byte per pixel %40s"," ");
  sprintf(buff+160, "NAXIS   =                    2   / 2 axes  %40s"," ");
  sprintf(buff+240, "NAXIS1  = %20u   / Nb de colonnes %40s",a_width," ");
  sprintf(buff+320, "NAXIS2  = %20u   / Nb de lignes %40s"  ,a_height," ");
  sprintf(buff+400, "ORIGIN  = 'Image             '  / Programme Image (LAL) %40s"," ");
  sprintf(buff+480, "OBJECT  = 'Image Traite      '  / Resultat traitement %40s"," ");
  sprintf(buff+560, "CRVAL1  =         .1000000E+01  / Pour                %40s"," ");
  sprintf(buff+640, "CRPIX1  =                  1.0  / faire               %40s"," ");
  sprintf(buff+720, "CDELT1  =         .1000000E+01  / plaisir             %40s"," ");
  sprintf(buff+800, "CRVAL2  =         .1000000E+01  / a                   %40s"," ");
  sprintf(buff+880, "CRPIX2  =                  1.0  / Luciano             %40s"," ");
  sprintf(buff+960, "CDELT2  =         .1000000E+01  / et Jim              %40s"," ");
  sprintf(buff+1040,"END    ");
  for(i=1045;i<HEAD_SIZE;i++)  *(buff+i)=' ';
  fwrite(buff,(size_t)(1),(size_t)(HEAD_SIZE),imfip);

/*   Taille total image   */
  TotNbPix=a_width * a_height;
	
  if(a_swap == 0)     /*  On ecrit SANS byteSwapper : */
    fwrite(picmap,(size_t)(sizeof(short int)),(size_t)(TotNbPix),imfip);

  else for(j=0;j<TotNbPix;j+=BUFF_SIZE/2)       /*  On byteSwappe : */
    {                                           /* j : index en short ints */
    imx=TotNbPix-j;
    if (imx > BUFF_SIZE/2)  imx = BUFF_SIZE/2;  /* imx : en nombre de short ints  */
    cpt=(char *)((picmap)+j);              /* picmap est un short int * */
    for(i=0;i<2*imx;i+=2)
      {
      buff[i+1]=(*(cpt+i));
      buff[i]=(*(cpt+i+1));
      }
    fwrite(buff,(size_t)(sizeof(short int)),(size_t)(imx),imfip);
   }

/*CInfoF("Fichier Ecrit (total %d pixels, %d bytes).\n",TotNbPix,TotNbPix*2+HEAD_SIZE);*/

  CMemoryFreeBlock (buff);
}
/******************************************************************************/
static void StripString (
 char* a_cst
,char a_opt
,char a_bc
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int i,j,ls;
/*............................................................................*/
  ls = strlen(a_cst);
  if (a_opt == 'L' || a_opt == 'B')
    {
      i = 0;
      while(a_cst[i] == a_bc) i++;
      if (i != 0)
        {
          for(j=i;j<ls;j++)  a_cst[j-i] = a_cst[j];
          ls -= i;
          a_cst[ls] = '\0';
        }
    }
  if ((a_opt == 'T' || a_opt == 'B') && ls > 0)
    {
      i = ls-1;
      while(a_cst[i] == a_bc)  i--;
      a_cst[i+1] = '\0';
    }
}
/******************************************************************************/
static int FindPosition (
 char* a_cst
,char a_sec
)
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  int i,ls;
/*............................................................................*/
  ls = strlen(a_cst);
  for(i=0;i<ls;i++)
    {
      if (a_cst[i] == a_sec)  return(i);
    }
  return (-1);
}
/***************************************************************************/
static void ReportError (
 char* a_name
,int a_code
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  CErrorHandle ("Co","CFITS",a_name,a_code);
}
