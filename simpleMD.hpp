#ifndef _SIMPLEMD_HPP
#define _SIMPLEMD_HPP

#include "heap.hpp"
#include "Particle.hpp"
#include "Event.hpp"
#include <string>
#include <vector>
#include <fstream>

template<unsigned NSD>
class simpleMD {

private:
  unsigned nsd;
  double walls[NSD][2];
  unsigned nParticles;
  double L; // box length
  double vo; // characteristic particle velocity
  double R; //particle radius
  double coeff_rest;
  double Tmax; // simulation ending time
  Heap<Event> EventHeap;
  std::ofstream datafile;
  std::vector< Particle<NSD> > particles;
  void setup();
  void enqueueEvents(int id, double time);
  Event calculateMinEvent(int id, double time);
  void handleCollisionEvent(Event e);

public:
  simpleMD();
  ~simpleMD() {datafile.close();}
  void run();
  void draw(const std::string &fname);
};

#include "simpleMD_implementation.hpp"

#endif
