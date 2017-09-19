/* class eventbus it offers the primitive requested */

#ifndef EVENTBUS_H
#define EVENTBUS_H

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <cassert>
#include <thread>
#include "nthread.hpp"
#include "listener.hpp"
#include "vectorsync.hpp"

enum typethread {synch, principal, aux, aux2, ded};
class Publisher;

namespace detail {
static size_t nextTypeId = 0;
}

template <typename T>
size_t GetTypeId() {
  static const size_t id = detail::nextTypeId++;
  return id;
}

//
class EventBus {

 public:

	//Destroy all the threads created: principal, aux and dedicated
	~EventBus()  {
		//delete principal thread
		if(mainThread)
			delete mainThread;
		//delete auxiliary thread
		if(auxiliaryThread)
			delete auxiliaryThread;
		//delete second auxiliary thread
		if(auxiliaryThread2)
			delete auxiliaryThread2;
		//delete dedicated threads
		for (auto thread: dedThreads) {
			if(thread)
				delete thread;
		}


	}

  //Subscribe a listener which overloaded the () operator for every event
  template <typename Event, class Listener>
  void subscribe(Listener* listener) {
    struct ListenerImpl : BaseListener {
      Listener* listener_;

      ListenerImpl(Listener* listener) : BaseListener(listener), listener_(listener) {}

      void operator()(const void* event) override {
        const auto e = static_cast<const Event*>(event);
        (*listener_)(*e);
      }
    };
    auto& listeners = listeners_map_[GetTypeId<Event>()];
    // Protect the access of the map
    std::unique_lock<std::mutex> uLock(*(listeners.getLock()));
    listeners.getVector().push_back(std::make_shared<ListenerImpl>(listener));
    uLock.unlock();
  }

  //Subscribe with a specific method
  template <typename Event, class Listener>
  void subscribe(Listener* listener, void (Listener::*method)(const Event&)) {
    struct ListenerImpl : BaseListener {
      typedef void (Listener::*Method)(const Event&);
      Listener* listener_;
      Method method_;

      ListenerImpl(Listener* listener, Method method) : BaseListener(listener), listener_(listener), method_(method) {}

      void operator()(const void* event) override {
        const auto e = static_cast<const Event*>(event);
        (listener_->*method_)(*e);
      }

      void *operator()() {
    	 return listener_;
      }

    };
    auto& listeners = listeners_map_[GetTypeId<Event>()];
    // Protect the access of the map
    std::lock_guard<std::mutex> uLock(*(listeners.getLock()));
    listeners.getVector().push_back(std::make_shared<ListenerImpl>(listener, method));
  }

  /* Publish in an event and returns for testing purpose the number of modules notified*/
    template <typename Event>
    int sendSync(const Event& event) {
  	  auto id = GetTypeId<Event>();
  	  auto it = listeners_map_.find(id);
  	  // Protects the access of the map
  	  std::lock_guard<std::mutex> uLock(*(it->second.getLock()));
  	  for (auto listener : it->second.getVector()) {
  		  (*listener)(&event);
  	  }
    }

    /* Publish in an event and returns for testing purpose the number of modules notified*/
    template <typename Event>
    int sendAsync(const Event& event) {
  	int notified = 0;
      auto id = GetTypeId<Event>();
      // Protect the access of the map
      auto it = listeners_map_.find(id);
      std::lock_guard<std::mutex> uLock(*(it->second.getLock()));
      for (auto listener : it->second.getVector()) {
      	  std::cout << "im sending:" << std::endl;
      	  busMainThread.Push(&(*listener), (void *)&event, it->second.getLock());
      	  notified++;
        }

      return notified;
    }

    /* Unsubscribe a listener */
  template <typename Event, class Listener>
    void unsubscribe(Listener* listener) {
  		std::cout<<"unsubscribing:"<<std::endl;
  		auto& listeners = listeners_map_[GetTypeId<Event>()];
  		std::lock_guard<std::mutex> uLock(*(listeners.getLock()));
  	    for(auto it = listeners.getVector().begin(); it != listeners.getVector().end(); it++)  {
  	    	if ((*(*it)).baseListener == listener)  {
  	      			std::cout<<"Erasing"<<std::endl;
  	      			listeners.getVector().erase(it);
  	    	}
  	    }

    }

