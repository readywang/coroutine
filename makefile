all:
	g++  coroutine.cpp  -g -c
	g++  main.cpp -g -o main coroutine.o
	g++  coroutine1.cpp  -g -c
	g++  main1.cpp -g -o main1 coroutine1.o
clean:
	rm -f coroutine.o main coroutine1.o main1