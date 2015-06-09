#ifndef RN_GEN_H
#define RN_GEN_H

#include <unistd.h>
#include <stdlib.h>  
#include <limits.h>
#define my_srandom  srand
#define my_random   rand
#define SIZE_T             sizeof(size_t)
class RandomNumGen
{
   public:
	  //RandomNumGen() { my_srandom(getpid()); }
	  RandomNumGen(unsigned seed) { my_srandom(seed); }
	  const size_t operator() (const size_t range) const {
		if(SIZE_T == 4)	
			return my_random()%range;
		else{
			size_t temp1 = my_random(),temp2=my_random(),temp3=my_random()%2;
			return ((temp1<<33)^(temp2<<2)^temp3)%range;
		}
	  }
};
#endif // RN_GEN_H