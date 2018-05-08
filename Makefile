main: main.o uttt.o
	g++ main.o uttt.o -o main -lmcts -lgame -L./mcts -L./game

main.o: main.cpp
	g++ -std=c++11 -Wall -O3 main.cpp -c -I. -I./game -I./mcts

uttt.o: uttt.cpp
	g++ -std=c++11 -Wall -O3 uttt.cpp -c -I. -I./game -I./mcts

clean:
	rm -rf *.o main
