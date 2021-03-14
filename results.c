/*
 *
 *   results.c
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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

#include "types.h"
#include "queue.h"
#include "invar.h"

#include "perseo.h"
#include "stimuli.h"
#include "modules.h"
#include "connectivity.h"
#include "synapses.h"
#include "neurons.h"
#include "events.h"



/*-------------------*
 *  LOCAL VARIABLES  *
 *-------------------*/

char    *DocFileName = EMPTY_STRING; /* Name of the file containing a descrition of the log files. */
FILE        *DocFile = NULL;         /* File header of the descrition file. */



/*----------------------------------*
 *  GLOBAL FUNCTIONS AND VARIABLES  *
 *----------------------------------*/

/**
 *   Prints the program presentation: the version number and date, 
 *   the authors and the list of active flags set at compile time.
 */

void printProgramPresentation () {

   fprintf(stderr, "\nPerseo  ver. %s\n", VERSION);
   fprintf(stderr, "Dev. %s\n", AUTHORS);            /* Authors. */
   fprintf(stderr, "Ref. %s\n", REFERENCES);         /* References. */
   fprintf(stderr, "Ack. %s\n\n", ACKNOWLEDGEMENTS); /* Acknoledgements. */

#ifdef PRINT_ERROR
   fprintf(stderr, "Error logging active.\n");
#endif
#ifdef PRINT_STATUS
   fprintf(stderr, "Progress display on line.\n");
#endif
#ifdef PRINT_DEBUG
   fprintf(stderr, "Debug flag active.\n");
#endif

   fprintf(stderr, "\n");
}



/*----------------------------------------*
 *                                        *
 *   Population emission rates logging.   *
 *                                        *
 *----------------------------------------*/

/*** Global parameters to set in INIT_FILE. ***/
boolean  RatesResults;               /* If true prints the rates. */
char *  RatesFileName = "rates.dat"; /* Name of the log file. */
real    RatesSampling = 0.5;         /* Sampling interval in ms. */
real   RatesStartTime = START_TIME_OFFSET; /* Time at which start the log. */

/*** Local variables. ***/
static FILE            *RatesFile = NULL; /* File header. */


/*---------------*
 *  updateRates  *
 *---------------*/

/**
 *  Updates the number of spikes emitted by the neurons 
 *  in a population from the last sampling of the emission 
 *  rates. The index of the emitting neuron is <n>.
 */

void updateRates (indexn n) /* Emitting neuron index. */
{
   Populations[Neurons[n].Pop->ID].SpikeCounter++;
}


/*------------*
 *  outRates  *
 *------------*/

/**
 *  Logs the number of spikes emitted per unit time
 *  by all the neuron in the populations (the population
 *  emission rate) of the network. The emission rates is 
 *  by the ration between the # of spikes emitted in a 
 *  sampling interval divided by the product of the sampling 
 *  interval and the number of neurons in the population.
 */

void outRates (real t) /* Actual simulation time. */
{
   static real LocalTime;
   static boolean  Start = true;
   static boolean      b;
   static int        i,j;

   /*** Boot. ***/
   if (Start) {
      if (t >= RatesStartTime) {
         LocalTime = RatesStartTime;

         /*** File description. ***/
         fprintf(DocFile, "#----------\n");
         fprintf(DocFile, "# File name: '%s'\n",RatesFileName);
         fprintf(DocFile, "# 1.   Time [ms]\n");
         fprintf(DocFile, "# n>1. Population n-1 emission rates [Hz]\n\n");
      } else
         return;
      Start = false;
   }

   /*** Emission rate log. ***/
   while (LocalTime+RatesSampling < t) {
      fprintf(RatesFile, "%.7g", LocalTime);
      for (i=0; i<NumPopulations; i++) {
         fprintf(RatesFile, " %.7g", (Populations[i].SpikeCounter * 1000.0) /
                                     (Populations[i].N * RatesSampling));
         Populations[i].SpikeCounter = 0;
      }
      fprintf(RatesFile, "\n");

      LocalTime += RatesSampling;
   }
}



/*-----------------------------------------*
 *                                         *
 *   Synaptic transition number logging.   *
 *                                         *
 *-----------------------------------------*/

// NOTES: It is meaningful only for bistable dynamical synapses.

/*** Global parameters to set in INIT_FILE. ***/
boolean  SynTransResults = false;          /* If true prints the number of synaptic transition. */
char *  SynTransFileName = "syntrans.dat"; /* Name of the log file. */
real    SynTransSampling = 50.;            /* Sampling interval in ms. */
real   SynTransStartTime = START_TIME_OFFSET; /* Time at which start the log. */

/*** Local variables. ***/
static int      SynTransUp = -1;   /* Up transition number. */
static int    SynTransDown;        /* Down transition number. */
static FILE * SynTransFile = NULL; /* File header. */


/*------------------*
 *  updateSynTrans  *
 *------------------*/

