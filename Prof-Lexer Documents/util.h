#include <assert.h>

#ifndef UTILH
#define UTILH

typedef char *string;
//typedef char bool;

#define TRUE 1
#define FALSE 0

void *checked_malloc(int);
string String(char *);


#endif // UTILH
