#ifndef _FRAMERENDERER_HPP
#define _FRAMERENDERER_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <string>



class FrameRenderer {

public:
  std::ifstream infile; 
 double timestep;
  double Tfinal;
  std::string imageBasename;
  std::vector< std::vector<double> > points;
  std::vector<double> radii;

  FrameRenderer();
  FrameRenderer(const char *f, double dt, double Tmax);
  void setImageBasename();
  void Render(int nprocs);
  
};

void render_range(const FrameRenderer *FR, int f1, int f2);

#endif