/**
 *  Updates the number of up and down transitions when a 
 *  synapse changes its long-term state.
 */

void updateSynTrans (int JflagBefore, int JflagAfter) // Up and down state before and after the synaptic update.
{
   if (JflagBefore > JflagAfter)
      SynTransDown++;
   else
      if (JflagBefore < JflagAfter)
        SynTransUp++;
}


/*---------------*
 *  outSynTrans  *
 *---------------*/

/**
 *  Prints the total number of, up and down, synaptic 
 *  transition during the last sampling interval.
 */

void outSynTrans (real t) /* Actual simulation time. */
{
   static int NJup, NJdown;
   static real LocalTime;

   /*** Boot. ***/
   if (SynTransUp == -1)
      if (t >= SynTransStartTime)
      {
         LocalTime = SynTransStartTime;
         SynTransUp = SynTransDown = 0;

         /*** File description. ***/
         fprintf(DocFile, "#----------\n");
         fprintf(DocFile, "# Out file name: '%s'\n", SynTransFileName);
         fprintf(DocFile, "# Synaptic seed: %i\n", SynapsesSeed);
         fprintf(DocFile, "# 1. Time [ms]\n");
         fprintf(DocFile, "# 2. Number of jumps up\n");
         fprintf(DocFile, "# 3. Number of jumps down\n\n");
      } else
         return;

   /*** Number of synaptic transition log. ***/
   while (LocalTime+SynTransSampling < t) {
      fprintf(SynTransFile, "%.7g %d %d\n", LocalTime, SynTransUp, SynTransDown);
      LocalTime += SynTransSampling;
      SynTransUp = SynTransDown = 0;
   }

}



/*-------------------------------*
 *                               *
 *   Synaptic matrix printing.   *
 *                               *
 *-------------------------------*/

/*** Global parameters to set in INIT_FILE. ***/
boolean  SynapsesResults = false; /* If true prints the synaptic matrix. */
char *  SynapsesFileName = "synapses"; /* The mantissa of the name of the log file. */
indexn   SynapsePreStart = 0; /* Range of he synaptic matrix to traverse. */
indexn     SynapsePreEnd = 99;
indexn  SynapsePostStart = 0;
indexn    SynapsePostEnd = 99;

/*** Local variables. ***/
static FILE * SynapsesFile = NULL; /* File header. */
static timex   SynapseTime;        /* Simulation calling time for local purpose. */


/*-------------------*
 *  outSynapsesHook  *
 *-------------------*/

/**
 *  Callback function used in outSynapses to print information
 *  about the single synapses in the matrix range to log. The 
 *  function has to be of InspectFuncPtr type.
 */

void outSynapsesHook(indexn        i, // post-synaptic neuron.
                     indexn        j, // pre-synaptic neuron.
                     void         *s, // pointer to the synapse.
                     connectivity *c, // pointer to the synaptic population.
                     int           l) // Layer corresponding to the transmission delay.
{
   static int k;
   static synapse_state ss;
/*   static struct synapse_state_struct ss; */
   static real lStateVars[MAX_NSSS];

   if (ss.StateVars != lStateVars) 
      ss.StateVars = lStateVars;

   (*c->getSynapseState)(i, j, s, c, l, SynapseTime, &ss);

   fprintf(SynapsesFile, "%d %d %.7g", (int)i, (int)j, SynapticMatrix[l].Delay);
   for (k=0; k<ss.NumStateVars; k++)
      fprintf(SynapsesFile, " %.7g", ss.StateVars[k]);
   fprintf(SynapsesFile, "\n");
}


/*---------------*
 *  outSynapses  *
 *---------------*/

/**
 *  Prints the synaptic state at time <Time> for all the
 *  synapses of the synaptic matrix having as pre and post
 *  synaptic neurons those ones in the range specified by
 *  SynapsePreStart/Stop and SynapsePostStart/Stop.
 *  Each row in the output file is an existing synapse.
 *  The output file name is composed by the string in
 *  SynapsesFileName, and the integer part of <Time>.
 *  NOTES: the procedure can be time expensive, requiring
 *         computational resources proportional to the 
 *         number of the synapses in the network.
 */

