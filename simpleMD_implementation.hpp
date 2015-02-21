#ifndef _SIMPLEMD_CPP
#define _SIMPLEMD_CPP

#include <cstdlib>
#include <ctime>

template<unsigned NSD>
simpleMD<NSD>::simpleMD() {
  particles.clear();
  srand48(clock());
  setup();
}



template<unsigned NSD>
void simpleMD<NSD>::setup() {
  
  nsd = NSD;
  L = 100;
  nParticles = 20000;
  vo = 10;
  R = 0.1;
  coeff_rest = 1.0;
  Tmax = 10;
  
  //open data file
  datafile.open("output.smd");

  //setup walls  
  for(int i=0; i<NSD; ++i) {
    walls[i][0] = 0.0;
    walls[i][1] = L;
  }
  
  //create particles
  unsigned pnum=0;
  int maxtries = 1000;
  int trynum = 0;
  while(pnum < nParticles) {
    Particle<NSD> P;
    P.id = pnum;
    P.R = R;
    P.nCollisions = 0;
    for(unsigned i=0; i<NSD; ++i) {
      P.x[i] = walls[i][0] + P.R + (walls[i][1]-walls[i][0]-2*P.R)*drand48();
      P.v[i] = 2*vo*(drand48()-0.5);
    }
    bool collision = false;
    for(unsigned i=0; i<particles.size(); ++i) {
      if(P.collides(particles[i])) {
	collision = true;
	break;
      }
    }
    if(!collision) {
      particles.push_back(P);
      pnum++;
      trynum = 0;
    }
    if(trynum++ > maxtries) {
      std::cout << "Could not place particle. Consider using a different placement method\n";
      exit(1);
    }
  }

  //write data file initial data
  datafile << nParticles << " " << NSD << " " << L << std::endl;
  for(unsigned p=0; p<nParticles; ++p) {
    Particle<NSD> P = particles[p];
    for(unsigned i=0; i<NSD; ++i) {
      datafile << P.x[i] << " ";
    }
    for(unsigned i=0; i<NSD; ++i) {
      datafile << P.v[i] << " ";
    }
    datafile << P.R << std::endl;
  }
}

template<unsigned NSD>
void simpleMD<NSD>::run() {
  double time = 0;

  for(unsigned p=0; p<nParticles; ++p) {
    enqueueEvents(p, time);
  }
  
  //main loop
  while(time < Tmax) {


    //std::cout << "Time = " << time << std::endl;
    //EventHeap.print();
    //std::cout << std::endl;
    //get next valid event
    Event e;
    bool good1, good2, good3;
    do {
      good1=good2=good3=false;
      e = EventHeap.extract();
      /*
      std::cout << "\t" << e;
      if(e.p1>=0)
	std::cout << "  nc1="<<particles[e.p1].nCollisions;
      if(e.p2>=0)
	std::cout << "  nc1="<<particles[e.p2].nCollisions;
      std::cout << std::endl;
      */
      good1 = (e.p1<0) ? true : (e.nColl1 == particles[e.p1].nCollisions);
      good2 = (e.p2<0) ? true : (e.nColl2 == particles[e.p2].nCollisions);
      good3 = (e.p1>=0 || e.p2>=0);
      //std::cout << "\t\t"<<good1 << " " << good2 << " " << good3 << std::endl;
    }while(!(good1 && good2 && good3));

    //std::cout << "e = " << e << std::endl <<std::endl;

    double dt = e.time - time;
    
    //update particle positions: O(N)
    time = e.time;
    for(unsigned p=0; p<nParticles; ++p) {
      for(unsigned i=0; i<NSD; ++i) {
	particles[p].x[i] += particles[p].v[i]*dt;
      }
    }

    //handle collision event: O(1)
    handleCollisionEvent(e);

    //get new events for e.p1 and e.p2
    Event E1;
    if(e.p1 >= 0) {
      enqueueEvents(e.p1, time);
    }
    if(e.p2 >= 0) {
      enqueueEvents(e.p2, time);
    }
    
  }//end while(t<Tmax)
  
}

template<unsigned NSD>
void simpleMD<NSD>::enqueueEvents(int id,double time) {

  Event e;

  //particle collision
  Particle<NSD> P = particles[id];
  for(unsigned p=0; p<nParticles; ++p) {
    if(id == p) continue;
    
    double dt = P.collisionTime(particles[p]);
    if(dt > 0) {
      e.time = time+dt;
      e.p1 = id;
      e.p2 = p;
      e.nColl1 = P.nCollisions;
      e.nColl2 = particles[p].nCollisions;
      EventHeap.insert(e);
    }
  }
  //wall collision
  for(unsigned i=0; i<NSD; ++i) {
    int wallid;
    double dt;
    if(P.v[i]<0) {
      dt = (walls[i][0]-P.x[i]+P.R)/P.v[i];
      wallid = -(2*i + 1);
    }
    else {
      dt = (walls[i][1]-P.x[i]-P.R)/P.v[i];
      wallid = -(2*i + 2);
    }
    e.time = time+dt;
    e.p1 = id;
    e.p2 = wallid;
    e.nColl1 = P.nCollisions;
    e.nColl2 = 0;
    EventHeap.insert(e);
  }

}

