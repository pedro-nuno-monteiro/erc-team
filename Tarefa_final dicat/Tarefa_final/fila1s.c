#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lcgrand.h"
#include "fila_circular.h"

#define Q_LIMIT 200
#define BUSY 1
#define IDLE 0
#define INFINITO 1e+30

typedef struct {
	/**Variaveis-TEMPO*/
    float sim_time; /**tempo atual da simulação*/
    float time_last_event; /**armazena o tempo do ultimo evento ocorrido*/
    
    /**Variaveis-Servidor*/
    int server_status[12]; /**Tabela que indica o estado de cada servidor (IDLE ou BUSY)*/
    float time_next_event[12];/**Tabela que armazena o tempo do prox servidor*/
    float mean_service;/**media de tempo de serviço */
    int num_servers; /**Nº total de servidores no sistema*/
    
    /**Variaveis-Clientes*/
    int lost_customers; /**conta o nºde clientes perdidos */
    float mean_interarrival;/**Média do tempo entre chegadas dos clientes*/
    float area_num_in_q;/**Aramezena a area do nº de clientes na fila */
    int num_custs_delayed; /**conta o nºde clientes que foram atendidos*/
    float total_of_delays; /**soma de total de atrasos de todos os clientes perdidos*/
    int num_events; /**Nº de eventos, é dinamico pois depende dos clientes*/
    float time_arrival[Q_LIMIT]; /**Tabela que armazena os tempos de chegada dos clientes na fila usando with_queue=1*/
   
    /**Variaveis-Estatisticas*/
    int area_server_status[12];/**Tabela que armazena a area */
    int next_event_type; /*!<0 - a lista de eventos esta vazia; 1 é uma chegada; 2 ate num_srvers+1 é uma partida desse servidor */
	int idseq[12]; /**Tabela que aramazena os indentificadores de sequencia para a geração de variaveis aleatorias */
    int num_delays_required; /**Nº total de clientes que devem ser atendidos antes da execução do codigo terminar */
	int with_queue; /**Indica se há fila (1-sim, 0-não)*/

    myQueue circularQueue; /**Fila circular*/
    
} QueueSystem;

float expon(float mean, int stream);
int selectFreeServer(const QueueSystem *system);
void initialize(QueueSystem *system);
void report(const QueueSystem *system, FILE *outfile);
void update_time_avg_stats(QueueSystem *system);
void timing(QueueSystem *system);
void arrive(QueueSystem *system);
void depart(QueueSystem *system);




/** Retorna uma variavel aleatoria exponencial com media "mean" */ 
float expon(float mean, int stream) { 
	/** Função para gerar uma variavel exponencial*/
	return -mean * log(lcgrand(stream)); 
}


/**função para selecionar um servidor disponivel
o estado de cada servidor ou IDLE ou BUSY 
* \return devolve o primeiro servidor livre ou -1 se tiverem todos ocupados */
int selectFreeServer(const QueueSystem *system) { 
	/**percorrre a lista de servidores*/
	/**server_status--um array que indica on estado de cada servidor (IDLE ou BUSY)
	* num_servers--numero total de servidores no sistema*/
    for (int i = 2; i <= system->num_servers+1; ++i) {
        if (system->server_status[i] == IDLE) {
            return i;/**retorna o indice do primeiro servidor diponivel */
        }
    }
    return -1;/**retorna -1 se todos os servidores estiverem ocupados*/
}

