#include "random.h"

#ifdef _WIN32

void resetRandom() {
	srand( time( 0 ) + clock() );
}

#else

void resetRandom() {
	srand48( time( 0 ) + clock() );
}

#endif
