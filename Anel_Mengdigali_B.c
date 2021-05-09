//Anel Mengdigali
//Part B:

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include <semaphore.h> //for semaphore + pthread usage
#define N 1 //buffer size 

sem_t empty; //semaphore
sem_t full; //semaphore

pthread_mutex_t entrance; //mutex

int buffer = 0; //to control buffer size

void *flush( void *ptr ) {
    
    //input casting and identifying length of it:
    char *s;
    s = (char *) ptr;
    int l = strlen(s);
    
    //if thread is the lower thread (operating as producer):
    if( s == "abcdefghij" ) {
           
        for( int i = 0; i < l; i++ ) {
            
            //decreasing value from 1 to 0, so to prevent the lower thread execution for the 2nd time: 
            sem_wait(&empty); 
            
            //mutex lock the entrance for critical region:
            pthread_mutex_lock(&entrance); 
            
            buffer += 1; //increasing buffer
            
            //the critical region stats:
            putchar(s[i]);
            fflush(stdout);
            sleep(rand() % 2);
            //:the critical region ends
            
            //mutex unlock the entrance for critical region so the upper thread can come:
            pthread_mutex_unlock(&entrance); 
            
            //increasing value from 0 to 1 as this thread done its job, so sending signal to waiting queue containig the upper thread to wakeup, and come to execute:	
            sem_post(&full); 
            
        }
        
    }
    
    //if thread is the upper thread, operating as consumer
    else if ( s == "ABCDEFGHIJ" ) {

        for( int i = 0; i < l; i++ ) {
        
            //decreasing value from 1 to 0, so to prevent the upper thread execution for the 2nd time: 
            sem_wait(&full); 
            
	    //mutex lock the entrance for critical region:
            pthread_mutex_lock(&entrance); 
            
            buffer -= 1; //decreasing buffer
            
            //the critical region starts:
            putchar(s[i]);
            fflush(stdout);
            sleep(rand() % 2);
            //:the critical region ends
            
	    //mutex unlock the netrance for critical region so lower thread can come:
            pthread_mutex_unlock(&entrance); 
            
            //increasing value from 0 to 1 as this thread done its job, so sending signal to the lower thread to execute:
            sem_post(&empty); 
            
        }
        
    }
    
}

/* Generally, solved as producer-consumer problem using letters instead of items.
The aim is to print alternating letters between 2 strings, to reach that 2 things have to be done:
 1) mutual exclusion done through mutex: to have 1 thread at a time in critical region.
 2) synchronization done through semaphores: to have 1 letter in operation by each thread. */
 
int main() {
    
    //2 messages that will be given to 2 threads:
    char *s1 = "abcdefghij";
    char *s2 = "ABCDEFGHIJ";
    
    //2 threads initialisation for 2 messages:
    pthread_t lower, upper; 
    
    //semaphore usage to obtain synchronization (only 1 item added and consumed) of the 2 threads:
    sem_init( &empty, 0, N ); //semaphore (controlling empty buffer, space is 1) initialisation for threads
    sem_init( &full, 0, 0 );  //semaphore (controlling full buffer) initialisation for threads
    
    //mutex usage to obtain mutual exclusion of the 2 threads, as mutex controls entrance in critical region by locking and unlocking operations:
    pthread_mutex_init( &entrance, NULL ); //mutex initialisation
    
    //creation of threads to do segment of code of the flush function:
    int pid1 = pthread_create( &lower, NULL, flush, (void*) s1 );
    int pid2 = pthread_create( &upper, NULL, flush, (void*) s2 );
    
    //the main process waits for those 2 treads to terminate before it finishes by using basic function of Pthread, pthread_join():
    pthread_join( lower, NULL );
    pthread_join( upper, NULL );
    
    printf("\n");
    
    //mutex deletion after threads execution is done:
    pthread_mutex_destroy(&entrance); 
    
    //semaphores deletion after threads execution is done:
    sem_destroy(&empty);
    sem_destroy(&full);
    
    exit(0);
    
}
