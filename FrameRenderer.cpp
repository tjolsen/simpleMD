#include "FrameRenderer.hpp"
#include <thread>

FrameRenderer::FrameRenderer() : FrameRenderer("output.smd", 0.1, 10) {}

FrameRenderer::FrameRenderer(const char *f, double dt, double Tmax) :
  infile(f), timestep(dt), Tfinal(Tmax), imageBasename("img_")
{}

void FrameRenderer::Render(int nprocs) {
  
  //sanity check, min threads = 1
  if(nprocs < 1) {
    nprocs = 1;
  }
  
  int Nframe_end = (int)(Tfinal/timestep);
  
  int framesPerProc = (Nframe_end + 1)/nprocs;
  std::vector<std::thread> t;
  for(int i=0; i<nprocs; ++i) {
    int f1 = i*framesPerProc;
    int f2 = (i==nprocs-1)?Nframe_end : (i+1)*framesPerProc;

    //render_range(this, f1, f2);
    t.push_back(std::thread(render_range, this, f1, f2));
  }
  
  for(int i=0; i<nprocs; ++i) {
    t[i].join();
  }

  
  return;
}


void render_range(const FrameRenderer *FR, int f1, int f2) {

  std::vector<std::vector<double>> points_loc = FR->points;
  double timestep = FR->timestep;
  std::string imageBasename = FR->imageBasename;
  
  char fname[128];
  
  for(int f=f1; f<f2; ++f) {
    
    
  }
  
  return;
}
