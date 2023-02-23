#include "include/crypto.h"
#include "schedule.h"
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include "clock.h"

#define MAX_NUM_THREADS 1024 
#define MAX_FILENAME_LENGTH 255
#define BUFFER_SIZE 148

static char * message_buffer[BUFFER_SIZE];
static int count ;
static int running = 1 ;

pthread_t message_receiver_tid ;
pthread_t decryptor_tid[ MAX_NUM_THREADS ] ;

pthread_mutex_t mutex;
sem_t thread_sema;

static void handleSIGUSR2( int sig )
{
    printf("Time to shutdown\n");
    running = 0 ;
}

int insertMessage( char * message )
{    
    pthread_mutex_lock( & mutex );
    
    assert( count < BUFFER_SIZE && "Tried to add a message to a full buffer");
    strncpy( message_buffer[count] , message, MAX_FILENAME_LENGTH ); 
    count++;

    pthread_mutex_unlock( & mutex );

    // sem_post( &thread_sema );
      
    return 0;
}

int removeMessage( char *message )
{    
    // sem_wait( &thread_sema );
    pthread_mutex_lock( & mutex );
    
    assert( count && "Tried to remove a message from an empty buffer");
    strncpy( message, message_buffer[count-1], MAX_FILENAME_LENGTH ); 
    count--;

    pthread_mutex_unlock( & mutex );

    return 0;
}

static void * tick ( void ) 
{
   return NULL ;
}

void * receiver_thread( void * args )
{    
    while( running )
    {
        char * message_file = retrieveReceivedMessages( );

        if( message_file )
        {
            insertMessage( message_file ) ;
            sem_post( &thread_sema );
        }
    }
}

void * decryptor_thread( void * args )
{
    while( running )
    {
        char * input_filename  = ( char * ) malloc ( sizeof( char ) * MAX_FILENAME_LENGTH ) ;
        char * output_filename  = ( char * ) malloc ( sizeof( char ) * MAX_FILENAME_LENGTH ) ;
        char * message = ( char * ) malloc ( sizeof( char ) * MAX_FILENAME_LENGTH ) ;

        memset( message,         0, MAX_FILENAME_LENGTH ) ;
        memset( input_filename,  0, MAX_FILENAME_LENGTH ) ;
        memset( output_filename, 0, MAX_FILENAME_LENGTH ) ;
        
        sem_wait( &thread_sema );
        removeMessage( message );

        strncpy( input_filename, "ciphertext/", strlen( "ciphertext/" ) ) ;
        strcat ( input_filename, message );

        strncpy( output_filename, "results/", strlen( "results/" ) ) ;
        strcat ( output_filename, message );
        output_filename[ strlen( output_filename ) - 8 ] = '\0';
        strcat ( output_filename, ".txt" );

        decryptFile( input_filename, output_filename );
        
        free( input_filename ) ;
        free( output_filename ) ;
        free( message ) ;
    }
}

int main( int argc, char * argv[] )
{
    if( argc != 2 )
    {
        printf("Usage: ./a.out [number of threads]\n") ;
    }
    int num_threads = atoi( argv[1] ) ;
    pthread_t tid[ MAX_NUM_THREADS ] ;
  
    srand (time(NULL)) ;

    sem_init( &thread_sema, 0, 0 );

    // initialize the message buffer
    int i ;
    for( i = 0; i < BUFFER_SIZE; i++ )
    {
        message_buffer[i] = ( char * ) malloc( MAX_FILENAME_LENGTH ) ;
    }

    count = 0 ;

    struct sigaction act;
    memset ( & act, '\0', sizeof( act ) ) ;

    act . sa_handler = & handleSIGUSR2 ;
    if ( sigaction( SIGUSR2, &act, NULL ) < 0 )  
    {
        perror ( "sigaction: " ) ;
        return 0;
    }

    initializeClock( ONE_SECOND ) ;
    registerWithClock( tick ) ;

    initializeSchedule( "schedule.txt" ) ;

    pthread_create( &message_receiver_tid, NULL, receiver_thread, NULL ) ;

    for( i = 0; i < num_threads; i++ )
    {
        pthread_create( & decryptor_tid[i], NULL, decryptor_thread, NULL ) ;
    }

    startClock( ) ;

    while( running ) ;

    stopClock( ) ;

    pthread_join( message_receiver_tid, NULL ) ;

    for( i = 0; i < num_threads; i++ )
    {
        pthread_join( decryptor_tid[i], NULL ) ;
    }

    for( i = 0; i < BUFFER_SIZE; i++ )
    {
        free( message_buffer[i] ) ;
    }
    freeSchedule( ) ;

    return 0 ;
}
