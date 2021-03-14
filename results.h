/*
 *
 *   results.h
 *
 *   The header file of the function library for the system observables 
 *   logging and other prints.
 *   NOTE: In future version of Perseo should be redesigned the logging
 *         system here defined, using an event based approach, to avoid
 *         "TEMP" comments all around the source code of the project.
 *
 *   Project: PERSEO 2.x
 *
 */



#ifndef __RESULTS_H__
#define __RESULTS_H__



#include <stdio.h>

#include "types.h"
#include "events.h"



/*----------------------------------*
 *  GLOBAL FUNCTIONS AND VARIABLES  *
 *----------------------------------*/

#define DEFAULT_LOG_FILENAME "perseo.log"


extern char *DocFileName;  /* Name of the file containing a descrition of the log files. */
extern FILE *DocFile;      /* File header of the descrition file. */


/**
 *   Prints the program presentation: the version number and date, 
 *   the authors and the list of active flags set at compile time,
 *   with other informations.
 */

void printProgramPresentation ();



/*----------------------------------------*
 *                                        *
 *   Population emission rates logging.   *
 *                                        *
 *----------------------------------------*/

/*** Global parameters to set in INIT_FILE. ***/
extern boolean  RatesResults; /* If true prints the rates. */
extern char *  RatesFileName; /* Name of the log file. */
extern real    RatesSampling; /* Sampling interval in ms. */
extern real   RatesStartTime; /* Time at which start the log. */


/**
 *  Updates the number of spikes emitted by the neurons 
 *  in a population from the last sampling of the emission 
 *  rates. The index of the emitting neuron is <n>.
 */

void updateRates (indexn n); /* Emitting neuron index. */


/**
 *  Logs the number of spikes emitted per unit time
 *  by all the neuron in the populations (the population
 *  emission rate) of the network. The emission rates is 
 *  by the ration between the # of spikes emitted in a 
 *  sampling interval divided by the product of the sampling 
 *  interval and the number of neurons in the population.
 */

void outRates (real t); /* Actual simulation time. */



/*-----------------------------------------*
 *                                         *
 *   Synaptic transition number logging.   *
 *                                         *
 *-----------------------------------------*/

// NOTES: It is meaningful only for bistable dynamical synapses.

/*** Global parameters to set in INIT_FILE. ***/
extern boolean  SynTransResults; /* If true prints the number of synaptic transition. */
extern char *  SynTransFileName; /* Name of the log file. */
extern real    SynTransSampling; /* Sampling interval in ms. */
extern real   SynTransStartTime; /* Time at which start the log. */


/**
 *  Updates the number of up and down transitions when a 
 *  synapse changes its long-term state.
 */

void updateSynTrans (int JflagBefore, int JflagAfter); // Up and down state before and after the synaptic update.


/**
 *  Prints the total number of, up and down, synaptic 
 *  transition during the last sampling interval.
 */

void outSynTrans (real t); /* Actual simulation time. */



/*-------------------------------*
 *                               *
 *   Synaptic matrix printing.   *
 *                               *
 *-------------------------------*/

/*** Global parameters to set in INIT_FILE. ***/
extern boolean  SynapsesResults; /* If true prints the synaptic matrix. */
extern char *  SynapsesFileName; /* The mantissa of the name of the log file. */
extern indexn   SynapsePreStart; /* Range of he synaptic matrix to traverse. */
extern indexn     SynapsePreEnd;
extern indexn  SynapsePostStart;
extern indexn    SynapsePostEnd;


/**
 *  Prints the synaptic state at time <Time> for all the
 *  synapses of the synaptic matrix having as pre and post
 *  synaptic neurons those ones in the range specified by
 *  SynapsePreStart/Stop and SynapsePostStart/Stop.
 *  Each row in the output file is an existing synapse.
 *  The output file name is composed by the string in
 *  SynapsesFileName, and the integer part of <Time>.
 */

void outSynapses (real Time);



/*-------------------------------------------*
 *                                           *
 *   Detailed synaptic transition logging.   *
 *                                           *
 *-------------------------------------------*/