void outSynapses (real Time)
{
   char Buffer[80];

   /*** Boot. ***/
   doubleToTimex(Time,SynapseTime);

   /*** Output file opening. ***/
   sprintf(Buffer, "%s%i.dat", SynapsesFileName, (int)Time);
   if ((SynapsesFile = fopen(Buffer, "wt")) == NULL) {
      sprintf(Buffer, "unable to open '%s'.", SynapsesFileName);
      printFatalError("openOutputFiles", Buffer);
   }

   /*** File description. ***/
   fprintf(DocFile, "#----------\n");
   fprintf(DocFile, "# Out file name: '%s%i.dat'\n", SynapsesFileName, (int)Time);
   fprintf(DocFile, "# Post-synaptic neurons between [%i,%i]\n", (int)SynapsePostStart, (int)SynapsePostEnd);
   fprintf(DocFile, "# Pre-synaptic neurons between [%i,%i].\n", (int)SynapsePreStart, (int)SynapsePreEnd);
   fprintf(DocFile, "# 1. POST synaptic neuron (i)\n");
   fprintf(DocFile, "# 2. PRE synaptic neuron (j)\n");
   fprintf(DocFile, "# 3. Trasmission delay [ms]\n");
   fprintf(DocFile, "# n>3. Synaptic state variables\n\n");

   /*** Scanning of the specified part of the synaptic matrix. ***/
   scanSynapticMatrix(SynapsePostStart, SynapsePostEnd, SynapsePreStart, SynapsePreEnd,
                      &outSynapsesHook);

   /*** File closing. ***/
   fclose(SynapsesFile);
}



/*-------------------------------------------*
 *                                           *
 *   Detailed synaptic transition logging.   *
 *                                           *
 *-------------------------------------------*/

/*** Global parameters to set in INIT_FILE. ***/
boolean detailSynTransResults = false;          /* If true prints the detailed synaptic transitions. */
char *detailSynTransFileName = "syndetail.dat"; /* Name of the log file. */

/*** Local variables. ***/
static FILE *detailSynTransFile = NULL; /* File header. */


/*------------------*
 *  detailSynTrans  *
 *------------------*/

/**
 *  Each long-term synaptic transition is logged on a row
 *  of the output file. If the synapse is not dynamic the
 *  file will be empty.
 */

void detailSynTrans (indexn        i, /* Post-synaptic neuron. */
                     indexn        j, /* Pre-synaptic neuron. */
                     int JflagBefore, /* Logical long-term synaptic state before update. */
                     int  JflagAfter, /* Logical long-term synaptic state after update. */
                     timex         t) /* Synaptic update time. */
{
   static int Start = 0;

   if (Start == 0) {

      /*** File description. ***/
      fprintf(DocFile, "#----------\n");
      fprintf(DocFile, "# Out file name: '%s'\n", detailSynTransFileName);
      fprintf(DocFile, "# 1. Post-synaptic neuron (i)\n");
      fprintf(DocFile, "# 2. Pre-synaptic neuron (j)\n");
      fprintf(DocFile, "# 3. Upward (1) or downward (0) long-term synaptic transition\n");
      fprintf(DocFile, "# 4. Synaptic update time [ms]\n\n"); 
      Start = 1;
   }

   if (JflagAfter > JflagBefore)
      fprintf(detailSynTransFile, "%d %d 1 %.7g\n", i, j, timexToDouble(t));
   else if (JflagAfter < JflagBefore)
      fprintf(detailSynTransFile, "%d %d 0 %.7g\n", i, j, timexToDouble(t));
}



/*-------------------------------------------------*
 *                                                 *
 *   Synaptic matrix prints at population level.   *
 *                                                 *
 *-------------------------------------------------*/

/*** Global parameters to set in INIT_FILE. ***/
boolean SynStructResults = false;       /* If true prints the synaptic matrix. */
char  *SynStructFileName = "synstruct"; /* Name of the log file. */

/*** Local variables. ***/
static FILE   *SynStructFile = NULL; /* File header. */
static int       **SynStruct = NULL; /* The matrix of the logical long-term state for synaptic population. */
static int *NumSynPerLTState;        /* Array of number of synapse per long-term state per synapse population. */
static timex   SynStructTime;        /* Simulation calling time for local purpose. */


/*--------------------*
 *  outSynStructHook  *
 *--------------------*/

/**
 *  Callback function used in outSynStruct to print information
 *  about the number of synapse with a given long-term state per
 *  synaptic population (the one listed in Connectivity). The 
 *  function has to be of InspectFuncPtr type.
 */

void outSynStructHook(indexn        i, // post-synaptic neuron.
                      indexn        j, // pre-synaptic neuron.
                      void         *s, // pointer to the synapse.
                      connectivity *c, // pointer to the synaptic population.
                      int           l) // Layer corresponding to the transmission delay.
{
   static synapse_state ss;
   static real lStateVars[MAX_NSSS];

   if (ss.StateVars != lStateVars) 
      ss.StateVars = lStateVars;

   (*c->getSynapseState)(i, j, s, c, l, SynStructTime, &ss);

   if (c->NumSynapseStableState > 1)
      SynStruct[c->ID][(int)ss.StateVars[1]]++;
   else
      SynStruct[c->ID][0]++;
}


/*----------------*
 *  outSynStruct  *
 *----------------*/

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

