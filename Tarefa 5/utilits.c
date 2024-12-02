#include "fila1s.h"
#include "utilits.h"


int receive_input_file(int argc, char *argv[],  Files *files, circular_queue *q1, InitialValues * init) {

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

		char buffer[256]; 
		while (fgets(buffer, sizeof(buffer), files->infile)) {
			if (buffer[0] != '#') {  // Sai do loop ao encontrar a primeira linha que não começa com '#'
				break;
			}
		}

		/* Read input parameters. */
		
		sscanf(buffer, "%f %f %d", &init->mean_interarrival, &init->mean_service, &init->num_delays_required);
		fscanf(files->infile, "%d %d %d", &init->number_of_reps, &init->number_of_servers, &init->without_infinite_queue);
		fscanf(files->infile, "%d %d", &init->streams[0], &init->streams[1]);

		if(init->without_infinite_queue == 1) {
			fscanf(files->infile, " %d", &q1->dis);
		}

		if(init->number_of_reps <= 0 || init->number_of_reps >= MAX_SERVERS) {
			fprintf(stderr, "O numero de corridas tem de ser inferior a 9\n");
			something_wrong = 1; 
		}

		/* Verifies the validity of the parameters */ 
		if (init->streams[0] <= 0 || init->streams[1] <= 0 || init->streams[0] >= 100 || init->streams[1] >= 100) {
			fprintf(stderr, "Dados de leitura incorretos -> Caracteres invalidos ou numeros negativos\n");
			something_wrong = 1; 
		}

		if(init->streams[0] == init->streams[1]){
			fprintf(stderr, "As sementes nao podem ser iguais\n");
			something_wrong = 1;
		}

		if (init->mean_interarrival <= 0 || init->mean_service <= 0 || init->num_delays_required <= 0 || init->number_of_servers <= 0 ) {
			fprintf(stderr, "Dados de leitura incorretos -> Caracteres invalidos ou numeros negativos\n");
			something_wrong = 1;
		}

		if(init->number_of_servers> MAX_SERVERS) {
			fprintf(stderr, "O numero de server nao respeita as condicoes necessarias\n");
			something_wrong = 1;
		}

		if(init->without_infinite_queue != 0 && init->without_infinite_queue != 1 ) {
			fprintf(stderr, "Dados incorretos:    With Queue = 1      Without Queue = 0 \n");
			something_wrong = 1;
		}

		if(q1->dis != 0 && q1->dis != 1 && init->without_infinite_queue == 1) {
			fprintf(stderr, "Dados incorretos:    FIFO = 0      LIFO = 1 \n");
			something_wrong = 1;
		}

		/* We calculate the offered rate */
		init->A = 1 / init->mean_interarrival * init->mean_service; 
		
		/* We confirm whether the traffic offered is greater than or equal to the number of servers */
		if(init->A >= init->number_of_servers && init->without_infinite_queue == 1) {
			fprintf(stderr, "Quando nao temos fila de espera, o trafego oferecido nao pode ser maior ou igual que o numero de canais. Tem de alterar os valores de Mean_Interarrival e/ou Mean_Service. \n");
			something_wrong = 1;
		}

		if (something_wrong == 1) { /* Just one of the parameters is not correct and the program ends */
			exit(EXIT_FAILURE);	
		}
	}
	return is_ok;
}

