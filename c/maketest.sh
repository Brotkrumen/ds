gcc -g -Wall -Wextra -pedantic-errors -std=c99 test.c -o test
valgrind --leak-check=yes ./test
