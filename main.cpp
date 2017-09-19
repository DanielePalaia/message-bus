#include <iostream>
#include <string>
#include <cassert>
#include <thread>
#include "eventbus.hpp"

/* A simple test with main (similar to tests.cpp */
extern EventBus bus;

/* Module Foo thread */
void StartFoo()  {
	Foo foo;
	foo.RunTestAsyn();
}

/* Module FooB thread */
void StartFooB()  {
	FooB foob;
	foob.RunTestAsyn();
}

int main(int argc, char* argv[]) {

	bus.createThreads();

	/* Run the threads and let the process wait for the bus */
	std::thread t2(StartFoo);
	std::thread t3(StartFooB);
	bus.joinThreads();

	return 0;
}
