#include <limits.h>
#include "gtest/gtest.h"
#include "eventbus.hpp"
#include "event.hpp"
#include <memory>

class BusTest : public ::testing::Test {
	protected:
		virtual void SetUp() {}
	  	virtual void TearDown() {}
};

extern EventBus bus;

/* Module Foo thread */
void StartFooAsyn()  {
	Foo foo;
	foo.RunTestAsyn();
}

/* Module FooB thread */
void StartFooBAsyn()  {
	FooB foob;
	foob.RunTestAsyn();
}

void StartFooASyn()  {
	Foo foo;
	foo.RunTestSyn();
}

/* Module FooB thread */
void StartFooBSyn()  {
	FooB foob;
	foob.RunTestSyn();
}

/* Declare a bus and a listener and let the listener do a subscribe
 * Check that the map has at least 1 element and that the listener is found in the map*/
TEST(BusTest, subscribe)
{

	EventBus bus;
	Foo foo;

	bus.subscribe<EventA>(&foo);

	auto map = bus.getListenersMap();
	auto mapEvent = map[GetTypeId<EventA>()];

	/* The map should now contain 1 element */
	EXPECT_EQ(1, mapEvent.getVector().size());

}

/*Do a couple of subscription and one unsubscription and checks for correctess */
TEST(BusTest, unsubscribe)
{

	EventBus bus;
	Foo foo;
	FooB foo2;
	bool success = false;

	bus.subscribe<EventA>(&foo);
	bus.subscribe<EventA>(&foo2);
	bus.unsubscribe<EventA>(&foo);

	//Check the element not to be present
	bool found = bus.checkListenerPresent<EventA>(&foo);
	bool found2 = bus.checkListenerPresent<EventA>(&foo2);

	//Instead foo2 has to be present
	auto map = bus.getListenersMap();
	auto mapEvent = map[GetTypeId<EventA>()];

	if(found == false && found2) success = true;

	/* The map should now contain 1 element */
 	EXPECT_EQ(1, mapEvent.getVector().size());
	EXPECT_EQ(success, true);

}

/* Test the publish in async mode, do some subscribe do a send async and test the map */
TEST(BusTest, sendAsync)
{

	EventBus bus;
	Foo foo;
	FooB foob;


	const EventA event(5);
	bool success = false;
	bool found = false;

	bus.subscribe<EventA>(&foo);
	bus.subscribe<EventA>(&foob);
	int notified = bus.sendAsync<EventA>(event);

	/* The map should now contain 1 element */
	EXPECT_EQ(2, notified);

}

/* Test the publish in sync mode, do some subscribe do a send async and test the map */
TEST(BusTest, sendSync)
{

	EventBus bus;
	Foo foo;
	FooB foob;


	const EventA event(5);
	bool success = false;
	bool found = false;

	bus.subscribe<EventA>(&foo);
	bus.subscribe<EventA>(&foob);
	int notified = bus.sendAsync<EventA>(event);

	/* The map should now contain 1 element */
	EXPECT_EQ(2, notified);

}

/* Test multithreading async*/
TEST(BusTest, multithreadingAsync)
{

	/* Run the threads and let the process wait for the bus */
	bus.createThreads();
	std::thread t2(StartFooAsyn);
	std::thread t3(StartFooBAsyn);
	t2.detach();
	t3.detach();
	// Wait for thread-scenarios to be completed
	sleep(1);
	auto map = bus.getListenersMap();
	auto mapEvent = map[GetTypeId<EventA>()];
	auto mapEvent2 = map[GetTypeId<EventB>()];

	/* The map should now contain 4 elements (to be coherent with scenario runned) */
	EXPECT_EQ(3, mapEvent.getVector().size() + mapEvent2.getVector().size());
	/* Terminate both the process and all threads */

}

/* Test multithreading sync*/
TEST(BusTest, multithreadingSync)
{

	bus.Reset();
	/* Run the threads and let the process wait for the bus */
	bus.createThreads();
	std::thread t2(StartFooASyn);
	std::thread t3(StartFooBSyn);
	t2.detach();
	t3.detach();
	// Wait for thread-scenarios to be completed
	sleep(1);
	auto map = bus.getListenersMap();
	auto mapEvent = map[GetTypeId<EventA>()];
	auto mapEvent2 = map[GetTypeId<EventB>()];

	/* The map should now contain 4 elements (to be coherent with scenario runned) */
	EXPECT_EQ(3, mapEvent.getVector().size() + mapEvent2.getVector().size());
	/* Terminate both the process and all threads */

}