/*** Global parameters to set in INIT_FILE. ***/
extern boolean detailSynTransResults; /* If true prints the detailed synaptic transitions. */
extern char  *detailSynTransFileName; /* Name of the log file. */


/**
 *  Each long-term synaptic transition is logged on a row
 *  of the output file. If the synapse is not dynamic the
 *  file will be empty.
 */

void detailSynTrans (indexn        i,  /* Post-synaptic neuron. */
                     indexn        j,  /* Pre-synaptic neuron. */
                     int JflagBefore,  /* Logical long-term synaptic state before update. */
                     int  JflagAfter,  /* Logical long-term synaptic state after update. */
                     timex         t); /* Synaptic update time. */



/*-------------------------------------------------*
 *                                                 *
 *   Synaptic matrix prints at population level.   *
 *                                                 *
 *-------------------------------------------------*/

/*** Global parameters to set in INIT_FILE. ***/
extern boolean SynStructResults; /* If true prints the synaptic matrix. */
extern char  *SynStructFileName; /* Name of the log file. */


/**
 *  Prints for each connectivity element (a synapse 
 *  population, the synapse connecting the neurons from
 *  a module to the ones of another module in the 
 *  network) the number of synapses in the different
 *  long-term synaptic states allowed for that synapse
 *  type.
 *  Each row in the output file is an existing synaptic
 *  population, so that connecivity summary is a sparse
 *  matrix (row, column, value).
 *  The output file name is composed by the string in
 *  <SynStructFile>, and the integer part of the time
 *  associated to <Event>.
 *  NOTES: the procedure can be time expensive, requiring
 *         computational resources proportional to the 
 *         number of the synapses in the network.
 */

int outSynStruct (event *Event);



/*--------------------------------------------*
 *                                            *
 *   Synaptic structure of dendritic trees.   *
 *                                            *
 *--------------------------------------------*/

/*** Global parameters to set in INIT_FILE. ***/
extern boolean DenStructResults; /* If true prints the synaptic structures of dendritic trees. */
extern char  *DenStructFileName; /* Name of the log file. */


/**
 *  Prints for each pre-synaptic neuron population and
 *  post-synaptic neuron the number of synapses in the
 *  different long-term synaptic states allowed for that 
 *  synapse type.
 *  Each row in the output file is an existing synaptic
 *  population, so that printed summary is a sparse
 *  matrix (row, column, value).
 *  The output file name is composed by the string in
 *  <DenStructFile>, and the integer part of <Time>.
 *  NOTES: the procedure can be time expensive, requiring
 *         computational resources proportional to the 
 *         number of the synapses in the network.
 */

int outDenStruct (event *Event);



/*-----------------------------*
 *                             *
 *   Emitted spikes logging.   *
 *                             *
 *-----------------------------*/

/*** Global parameters to set in INIT_FILE. ***/
extern boolean SpikesResults; /* If true prints the emission time. */
extern char  *SpikesFileName; /* Name of the log file. */
extern indexn     SpikeStart; /* First neuron whose activity have to be recorded. */
extern indexn       SpikeEnd; /* Last neuron whose activity have to be recorded. */


/*------------*
 *  outSpike  *
 *------------*/

/**
 *  Prints the emission time of the spikes emitted by
 *  the neurons in the range [<SpikeStart>, <SpikeEnd>].
 *  Each row of the output file is a spike, with its
 *  neuron and time label. Emission times are in 
 *  increasing order.
 */

void outSpike (indexn n, /* Emitting neuron. */
               timex  t); /* Emission time. */


/**
 *  Turn on the switch to dump the spikes.
 */

int startSpikeRecording(event *Event);


/**
 *  Turn on the switch to dump the spikes.
 */

int stopSpikeRecording(event *Event);



/*------------------------------------------*
 *                                          *
 *   Asynchronous SYNAPTIC STATE logging.   *
 *                                          *
 *------------------------------------------*/

/*** Global parameters to set in INIT_FILE. ***/
extern boolean  SynStateResults; /* If true prints the synaptic states. */
extern char   *SynStateFileName; /* Name of the log file. */
extern indexn  SynStatePreStart; /* Synaptic population whose activity have to be recorded.  */
extern indexn    SynStatePreEnd; /* It is defined with the first and the last pre- and post- */
extern indexn SynStatePostStart; /* synaptic neuron of the involved synapses.                */
extern indexn   SynStatePostEnd;


