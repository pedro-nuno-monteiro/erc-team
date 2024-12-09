#include "fila1s.h"
#include "utilits.h"
#include <math.h>

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

		int dis = 0;

		if(init->without_infinite_queue == 1) {
			fscanf(files->infile, " %d", &dis);
		}

		for(int i = 0; i < init->number_of_reps; i++) {
			q1[i].dis = dis;
		}

		if(init->number_of_reps <= 0 || init->number_of_reps > MAX_SERVERS) {
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

	/* Ask for all parameters and guarantees the validity of those */

	do {
		printf("Number of servers -> ");
		scanf("%d", &init->number_of_servers);

		if (init->number_of_servers <= 0 || init->number_of_servers > MAX_SERVERS) {  
			printf("Por favor, insira um valor valido (1 - %d).\n\n", MAX_SERVERS);
			int ch;
			while ((ch = getchar()) != '\n' && ch != EOF);
			init->number_of_servers = -1;
		}
	} while(init->number_of_servers <= 0 || init->number_of_servers > MAX_SERVERS);

	do {
		printf("Number of runs -> ");
		scanf("%d", &init->number_of_reps);

		if (init->number_of_reps <= 0 || init->number_of_reps > MAX_SERVERS) {  
			
			printf("Por favor, insira um valor valido (1 - %d).\n\n", MAX_SERVERS);
			int ch;
			while ((ch = getchar()) != '\n' && ch != EOF);
			init->number_of_servers = -1;
		}
	} while(init->number_of_reps <= 0 || init->number_of_reps > MAX_SERVERS);

	do {
		printf("Choose - without queue (0) or infinite queue (1) -> ");
		scanf("%d", &init->without_infinite_queue);

		if (init->without_infinite_queue != 1 && init->without_infinite_queue != 0) {  
			printf("Por favor, insira 0 ou 1.\n\n");
			int ch;
			while ((ch = getchar()) != '\n' && ch != EOF);
			init->without_infinite_queue = -1;
		}
	} while(init->without_infinite_queue != 1 && init->without_infinite_queue != 0 );

	do {
		do {
			printf("Mean interarrival time -> ");
			scanf("%f", &init->mean_interarrival);

			if (init->mean_interarrival <= 0) {
				printf("Por favor, insira um valor positivo.\n\n");
				int ch;
				while ((ch = getchar()) != '\n' && ch != EOF); 
				init->mean_interarrival = -1;
			}
		} while(init->mean_interarrival <= 0);

		do {
			printf("Mean service time -> ");
			scanf("%f", &init->mean_service);

			if (init->mean_service <= 0) { 
				printf("Por favor, insira um valor positivo.\n\n");
				int ch;
				while ((ch = getchar()) != '\n' && ch != EOF);
				init->mean_service = -1;
			}
		} while(init->mean_service <= 0);
		
		init->A = 1/init->mean_interarrival * init->mean_service;

		if(init->A >= init->number_of_servers && init->without_infinite_queue == 1) {
			printf("O trafego oferecido nao pode ser maior ou igual que o numero de canais.\nPor favor, altere os valores 'Mean Interarrival' e/ou 'Mean Service'\n\n");
		}
	
	} while(init->A >= init->number_of_servers && init->without_infinite_queue == 1);

	do {
		printf("Number of delayed customers -> ");
		scanf("%d", &init->num_delays_required);

		if(init->num_delays_required <= 0) { 
			printf("Por favor, insira um valor positivo.\n\n");
			int ch;
			while ((ch = getchar()) != '\n' && ch != EOF);
			init->num_delays_required = -1;
		}
	} while(init->num_delays_required <= 0);

	if(init->without_infinite_queue == 1) {
		int dis = -1; 
		do {
			printf("Select the discipline (0 for FIFO and 1 for LIFO) -> ");
			scanf("%d", &dis);
			
			if(dis != 0 && dis != 1) { 
				printf("Por favor, insira 0 ou 1.\n\n");
				int ch;
				while ((ch = getchar()) != '\n' && ch != EOF);
				dis = -1;
			}
		} while(dis != 0 && dis != 1);

		for(int i = 0; i < init->number_of_reps; i++) {
			q[i].dis = dis;
		}
	}

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
			
			scanf("%d", &init->streams[i]);

			if(init->streams[i] <= 0) {  
				printf("Por favor, insira um valor positivo.\n\n");
				int ch;
				while ((ch = getchar()) != '\n' && ch != EOF); 
				init->streams[i] = -1;
			}

			/* We guarantee that each seed introduced is within the desired values */
			if(init->streams[i] <= 0 || init->streams[i] > 100) {
				printf("As sementes tem de estar compreendidas entre [1, 100].\n\n");
			}

			/* This loop ensures that the two seeds are different from each other */
		} while(init->streams[i] <= 0 || init->streams[i] > 100 || (i == 1 && init->streams[0] == init->streams[1]));
	}
}

void generate_other_streams(InitialValues * init, int index, SystemState * state) {

	/* Based on the parameter given. It's minus 1 because the index start at 1 */
	state->run_streams[0] = init->streams[0] + index - 1;
	state->run_streams[1] = init->streams[1] + (index - 1) * init->number_of_servers; 

	if(state->run_streams[1] > 100) {
		state->run_streams[1] = state->run_streams[1] - 100; 
	}

	/* Generate seeds for each server.
	If there are n servers, generate n - 1 additional seeds. 
	The first seed corresponds to arrivals and is used for the first server. */
	for(int i = 2; i <= init->number_of_servers + 1; ++i) {
		
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
	for(int j = 0; j <= init->number_of_servers ; j++) {
		printf("%d%s", state->run_streams[j], (j <= init->number_of_servers - 1) ? ", " : " ]\n");
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

double erlang_C(double A, unsigned int n){

	double E_B, E_C; 

	E_B = erlang_B(A, n); /* Calculate Erlang B value */

	E_C = (n * E_B) / (n - A * (1 - E_B)); /* Apply the Erlang C formula */

	return E_C;

}

void intervalo_confianca(float array[], InitialValues * init, float *inferior, float *superior) {

	float value;

	/* Calculate the sum of all elements in the array */
	float soma = 0, media, soma_quadrado = 0, variancia, desvio_padrao; 
	for(int i = 1; i <= init->number_of_reps; i++) {
		soma += array[i];
	}

	/* Calculate the mean */
	media = soma / init->number_of_reps; 

	/* Calculate the sum of squared deviations from the mean */
	for(int i = 1; i <= init->number_of_reps; i++) {
		soma_quadrado += (array[i] - media) * (array[i] - media);
	}

	/* Calculate variance and standard deviation */
	variancia = soma_quadrado / (init->number_of_reps - 1);
	desvio_padrao = sqrt(variancia);

	FILE *file;

	/* Open the file containing t-Student values for 95% confidence level */
	file = fopen("t_student_95.txt", "r");
	if (file == NULL) {
		fprintf(stderr, "Erro ao abrir o ficheiro t_student_95.txt .\n");
		exit(EXIT_FAILURE);
	}

	/* Read t-Student values from the file until the 'number_of_reps - 1' */
	for (int i = 1; i < init->number_of_reps; i++) {
		if (fscanf(file, " %f", &value) != 1) {
			fprintf(stderr, "Erro ao ler o valor no indice %d do ficheiro\n", i);
			fclose(file);
			exit(EXIT_FAILURE);
		}
	}

	float aux; 

	/* Calculate the margin of error */
	aux = desvio_padrao / sqrt(init->number_of_reps) * value;
	
	/* Calculate the lower and upper bounds of the confidence interval */
	*inferior = media - aux;  
	*superior = media + aux;
}