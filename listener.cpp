#include <iostream>
#include "listener.hpp"
#include "event.hpp"
#include "eventbus.hpp"
#include "publisher.hpp"

extern EventBus bus;

// Tests scenarios runned as threads
void Foo::RunTestAsyn()  {
		std::cout << "FooRunningFoo" << std::endl;
		Publisher pub(principal);
		std::cout << "after FooRunningFoo" << std::endl;
      	EventA *event = new EventA(5);
    	EventA *event2 = new EventA(10);
      	//const EventB eventb("Hello world");
      	/*Send a subscribe to the bus*/
      	bus.subscribe<EventA>(this);
      	pub.Publish<EventA>(*event);
      	pub.Publish<EventA>(*event2);

      	std::cout<<"notified"<<std::endl;
}

void Foo::RunTestSyn()  {
		std::cout << "FooRunningFoo" << std::endl;
		Publisher pub(synch);
		std::cout << "after FooRunningFoo" << std::endl;
      	EventA *event = new EventA(5);
    	EventA *event2 = new EventA(10);
      	//const EventB eventb("Hello world");
      	/*Send a subscribe to the bus*/
      	bus.subscribe<EventA>(this);
      	pub.Publish<EventA>(*event);
      	pub.Publish<EventA>(*event2);

      	std::cout<<"notified"<<std::endl;
}


void FooB::RunTestSyn()  {
	std::cout << "FooRunningFooB" << std::endl;
  	EventA *event = new EventA(5);
	EventA *event2 = new EventA(10);
	EventB *event2b = new EventB("Hello world!!");
	Publisher pub(synch);
	std::cout << "after FooRunningFoo" << std::endl;
  	/*Send a subscribe to the bus*/
	bus.subscribe<EventA>(this);
  	bus.subscribe<EventB>(this);
  	pub.Publish<EventA>(*event);
  	pub.Publish<EventB>(*event2b);

  	std::cout<<"notified"<<std::endl;

}

void FooB::RunTestAsyn()  {
	std::cout << "FooRunningFooB" << std::endl;
  	EventA *event = new EventA(5);
	EventA *event2 = new EventA(10);
	EventB *event2b = new EventB("Hello world!!");
	Publisher pub(principal);
	std::cout << "after FooRunningFoo" << std::endl;
  	/*Send a subscribe to the bus*/
	bus.subscribe<EventA>(this);
  	bus.subscribe<EventB>(this);
  	pub.Publish<EventA>(*event);
  	pub.Publish<EventB>(*event2b);

  	std::cout<<"notified"<<std::endl;

}





