/*
 *
 *   events.c
 *
 *     Manages events arising during the life of the simulation
 *   like dumping of some information, start and stop composite 
 *   stimulation or updating parameters of the populations.
 *     The events are created by newEvent(). 
 *     During the simulation the function manageEvent() have to
 *   be called in order to verify if it is the time for an event.
 *
 *   Project: Perseo 2.1.x
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "sortedqueue.h"
#include "events.h"
#include "perseo.h"



/*-------------------*
 *  LOCAL VARIABLES  *
 *-------------------*/

sorted_queue Events; /* The sorted queue containing the external commands sorted in time. */



/*-------------------*
 *  LOCAL FUNCTIONS  *
 *-------------------*/


/**
 *  Compare to events returning 1, 0 or -1 depending
 *  on if the time to manage Left event is greater, 
 *  equal or lower than the Right one.
 *  This function is used to define the metrics of
 *  the elements in the sorted queue storing the 
 *  events.
 */
int cmpEvents(event *Left, event *Right)
{
   if ((*Left).Time < (*Right).Time)
      return -1;
   else
      if ((*Left).Time > (*Right).Time)
         return 1;
   return 0;
}



/*--------------------*
 *  GLOBAL FUNCTIONS  *
 *--------------------*/

/*--------------------*
 *  initEventManager  *
 *--------------------*/

/**
 *  Initializes variables and structures needed to
 *  manage the events. It has to be launched before
 *  to use the other functions of this library.
 */

void initEventManager()
{
   initSortedQueue(&Events, &cmpEvents);
}


/*------------*
 *  newEvent  *
 *------------*/

/**
 *  Insert a new command in the event queue, which has to be
 *  managed at time <Time> using function <cmdFunc> with
 *  parameters variable in number.
 *  Returns the created event.
 */ 

event *newEvent(double    Time, /* Time when the event will be managed. */
                int (*cmdFunc)(event *),  /* Hook function managing the event. */
                char *ParamStr, /* Optional string parameters. It may be NULL. */
                int   ParamNum, /* Number of double parameters. */
                ... )           /* double parameters, variable in number. */
{
   static event   *Event;
   static va_list marker;
   static int          k;

   /*** Allocates memory. ***/
   Event = getMemory(sizeof(event), "ERROR (newEvent): Out of memory (A).");
   if (ParamNum > 0)
      Event->Param = getMemory(sizeof(double) * ParamNum, "ERROR (newEvent): Out of memory (B).");
   else
      Event->Param = NULL;
   if (ParamStr != NULL) {
      Event->CharNum = strlen(ParamStr) + 1;
      Event->ParamStr = getMemory(sizeof(char) * Event->CharNum, "ERROR (newEvent): Out of memory (C).");
   } else {
      Event->CharNum = 0;
      Event->ParamStr = NULL;
   }

   /*** Sets the parameters of the event. ***/
   Event->Time = Time;
   Event->cmdFunc = cmdFunc;
   Event->ParamNum = ParamNum;
   va_start(marker, ParamNum);
   for (k=0; k<ParamNum; k++)
       Event->Param[k] = va_arg( marker, double);
   va_end(marker);
   if (ParamStr != NULL)
      strcpy(Event->ParamStr, ParamStr);

   /*** Appends the event to the queue. ***/
   putSortedQueueItem(&Events, Event);

   return Event;
}


/*------------*
 *  putEvent  *
 *------------*/

/**
 *  Puts an existing and possibly updated event into the 
 *  event queue in order to allow the simulation to manage
 *  it at the time stored in the event structure.
 */ 

void putEvent(event *Event)
{
   /*** Appends the event to the queue. ***/
   putSortedQueueItem(&Events, Event);
}


/*---------------*
 *  deleteEvent  *
 *---------------*/

/**
 *  Free the memory allocated for the event passed as parameter.
 */ 

void deleteEvent(event *Event)
{
   free(Event->Param);
   MemoryAmount -= sizeof(double) * Event->ParamNum;
   if (Event->ParamStr != NULL) {
      free(Event->ParamStr);
      MemoryAmount -= Event->CharNum;
   }
   free(Event);
   MemoryAmount -= sizeof(event);
}


/*---------------*
 *  manageEvent  *
 *---------------*/

/**
 *  Manages all the events with time smaller than <Time>.
 */

void manageEvent (double Time) /* Next actual time of the simulation. */
{
   static event *Event;
   if (!isSortedQueueEmpty(&Events))
      while (Time > ((event *)frontSortedQueueItem(&Events))->Time) {
         Event = (event *)getSortedQueueItem(&Events);
         if ((*(Event->cmdFunc))(Event))
            deleteEvent(Event);
         if (isSortedQueueEmpty(&Events)) break;
      }
}