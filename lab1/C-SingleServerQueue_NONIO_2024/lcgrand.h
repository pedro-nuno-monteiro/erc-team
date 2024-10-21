#ifndef LCGRAND_H
#define LCGRAND_H

/*! Define the constants. */
#define MODLUS 2147483647
#define MULT1 24112
#define MULT2 26143


/*! The following 3 declarations are for use of the random-number generator
lcgrand and the associated functions lcgrandst and lcgrandgt for seed
management. This file (named lcgrand.h) should be included in any program
using these functions by executing
#include "lcgrand.h"
before referencing the functions. */


/*!To obtain the next U(0,1) random number from stream "stream," execute
   u = lcgrand(stream);
   @param stream index for the random stream to be used 
   @return U(0,1) random number from stream "stream," 
*/
float lcgrand(int stream);

// as seguintes não vamos utilizar
// só vamos utilizar a de cima - lcgrand - que retorna um valor aleatório entre 0 e 1 uniformemente distribuído
// devolve 1 ou +, mas a sua média dará 0.5

/*! Set the current zrng for stream "stream" to zset.
	@param zset value of stream "stream"
	@param stream index for the random strem to be set 
*/
void lcgrandst(long zset, int stream);

/*! Return the current zrng for stream "stream". 
	@param stream index for the random strem to be set 
 * */
long lcgrandgt(int stream);

#endif
