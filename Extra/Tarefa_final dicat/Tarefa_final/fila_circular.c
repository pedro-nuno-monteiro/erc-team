// Addated from https://prepinsta.com/data-structures-algorithms/circular-queue-using-array-in-c/

/* Para não me esquecer da definição
typedef struct myQueue
{
    float * queue;    
    unsigned int capacity;     
    int front;    
    int rear;   
} myQueue;
*/

#include "fila_circular.h"

/*! \brief create empty queue of capacity zero
 * 
 * */
void inicQueue(myQueue * q)
{
    q->capacity = 0;  /*!< ZERO capacity */
    q->queue = NULL;  /*!< the queue is now NULL for consistency */
    q->front = q->rear = -1;  /*!< queue is iniatlly empty */
}


/*! \brief To get the queue capacity
 * \return queue capacity
 * 
 * */
int getCapacity(const myQueue * q)
{
    return q->capacity;
}


/*! \brief To get the queue ocupancy
 * \return number of elements in the queue
 * 
 * */
int getSize(const myQueue * q)
{
    if(q->front == q->rear)
    {
        if(q->front == -1) return 0; /*!< queue is empty */
        else if(q->front >= 0) return 1; /*!< queue has one element */
    }  
    if ((q->front == q->rear + 1) || (q->front == 0 && q->rear == q->capacity - 1))
        return q->capacity;   /*!< queue is full */
    if(q->rear > q->front) return (q->rear - q->front +1); 
    else return (q->rear - q->front + 1 + q->capacity); 
}



/*! \brief if zero becomes CAPACITY else double queue capacity
 *  \return 0 if the queue was allocated succesfully, -1 if unable to resize queue
 * */
int resizeQueue(myQueue * q)
{
    #ifdef DEBUG
        const int debug = 0; // 0 if debug is off for this function
    #endif
    
    if(q->capacity == 0)  /*!< If no capacity reserves base capacity */
    {
        q->capacity = CAPACITY; /*!< base capacity */
        #ifdef DEBUG
            if(debug) fprintf (stderr, "malloc() is going to allocate %d floats\n", q->capacity);
        #endif
        q->queue = malloc((q->capacity)*sizeof(float));
        #ifdef DEBUG
            if(debug && q->queue == NULL) fprintf(stderr, "Unable to allocate queue\n");
        #endif
        if(q->queue == NULL) return -1; /*!< unable to resize queue */  
        return 0; /*!< queue was allocated succesfully */  
    }
    else 
    { 
        q->capacity = 2 * q->capacity; /*!< doubles previous capacity */
        q->queue = realloc(q->queue, (q->capacity)*sizeof(float));
        #ifdef DEBUG
            if(debug && q->queue == NULL)  fprintf(stderr, "Unable to resize queue\n");
        #endif
        if(q->queue == NULL) return -1; /*!< unable to resize queue */  
        // q->front  and q->rear  remain unchanged 
        return 0; /*!< queue was resized succesfully */  
    }
    fprintf (stderr, "Exiting resizeQueue\n");
}


/*! \brief Here we check if the Circular queue is full or not.
 * \return Information about the queue: 1 is the queue is full, 0 otherwise
 */
int checkFull (const myQueue * q)
{
    if(q->capacity == 0) return 1; /*!< queue is full  due to ZERO capacity */
    if ((q->front == q->rear + 1) || (q->front == 0 && q->rear == q->capacity - 1))
    {
        return 1; /*!< queue is full */ 
    }
    return 0;     /*!< queue is not full */ 
}

/*! \brief Here we check if the Circular queue is empty or not
 *  \return Information about the queue: 1 is the queue is empty, 0 otherwise
 */
int checkEmpty (const myQueue *q)
{
    if (q->front == -1)
    {
        return 1; /*!< queue is empty */ 
    }
    return 0; /*!< queue is noy empty */ 
}

/*! \brief Addition in the Circular Queue.
 * \param q where value will be added  to the rear
 * \param[in] value to be added to the queue
 * \return Information about succes (+1) or not, due to full queue and then failure of realloc (-1) 
 */
int enQueue (myQueue * q, float value)
{
    #ifdef DEBUG
        const int debug = 0; /*!< 0 if debug is off for this function */
    #endif
    
    int errorCode = 1;   /*!< By omission, it will enQueue*/ 
    if (checkFull (q))
    {
        #ifdef DEBUG
            if(debug) fprintf(stderr, "Overflow condition going to rezise\n");
        #endif
        resizeQueue(q);
        if(NULL == q->queue) errorCode = -1; /*!< Did not enQueue, due failing to resize */ 
        else errorCode = 1;   /*!< Will enQueue, after successuful resize */ 
    }
    if(1 == errorCode)  /*!< It will enQueue */ 
    {
        if (q->front == -1)  q->front = 0;

        q->rear = (q->rear + 1) % q->capacity;
        q->queue[q->rear] = value;
        #ifdef DEBUG
            if(debug) fprintf(stderr, "%.3f was enQueued to circular queue\n", value);
        #endif
    }
    return errorCode ; /*!< Status of enQueue operation, in cicular fashion*/ 
}


/*! \brief Removal from the Circular Queue
 *  \param[out] value of the element that was at the head or the queue.
 *  \return -1 try to deque on a empty queue 
 *  \return +1 if successfull
 * */
int deQueue (myQueue * q, float * value)
{
    #ifdef DEBUG
        const int debug = 0; // 0 if debug is off for this function
    #endif
    if (q->queue == NULL || checkEmpty ( q ))
    {
        #ifdef DEBUG
           if(debug) fprintf(stderr, "Underflow condition\n");
        #endif
        return -1; // try to deque on a empty queue 
    }
    else
    {
        *value = q->queue[q->front];
        if (q->front == q->rear)
        {
            q->front = q->rear = -1;
        }
        else
        {
            q->front = (q->front + 1) % q->capacity;
        }
        #ifdef DEBUG
            if(debug) fprintf(stderr, "%.3f was deQueued from circular queue\n", *value);
        #endif    
        return 1;  // success
    }
}

/*! \brief  Display the queue */
void printQueue (const myQueue * q)
{
    int i;
    if (checkEmpty (q))
        printf ("Queue is empty\n");
    else
    {
        printf ("\nThe queue looks like: \n");
        for (i = q->front; i != q->rear; i = (i + 1) % q->capacity)
        {
            printf ("%.3f ", q->queue[i]);
        }
        printf ("%.3f \n\n",q->queue[i]);
    }
}

/*! \brief  free memory alocated to the queue 
 *  This queue returns to inic state
 * */
void freeQueue (myQueue * q)
{
    free(q->queue);           /*!< all space was released */
    q->queue = NULL;          /*!< the queue is now NULL for consistency */
    q->capacity = 0;          /*!< capacity is ZERO */
    q->front = q->rear = -1;  /*!< queue is iniatlly empty */ 
}
