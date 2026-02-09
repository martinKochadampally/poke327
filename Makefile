poke327: queue.o map.o poke327.o
	gcc queue.o map.o poke327.o -o poke327

#-c makes this stop after compiling returning the object file (.o files).
queue.o: queue.c queue.h
	gcc -Wall -Werror -g queue.c -c

map.o: map.c map.h
	gcc -Wall -Werror -g map.c -c

poke327.o: poke327.c queue.h map.h
	gcc -Wall -Werror -g poke327.c -c

clean:
	rm -f *.o poke327 *~