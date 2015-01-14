CFLAGS = -O3 -c -Wall -I/home/tyler/src/timg/src/ -std=c++11
LFLAGS = -L/home/tyler/src/timg/lib/ -ltimg -lm -lpng -ljpeg -ltiff -pthread
CPP = g++

all: .simpleMD_depend .basic_postProcess_depend simpleMD_driver basic_postProcess

.simpleMD_depend: simpleMD_driver.cpp
	rm -f .simpleMD_depend
	$(CPP) -MM $^ -MF .simpleMD_depend

.basic_postProcess_depend: basic_postProcess.cpp FrameRenderer.cpp
	rm -f .basic_postProcess_depend .tmp
	for f in $^; do $(CPP) $(CFLAGS) -MM $$f -MF .tmp; cat .tmp >> .basic_postProcess_depend; done

include .simpleMD_depend
include .basic_postProcess_depend

simpleMD_driver: simpleMD_driver.o
	$(CPP) $< $(LFLAGS) -o $@

basic_postProcess: basic_postProcess.o FrameRenderer.o
	$(CPP) $^ $(LFLAGS) -o $@	

%.o: %.cpp
	$(CPP) $< $(CFLAGS)



clean:
	rm *.o simpleMD_driver basic_postProcess .simpleMD_depend .basic_postProcess_depend *.png .tmp

.PHONY: clean all