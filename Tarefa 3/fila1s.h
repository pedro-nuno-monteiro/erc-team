#ifndef FILA1S_H
#define FILA1S_H

/*! \file fila1s.h
 *  \brief Header file for simulation and modelling.
 *  
 *  It uses the code from Simulation and Modelling 5e by Averill M. Law B.
 */

/*! External definitions for single-server queueing system. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lcgrand.h"
#include <time.h>
#include <string.h>

/*! Limit on queue length. */
#define Q_LIMIT 100

/*! Maximum number of servers allowed. */
#define MAX_SERVERS 99  /*!< No repetitions; thus, n < 99. */

/*! Mnemonics for server states: BUSY or IDLE. */
#define BUSY 1
#define IDLE 0

/*! 
 * @brief Structure that represents the system state.
 */
typedef struct {
    int next_event_type;            /*!< Type of the next event to occur (e.g., arrival, departure). */
    int num_custs_delayed;          /*!< Number of customers that have been delayed. */
    int num_delays_required;        /*!< Number of delays required to complete the simulation. */
    int num_in_q;                   /*!< Number of customers in the queue. */
    int server_status[MAX_SERVERS + 2];              /*!< Server status (0 = idle, 1 = busy). */
    float time_arrival[Q_LIMIT + 1];    /*!< Time of arrival for each customer in the queue. */
    float mean_interarrival;        /*!< Mean time between customer arrivals (inter-arrival time). */
    float mean_service;             /*!< Mean service time. */
    int number_of_servers;          /*!< Number of active servers. */
    int num_events;                 /*!< Dynamically calculated based on servers. */
    int streams[MAX_SERVERS + 1];   /*!< Stream identifiers. */
    int without_infinite_queue;     /*!< 1 for infinite queue, 0 for limited queue. */
    float A; /* !< Offered traffic. */
} SystemState;

/*! 
 * @brief Structure that holds statistics for time-average calculations.
 */
typedef struct {
    float area_num_in_q;        /*!< Cumulative area under the number-in-queue function. */
    float area_server_status[MAX_SERVERS + 2];   /*!< Cumulative area under the server-status function (busy or idle). */
    float total_of_delays;      /*!< Total delay time. */
    int lost_customers;         /*!< Number of lost customers (Erlang-B model). */
} Statistics;

/*! 
 * @brief Structure that keeps track of event timing functions.
 */
typedef struct {
    float sim_time;             /*!< Current simulation time. */
    float time_last_event;      /*!< Time of the last event. */
    float time_next_event[MAX_SERVERS + 2];   /*!< Scheduled times for the next events. */
} EventList;

/*! 
 * @brief Structure with file pointers for input and output op.
 */
typedef struct {
    FILE * infile;   /*!< Input file pointer for reading data. */
    FILE * outfile;  /*!< Output file pointer for storing/writing results. */
} Files;

/*! 
 * @brief Prompts the user to enter two unique seeds, one for arrivals and one for departures.
 *
 * This function requests seeds within a specific range [1, 100], ensuring the seeds are positive,
 * within the specified range, and distinct from each other.
 *
 * @param state Pointer to the system state structure where the seeds are stored.
 */
void ask_streams(SystemState *state);

/*! 
 * @brief Generates additional seed streams for each server in the system.
 *
 * This function fills the `streams` array in `state` with unique seeds for each server. 
 * It ensures that the seeds are sequential, distinct from the arrival stream seed, 
 * and wraps around if the seed exceeds 100.
 *
 * @param state Pointer to the system state structure containing the number of servers 
 *              and an array to store the generated seeds.
 */
void generate_other_streams(SystemState *state);

/*! 
 * @brief Selects an available server with the lowest usage rate.
 *
 * This function iterates through all servers and identifies the free server (IDLE) 
 * with the lowest utilization rate. If all servers are busy, it returns -1.
 *
 * @param state Pointer to the system state structure that contains the server status.
 * @param stats Pointer to the statistics structure containing server utilization data.
 * 
 * @return The index of the available server with the lowest utilization rate, 
 *         or -1 if all servers are busy.
 */
int selectFreeServer(SystemState * state, Statistics * stats);

/*! 
 * @brief Initializes the state of the simulation system.
 *
 * This function sets up the initial conditions for the simulation, including the simulation 
 * clock, random number generators, state variables, and event list. It also initializes the 
 * server statuses and statistical counters.
 *
 * @param state Pointer to the system state structure that holds the simulation state and server information.
 * @param stats Pointer to the statistics structure that holds the statistical data.
 * @param events Pointer to the event list structure that keeps track of event times and simulation time.
 * @param stream The random number stream used to generate the arrival times.
 */
