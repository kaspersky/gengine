manager: manager.o
	g++ manager.o -shared -o libmanager.so

manager.o: manager.cpp
	g++ -std=c++11 -Wall -O3 manager.cpp -c -I. -I../game -fPIC

clean:
	rm -rf *.o *.so
