# build an executable named myprog from myprog.c
CC = g++

all:
	$(CC) -std=c++11 -o bus main.cpp global.cpp listener.cpp nthread.cpp -lpthread

test: 
	$(CC) -std=c++11 -shared -fPIC -o libbus.so main.cpp global.cpp listener.cpp nthread.cpp -lpthread 
	$(CC) -std=c++11 -o tests Main_TestAll.cpp tests.cpp -lpthread -lgtest -lbus

clean:
	rm bus
