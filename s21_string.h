#ifndef S21_STRING_H
#define S21_STRING_H

//macroses
#define NULL ((void*)0)

//type definitions
typedef unsigned long size_t;



size_t s21_strlen(const char *str);
int s21_strcmp(const char *source1, const char *source2);
char* s21_strcpy(char* destination, const char* source);
char *s21_strcat(char* destination, const char* source);
char *s21_strchr(const char *str, int ch);
char *s21_strstr(const char *inv_str, const char *s_str);

#endif