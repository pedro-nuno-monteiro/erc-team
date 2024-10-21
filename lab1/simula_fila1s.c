
/*! \mainpage
 *
 * Code taken from  Simulation and Modelling 5e, Averill M. Law B with some modifications
 * 
 * This code simulates a singe server queue (FIFO) - M/M/n/\infity
 * 
 * This program requires the input file mm1in.txt with 3 parameters:
 * mean interarrival time, mean service time and the the number of delayed customers
 * 
 */

#include "lcgrand.h"
#include "fila1s.h"


/*! global variables ZZZ */
/*! Once these variables are no longer local, they should be described! */
/*int next_event_type, num_custs_delayed, num_delays_required, num_events,
num_in_q, server_status;
float area_num_in_q, area_server_status, mean_interarrival, mean_service,
sim_time, time_arrival[Q_LIMIT + 1], time_last_event, time_next_event[3],
total_of_delays;*/
// FILE *infile, *outfile;



/*! Main function. */
int main()

{

    SystemState state;
    Statistics stats;
    EventList events;
    Files files;

    int is_ok = EXIT_FAILURE; /*! in sddlib.h */

    /*! Open input file. */

    // trocar para não ser hard coded
    // pedir ao user ou passar como argumento na linha de comando

    files.infile = fopen("mm1in.txt", "r");
    if(!files.infile)
    {
	perror("File opening of mm1in.txt failed");
	return is_ok;
    }
    /*! Open output file. */
    files.outfile = fopen("mm1out.txt", "w");
    if(!files.outfile)
    {
	perror("File opening of mm1out.txt failed");
	return is_ok;
    }

    /*! Specify the number of events for the timing function. */
    int num_events = 2;
    /*! Read input parameters. */
    fscanf(files.infile, "%f %f %d", &state.mean_interarrival, &state.mean_service, &state.num_delays_required);
    /*! Write report heading and input parameters. */
    fprintf(files.outfile, "Single-server queueing system\n\n");
    fprintf(files.outfile, "Mean interarrival time%11.3f minutes\n\n",state. mean_interarrival);
    fprintf(files.outfile, "Mean service time%16.3f minutes\n\n", state.mean_service);
    fprintf(files.outfile, "Number of customers%14d\n\n", state.num_delays_required);
    
    
    // PODE SER APENAS UMA FUNÇÃO QUE É CHAMADA

    /*! Initialize the simulation. */
    initialize(&state, &stats, &events);
    /*! Run the simulation while more delays are still needed. */
    while (state.num_custs_delayed < state.num_delays_required) {
    /*! Determine the next event. */
	    timing(&state, &stats, &files, &events, num_events);
	    /*! Update time-average statistical accumulators. */
	    update_time_avg_stats(&state, &stats, &events);
	    /*! Invoke the appropriate event function. */
	    switch (state.next_event_type) {
		    case 1:
			    arrive(&state, &stats, &files, &events);
		    break;
		    case 2:
			    depart(&state, &stats, &events);
		    break;
	    }
    }
    /*! Invoke the report generator and end the simulation. */
    report(&state, &stats, &files, &events);
    fclose(files.infile);
    fclose(files.outfile);
    return 0;
}
