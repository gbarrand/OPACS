#include <stdlib.h>

#include <CFile.h>

typedef unsigned long        Ulong;
/***************************************************************************/
FILE* CFileOpen (
 char* a_fname 
,char** a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  FILE*  fp;
/*.........................................................................*/
  if(!a_fname || !(*a_fname)) return 0;
  fp     = fopen(a_fname,"rb");
  a_name = NULL;
  return fp;
}
/***************************************************************************/
long CFileGetSize (
 FILE* This 
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
/*.........................................................................*/
  if(!This) return 0;
  fseek    (This, 0L, SEEK_END);  /* end of file*/
  return   ftell(This);
}
/***************************************************************************/
char* CFileRead (
 FILE* This 
,Ulong* a_size
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  long      filesize;
  char*     buff;
  int       nitem;
/*.........................................................................*/
  if(a_size) *a_size    = 0;
  if(!This)  return 0;
  filesize   = CFileGetSize(This);
  if(!filesize) return 0;
 /* add one byte to be able to add \n if file contain lines */
  buff       = (char*)malloc((size_t)( (filesize+1) * sizeof(char)));
  if(!buff)  return 0;
  rewind     (This);
  nitem      = fread((char *)buff, (size_t)filesize, (size_t)1, This);
  if(nitem!=1)
    {
      printf    ("can't read file.\n");
      free      (buff);
      return    0;
    }
  buff[filesize] = 0;
  if(a_size)     *a_size = filesize;
  return         buff;
}  
