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


	int something_wrong = 0; /* Auxiliary variable to know if something went wrong when reading the parameters */

	/*! Check if the input file is provided as an argument. */
	
	if (argc >= 2) { /* If in the input argument we have the name of the file we want to read */
		
		/*! Open input file and read the parameters. */
		if(argc == 2) {
			printf("Usage: %s <%s>\n", argv[0], argv[1]);
			files.infile = fopen(argv[1], "ra");

			if(!files.infile) {
				printf("File opening of  %s", argv[1]);
				perror(" failed");
				return is_ok;
			}

			/*! Read input parameters. */
			fscanf(files.infile, "%f %f %d %d %d %d %d", &state.mean_interarrival, &state.mean_service, &state.num_delays_required, &state.number_of_servers, &state.without_infinite_queue, &state.streams[0], &state.streams[1]);
			
			/*! Verifies the validity of the parameters */ 
			if (state.streams[0] <= 0 || state.streams[1] <= 0 || state.streams[0] >= 100 || state.streams[1] >= 100) {
				fprintf(stderr, "Dados de leitura incorretos -> Caracteres invalidos ou numeros negativos\n");
				something_wrong = 1; 
			}

			if(state.streams[0] == state.streams[1]){
				fprintf(stderr, "As sementes nao podem ser iguais\n");
				something_wrong = 1;
			}

			if (state.mean_interarrival <= 0 || state.mean_service <= 0 || state.num_delays_required <= 0 || state.number_of_servers <= 0 ) {
				fprintf(stderr, "Dados de leitura incorretos -> Caracteres invalidos ou numeros negativos\n");
				something_wrong = 1;
			}

			if(state.number_of_servers> MAX_SERVERS){
				fprintf(stderr, "O numero de server nao respeita as condicoes necessarias\n");
				something_wrong = 1;
			}

			if (state.without_infinite_queue != 0 && state.without_infinite_queue != 1 ) {
				fprintf(stderr, "Dados incorretos:    With Queue = 1      Without Queue = 0 \n");
				something_wrong = 1;
			}

			/* We calculate the offered rate */
			state.A = 1/state.mean_interarrival * state.mean_service; 
			
			/* We confirm whether the traffic offered is greater than or equal to the number of servers */
			if(state.A >= state.number_of_servers){
				fprintf(stderr, "O trafego oferecido nao pode ser maior ou igual que o numero de canais. Tem de alterar os valores de Mean_Interarrival e/ou Mean_Service. \n");
				something_wrong = 1;
			}

			if (something_wrong == 1){ /* Just one of the parameters is not correct and the program ends */
				exit(EXIT_FAILURE);	
			}
		}

		/* Call the function and generate the remaining seeds */
		generate_other_streams(&state);

	} else {
	 
		#ifdef _WIN32
			system("cls");
		#else
			system("clear");
		#endif 

		/*! Ask for parameters all parameters and guarantees the validity of those */

			do {
				printf("Number of servers -> ");
				if (scanf("%d", &state.number_of_servers) != 1) {  
				#ifdef _WIN32
					system("cls");
				#else
					system("clear");
				#endif
					printf("Por favor, insira um numero positivo.\n");
					int ch;
					while ((ch = getchar()) != '\n' && ch != EOF);
					state.number_of_servers = -1;
				}
			} while(state.number_of_servers <= 0 || state.number_of_servers>MAX_SERVERS);


		do{
			#ifdef _WIN32
				system("cls");
			#else
				system("clear");
			#endif 

			do {
				printf("\nMean interarrival time -> ");
				if (scanf("%f", &state.mean_interarrival) != 1) {

					#ifdef _WIN32
						system("cls");
					#else
						system("clear");
					#endif 

					printf("Por favor, insira um numero positivo.\n");
					int ch;
					while ((ch = getchar()) != '\n' && ch != EOF);  // Cleans the buffer
					state.mean_interarrival = -1;  // Defines an invalid number to repeat the loop
				}
				
				if(state.mean_interarrival <= 0 ) {
					printf("O tempo medio entre chegadas nao pode ser negativo. \n");
				}

			} while(state.mean_interarrival <= 0);

			do {
				printf("\nMean service time -> ");
				if (scanf("%f", &state.mean_service) != 1) { 
					#ifdef _WIN32
						system("cls");
					#else
						system("clear");
					#endif

					printf("Por favor, insira um numero positivo.\n");
					int ch;
					while ((ch = getchar()) != '\n' && ch != EOF);
					state.mean_service = -1;
				}
				
				if(state.mean_service <= 0) {
					printf("O tempo medio de servico nao pode ser negativo. \n");
				}

			} while(state.mean_service <= 0);
			
			state.A = 1/state.mean_interarrival * state.mean_service;

			if(state.A >= state.number_of_servers){
				printf("O trafego oferecido nao pode ser maior ou igual que o numero de canais. Tem de alterar os valores de Mean_Interarrival e/ou Mean_Service. \n");
			}
		
		} while(state.A >= state.number_of_servers);
		

		#ifdef _WIN32
			system("cls");
		#else
			system("clear");
		#endif 


		do {
			printf("Number of customers -> ");
			if (scanf("%d", &state.num_delays_required) != 1) { 

			#ifdef _WIN32
				system("cls");
			#else
				system("clear");
			#endif

				printf("Por favor, insira um numero positivo.\n");
				int ch;
				while ((ch = getchar()) != '\n' && ch != EOF);
				state.num_delays_required = -1;
      		}
		} while(state.num_delays_required <= 0);

		#ifdef _WIN32
			system("cls");
		#else
			system("clear");
		#endif 

	

		do {
			printf("Without Queue = 0  ou Infinite Queue = 1 -> ");
			if (scanf("%d", &state.without_infinite_queue) != 1) {  
			#ifdef _WIN32
				system("cls");
			#else
				system("clear");
			#endif
				printf("Por favor, insira um numero positivo.\n");
				int ch;
				while ((ch = getchar()) != '\n' && ch != EOF);
				state.num_delays_required = -1;
      }
		} while(state.without_infinite_queue != 1 && state.without_infinite_queue != 0 );

		ask_streams(&state);

		#ifdef _WIN32
			system("cls");
		#else
			system("clear");
		#endif 

		generate_other_streams(&state);
		
		/*! Open input file and write the parameters and the random seeds. */
		files.infile = fopen("mm1in.txt", "w");
		fprintf(files.infile, "%.2f %.2f %d\n", state.mean_interarrival, state.mean_service, state.num_delays_required);
		fprintf(files.infile, "%d %d\n", state.number_of_servers, state.without_infinite_queue);
		fprintf(files.infile, "%d %d\n", state.streams[0], state.streams[1]);
	}

	/* Prints all the parameters */
	printf("Mean interarrival time: %f\n", state.mean_interarrival);
	printf("Mean service time: %f\n", state.mean_service);
	printf("Number of customers: %d\n", state.num_delays_required);
	printf("Number of servers: %d\n", state.number_of_servers);

	if(state.without_infinite_queue == 0){
		printf("Without Queue \n");
	}
	else{
		printf("Infinite Queue \n");
	}


	/*! Asks for the output file. Open the output file */
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


	/*! Initialize the simulation. */
	initialize(&state, &stats, &events, state.streams[0]);
	
  /*! Run the simulation while the required number of customers has not been delayed. */
	while (state.num_custs_delayed < state.num_delays_required) {

		/*! Determine the next event (either an arrival or departure). */
		timing(&state, &stats, &files, &events);
		
    /*! Update the time-average statistics based on the time elapsed since the last event. */
		update_time_avg_stats(&state, &stats, &events);

		/*! Process the next event based on its type (1 for arrival, 2 for departure). */
		switch (state.next_event_type) {
			case 1:
				arrive(&state, &stats, &files, &events);
				break;

			default: /* Se o next_event_type estiver entre 2 e number_of_server+1 */
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
