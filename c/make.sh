gcc -g -Wall -Wextra -pedantic-errors -std=c99 main.c -o main 
valgrind --leak-check=yes ./main /home/stepo/workspace/ds/testdata/input3.data
valgrind --tool=cachegrind ./main /home/stepo/workspace/ds/testdata/input3.data
