#include "fila1s.h"
#include "lcgrand.h"
#include "utilits.h"
#include "circular_queue_dynamic.h"

int selectFreeServer(SystemState * state, Statistics * stats) { 
	
	int livre = -1; /* At the beginning we assume that no server is IDLE */
	int aux = stats->area_server_status[2]; /* Let's assume that the first server is the one with the lowest utilization rate */ 
	int indice = 2; /* We assume that the first server (index = 2) is the one with the lowest utilization rate */

	/* Loop through each server to find the free server with the lowest usage rate */
	for (int i = 2; i <= state->number_of_servers+1; ++i) {
		
		if (state->server_status[i] == IDLE) {
			livre = 1; /* There is at least one free server */
			
			/* If this server has a lower usage rate, update the chosen server index */
			if(stats->area_server_status[i] <= aux) {
				aux = stats->area_server_status[i];
				indice = i; 
			}			
		}
	}

	/* Return -1 if no server is free, otherwise return the index of the free server */
	if (livre == -1) return -1;
	else return indice;
}

float expon(float mean, int stream) {
	return -mean * logf(lcgrand(stream));
}

void initialize(SystemState * state, Statistics * stats, EventList * events, int stream, circular_queue * q1) {
	
	events->sim_time = 0.0; /* Initialize the simulation clock. */

	/* Initialize the next arrival event time */
	/* time_next_event[1] representa o evento de chegada*/
	events->time_next_event[1] = events->sim_time + expon(state->mean_interarrival, stream);

	/* Initialize all servers as IDLE */
	/* Initialize departure events for each server */
	for (int i = 2; i <= state->number_of_servers + 1; ++i) {
    state->server_status[i] = IDLE;
		stats->area_server_status[i] = 0.0;
		
		/* We use i + 2 here because we start at [2] since position [1] is for the arrival event */
		events->time_next_event[i] = 1.0e+30; /* Set a large value for departure times initially */
	}

	/* Update number of events */
	state->num_events=state->number_of_servers + 1;

	/* Initialize other state variables */
	state->num_in_q = 0; 						/* Number of custumers in queue start in zero */
	events->time_last_event = 0.0; 	/* No events have occured yet */

	/* Initialize the statistical counters */
	state->num_custs_delayed =	 0; 	/* No customers delayed initially */
	stats->total_of_delays	 =	 0.0; /* No delays initially */
	stats->area_num_in_q	 =	 0.0; 	/* No customers in queue, so no area under the curve */
	stats->lost_customers    =   0; 	/* No customers lost initially */
	stats->num_occupied_servers = 0;    /* No server occupied */
	
	/* Initialize event list. Since no customers are present, the departure
	(service completion) event is eliminated from consideration. */	

	/* Initialize circular queue with FIFO discipline by default*/
	if (q1->dis == 0) {
		inic(q1, FIFO);
	}
	else {
		inic(q1, LIFO);
	}
}

void report(SystemState * state, Statistics * stats, Files * files, EventList * events, circular_queue * q1) {

	/* Write report heading and input parameters to the output file */
	fprintf(files->outfile, "Simulation of a queueing system with %d servers\n\n", state->number_of_servers);
	fprintf(files->outfile, "Mean interarrival time; %11.2f ; minutes\n\n", state->mean_interarrival);
	fprintf(files->outfile, "Mean service time; %16.2f ; minutes\n\n", state->mean_service);
	if(state->without_infinite_queue == 1){
		fprintf(files->outfile, "Number of customers ; %14d\n\n", state->num_delays_required);
	}
	else{
		fprintf(files->outfile, "Number of customers ; %14d\n\n", state->num_delays_required + stats->lost_customers);
	}
	if(state->without_infinite_queue == 0 ){
		fprintf(files->outfile, "Without Queue\n\n");
	}
	else{

		fprintf(files->outfile, "With Queue\n\n");

		if(q1->dis == 0) {
			fprintf(files->outfile, "FIFO discipline\n\n");
		}
		else {
			fprintf(files->outfile, "LIFO discipline\n\n");
		}
	}
		
	/* Print the average delay in queue per client */
	fprintf(files->outfile, "\n\nAverage delay in queue per client ; %12.3f ; minutes\n\n", stats->total_of_delays / state->num_custs_delayed);
	//fprintf(files->outfile, "Average number of occupied servers: %11.3f\n\n", stats->num_occupied_servers / state->num_custs_delayed);
	if(state->without_infinite_queue == 0) {  /* If we don't have an infinite queue */
		/* Print the average number of lost clients */
		fprintf(files->outfile, "Average number of lost clients ; %12d\n\n\n", stats->lost_customers);
	} 
	else { /* If we have an infinite queue */
		/* Print the average number of clients in queue */
		fprintf(files->outfile, "Average number of clients in queue ; %11.3f\n\n\n", stats->area_num_in_q / events->sim_time);
	}
	if(state->without_infinite_queue == 0){
		fprintf(files->outfile, "Blocking Rate: ; %31.3f\n\n\n", erlang_B(state->A, state->number_of_servers));
	}
	
	/* Print the average server utilization. 
	We use this loop to ensure that if area_server_status == 0 then we avoid divisions by zero */
	for (int i = 2; i <= state->number_of_servers + 1; i++ ) {
		if (events->sim_time > 0) {
			fprintf(files->outfile, "Server %d utilization ; %25.3f\n\n", i-1 ,stats->area_server_status[i] / events->sim_time);
		} 
		else { /* To avoid division by zero if events->sim_time > 0 */
			fprintf(files->outfile, "Server %d utilization ; %25.3f \n\n", i-1, 0.0);
    }
	}
	fprintf(files->outfile, "Time simulation ended ; %27.3f ; minutes", events->sim_time);
}

