CC = g++
LD = $(CC)
CFLAGS = -c -Wall -g #-Os
IFLAGS =  -I./ -I$(BDXRECO_ROOT)/src/libraries -I$(BDXRECO_ROOT)/src/external/jana_0.7.7p1/src
ROOTINC := $(shell root-config --cflags)
ROOTLIB := $(shell root-config --glibs)
LIBS = -L$(BDXRECO_ROOT)/lib -lbdxReco -lJANA -lbdxRecoExt -lRooFit -lRooFitCore -lMinuit -lMinuit2 -lProof -let
LFLAGS = -Wl,-rpath $(BDXRECO_ROOT)/lib


SRC = $(wildcard *.cc)

TARGET = ana

OBJECTS = $(patsubst %.cc, %.o, $(wildcard *.cc))

all:   dict dict2 $(TARGET)

dict: BDXDSTSelector.h
	@echo "Generating dictionary $@..."
	@rootcint -v -f BDXDSTSelector_Dict.cc -c -p $(IFLAGS) BDXDSTSelector.h	BDXDSTSelector_LinkDef.h

dict2: BDXDSTSelector2.h
	@echo "Generating dictionary $@..."
	@rootcint -v -f BDXDSTSelector2_Dict.cc -c -p $(IFLAGS) BDXDSTSelector2.h BDXDSTSelector2_LinkDef.h	

$(TARGET): $(OBJECTS)
	$(LD) -shared -fPIC -o libBDXDSTSelector.so $(LFLAGS) $(ROOTLIB) $(LIBS) $^  
	$(LD) -o $@ $^ $(LFLAGS) $(ROOTLIB) $(LIBS)

%.o: %.cc
	@echo $(OPTIONS)
	$(CC) -fPIC -g $(CFLAGS) $(IFLAGS) $(ROOTINC) $(OPTIONS) $^ -o $@

clean:
	rm $(TARGET) $(OBJECTS)
	rm libBDXDSTSelector.so
