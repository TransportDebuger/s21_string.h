#include "s21_string.h"
#include <stdlib.h>

int s21_strlen(const char *str) {
    char *p = str;
    int len = 0;
    while (*p != '\0') {
        p++;
        len++;
    }
    return len;
}

int s21_strcmp(const char *source1; const char *source2) {
    int i = 0, result = 1;
    while (i < s21_strlen(source1) && i < s21_strlen(source2)) {
        if (source1[i] != source2[i]) {
            result = 0;
            break;
        }
        i++;
    }
    return result;
}

char* s21_strcpy(char* destination; const char* source) {
    const int n = s21_strlen(source);
    char *result = malloc((n + 1) * sizeof(char));

    while (int i = 0; i < n; i++) {
        result[i] = source[i];
    }
    free(destination);
    return result;
}

char *s21_strcat(char* destination; const char* source) {
    int n = s21_strlen(destination) + s21_strlen(source);
    char *result = malloc((n + 1) * sizeof(char));
    int i = 0;
    for (; i < s21_strlen(destination); i++) {
        result[i] = destination[i];
    }
    for (; i < s21_strlen(source); i++) {
        result[i] = source[i];
    }
    free(destination);
    return result;
}

char *s21_strchr(const char *str, int ch) {
    char *result = NULL;

    for (int i = 0; i < s21_strlen(str); i++) {
        if (str[i] == ch) {
            result = str + i;
            break;
        }
    }
    return result;
}

int compare(const char *i_str, const char *s_str) {
    int result = 0;
    for (int i = 0; i < s21_strlen(s_str); i++) {
        if (s_str[i] == i_str[i]) result = *s_str;
        else {
            result = 0;
            break;
        }
    }
    return result;
}

char *s21_strstr(const char *inv_str, const char *s_str) {
    char *result = NULL;
    while (*inv_str != '\0') {
        if (*inv_str == *s_str && compare(inv_str, s_str)) {

        }
        inv_str++;
    }
    return result;
}

