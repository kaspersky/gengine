game: game.o
	g++ game.o -shared -o libgame.so

game.o: game.cpp
	g++ -std=c++11 -Wall -O3 game.cpp -c -I. -fPIC

clean:
	rm -rf *.o *.so
