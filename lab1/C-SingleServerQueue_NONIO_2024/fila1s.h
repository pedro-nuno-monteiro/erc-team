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

/*/*! Global variabled defined in simula_fila1s.c */
// variáveis definidas noutro sítio (por isso é que têm extern int)
/*extern int next_event_type, num_custs_delayed, num_delays_required, num_events,
num_in_q, server_status;
extern float area_num_in_q, area_server_status, mean_interarrival, mean_service,
sim_time, time_arrival[Q_LIMIT + 1], time_last_event, time_next_event[3],
total_of_delays;
extern FILE *infile, *outfile;*/


typedef struct {
    int next_event_type;       
    int num_custs_delayed;     
    int num_delays_required;   
    int num_in_q;              
    int server_status;         
    float time_arrival[Q_LIMIT + 1];  
    float mean_interarrival;  // Média do intervalo entre chegadas
    float mean_service;
} SystemState;

typedef struct {
    float area_num_in_q;       
    float area_server_status;  
    float total_of_delays;     
} Statistics;

typedef struct {
    float sim_time;            
    float time_last_event;     
    float time_next_event[3];  
} EventList;

typedef struct {
    FILE *infile;   // Ficheiro de entrada
    FILE *outfile;  // Ficheiro de saída
} Files;


/*!
 * Initialization function.
 * \note arguments to be added
 * */
void initialize(SystemState *state, Statistics *stats, EventList *events);

/*! Timing function. */
void timing(SystemState *state, Statistics *stats, Files* files, EventList *events, int num_events);

/*! Arrival event function. */
void arrive(SystemState *state, Statistics *stats, Files* files, EventList *events);

/*! Departure event function. */
void depart(SystemState *state, Statistics *stats, EventList *events);

/*! Report generator function. */
void report(SystemState* state, Statistics* stats, Files* files, EventList* events);

/*! Update area accumulators for time-average statistics. */
void update_time_avg_stats(SystemState *state, Statistics *stats, EventList *events);

/*! Exponential variate generation function. 
 * @param mean is the expected value of the random variable
 * @param stream identifies the stream for the uniform distribution
 * @return an exponential random variate with mean "mean".
 * */
float expon(float mean, int stream);

#endif
