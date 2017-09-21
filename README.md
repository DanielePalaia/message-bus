# message-bus
A generic multithreading message bus written in C++11

Still instable and not 100% complete, but a good starting point.

This is an implementation of a generic multithreading message-bus using the publish/subscribe deisgn pattern written using the new C++11 threading primitives and tested with google test and google mock.

We can imagine a set of modules (listeners) which want to communicate in an event-driven fashion. A module can subscribe itself to a topic and modules can dispatch events to a topic so that the ones subscribed will be notified.

An application which needs this functionality can use the eventbus.hpp and publish.hpp primitive (publish, subscribe, unsubscribe) and use the message bus functionality.

Everything is completely generic using design C++ template so, a module subscribe using:

template <typename Event, class Listener>
  void subscribe(Listener* listener)
  
if the class Listener is overloading the operator () with the a specify Event or it can use:

template <typename Event, class Listener>
  void subscribe(Listener* listener, void (Listener::*method)(const Event&)) 
  
To specify a method to be called in case of publishing to that Event.

A module can then publish an event with the send (publish) function:
  
template <typename Event>
  int sendSync(const Event& event)
    
where the typename Event is the topic and the Event in input is the event to publish (the method registerd would be called with the type Event.
 
 Or using threads can use, in this case the call will be asynchronous dispatched to a thread
 
 template <typename Event>
  int sendAsync(const Event& event)
  
 See listeners.cpp for a mocking module example.