int outSynStruct (event *Event)
{
   static int N;
   int i, j, k;
   char Buffer[80];

   /*** File opening. ***/
   sprintf(Buffer, "%s%i.dat", SynStructFileName, (int)Event->Time);
   if ((SynStructFile = fopen(Buffer, "wt")) == NULL) {
      sprintf(Buffer, "unable to open '%s'.", SynStructFileName);
      printFatalError("outSynStruct", Buffer);
   }

   if (SynStruct == NULL) {

      /*** Support structures creation. ***/
      SynStruct = getMemory(sizeof(*SynStruct) * NumConnectivityArray, "ERROR (outSynStruct): Out of memory (SynStruct).\n");

      /*** Carries out the total number of long-term synaptic states and allocates the needed memory. ***/
      for (N = 0, i=0; i<NumConnectivityArray; i++)
         N += ConnectivityArray[i].NumSynapseStableState;
      NumSynPerLTState = getMemory(sizeof(*NumSynPerLTState) * N, "ERROR (outSynStruct): Out of memory (NumSynPerLTState).\n");

      /*** Links the support structures. ***/
      for (N = 0, i=0; i<NumConnectivityArray; i++) {
         SynStruct[i] = &NumSynPerLTState[N];
         N += ConnectivityArray[i].NumSynapseStableState;
      }
   }

   /*** Boots the support structures. ***/
   for (i=0; i<N; i++)
      NumSynPerLTState[i] = 0;

   /*** File description. ***/
   fprintf(DocFile, "#----------\n");
   fprintf(DocFile, "# Out file name: '%s%i.dat'\n", SynStructFileName, (int)Event->Time);
   fprintf(DocFile, "# 1. Post-synaptic neuron population\n");
   fprintf(DocFile, "# 2. Pre-synaptic neuron population\n");
   fprintf(DocFile, "# n+2. Number of synapse in the n-th long-term state per synaptic population\n\n");

   /*** Synaptic matrix scanning. ***/
   scanSynapticMatrix(0, NumNeurons-1, 0, NumNeurons-1, outSynStructHook);

   /*** Prints the number of synapses for the long-term states allowed per connectivity element. ***/
   for (i=0; i<NumPopulations; i++)
      for (j=0; j<NumPopulations; j++) 
         if (Connectivity[i][j] != NULL) {
            fprintf(SynStructFile, "%d %d", i, j);
            for (k=0; k<Connectivity[i][j]->NumSynapseStableState; k++)
               fprintf(SynStructFile, " %d", SynStruct[Connectivity[i][j]->ID][k]);
            fprintf(SynStructFile, "\n");
         }

   /*** File closing. ***/
   fclose(SynStructFile);

   return 1;
}



/*--------------------------------------------*
 *                                            *
 *   Synaptic structure of dendritic trees.   *
 *                                            *
 *--------------------------------------------*/

/*** Global parameters to set in INIT_FILE. ***/
boolean DenStructResults = false;       /* If true prints the synaptic structures of dendritic trees. */
char  *DenStructFileName = "denstruct"; /* Name of the log file. */

/*** Local variables. ***/
static FILE     *DenStructFile = NULL; /* File header. */
static int        ***DenStruct = NULL; /* The matrix of the logical long-term state for neuron and pre-synaptic population. */
static int *DSNumSynPerLTState;        /* Array of number of synapse per long-term state. */
static timex     DenStructTime;        /* Simulation calling time for local purpose. */


/*--------------------*
 *  outDenStructHook  *
 *--------------------*/

/**
 *  Callback function used in outDenStruct to print information
 *  about the number of synapse with a given long-term state per
 *  synaptic population and post-synaptic neuron. The function 
 *  has to be of InspectFuncPtr type.
 */

void outDenStructHook(indexn        i, // post-synaptic neuron.
                      indexn        j, // pre-synaptic neuron.
                      void         *s, // pointer to the synapse.
                      connectivity *c, // pointer to the synaptic population.
                      int           l) // Layer corresponding to the transmission delay.
{
   static synapse_state ss;
   static real lStateVars[MAX_NSSS];

   if (ss.StateVars != lStateVars) 
      ss.StateVars = lStateVars;

   (*c->getSynapseState)(i, j, s, c, l, SynStructTime, &ss);

   if (c->NumSynapseStableState > 1)
      DenStruct[Neurons[j].Pop->ID][i][(int)ss.StateVars[1]]++;
   else
      DenStruct[Neurons[j].Pop->ID][i][0]++;
}


/*----------------*
 *  outDenStruct  *
 *----------------*/

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

