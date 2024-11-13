//! Addapted from 
//! https://www.simplilearn.com/tutorials/data-structure-tutorial/circular-queue-in-data-structure
//! uses malloc, realloc and free
#ifndef CIRCULAR_QUEUE_DYNAMIC
#define CIRCULAR_QUEUE_DYNAMIC

#include <stdio.h>
#include <stdlib.h> // malloc , realloc, free

/*! Representation of the struct storing a circular queue */
struct circular_queue {
    unsigned int max_size; /*!< current max_size of the queue (starts with 2) */
    int front, rear;       /*!< index of the front and rear (both -1 if empty)*/
    double *tab;           /*!< pointer to a block with max_size elements */
};

/*! Inciializes queue q - this is the first routine that must be called before using q.
 * @param q pointer to the queue to be inicialized
 * @return if memory allocation failed returns 0 (false) otherwise returns 1 (true) 
 * */
int inic(struct circular_queue * q);

/*! Frees space allocated by the queue 
 * @param q pointer to the queue to be cleared - the allocated memory is released
 * */
void freeQ(struct circular_queue * q);

/*! Informs is the queue is empty
 * @param q pointer to the queue to be evaluated
 * @return  1 (true) if *q is empty otherwise returns 0 (false)  */
int isEmpty (const struct circular_queue * q);

/*! Informs is the queue is full
 * @param q pointer to the queue to be evaluated
 * @return  1 (true) if *q is full otherwise returns 0 (false)  */
int isFull (const struct circular_queue * q); 

/*! Doubles the allocated space, but only if the queue is full
 * @param q pointer to the queue to enlarged
 * @return  -1 if the queue is not full (does nothing), if memory allocation failed returns 0 (false) otherwise returns 1 (true) 
 * */
int expand (struct circular_queue * q); 

/*! Enqueues a new elemento to the queue
 * @param q pointer to the queue
 * @param val value to me added to the end of the queue
 * @return  1 if value was successuly added, otherwise returns 0 indicating queue was full and the call to expand failed to enlarge the queue.
 * */
int enQ (struct circular_queue * q, double val);

/*! Dequeues the front elemento of the queue 
 * @param q pointer to the queue 
 * @param val value that was at the front of the queue
 * @return 1 if the queue was not empty, otherwise it returns 0 indicating this function should not have been called
 * */
int deQ (struct circular_queue * q, double *val) ;

/*! Prints the value of all elements in the queue (for DEBUG purposes)
 * @param q pointer to the queue 
 * */
void printQ(const struct circular_queue * q);

#endif
