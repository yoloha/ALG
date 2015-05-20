AR       := ar
ARFLAGS  := cr
CXX      := g++
CPPFLAGS := -O2

all : tm_usage.o
	@echo -n ""

clean :
	rm *.o

tm_usage.o : tm_usage.cpp tm_usage.h
	$(CXX) $(CPPFLAGS) -c $< -o $@


