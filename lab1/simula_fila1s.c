#include "lcgrand.h"
#include "fila1s.h"

/*! \mainpage
 *
 * Code taken from  Simulation and Modelling 5e, Averill M. Law B with some modifications
 * 
 * This code simulates a singe server queue (FIFO) - M/M/n/\infity, where
 * where:
 * - m: Markovian (Poisson) arrival process,
 * - m: Exponential service time distribution,
 * - 1: A single server,
 * - \infty: Infinite queue capacity.
 * 
 * This simulation requires an input file `mm1in.txt` containing three parameters:
 * 1. Mean interarrival time,
 * 2. Mean service time,
 * 3. Number of customers to be delayed.
 * 
 * The simulation outputs the results to a file `mm1out.txt`.
 */


/*! Main function that runs the simulation of the M/M/1 queueing system.
 *
 * The main function reads input parameters, initializes the simulation,
 * processes events, and outputs results.
 *
 * @return Exit status of the program, EXIT_SUCCESS or EXIT_FAILURE.
 */
int main() {

	SystemState state;	/*!< Structure to hold the system state variables. */
	Statistics stats;		/*!< Structure to hold the statistical variables. */
	EventList events;		/*!< Structure to hold the event list variables. */
	Files files;				/*!< Structure to hold the file pointers. */

	/*! Set the initial return value to EXIT_FAILURE in case something goes wrong. */
	int is_ok = EXIT_FAILURE;

	/*! Open input file. */

	// trocar para não ser hard coded
	// pedir ao user ou passar como argumento na linha de comando

	files.infile = fopen("mm1in.txt", "r");
	if(!files.infile) {
		perror("File opening of mm1in.txt failed");
		return is_ok;
  }
  
	/*! Open output file. */
	files.outfile = fopen("mm1out.txt", "w");
	if(!files.outfile) {
		perror("File opening of mm1out.txt failed");
		return is_ok;
  }

	/*! Specify the number of event types (arrival and departure) */
	int num_events = 2;

	/*! Read input parameters. */
	fscanf(files.infile, "%f %f %d", &state.mean_interarrival, &state.mean_service, &state.num_delays_required);

	/*! Write report heading and input parameters to the output file. */
	fprintf(files.outfile, "Single-server queueing system\n\n");
	fprintf(files.outfile, "Mean interarrival time%11.3f minutes\n\n",state. mean_interarrival);
	fprintf(files.outfile, "Mean service time%16.3f minutes\n\n", state.mean_service);
	fprintf(files.outfile, "Number of customers%14d\n\n", state.num_delays_required);
	
	// PODE SER APENAS UMA FUNÇÃO QUE É CHAMADA

	/*! Initialize the simulation. */
	initialize(&state, &stats, &events);
	
  /*! Run the simulation while the required number of customers has not been delayed. */
	while (state.num_custs_delayed < state.num_delays_required) {

		/*! Determine the next event (either an arrival or departure). */
		timing(&state, &stats, &files, &events, num_events);
		
    /*! Update the time-average statistics based on the time elapsed since the last event. */
		update_time_avg_stats(&state, &stats, &events);

		/*! Process the next event based on its type (1 for arrival, 2 for departure). */
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
	
	return EXIT_SUCCESS;
}
