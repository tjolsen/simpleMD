#ifndef _PARTICLE_HPP
#define _PARTICLE_HPP

#include <iostream>
#include <stdio.h>
#include <cmath>

template<unsigned NSD>
class Particle {
public:
  unsigned id;
  unsigned nCollisions;
  double x[NSD];
  double v[NSD];
  double R;
  Particle(): nCollisions(0){}
  bool collides(const Particle<NSD> &P)const {
    double d=0.0;
    for(unsigned i=0; i<NSD; ++i)
      d += (x[i] - P.x[i])*(x[i] - P.x[i]);

    return (sqrt(d) < (R+P.R));
  }
  
  void print() const {
    fprintf(stdout, "Particle %d\n\tR = %f\n\tx = [",id,R);
    for(unsigned i=0; i<NSD; ++i)
      fprintf(stdout, "%f,", x[i]);
    fprintf(stdout,"]\n\tv = [");
    for(unsigned i=0; i<NSD; ++i)
      fprintf(stdout, "%f,", v[i]);
    fprintf(stdout,"]\n\n");
  }
  
  double collisionTime(const Particle & P) const {
    //negative returned time is to be interpreted as no collision    
    
    //compute quadratic formula coefficients (done on whiteboard, should prob document...)
    double a=0, b=0, c=0;
    for(unsigned i=0; i<NSD; ++i) {
      a += (v[i] - P.v[i])*(v[i] - P.v[i]);
      b += 2 * (v[i]-P.v[i])*(x[i]-P.x[i]);
      c += (x[i] - P.x[i])*(x[i] - P.x[i]);
    }
    c -= (R+P.R)*(R+P.R);
    
    double t1, t2;
    //check parallel motion
    double PARTOL = 1e-10;
    
    if(a/R < PARTOL) {
      return -1; 
    }
    
    if(b >= 0) { //particles moving away from each other
      return -1;
    }
    
    double quadform_sqrt = b*b - 4*a*c;
    if(quadform_sqrt < 0) {
      return -1;
    }
    
    t1 = (-b + sqrt(quadform_sqrt))/(2*a);
    t2 = (-b - sqrt(quadform_sqrt))/(2*a);
    
    if(t1>0 && t2>0) {
      return (t1<t2)?t1:t2;
    }
    else if(t1>0 && t2<0) {
      return t1;
    }
    else if(t1<0 && t2>0) {
      return t2;
    }
    else {
      return -1;
    }
    
  }
};



#endif
