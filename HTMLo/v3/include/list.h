/*
 * Copyright (C) 1992, Board of Trustees of the University of Illinois.
 *
 * Permission is granted to copy and distribute source with out fee.
 * Commercialization of this product requires prior licensing
 * from the National Center for Supercomputing Applications of the
 * University of Illinois.  Commercialization includes the integration of this 
 * code in part or whole into a product for resale.  Free distribution of 
 * unmodified source and use of NCSA software is not considered 
 * commercialization.
 *
 */


typedef struct LISTSTRUCT  *List;

/*
 * Public functions. Must be clean so that production of libHTMLo.exp with omake works.
 */
extern List ListCreate            ();
extern void ListDestroy           (List);
extern int  ListAddEntry          (List,char*);
extern int  ListDeleteEntry       (List,char*);
extern int  ListMakeEntryCurrent  (List,char*);
extern int  ListCount             (List);

extern char* ListHead             (List);
extern char* ListTail             (List);
extern char* ListCurrent          (List);
extern char* ListNext             (List);
extern char* ListPrev             (List);
extern char* ListGetIndexedEntry  (List,int);
