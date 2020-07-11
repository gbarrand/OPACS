#ifdef __cplusplus
extern "C"{
#endif
void HTSortInit (void);
void HTSortAdd (char *str);
void HTSortSort (void);
int HTSortCurrentCount (void);
char *HTSortFetch (int i);
#ifdef __cplusplus
}
#endif
