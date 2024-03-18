#include "s21_string.h"
void* s21_trim(const char* src, const char* trim_chars) {
  char* rez = S21_NULL;
  if (src) {
    if (!trim_chars) trim_chars = " ";

    char* ptr_start_src = (char*)src;
    char* ptr_end_src = (char*)src + s21_strlen(src);

    for (; *ptr_start_src && s21_strchr(trim_chars, *ptr_start_src);
         ptr_start_src++)
      ;
    for (; ptr_end_src != ptr_start_src &&
           s21_strchr(trim_chars, *(ptr_end_src - 1));
         ptr_end_src--)
      ;

    rez = (char*)calloc(ptr_end_src - ptr_start_src + 1, sizeof(char));
    if (rez) {
      s21_strncpy(rez, ptr_start_src, ptr_end_src - ptr_start_src);
      *(rez + (ptr_end_src - ptr_start_src)) = '\0';
    }
  }
  return (void*)rez;
}
