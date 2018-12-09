IDIR = ./lib

CXXFLAGS = -I $(IDIR) -Wall -std=c++11 -lpcap -lpthread
CXXFLAGS += -O1


SCRIPT_DIR=./script
SCRIPTS = $(wildcard $(SCRIPT_DIR)/*.cpp)

TARGETS = $(patsubst %.cpp, %, $(SCRIPTS))

OBJ = ./obj/device.o ./obj/route.o ./obj/arp.o ./obj/ip.o
all: $(OBJ) $(TARGETS) 

debug: CXXFLAGS += -DDEBUG -g
debug: $(OBJ) $(TARGETS)

$(TARGETS): %: %.cpp $(OBJ)
	$(CXX) -o $@ $(OBJ) $< $(CXXFLAGS)

$(OBJ): obj/%.o: lib/%.cpp 
	$(CXX) -o $@ $< $(CXXFLAGS) -c

clean:
	rm $(OBJ) $(TARGETS)
