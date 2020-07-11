#ifndef HTUTILS_H
#define HTUTILS_H

/*
  Utitlity macros for the W3 code library
  MACROS FOR GENERAL USE
  Generates: HTUtils.h
  See also: the system dependent file "tcp.h"
*/

#ifndef DEBUG
#define DEBUG   /* Noone ever turns this off as trace is too important */
#endif          /* Keeep option for really small memory applications tho */
                
#ifdef SHORT_NAMES
#define WWW_TraceFlag HTTrFlag
#endif

#ifdef DEBUG
#define TRACE (WWW_TraceFlag)
#define PROGRESS(str) printf(str)
        extern int WWW_TraceFlag;
#else
#define TRACE 0
#define PROGRESS(str) /* nothing for now */
#endif

#define CTRACE if(TRACE)fprintf
#define tfp    stderr

#define PUBLIC                  /* Accessible outside this module     */
#define PRIVATE static          /* Accessible only within this module */

#ifndef CONST
#define CONST
#endif
#define NOPARAMS (void)
#define PARAMS(parameter_list) parameter_list
#define NOARGS (void)
#define ARGS1(t,a) \
                (t a)
#define ARGS2(t,a,u,b) \
                (t a, u b)
#define ARGS3(t,a,u,b,v,c) \
                (t a, u b, v c)
#define ARGS4(t,a,u,b,v,c,w,d) \
                (t a, u b, v c, w d)
#define ARGS5(t,a,u,b,v,c,w,d,x,e) \
                (t a, u b, v c, w d, x e)
#define ARGS6(t,a,u,b,v,c,w,d,x,e,y,f) \
                (t a, u b, v c, w d, x e, y f)
#define ARGS7(t,a,u,b,v,c,w,d,x,e,y,f,z,g) \
                (t a, u b, v c, w d, x e, y f, z g)
#define ARGS8(t,a,u,b,v,c,w,d,x,e,y,f,z,g,s,h) \
                (t a, u b, v c, w d, x e, y f, z g, s h)
#define ARGS9(t,a,u,b,v,c,w,d,x,e,y,f,z,g,s,h,r,i) \
                (t a, u b, v c, w d, x e, y f, z g, s h, r i)
#define ARGS10(t,a,u,b,v,c,w,d,x,e,y,f,z,g,s,h,r,i,q,j) \
                (t a, u b, v c, w d, x e, y f, z g, s h, r i, q j)

typedef char HTBoolean;
#define YES  ((HTBoolean)1)
#define NO   ((HTBoolean)0)

#define TCP_PORT 80     /* Allocated to http by Jon Postel/ISI 24-Jan-92 */

/*      Inline Function WHITE: Is character c white space? */
/*      For speed, include all control characters */

#define WHITE(c) (((unsigned char)(c)) <= 32)


/*Sucess (>=0) and failure (<0) codes */

#define HT_REDIRECTING  29998
#define HT_LOADED       29999           /* Instead of a socket */
#define HT_INTERRUPTED -29998
#define HT_NOT_LOADED  -29999
#define HT_OK               0           /* Generic success*/

#define HT_NO_ACCESS    -10             /* Access not available */
#define HT_FORBIDDEN    -11             /* Access forbidden */
#define HT_INTERNAL     -12             /* Weird -- should never happen. */
#define HT_BAD_EOF      -12             /* Premature EOF */

#define outofmem(file, func) \
 { fprintf(stderr, "%s %s: out of memory.\nProgram aborted.\n", file, func); \
  exit(1);}


/*Upper- and Lowercase macros
   The problem here is that toupper(x) is not defined officially unless isupper(x) is.
   These macros are CERTAINLY needed on #if defined(pyr) || define(mips) or BDSI
   platforms. For safefy, we make them mandatory.
*/

/* Pyramid and Mips can't uppercase non-alpha */
#ifndef TOLOWER
#define TOLOWER(c) (isupper(c) ? tolower(c) : (c))
#define TOUPPER(c) (islower(c) ? toupper(c) : (c))
#endif /* ndef TOLOWER */

#define CR '\015'	/* Must be converted to ^M for transmission */
#define LF '\012'	/* Must be converted to ^J for transmission */

#define strdup_(str)  ((str) != NULL ? (strcpy((char*)malloc((unsigned)strlen(str) + 1), str)) : (char*)NULL)

#endif /* HTUTILS_H */