int outDenStruct (event *Event) 
{
   static int N;
   int i, j, k;
   char Buffer[80];

   /*** Output file opening. ***/
   sprintf(Buffer, "%s%i.dat", DenStructFileName, (int)(Event->Time));
   if ((DenStructFile = fopen(Buffer, "wt")) == NULL) {
      sprintf(Buffer, "unable to open '%s'.", DenStructFileName);
      printFatalError("outDenStruct", Buffer);
   }

   if (DenStruct == NULL) {

      /*** Support structures creation. ***/
      DenStruct = getMemory(sizeof(*DenStruct) * NumPopulations, "ERROR (outSynStruct): Out of memory (DenStruct).\n");
      for (i=0; i<NumPopulations; i++)
         DenStruct[i] = getMemory(sizeof(**DenStruct) * NumNeurons, "ERROR (outSynStruct): Out of memory (*DenStruct).\n");

      /*** Carries out the total number of long-term synaptic states and allocates the needed memory. ***/
      N = 0;
      for (i=0; i<NumPopulations; i++)
         for (j=0; j<NumPopulations; j++)
            if (Connectivity[i][j] != NULL)
               N += Connectivity[i][j]->NumSynapseStableState * Populations[i].N;
      DSNumSynPerLTState = getMemory(sizeof(*DSNumSynPerLTState) * N, "ERROR (outDenStruct): Out of memory (DSNumSynPerLTState).\n");

      /*** Links the support structures. ***/
      N = 0;
      for (i=0; i<NumPopulations; i++)
         for (j=0; j<(int)NumNeurons; j++)
            if (Connectivity[Neurons[j].Pop->ID][i] != NULL) {
               DenStruct[i][j] = &DSNumSynPerLTState[N];
               N += Connectivity[Neurons[j].Pop->ID][i]->NumSynapseStableState;
            } else
               DenStruct[i][j] = NULL;
   }

   /*** Boots the support structures. ***/
   for (i=0; i<N; i++)
      DSNumSynPerLTState[i] = 0;
   doubleToTimex(Event->Time, DenStructTime);

   /*** File description. ***/
   fprintf(DocFile, "#----------\n");
   fprintf(DocFile, "# Out file name: '%s%i.dat'\n", SynStructFileName, (int)(Event->Time));
   fprintf(DocFile, "# 1. Post-synaptic neuron\n");
   fprintf(DocFile, "# 2. Pre-synaptic neuron population\n");
   fprintf(DocFile, "# n+2. Number of synapse in the n-th long-term state per synaptic population\n\n");

   /*** Synaptic matrix scanning. ***/
   scanSynapticMatrix(0, NumNeurons-1, 0, NumNeurons-1, outDenStructHook);

   /*** Prints the number of synapses for the long-term states allowed per neuron and connectivity element. ***/
   for (i=0; i<NumPopulations; i++)
      for (j=0; j<(int)NumNeurons; j++) 
         if (DenStruct[i][j] != NULL) {
            fprintf(DenStructFile, "%d %d", j, i);
            for (k=0; k<Connectivity[Neurons[j].Pop->ID][i]->NumSynapseStableState; k++)
               fprintf(DenStructFile, " %d", DenStruct[i][j][k]);
            fprintf(DenStructFile, "\n");
         }

   /*** File closing. ***/
   fclose(DenStructFile);

   return 1;
}



/*-----------------------------*
 *                             *
 *   Emitted spikes logging.   *
 *                             *
 *-----------------------------*/

/*** Global parameters to set in INIT_FILE. ***/
boolean SpikesResults = false;        /* If true prints the emission time. */
char  *SpikesFileName = "spikes.dat"; /* Name of the log file. */
indexn     SpikeStart = 0;            /* First neuron whose activity have to be recorded. */
indexn       SpikeEnd = 99;           /* Last neuron whose activity have to be recorded. */

/*** Local variables. ***/
static FILE * SpikesFile = NULL; /* File header. */


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
               timex  t) /* Emission time. */
{
   static int Start = 0;

   if (Start == 0)
   {
      fprintf(DocFile, "#----------\n");
      fprintf(DocFile, "# Out file name: '%s'\n", SpikesFileName);
      fprintf(DocFile, "# Neurons between [%i,%i].\n", (int)SpikeStart, (int)SpikeEnd);
      fprintf(DocFile, "# 1. Emitting Neuron\n");
      fprintf(DocFile, "# 2. Emission Time [ms].\n\n");
      Start = 1;
   }

   if (n >= SpikeStart && n <= SpikeEnd)
      fprintf(SpikesFile, "%d %.7g\n", n, timexToDouble(t));
}


/**
 *  Turn on the switch to dump the spikes.
 */

int startSpikeRecording(event *Event)
{
   SpikesResults = true;

   return 1;
}


/**
 *  Turn on the switch to dump the spikes.
 */

int stopSpikeRecording(event *Event)
{
   SpikesResults = false;

   return 1;
}


/*------------------------------------------*
 *                                          *
 *   Asynchronous SYNAPTIC STATE logging.   *
 *                                          *
 *------------------------------------------*/

/*** Global parameters to set in INIT_FILE. ***/
boolean  SynStateResults = false;          /* If true prints the synaptic states. */
char   *SynStateFileName = "synstate.dat"; /* Name of the log file. */
indexn  SynStatePreStart = 0;              /* Synaptic population whose activity have to be recorded.  */
indexn    SynStatePreEnd = 9;              /* It is defined with the first and the last pre- and post- */
indexn SynStatePostStart = 0;              /* synaptic neuron of the involved synapses.                */
indexn   SynStatePostEnd = 9;

