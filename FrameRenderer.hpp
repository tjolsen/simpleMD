#ifndef _FRAMERENDERER_HPP
#define _FRAMERENDERER_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <string>



class FrameRenderer {

public:
  struct event {
    double et;
    unsigned np;
    std::vector<int> pid;
    std::vector<std::vector<double>> newV;
  };
  
  std::ifstream infile; 
  double timestep;
  double Tfinal;
  double L;
  std::string imageBasename;
  std::vector< std::vector<double> > points;
  std::vector< std::vector<double> > velocity;
  std::vector<double> radii;
  std::vector<event> Events;
    
  FrameRenderer();
  FrameRenderer(const char *f, double dt, double Tmax);
  void setImageBasename();
  void Render(int nprocs);
  
};

void render_range(const FrameRenderer *FR, int f1, int f2, double L);

#endif
