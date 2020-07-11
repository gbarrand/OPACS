/*Contains parts of the original tcp.h.*/

#ifndef HTNET_H
#define HTNET_H

#ifdef VMS
#include <unixio.h>
#elif defined(NT)
#else  /*UNIX*/
#include <unistd.h>
#endif  /* UNIX */

/* Default values of those: */
#define NETCLOSE close      /* Routine to close a TCP-IP socket         */
#define NETREAD  HTDoRead   /* Routine to read from a TCP-IP socket     */
#define NETWRITE write      /* Routine to write to a TCP-IP socket      */

#ifdef __cplusplus
extern "C"{
#endif
int HTDoRead (int,void*,unsigned);   
#ifdef __cplusplus
}
#endif

#endif /* HTNET_H */



