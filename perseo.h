/*
 *
 *   perseo.h
 *
 *   the header file of the simulation core where the external spikes are created 
 *   and managed, and the dynamic variables are updated.
 *   The main function is here.
 *
 *   Project: PERSEO 2.x
 *
 */



#ifndef __PERSEO_H__
#define __PERSEO_H__



#include "types.h"
#include "invar.h"



/*--------------------*
 *  GLOBAL CONSTANTS  *
 *--------------------*/

#define PRINT_ERROR    /* If defined activates the error print. */
#define PRINT_STATUS   /* If defined prints the simulation status at real-time. */
// #define PRINT_DEBUG    /* If defined prints debug information. */

#define EXCEPTION         0   /* An element in the axon coding an addressing exception:   *
                               * two post-synaptic neurons are farther than MAX_DISTANCE  */
#define ANALOG_DEPTH    256   /* Max number of synaptic efficacies values allowed in a connectivity block. */
#define START_TIME_OFFSET 0.0 /* Starting time for the simulation. */

#define INIT_FILE "perseo.ini"       /* Definition file to define the system and simulation properties. */

#define VERSION "2.1.3i, June 24th, 2013"   /* Version number and date. */
#define AUTHORS "M. Mattia" /* Author list. */
#define REFERENCES "M. Mattia, P. Del Giudice. Neural Comput. 2000;12(10):2305-29"
#define ACKNOWLEDGEMENTS "P. Del Giudice, G. Gigante, M. Pannunzi" /* Acknowledgement list. */
// #define AUTHORS "Maurizio Mattia and Paolo Del Giudice" /* Author list. */



/*----------------------*
 *   GLOBAL VARIABLES   *
 *----------------------*/

extern real                 Life; /* Life time of the simulation in ms. */
extern int           NeuronsSeed; /* Seed of pseudo-random number generator for the initialization of neuron states. */
extern int          SynapsesSeed; /* Seed of pseudo-random number generator for the initialization of synapse states. */
extern boolean    QuitSimulation; /* If true the simulation interrupts. Is changed *
                                   * when a signal is sent to the process.         */
extern unsigned int MemoryAmount; /* The amount of memory allocated by Perseo. */



/*--------------------*
 *  GLOBAL FUNCTIONS  *
 *--------------------*/

/**
 *  Management of the external interrupt signals (^C or stop 
 *  signals from other processes).
 */

void interruptSimulation (int Signal);


/**
 *  Prints an error on stderr, without
 *  closing the application.
 */

void printError(char *FunctionName, char *ErrorMessage);


/**
 *  Prints a fatal error on stderr and
 *  close the application.
 */

void printFatalError(char *FunctionName, char *ErrorMessage);


/**
 *  It is a customized version of standard malloc function
 *  which return a pointer to the allocated memory of size
 *  <Size>. In addion updates a counter of memory usage and
 *  exit program with an error message <ErrorMessage>.
 */

void *getMemory(size_t        Size,  // Size in byte of memory to allocate.
                char *ErrorMessage); // Error message if the allocation process fails.


/**
 *  Adds an event to the queue of the selected layer.
 */

void addNewSpike (
                  indexn n,   /* Emitting neuron.                 */
                  timex  t,   /* Emission time                    */
                  real ISI,   /* ISI from the last event.         */
                  int    l    /* Layer where the event is loaded. */
                 );



#endif /* __PERSEO_H__ */