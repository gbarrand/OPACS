/*  */

/*      Displaying messages and getting input for WWW Library
**      =====================================================
**
**         May 92 Created By C.T. Barker
**         Feb 93 Portablized etc TBL
*/

#include "HTUtils.h"

#ifdef __cplusplus
extern "C"{
#endif
/*      Display a message and get the input
**
**      On entry,
**              Msg is the message.
**
**      On exit,
**              Return value is malloc'd string which must be freed.
*/
char * HTPrompt PARAMS((CONST char * Msg, CONST char * deflt));
char * HTPromptPassword PARAMS((CONST char * Msg));


/*      Display a message, don't wait for input
**
**      On entry,
**              The input is a list of parameters for printf.
*/
void HTAlert PARAMS((CONST char * Msg));


/*      Display a progress message for information (and diagnostics) only
**
**      On entry,
**              The input is a list of parameters for printf.
*/
void HTProgress PARAMS((CONST char * Msg));
int HTCheckActiveIcon PARAMS((int twirl));


void HTClearActiveIcon NOPARAMS;

/*      Display a message, then wait for 'yes' or 'no'.
**
**      On entry,
**              Takes a list of parameters for printf.
**
**      On exit,
**              If the user enters 'YES', returns TRUE, returns FALSE
**              otherwise.
*/
HTBoolean HTConfirm PARAMS((CONST char * Msg));

#ifdef __cplusplus
}
#endif



