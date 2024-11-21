#include "circular_queue_dynamic.h"

int inic(circular_queue * q, discipline dis) {
	int result = 1; // ok
	q->front = -1;
	q->rear = -1;
	q->max_size = 2; // initially only two postions are allocated
	q->dis = dis;
	// Dynamically allocate memory using malloc()
	q->tab = (double*) malloc(q->max_size*sizeof(double));
	if(q->tab == NULL) {
			printf("Memory not allocated.\n");
			result = 0; // not ok
	}
	return result;
}

void freeQ(circular_queue * q) {
	free(q->tab);
	q->max_size = 0;
}

int isEmpty (const circular_queue * q) { 
	return q->front == -1 && q->rear == -1; // is empty
}

int isFull (const circular_queue * q) { 
	return (q->rear + 1) % q->max_size == q->front; // is full
}

int expand (circular_queue * q) {
	if(!isFull(q)) return -1; // Should NOT be called
	
	// Dynamically re-allocate memory using realloc()
	q->tab = (double*)realloc(q->tab, 2*q->max_size * sizeof(double));
	
	if (q->tab == NULL) {
		printf("Reallocation Failed\n");
		return 0;
	}

	// if front larger than rear must copy to new block
	if(q->rear < q->front) {  
		int i = q->max_size;
		int source = q->front;
		while(source <= q->max_size + q->rear) {
			q->tab[i++] = q->tab[source % q->max_size];
			source++;
		}
		q->front = q->max_size;
		q->rear = 2*q->max_size-1;
	}
	
	// Expansion was well succeeded
	q->max_size *= 2; // new max_size
	return 1; 
}

int enQ (circular_queue * q, double val) {

	// If full, expands before attempting insert
	if(isFull(q) && !expand(q)) return 0; // ends in case expand failed

	// Now inserts
	if (isEmpty(q)) {   // Inserts in empty queue
		q->front = 0;  
		q->rear = 0;
		q->tab[q->rear] = val;
	}
	else {
		// Inserts in queue
		q->rear = (q->rear + 1) % q->max_size ;
		q->tab[q->rear] = val;
	}
	return 1;
}

int deQ (circular_queue * q, float *val) {
    
	// If is empty nothing to do
	if(isEmpty(q)) return 0; 
	// Removes elemet

	if (q->dis == FIFO) {
		*val = q->tab[q->front];  //obtém valor no topo da fila

		if(q->front == q->rear) {
			// queue becomes empty
			q->front = -1;
			q->rear = -1;
		} 
		//updates front
		else q->front = (q->front + 1) % q->max_size ;

	}
	else {
		*val = q->tab[q->rear];

		if(q->front == q->rear) {
			// queue becomes empty
			q->front = -1;
			q->rear = -1;
		} 
		//updates rear
		else q->rear = (q->rear - 1) % q->max_size;
	}
	return 1;
}

void printQ(const circular_queue * q) {
	if(isEmpty(q)) printf("\nIs empty\n");
	else {
		if(isFull(q)) printf("Printing a FULL Queue\n");
		
		printf ("Queue from front to rear:\n");
		
		unsigned int i = q->front;
		if(q->rear >= q->front) {
			while (i <= q->rear) {
				printf("%.2lf\n", q->tab[i % q->max_size]);
				i++;
			}
		}
		else { // q->rear > q-front
			while (i <= q->rear + q->max_size) {
				printf("%.2lf\n", q->tab[i % q->max_size]);
				i++;
			}
		}			
	}
}