void initialize(SystemState *state, Statistics *stats, EventList *events, int stream);

/** 
 * @brief Determines the next event based on the times stored in the event list and advances the simulation clock to the time of the next event.
 * 
 * This function searches through the event list to find the event with the smallest time, sets that event as the next event to occur, and updates 
 * the simulation clock. If the event list is empty, the simulation is terminated.
 * 
 * @param state Pointer to the system state structure, updated with the next event type.
 * @param stats Pointer to the statistics structure, used for access to simulation variables.
 * @param files Pointer to the file management structure, used for writing termination messages if the event list is empty.
 * @param events Pointer to the event list structure, updated to set the simulation time to the time of the next event.
 * 
 * @note If the event list is empty, the simulation will stop and an error message will be written to the output file.
 */
void timing(SystemState *state, Statistics *stats, Files* files, EventList *events);

/** 
 * @brief Handles customer arrivals at the queue by scheduling the next arrival, checking server availability,
 *        and managing the queue if all servers are occupied.
 * 
 * This function schedules the next arrival event, selects an available server (if any), and manages the queue. 
 * If no servers are available and the queue is not infinite, the customer is rejected. If the queue has space, 
 * the customer's arrival time is added to the queue.
 * 
 * @param state Pointer to the system state structure, updated to track queue size, server status, and number of lost customers.
 * @param stats Pointer to the statistics structure, updated to increment lost customers if the queue is full.
 * @param files Pointer to the file management structure, used for handling queue overflow conditions.
 * @param events Pointer to the event list structure, updated to schedule the next arrival event.
 */
void arrive(SystemState *state, Statistics *stats, Files* files, EventList *events);

/** 
 * @brief Handles customer departures by updating queue length, calculating delay for the customer beginning service,
 *        and setting the server status to idle if the queue is empty.
 * 
 * This function handles the departure of a customer from the queue, updating the system's state by reducing the queue size, 
 * calculating the delay for the departing customer, and scheduling the next departure event. If the queue is empty, 
 * the server status is set to idle.
 * 
 * @param state Pointer to the system state structure, updated to track server status, queue length, and customer arrival times.
 * @param stats Pointer to the statistics structure, updated to accumulate total delay and number of customers delayed.
 * @param events Pointer to the event list structure, updated to schedule the next departure or set it to infinity if the server becomes idle.
 */
void depart(SystemState *state, Statistics *stats, EventList *events);

/** 
 * @brief Writes the simulation results to the output file, including performance metrics.
 *
 * This function generates a formatted report of the simulation, including metrics such as the 
 * average delay, lost customers, server utilization, and other system parameters. The results 
 * are written to the output file.
 * 
 * @param state Pointer to the system state structure that contains parameters such as mean interarrival time, 
 *              mean service time, and the number of customers.
 * @param stats Pointer to the statistics structure that contains calculated values like delays, lost customers, 
 *              and server utilization statistics.
 * @param files Pointer to the file management structure that handles the output file.
 * @param events Pointer to the event list structure that tracks simulation time and events.
 * 
 * @note This function assumes the `files->outfile` has been opened before calling and will write the results to this file.
 */
void report(SystemState* state, Statistics* stats, Files* files, EventList* events);

/** 
 * @brief Updates the time-based statistics for the simulation, such as the area under the 
 *        number-in-queue and server utilization curves.
 * 
 * This function calculates the time elapsed since the last event and updates the time-based 
 * statistics, including the area under the number-in-queue function and server utilization.
 * 
 * @param state Pointer to the system state structure containing the current state of the system, 
 *              including the number of customers in the queue and server status.
 * @param stats Pointer to the statistics structure that will be updated with the time-based metrics.
 * @param events Pointer to the event list structure that contains simulation time and last event time.
 * 
 * @note The function updates the time-based statistics by integrating over the elapsed time since 
 *       the last event to track changes in the system state.
 */
void update_time_avg_stats(SystemState *state, Statistics *stats, EventList *events);

/*! 
 * @brief Calculates an exponentially distributed random variable.
 *
 * This function generates a random variate that follows an exponential distribution
 * with a given mean. It uses the specified stream for random number generation.
 *
 * @param mean The mean of the exponential distribution.
 * @param stream The seed for the random number generation stream.
 *
 * @return An exponentially distributed random variable with the specified mean.
 */
float expon(float mean, int stream);

#endif
