shell : obj/shell.o obj/log.o obj/check.o obj/convert.o obj/execute.o obj/get.o 
	gcc $(CFLAGS) obj/* -o shell

obj/shell.o : src/shell.c
	gcc $(CFLAGS) -c src/shell.c -o obj/shell.o

obj/log.o : src/log.c
	gcc $(CFLAGS) -c src/log.c -o obj/log.o

obj/check.o: src/check.c
	gcc $(CFLAGS) -c src/check.c -o obj/check.o

obj/get.o: src/get.c
	gcc $(CFLAGS) -c src/get.c -o obj/get.o

obj/convert.o : src/convert.c
	gcc $(CFLAGS) -c src/convert.c -o obj/convert.o

obj/execute.o : src/execute.c
	gcc $(CFLAGS) -c src/execute.c -o obj/execute.o

clean:
	rm -f obj/*.o

install :
	cp shell /usr/bin/

