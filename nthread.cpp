/* This class manage a thread takes the information that arrive from a queue */
#include "nthread.hpp"

class BaseListener;

/*Run as a thread*/
void NThread::Run()  {

	/*Loop catching requests from the module*/
	while(1)  {
		std::cout << "im running" << std::endl;
		std::unique_lock<std::mutex> ulCV(mutexCV);
		/* Noone still filled the queue wait for it */
		if (notificationQueue.size() == 0) cv.wait(ulCV);
		else ulCV.unlock();
		/*Take the element for the queue */
    	std::unique_lock<std::mutex> lockCond(mutexNotificationQueue);
    	QThread req = notificationQueue.front();
    	notificationQueue.pop();
    	lockCond.unlock();
    	BaseListener *listener = req.listener;
    	void *event = req.event;
    	std::lock_guard<std::mutex> lg(*req.mutexRM);
    	if(listener)
    		(*listener)(event);
    }

}

void NThread::Push(BaseListener *listener, void* event, std::mutex *mutexRM)  {
	QThread elem;
    elem.listener = listener;
    elem.event = event;
    elem.mutexRM = mutexRM;
    std::cout << "Pushing" << std::endl;

    std::unique_lock<std::mutex> notQueueLock(mutexNotificationQueue);
    notificationQueue.push(elem);
    notQueueLock.unlock();
    /* notify the bus that the queue is not empty anymore */
    std::lock_guard<std::mutex> lgCV(mutexCV);
    std::cout << "notifying" << std::endl;
    cv.notify_one();
    std::cout << "Notified size: ;" << notificationQueue.size() << std::endl;
}
