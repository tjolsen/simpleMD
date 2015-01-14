#include "FrameRenderer.hpp"
#include "timg.h"
#include "timg_drawing.h"
#include <thread>
#include <iostream>
#include <cstdlib>

FrameRenderer::FrameRenderer() : FrameRenderer("output.smd", 0.1, 10) {}

FrameRenderer::FrameRenderer(const char *smdFileName, double dt, double Tmax) :
  infile(smdFileName), timestep(dt), Tfinal(Tmax), imageBasename("img_")
{

  if(!infile.good()) {
    std::cout << "Could not open " << smdFileName << std::endl;
    exit(1);
  }

  unsigned nparticles, nsd;
  double r;
  infile >> nparticles >> nsd >> L;
  for(unsigned i=0; i<nparticles; ++i) {
    std::vector<double> p(nsd,0), v(nsd,0);
    for(unsigned d=0; d<nsd; ++d) {
      infile >> p[d];
    }
    for(unsigned d=0; d<nsd; ++d) {
      infile >> v[d];
    }
    infile >> r;
    
    points.push_back(p);
    velocity.push_back(v);
    radii.push_back(r);
  }

  std::string s;
  while(!infile.eof()) {
    
    infile >> s;
    if(infile.eof())
      break;

    event e;
    
    infile >> e.np >> e.et;

    int p;
    std::vector<double> v(nsd,0);
    for(unsigned i=0; i<e.np; ++i) {
      infile >> p;
      for(unsigned d=0; d<nsd; ++d) {
	infile >> v[d];
      }
      e.pid.push_back(p);
      e.newV.push_back(v);
    }
    
    Events.push_back(e);
  }

  std::cout << "Processed " << Events.size() << " events\n";
}

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
    t.push_back(std::thread(render_range, this, f1, f2, L));
  }
  
  for(int i=0; i<nprocs; ++i) {
    t[i].join();
  }

  
  return;
}



void render_range(const FrameRenderer *FR, int f1, int f2, double L) {

  std::vector<std::vector<double>> points_loc = FR->points;
  std::vector<std::vector<double>> velocity_loc = FR->velocity;
  double timestep = FR->timestep;
  std::string imageBasename = FR->imageBasename;
  
  char fname[128];
  
  unsigned event_index = 0;
  double time = 0;
  for(int f=f1; f<f2; ++f) {

    double tFrame = timestep*f;

    while(event_index<FR->Events.size() &&
	  tFrame > FR->Events[event_index].et) {
      
      double dt = FR->Events[event_index].et-time;
      
      //update particle positions for new time
      for(unsigned p=0; p<points_loc.size(); ++p) {
	for(unsigned d=0; d<points_loc[p].size(); ++d) {
	  points_loc[p][d] += dt*velocity_loc[p][d];
	}
      }

      //update particle velocities based on event
      for(unsigned i=0; i<FR->Events[event_index].np; ++i) {
	for(unsigned d=0; d<FR->Events[event_index].newV[i].size(); ++d) {
	  velocity_loc[FR->Events[event_index].pid[i]][d] = FR->Events[event_index].newV[i][d];
	}
      }
      
      //prepare for next loop iteration
      event_index++;
      time += dt;
    }

    //advance time up to frame time
    double dt = tFrame - time;
    for(unsigned p=0; p<points_loc.size(); ++p) {
      for(unsigned d=0; d<points_loc[p].size(); ++d) {
	points_loc[p][d] += dt*velocity_loc[p][d];
      }
    }
    time += dt;
    
    //all events handled up to current frame. Time to draw...
   int npix = 1000;
    pixel_t white = {.r=255, .g=255, .b=255, .a=255};
    pixel_t black = {.r=0, .g=0, .b=0, .a=255};
    timg_t *img = timg_createfromval(npix, npix, white);
    
    
    double scale = npix/L;
    for(unsigned p=0; p<points_loc.size(); ++p) {
      
      int r,c;
      r = npix - (int)(points_loc[p][1]*scale);
      c = (int)(points_loc[p][0]*scale);
      int rad = (int)(FR->radii[p] * scale);
      
      
      black.r = (unsigned char)(double(p+1)*255/points_loc.size());
      timg_drawCircle(img, r, c, rad, black);
      
    }
    
    sprintf(fname, "%s%05d.png", FR->imageBasename.c_str(), f);
    timg_writepng(fname, img);  
    timg_destroy(img);
    
  }
  
  return;
}
