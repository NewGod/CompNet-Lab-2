OBJDIR := obj 
LIB := lib
CFLAGS := $(CFLAGS) $(DEFS) $(LABDEFS) -O1 -fno-builtin -I$(LIB) -MD
CFLAGS += -Wall -Wno-format -Wno-unused -Werror -m32 --std=c++11
device.o: 
	g++ -c $(CFLAGS) lib/device.cpp -o obj/device.o
safeQueue: 
	g++ test/safequeuetest.cpp -o test/safequeuetest -I"lib/" --std=c++11
clean: 
	rm test/safequeuetest
