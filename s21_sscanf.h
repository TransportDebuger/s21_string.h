#ifndef s21_STRING_H
#define s21_STRING_H

#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "s21_string.h"

typedef struct {
  int minus;
  int plus;
  int width;
  int percision;
  int space;
  int flagSize;
  int l;
  int h;
  int L;
  int tag;
  int zero;
  int isHex;
} flags_t;

/*SSCANF*/

int s21_sscanf(const char *str, const char *format, ...);

#endif