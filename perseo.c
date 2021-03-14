/*
 *
 *   perseo.c
 *
 *   The core of the simulation where the external spikes are created 
 *   and managed, and the dynamic variables are updated.
 *   The main function is here.
 *
 *   Project: PERSEO 2.x
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>

#include "types.h"
#include "queue.h"
#include "timer.h"
#include "invar.h"
#include "randdev.h"

#include "perseo.h"
#include "init.h"
#include "results.h"
#include "stimuli.h"
#include "events.h"
#include "commands.h"

#include "modules.h"
#include "delays.h"
#include "neurons.h"



/*----------------*
 *  LOCAL MACROS  *
 *-------------- -*/

#define NULL_LAYER -1   /* The null pointer to a delay layer. */



/*----------------------*
 *   GLOBAL VARIABLES   *
 *----------------------*/

real                 Life;         /* Life time of the simulation in ms. */
int           NeuronsSeed;         /* Seed of pseudo-random number generator for the initialization of neuron states. */
int          SynapsesSeed;         /* Seed of pseudo-random number generator for the initialization of synapse states. */
boolean    QuitSimulation = false; /* If true the simulation interrupts. Is changed *
                                    * when a signal is sent to the process.         */
unsigned int MemoryAmount = 0;     /* The amount of memory allocated by Perseo. */



/*-------------------*
 *  LOCAL VARIABLES  *
 *-------------------*/

int OldestLayer = NULL_LAYER; /* The delay layer containing the oldest spike. */
void findOldestLayer ();      /* Local function used in a global function. */



/*--------------------*
 *  GLOBAL FUNCTIONS  *
 *--------------------*/

/*-----------------------*
 *  interruptSimulation  *
 *-----------------------*/

/**
 *  Management of the external interrupt signals (^C or stop 
 *  signals from other processes).
 */

void interruptSimulation (int Signal)
{
   QuitSimulation = true;
   if (Signal == SIGINT)
      fprintf (stderr, "\nSimulation manual stopping (^C).\n");
   else
      fprintf (stderr, "\nSimulation manual stopping.\n");
}


/*--------------*
 *  printError  *
 *--------------*/

/**
 *  Prints an error on stderr, without
 *  closing the application.
 */

void printError(char *FunctionName, char *ErrorMessage)
{
#ifdef PRINT_ERROR
   fprintf(stderr, "ERROR (%s): %s", FunctionName, ErrorMessage);
#endif
}


/*-------------------*
 *  printFatalError  *
 *-------------------*/

/**
 *  Prints a fatal error on stderr and
 *  close the application.
 */

void printFatalError(char *FunctionName, char *ErrorMessage)
{
#ifdef PRINT_ERROR
   fprintf(stderr, "\nERROR (%s): %s", FunctionName, ErrorMessage);
#endif

   exit( EXIT_FAILURE );
}


/*-------------*
 *  getMemory  *
 *-------------*/

/**
 *  It is a customized version of standard malloc function
 *  which return a pointer to the allocated memory of size
 *  <Size>. In addion updates a counter of memory usage and
 *  exit program with an error message <ErrorMessage>.
 */

void *getMemory(size_t        Size, // Size in byte of memory to allocate.
                char *ErrorMessage) // Error message if the allocation process fails.
{
   static void *ptr;
            
   if ((ptr = malloc(Size)) == NULL) {
#ifdef PRINT_ERROR
      fprintf(stderr, "\n%s", ErrorMessage);
#endif
      exit( EXIT_FAILURE );
   } else {
#ifdef PRINT_STATUS
      MemoryAmount += Size;
#endif
   }

   return ptr;
}


/*----------------*
 *   addNewSpike  *
 *----------------*/

/**
 *  Adds an event to the queue of the selected layer.
 */