/**a tabela server_status só é usada no indice 2 até num_servers+1 (se o numero de servidores for 1 só é usado o indice 2)
//time_next_event indice zero indica que não há mais eventos
//indice 1 indica uma chegada 
//indice 2 até num_servers+1 indica uma partida nesse servidor se o time_next_event nesse indice for diferente de infinito*/
void initialize(QueueSystem *system) { 
	
	/**inicializa as variaveis com valores iniciais*/
    system->sim_time = 0.0;/**tempo de simulação começa a zero*/
    system->lost_customers = 0; /**os clientes na fila começam a zero */
    system->time_last_event = 0.0;/**tempo do ultimo evento começa a zero*/ 
    system->num_custs_delayed = 0;/**numero de clientes atendidos começa a zero*/
    system->total_of_delays = 0.0;/**o total de atrasos começa a zero */
    system->num_events=system->num_servers+1 ; /**Atualização do número de eventos*/
	
	/**Inicializa a fila circular*/
    inicQueue(&(system->circularQueue));
     
     
	system->time_next_event[0]=INFINITO; 
    /**estado inicial dos servidores como IDLE*/
    for (int i = 2; i <= system->num_servers+1; ++i) {
        system->server_status[i] = IDLE;
        system->time_next_event[i] = INFINITO; /** isto são as partidas */
        system->area_server_status[i]=0.0;
    }
    /**inicializa as sementes a usar para chegadas (1) e tempos de serviço (2 ate num_servers+1) zero n é usado*/
    system->idseq[1]=1; 
    for (int i = 2; i <= system->num_servers+1; ++i) { 
        system->idseq[i] = i;
    }
     /**gera a primeira chegada*/
	system->time_next_event[1] = system->sim_time + expon(system->mean_interarrival, system->idseq[1]);
	for (int i=0; i<=system->num_servers+1;i++)
	{
		fprintf(stderr, "\ninitialize; system->time_next_event[%d] = %f", i, system->time_next_event[i]);
	}
		
	
}

void report(const QueueSystem *system, FILE *outfile) {
/** relatorio do desempenho do sistema de fila*/
	fprintf(outfile, "\n\nAverage delay in queue%11.3f minutes\n\n",/** imprime a media do atraso na fila por cliente*/
	system->total_of_delays / system->num_custs_delayed);
	fprintf(outfile, "Average number in queue%10.3f\n\n",/**imprime a media do numero de clientes na fila*/
	system->area_num_in_q / system->sim_time);
	for (int i=2; i<=system->num_servers+1; i++ ){
		fprintf(outfile, "Server utilization%15.3f\n\n",/**imprime a utilização media dos servidores*/
		system->area_server_status[i] / system->sim_time);
	}
	fprintf(outfile, "Time simulation ended%12.3f minutes\n", /** imprime o tempo total da simulação.*/
	system->sim_time);
	fprintf(outfile, "Lost customers%16d\n\n", /**imprime o numero de clientes perdidos*/
	system->lost_customers);
}
/**Função que atualiza ass estatisticas do codigo */
void update_time_avg_stats(QueueSystem *system)
{
	float time_since_last_event = system->sim_time - system->time_last_event;/**tempo decorrido desde do ultimo evento*/
	/** atualiza o tempo do ultimo evento para o tempo atual*/
	system->time_last_event = system->sim_time;
	
	/**se o sistema tem fila de espera atualiza a area da função number_queue permite calcular o n de utilizadores na fila de espera*/
    if(system->with_queue==1) system->area_num_in_q += getSize(&system->circularQueue) * time_since_last_event;
	/**atualiza a area media ponderada do numero de clientes na fila para cada servidor*/
    for (int i = 2; i <= system->num_servers+1; ++i)
        system->area_server_status[i] += system->server_status[i] * time_since_last_event;
}

void timing(QueueSystem *system) {
	/**determinar o prox evento e avanca o tempo da simulacao*/
    int i;
    float min_time_next_event = INFINITO-1000;  /**experimentar depois só com infinito*/


    system->next_event_type =0; /**caso em que a fila está vazia */

    for (i = 1; i <= system->num_events; ++i)
        if (system->time_next_event[i] < min_time_next_event) {
            min_time_next_event = system->time_next_event[i];
            system->next_event_type = i; /**identifica o servidor a libertar para saber na função depart (é o que tem menor tempo)*/
        }
    
    /**fprintf(stderr, "\nTimming: system->next_event_type==%d", system->next_event_type);
    
    if (system->server_status[system->next_event_type]>=2){
		fprintf(stderr, "\nTimming: system->server_status[system->next_event_type]==%d", system->server_status[system->next_event_type]);
    }
 	*/	
        
	/** Verifica se a lista de eventos esta vazia. */
    if (system->next_event_type == 0) {
		/** A lista de eventos está vazia, então a simulação para. */
        fprintf(stderr, "\nEvent list empty at time %f", system->sim_time);
        exit(1);
    }
	/**Se a lista de eventos nao esta vazia então a simulacao para */
	/**avanca o tempo da simulacao para o tempo do prox evento*/
    system->sim_time = min_time_next_event; /**Para calcular e atualizar as estatisticas de tempo medio relacionadas ao nº de clientes na fila e estado dos servidores*/
    
}




