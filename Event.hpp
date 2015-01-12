#ifndef _EVENT_HPP
#define _EVENT_HPP

#include <iostream>
#include <stdio.h>

class Event {
public:
  double time;
  int p1;
  int p2;
  unsigned nColl1, nColl2;
  inline bool operator<(const Event &rhs) {return this->time < rhs.time;}
  
  void print() {fprintf(stdout,"Event(t=%g, p1=%d, p2=%d)\n", time, p1, p2);}
  //simple constructor. initialize to values that will never be accidentally interpreted at runtime
  Event():time(0), p1(-1), p2(-1), nColl1(0), nColl2(0) {};
};


std::ostream & operator<<(std::ostream & out, const Event &e) {
  out << "Event(t="<<e.time<<", p1="
      <<e.p1<<", p2="<<e.p2
      <<", nColl1="<<e.nColl1
      <<", nColl2="<<e.nColl2<<")";
  return out;
}

#endif
