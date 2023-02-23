// =======================================================
//
// Do not modify this file
//
// =======================================================

#include "schedule.h"
#include "clock.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <signal.h>

#define DELAY_TIME 3
#define FIVE_SECONDS 5

struct schedule_item
{
  int time ;
  char * filename ;
  int received ;
} schedule_item ;

static struct schedule_list * schedule ;
static struct schedule_list * front ;
static int current_time = 0 ;
static int final_time = 0 ;

static void * tick( void ) 
{
  current_time ++ ;
  if( current_time > final_time + FIVE_SECONDS )
  {
    raise( SIGUSR2 );
  }
  return NULL ;
}

char * retrieveReceivedMessages(  )
{
  char * result = NULL ;
  if( front && front -> message -> time <= current_time )
  {
    assert( ( current_time - front -> message -> time < DELAY_TIME ) && 
              "Your receiver thread did not process the messages fast enough.");

    result = front -> message -> filename ;
    front = front -> next ;
  }
  return result ;
}

int initializeSchedule( char * filename ) 
{
  schedule = NULL ;
  front = NULL;
  readSchedule     ( filename ) ;    
  initializeClock  ( ONE_SECOND ) ;
  registerWithClock( tick ) ;
}

int readSchedule( char * schedule_file )
{
  FILE * fd;

  fd = fopen( schedule_file, "r" );
  if( fd == NULL )
  {
    printf( "Could not open schedule file: %s\n", schedule_file ) ;
    exit( 0 ) ;
  }

  int i = 0;

  char * time_string = ( char * ) malloc( sizeof( char ) * MAX_TIME_DIGITS ) ;
  char * filename = ( char * ) malloc ( sizeof( char ) * MAX_SCHEDULE_FILENAME_SIZE ) ;

  while ( fscanf ( fd, "%s%s\n", time_string, filename ) != EOF ) 
  {
    int entry_time = atoi( time_string ) ;
    final_time = entry_time ;

    struct schedule_item * item = ( struct schedule_item * ) malloc ( sizeof(struct  schedule_item ) );
    struct schedule_list * list_entry =  ( struct schedule_list * ) malloc ( sizeof( struct schedule_list ) );

    item -> filename = ( char * ) malloc ( sizeof( char ) * MAX_SCHEDULE_FILENAME_SIZE ) ;
    item -> time = entry_time ;

    strncpy( item -> filename, filename, MAX_SCHEDULE_FILENAME_SIZE ) ;

    list_entry -> message = item ;
    list_entry -> next = NULL ;

    struct schedule_list * list  = schedule ; 
    if( schedule == NULL )
    {
      schedule = list_entry ; 
      front    = schedule ;
    }
    else
    {
      while( list && list->next != NULL )
      {
        list = list -> next ;
      }
      list -> next = list_entry ;
    }

  }
  fclose( fd ) ;

  free( time_string ) ;
  free( filename ) ;
}

int freeSchedule( void )
{
  while( schedule )
  {
    struct schedule_list * node = schedule ; 

    free( schedule -> message -> filename ) ;
    free( schedule -> message ) ;
    
    schedule = schedule -> next ;

    free( node );
  }
  return 0 ;
}
