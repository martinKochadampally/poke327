poke327: queue.o heap.o map.o poke.o pokemon.o
	g++ queue.o map.o heap.o poke.o pokemon.o -o poke -lncurses

# -c makes this stop after compiling returning the object file (.o files).
queue.o: queue.cpp queue.h
	g++ -Wall -Werror -g queue.cpp -c

map.o: map.cpp map.h heap.h
	g++ -Wall -Werror -g map.cpp -c

heap.o: heap.cpp heap.h
	g++ -Wall -Werror -g heap.cpp -c

pokemon.o: pokemon.cpp pokemon.h
	g++ -Wall -Werror -g pokemon.cpp -c

poke.o: poke.cpp queue.h map.h pokemon.h
	g++ -Wall -Werror -g poke.cpp -c

clean:
	rm -f *.o poke *~