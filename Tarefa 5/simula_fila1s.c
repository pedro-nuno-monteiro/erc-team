#include "fila1s.h"
#include "utilits.h"
#include "lcgrand.h"
#include "circular_queue_dynamic.h"

/*! \mainpage
 *
 * Code taken from Simulation and Modelling 5e, Averill M. Law B with some modifications
 * 
 * This code simulates a queueing system - M/M/n/Y, where
 * where:
 * - m: Markovian (Poisson) arrival process,
 * - m: Exponential service time distribution,
 * - n: Number of servers,
 * - Y: Queue capacity (being 0 or infinite).
 * 
 * This simulation may be run with or without an input file. With the input file, the parameters are:
 * 1. Mean interarrival time,
 * 2. Mean service time,
 * 3. Number of customers to be delayed.
 * 4. Number of servers,
 * 5. Queue capacity (0 for infinite queue or any other number for a finite queue).
 * 6. Random seeds for the arrival and departure processes.
 * 7. Queue discipline (0 for FIFO or 1 for LIFO).
 * 
 * Without the input file, the program will ask the user for the parameters.
 * 
 * The simulation outputs the results to a file, with the name given by the user.
 */

/*! Main function that runs the simulation of the M/M/n/Y queueing system.
 *
 * The main function reads input parameters, initializes the simulation,
 * processes events, and outputs results.
 *
 * @return Exit status of the program, EXIT_SUCCESS or EXIT_FAILURE.
 */
int main(int argc, char *argv[]) {

	SystemState state[MAX_SERVERS+1];	/* Structure to hold the system state variables. */
	Statistics stats[MAX_SERVERS+1];		/* Structure to hold the statistical variables. */
	EventList events[MAX_SERVERS+1];		/* Structure to hold the event list variables. */
	Files files;				/* Structure to hold the file pointers. */
	circular_queue q1;	/* Structure to hold the queue. */
	initial_values ini;


	/* Asks for the output file. Open the output file */
	char nome_saida[100];
	while (1) {
		printf("Escreva o ficheiro onde quer escrever o report (com extensao .csv): ");
		scanf("%99s", nome_saida);  /* Reads the file name, limiting it to 99 characters */ 

		/* Checks if the name ends with ".txt" */
		if (strlen(nome_saida) > 4 && strcmp(nome_saida + strlen(nome_saida) - 4, ".csv") == 0) {
			break;  /* Exit the loop if the name ends with ".txt" */
		} else {
			printf("Erro: O nome do ficheiro deve terminar com '.csv'. Tente novamente.\n");
		}
	}

		files.outfile = fopen(nome_saida, "w");
		if (!files.outfile) {
			perror("File opening failed");
			return EXIT_FAILURE;
	}

		int reps = 9;

		/* Check if the input file is provided as an argument. */
		if (argc >= 2) { /* If in the input argument we have the name of the file we want to read */
			receive_input_file(argc, argv, &files, &q1, &ini);
		
		}
		else {
			ask_for_par(&files, &q1, &ini);
		}
	
		/* Prints all the parameters */
		printf("\n\nParameters: \n\n");
		printf("Mean interarrival time: %f\n", ini.mean_interarrival);
		printf("Mean service time: %f\n", ini.mean_service);
		printf("Number of customers: %d\n", ini.num_delays_required);
		printf("Number of servers: %d\n", ini.number_of_servers);
		printf("Number of runs: %d\n", ini.number_of_reps);
		if(ini.without_infinite_queue == 0) printf("Without Queue \n\n");
		else {
			printf("With Queue \n");
			if(q1.dis == 0) printf("FIFO \n\n");
			else printf("LIFO \n\n");
		}



	for(int i=1; i <= ini.number_of_reps; i++){

		/* Call the function and generate the remaining seeds */
		generate_other_streams(&ini, i, &state[i]);

		/* Initialize the simulation. */
		initialize(&state[i], &stats[i], &events[i], state[i].run_streams[0], &q1, &ini);
	
	/* Run the simulation while the required number of customers has not been delayed. */
		while (state[i].num_custs_delayed < ini.num_delays_required) {

			/* Determine the next event (either an arrival or departure). */
			timing(&state[i], &stats[i], &files, &events[i], &ini);
			
		/* Update the time-average statistics based on the time elapsed since the last event. */
			update_time_avg_stats(&state[i], &stats[i], &events[i], &ini);

			/* Process the next event based on its type (1 for arrival, 2 for departure). */
			switch (state[i].next_event_type) {
				case 1:
					arrive(&state[i], &stats[i], &files, &events[i], &q1, &ini);
					break;

				default: /* Se o next_event_type estiver entre 2 e number_of_server+1 */
					depart(&state[i], &stats[i], &events[i], &q1, &ini);
					break;
			}
		}

		/* Invoke the report generator and end the simulation. */

	}

	report(&state, &stats, &files, &events, &q1, &ini);
	
	fclose(files.infile);
	fclose(files.outfile);
	
	return EXIT_SUCCESS;
}
