#ifndef FILA1S_H
#define FILA1S_H

/*! \file fila1s.h
 *  \brief Header file for simulation and modelling.
 *  
 *  It uses the code from Simulation and Modelling 5e by Averill M. Law B.
 */

/*! Stream (1.100) to be defined by input and different for each variable */
/*! External definitions for single-server queueing system. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lcgrand.h"
#include <time.h>

/*! Header file for random-number generator. */
/*! Limit on queue length. */
#define Q_LIMIT 100

/*! Mnemonics for server's being busy */
#define BUSY 1

/*! and idle. */
#define IDLE 0

/*! 
 * @brief Structure that represents the system state.
 */
typedef struct {
    int next_event_type;            /*!< Type of the next event to occur (e.g., arrival, departure). */
    int num_custs_delayed;          /*!< Number of customers that have been delayed. */
    int num_delays_required;        /*!< Number of delays required to complete the simulation. */
    int num_in_q;                   /*!< Number of customers in the queue. */
    int server_status;              /*!< Server status (0 = idle, 1 = busy). */
    float time_arrival[Q_LIMIT + 1];    /*!< Time of arrival for each customer in the queue. */
    float mean_interarrival;        /*!< Mean time between customer arrivals (inter-arrival time). */
    float mean_service;             /*!< Mean service time. */
} SystemState;

/*! 
 * @brief Structure that holds statistics for time-average calculations.
 */
typedef struct {
    float area_num_in_q;        /*!< Cumulative area under the number-in-queue function. */
    float area_server_status;   /*!< Cumulative area under the server-status function (busy or idle). */
    float total_of_delays;      /*!< Total amount of delay. */
} Statistics;

/*! 
 * @brief Structure that keeps track of event timing functions.
 */
typedef struct {
    float sim_time;             /*!< Simulation time. */
    float time_last_event;      /*!< Time of the last event. */
    float time_next_event[3];   /*!< Scheduled times for the next events. */
} EventList;

/*! 
 * @brief Structure with file pointers for input and output op.
 */
typedef struct {
    FILE * infile;   /*!< Input file pointer for reading data. */
    FILE * outfile;  /*!< Output file pointer for storing/writing results. */
} Files;

/*!
 * Initialization function.
 * */
void initialize(SystemState *state, Statistics *stats, EventList *events, int stream);

/*! Timing function. */
void timing(SystemState *state, Statistics *stats, Files* files, EventList *events, int num_events);

/*! Arrival event function. */
void arrive(SystemState *state, Statistics *stats, Files* files, EventList *events, int stream);

/*! Departure event function. */
void depart(SystemState *state, Statistics *stats, EventList *events, int stream);

/*! Report generator function.
 * 	Compute and write estimates of desired measures of performance.
 * */
void report(SystemState* state, Statistics* stats, Files* files, EventList* events);

/*!
 * @brief Update area accumulators for time-average statistics.
 *
 * This function updates statistical accumulators for calculating time-averaged
 * values, such as the average number of items in the queue and the server's busy time.
 * It computes the time since the last event, updates the `time_last_event` marker,
 * and then increments the appropriate accumulators based on the current state of the system.
 */
void update_time_avg_stats(SystemState *state, Statistics *stats, EventList *events);

/*! Exponential variate generation function. 
 * @param mean is the expected value of the random variable
 * @param stream identifies the stream for the uniform distribution
 * @return an exponential random variate with mean "mean".
 * */
float expon(float mean, int stream);

#endif