void arrive(QueueSystem *system) {
        float delay;
        /**idseq[1] está o indentificadir da sequencia usada para as chegadas */
        system->time_next_event[1] = system->sim_time + expon(system->mean_interarrival, system->idseq[1]); 
        /**Verificar se o servidor está ocupado*/
        /**Usando a função select_server que verifica se existe um servidor não ocupado 
        //se o resultado for -1 então os servidotres estão todos ocupados 
        //logo o cliente não é logo atendido
         //if (select_server(server_status, *num_servers) == -1) {*/
		int selected_server = selectFreeServer(system);
		if (selected_server == -1) {
                /**Se ele estiver ocupado 
                 * adiciona mais um cliente à fila circular**/
                if (system->with_queue )
                {
					if( -1 == enQueue(&(system->circularQueue), system->sim_time)) {
						fprintf(stderr, "\nOverflow of the array time_arrival at time %f", system->sim_time);
						exit(2);
					}
				}
				else {
					/**incrementa o numero de chamadas perdidas*/ 
					system->lost_customers++;
				}

					 
        }
        else {
			/**Servidor estiver Disponivel
			 * o atraso do cliente começa a zero
			 * seleciona o servidor disponivel
			 * se o servidor que for ocupado, passa a estar ocupado */
			delay = 0.0;
			system->total_of_delays += delay;
			++system->num_custs_delayed;
			system->server_status[selected_server] = BUSY;
			system->time_next_event[selected_server] = system->sim_time + expon(system->mean_service, system->idseq[selected_server]);
	 }
	 
               
 }


/**função responsavel por processar o cliente que sai da fila*/
void depart(QueueSystem *system)
{
	/**Teste Debug-- Mensagens de ERRO*/
	if (system->next_event_type==1){
		fprintf(stderr, "\nErro em depart pois entrou com next_event_type==1(chegada)");
		exit(-1);  
	}
	if (system->next_event_type> system->num_servers+1){
		fprintf(stderr, "\nErro em depart pois pretende libertar servidor inexistente %d", system->next_event_type);
		exit(-1); 
	}
	if (system->server_status[system->next_event_type]== IDLE){
		fprintf(stderr, "\nErro em depart pois pretende libertar servidor desocupado %d", system->next_event_type);
		exit(-1);  
	}
	
    float delay;
	int busy_server = system->next_event_type; /**variavel auxiliar*/
	/** Verifica se a fila esta vazia */
	if (checkEmpty(&(system->circularQueue))|| system->with_queue!=1) { /**fila vazia ou sistema com perda */
		/**se tem perda acabou (não faz mais nada)*/ 
		/** Se não há clientes à espera  significa q o servidor concluiu o serviço  
		 * passa a estar desocupado */
		system->server_status[busy_server] = IDLE;
		system->time_next_event[busy_server] = INFINITO;
    } 
    else {
		/**vem para aqui se for um sistema com fila e esta não estiver vazia*/
		float front_time;
		/**int inf= para testar se correu bem*/
		deQueue(&system->circularQueue, &front_time); /**front_time corresponde ao time_arrival[1] na fila não circular*/

		/**calcula o atraso do cliente q esta a iniciar o serviço e atualiza o contador de atraso*/
	    delay = system->sim_time - front_time;
        system->total_of_delays += delay;
		/** aumenta o numero de clientes atrasados e agenda a saida */
		++(system->num_custs_delayed);
		/**antes era sempre 2 agora é do 2 ate ao num_server+1 */
		system->time_next_event[busy_server] = system->sim_time+expon(system->mean_service, system->idseq[busy_server]); 
		
    }
 }


