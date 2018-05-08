mcts: mcts.o mcts2.o
	g++ mcts.o mcts2.o -shared -o libmcts.so

mcts.o: mcts.cpp
	g++ -std=c++11 -Wall -O3 mcts.cpp -c -I. -I../game -fPIC

mcts2.o: mcts2.cpp
	g++ -std=c++11 -Wall -O3 mcts2.cpp -c -I. -I../game -fPIC

clean:
	rm -rf *.o *.so
