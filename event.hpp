/* Simple event class the events are simply mocked */

#ifndef EVENT_H
#define EVENT_H

#include <iostream>
#include <atomic>

// Two simple mocked events
struct EventA {
  int x;
  explicit EventA(int x) : x(x) {}
};

struct EventB {
  std::string s;
  explicit EventB(std::string s) : s(s) {}
};


#endif
