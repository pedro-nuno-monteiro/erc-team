#ifndef FILA1S_H
#define FILA1S_H

/*! Code from 
 * Simulation and Modelling 5e
 * Averill M. Law B
*/

/*! Stream (1.100) to be defined by input and different for each variable */
/*! External definitions for single-server queueing system. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lcgrand.h"

 /*! Header file for random-number generator. */
#define Q_LIMIT 100
 /*! Limit on queue length. */
#define BUSY 1
 /*! Mnemonics for server's being busy */
#define IDLE 0
 /*! and idle. */

/*! Global variabled defined in simula_fila1s.c */
// variáveis definidas noutro sítio (por isso é que têm extern int)
extern int next_event_type, num_custs_delayed, num_delays_required, num_events,
num_in_q, server_status;
extern float area_num_in_q, area_server_status, mean_interarrival, mean_service,
sim_time, time_arrival[Q_LIMIT + 1], time_last_event, time_next_event[3],
total_of_delays;
extern FILE *infile, *outfile;

/*!
 * Initialization function.
 * \note arguments to be added
 * */
void initialize(void);

/*! Timing function. */
void timing(void);

/*! Arrival event function. */
void arrive(void);

/*! Departure event function. */
void depart(void);

/*! Report generator function. */
void report(void);

/*! Update area accumulators for time-average statistics. */
void update_time_avg_stats(void);

/*! Exponential variate generation function. 
 * @param mean is the expected value of the random variable
 * @param stream identifies the stream for the uniform distribution
 * @return an exponential random variate with mean "mean".
 * */
float expon(float mean, int stream);

#endif
