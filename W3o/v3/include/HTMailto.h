#ifndef HTMAILTO_H
#define HTMAILTO_H

typedef void(*HTGetMailtoKludgeInfoFunction)(char**,char**);

#ifdef __cplusplus
extern "C"{
#endif
void HTSetGetMailtoKludgeInfoProc (HTGetMailtoKludgeInfoFunction);
#ifdef __cplusplus
}
#endif

#endif /* not HTMAILTO_H */
