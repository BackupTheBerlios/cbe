#include "time.h"
#include "stdlib.h"

#ifdef _WIN32

void resetRandom();

double inline rnd() {
	return rand()/( RAND_MAX+1.0 );
}

#else

void resetRandom();

double inline rnd() {
	return drand48();
}

#endif

int inline rndInt( int a ) {
	return (int)(rnd()*a);
}
