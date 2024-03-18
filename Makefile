SHELL := /bin/bash
OS=$(shell uname)
FLAG = -Wall -Wextra -Werror -std=c11
ifeq ($(OS), Darwin)
 CHECK= -lcheck -lpthread -lm
else
 CHECK= -lcheck -lpthread -lm -lsubunit
endif


all: clean s21_string.a test

s21_string.a: clean
	gcc $(FLAG) -c *.c
	ar rcs s21_string.a *.o
	ranlib s21_string.a

test: s21_string.a
	gcc -g *.c ./Test/*.c $(CHECK) -o test_exe
	./test_exe
	

gcov_report: s21_string.a
	gcc -g --coverage *.c ./Test/*.c $(CHECK) -o test_exe
	./test_exe
	lcov -t "test" -o test.info -c -d .
	genhtml -o report test.info
	rm -rf *.gcda *.gcno *.out test_exe

leaks: clean test
	valgrind --log-file="log_file_valgrind.txt" --trace-children=yes --track-fds=yes --track-origins=yes --leak-check=full --show-leak-kinds=all -s ./test_exe
clean:
	rm -rf *.o *.a *.gcda *.txt *.gcno *.out *.info report test_exe