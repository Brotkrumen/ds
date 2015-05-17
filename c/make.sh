gcc -g -Wall -Wextra -pedantic-errors -std=c99 main.c -o main 
valgrind --leak-check=yes ./main input1.data
