// =======================================================
//
// Do not modify this file
//
// =======================================================

#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

// Default our tick interval to 1 second
#define ONE_SECOND 1
#define DEFAULT_INTERVAL ONE_SECOND

extern int interval  ;

// Linked list of parties who want to be notified
// by the alarm
struct notify_list {
  void * ( * tick ) ( void ) ;
  struct notify_list * next ;
} ;

int initializeClock( int interval ) ;

void registerWithClock(  void * ( * tick ) ( void ) ) ;

void startClock( ) ;

void stopClock( ) ;

#endif
