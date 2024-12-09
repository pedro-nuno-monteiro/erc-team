#ifndef CIRCULAR_QUEUE_DYNAMIC
#define CIRCULAR_QUEUE_DYNAMIC

#include <stdio.h>
#include <stdlib.h>

/*! Struct that holds the discipline of the queue */
typedef enum {
    FIFO, /*!< first in first out, 0 */
    LIFO  /*!< last in first out, 1 */
} discipline;

/*! Representation of the struct storing a circular queue */
typedef struct {
    unsigned int max_size; /*!< current max_size of the queue (starts with 2) */
    int front, rear;       /*!< index of the front and rear (both -1 if empty)*/
    double *tab;           /*!< pointer to a block with max_size elements */
    discipline dis;        /*!< discipline of the queue */
} circular_queue;

/*! Inciializes queue q - this is the first routine that must be called before using q.
 * @param q pointer to the queue to be inicialized and the discipline of the queue
 * @return if memory allocation failed returns 0 (false) otherwise returns 1 (true) 
 * */
int inic(circular_queue * q, discipline dis);

/*! Frees space allocated by the queue 
 * @param q pointer to the queue to be cleared - the allocated memory is released
 * */
void freeQ(circular_queue * q);

/*! Informs is the queue is empty
 * @param q pointer to the queue to be evaluated
 * @return 1 (true) if *q is empty otherwise returns 0 (false)  */
int isEmpty (const circular_queue * q);

/*! Informs is the queue is full
 * @param q pointer to the queue to be evaluated
 * @return 1 (true) if *q is full otherwise returns 0 (false)  */
int isFull (const circular_queue * q); 

/*! Doubles the allocated space, but only if the queue is full
 * @param q pointer to the queue to enlarged
 * @return -1 if the queue is not full (does nothing), if memory allocation failed returns 0 (false) otherwise returns 1 (true) 
 * */
int expand (circular_queue * q); 

/*! Enqueues a new elemento to the queue
 * @param q pointer to the queue
 * @param val value to be added to the end of the queue
 * @return 1 if value was successuly added, otherwise returns 0 indicating queue was full and the call to expand failed to enlarge the queue.
 * */
int enQ (circular_queue * q, double val);

/*! Dequeues the front element of the queue 
 * @param q pointer to the queue 
 * @param val value that was at the front of the queue
 * @return 1 if the queue was not empty, otherwise it returns 0 indicating this function should not have been called
 * */
int deQ (circular_queue * q, float *val) ;

/*! Prints the value of all elements in the queue (for DEBUG purposes)
 * @param q pointer to the queue 
 * */
void printQ(const circular_queue * q);

#endif