/**
 *  Logs each asynchronous update of the synapse in the
 *  range of interest. Each update corresponds to a row
 *  of the output file, indicating the pre- and post-
 *  synaptic neurons connected by the synapses, the time
 *  of the event and the state variables like the actual 
 *  long-term synaptic efficacy, the long-term synaptic
 *  level (potentiate or depressed) and the internal dynamic
 *  variables (actvie vescicles, calcium concentration , ...).
 *  Fixed synapses have empty log.
 */

void outSynapticState (indexn i, // Post-synaptic neuron.
                       indexn j, // Pre-sinaptic neuron.
                       timex  t, // Time at which occurs the update.
                       int  NSV, // Number of state variables passed as a list of variable parameters. 
                       ...);


/**
 *  Prints a snapshot of synapse states in the sub-matrix 
 *  specified in the initialization file. It is useful
 *  as the first and last print during the recording of
 *  the asynchronous updated synaptic states.
 */

void flushSynapticState (double Time);



/*------------------------------------------*
 *                                          *
 *   Asynchronous NEURONAL STATE logging.   *
 *                                          *
 *------------------------------------------*/

/*** Global parameters to set in INIT_FILE. ***/
extern boolean NeuStateResults; /* If true prints the neuronal state. */
extern char  *NeuStateFileName; /* Name of the log file. */
extern indexn    NeuStateStart; /* First neuron to record the activity. */
extern indexn      NeuStateEnd; /* Last neuron to record the activity. */


/**
 *  Logs each asynchronous update of the neurons in the
 *  range of interest. Each update corresponds to a row
 *  in the output file, indicating the neuron index, the 
 *  time of the event and the state variables like the 
 *  membrane potential.
 */

void outNeuronalState (indexn i,  // Neuron.
                       timex  t, // Time at which occurs the update.
                       int  NSV, // Number of state variables passed as a list of variable parameters. 
                       ...);


/**
 *  Prints a snapshot of the state of a neuron subset
 *  specified in the initialization file. It is useful
 *  as the first and last print during the recording of
 *  the asynchronous updated neuronal states.
 */

void flushNeuronalState (double Time);



/*-------------------------*
 *                         *
 *   Stimuli composition   *
 *                         *
 *-------------------------*/


/**
 *  Prints the composition of the different defined stimuli.
 *  TO DO: At this time it is not implemented being not 
 *         implemented the multi-stimuli management, and 
 *         the function provide an empty log file.
 */

void outStimuli ();



/*-------------------------------*
 *                               *
 *   Afferent current logging.   *
 *                               *
 *-------------------------------*/

/*** Global parameters to set in INIT_FILE. ***/
extern boolean CurrentResults; /* If true prints the afferent current to the selected neuron. */
extern char  *CurrentFileName; /* Name of the log file. */
extern real   CurrentSampling; /* Sampling period in ms. */
extern real  CurrentStartTime; /* Starting time of the registration. */
extern indexn   CurrentNeuron; /* Neuron whose current have to be logged.. */


/**
 *  Update the charge accumulated from the last sampling time.
 *  dividing currents from due to different pre-synaptic populations.
 */

void updateCurrent (indexn            i, // Receiving neuron.
                    int               j, // Pre-synaptic neuron or -1.
                    real IncomingCharge); // Charge due to an incoming spike.


/**
 *  Prints each sampling period the charge accumulated 
 *  per unit time by the selected neuron, due to the 
 *  incoming synaptic current.
 */

void outCurrent (real Time);



/*-----------------------------------------*
 *                                         *
 *   Opening and closure of output files   *
 *                                         *
 *-----------------------------------------*/


/**
 *  Opens the output files.
 */

void openOutputFiles ();


/**
 *  Flushes the output files.
 */

void flushOutputFiles ();


/**
 *  Closes the output files.
 */

void closeOutputFiles ();



#endif /* __RESULTS_H__ */
