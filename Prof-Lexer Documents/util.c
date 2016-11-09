/*
 * util.c - commonly used utility functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

#ifndef UTILC
#define UTILC

void *checked_malloc(int len) {
  void *p = malloc(len);
  if (!p) {
     fprintf(stderr,"\nRan out of memory!\n");
     exit(1);
  }
  return p;
}

string String(char *s) {
  string p = checked_malloc(strlen(s)+1);
  strcpy(p,s);
  return p;
}

#endif // UTILC