int main() 
{
	int menuChoice;
	QueueSystem system;
	FILE *outfile = NULL; 
 
    /**zero n é usado 
    a capacidade minima deve de ser smp num_serveres+1
    uma vez que vamos precisar de ter eventos por servidor mais um para as chegadas, podemos usar um truque seguinte:
    time_next_event[0... numservers -1] tem o instante de partida desse servidor ou um valor muito elevado "infinito " (chamamos uma constante omeuinfinito=biblioteca-->climits=floats=FLT_MAX (incluir biblioteca))
    time_next_event[numserveers] passa ser a chegada seguinte  */   

    do {
        printf("\nMenu:\n");
        printf("1. Definir mean_interarrival\n");
        printf("2. Definir mean_service\n");
        printf("3. Definir num_delays_required\n");
        printf("4. Definir num_servers\n");
        printf("5. Definir with_queue (1 para sim, 0 para não)\n");
        printf("6. Executar simulação\n");
        printf("0. Sair\n");

        printf("Escolha: ");
        scanf("%d", &menuChoice);

        switch (menuChoice) {
            case 1:
                printf("Digite mean_interarrival: ");
                scanf("%f", &system.mean_interarrival);
                break;
            case 2:
                printf("Digite mean_service: ");
                scanf("%f", &system.mean_service);
                break;
            case 3:
                printf("Digite num_delays_required: ");
                scanf("%d", &system.num_delays_required);
                break;
            case 4:
                printf("Digite num_servers: ");
                scanf("%d", &system.num_servers);
                break;
            case 5:
                printf("Digite with_queue (1 para sim, 0 para não): ");
                scanf("%d", &system.with_queue);
                break;
            case 6: 
                outfile = fopen("mm1out.txt", "w");
                if (outfile == NULL) {
                    printf("Erro ao abrir o arquivo mm1out.txt\n");
                    return 1; 
                }
                /** Modificacoes para lidar com os multiplos servidores*/
				fprintf(outfile, "Single-server queueing system\n\n");
				fprintf(outfile, "Mean interarrival time%11.3f minutes\n\n", system.mean_interarrival);
				fprintf(outfile, "Mean service time%16.3f minutes\n\n", system.mean_service);
				fprintf(outfile, "Number of customers%14d\n\n", system.num_delays_required);
				fprintf(outfile, "Number of servers%12d\n\n", system.num_servers);
				fprintf(outfile, "Queueing system with %s\n\n", (system.with_queue == 1) ? "infinite queue" : "no queue");
				
				/**Inicilaiza as variaveis do sistema*/
				initialize(&system); 

				/**Vamos usar um while para percorrer todos os clientes atendidos ate atingir o numero de clientes pretendidos	*/
				while (system.num_custs_delayed < system.num_delays_required) {
				
				/**Verificar se há eventos agendados*/
				int events_scheduled = 0;
				for (int i = 2; i <= system.num_servers + 1; ++i) {
					if (system.time_next_event[i] != INFINITO) {
						events_scheduled = 1;
						break;
					}
				}

			   
				if (!events_scheduled) {
					fprintf(outfile, "Não há eventos agendados. \n");
					break;
				}
				timing(&system);
				update_time_avg_stats(&system);
				/**se o proximo evento for uma chegada */
				if (system.next_event_type == 0) {
					  fprintf(stderr, "Isto não devia ter acontecido! lista de eventos vazia/n");
						exit(1);
				}
				else if (system.next_event_type == 1) {
						arrive(&system);
					}
					/**se o proximo evento for uma saida */
					else if (system.next_event_type >=2 && system.next_event_type <= system.num_servers+1) {
						depart(&system);
					}
					/**se n for uma saida ou uma chegada então ERRO*/
					else {
						fprintf(stderr, "Isto não devia ter acontecido! next_event_type=%d quando num_servers=%d/n", system.next_event_type, system.num_servers);
						exit(1);
					}
				}
				/** Gera o relatorio final*/
				report(&system, outfile);
				/**Fecha os ficheiros*/
				fclose(outfile);
				break;
			case 0:
				break;
			default:
                printf("Escolha inválida. Tente novamente.\n");
        }

    } while (menuChoice != 0);
				
				return 0;
} 
