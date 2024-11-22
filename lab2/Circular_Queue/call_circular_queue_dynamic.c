#include <stdio.h>
#include "circular_queue_dynamic.h"

/*! \mainpage
 *
 * Code addapted from
 * https://www.simplilearn.com/tutorials/data-structure-tutorial/circular-queue-in-data-structure
 * uses malloc, realloc and free
 * 
 * To help students get familiar with the use of the structure
 */

int main()
{
    struct circular_queue q1;
    inic(&q1);
    printQ(& q1);
    
    double x[10] = {0.0};
    
    for(unsigned int i = 1; i < 10; i++) x[i] = x[i-1] + 0.1;
    
    printf("Inserting:\n");
    for(unsigned int i=0; i < 4 ; i++)
    {
      if(!enQ(&q1, x[i])) printf("\nenQ failled");
      else printf("%.2f ", x[i]);  
    }
    printf("\nthe queue after Inserting:\n");
    printQ(& q1);
    
    printf("\n\nRemoves 3 top elements from que queue\n:");
    double y[10] ;
    unsigned int k; 
    for(k=0; k < 3 ; k++)
    {
       if(!deQ(&q1, &y[k])) {printf("\ndeQ failled"); break; }
       else printf("%.2f ", y[k]); 
    }
    printf("\nthe queue after the removal:\n");
    printQ(& q1);
    
    printf("\n\nInserting:\n");
    for(unsigned int i=4; i < 8 ; i++)
    {
      if(!enQ(&q1, x[i])) printf("\nenQ failled");
      else printf("%.2f ", x[i]);  
    }
    printf("\nthe queue after Inserting:\n");
    printQ(& q1);
    
    
    printf("\n\nRemoves all elements, including when the queue is EMPTY resulting in error message\n");
    for(k=4; k < 10 ; k++)
    {
       if(!deQ(&q1, &y[k])) { printf("\ndeQ failled"); break; }
       else printf("%.2f ", y[k]); 
    }
  
    freeQ(&q1); // liberta espaço
}
