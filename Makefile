shell : obj/shell.o obj/main.o
	gcc $(CFLAGS) obj/* -o shell

obj/shell.o : src/shell.c
	gcc $(CFLAGS) -c src/shell.c -o obj/shell.o

obj/main.o : src/main.c
	gcc $(CFLAGS) -c src/main.c -o obj/main.o

clean:
	rm -f obj/*.o
