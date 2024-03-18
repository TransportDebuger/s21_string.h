#include "s21_string.h"

void *s21_to_upper(const char *str) {
  char *str_new = S21_NULL;
  if (str) {
    str_new = (char *)malloc((s21_strlen(str) + 1) * sizeof(char));
    for (s21_size_t i = 0; i <= s21_strlen(str); i++) {
      if (str[i] < 'a' || str[i] > 'z') {
        str_new[i] = str[i];
      } else {
        str_new[i] = str[i] - 32;
      }
    }
  }

  return (void *)str_new;
}