void update_time_avg_stats(SystemState * state, Statistics * stats, EventList * events) {
	
	float time_since_last_event; /* Compute time since last event, and update last-event-time marker. */

	/* This calculation determines the time that has passed since the last processed event.
  It is essential to calculate the areas under the curves of the state variables */
	time_since_last_event = events->sim_time - events->time_last_event;
	events->time_last_event	 = events->sim_time; 
	
	/* This metric is useful to calculate the average number of customers in the queue during the simulation */
	stats->area_num_in_q  += state->num_in_q * time_since_last_event; /* Update area under number-in-queue function. */

	/* Update the area under the server-status function for each server */
	for (int i = 2; i <= state->number_of_servers + 1; ++i) {
    stats->area_server_status[i] += state->server_status[i] * time_since_last_event;
	}
}

void timing(SystemState * state, Statistics * stats, Files * files, EventList * events) {

	/* is a high value that will be used to compare and determine the shortest event time */
	float min_time_next_event = 1.0e+29;

	/* initializes to zero because there is no event selected */
	state->next_event_type = 0;
	
	/* The function loops through all event types and checks the time of each stored event. 
	The event with the shortest time is the next to happen */
	for (int i = 1; i <= state->num_events; ++i) /* event [1] is included in the comparison because it may be the next event */
		if (events->time_next_event[i] < min_time_next_event) {
			min_time_next_event = events->time_next_event[i];
			state->next_event_type = i; /* the index value of the event with the shortest time is updated */
		}

	/* Check to see whether the event list is empty. */
	if (state->next_event_type == 0) { /* if no event was selected it means that the event list is empty so the simulation is stopped */
		/* The event list is empty, so stop the simulation. */
		fprintf(files->outfile, "\nEvent list empty at time %f", events->sim_time);
		exit(1);
	}

	/* The event list is not empty so we advance the simulation clock */
	events->sim_time = min_time_next_event; 
}

void arrive(SystemState * state, Statistics * stats, Files * files, EventList * events, circular_queue * q1) {
	
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
  
		stats->total_of_delays += delay;
  }
	else { /* All servers are occupied */

		if(state->without_infinite_queue == 0) { /* If we don't have a queue -> M/M/n/0 (Erlang-B): Reject customer */
			++stats->lost_customers;
			--state->num_delays_required; ////////////////////////////////7
		}
		else {

			++state->num_in_q; /* We increased the number of users in the waiting list */
			
			/* Check to see whether an overflow condition exists. */
			
			if(!enQ(q1, events->sim_time)) {
				fprintf(files->outfile, "\nOverflow of the array time_arrival at %f", events->sim_time);
			}
		}
	}
}

void depart(SystemState *state, Statistics *stats, EventList *events, circular_queue * q1) {
	
	float delay, arrival_time;

	/* If the queue is empty -> IDLE */
	if (state->num_in_q == 0) {
		/* The queue is empty so make the server idle and eliminate the
		departure (service completion) event from consideration. */
		state->server_status[state->next_event_type] = IDLE; /* The next_event type corresponds to the server index -> see in the timing function */
		events->time_next_event[state->next_event_type] = 1.0e+30; /* Sets the next event to infinite */
	}
	else { /* The queue is not empty */
		
		/* So the number of customers in the queue decreases */
		--state->num_in_q;

		if(!deQ(q1, &arrival_time)) {
			printf("\nUnderflow of the array time_arrival at %f", events->sim_time);
		}

		/* Compute the delay of the customer who is beginning service and update the total delay accumulator. */
		delay = events->sim_time - arrival_time;

		stats->total_of_delays += delay;

		/* Increment the number of customers delayed, and schedule departure. */
		++state->num_custs_delayed;
		events->time_next_event[state->next_event_type] = events->sim_time + expon(state->mean_service, state->streams[state->next_event_type]);
	}
}