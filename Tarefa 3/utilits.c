#include "fila1s.h"
#include "utilits.h"


int receive_input_file(int argc, char *argv[], SystemState *state, Files *files) {

	/* Set the initial return value to EXIT_FAILURE in case something goes wrong. */
	int is_ok = EXIT_FAILURE;
	int something_wrong = 0; /* Auxiliary variable to know if something went wrong when reading the parameters */
	
	if(argc == 2) {
		printf("Usage: %s <%s>\n", argv[0], argv[1]);
		files->infile = fopen(argv[1], "ra");

		if(!files->infile) {
			printf("File opening of  %s", argv[1]);
			perror("failed");
			return is_ok;
		}

		/* Read input parameters. */
		fscanf(files->infile, "%f %f %d %d %d %d %d", &state->mean_interarrival, &state->mean_service, &state->num_delays_required, &state->number_of_servers, &state->without_infinite_queue, &state->streams[0], &state->streams[1]);
		
		/* Verifies the validity of the parameters */ 
		if (state->streams[0] <= 0 || state->streams[1] <= 0 || state->streams[0] >= 100 || state->streams[1] >= 100) {
			fprintf(stderr, "Dados de leitura incorretos -> Caracteres invalidos ou numeros negativos\n");
			something_wrong = 1; 
		}

		if(state->streams[0] == state->streams[1]){
			fprintf(stderr, "As sementes nao podem ser iguais\n");
			something_wrong = 1;
		}

		if (state->mean_interarrival <= 0 || state->mean_service <= 0 || state->num_delays_required <= 0 || state->number_of_servers <= 0 ) {
			fprintf(stderr, "Dados de leitura incorretos -> Caracteres invalidos ou numeros negativos\n");
			something_wrong = 1;
		}

		if(state->number_of_servers> MAX_SERVERS) {
			fprintf(stderr, "O numero de server nao respeita as condicoes necessarias\n");
			something_wrong = 1;
		}

		if (state->without_infinite_queue != 0 && state->without_infinite_queue != 1 ) {
			fprintf(stderr, "Dados incorretos:    With Queue = 1      Without Queue = 0 \n");
			something_wrong = 1;
		}

		/* We calculate the offered rate */
		state->A = 1 / state->mean_interarrival * state->mean_service; 
		
		/* We confirm whether the traffic offered is greater than or equal to the number of servers */
		if(state->A >= state->number_of_servers) {
			fprintf(stderr, "O trafego oferecido nao pode ser maior ou igual que o numero de canais. Tem de alterar os valores de Mean_Interarrival e/ou Mean_Service. \n");
			something_wrong = 1;
		}

		if (something_wrong == 1) { /* Just one of the parameters is not correct and the program ends */
			exit(EXIT_FAILURE);	
		}
	}
	return is_ok;
}

