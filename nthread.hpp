#ifndef NTHREAD_H
#define NTHREAD_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include "global.hpp"

class NThread  {
	private:
			std::queue<QThread> notificationQueue;
			std::condition_variable cv;
			std::mutex mutexNotificationQueue;
			std::mutex mutexCV;

  	public:
    		NThread()
    		{}
    		// Push a request in the queue
    		void Push(BaseListener *listener, void* event, std::mutex *mutexRM);
    		// Run as a thread
    		void Run();

};

#endif
