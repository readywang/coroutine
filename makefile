all:
	g++  coroutine.cpp  -g -c
	g++  main.cpp -g -o main coroutine.o
clean:
	rm -f coroutine.o main