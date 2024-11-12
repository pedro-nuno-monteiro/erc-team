#include "fila_circular.h"

/* To test Circular queue */
int main ()
{
    myQueue testQ;

    
    float tab[20]; /*!< For testing */
    for(int i = 0; i < 20; i++) tab[i] = (i+1)* 2.5;
    
    inicQueue(& testQ);
        
    // NÂO rempver    
    #ifdef DEBUG 
        printf("The queue was created\n");
        printf("Capacity of queue is %d\n", getCapacity(& testQ));
        printf("    Size of queue is %d\n", getSize(& testQ));
    #endif

    float valor;
    if(-1 == deQueue (& testQ, &valor))
    {
        fprintf(stderr, "CORRECT: Unable to deQueue an empty Queue.\n");
    }

    /* Add 5 elements to the Queue */
    int i = 0;
    while( i < 5)
    {
        if(-1 == enQueue (& testQ, tab[i])) 
        {
           fprintf(stderr, "Erroe: Unable to enQueue. This should not happen.\n"); 
           return -1;
        }
        i++;
    }
   
    #ifdef DEBUG
        printQueue (& testQ);
        printf("Capacity of queue is %d\n", getCapacity(& testQ));
        printf("    Size of queue is %d\n", getSize(& testQ));
    #endif
    
     /* Add 2 additional elements to the Queue */
    while( i < 5+2)
    {
        if(-1 == enQueue (& testQ, tab[i])) 
        {
           fprintf(stderr, "Erro: Unable to enQueue. This should not happen.\n"); 
           return -1;
        }
        i++;
    }
    
    #ifdef DEBUG
        printQueue (& testQ);
        fprintf(stderr, "Capacity of queue is %d\n", getCapacity(& testQ));
        fprintf(stderr, "    Size of queue is %d\n", getSize(& testQ));
    #endif
    
    /* Remove 2 elements from the Queue */
    int info;
    for(int k = 0; k < 2; k++)
    {
        info = deQueue (& testQ, &valor);
        if(+1 == info) fprintf(stderr, "Removed from Queue the value %.3f.\n", valor);
        else fprintf(stderr, "ERRO: Tentativa de remover valor de fila vazia\n");
    }

    #ifdef DEBUG
        printQueue (& testQ);
        fprintf(stderr, "Capacity of queue is %d\n", getCapacity(& testQ));
        fprintf(stderr, "    Size of queue is %d\n", getSize(& testQ));
    #endif
    
    /* Remove 4 elements from the Queue */
    for(int k= 0; k < 4; k++)
    {
        info = deQueue (& testQ, &valor);
         if(+1 == info) fprintf(stderr, "Removed from Queue the value %.3f.\n", valor);
        else fprintf(stderr, "ERRO: Tentativa de remover valor de fila vazia\n");
    }  
     
    #ifdef DEBUG
        printQueue (& testQ);
        fprintf(stderr, "Capacity of queue is %d\n", getCapacity(& testQ));
        fprintf(stderr, "    Size of queue is %d\n", getSize(& testQ));
    #endif 

     /* Add 4 additional elements to the Queue */
    while( i < 5+2+4)
    {
        if(-1 == enQueue (& testQ, tab[i])) 
        {
           fprintf(stderr, "Erro: Unable to enQueue. This should not happen.\n"); 
        }
        i++;
    }

    #ifdef DEBUG
        printQueue (& testQ);
        fprintf(stderr, "Capacity of queue is %d\n", getCapacity(& testQ));
        fprintf(stderr, "    Size of queue is %d\n", getSize(& testQ));
    #endif 

    printf("Freeing de queue\n");
    
    freeQueue( &testQ);
    
    #ifdef DEBUG
        printQueue (& testQ);
        fprintf(stderr, "Capacity of queue is %d\n", getCapacity(& testQ));
        fprintf(stderr, "    Size of queue is %d\n", getSize(& testQ));
    #endif 

    return 0;
}
