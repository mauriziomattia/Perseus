/*
 *
 *   events.h
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



#ifndef __EVENTS_H__
#define __EVENTS_H__



/*----------------*
 *  GLOBAL TYPES  *
 *----------------*/

/**
 *  An event with its parameters, managing function
 *  and absolute time, in ms, when the event will be
 *  managed.
 */

typedef struct event_{
   double    Time; /* Time, in ms, when the event has to be managed. */
   int  (*cmdFunc)(struct event_ *); /* Hook function to manage the event. */
   int   ParamNum; /* Number of parameters defining the event. */
   double  *Param; /* The parameters array. */
   int    CharNum; /* Number of character in the optional ParamStr. */
   char *ParamStr; /* Parameter string, to assign explicitly. It is optional. */
}  event;



/*--------------------*
 *  GLOBAL FUNCTIONS  *
 *--------------------*/

/**
 *  Initializes variables and structures needed to
 *  manage the events. It has to be launched before
 *  to use the other functions of this library.
 */

void initEventManager();


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
                ... );          /* double parameters, variable in number. */


/**
 *  Puts an existing and possibly updated event into the 
 *  event queue in order to allow the simulation to manage
 *  it at the time stored in the event structure.
 */ 

void putEvent(event *Event);


/**
 *  Free the memory allocated for the event passed as parameter.
 */ 

void deleteEvent(event *Event);


/**
 *  Manages all the events with time smaller than <Time>.
 */

void manageEvent (double Time); /* Next actual time of the simulation. */



#endif /* __EVENTS_H__ */