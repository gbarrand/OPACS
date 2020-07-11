/*
        mapana.c
         Created           :  8-DEC-1989 by Christian Arnault
*/
 
#include <stdio.h>                   /* I/O definitions                       */
#include <stdlib.h>
#include <string.h>


typedef struct _CSymbRec*  CSymb;

typedef struct _CSymbRec
{
  char* name;
  int   type;
} CSymbRec;

char* Concatenate     (char*,char*);
char* DuplicateString (char*);

#define BLOCK_SYMB 64
static struct {
  int       symbn;
  int       symbi;
  CSymbRec* symbs;
} Class = {0,0,NULL};

#ifdef __vax
char com_str_vaxc[] = "PIC,USR,OVR,REL,GBL,  SHR,NOEXE,  RD,  WRT,NOVEC";
char com_str_decc[] = "PIC,USR,OVR,REL,GBL,NOSHR,NOEXE,  RD,  WRT,NOVEC";
#elif __alpha
char com_str_vaxc[] = "PIC,USR,OVR,REL,GBL,  SHR,NOEXE,  RD,  WRT,NOVEC";  /*Not tested.*/
char com_str_decc[] = "NOPIC,OVR,REL,GBL,NOSHR,NOEXE,  WRT,NOVEC,  MOD";
#else
char com_str_vaxc[] = "";
char com_str_decc[] = "";
#endif
/***************************************************************************/
int main (
 int argc
,char* argv[]
)
/***************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
  char* name = NULL;
  int major_id;
  int minor_id;
  
  char* name_lis;
  char* name_opt;
  char* name_sopt;
  char* name_map;
  
  char* symbol_file_name;
  char prefix[80];
  
  FILE* file_lis;
  FILE* file_opt;
  FILE* file_sopt;

#ifdef __vax
  char* name_smar;
  FILE* file_smar;
#endif
  
  FILE* file_map;
  FILE* file_sym;
  
  char line[256];
  char* str;
  char* blank;
  char* nl;
  
  static char common[32] = "";
   int count;
/*.........................................................................*/

  major_id = 1;
  minor_id = 1;
  symbol_file_name = NULL;
  
  for (argc--,argv++;argc;argc--,argv++)
  {
    str = *argv;
    if (*str == '-')
    {
      str++;
      switch (*str)
      {
        case 's' :
          str += 2;
          symbol_file_name = str;
          break;
        case 'e' :
          str += 2;
          break;
      }
    }
    else name = str;
  }
  
  name_lis  = Concatenate (name, ".lis");
  name_opt  = Concatenate (name, ".opt");
  name_sopt = Concatenate (name, "s.opt");
#ifdef __vax
  name_smar = Concatenate (name, "s.mar");
#endif
  name_map  = Concatenate (name, ".map");
  
  file_opt  = fopen (name_opt, "w+");
  file_sopt = fopen (name_sopt, "w+");
#ifdef __vax
  file_smar = fopen (name_smar, "w+");
#endif
  file_map  = fopen (name_map, "r");
  
  fprintf (file_sopt, "%ss/shareable\n", name);
  
#ifdef __vax
  fprintf (file_smar, "     .title transfer_vector\n");
  fprintf (file_smar, "     .ident /1-001/\n");
  fprintf (file_smar, "     .macro routine name name2\n");
  fprintf (file_smar, "     .extrn         name\n");
  fprintf (file_smar, "     .align quad\n");
  fprintf (file_smar, "     .transfer      name\n");
  fprintf (file_smar, "     .mask          name\n");
  fprintf (file_smar, "     jmp            name+2\n");
  fprintf (file_smar, "     .if gt %%length(name2)\n");
  fprintf (file_smar, "     .extrn         name2\n");
  fprintf (file_smar, "     .align quad\n");
  fprintf (file_smar, "     .transfer      name2\n");
  fprintf (file_smar, "     .mask          name2\n");
  fprintf (file_smar, "     jmp            name2+2\n");
  fprintf (file_smar, "     .endc\n");
  fprintf (file_smar, "     .endm\n");
  fprintf (file_smar,
    "     .psect $tvector,pic,usr,con,rel,lcl,shr,exe,rd,nowrt,quad\n");
