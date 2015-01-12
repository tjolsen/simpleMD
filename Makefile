cppfiles = $(shell ls *.cpp)
ofiles = $(cppfiles:.cpp=.o)
exefiles = $(cppfiles:.cpp=.exe)

CFLAGS = -O3 -c -Wall -I/home/tyler/src/timg/src/
LFLAGS = -L/home/tyler/src/timg/lib/ -ltimg -lm -lpng -ljpeg -ltiff
CPP = g++

all: depend $(exefiles)


depend: .depend

.depend: $(cppfiles)
	rm -f ./.depend
	$(CPP) $(CFLAGS) -MM $^ -MF ./.depend

include .depend

%.exe: %.o
	$(CPP) $< $(LFLAGS) -o $@

%.o: %.cpp
	$(CPP) $< $(CFLAGS)

clean:
	rm $(ofiles) $(exefiles) .depend *.png

.PHONY: depend clean all