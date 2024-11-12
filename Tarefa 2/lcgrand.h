#ifndef LCGRAND_H
#define LCGRAND_H

/*! \file lcgrand.h
 *  \brief Header file for the random number generator functions.
 *
 *  This file defines constants and declares functions for generating 
 *  uniformly distributed random numbers. 
 *  It includes the main function lcgrand and the associated function 
 *  lcgrandst and lcgrandft for seed management.
 *  This file (named lcgrand.h) should be included in any program
 *  using these functions by executing #include "lcgrand.h"
 */

/*! Define the constants. */
#define MODLUS 2147483647  /*!< Modulus value for the LCG algorithm. */
#define MULT1 24112        /*!< Multiplier value for the LCG algorithm. */
#define MULT2 26143        /*!< Multiplier 2 for the LCG algorithm. */

/*!
 *  @brief Generate the next U(0, 1) random number
 *
 * To obtain the next U(0,1) random number from stream "stream" execute
 * u = lcgrand(stream);
 *
 * @param stream index for the random stream to be used 
 * @return U(0,1) random number from stream "stream" 
 */
float lcgrand(int stream);

// as seguintes não vamos utilizar
// só vamos utilizar a de cima - lcgrand - que retorna um valor aleatório entre 0 e 1 uniformemente distribuído
// devolve 1 ou +, mas a sua média dará 0.5

/*!
 * @brief Set the seed for the random number stream.
 *
 * @param zset The new seed value to set for the stream.
 * @param stream Index of the random number stream to set the seed for.
 */
void lcgrandst(long zset, int stream);

/*! 
 * @brief Get the current seed for the random number stream.
 *
 * This function returns the current seed value for the random number 
 * generator stream specified by `stream`.
 *
 * @param stream Index of the random number stream.
 * @return The current seed (zrng) for the specified stream.
 * */
long lcgrandgt(int stream);

#endif