void addNewSpike (
                  indexn n,   /* Emitting neuron.                       */
                  timex  t,   /* Emission time                          */
                  real ISI,   /* ISI from the last event.               */
                  int    l    /* Delay layer where the event is loaded. */
                 )
{
   static spike sp;   /* Local variable . */
   
   /*** TEMP: Some output... It should be managed using the event queue. ***/
   if (l==0) {
      if (RatesResults) updateRates(n);
      if (SpikesResults) outSpike(n, t);
   }

   /*** Boot. ***/
   sp.Emission = t;
   sp.Neuron   = n;
   sp.ISI      = ISI;

   /*** Is the queue of the layer empty? ***/
   if (SynapticMatrix[l].Empty) {
      SynapticMatrix[l].Spike = sp;
      SynapticMatrix[l].Empty = false;
      findOldestLayer();
   } else
      putQueueElement(&(SynapticMatrix[l].Queue), &sp);
}


/*-------------------*
 *  LOCAL FUNCTIONS  *
 *-------------------*/


/*----------------------*
 *  ariseExternalSpike  *
 *----------------------*/

/**
 *  Extract the oldest external spikes loaded in different 
 *  populations, storing it in *ExtSpike. A new
 *  external spike is scheduled with an exponential interval
 *  (external spike trains are indendent Poissonian processes).
 *  The receiving neuron is selected randomly from the ones
 *  belonging to the related population.
 */

void ariseExternalSpike (spike * ExtSpike)
{
   static int             i,j,k; /* Local variables. */
   static int         OldestPop;
   static timex *OldestEmission;

   /*** TEMP: The search code has to be optimized using AVL ***
    ***       tree expecially for large population numbers. ***/

   /*** Search for the oldest external spike. ***/
   OldestEmission = &(Populations[0].Emission);
   OldestPop = 0;
   for (i=1; i<NumPopulations; i++)
     if (diffTimex(*OldestEmission,Populations[i].Emission)>0.0) {
       OldestPop = i;
       OldestEmission = &(Populations[i].Emission);
     }

   /*** Select the receiving post-synaptic neuron... ***/
   ExtSpike->Emission = *OldestEmission;
   j = (indexn)(Populations[OldestPop].N * Random());
   ExtSpike->Neuron = (indexn)(&(Populations[OldestPop].Neurons[j]) - Neurons);

   /*** Time to the next external spike delivered to the ***
    *** population with the oldest external spike.       ***/
   Populations[OldestPop].Emission.Millis -= Populations[OldestPop].InvNuExt * log(1-Random());

   /*** Compression of the time representation. ***/
   if (Populations[OldestPop].Emission.Millis > 1.0) {
      i = (int)(Populations[OldestPop].Emission.Millis);
      Populations[OldestPop].Emission.Seconds += i;
      Populations[OldestPop].Emission.Millis -= (double)i;
   }
}


/*----------------------*
 *  whereIsOldestSpike  *
 *----------------------*/

/**
 *  Return the Layer index containing the oldest 
 *  spike to manage. It is NULL_LAYER if no recurrent
 *  spikes are available or the oldest spike is external. 
 */

int whereIsOldestSpike (spike * ExtSpike)
{
   /*** Are recurrent spikes available? ***/
   if (OldestLayer == NULL_LAYER)
      return NULL_LAYER;
   else
      /*** Is the oldest external spike older than the internal one? ***/
      if (diffTimex(ExtSpike->Emission, SynapticMatrix[OldestLayer].Spike.Emission) <= 0.0)
         return NULL_LAYER;

   /*** The oldest spikes is recurrent. ***/
   return OldestLayer;
}



/*--------------------*
 *   findOldestLayer  *
 *--------------------*/

/**
 *   Determina il Layer che ha l'impulso da gestire piu'   *
 *   vecchio aggiornando la variabile OldestLayer.         *
 */

