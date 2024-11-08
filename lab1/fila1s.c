/*! Code from 
 * Simulation and Modelling 5e
 * Averill M. Law B
*/

#include "fila1s.h"
#include "lcgrand.h"

/**função para selecionar um servidor disponivel o estado de cada servidor pode ser IDLE ou BUSY 
   devolve o primeiro servidor livre ou -1 se tiverem todos ocupados */

void ask_streams(SystemState *state){

	



}

int selectFreeServer(SystemState * state) { 
	/**percorrre a lista de servidores*/
	/**server_status--um array que indica on estado de cada servidor (IDLE ou BUSY)
	* num_servers--numero total de servidores no sistema*/
    for (int i = 2; i <= state->number_of_servers+1; ++i) {
        if (state->server_status[i] == IDLE) {
            return i;/**retorna o indice do primeiro servidor diponivel */
        }
    }
    return -1;/**retorna -1 se todos os servidores estiverem ocupados*/
}

float expon(float mean, int stream) {
	return -mean * logf(lcgrand(stream));
}

/* Esta funcao tem como objetivo inicializar o estado do sistema de simulação*/
void initialize(SystemState * state, Statistics * stats, EventList * events, int stream) {
	
	events->sim_time = 0.0; /*! Initialize the simulation clock. */

	/*! Initialize the random number gernerators sequencenes */
	/*! stream 1 for inter-arrival time */
	/*! stream 2 for departure time */
	/*! Initialize the state variables. */

	events->time_next_event[1] = events->sim_time + expon(state->mean_interarrival, stream); /* time_next_event[1] representa o evento de chegada*/
	// events->time_next_event[2] = 1.0e+30;

	/*Inicialize all the servers as IDLE*/
	/*Inicializa os eventos de partida para cada servidor*/
	for (int i = 2; i <= state->number_of_servers + 1; ++i) {
    	state->server_status[i] = IDLE;
		stats->area_server_status[i] = 0.0;
		/*Aqui colocamos i+2 porque começamos em [2] visto que a posiçao [1] é para o evento de chegada*/
		events->time_next_event[i] = 1.0e+30; /** isto são as partidas para cada servidor*/
	}


	state->num_events=state->number_of_servers+1 ; /**Atualização do número de eventos*/

	state->num_in_q = 0; /** Number of custumers in queu start in zero */
	events->time_last_event = 0.0; /* Time of last event starts in zero */
	/*! Initialize the statistical counters. */
	state->num_custs_delayed =	 0; /* Number of custumers */
	stats->total_of_delays	 =	 0.0; /* Number of delays */
	stats->area_num_in_q	 =	 0.0; 
	/*! Initialize event list. Since no customers are present, the departure
	(service completion) event is eliminated from consideration. */
	
}

/* Tem como objetivo escrever os resultados da simulação no ficheiro de saida*/
void report(SystemState * state, Statistics * stats, Files * files, EventList * events) {

	/*! Write report heading and input parameters to the output file. */
	fprintf(files->outfile, "Single-server queueing system\n\n");
	fprintf(files->outfile, "Mean interarrival time%11.3f minutes\n\n", state->mean_interarrival);
	fprintf(files->outfile, "Mean service time%16.3f minutes\n\n", state->mean_service);
	fprintf(files->outfile, "Number of customers%14d\n\n", state->num_delays_required);


	/** Imprime a média do atraso na fila por cliente */
	fprintf(files->outfile, "\n\nAverage delay in queue per client %11.3f minutes\n\n", stats->total_of_delays / state->num_custs_delayed);

	/** Imprime a média do número de clientes que estiveram na fila*/
	fprintf(files->outfile, "Average number of clients in queue %10.3f\n\n", stats->area_num_in_q / events->sim_time);

	/** Imprime a utilização media dos servidores*/
	for (int i = 2; i <= state->number_of_servers + 1; i++ ){
		if (events->sim_time > 0) {
        	fprintf(files->outfile, "Server %d utilization%25.3f\n\n", i-1 ,stats->area_server_status[i] / events->sim_time);
   		 } 
		 
		else { /*Para evitar divisoes por zero se events->sim_time > 0 */
        	fprintf(files->outfile, "Server %d utilization%25.3f \n\n", i-1, 0.0);
    	}
	}


	fprintf(files->outfile, "Time simulation ended%26.3f minutes", events->sim_time);
}

/* Esta funçao é responsavel por atualizar as estatisticas temporais da simulação */
void update_time_avg_stats(SystemState * state, Statistics * stats, EventList * events) {
	
	
	float time_since_last_event; /*! Compute time since last event, and update last-event-time marker. */

	/* Este cálculo determina o tempo que passou desde o último evento processado. Isto é essencial para calcular as áreas sob as curvas das variáveis de estado */
	time_since_last_event = events->sim_time - events->time_last_event;
	events->time_last_event	 = events->sim_time; 
	
	/*Esta métrica é útil para calcular a média do número de clientes na fila durante a simulação */
	stats->area_num_in_q  += state->num_in_q * time_since_last_event; /*! Update area under number-in-queue function. */


	for (int i = 2; i <= state->number_of_servers + 1; ++i)
        stats->area_server_status[i] += state->server_status[i] * time_since_last_event;
	

}