/*** Local variables. ***/
static FILE *SynStateFile = NULL; /* File header. */
static timex SynStateTime;        /* Simulation calling time for local purpose. */


/*---------------------*
 *   outSynapticState  *
 *---------------------*/

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
                       ...)
{
   static int k;
   static va_list marker;

   if (i >= SynStatePostStart && i <= SynStatePostEnd &&
      j >= SynStatePreStart && j <= SynStatePreEnd) {
      fprintf(SynStateFile, "%d %d %.7g", i, j, timexToDouble(t));
      va_start(marker, NSV);
      for (k=0; k<NSV; k++)
         fprintf(SynStateFile, " %.7g", va_arg( marker, real));
      fprintf(SynStateFile, "\n");
      va_end(marker);
   }
}


/*--------------------------*
 *  flushSynapticStateHook  *
 *--------------------------*/

/**
 *  Callback function used in flushSynapticState to print information
 *  about the single synapses in the matrix range to log. The 
 *  function has to be of InspectFuncPtr type.
 */

void flushSynapticStateHook(indexn        i, // post-synaptic neuron.
                            indexn        j, // pre-synaptic neuron.
                            void         *s, // pointer to the synapse.
                            connectivity *c, // pointer to the synaptic population.
                            int           l) // Layer corresponding to the transmission delay.
{
   static int k;
   static synapse_state ss;
   static real lStateVars[MAX_NSSS];

   if (ss.StateVars != lStateVars) 
      ss.StateVars = lStateVars;

   (*c->getSynapseState)(i, j, s, c, l, SynStateTime, &ss);

   fprintf(SynStateFile, "%d %d %.7g", i, j, timexToDouble(SynStateTime));
   for (k=0; k<ss.NumStateVars; k++)
      fprintf(SynStateFile, " %.7g", ss.StateVars[k]);
   fprintf(SynStateFile, "\n");
}


/*-----------------------*
 *   flushSynapticState  *
 *-----------------------*/

/**
 *  Prints a snapshot of synapse states in the sub-matrix 
 *  specified in the initialization file. It is useful
 *  as the first and last print during the recording of
 *  the asynchronous updated synaptic states.
 */

void flushSynapticState (double Time)
{
   static int Start = 0;

   if (Start == 0) {

      /*** File description. ***/
      fprintf(DocFile, "#----------\n");
      fprintf(DocFile, "# Out file name: '%s'\n", SynStateFileName);
      fprintf(DocFile, "# Post-synaptic neurons between [%i,%i].\n", (int)SynStatePostStart, (int)SynStatePostEnd);
      fprintf(DocFile, "# Pre-synaptic neurons between [%i,%i].\n", (int)SynStatePreStart, (int)SynStatePreEnd);
      fprintf(DocFile, "# 1. Post-synaptic neuron (i)\n");
      fprintf(DocFile, "# 2. Pre-synaptic neuron (j)\n");
      fprintf(DocFile, "# 3. Record time [ms]\n");
      fprintf(DocFile, "# n>3. State variables\n\n"); 
      Start = 1;
   }
   doubleToTimex(Time, SynStateTime);

   /*** Scanning of the specified part of the synaptic matrix. ***/
   scanSynapticMatrix(SynStatePostStart, SynStatePostEnd, SynStatePreStart, SynStatePreEnd,
                      flushSynapticStateHook);
}



/*------------------------------------------*
 *                                          *
 *   Asynchronous NEURONAL STATE logging.   *
 *                                          *
 *------------------------------------------*/

/*** Global parameters to set in INIT_FILE. ***/
boolean NeuStateResults = false;          /* If true prints the neuronal state. */
char  *NeuStateFileName = "neustate.dat"; /* Name of the log file. */
indexn    NeuStateStart = 0;              /* First neuron to record the activity. */
indexn      NeuStateEnd = 9;              /* Last neuron to record the activity. */

/*** Local variables. ***/
static FILE * NeuStateFile = NULL; /* File header. */


/*--------------------*
 *  outNeuronalState  *
 *--------------------*/

/**
 *  Logs each asynchronous update of the neurons in the
 *  range of interest. Each update corresponds to a row
 *  in the output file, indicating the neuron index, the 
 *  time of the event and the state variables like the 
 *  membrane potential.
 */

void outNeuronalState (indexn i, // Neuron.
                       timex  t, // Time at which occurs the update.
                       int  NSV, // Number of state variables passed as a list of variable parameters. 
                       ...)
{
   static int k;
   static va_list marker;

   if (i >= NeuStateStart && i <= NeuStateEnd) {
      fprintf(NeuStateFile, "%d %.7g", i, timexToDouble(t));
      va_start(marker, NSV);
      for (k=0; k<NSV; k++)
         fprintf(NeuStateFile, " %.7g", va_arg( marker, real));
      fprintf(NeuStateFile, "\n");
      va_end(marker);
   }
}