#endif
  
  prefix[0] = '\0';
  if (symbol_file_name!=NULL)
  {
    file_sym  = fopen (symbol_file_name, "r");
    fscanf (file_sym, "%[^\n]%*c", line);
    sscanf (line, "%d %d", &major_id, &minor_id);
    fgets (prefix, sizeof(prefix), file_sym);
    nl = strchr (prefix, '\n');
    if (nl!=NULL) *nl = '\0';
  }
  
  fprintf (file_opt, "name = %s\n", name);  
  fprintf (file_opt, "identification = \042V%d.%d\042\n", major_id, minor_id);
  fprintf (file_opt, "gsmatch = lequal,%d,%d\n", major_id, minor_id);

#ifdef __vax
  fprintf (file_opt, "unsupported = 1\n");
  fprintf (file_opt, "cluster = cluster0,,,%ss\n", name);
#endif
  
  fprintf (file_opt, "cluster = cluster1\n");
  fprintf (file_opt, "collect = cluster1,$code,$data\n");

/*  printf ("debug:lis:%s\n",name_lis);*/
  file_lis  = fopen (name_lis, "r");
  do
    {
      fgets (line, sizeof(line), file_lis);
      nl = strchr (line, '\n');
      if (nl!=NULL) *nl = '\0';
      str = (char*) strchr(line, '-');
      if ( (str!=NULL) && str[1] == 'R')
	{
	  blank   = strchr (line, ' ');
	  *blank  = '\0';
	  if( (line!=NULL) && (*line!='\0') )
	    {
	      if(Class.symbi>=Class.symbn)
		{
		  Class.symbs = Class.symbn!=0 ? 
		      (CSymbRec*)realloc(Class.symbs,(Class.symbn+BLOCK_SYMB)*sizeof(CSymbRec)) :
		      (CSymbRec*)malloc(                        (BLOCK_SYMB)*sizeof(CSymbRec)) ;
		  Class.symbn += BLOCK_SYMB;
		}       
	      Class.symbs[Class.symbi].name = DuplicateString(line);
	      Class.symbs[Class.symbi].type = 0;  /*Assume procedure*/
	      Class.symbi++;
	    }
	}
    } while (feof(file_lis)==0);
  fclose (file_lis);

  do
    {
      fgets (line, sizeof(line), file_map);
      nl = strchr (line, '\n');
      if (nl!=NULL) *nl = '\0';
      if (  (strstr(line, com_str_decc)!=NULL) || (strstr (line, com_str_vaxc)!=NULL) )
	{
	  str    = &line[0];
	  while  (*str == ' ') str++;
	  blank  = (char*) strchr (str, ' ');
	  *blank = '\0';
	  if (strcmp (str, common)!=0)
	    {
	      strcpy (common, str);
	      for(count=0;count<Class.symbi;count++)
		{
		  if(strcmp(str,Class.symbs[count].name)==0)
		    {
		      Class.symbs[count].type = 1;
		    }
		}
	    }
	}
    } while (feof(file_map)==0);
  
  for(count=0;count<Class.symbi;count++)
    {
      char* name;
      name  = Class.symbs[count].name;
      if(Class.symbs[count].type==1)
	{
#ifdef __vax
	  fprintf (file_opt,  "universal = %s\n", name);
#else
          fprintf (file_opt,  "symbol_vector = (%s = data)\n", name);
#endif
        }
      else
	{
#ifdef __vax
	  fprintf (file_opt,  "universal = %s\n", name);
#else
	  fprintf (file_opt,  "symbol_vector = (%s = procedure)\n", name);
#endif
  
#ifdef __vax
          fprintf (file_smar, "     routine %s\n", name);
#endif
        }
    }
#ifdef __vax
  fprintf (file_smar, "     .end\n");
#endif
  
  for(count=0;count<Class.symbi;count++)
    {
      char* name;
      name  = Class.symbs[count].name;
      if(Class.symbs[count].type==1)
	{
	  fprintf (file_opt, "psect_attr = %s, noshr\n", name);
        }
    }

  fclose  (file_opt);
  fclose  (file_sopt);

#ifdef __vax
  fclose  (file_smar);
#endif
  
  fclose  (file_map);

  return  EXIT_SUCCESS;
}

/*---------------------------------------------------------------------------*/ 
char* Concatenate (char* str1,char* str2)
/*---------------------------------------------------------------------------*/
{
  char* str;
  
  str = (char*) malloc (strlen(str1) + strlen(str2) + 1);
  strcpy (str, str1);
  strcat (str, str2);
  return (str);
}

/*---------------------------------------------------------------------------*/ 
char* DuplicateString (char* str1)
/*---------------------------------------------------------------------------*/
{
  char* str;
  
  str = (char*) malloc (strlen(str1) + 1);
  strcpy (str, str1);
  return (str);
}
