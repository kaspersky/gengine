mcts: mcts.o mcts2.o game.o
	g++ mcts.o mcts2.o game.o -shared -o libmcts.so

mcts.o: mcts.cpp
	g++ -std=c++11 -Wall -O3 mcts.cpp -c -I. -fPIC

mcts2.o: mcts2.cpp
	g++ -std=c++11 -Wall -O3 mcts2.cpp -c -I. -fPIC

game.o: game.cpp
	g++ -std=c++11 -Wall -O3 game.cpp -c -I. -fPIC

clean:
	rm -rf *.o *.so
