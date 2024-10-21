/*! Code from 
 * Simulation and Modelling 5e
 * Averill M. Law B
*/
#include "fila1s.h"
#include "lcgrand.h"


/*! Exponential variate generation function. 
 * @param mean is the expected value of the random variable
 * @param stream identifies the stream for the uniform distribution
 * @return an exponential random variate with mean "mean".
 * */
float expon(float mean, int stream) 
{
    return -mean * logf(lcgrand(stream)); /*!  Return an exponential random variate with mean "mean". */
}

/*!
 * Initialization function.
 * \note arguments to be added
 * */
void initialize(SystemState *state, Statistics *stats, EventList *events)
{
	
	events->sim_time = 0.0; /*! Initialize the simulation clock. */
        /*! Initialize the random number gernerators sequencenes */
        /*! stream 1 for inter-arrival time */
        /*! stream 2 for departure time */
	/*! Initialize the state variables. */
	state->server_status = IDLE;
	state->num_in_q = 0;
	events->time_last_event = 0.0;
	/*! Initialize the statistical counters. */
	state->num_custs_delayed =	 0;
	stats->total_of_delays	 =	 0.0;
	stats->area_num_in_q	 =	 0.0;
	stats->area_server_status	 =	 0.0;
	/*! Initialize event list. Since no customers are present, the departure
	(service completion) event is eliminated from consideration. */
	events->time_next_event[1] = events->sim_time + expon(state->mean_interarrival,1);
	events->time_next_event[2] = 1.0e+30;
}

/*! Report generator function. */
void report(SystemState* state, Statistics* stats, Files* files, EventList* events) 
{
	/*! Compute and write estimates of desired measures of performance. */

	fprintf(files->outfile, "\n\nAverage delay in queue%11.3f minutes\n\n",
	stats->total_of_delays / state->num_custs_delayed);
	fprintf(files->outfile, "Average number in queue%10.3f\n\n",
	stats->area_num_in_q / events->sim_time);
	fprintf(files->outfile, "Server utilization%15.3f\n\n",
	stats->area_server_status / events->sim_time);
	fprintf(files->outfile, "Time simulation ended%12.3f minutes", events->sim_time);
}


/*! Update area accumulators for time-average statistics. */
void update_time_avg_stats(SystemState *state, Statistics *stats, EventList *events)
{
	float time_since_last_event; /*! Compute time since last event, and update last-event-time marker. */
	
	time_since_last_event = events->sim_time - events->time_last_event;
	events->time_last_event	 = events->sim_time;
	
	stats->area_num_in_q  += state->num_in_q * time_since_last_event; /*! Update area under number-in-queue function. */
	
	stats->area_server_status += state->server_status * time_since_last_event; /*! Update area under server-busy indicator function. */
}

/*! Timing function. */
void timing(SystemState *state, Statistics *stats, Files* files, EventList *events, int num_events) 
{
	int i;
	float min_time_next_event = 1.0e+29;

	state->next_event_type = 0;
	/*! Determine the event type of the next event to occur. */
	for (i = 1; i <= num_events; ++i)
		if (events->time_next_event[i] < min_time_next_event) {
			min_time_next_event = events->time_next_event[i];
			state->next_event_type = i;
		}
	/*! Check to see whether the event list is empty. */
	if (state->next_event_type == 0) {
		/* The event list is empty, so stop the simulation. */
		fprintf(files->outfile, "\nEvent list empty at time %f", events->sim_time);
		exit(1);
	}
	/*! The event list is not empty, so advance the simulation clock. */
	events->sim_time = min_time_next_event;
}

/*! Arrival event function. */
void arrive(SystemState *state, Statistics *stats, Files* files, EventList *events)
{
        float delay;
        
        events->time_next_event[1] = events->sim_time + expon(state->mean_interarrival,1); /*! Schedule next arrival. */
        /*! Check to see whether server is busy. */
        if (state->server_status == BUSY) {
                ++state->num_in_q; /*! Server is busy, so increment number of customers in queue. */
                /*! Check to see whether an overflow condition exists. */
                if (state->num_in_q > Q_LIMIT) {
                        /*! The queue has overflowed, so stop the simulation. */
                        fprintf(files->outfile, "\nOverflow of the array time_arrival at");
                        fprintf(files->outfile, " time %f", events->sim_time);
                        exit(2);
                }
                /*! There is still room in the queue, so store the time of arrival of the
                arriving customer at the (new) end of time_arrival. */
                state->time_arrival[num_in_q] = events->sim_time;
        }
        else {
                /*! Server is idle, so arriving customer has a delay of zero. (The
                following two statements are for program clarity and do not affect
                the results of the simulation.) */
                delay = 0.0;
                stats->total_of_delays += delay;
                /*! Increment the number of customers delayed, and make server busy. */
                ++state->num_custs_delayed;
                state->server_status = BUSY;
                /*! Schedule a departure (service completion). */
                events->time_next_event[2] = events->sim_time + expon(state->mean_service,2);
        }
}

/*! Departure event function. */
void depart(SystemState *state, Statistics *stats, EventList *events)
{
	int  i;
	float delay;
	/*! Check to see whether the queue is empty. */
	if (state->num_in_q == 0) {
		/*! The queue is empty so make the server idle and eliminate the
		departure (service completion) event from consideration. */
		state->server_status = IDLE;
		events->time_next_event[2] = 1.0e+30;
	}
	else {
		/*! The queue is nonempty, so decrement the number of customers in
		queue. */
		--state->num_in_q;
		/*! Compute the delay of the customer who is beginning service and update
		the total delay accumulator. */
		delay = events->sim_time - state->time_arrival[1];
		stats->total_of_delays += delay;
		/*! Increment the number of customers delayed, and schedule departure. */
		++num_custs_delayed;
		events->time_next_event[2] = events->sim_time + expon(state->mean_service,2);
		/*! Move each customer in queue (if any) up one place. */
		for (i = 1; i <= state->num_in_q; ++i)
			state->time_arrival[i] = state->time_arrival[i + 1];
	}
}

