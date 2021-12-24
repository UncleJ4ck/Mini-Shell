shell : obj/shell.o obj/main.o
	gcc obj/* -o shell

obj/shell.o : src/shell.c
	gcc -c src/shell.c -o obj/shell.o

obj/main.o : src/main.c
	gcc -c src/main.c -o obj/main.o