template<unsigned NSD>
Event simpleMD<NSD>::calculateMinEvent(int id, double time) {
  
  double minDT=-1;
  int minID=id;
  Particle<NSD> P = particles[id];

  //check particle collision times
  for(int i=0; i<nParticles; ++i) {
    if(i==id)
      continue;
    
    double dt = P.collisionTime(particles[i]);
    if(dt > 0) {
      if(minDT > 0) {
	minDT = (minDT<dt) ? minDT : dt;
	minID = (minDT<dt) ? minID : i;
      }
      else {
	minDT = dt;
	minID = i;
      }
    }
  }
  
  //check wall collision times
  for(unsigned i=0; i<NSD; ++i) {
    int wallid;
    double dt;
    if(P.v[i] < 0) {
      dt = (walls[i][0]-P.x[i]+P.R)/P.v[i];
      wallid = -(2*i + 1);
    }
    else {
      dt = (walls[i][1]-P.x[i]-P.R)/P.v[i];
      wallid = -(2*i + 2);
    }
    
    if(dt > 0) {
      if(minDT > 0) {
	minDT = (minDT<dt) ? minDT : dt;
	minID = (minDT<dt) ? minID : wallid;
      }
      else {
	minDT = dt;
	minID = wallid;
      }
    }
  }
  
  //pack and return
  Event e;
  e.time = time+minDT;
  e.p1 = id;
  e.p2 = minID;
  e.nColl1 = particles[id].nCollisions;
  e.nColl2 = (minID<0)? 0 : particles[minID].nCollisions;
  return e;
}

template<unsigned NSD>
void simpleMD<NSD>::handleCollisionEvent(Event e) {
  
  //increment nCollision values for particles
  if(e.p1 >= 0)
    particles[e.p1].nCollisions++;
  if(e.p2 >= 0)
    particles[e.p2].nCollisions++;

  int id1 = (e.p1 > e.p2) ? e.p1 : e.p2; //max id, never a wall
  int id2 = (e.p1 > e.p2) ? e.p2 : e.p1; //min id, sometimes a wall

  //handle wall collision
  if(id2 < 0) {
    int dim = (-(id2+1))/2;
    particles[id1].v[dim] *= -coeff_rest;

    //log wall collision
    datafile << "$Event "<<1<<" "<<e.time<<std::endl;
    datafile << id1 << " ";
    for(unsigned i=0; i<NSD-1; ++i) {
      datafile << particles[id1].v[i] << " ";
    }
    datafile << particles[id1].v[NSD-1] << std::endl;;
    
    return;
  }
  
  //handle particle collision (REFERENCE VARIABLES!!!!)
  Particle<NSD> & P1 = particles[id1];
  Particle<NSD> & P2 = particles[id2];

  double n[NSD]; //unit normal vector joining particle centers
  for(unsigned i=0; i<NSD; ++i) {
    n[i] = (P2.x[i]-P1.x[i])/(P1.R + P2.R);
  }
  
  double u1n=0, u2n=0;
  for(unsigned i=0; i<NSD; ++i) {
    u1n += P1.v[i]*n[i];
    u2n += P2.v[i]*n[i];
  }
  
  //handle masses properly (or at least proportionally)
  double m1=1, m2=1;
  for(unsigned i=0; i<NSD; ++i) {
    m1 *= P1.R;
    m2 *= P2.R;
  }
  double v2n = (m1*u1n + m2*u2n - m1*coeff_rest*(u2n-u1n))/(m1+m2);
  double v1n = (v2n + coeff_rest*(u2n-u1n));
  
  //update velocities (updated in array since these are reference vars)
  for(unsigned i=0; i<NSD; ++i) {
    P1.v[i] += (v1n-u1n)*n[i];
    P2.v[i] += (v2n-u2n)*n[i];
  }

  //log event in output
  datafile << "$Event "<<2<<" "<<e.time<<std::endl;
  datafile << id1 << " ";
  for(unsigned i=0; i<NSD-1; ++i) {
    datafile << particles[id1].v[i] << " ";
  }
  datafile << particles[id1].v[NSD-1] << std::endl;;
  datafile << id2 << " ";
  for(unsigned i=0; i<NSD-1; ++i) {
    datafile << particles[id2].v[i] << " ";
  }
  datafile << particles[id2].v[NSD-1] << std::endl;;
}

/*
template<unsigned NSD>
void simpleMD<NSD>::draw(const std::string &fname) {
  
  if(NSD < 2)
    exit(1);
  
  int npix = 1000;
  pixel_t white = {.r=255, .g=255, .b=255, .a=255};
  pixel_t black = {.r=0, .g=0, .b=0, .a=255};
  timg_t *img = timg_createfromval(npix, npix, white);
  
  
  double scale = npix/L;
  for(int p=0; p<particles.size(); ++p) {
    
    int r,c;
    r = npix - (int)(particles[p].x[1]*scale);
    c = (int)(particles[p].x[0]*scale);
    int rad = (int)(particles[p].R * scale);
    
    
    black.r = (unsigned char)(double(p+1)*255/nParticles);
    timg_drawCircle(img, r, c, rad, black);
    
  }
  timg_writepng(fname.c_str(), img);  
  timg_destroy(img);
}
*/

#endif
