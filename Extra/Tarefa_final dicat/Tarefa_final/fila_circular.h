// Addated from https://prepinsta.com/data-structures-algorithms/circular-queue-using-array-in-c/

#include <stdio.h>
#include <stdlib.h>

#define CAPACITY 2

/*!<uncoment (comment) definition of DEBUG  e debug messafes are (NOT) desired */
// #define DEBUG  

typedef struct myqueue
{
    float * queue;    /*!< pointer to queue block of size capacity */  
    unsigned int capacity;     /*!< present queue capacity */    
    int front;   /*!< front of the queue -1 if empty */    
    int rear;    /*!< rear of the queue -1 if empty */  
} myQueue;


/*! \brief create empty queue of capacity zero
 * 
 * */
void inicQueue(myQueue * q);

/*! \brief number of elements in queue
 * 
 * */
int getCapacity(const myQueue * q);

/*! \brief number of elements in queue
 * 
 * */
int getSize(const myQueue * q);


/*! \brief if zero becomes CAPACITY else double queue capacity
 *  \return 0 if the queue was allocated succesfully, -1 if unable to resize queue
 * */
int resizeQueue(myQueue * q);


/*! \brief Here we check if the Circular queue is full or not.
 * \return Information about the queue: 1 is the queue is full, 0 otherwise
 */
int checkFull (const myQueue * q);


/*! \brief Here we check if the Circular queue is empty or not
 *  \return Information about the queue: 1 is the queue is empty, 0 otherwise
 */
int checkEmpty (const myQueue *q);


/*! \brief Addition in the Circular Queue.
 * \param q where value will be added  to the rear
 * \param[in] value to be added to the queue
 * \return Information about succes (+1) or not, due to full queue and then failure of realloc (-1) 
 */
int enQueue (myQueue * q, float value);


/*! \brief Removal from the Circular Queue
 *  \param[out] value of the element that was at the head or the queue.
 *  \return -1 try to deque on a empty queue 
 *  \return +1 if successfull
 * */
int deQueue (myQueue * q, float * value);


/*! \brief  Display the queue */
void printQueue (const myQueue * q);


/*! \brief  free memory alocated to the queue 
 *  This queue returns to inic state
 * */
void freeQueue (myQueue * q);

