#include "FrameRenderer.hpp"
#include <iostream>
#include <thread>

int main(int argc, char **argv) {
  
  if(argc < 2) {
    std::cout << "Supply smd file name\n";
    return 1;
  }

  FrameRenderer FR(argv[1],0.01,10);
  FR.Render(std::thread::hardware_concurrency());

  return 0;
}
