ROOT_LIBS  = $(shell root-config --libs)
LIBRARIES  := $(LIBRARIES) -I. -Iutil -lTree -lHist $(ROOT_LIBS)
INCLUDES := $(INCLUDES) $(shell root-config --cflags)
CXXFLAGS := $(CXXFLAGS) -Werror -pedantic -std=c++0x

all: ggst

ggst: FORCE
	$(CXX) -o ggst ggst.cpp $(CXXFLAGS) $(LDFLAGS) $(INCLUDES) $(LIBRARIES)

clean: FORCE
	$(RM) *.o *~ core
	$(RM) ggst

FORCE:

# DO NOT DELETE
