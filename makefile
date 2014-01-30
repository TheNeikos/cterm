task build:
	gcc -Wall -std=c99 -o main -ggdb -lxcb -lm main.c 
task debug:
	gcc -Wall -std=c99 -o main_debug -g -O0 -lxcb -lm main.c 