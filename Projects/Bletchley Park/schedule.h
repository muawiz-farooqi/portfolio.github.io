// =======================================================
//
// Do not modify this file
//
// =======================================================

#ifndef __SCHEDULE_H___
#define __SCHEDULE_H___

#define MAX_SCHEDULE_FILENAME_SIZE 25
#define MAX_TIME_DIGITS 20


struct schedule_list
{
  struct schedule_item * message ;
  struct schedule_list * next ;
} ;

int initializeSchedule( char * filename ) ;

int freeSchedule( void ) ;

int readSchedule( char * filename ) ;

char * retrieveReceivedMessages(  ) ;

#endif
