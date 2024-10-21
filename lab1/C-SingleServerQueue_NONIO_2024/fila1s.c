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
void initialize(void)
{
	
	sim_time = 0.0; /*! Initialize the simulation clock. */
        /*! Initialize the random number gernerators sequencenes */
        /*! stream 1 for inter-arrival time */
        /*! stream 2 for departure time */
	/*! Initialize the state variables. */
	server_status = IDLE;
	num_in_q = 0;
	time_last_event = 0.0;
	/*! Initialize the statistical counters. */
	num_custs_delayed =	 0;
	total_of_delays	 =	 0.0;
	area_num_in_q	 =	 0.0;
	area_server_status	 =	 0.0;
	/*! Initialize event list. Since no customers are present, the departure
	(service completion) event is eliminated from consideration. */
	time_next_event[1] = sim_time + expon(mean_interarrival,1);
	time_next_event[2] = 1.0e+30;
}

/*! Report generator function. */
void report(void) 
{
	/*! Compute and write estimates of desired measures of performance. */

	fprintf(outfile, "\n\nAverage delay in queue%11.3f minutes\n\n",
	total_of_delays / num_custs_delayed);
	fprintf(outfile, "Average number in queue%10.3f\n\n",
	area_num_in_q / sim_time);
	fprintf(outfile, "Server utilization%15.3f\n\n",
	area_server_status / sim_time);
	fprintf(outfile, "Time simulation ended%12.3f minutes", sim_time);
}


/*! Update area accumulators for time-average statistics. */
void update_time_avg_stats(void)
{
	float time_since_last_event; /*! Compute time since last event, and update last-event-time marker. */
	
	time_since_last_event = sim_time - time_last_event;
	time_last_event	 = sim_time;
	
	area_num_in_q  += num_in_q * time_since_last_event; /*! Update area under number-in-queue function. */
	
	area_server_status += server_status * time_since_last_event; /*! Update area under server-busy indicator function. */
}

/*! Timing function. */
void timing(void) 
{
	int i;
	float min_time_next_event = 1.0e+29;
	next_event_type = 0;
	/*! Determine the event type of the next event to occur. */
	for (i = 1; i <= num_events; ++i)
		if (time_next_event[i] < min_time_next_event) {
			min_time_next_event = time_next_event[i];
			next_event_type = i;
		}
	/*! Check to see whether the event list is empty. */
	if (next_event_type == 0) {
		/* The event list is empty, so stop the simulation. */
		fprintf(outfile, "\nEvent list empty at time %f", sim_time);
		exit(1);
	}
	/*! The event list is not empty, so advance the simulation clock. */
	sim_time = min_time_next_event;
}

/*! Arrival event function. */
void arrive(void)
{
        float delay;
        
        time_next_event[1] = sim_time + expon(mean_interarrival,1); /*! Schedule next arrival. */
        /*! Check to see whether server is busy. */
        if (server_status == BUSY) {
                ++num_in_q; /*! Server is busy, so increment number of customers in queue. */
                /*! Check to see whether an overflow condition exists. */
                if (num_in_q > Q_LIMIT) {
                        /*! The queue has overflowed, so stop the simulation. */
                        fprintf(outfile, "\nOverflow of the array time_arrival at");
                        fprintf(outfile, " time %f", sim_time);
                        exit(2);
                }
                /*! There is still room in the queue, so store the time of arrival of the
                arriving customer at the (new) end of time_arrival. */
                time_arrival[num_in_q] = sim_time;
        }
        else {
                /*! Server is idle, so arriving customer has a delay of zero. (The
                following two statements are for program clarity and do not affect
                the results of the simulation.) */
                delay = 0.0;
                total_of_delays += delay;
                /*! Increment the number of customers delayed, and make server busy. */
                ++num_custs_delayed;
                server_status = BUSY;
                /*! Schedule a departure (service completion). */
                time_next_event[2] = sim_time + expon(mean_service,2);
        }
}

/*! Departure event function. */
void depart(void)
{
	int  i;
	float delay;
	/*! Check to see whether the queue is empty. */
	if (num_in_q == 0) {
		/*! The queue is empty so make the server idle and eliminate the
		departure (service completion) event from consideration. */
		server_status = IDLE;
		time_next_event[2] = 1.0e+30;
	}
	else {
		/*! The queue is nonempty, so decrement the number of customers in
		queue. */
		--num_in_q;
		/*! Compute the delay of the customer who is beginning service and update
		the total delay accumulator. */
		delay = sim_time - time_arrival[1];
		total_of_delays += delay;
		/*! Increment the number of customers delayed, and schedule departure. */
		++num_custs_delayed;
		time_next_event[2] = sim_time + expon(mean_service,2);
		/*! Move each customer in queue (if any) up one place. */
		for (i = 1; i <= num_in_q; ++i)
			time_arrival[i] = time_arrival[i + 1];
	}
}

