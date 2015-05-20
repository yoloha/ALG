AR       := ar
ARFLAGS  := cr
CXX      := g++
CPPFLAGS := -O2 -I../lib

all : ../bin/DPT_balance_color

../bin/DPT_balance_color : main.o boundingBox.o boundingBox_private.o colorBalancing.o
	$(CXX) $^ -o $@

main.o: main.cpp boundingBox.o boundingBox_private.o colorBalancing.o
	$(CXX) $(CPPFLAGS) -c $< -o $@

boundingBox.o : boundingBox.cpp colorBalancing.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

boundingBox_private.o : boundingBox_private.cpp colorBalancing.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

colorBalancing.o : colorBalancing.cpp colorBalancing.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

clean :
	rm *.o