    /* Check if a publisher is present in an event */
   template <typename Event, class Listener>
   bool checkListenerPresent(Listener* listener) {
	   auto& listeners = listeners_map_[GetTypeId<Event>()];
    	std::lock_guard<std::mutex> uLock(*(listeners.getLock()));
    	for(auto it = listeners.getVector().begin(); it != listeners.getVector().end(); it++)  {
    		if ((*(*it)).baseListener == listener)  {
    			return true;
    	    }
    	}
    	      	return false;
  }

    /* Returns the listeners map */
    std::unordered_map<size_t, vectorSync<std::shared_ptr<BaseListener>>> getListenersMap()  { return listeners_map_; }


  /*Threads management*/
  /*Bind a publisher to a thread*/
  void bindListener(Publisher *publisher, typethread type)    {
	 std::lock_guard<std::mutex> lg(mutexBindingMap);
	 switch (type) {
	 //The listener is associated to the main thread
	 case principal:
		 threadBindingMap[publisher] = &busMainThread;
		 break;
	//To the first auxiliary thread
	 case aux:
		 threadBindingMap[publisher] = &busAuxiliaryThread;
		 break;
	 //To the second auxiliary thread
	 case aux2:
		 threadBindingMap[publisher] = &busAuxiliaryThread2;
		 break;
	 //else create a dedicate thread for the listener
	 default:
		 NThread *ded = createDedicatedThreads();
		 threadBindingMap[publisher] = ded;

	 }

  }

  /*Returns the thread management structure binded to the publisher*/
  template <typename Event>
  vectorSync<std::shared_ptr<BaseListener>> *getThreadQueue(Publisher *publisher, NThread *t)     {
	  std::lock_guard<std::mutex> lg(mutexBindingMap);
	  t = threadBindingMap[publisher];
	  auto id = GetTypeId<Event>();

	  auto it = listeners_map_.find(id);
	 return &it->second;
  }

  /* Run the threads */
  static void StartThread(NThread *busThread)  {
	  busThread->Run();
  }

/* Create the main thread and the auxiliary threads */
void createThreads() {
	mainThread = new std::thread(StartThread, &(this->busMainThread));
	mainThread->detach();
	auxiliaryThread = new std::thread(StartThread, &(this->busAuxiliaryThread));
	auxiliaryThread->detach();
	auxiliaryThread2 = new std::thread(StartThread, &(this->busAuxiliaryThread2));
	auxiliaryThread2->detach();
}

/* Create dedicated threads on demand */
NThread *createDedicatedThreads() {

	NThread *ded = new NThread();
	busDedicatedThreads.push_back(ded);
	std::thread *dedThread  = new std::thread(StartThread, ded);
	dedThreads.push_back(dedThread);
	return ded;

}

/* Join the threads */
void joinThreads()   {
	mainThread->join();
	auxiliaryThread->join();
	auxiliaryThread2->join();
}

//Not thread safe reset the registration mapping of the bus
void Reset()  {

	listeners_map_.clear();

}

 private:

  /* Main and auxiliary threads */
  std::thread *mainThread=NULL;
  std::thread *auxiliaryThread=NULL;
  std::thread *auxiliaryThread2=NULL;
  // vector of dedicated threads
  std::vector<std::thread*> dedThreads;
  // Vector managment structure
  NThread busMainThread;
  NThread busAuxiliaryThread;
  NThread busAuxiliaryThread2;
  std::vector<NThread*> busDedicatedThreads;
  std::unordered_map<size_t, vectorSync<std::shared_ptr<BaseListener>>> listeners_map_;
  std::unordered_map<Publisher*, NThread *> threadBindingMap;
  std::mutex mutexBindingMap;
};

#endif