void ask_for_par(SystemState *state, Files *files) {

	clear_screen();

	/* Ask for all parameters and guarantees the validity of those */

	do {
		printf("Number of servers -> ");
		
		if (scanf("%d", &state->number_of_servers) != 1) {  
			
			clear_screen();
			
			printf("Por favor, insira um numero positivo.\n");
			int ch;
			while ((ch = getchar()) != '\n' && ch != EOF);
			state->number_of_servers = -1;
		}
	} while(state->number_of_servers <= 0 || state->number_of_servers>MAX_SERVERS);

	do {
		
		clear_screen();

		do {
			printf("\nMean interarrival time -> ");

			if (scanf("%f", &state->mean_interarrival) != 1) {

				clear_screen();

				printf("Por favor, insira um numero positivo.\n");
				int ch;
				while ((ch = getchar()) != '\n' && ch != EOF);  // Cleans the buffer
				state->mean_interarrival = -1;  // Defines an invalid number to repeat the loop
			}
			
			if(state->mean_interarrival <= 0 ) {
				printf("O tempo medio entre chegadas nao pode ser negativo. \n");
			}

		} while(state->mean_interarrival <= 0);

		do {
			printf("\nMean service time -> ");
			
			if (scanf("%f", &state->mean_service) != 1) { 
				clear_screen();

				printf("Por favor, insira um numero positivo.\n");
				int ch;
				while ((ch = getchar()) != '\n' && ch != EOF);
				state->mean_service = -1;
			}
			
			if(state->mean_service <= 0) {
				printf("O tempo medio de servico nao pode ser negativo. \n");
			}

		} while(state->mean_service <= 0);
		
		state->A = 1/state->mean_interarrival * state->mean_service;

		if(state->A >= state->number_of_servers){
			printf("O trafego oferecido nao pode ser maior ou igual que o numero de canais. Tem de alterar os valores de Mean_Interarrival e/ou Mean_Service. \n");
		}
	
	} while(state->A >= state->number_of_servers);
		
	clear_screen();

	do {
		printf("Number of customers -> ");
		if (scanf("%d", &state->num_delays_required) != 1) { 

		clear_screen();

			printf("Por favor, insira um numero positivo.\n");
			int ch;
			while ((ch = getchar()) != '\n' && ch != EOF);
			state->num_delays_required = -1;
		}
	} while(state->num_delays_required <= 0);

	clear_screen();

	do {
		printf("Without Queue = 0  ou Infinite Queue = 1 -> ");

		if (scanf("%d", &state->without_infinite_queue) != 1) {  
			clear_screen();

			printf("Por favor, insira um numero positivo.\n");
			int ch;
			while ((ch = getchar()) != '\n' && ch != EOF);
			state->num_delays_required = -1;
		}
	} while(state->without_infinite_queue != 1 && state->without_infinite_queue != 0 );

	ask_streams(state);

	clear_screen();

	generate_other_streams(state);
	
	/* Open input file and write the parameters and the random seeds. */
	files->infile = fopen("mm1in.txt", "w");
	fprintf(files->infile, "%.2f %.2f %d\n", state->mean_interarrival, state->mean_service, state->num_delays_required);
	fprintf(files->infile, "%d %d\n", state->number_of_servers, state->without_infinite_queue);
	fprintf(files->infile, "%d %d\n", state->streams[0], state->streams[1]);
}

void ask_streams(SystemState *state) {

	/* Ask for 2 seeds, the first is for arrivals and the second for departures */
	for(int i = 0; i < 2; i++) {
		do {
			if(i == 0) printf("Escreve a semente correspondente as chegadas: ");
			else printf("Escreve a semente correspondente as partidas: ");
			
			if (scanf("%d", &state->streams[i]) != 1) {  
				printf("Por favor, insira um numero positivo.\n");
				int ch;
				while ((ch = getchar()) != '\n' && ch != EOF); 
				state->streams[i] = -1;
			}

			/* We guarantee that each seed introduced is within the desired values */
			if(state->streams[i] <= 0 || state->streams[i] > 100) {
				printf("As sementes têm de estar compreendidas entre [1, 100]. \n");
			}

			/* This loop ensures that the two seeds are different from each other */
		} while(state->streams[i] <= 0 || state->streams[i] > 100 || (i == 1 && state->streams[0] == state->streams[1]));
	}
}

void generate_other_streams(SystemState *state) {

	/* Generate seeds for each server. 
	If there are n servers, generate n-1 additional seeds. 
	The first seed corresponds to arrivals and is used for the first server. */

	for(int i = 2; i<=state->number_of_servers + 1; i++) {
		
		/* Generate the next seed by incrementing the previous one. */
		state->streams[i] = state->streams[i - 1] + 1; 

		/* Ensure seeds are unique by checking against the arrival stream seed.
		Adjust by adding 2 if there's a conflict. */
		if(state->streams[i] == state->streams[0]) {
			state->streams[i] = state->streams[i - 1] + 2;
		}

		/* Wrap around if the seed exceeds the upper limit (100). */
		if (state->streams[i] == 101) {
			state->streams[i] = 1;
		}	
	}

	printf("Streams = [ ");

  /* Print all generated seeds for verification. */
	for(int i = 0; i <= state->number_of_servers + 1 ; i++) {

		if(i < state->number_of_servers + 1) {
			printf(" %d, ", state->streams[i]);
		}
		else {
			printf(" %d ] \n", state->streams[i]);
		}
	}
}

void clear_screen() {
	#ifdef _WIN32
			system("cls");
	#else
			system("clear");
	#endif
}