/*-----------------------*
 *   flushNeuronalState  *
 *-----------------------*/

/**
 *  Prints a snapshot of the state of a neuron subset
 *  specified in the initialization file. It is useful
 *  as the first and last print during the recording of
 *  the asynchronous updated neuronal states.
 */

void flushNeuronalState (double Time)
{
   static int i, k;
   static int Start = 0;
   static timex   t;
   static neuron_state ns;
   static real lStateVars[MAX_NSSS];

   if (Start == 0) {

      /*** File description. ***/
      fprintf(DocFile, "#----------\n");
      fprintf(DocFile, "# Out file name: '%s'\n", NeuStateFileName);
      fprintf(DocFile, "# Neurons between [%i,%i].\n", NeuStateStart, NeuStateEnd);
      fprintf(DocFile, "# 1. Neuron (i)\n");
      fprintf(DocFile, "# 2. Record time [ms].\n");
      fprintf(DocFile, "# n>2. State variables\n\n"); 

      ns.StateVars = lStateVars;
      Start = 1;
   }
   doubleToTimex(Time, t);

   /*** Scanning of the specified subset of neurons. ***/
   for (i=(int)NeuStateStart; i<=(int)NeuStateEnd; i++) {
      (*getNeuronState)(i, t, &ns);

      fprintf(NeuStateFile, "%d %.7g", i, Time);
      for (k=0; k<ns.NumStateVars; k++)
         fprintf(NeuStateFile, " %.7g", ns.StateVars[k]);
      fprintf(NeuStateFile, "\n");
   }
}



/*-------------------------*
 *                         *
 *   Stimuli composition   *
 *                         *
 *-------------------------*/

/*** Local variables. ***/
static char *StimuliFileName = "stimuli.dat"; /* Name of the log file. */
static FILE     *StimuliFile = NULL;          /* File header. */


/*--------------*
 *  outStimuli  *
 *--------------*/

/**
 *  Prints the composition of the different defined stimuli.
 *  TO DO: At this time it is not implemented being not 
 *         implemented the multi-stimuli management, and 
 *         the function provide an empty log file.
 */

void outStimuli ()
{
   char Buffer[80];

   /*** File description. ***/
   fprintf(DocFile, "#----------\n");
   fprintf(DocFile, "# Out file name: '%s'\n", StimuliFileName);
   fprintf(DocFile, "# 1. Stimulus number,\n");
   fprintf(DocFile, "# 2. Neuron index.\n\n");

   /*** Output file opening. ***/
   if ((StimuliFile = fopen(StimuliFileName, "wt")) == NULL) {
      sprintf(Buffer, "unable to open '%s'.", SynapsesFileName);
      printFatalError("outStimuli", Buffer);
   }

   /*** Stimuli composition scanning. ***/
   // TO DO...

   /*** File closure. ***/
   fclose(StimuliFile);
}



/*-------------------------------*
 *                               *
 *   Afferent current logging.   *
 *                               *
 *-------------------------------*/

/*** Global parameters to set in INIT_FILE. ***/
boolean CurrentResults = false;             /* If true prints the afferent current to the selected neuron. */
char  *CurrentFileName = "current.dat";     /* Name of the log file. */
real   CurrentSampling = 1.0;               /* Sampling period in ms. */
real  CurrentStartTime = START_TIME_OFFSET; /* Starting time of the registration. */
indexn   CurrentNeuron = 9;                 /* Neuron whose current have to be logged.. */

/*** Local variables. ***/
static real      *Charge = NULL; /* Charge received from the last sampling time per each pre-synaptic population. */
static FILE *CurrentFile = NULL; /* File header. */


/*-----------------*
 *  updateCurrent  *
 *-----------------*/

/**
 *  Update the charge accumulated from the last sampling time.
 *  dividing currents from due to different pre-synaptic populations.
 */

void updateCurrent (indexn            i, // Receiving neuron.
                    int               j, // Pre-synaptic neuron or -1.
                    real IncomingCharge) // Charge due to an incoming spike.
{
   if (i == CurrentNeuron) 
      if (j >= 0)
         Charge[Neurons[j].Pop->ID] += IncomingCharge;
      else
         Charge[NumPopulations] += IncomingCharge;
}


/*--------------*
 *  outCurrent  *
 *--------------*/

/**
 *  Prints each sampling period the charge accumulated 
 *  per unit time by the selected neuron, due to the 
 *  incoming synaptic current.
 */

