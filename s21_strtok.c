#include "s21_string.h"

char* s21_strtok(char* str, const char* delim) {
  static char* last;
  register int ch;

  if (str == 0) str = last;

  do {
    if ((ch = *str++) == '\0') return 0;
  } while (s21_strchr(delim, ch));

  --str;
  last = str + s21_strcspn(str, delim);
  if (*last != 0) {
    *last++ = 0;
  }
  return str;
}