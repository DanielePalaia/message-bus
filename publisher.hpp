#ifndef PUBLISHER_H
#define PUBLISHER_H

#include "event.hpp"
#include "eventbus.hpp"
#include "nthread.hpp"

extern EventBus bus;

class Publisher  {

public:

	Publisher(typethread type)  {

		bus.bindListener(this, principal);
		this->type = type;

	}

	template <typename Event>
	void Publish(const Event& event)  {

		if (type == synch)
			bus.sendSync<Event>(event);
		if (type == principal)
			bus.sendAsync<Event>(event);
		else  {
			NThread *t;
			vectorSync<std::shared_ptr<BaseListener>> *v = bus.getThreadQueue<Event>(this, t);

		}

	}

	typethread getType() { return type; }



private:
	typethread type;


};

#endif
