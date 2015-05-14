CXXFLAGS=-g
all: alg
alg:main.o colorBalancing.o 
	g++ -o DPT_balance_color main.cpp colorBalancing.o 	
.PHONY: clean
clean:
	rm *.o