void ask_for_par(Files *files, circular_queue *q, InitialValues *init) {

	clear_screen();

	/* Ask for all parameters and guarantees the validity of those */

	do {
		printf("Number of servers -> ");
		
		if (scanf("%d", &init->number_of_servers) != 1) {  
			
			clear_screen();
			
			printf("Por favor, insira um valor positivo.\n");
			int ch;
			while ((ch = getchar()) != '\n' && ch != EOF);
			init->number_of_servers = -1;
		}
	} while(init->number_of_servers <= 0 || init->number_of_servers>MAX_SERVERS);

	do {
		printf("Number of runs -> ");
		
		if (scanf("%d", &init->number_of_reps) != 1) {  
			
			clear_screen();
			
			printf("Por favor, insira um valor positivo.\n");
			int ch;
			while ((ch = getchar()) != '\n' && ch != EOF);
			init->number_of_servers = -1;
		}
	} while(init->number_of_reps <= 0 || init->number_of_reps>MAX_SERVERS);

	do {
		printf("\nWithout Queue = 0  ou Infinite Queue = 1 -> ");

		if (scanf("%d", &init->without_infinite_queue) != 1) {  
			clear_screen();

			printf("Por favor, insira 0 ou 1.\n");
			int ch;
			while ((ch = getchar()) != '\n' && ch != EOF);
			init->num_delays_required = -1;
		}
	} while(init->without_infinite_queue != 1 && init->without_infinite_queue != 0 );

	do {
		
		clear_screen();

		do {
			printf("\nMean interarrival time -> ");

			if (scanf("%f", &init->mean_interarrival) != 1) {

				clear_screen();

				printf("Por favor, insira um valor positivo.\n");
				int ch;
				while ((ch = getchar()) != '\n' && ch != EOF);  // Cleans the buffer
				init->mean_interarrival = -1;  // Defines an invalid number to repeat the loop
			}
		} while(init->mean_interarrival <= 0);

		do {
			printf("\nMean service time -> ");
			
			if (scanf("%f", &init->mean_service) != 1) { 
				clear_screen();

				printf("Por favor, insira um valor positivo.\n");
				int ch;
				while ((ch = getchar()) != '\n' && ch != EOF);
				init->mean_service = -1;
			}
		} while(init->mean_service <= 0);
		
		init->A = 1/init->mean_interarrival * init->mean_service;

		if(init->A >= init->number_of_servers && init->without_infinite_queue == 1) {
			printf("O trafego oferecido nao pode ser maior ou igual que o numero de canais. Tem de alterar os valores de Mean_Interarrival e/ou Mean_Service. \n");
		}
	
	} while(init->A >= init->number_of_servers && init->without_infinite_queue == 1);
		
	clear_screen();

	do {
		printf("Number of delayed customers -> ");
		
		if (scanf("%d", &init->num_delays_required) != 1) { 

			clear_screen();

			printf("Por favor, insira um valor positivo.\n");
			int ch;
			while ((ch = getchar()) != '\n' && ch != EOF);
			init->num_delays_required = -1;
		}
	} while(init->num_delays_required <= 0);

	if(init->without_infinite_queue == 1){
		do {
			printf("Select the discipline (0 for FIFO and 1 for LIFO) -> ");
			
			if (scanf("%d", &q->dis) != 1) { 

				clear_screen();

				printf("Por favor, insira 0 ou 1.\n");
				int ch;
				while ((ch = getchar()) != '\n' && ch != EOF);
				q->dis = -1;
			}
		} while(q->dis != 0 && q->dis != 1);	
	}

	clear_screen();

	ask_streams(init);

	clear_screen();

	/* Open input file and write the parameters and the random seeds. */
	files->infile = fopen("mm1in.txt", "w");

	fprintf(files->infile, "# mean_interarrival | mean_service | num_delays_required\n");
	fprintf(files->infile, "# number_of_reps | number_of_servers | without_infinite_queue\n");
	fprintf(files->infile, "# stream_0 | stream_1\n");
	if(init->without_infinite_queue == 1){
		fprintf(files->infile, "# discipline\n");
	}
	
	fprintf(files->infile, "%.2f %.2f %d\n", init->mean_interarrival, init->mean_service, init->num_delays_required);
	fprintf(files->infile, "%d %d %d\n", init->number_of_reps, init->number_of_servers, init->without_infinite_queue);
	fprintf(files->infile, "%d %d\n", init->streams[0], init->streams[1]);
	if(init->without_infinite_queue == 1){
		fprintf(files->infile, "%d \n", q->dis);
	}
	
}

void ask_streams(InitialValues * init) {

	/* Ask for 2 seeds, the first is for arrivals and the second for departures */
	for(int i = 0; i < 2; i++) {
		do {
			if(i == 0) printf("Escreve a semente correspondente as chegadas: ");
			else printf("Escreve a semente correspondente as partidas: ");
			
			if (scanf("%d", &init->streams[i]) != 1) {  
				printf("Por favor, insira um numero positivo.\n");
				int ch;
				while ((ch = getchar()) != '\n' && ch != EOF); 
				init->streams[i] = -1;
			}

			/* We guarantee that each seed introduced is within the desired values */
			if(init->streams[i] <= 0 || init->streams[i] > 100) {
				printf("As sementes têm de estar compreendidas entre [1, 100]. \n");
			}

			/* This loop ensures that the two seeds are different from each other */
		} while(init->streams[i] <= 0 || init->streams[i] > 100 || (i == 1 && init->streams[0] == init->streams[1]));
	}
}

void generate_other_streams(InitialValues * init, int index, SystemState * state) {


	state->run_streams[0] = init->streams[0] + index - 1; /* Based on the parameter given. It´s minus 1 because the index start at 1 */
	state->run_streams[1] = init->streams[1] + (index-1) * init->number_of_servers; 
	/* Generate seeds for each server. 
	If there are n servers, generate n-1 additional seeds. 
	The first seed corresponds to arrivals and is used for the first server. */
	for(int i = 2; i<=init->number_of_servers + 1; i++) {
		
		/* Generate the next seed by incrementing the previous one. */
		state->run_streams[i] = state->run_streams[i - 1] + 1; 

		/* Ensure seeds are unique by checking against the arrival stream seed.
		Adjust by adding 2 if there's a conflict. */
		if(state->run_streams[i] == state->run_streams[0]) {
			state->run_streams[i] = state->run_streams[i - 1] + 2;
		}

		/* Wrap around if the seed exceeds the upper limit (100). */
		if (state->run_streams[i] == 101) {
			state->run_streams[i] = 1;
		}	
	}

	printf("Streams used for the run %d = [ ", index);

  /* Print all generated seeds for verification. */
	for(int i = 0; i <= init->number_of_servers + 1 ; i++) {

		if(i < init->number_of_servers + 1) {
			printf(" %d, ", state->run_streams[i]);
		}
		else {
			printf(" %d ] \n", state->run_streams[i]);
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

double erlang_B(double A, unsigned int n) { 
	
    double E = 1.0;  /*If n=0: E(A, 0) = 1*/ 

    double AE;
    for (int i = 1; i <= n; i++) {
		AE = A *E;
        E = AE / (AE + i); /*In each iteration i is E(A, i)*/
    }
    return E;
}
