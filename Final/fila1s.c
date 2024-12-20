#include "fila1s.h"
#include "lcgrand.h"
#include "utilits.h"
#include "circular_queue_dynamic.h"

int selectFreeServer(SystemState * state, Statistics * stats, InitialValues *init) { 
	
	int livre = -1; /* At the beginning we assume that no server is IDLE */
	float aux = stats->area_server_status[2]; /* Let's assume that the first server is the one with the lowest utilization rate */ 
	int indice = 2; /* We assume that the first server (index = 2) is the one with the lowest utilization rate */

	/* Loop through each server to find the free server with the lowest usage rate */
	for(int i = 2; i <= init->number_of_servers + 1; ++i) {

		if (state->server_status[i] == IDLE) {
			livre = 1; /* There is at least one free server */

			indice = i; 
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

void initialize(SystemState * state, Statistics * stats, EventList * events, int stream, circular_queue * q1, InitialValues *init) {
	
	/* Initialize the simulation clock. */
	events->sim_time = 0.0;

	/* Initialize the next arrival event time */
	/* time_next_event[1] representa o evento de chegada*/
	events->time_next_event[1] = events->sim_time + expon(init->mean_interarrival, stream);

	/* Initialize all servers as IDLE */
	/* Initialize departure events for each server */
	for(int i = 2; i <= init->number_of_servers + 1; ++i) {
    state->server_status[i] = IDLE;
		stats->area_server_status[i] = 0.0;
		
		/* We use i + 2 here because we start at [2] since position [1] is for the arrival event */
		events->time_next_event[i] = 1.0e+30; /* Set a large value for departure times initially */
	}

	events->time_last_event = 0.0; 	/* No events have occured yet */
	
	state->next_event_type = 0; 		/* No next event initially */
	state->num_custs_delayed = 0; 	/* No costumers delayed initially */
	state->num_in_q = 0; 						/* Number of costumers in queue start in zero */
	state->num_events = init->number_of_servers + 1;

	stats->area_num_in_q = 0.0; 			/* No costumers in queue, so no area under the curve */
	stats->total_of_delays = 0.0; 		/* No delays initially */
	stats->lost_costumers = 0; 				/* No costumers lost initially */
	stats->num_occupied_servers = 0;	/* No server occupied */
	stats->waiting_costumers = 0; 
	stats->real_number_of_costumers_chegados = 0; 
	stats->real_number_of_costumers_partidos = 0;

	/* Initialize event list. Since no costumers are present, the departure
	(service completion) event is eliminated from consideration. */	

	/* Initialize circular queue with FIFO discipline by default*/
	if (q1->dis == 0) {
		inic(q1, FIFO);
	}
	else {
		inic(q1, LIFO);
	}
}

void report(SystemState * state, Statistics * stats, Files * files, EventList * events, circular_queue * q1, InitialValues *init) {

	/* Write report heading and input parameters to the output file */
	fprintf(files->outfile, "Simulation of a queueing system with,%d , servers\n", init->number_of_servers);
	fprintf(files->outfile, "Mean interarrival time, %.2f, minutes\n", init->mean_interarrival);
	fprintf(files->outfile, "Mean service time, %.2f, minutes\n", init->mean_service);
	
	if(init->without_infinite_queue == 0 ) {
		fprintf(files->outfile, "Without Queue\n");
	}
	else {
		fprintf(files->outfile, "With Queue\n");

		if(q1->dis == 0) {
			fprintf(files->outfile, "FIFO discipline\n");
		}
		else {
			fprintf(files->outfile, "LIFO discipline\n");
		}
	}
	fprintf(files->outfile, "\n");

	for(int i = 1; i <= init->number_of_reps; i++) {
		fprintf(files->outfile, ", %da corrida", i);
	}
	fprintf(files->outfile, "\n");

	fprintf(files->outfile, "Number of costumers");
	for(int k = 1; k <= init->number_of_reps; k++) {
		fprintf(files->outfile, ",%d", init->num_delays_required);
	}
	fprintf(files->outfile, ", costumers\n");

	if(init->without_infinite_queue == 0) {
		fprintf(files->outfile, "Average number of costumers delayed");
		for(int k = 1; k <= init->number_of_reps; k++) {
			fprintf(files->outfile, ", %d", abs(init->num_delays_required - stats[k].lost_costumers));
		}
		fprintf(files->outfile, ", costumers\n");
	}

	/* Print the average delay in queue per client */
	if(init->without_infinite_queue) {
		fprintf(files->outfile, "Average delay in queue per client");
		for(int k = 1; k <= init->number_of_reps; k++) {
			fprintf(files->outfile, ",%.3f", stats[k].total_of_delays / state[k].num_custs_delayed);
		}
		fprintf(files->outfile, ", minutes\n");
	}
	
	/* Print the average number of lost clients */
	if(init->without_infinite_queue == 0) {
		fprintf(files->outfile, "Average number of lost clients");
		for(int k = 1; k <= init->number_of_reps; k++) {
			fprintf(files->outfile, ",%d", stats[k].lost_costumers);
		}
	} 
	else {
		/* Print the average number of clients in queue */
		fprintf(files->outfile, "Average number of clients in queue");
		for(int k = 1; k <= init->number_of_reps; k++) {
			fprintf(files->outfile, ",%.3f", stats[k].area_num_in_q / events[k].sim_time);
		}
	}
	fprintf(files->outfile, ", clients\n");

	float blocking_rate[init->number_of_reps];
	float waiting_rate[init->number_of_reps];

	if(init->without_infinite_queue == 0) {
		fprintf(files->outfile, "Blocking rate");
		for(int k = 1; k <= init->number_of_reps; k++) {
			blocking_rate[k] = ((float)stats[k].lost_costumers / state[k].num_custs_delayed);
			fprintf(files->outfile, ",%.4f", blocking_rate[k]);
		}
		fprintf(files->outfile, "\n");
	}
	else {
		fprintf(files->outfile, "Waiting rate");
		for(int k = 1; k <= init->number_of_reps; k++) {
			waiting_rate[k] = ((float)stats[k].waiting_costumers / state[k].num_custs_delayed);
			fprintf(files->outfile, ",%.4f", waiting_rate[k]);
		}
		fprintf(files->outfile, "\n");
	}

	/* Print the average server utilization. 
	We use this loop to ensure that if area_server_status == 0 then we avoid divisions by zero */
	for(int k = 2; k <= init->number_of_servers + 1; k++ ) {

		fprintf(files->outfile, "Server %d utilization", k - 1);
		
		for(int j = 1; j <= init->number_of_reps; j++) {
			if (events[j].sim_time > 0) {
				fprintf(files->outfile, ",%.4f", stats[j].area_server_status[k] / events[j].sim_time);
			} else {
				fprintf(files->outfile, ", 0.0000");
			}
		}
		fprintf(files->outfile, "\n");
	}

	float soma[init->number_of_reps];
	for (int i = 1; i <= init->number_of_reps; i++) {
		soma[i] = 0;
	}
	for(int i = 1; i <= init->number_of_reps; i++) {
		for(int j = 2; j <= init->number_of_servers + 1; j++) {
			soma[i] += stats[i].area_server_status[j] / events[i].sim_time;
		}
	}

	fprintf(files->outfile, "Soma dos servidores");
	for(int j = 1; j <= init->number_of_reps; j++) {
		fprintf(files->outfile, ",%.4f", soma[j]);
	}
	fprintf(files->outfile, "\n");

	fprintf(files->outfile, "Simulation time");
	for (int j = 1; j <= init->number_of_reps; j++) {
		fprintf(files->outfile, ",%.3f", events[j].sim_time);
	}
	fprintf(files->outfile, ", minutes\n");	

	if(init->number_of_reps > 1) {
		fprintf(files->outfile, "\nIntervalos de Confianca\n");
		float intervalo[2];
		intervalo_confianca(soma, init, &intervalo[0], &intervalo[1]);
		fprintf(files->outfile, "Trafego Transportado,");
		fprintf(files->outfile, "[ %.3f %.3f ] \n", intervalo[0], intervalo[1]);
		
		if(init->without_infinite_queue == 0) {
			fprintf(files->outfile, "Blocking rate,");
			intervalo_confianca(blocking_rate, init, &intervalo[0], &intervalo[1]);
		}
		else {
			fprintf(files->outfile, "Waiting rate,");
			intervalo_confianca(waiting_rate, init, &intervalo[0], &intervalo[1]);
		}
		fprintf(files->outfile, "[ %.3f %.3f ] \n", intervalo[0], intervalo[1]);
	}

	fprintf(files->outfile, "\nValores Teoricos");	
	fprintf(files->outfile, "\nTrafego Oferecido , %.3f", init->A);
	
	if(init->without_infinite_queue == 0) {
		float E_B = erlang_B(init->A, init->number_of_servers);
		float A_C = init->A * (1 - E_B);

		fprintf(files->outfile, "\nEarlang - B, %.3f", E_B);
		fprintf(files->outfile, "\nTrafego Transportado, %.3f", A_C);
	}
	else {
		float E_C = erlang_C(init->A, init->number_of_servers);
		float W_e = init->mean_service / (init->number_of_servers - init->A);
		float L_q = W_e * E_C / init->mean_interarrival;

		fprintf(files->outfile, "\nEarlang - C, %.3f", E_C);
		fprintf(files->outfile, "\nW_e, %.3f", W_e);
		fprintf(files->outfile, "\nL_q, %.3f", L_q);
	}
}

void update_time_avg_stats(SystemState * state, Statistics * stats, EventList * events, InitialValues *init) {
	
	float time_since_last_event; /* Compute time since last event, and update last-event-time marker. */

	/* This calculation determines the time that has passed since the last processed event.
    It is essential to calculate the areas under the curves of the state variables */
	time_since_last_event = events->sim_time - events->time_last_event;
	events->time_last_event = events->sim_time; 
	
	/* This metric is useful to calculate the average number of costumers in the queue during the simulation */
	stats->area_num_in_q  += state->num_in_q * time_since_last_event; /* Update area under number-in-queue function. */

	/* Update the area under the server-status function for each server */
	for (int i = 2; i <= init->number_of_servers + 1; ++i) {
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

void arrive(SystemState * state, Statistics * stats, Files * files, EventList * events, circular_queue * q1, InitialValues *init) {
	
	float delay;
	++stats->real_number_of_costumers_chegados;

	/* Schedule the next arrival event */
	events->time_next_event[1] = events->sim_time + expon(init->mean_interarrival, init->streams[0]); 

	/* Checks if there are free servers, if there are not, the function returns -1 */
	int free_server_index = selectFreeServer(state, stats, init);

	if (free_server_index != -1) { /* There are free servers */
		delay = 0.0; /* The costumer is served immediately, so the delay = 0 */

		/* Increases the number of costumers served */
		++state->num_custs_delayed;

		/* Mark the server as busy */
		state->server_status[free_server_index] = BUSY;

		/* Schedule a departure for this costumer (service completion) */
		events->time_next_event[free_server_index] = events->sim_time + expon(init->mean_service, state->run_streams[free_server_index - 1]);
  
		stats->total_of_delays += delay;
  } 
	else { /* All servers are occupied */

		/* If we don't have a queue -> M/M/n/0 (Erlang-B): Reject costumer */
		if(init->without_infinite_queue == 0) {
			++stats->lost_costumers;
		}
		else {

			++stats->waiting_costumers; 

			++state->num_in_q; /* We increased the number of users in the waiting list */
			
			/* Check to see whether an overflow condition exists. */
			if(!enQ(q1, events->sim_time)) {
				fprintf(files->outfile, "\nOverflow of the array time_arrival at %f", events->sim_time);
			}
		}
	}
}

void depart(SystemState *state, Statistics *stats, EventList *events, circular_queue * q1, InitialValues *init) {
	
	float delay, arrival_time;
	stats->real_number_of_costumers_partidos++;

	/* If the queue is empty -> IDLE */
	if (state->num_in_q == 0) {

		/* The queue is empty so make the server idle and eliminate the
		departure (service completion) event from consideration. */
		state->server_status[state->next_event_type] = IDLE; /* The next_event type corresponds to the server index -> see in the timing function */
		events->time_next_event[state->next_event_type] = 1.0e+30; /* Sets the next event to infinite */
	}
	else { /* The queue is not empty */
		
		/* So the number of costumers in the queue decreases */
		--state->num_in_q;

		if(!deQ(q1, &arrival_time)) {
			printf("\nUnderflow of the array time_arrival at %f", events->sim_time);
		}
		
		/* Compute the delay of the costumer who is beginning service and update the total delay accumulator. */
		delay = events->sim_time - arrival_time;

		stats->total_of_delays += delay;

		/* Increment the number of costumers delayed, and schedule departure. */
		++state->num_custs_delayed;

		events->time_next_event[state->next_event_type] = events->sim_time + expon(init->mean_service, state->run_streams[state->next_event_type]);
	}
}