/* Determina o proximo evento com base nos tempos armazenados na lista de evento. Avança o tempo de simulaçao para o tempo do proximo evento
Verifica se a lista de eventos está vazia. Se a fila estiver vazia significa que nao existem mais eventos a serem processados e a simulaçao termina*/
void timing(SystemState * state, Statistics * stats, Files * files, EventList * events) {

	/* é um valor elevado que vai servir para comparar e determinar o menor tempo de evento*/
	float min_time_next_event = 1.0e+29;

	/* inicializa como zero pois nao há evento selecionado*/
	state->next_event_type = 0;
	
	/*! A funcao percorre todos os tipos de evento e verifica o tempo de cada evento armazenado. O evento com menor tempo é o próximo a acontecer */
	for (int i = 1; i <= state->num_events; ++i) /* o evento [1] é incluido na comparaçao porque pode ser o evento seguinte */
		if (events->time_next_event[i] < min_time_next_event) {
			min_time_next_event = events->time_next_event[i];
			state->next_event_type = i; /* é atualizado o valor do indice do evento com menor tempo*/
		}

	/*! Check to see whether the event list is empty. */
	if (state->next_event_type == 0) { /* se nenhum evento foi selecionado significa que a lista de eventos está vazia entao a simulaçao é interrompida*/
		/* The event list is empty, so stop the simulation. */
		fprintf(files->outfile, "\nEvent list empty at time %f", events->sim_time);
		exit(1);
	}
	/* A lista de eventos nao está vazia entao avançamos o relogio de simulaçao*/
	events->sim_time = min_time_next_event; 

}

/* Trata dos clientes que chegam à fila */
void arrive(SystemState * state, Statistics * stats, Files * files, EventList * events, int stream[]) {
	
	float delay;
	
	/* Agenda o proximo evento de chegada */
	events->time_next_event[1] = events->sim_time + expon(state->mean_interarrival,stream[0]); 

	/* Verifica se há servidores livres, se nao houver a funçao retorna -1 */
	int free_server_index = selectFreeServer(state);

	if (free_server_index != -1) { /* Há server livres */
        delay = 0.0; /* O cliente é atendido logo, logo o delay = 0 */
        // stats->total_of_delays += delay;

        // Incrementa o número de clientes atendidos
        ++state->num_custs_delayed;

        // Marca o servidor como ocupado
        state->server_status[free_server_index] = BUSY;

        // Agendar uma partida para este cliente (conclusão de atendimento)
        events->time_next_event[free_server_index] = events->sim_time + expon(state->mean_service, stream[1]);
    }

	else{ /* Todos os servers estao ocupados */
		++state->num_in_q; /*! Aumentamos o numero de utilizadores na fila de espera */
		
		/*! Check to see whether an overflow condition exists. */
		if (state->num_in_q > Q_LIMIT) {
			/*! The queue has overflowed, so stop the simulation. */
			fprintf(files->outfile, "\nOverflow of the array time_arrival at");
			fprintf(files->outfile, " time %f", events->sim_time);
			exit(2);
		}
		/*! There is still room in the queue, so store the time of arrival of the
		arriving customer at the (new) end of time_arrival. */
		state->time_arrival[state->num_in_q] = events->sim_time;

	}
}

void depart(SystemState *state, Statistics *stats, EventList *events, int stream) {
	
	float delay;

	/*! Se a fila estiver vazia */
	if (state->num_in_q == 0) {
		/*! The queue is empty so make the server idle and eliminate the
		departure (service completion) event from consideration. */
		state->server_status[state->next_event_type] = IDLE; /* O next_event type corresponde ao indice do server -> ver na funçao timing*/
		events->time_next_event[state->next_event_type] = 1.0e+30; /* Define o proximo evento como infinito*/
	}
	else { /* A fila nao está vazia*/
		
		/* Entao o numero de clientes na fila diminui*/
		--state->num_in_q;

		/*! Compute the delay of the customer who is beginning service and update the total delay accumulator. */
		delay = events->sim_time - state->time_arrival[1];

		stats->total_of_delays += delay;

		/*! Increment the number of customers delayed, and schedule departure. */
		++state->num_custs_delayed;
		events->time_next_event[2] = events->sim_time + expon(state->mean_service,stream);

		/*! Move each customer in queue (if any) up one place. Com fila circular é muito melhor*/
		for (int i = 1; i <= state->num_in_q; ++i)
			state->time_arrival[i] = state->time_arrival[i + 1];
	}

}