void findOldestLayer ()
{
   static int   i;
   static timex t;

   /*** Which layer have the oldest spike to manage? ***/
   OldestLayer = NULL_LAYER;
   doubleToTimex(Life + 100.0, t); /* A maximum time not reachable. */
   for (i=0; i<DelayNumber; i++)
      if (diffTimex(t, SynapticMatrix[i].Spike.Emission) > 0.0 &&
          !SynapticMatrix[i].Empty) {
         OldestLayer = i;
         t           = SynapticMatrix[i].Spike.Emission;
      }
}


/*----------------------*
 *  endSpikeManagement  *
 *----------------------*/

/**
 *  Moves the managed event to the next layer and 
 *  gets the next event of the layer queue.
 */

void endSpikeManagement (int DelayLayer)
{
   /*** Moves the managed event to the next layer. ***/
   if (DelayLayer < DelayNumber-1) {
      SynapticMatrix[DelayLayer].Spike.Emission.Millis += DelayStep;
      addNewSpike(SynapticMatrix[DelayLayer].Spike.Neuron,
                  SynapticMatrix[DelayLayer].Spike.Emission,
                  SynapticMatrix[DelayLayer].Spike.ISI, DelayLayer+1);
   }

   /*** Gets the next event of the layer queue. ***/
   if (isQueueEmpty(SynapticMatrix[DelayLayer].Queue))
      SynapticMatrix[DelayLayer].Empty = true;
   else
      getQueueElement(&(SynapticMatrix[DelayLayer].Queue), 
                      &(SynapticMatrix[DelayLayer].Spike));

   findOldestLayer();
}



/*--------------*
 *  simulation  *
 *--------------*/

/**   
 *  Computes the dynamic evolution of the initialized
 *  network of neurons. This is the core function of 
 *  PERSEO which integrates in an asynchronous way the
 *  dynamical equation of the system.
 */