void outCurrent (real Time)
{
   static real LocalTime;
   static int  Start = 0;
   static int i;

   /*** Boot. ***/
   if (Start == 0)
      if (Time >= CurrentStartTime)
      {
         LocalTime = CurrentStartTime;
         Start = 1;

         /*** Support structures creation. ***/
         Charge = getMemory(sizeof(*Charge) * (NumPopulations+1), "ERROR (outCurrent): Out of memory.\n");
         for (i=0; i<=NumPopulations; i++)
            Charge[i] = 0.0;

         /*** File description. ***/
         fprintf(DocFile, "#----------\n");
         fprintf(DocFile, "# Out file name: '%s'\n", CurrentFileName);
         fprintf(DocFile, "# Neuron: %i\n", CurrentNeuron);
         fprintf(DocFile, "# 1. Time [ms]\n");
         fprintf(DocFile, "# n>1. Afferent current from the different neuron populations (external included)  [a.u./s]\n\n");
      } else
         return;

   /*** Prints the average current received in a sampling period. ***/
   while (LocalTime+CurrentSampling < Time) {
      fprintf(CurrentFile, "%.7g", LocalTime);
      for (i=0; i<=NumPopulations; i++) {
         fprintf(CurrentFile, " %.7g", Charge[i]/CurrentSampling*1000.0);
         Charge[i] = 0.0;
      }
      fprintf(CurrentFile, "\n");
      LocalTime += CurrentSampling;
   }
}



/*-----------------------------------------*
 *                                         *
 *   Opening and closure of output files   *
 *                                         *
 *-----------------------------------------*/


/**
 *  Opens the output files.
 */

void openOutputFiles ()
{
   char Buffer[80];

   /*** Emission rates. ***/
   if (RatesResults)
      if ((RatesFile = fopen(RatesFileName, "wt")) == NULL) {
         sprintf(Buffer, "unable to open '%s'.", RatesFileName);
         printFatalError("openOutputFiles", Buffer);
      }

   /*** File containing a description of the opened file. ***/
   if ((DocFile = fopen(DocFileName, "wt")) == NULL) {
      sprintf(Buffer, "unable to open '%s'.", DocFileName);
      printFatalError("openOutputFiles", Buffer);
   }

   /*** Number of synaptic transition. ***/
   if (SynTransResults)
      if ((SynTransFile = fopen(SynTransFileName, "wt")) == NULL) {
         sprintf(Buffer, "unable to open '%s'.", SynTransFileName);
         printFatalError("openOutputFiles", Buffer);
      }

   /*** Detailed synaptic transitions. ***/
   if (detailSynTransResults)
      if ((detailSynTransFile = fopen(detailSynTransFileName, "wt")) == NULL) {
         sprintf(Buffer, "unable to open '%s'.", detailSynTransFileName);
         printFatalError("openOutputFiles", Buffer);
      }

   /*** Emitted spikes. ***/
   if (SpikesResults)
      if ((SpikesFile = fopen(SpikesFileName, "wt")) == NULL) {
         sprintf(Buffer, "unable to open '%s'.", SpikesFileName);
         printFatalError("openOutputFiles", Buffer);
      }


   /*** Dynamics of the synaptic "potential". ***/
   if (SynStateResults)
      if ((SynStateFile = fopen(SynStateFileName, "wt")) == NULL) {
         sprintf(Buffer, "unable to open '%s'.", SynStateFileName);
         printFatalError("openOutputFiles", Buffer);
      }

   /*** Dynamics of the neuron state. ***/
   if (NeuStateResults)
      if ((NeuStateFile = fopen(NeuStateFileName, "wt")) == NULL) {
         sprintf(Buffer, "unable to open '%s'.", NeuStateFileName);
         printFatalError("openOutputFiles", Buffer);
      }

   /*** Afferent current. ***/
   if (CurrentResults)
      if ((CurrentFile = fopen(CurrentFileName, "wt")) == NULL) {
         sprintf(Buffer, "unable to open '%s'.", CurrentFileName);
         printFatalError("openOutputFiles", Buffer);
      }
}


/**
 *  Flushes the output files.
 */

void flushOutputFiles ()
{
   if (RatesResults) fflush(RatesFile);
   if (SynTransResults) fflush(SynTransFile);
   if (detailSynTransResults) fflush(detailSynTransFile);
   if (SpikesResults) fflush(SpikesFile);
   if (SynStateResults) fflush(SynStateFile);
   if (NeuStateResults) fflush(NeuStateFile);
   if (CurrentResults) fflush(CurrentFile);
   fflush(DocFile);
}


/**
 *  Closes the output files.
 */

void closeOutputFiles ()
{
   if (RatesResults) fclose(RatesFile);
   if (SynTransResults) fclose(SynTransFile);
   if (detailSynTransResults) fclose(detailSynTransFile);
   if (SpikesResults) fclose(SpikesFile);
   if (SynStateResults) fclose(SynStateFile);
   if (NeuStateResults) fclose(NeuStateFile);
   if (CurrentResults) fclose(CurrentFile);
   fclose(DocFile);
}
