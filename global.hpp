#ifndef GLOBAL_H
#define GLOBAL_H

#include<iostream>
#include<mutex>

/* GENERIC MODULES */
struct BaseListener {
   BaseListener *baseListener;
   BaseListener() {}
   BaseListener(BaseListener *listener)
   :baseListener(listener)
   {baseListener = listener;}
   virtual ~BaseListener() {}
   virtual void operator()(const void*) {};
 };

/* Element of the queue a base listener and an event to activate the notification channel */
struct QThread {
		BaseListener *listener;
		void *event;
		std::mutex *mutexRM;
};

#endif