void simulation (void)
{
   int              l; /* Delay layer having the oldest spike. */
   spike     ExtSpike; /* The external spike to manage. */
   spike     IntSpike; /* The internal spike (from a local neuron) to manage. */
   axon_segment * Pre; /* Pointer to the "axon" of the emitting neuron. */
   indexn           i; /* Scanning index of the synapses on the Pre axon. */
   int           Post; /* Post synaptic neuron to update. */
   indexn      nExcep; /* Number of the exceptions on the Pre axon. */
   real          Time; /* The actual network simulation time in ms. */
   byte         *pSyn; /* Pointer to a synapse. */
   char OutString[40]; /* Output local variable. */


#ifdef PRINT_STATUS
   double    Status = 0.0;    /* Progress status. */
   double IncStatus = 10.0;/* Sampling period of the progress status. */
#endif

   /*** Initializes local variables. ***/
   Time = START_TIME_OFFSET;
   ariseExternalSpike(&ExtSpike);
   OutString[0] = '\0';

   /*** TEMP: Some output... It should be managed using the event queue. ***/
   if (SynapsesResults) outSynapses(0);

#ifdef PRINT_STATUS
   startTimer();

   /*** Shows on the console the progress status. ***/
   fprintf(stderr, "\n\nNetwork Time %.7g ms (Memory: %g Mbytes)\r", Status, (real)MemoryAmount/1024.0/1024.0);
#endif

   /*** TEMP: Some output... It should be managed using the event queue. ***/
   if (SynStateResults) flushSynapticState(Time);
   if (NeuStateResults) flushNeuronalState(Time);

   /*** Main loop managing the spikes and events in the simulation. ***/
   while (Life > Time && !QuitSimulation) {

      /*** Manages all the events, if any, with time label lower than Time. ***/
      manageEvent(Time);

      /*** Is the oldest spike from outside? ***/
      l = whereIsOldestSpike(&ExtSpike);
      if (l == NULL_LAYER) {

         /*** The spike to manage comes from outside. ***/
         Time = timexToDouble(ExtSpike.Emission);

         /*** TEMP: Some output... It should be managed using the event queue. ***/
         if (RatesResults) outRates(Time);
         if (CurrentResults) outCurrent(Time);

         /*** Updates the neuron state. ***/
         (*updateNeuronState)(ExtSpike.Neuron, NULL, &ExtSpike);

         /*** Gets a new external spike. ***/
         ariseExternalSpike(&ExtSpike);

      } else {

         /*** The spike to manage comes from a neuron of the network. ***/
         IntSpike  = SynapticMatrix[l].Spike;
         Pre = &(SynapticMatrix[l].Pre[IntSpike.Neuron]);
         Time = timexToDouble(IntSpike.Emission);

         /*** TEMP: Some output... It should be managed using the event queue. ***/
         if (RatesResults) outRates(Time);
         if (SynTransResults) outSynTrans(Time);
         if (CurrentResults) outCurrent(Time);
         
         /*** Loop on the synaptically connected post-synaptic neurons. ***/
         Post   = -1;
         nExcep = 0;
         pSyn = Pre->Synapses;
         for (i=0; i<Pre->NumSynapses; i++) {

            /*** Computes the next post-synaptic neuron index. ***/
            if (Pre->DPost[i] != EXCEPTION)
               Post += Pre->DPost[i];
            else
               Post = Pre->Exception[nExcep++];

            /*** Updates the neuron state. ***/
            (*updateNeuronState)(Post, pSyn, &IntSpike);

            /*** Points to the next synapse on the axon. ***/
            pSyn += Connectivity[Neurons[Post].Pop->ID][Neurons[IntSpike.Neuron].Pop->ID]->SynapseSize;
         }

         /*** Updates the queue of the delay layer from which the managed spikes comes. ***/
         endSpikeManagement(l);
      }

#ifdef PRINT_STATUS
      /*** Shows on the console the progress status. ***/
      if (Time > Status + IncStatus) {
         Status = (int)(Time / IncStatus) * IncStatus;
         fprintf(stderr, "Network Time %.7g ms (Memory: %g Mbytes)\r", Status, (real)MemoryAmount/1024.0/1024.0);

      }
#endif

      /*** Reads new commands from the corresponding input file. ***/
      readCommands(&Time);
   }

   /*** TEMP: Some output... It should be managed using the event queue. ***/
   if (Time > Life) {
      if (SynStateResults) flushSynapticState(Life);
      if (NeuStateResults) flushNeuronalState(Life);
   } else {
      if (SynStateResults) flushSynapticState(Time);
      if (NeuStateResults) flushNeuronalState(Time);
   }


#ifdef PRINT_STATUS
   /*** Shows on the console the progress status. ***/
   fprintf(stderr, "Network Time %.7g ms (Memory: %g Mbytes)\r", Life, (real)MemoryAmount/1024.0/1024.0);

   elapseTimer();
   fprintf(stderr, "\n\nElapsed Time: %ss\n", timer(OutString));
#endif

   /*** TEMP: Some output... It should be managed using the event queue. ***/
   if (SynapsesResults) outSynapses(Life);
}



/*----------*
 *          *
 *   MAIN   *
 *          *
 *----------*/

/**
 *  Loads and interprets definition and command files, 
 *  builds data structure for the simulation and
 *  starts the integration of the system dynamics.
 */

int main (int ArgC, char * ArgV[])
{
   char OutString[40] = "";

#ifdef PRINT_STATUS
   printProgramPresentation();
#endif

   /***  Simulation boot... ***/
   initParameters (ArgC, ArgV);
   openOutputFiles();

#ifdef PRINT_STATUS
   startTimer();
#endif

   initNeuralNetwork();
   readCommands(NULL);

#ifdef PRINT_STATUS
   elapseTimer();
   fprintf(stderr, "\n\nElapsed Time: %ss\n", timer(OutString));
#endif

   /*** Simulation start... ***/
   if (!QuitSimulation) 
      simulation();

   /*** Simulation shutdown... ***/
   closeOutputFiles();

   return 0;
}



#undef NULL_LAYER
