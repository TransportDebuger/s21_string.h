#include "test.h"
// #include "test_me.h"

int run_tests(Suite *test_case) {
  int number_failed;
  SRunner *sr = srunner_create(test_case);
  srunner_set_fork_status(sr, CK_NOFORK);
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return number_failed;
}

int main() {
  int number_failed = 0;

  Suite *suites_list[] = {s21_string_suite(), sprintf_suite(),
                          s21_sscanf_suite(), NULL};
  for (Suite **current = suites_list; *current != NULL; current++) {
    printf("_________________________________________\n");
    number_failed += run_tests(*current);
  }

  // char str1[SIZE] = {'\0'};
  // char str2[SIZE] = {'\0'};
  // char *format = "%.0Le";
  // long double val = 15.35;
  // s21_sprintf(str1, format, val);
  // printf("%s длина =%ld\n", str1, s21_strlen(str1));
  // sprintf(str2, format, val);
  // printf("%s длина =%ld\n", str2, s21_strlen(str2));

  //  char str1[SIZE] = {'\0'};
  //   char str2[SIZE] = {'\0'};

  //   char format[] = "%0.*i %d %4.*i %013d %d";
  //   int val = 69;

  //       s21_sprintf(str1, format, 5, val, -10431, 13, 5311, 0, -581813581);
  //       printf("%s\n",str1);
  //       sprintf(str2, format, 5, val, -10431, 13, 5311, 0, -581813581);
  //       printf("%s\n",str2);

  return 0;
}