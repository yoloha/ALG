AR       := ar
ARFLAGS  := cr
CXX      := g++
#CPPFLAGS := -O2 -I../lib
CPPFLAGS := -g -Wall -I../lib

all : ../bin/DPT_balance_color

../bin/DPT_balance_color : main.o boundingBox.o boundingBox_private.o colorBalancing.o optimize.o ../lib/tm_usage.o linear_solver.o timer.o
	$(CXX) $^ -o $@

main.o: main.cpp ../lib/tm_usage.h timer.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

boundingBox.o : boundingBox.cpp colorBalancing.h timer.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

boundingBox_private.o : boundingBox_private.cpp colorBalancing.h timer.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

colorBalancing.o : colorBalancing.cpp colorBalancing.h timer.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

optimize.o : optimize.cpp colorBalancing.h ../lib/rnGen.h timer.h
	$(CXX) $(CPPFLAGS) -c $< -o $@
linear_solver.o : linear_solver.cpp timer.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

timer.o: timer.cpp timer.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

clean :
	-rm *.o
