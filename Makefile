main: main.o uttt.o
	g++ main.o uttt.o -o main -lmcts -L./moca

main.o: main.cpp
	g++ -std=c++11 -Wall -O3 main.cpp -c -I. -I./moca

uttt.o: uttt.cpp
	g++ -std=c++11 -Wall -O3 uttt.cpp -c -I. -I./moca

clean:
	rm -rf *.o main
