/*! Code from 
 * Simulation and Modelling 5e
 * Averill M. Law B
*/

#include "fila1s.h"
#include "lcgrand.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Function ask_streams: prompts the user to enter two seeds, one for arrivals and another for departures.
 * Output parameters: 
 *       - state->streams[i]: array updated with the first two seeds.
 * 
 * Input parameters: 
 *       - state: pointer to the system state structure where seeds are stored.
 */
void ask_streams(SystemState *state){


	/* We ask for 2 seeds, the first is reserved for arrivals and the second is used for departures */
	for(int i = 0; i < 2; i++) {
			do {
				if(i==0){
					printf("Escreve a semente correspondente as chegadas: ");
				}
				else{
					printf("Escreve a semente correspondente as partidas: ");
				}
				
				if (scanf("%d", &state->streams[i]) != 1) {  
					printf("Por favor, insira um numero positivo.\n");
					int ch;
					while ((ch = getchar()) != '\n' && ch != EOF); 
					state->streams[i] = -1;
				}

				if(state->streams[i] <= 0 || state->streams[i] > 100) { /* We guarantee that each seed introduced is within the desired values  */
					printf("As sementes têm de estar compreendidas entre [1, 100]. \n");
				}
			} while(state->streams[i] <= 0 || state->streams[i] > 100 || (i == 1 && state->streams[0] == state->streams[1])); /* This loop ensures that the two seeds are different from each other */
		}

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Function generate_other_streams: generates additional seed streams for servers.
 * 
 * Input/output parameters: 
 *         - state->streams:  array updated with generated seeds
 * 
 * Input parameters: 
 *         - state: structure containing the number of servers.
 */
void generate_other_streams(SystemState *state){

	/* This function generates the remaining seeds used. If there are n servers then we need to generate n-1 seeds because the first seed corresponding to arrival is used for the first server. */

	/* This loop serves to ensure that the seeds now generated are not the same as the seed used by the arrival.
	Furthermore, it guarantees that seed generation is circular, that is, if we reach index 1001 it becomes 1. 
	Seeds are generated from the starting seed for server 1 and the rest are obtained by adding 1 unit. */

	for(int i= 2; i<=state->number_of_servers + 1; i++){
		state->streams[i] = state->streams[i-1] + 1; 
		if(state->streams[i] == state->streams[0]){
			state->streams[i] = state->streams[i-1] + 2;
		}
		if (state->streams[i] == 101) {
			state->streams[i] = 1;
		}	
	}

	printf("Streams = [ ");

	/* This loop is only used to show the array with all the seeds that will be used */
	for(int i=0; i<=state->number_of_servers + 1 ; i++){
		if(i < state->number_of_servers+1){
			printf(" %d, ", state->streams[i]);
		}
		else{
			printf(" %d ] \n", state->streams[i]);
		}
	}

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Function selectFreeServer: selects an available server based on the area of each server.
 * 
 * Output parameters: index of the available server or -1 if all are busy.
 * 
 * Input/output parameters: 
 *         - stats->area_server_status: indicates the area of each server.
 * 
 * Input parameters: 
 *         - state->server_status: array with the status (IDLE/BUSY) of each server.
 */
int selectFreeServer(SystemState * state, Statistics * stats) { 
	
	int livre = -1; /* At the beginning we assume that no server is IDLE */
	int aux = stats->area_server_status[2]; /* Let's assume that the first server is the one with the lowest utilization rate */ 
	int indice = 2; /* We assume that the first server (index = 2) is the one with the lowest utilization rate */

	/* This loop serves to understand which server has the lowest usage rate and is at the same time free */
    for (int i = 2; i <= state->number_of_servers+1; ++i) {
        if (state->server_status[i] == IDLE) {
			livre = 1; /* There is at least one free server */
			if(stats->area_server_status[i]<=aux){
				aux = stats->area_server_status[i];
				indice = i; 
			}
            
        }
    }
	if (livre == -1){
		return -1; /* returns -1 if all servers are busy */
	}
	else{
		return indice; /* returns the index of the first available server */
	}
    
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Function expon:  calculates an exponentially distributed random variable.
 * 
 * Input parameters: 
 *        - mean: mean of the exponential distribution 
 *        - stream: seed for random number generation.
 * 
 * Returns: an exponentially distributed value with the specified mean.
 */
float expon(float mean, int stream) {
	return -mean * logf(lcgrand(stream));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** 
 * Function initialize: initializes the state of the simulation system, including setting up
 *                      the simulation clock, random number generators, state variables, and event list
 * 
 * Input parameters:
 *   - stream: Random number stream used for generating arrival times.
 * 
 * Input/Output parameters:
 *   - state: Pointer to the system state structure.
 *   - stats: Pointer to the statistics structure.
 *   - events: Pointer to the event list structure.
 *   - stream: Random number stream used for generating arrival times.

*/
void initialize(SystemState * state, Statistics * stats, EventList * events, int stream) {
	
	events->sim_time = 0.0; /*! Initialize the simulation clock. */


	events->time_next_event[1] = events->sim_time + expon(state->mean_interarrival, stream); /* time_next_event[1] representa o evento de chegada*/
	// events->time_next_event[2] = 1.0e+30;

	/* Initialize all servers as IDLE */
	/* Initialize departure events for each server */
	for (int i = 2; i <= state->number_of_servers + 1; ++i) {
    	state->server_status[i] = IDLE;
		stats->area_server_status[i] = 0.0;
		/* We use i+2 here because we start at [2] since position [1] is for the arrival event */
		events->time_next_event[i] = 1.0e+30; /** These are the departures for each server. We put a big number. */
	}


	state->num_events=state->number_of_servers+1 ; /** Update the number of events*/

	state->num_in_q = 0; /** Number of custumers in queu start in zero */
	events->time_last_event = 0.0; /* Time of last event starts in zero */
	/*! Initialize the statistical counters. */
	state->num_custs_delayed =	 0; /* Number of custumers */
	stats->total_of_delays	 =	 0.0; /* Number of delays */
	stats->area_num_in_q	 =	 0.0; /* Number of the utilization rate */
	stats->lost_customers    =   0; /* Number of lost costumers */
	/*! Initialize event list. Since no customers are present, the departure
	(service completion) event is eliminated from consideration. */
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Function report: writes the simulation results to the output file, including performance metrics
 *                  like average delay, lost customers, and server utilization.
 * 
 * Input/Output parameters:
 *   - state: Pointer to the system state structure.
 *   - stats: Pointer to the statistics structure.
 *   - files: Pointer to the file management structure.
 *   - events: Pointer to the event list structure.
 * 
 * Output parameters:
 *   - Writes the formatted simulation report to the output file.
 */
void report(SystemState * state, Statistics * stats, Files * files, EventList * events) {

	/*! Write report heading and input parameters to the output file. */
	fprintf(files->outfile, "Single-server queueing system\n\n");
	fprintf(files->outfile, "Mean interarrival time%11.3f minutes\n\n", state->mean_interarrival);
	fprintf(files->outfile, "Mean service time%16.3f minutes\n\n", state->mean_service);
	fprintf(files->outfile, "Number of customers%14d\n\n", state->num_delays_required);
	if(state->without_infinite_queue == 0 ){
		fprintf(files->outfile, "Without Queue\n\n");
	}
	else{
		fprintf(files->outfile, "With Queue\n\n");
	}
	

	/** Print the average delay in queue per client */
	fprintf(files->outfile, "\n\nAverage delay in queue per client %11.3f minutes\n\n", stats->total_of_delays / state->num_custs_delayed);

	if(state->without_infinite_queue == 0){  /* If we don't have an infinite queue */
		/** Print the average number of lost clients */
		fprintf(files->outfile, "Average number of lost clients %14.3f\n\n", stats->lost_customers);
	}

	else{ /* If we have an infinite queue */
		/** Print the average number of clients in queue */
		fprintf(files->outfile, "Average number of clients in queue %10.3f\n\n", stats->area_num_in_q / events->sim_time);
	}
	

	/** Print the average server utilization. 
	 *  We use this loop to ensure that if area_server_status == 0 then we avoid divisions by zero */
	for (int i = 2; i <= state->number_of_servers + 1; i++ ){
		if (events->sim_time > 0) {
        	fprintf(files->outfile, "Server %d utilization%25.3f\n\n", i-1 ,stats->area_server_status[i] / events->sim_time);
   		 } 
		 
		else { /* To avoid division by zero if events->sim_time > 0 */
        	fprintf(files->outfile, "Server %d utilization%25.3f \n\n", i-1, 0.0);
    	}
	}


	fprintf(files->outfile, "Time simulation ended%26.3f minutes", events->sim_time);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Function update_time_avg_stats: updates the time-based statistics for the simulation, such as the area under
 *                                 the number-in-queue and server utilization curves.
 * Input/Output parameters:
 *   - state: Pointer to the system state structure.
 *   - stats: Pointer to the statistics structure.
 *   - events: Pointer to the event list structure.
 */
void update_time_avg_stats(SystemState * state, Statistics * stats, EventList * events) {
	
	
	float time_since_last_event; /*! Compute time since last event, and update last-event-time marker. */

	/* This calculation determines the time that has passed since the last processed event.
       It is essential to calculate the areas under the curves of the state variables */
	time_since_last_event = events->sim_time - events->time_last_event;
	events->time_last_event	 = events->sim_time; 
	
	/* This metric is useful to calculate the average number of customers in the queue during the simulation */
	stats->area_num_in_q  += state->num_in_q * time_since_last_event; /*! Update area under number-in-queue function. */

	/* Update the area under the server-status function for each server */
	for (int i = 2; i <= state->number_of_servers + 1; ++i)
        stats->area_server_status[i] += state->server_status[i] * time_since_last_event;
	

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Function timing : determines the next event based on the times stored in the event list. Advances the simulation clock to the time of the next event.
 *                    Checks if the event list is empty; if so, the simulation terminates.
 * 
 * Input/Output parameters:
 *   - state: Pointer to the system state structure, updated to set the next event type.
 *   - stats: Pointer to the statistics structure, used here only for access to simulation variables.
 *   - files: Pointer to the file management structure, used for writing termination messages if the event list is empty.
 *   - events: Pointer to the event list structure, updated to set the simulation time to the time of the next event.
 */
void timing(SystemState * state, Statistics * stats, Files * files, EventList * events) {

	/* is a high value that will be used to compare and determine the shortest event time */
	float min_time_next_event = 1.0e+29;

	/* initializes to zero because there is no event selected */
	state->next_event_type = 0;
	
	/*! The function loops through all event types and checks the time of each stored event. 
	    The event with the shortest time is the next to happen */
	for (int i = 1; i <= state->num_events; ++i) /* event [1] is included in the comparison because it may be the next event */
		if (events->time_next_event[i] < min_time_next_event) {
			min_time_next_event = events->time_next_event[i];
			state->next_event_type = i; /* the index value of the event with the shortest time is updated */
		}

	/*! Check to see whether the event list is empty. */
	if (state->next_event_type == 0) { /* if no event was selected it means that the event list is empty so the simulation is stopped */
		/* The event list is empty, so stop the simulation. */
		fprintf(files->outfile, "\nEvent list empty at time %f", events->sim_time);
		exit(1);
	}

	/* The event list is not empty so we advance the simulation clock */
	events->sim_time = min_time_next_event; 

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Function arrive : handles customers arriving at the queue by scheduling the next arrival, checking server availability,
 *                    and managing the queue if all servers are occupied.
 * 
 * Input/Output parameters:
 *   - state: Pointer to the system state structure, updated to track queue size, server status, and number of lost customers.
 *   - stats: Pointer to the statistics structure, updated to increment lost customers if the queue is full.
 *   - files: Pointer to the file management structure, used for handling queue overflow conditions.
 *   - events: Pointer to the event list structure, updated to schedule the next arrival event.
 */
void arrive(SystemState * state, Statistics * stats, Files * files, EventList * events) {
	
	float delay;
	
	/* Schedule the next arrival event */
	events->time_next_event[1] = events->sim_time + expon(state->mean_interarrival,state->streams[0]); 

	/* Checks if there are free servers, if there are not, the function returns -1 */
	int free_server_index = selectFreeServer(state, stats);

	if (free_server_index != -1) { /* There are free servers */
        delay = 0.0; /* The customer is served immediately, so the delay = 0 */

        /* Increases the number of customers served */
        ++state->num_custs_delayed;

        /* Mark the server as busy */
        state->server_status[free_server_index] = BUSY;

        /* Schedule a departure for this customer (service completion) */
        events->time_next_event[free_server_index] = events->sim_time + expon(state->mean_service, state->streams[free_server_index-1]);
    }

	else{ /* Todos os servers estao ocupados */

		if(state->without_infinite_queue == 0){ /* If we don't have a queue -> M/M/n/0 (Erlang-B): Reject customer */
			stats->lost_customers++;
		}

		else{

			++state->num_in_q; /*! We increased the number of users in the waiting list */
			
			/*! Check to see whether an overflow condition exists. */
			if (state->num_in_q > Q_LIMIT) {
				/*! The queue has overflowed, so stop the simulation. */
				fprintf(files->outfile, "\nOverflow of the array time_arrival at");
				fprintf(files->outfile, " time %f", events->sim_time);
				exit(2);
			}
			/*! There is still room in the queue, so store the time of arrival of the
			arriving customer at the (new) end of time_arrival. */
			state->time_arrival[state->num_in_q] = events->sim_time;

		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Function depart : handles customer departures by updating queue length, calculating delay for the customer beginning service,
 *                   and setting the server status to idle if the queue is empty.
 * 
 * Input/Output parameters:
 *   - state: Pointer to the system state structure, updated to track server status, queue length, and customer arrival times.
 *   - stats: Pointer to the statistics structure, updated to accumulate total delay and number of customers delayed.
 *   - events: Pointer to the event list structure, updated to schedule the next departure or set it to infinity if the server becomes idle.
 */
void depart(SystemState *state, Statistics *stats, EventList *events) {
	
	float delay;

	/*! If the queue is empty */
	if (state->num_in_q == 0) {
		/*! The queue is empty so make the server idle and eliminate the
		departure (service completion) event from consideration. */
		state->server_status[state->next_event_type] = IDLE; /* The next_event type corresponds to the server index -> see in the timing function */
		events->time_next_event[state->next_event_type] = 1.0e+30; /* Sets the next event to infinite */
	}
	else { /* The queue is not empty */
		
		/* So the number of customers in the queue decreases */
		--state->num_in_q;

		/*! Compute the delay of the customer who is beginning service and update the total delay accumulator. */
		delay = events->sim_time - state->time_arrival[1];

		stats->total_of_delays += delay;

		/*! Increment the number of customers delayed, and schedule departure. */
		++state->num_custs_delayed;
		events->time_next_event[2] = events->sim_time + expon(state->mean_service,state->streams[state->next_event_type]);

		/*! Move each customer in queue (if any) up one place. With a circular queue it is much better */
		for (int i = 1; i <= state->num_in_q; ++i)
			state->time_arrival[i] = state->time_arrival[i + 1];
	}

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////