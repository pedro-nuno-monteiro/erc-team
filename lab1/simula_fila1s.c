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

	/*! There are only 2 seeds needes*/
	int sementes[2];

	/*! Check if the input file is provided as an argument. */
	if (argc >= 2) {
		
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
			fscanf(files.infile, "%f %f %d %d %d", &state.mean_interarrival, &state.mean_service, &state.num_delays_required, &sementes[0], &sementes[1]);
			
			/*! Verifies the validity of the parameters */ 
			if (sementes[0] <= 0 || sementes[1] <= 0 || sementes[0] >= 100 || sementes[1] >= 100) {
				fprintf(stderr, "Dados de leitura incorretos -> Caracteres invalidos ou numeros negativos\n");
				exit(EXIT_FAILURE);
			}

			/*! Verifies the validity of the parameters */ 
			if (state.mean_interarrival <= 0 || state.mean_service <= 0 || state.num_delays_required <= 0) {
				fprintf(stderr, "Dados de leitura incorretos -> Caracteres invalidos ou numeros negativos\n");
				exit(EXIT_FAILURE);
			}
		}

		/*! Write the parameters */ 
		printf("Mean interarrival time: %f\n", state.mean_interarrival);
		printf("Mean service time: %f\n", state.mean_service);
		printf("Number of customers: %d\n", state.num_delays_required);
		printf("Streams: %d , %d \n", sementes[0], sementes[1]);

	} else {

		/*! Ask for parameters and guarantees the validity of those */ 
		#ifdef _WIN32
			system("cls");
		#else
			system("clear");
		#endif 
		
		do {
			printf("\nMean interarrival time ([0, 1]) -> ");
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
			
			if(state.mean_interarrival <= 0 || state.mean_interarrival > 1) {
				printf("O tempo medio entre chegadas tem de estar compreendido entre [0, 1]. \n");
			}
		} while(state.mean_interarrival <= 0 || state.mean_interarrival > 1);

		#ifdef _WIN32
			system("cls");
		#else
			system("clear");
		#endif 

		do {
			printf("\nMean service time ([0, 1]) -> ");
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
			
			if(state.mean_service <= 0 || state.mean_service > 1) {
				printf("O tempo medio de servico tem de estar compreendido entre [0, 1]. \n");
			}
		} while(state.mean_service <= 0 || state.mean_service > 1);

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

		/*! Asks for 2 seeds and confirms they're not repited */ 
		for(int i = 0; i < 2; i++) {
			do {
				printf("Escreve a %d semente: ", i + 1);
				if (scanf("%d", &sementes[i]) != 1) {  
					printf("Por favor, insira um numero positivo.\n");
					int ch;
					while ((ch = getchar()) != '\n' && ch != EOF); 
					sementes[i] = -1;
				}

				if(sementes[i] < 0 || sementes[i] > 100) {
					printf("As sementes têm de estar compreendidas entre [0, 100]. \n");
				}
			} while(sementes[i] < 0 || sementes[i] > 100 || (i == 1 && sementes[0] == sementes[1]));
		}

		/*! Open input file and write the parameters and the random seeds. */
		files.infile = fopen("mm1in.txt", "w");
		fprintf(files.infile, "%.2f %.2f %d\n", state.mean_interarrival, state.mean_service, state.num_delays_required);
		fprintf(files.infile, "%d %d\n", sementes[0], sementes[1]);
	}

	/*! Specify the number of event types (arrival and departure) */
	int num_events = 2;

	/*! Open the output file */
	char nome_saida[100];
	while (1) {
		printf("Escreva o ficheiro onde quer escrever o report (com extensao .txt): ");
		scanf("%99s", nome_saida);  // Lê o nome do arquivo, limitando a 99 caracteres

		// Verifica se o nome termina com ".txt"
		if (strlen(nome_saida) > 4 && strcmp(nome_saida + strlen(nome_saida) - 4, ".txt") == 0) {
			break;  // Sai do loop se o nome terminar com ".txt"
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
	initialize(&state, &stats, &events, sementes[0]);
	
  /*! Run the simulation while the required number of customers has not been delayed. */
	while (state.num_custs_delayed < state.num_delays_required) {

		/*! Determine the next event (either an arrival or departure). */
		timing(&state, &stats, &files, &events, num_events);
		
    /*! Update the time-average statistics based on the time elapsed since the last event. */
		update_time_avg_stats(&state, &stats, &events);

		/*! Process the next event based on its type (1 for arrival, 2 for departure). */
		switch (state.next_event_type) {
			case 1:
				arrive(&state, &stats, &files, &events, sementes[0]);
				break;

			case 2:
				depart(&state, &stats, &events, sementes[1]);
				break;
		}
	}

	/*! Invoke the report generator and end the simulation. */
	report(&state, &stats, &files, &events);
	fclose(files.infile);
	fclose(files.outfile);
	
	return EXIT_SUCCESS;
}
