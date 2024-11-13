#include "fila1s.h"
#include "utilits.h"
#include "lcgrand.h"

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
int main(int argc, char *argv[]) {

	SystemState state;	/* Structure to hold the system state variables. */
	Statistics stats;		/* Structure to hold the statistical variables. */
	EventList events;		/* Structure to hold the event list variables. */
	Files files;				/* Structure to hold the file pointers. */

	/* Check if the input file is provided as an argument. */
	if (argc >= 2) { /* If in the input argument we have the name of the file we want to read */
		receive_input_file(argc, argv, &state, &files);
	
		/* Call the function and generate the remaining seeds */
		generate_other_streams(&state);
	}
	else {
		ask_for_par(&state, &files);
	}

	/* Prints all the parameters */
	printf("Mean interarrival time: %f\n", state.mean_interarrival);
	printf("Mean service time: %f\n", state.mean_service);
	printf("Number of customers: %d\n", state.num_delays_required);
	printf("Number of servers: %d\n", state.number_of_servers);

	if(state.without_infinite_queue == 0) printf("Without Queue \n");
	else printf("Infinite Queue \n");

	/* Asks for the output file. Open the output file */
	char nome_saida[100];
	while (1) {
		printf("Escreva o ficheiro onde quer escrever o report (com extensao .txt): ");
		scanf("%99s", nome_saida);  /* Reads the file name, limiting it to 99 characters */ 

		/* Checks if the name ends with ".txt" */
		if (strlen(nome_saida) > 4 && strcmp(nome_saida + strlen(nome_saida) - 4, ".txt") == 0) {
			break;  /* Exit the loop if the name ends with ".txt" */
		} else {
			printf("Erro: O nome do ficheiro deve terminar com '.txt'. Tente novamente.\n");
		}
  }

	files.outfile = fopen(nome_saida, "w");
    if (!files.outfile) {
			perror("File opening failed");
			return EXIT_FAILURE;
  }

	/* Initialize the simulation. */
	initialize(&state, &stats, &events, state.streams[0]);
	
  /* Run the simulation while the required number of customers has not been delayed. */
	while (state.num_custs_delayed < state.num_delays_required) {

		/* Determine the next event (either an arrival or departure). */
		timing(&state, &stats, &files, &events);
		
    /* Update the time-average statistics based on the time elapsed since the last event. */
		update_time_avg_stats(&state, &stats, &events);

		/* Process the next event based on its type (1 for arrival, 2 for departure). */
		switch (state.next_event_type) {
			case 1:
				arrive(&state, &stats, &files, &events);
				break;

			default: /* Se o next_event_type estiver entre 2 e number_of_server+1 */
				depart(&state, &stats, &events);
				break;
		}
	}

	/* Invoke the report generator and end the simulation. */
	report(&state, &stats, &files, &events);
	fclose(files.infile);
	fclose(files.outfile);
	
	return EXIT_SUCCESS;
}
