

main.ex: main.o utilities.o
	g++ main.o utilities.o -o main.ex

main.o: main.cpp
	g++ -c main.cpp -g -Wall -std=c++11

utilities.o: utilities.cpp
	g++ -c utilities.cpp -g -Wall -std=c++11

clean:
	rm *.o *.ex
