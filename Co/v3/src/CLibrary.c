/* +---------------------- Copyright notice -------------------------------+ */
/* | Copyright (C) 1995, Guy Barrand, LAL Orsay, (barrand@lal.in2p3.fr)    | */
/* |   Permission to use, copy, modify, and distribute this software       | */
/* |   and its documentation for any purpose and without fee is hereby     | */
/* |   granted, provided that the above copyright notice appear in all     | */
/* |   copies and that both that copyright notice and this permission      | */
/* |   notice appear in supporting documentation.  This software is        | */
/* |   provided "as is" without express or implied warranty.               | */
/* +---------------------- Copyright notice -------------------------------+ */
#include <stdio.h>

#ifdef HAS_DLD

/*IRIX seems to be like OSF1.*/

#ifdef __alpha /*ifdef_system*/
#include <dlfcn.h>
#endif

#ifdef __hpux /*ifdef_system*/
#include <dl.h>
#endif

#endif /*HAS_DLD*/

#include <CPrinter.h>
#include <CFile.h>
#include <CString.h>

#include <CLibrary.h>
/***************************************************************************/
void* CLibraryOpen (
 char* a_fname
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  void*  This = NULL;
/*.........................................................................*/
  if(a_fname==NULL) return NULL;

#ifdef HAS_DLD
 {FILE*  file;
  char*  fname;
  file   = CFileOpenForReading(a_fname,&fname);
  if(file==NULL) return NULL;
  fclose (file);
  
#ifdef __alpha /*ifdef_system*/
  This = dlopen (fname, RTLD_LAZY);
  if(This==NULL) /*Library could have been directly specified in the link commad of main executable.*/
    {
      CWarn ("Try to use main executable to load symbols.\n");
      This  = dlopen (NULL, RTLD_LAZY);
    }
#endif

#ifdef __hpux /*ifdef_system*/
 {shl_t  handle;
  handle = shl_load (fname,BIND_DEFERRED, 0L);
  This   = (void*)handle;}
#endif
  CStringDelete (fname);
  }

  if(This==NULL) CWarnF ("Can't open %s.\n",a_fname);

#else  /*not HAS_DLD*/
  CWarn ("Do not have dynamic loading.\n");
#endif /*HAS_DLD*/

  return    This;
}
/***************************************************************************/
void* CLibraryGetSymbol (
 void* This
,char* a_name
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  void* symbol = NULL;  
/*.........................................................................*/
  if(This==NULL)   return NULL;
  if(a_name==NULL) return NULL;

#ifdef HAS_DLD

#ifdef __alpha /*ifdef_system*/
  symbol      = dlsym (This,a_name);
#endif

#ifdef __hpux /*ifdef_system*/
 {shl_t       handle;
  handle      = (shl_t)This;
  shl_findsym (&handle,a_name,(short)TYPE_PROCEDURE,(void*)&symbol);}
#endif

  if(symbol==NULL) CWarnF ("Can't find symbol %s.\n",a_name);

#else  /*not HAS_DLD*/
  CWarn ("Do not have dynamic loading.\n");
#endif /*HAS_DLD*/

  return      symbol;
}

