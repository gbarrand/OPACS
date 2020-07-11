/*          HTAccess:  Access manager  for libwww
                                      ACCESS MANAGER
                                             
   This module keeps a list of valid protocol (naming scheme)
   specifiers with associated access code.  It allows documents to be
   loaded given various combinations of parameters. New access
   protocols may be registered at any time.
   
   Part of the libwww library .
   
 */
#ifndef HTACCESS_H
#define HTACCESS_H

/*      Definition uses:
*/
#include "HTUtils.h"
#include "HTAnchor.h"
#include "HTFormat.h"

#ifdef SHORT_NAMES
#define HTClientHost            HTClHost
#define HTOutputStream          HTOuStre
#define HTOutputFormat          HTOuForm
#endif

/*      Return codes from load routines:
**
**      These codes may be returned by the protocol modules,
**      and by the HTLoad routines.
**      In general, positive codes are OK and negative ones are bad.
*/

#define HT_NO_DATA -9999        /* return code: OK but no data was loaded */
                                /* Typically, other app started or forked */


typedef struct _HTProtocol {
        char * name;
        
        int (*load)PARAMS((
                CONST char *    full_address,
                HTParentAnchor * anchor,
                HTFormat        format_out,
                HTStream*       sink));
                
        HTStream* (*saveStream)PARAMS((HTParentAnchor * anchor));

} HTProtocol;

/*

Flags which may be set to control this module

 */
extern int HTDiag;                      /* Flag: load source as plain text */
extern char * HTClientHost;             /* Name or number of telnetting host */
extern FILE * logfile;                  /* File to output one-liners to */
extern HTStream* HTOutputStream;        /* For non-interactive, set this */
extern HTFormat HTOutputFormat;         /* To convert on load, set this */



#ifdef __cplusplus
extern "C"{
#endif
/*

Load a document from relative name

  ON ENTRY,
  
  relative_name           The relative address of the file to be accessed.
                         
  here                    The anchor of the object being searched
                         
  ON EXIT,
  
  returns    YES          Success in opening file
                         
  NO                      Failure
                         
 */
HTBoolean HTLoadRelative PARAMS((
                CONST char *            relative_name,
                HTParentAnchor *        here));


/*

Load a document from absolute name

  ON ENTRY,
  
  addr                    The absolute address of the document to be accessed.
                         
  filter                  if YES, treat document as HTML
                         
 */

/*

  ON EXIT,
  
 */

/*

  returns YES             Success in opening document
                         
  NO                      Failure
                         
 */
int HTLoadAbsolute PARAMS((CONST char * addr));


/*

Load a document from absolute name to a stream

  ON ENTRY,
  
  addr                    The absolute address of the document to be accessed.
                         
  filter                  if YES, treat document as HTML
                         
  ON EXIT,
  
  returns YES             Success in opening document
                         
  NO                      Failure
                         
   Note: This is equivalent to HTLoadDocument
   
 */
HTBoolean HTLoadToStream PARAMS((CONST char * addr, HTBoolean filter,
                                HTStream * sink));


/*

Make a stream for Saving object back

  ON ENTRY,
  
  anchor                  is valid anchor which has previously beeing loaded
                         
  ON EXIT,
  
  returns                 0 if error else a stream to save the object to.
                         
 */


HTStream * HTSaveStream PARAMS((HTParentAnchor * anchor));


/*

Register an access method

 */

HTBoolean HTRegisterProtocol PARAMS((HTProtocol * protocol));
#ifdef __cplusplus
}
#endif

#endif /* HTACCESS_H */
