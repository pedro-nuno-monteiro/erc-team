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
int main(int argc, char *argv[]) {

	SystemState state;	/*!< Structure to hold the system state variables. */
	Statistics stats;		/*!< Structure to hold the statistical variables. */
	EventList events;		/*!< Structure to hold the event list variables. */
	Files files;				/*!< Structure to hold the file pointers. */

	/*! Set the initial return value to EXIT_FAILURE in case something goes wrong. */
	int is_ok = EXIT_FAILURE;

	srand(time(NULL));

	int indices[3];


	/*! Open input file. */

	if (argc >= 2) {
		
		if(argc==2){
			printf("Usage: %s <%s>\n", argv[0], argv[1]);
			files.infile = fopen(argv[1], "ra");

			if(!files.infile) {
				perror("File opening of mm1in.txt failed");
				return is_ok;
			}
			/*! Read input parameters. */
			fscanf(files.infile, "%f %f %d %d %d %d", &state.mean_interarrival, &state.mean_service, &state.num_delays_required, &indices[0], &indices[1], &indices[2]);
			
			/*! Verifies the validity of the parameters */ 
			if (indices[0] <= 0 || indices[1] <= 0 || indices[2] <= 0 || indices[2] > 100) {
				fprintf(stderr, "Dados de leitura incorretos -> Caracteres invalidos ou numeros negativos\n");
				exit(EXIT_FAILURE);
			}
		}

		else if (argc==4){
			/*! Reads from the console the parameters */
			state.mean_interarrival = atof(argv[1]); 
        	state.mean_service = atof(argv[2]); 
        	state.num_delays_required = atoi(argv[3]);
			/*! Create 3 random seeds without repetition */ 
			for (int count = 0; count < 3; ) {
				int num = rand() % 100 + 1;
				int unique = 1;

				for (int j = 0; j < count; j++) {
					if (indices[j] == num) {
						unique = 0;
						break;
					}
				}
				if (unique) {
					indices[count] = num;
					count++;
				}
			}
			/*! Open input file and write the parameters and the random seeds. */
			files.infile = fopen("mm1in.txt", "w");
			fprintf(files.infile, "%.2f %.2f %d\n", state.mean_interarrival, state.mean_service, state.num_delays_required);
			fprintf(files.infile, "%d %d %d\n", indices[0], indices[1], indices[2]);
		}
		/*! Verifies the validity of the parameters */ 
		if (state.mean_interarrival <= 0 || state.mean_service <= 0 || state.num_delays_required <= 0) {
				fprintf(stderr, "Dados de leitura incorretos -> Caracteres invalidos ou numeros negativos\n");
				exit(EXIT_FAILURE);
		}
		/*! Write in the terminal the parameters */ 
		printf("Mean interarrival time: %f\n", state.mean_interarrival);
		printf("Mean service time: %f\n", state.mean_service);
		printf("Number of customers: %d\n", state.num_delays_required);
	}

	else {
		/*! Ask for parameters and guarantees the validity of those */ 
		do{
			printf("Mean interarrival time -> ");
			if (scanf("%f", &state.mean_interarrival) != 1) {  
            	printf("Por favor, insira um numero positivo.\n");
            	int ch;
            	while ((ch = getchar()) != '\n' && ch != EOF);  // Cleans the buffer
            	state.mean_interarrival = -1;  // Defines an invalid number to repeat the loop
        	}
		} while(state.mean_interarrival<=0);
		do{
			printf("Mean service time -> ");
			if (scanf("%f", &state.mean_service) != 1) {  
            	printf("Por favor, insira um numero positivo.\n");
            	int ch;
            	while ((ch = getchar()) != '\n' && ch != EOF);  // Cleans the buffer
            	state.mean_service = -1;  // Defines an invalid number to repeat the loop
        	}
		}while(state.mean_service<=0);
		do{
			printf("Number of customers -> ");
			if (scanf("%d", &state.num_delays_required) != 1) {  
            	printf("Por favor, insira um numero positivo.\n");
            	int ch;
            	while ((ch = getchar()) != '\n' && ch != EOF);  // Cleans the buffer
            	state.num_delays_required = -1;  // Defines an invalid number to repeat the loop
        	}
		}while(state.num_delays_required<=0);

		/*! Create 3 random seeds without repetition */ 
		for (int count = 0; count < 3; ) {
			int num = rand() % 100 + 1;
			int unique = 1;

			for (int j = 0; j < count; j++) {
				if (indices[j] == num) {
					unique = 0;
					break;
				}
			}
			if (unique) {
				indices[count] = num;
				count++;
			}
		}
		
		/*! Open input file and write the parameters and the random seeds. */
		files.infile = fopen("mm1in.txt", "w");
		fprintf(files.infile, "%.2f %.2f %d\n", state.mean_interarrival, state.mean_service, state.num_delays_required);
		fprintf(files.infile, "%d %d %d\n", indices[0], indices[1], indices[2]);
	}

	/*! Specify the number of event types (arrival and departure) */
	int num_events = 2;

	/*! Open output file. */
	files.outfile = fopen("mm1out.txt", "w");
	if(!files.outfile) {
		perror("File opening of mm1out.txt failed");
		return is_ok;
  }

	/*! Initialize the simulation. */
	initialize(&state, &stats, &events, indices[0]);
	
  /*! Run the simulation while the required number of customers has not been delayed. */
	while (state.num_custs_delayed < state.num_delays_required) {

		/*! Determine the next event (either an arrival or departure). */
		timing(&state, &stats, &files, &events, num_events);
		
    /*! Update the time-average statistics based on the time elapsed since the last event. */
		update_time_avg_stats(&state, &stats, &events);

		/*! Process the next event based on its type (1 for arrival, 2 for departure). */
		switch (state.next_event_type) {
			case 1:
				arrive(&state, &stats, &files, &events, indices[1]);
				break;

			case 2:
				depart(&state, &stats, &events, indices[2]);
				break;
		}
	}

	/*! Invoke the report generator and end the simulation. */
	report(&state, &stats, &files, &events);
	fclose(files.infile);
	fclose(files.outfile);
	
	return EXIT_SUCCESS;
}
