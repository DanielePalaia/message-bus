#ifndef LISTENER_H
#define LISTENER_H

#include<mutex>
#include<condition_variable>
#include<thread>
#include<vector>
#include"event.hpp"
#include"global.hpp"
#include"nthread.hpp"
#include<memory>

class Foo: public BaseListener {

	public:

    		void operator()(const EventA& a) {std::cout << "Foo::onA() " << a.x << std::endl;};
    		void operator()(const EventB& b) {std::cout << "Foo::onB()" << b.s << std::endl;};
    		void RunTestAsyn();
    		void RunTestSyn();

};

class FooB: public BaseListener {

	public:

    		void operator()(const EventA& a) {std::cout << "Foo::onA() " << a.x << std::endl;};
    		void operator()(const EventB& b) {std::cout << "Foo::onB()" << b.s << std::endl;};
    		void RunTestAsyn();
    		void RunTestSyn();
};

#endif
