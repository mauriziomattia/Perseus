/*
 *
 *   init.h
 *
 *   Library of functions to read definition files to 
 *   initialize the data structures needed to the simulation
 *
 *   Project: PERSEO 2.x
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <math.h>

#include "invar.h"
#include "randdev.h"

#include "types.h"
#include "perseo.h"
#include "results.h"
#include "stimuli.h"
#include "init.h"
#include "events.h"
#include "modules.h"
#include "neurons.h"
#include "connectivity.h"
#include "synapses.h"
#include "delays.h"
#include "commands.h"



/*---------------------*
 *  LOCAL DEFINITIONS  *
 *---------------------*/

#define BUFFER_DIM 256 /* Maximum static string size in the module. */



/*--------------------*
 *  GLOBAL FUNCTIONS  *
 *--------------------*/

/*---------------*
 *  initNeurons  *
 *---------------*/

/**
 *   Initialize the array of all neurons in the 
 *   network. It is defined in <modules.c>.
 */

void initNeurons (void)
{
   indexn i;

#ifdef PRINT_STATUS
   fprintf(stderr, "\nInitializing Neurons... ");
#endif

   /*** Initialize the seed of pseudorandom number generator. ***/
   if (isDefined("NEURONSSEED"))
      SetRandomSeed(NeuronsSeed);
   else
      Randomize();
   NeuronsSeed = GetRandomSeed();
   fprintf(DocFile, "# Neuronal Seed: %i\n", NeuronsSeed);
   fflush(DocFile);

   /*** Initialize the state variables of the neurons. ***/
   if ((*initNeuronVariables)())
      printFatalError("initNeurons", "Initialization type for neuron state variables unknown.");

   /*** Initializations of the other fixed neuron fields (Tr, Te, LastUpdate). ***/
   for (i=0; i<NumNeurons; i++) {
      doubleToTimex(START_TIME_OFFSET, Neurons[i].Tr);
	   doubleToTimex(START_TIME_OFFSET-Life, Neurons[i].Te);
	   doubleToTimex(START_TIME_OFFSET, Neurons[i].LastUpdate);
   }

#ifdef PRINT_STATUS
    fprintf(stderr, "100.0%% (Memory: %g Mbytes)\r", (real)MemoryAmount/1024.0/1024.0);
#endif
}


/*----------------------*
 *  initSynapticMatrix  *
 *----------------------*/

/**
 *  Allocates memory for the synaptic matrix creating
 *  random connectivity and initializing the state of 
 *  the synapses.
 */

void initSynapticMatrix (void)

{
   /*** Initialize the seed of pseudorandom number generator. ***/
   if (isDefined("SYNAPSESSEED"))
      SetRandomSeed(SynapsesSeed);
   else
      Randomize();
   SynapsesSeed = GetRandomSeed();
   fprintf(DocFile, "# Synaptic Seed: %i\n", SynapsesSeed);
   fflush(DocFile);

   /*** Sets the bounds of the delay distribution. ***/
   setDelayBounds();

   /*** Creates and fills the synaptic matrix. ***/
   createSynapticMatrix();

   /*** Initialized the state variables of the synapses. ***/
   scanSynapticMatrix (0, NumNeurons-1, 0, NumNeurons-1, &initSynapseState);
}


/*---------------------*
 *  initNeuralNetwork  *
 *---------------------*/

/**
 *  Allocates memory for and initializes the structures hosting 
 *  data necessary to the simulation (populations, connectivity,
 *  neurons and synapses).
 */

void initNeuralNetwork (void)

{
   /*** Interrupt management... ***/
   signal(SIGINT, interruptSimulation);
   signal(SIGTERM, interruptSimulation);

   /*** Populations initialization... ***/
   createPopulations();

   /*** Neurons initialization... ***/
   initNeurons();

   /*** Synapses initialization... ***/
   initSynapticMatrix();
}


/*------------------*
 *  initParameters  *
 *------------------*/

/**
 *   Declaration and definition of the simulation parameters.
 */

void initParameters (int ArgC, char * ArgV[])

{
   IVinteger i[100];
   IVinteger b[100];
   IVreal    r[100];
   char sError[BUFFER_DIM];

   /*** List of declarations. ***/
   addStringVariable  ("NEURONTYPE", &NeuronType, false);

   addStringVariable  ("MODULEFILE", &ModulesFileName, false);

   addStringVariable  ("DELAYDISTRIBTYPE", &DelayDistribType, false);
   addIntegerVariable ("DELAYNUMBER", &i[1], 1, INT_MAX, false);

   addStringVariable  ("SYNAPTICEXTRACTIONTYPE", &SynapticExtractionType, true);
   
   addStringVariable  ("CONNECTIVITYFILE", &ConnectivityFileName, false);

   addStringVariable  ("LOGFILE", &DocFileName, true);

   addRealVariable    ("LIFE", &r[0], 0, (IVreal)1e37, false);

   addIntegerVariable ("NEURONSSEED", &i[2], -INT_MAX, INT_MAX, true);
   addIntegerVariable ("SYNAPSESSEED", &i[3], -INT_MAX, INT_MAX, true);

   addStringVariable  ("COMMANDFILE", &CommandsFileName, true);

   addBooleanVariable ("OUTRATES", &b[0], true);
   addStringVariable  ("RATESFILE", &RatesFileName, true);
   addRealVariable    ("RATESSAMPLING", &r[23], (IVreal)1e-37, (IVreal)1e37, true);
   addRealVariable    ("STARTTIME", &r[24], (IVreal)-1e37, (IVreal)1e37, true);

   addBooleanVariable ("OUTSYNAPSES", &b[1], true);
   addStringVariable  ("SYNAPSESFILE", &SynapsesFileName, true);
   addIntegerVariable ("PRESTART", &i[5], 0, INT_MAX, true);
   addIntegerVariable ("PREEND", &i[6], 0, INT_MAX, true);
   addIntegerVariable ("POSTSTART", &i[7], 0, INT_MAX, true);
   addIntegerVariable ("POSTEND", &i[8], 0, INT_MAX, true);

   addBooleanVariable ("OUTSPIKES", &b[2], true);
   addStringVariable  ("SPIKESFILE", &SpikesFileName, true);
   addIntegerVariable ("NEURONSTART", &i[9], 0, INT_MAX, true);
   addIntegerVariable ("NEURONEND", &i[10], 0, INT_MAX, true);

   addBooleanVariable ("OUTSYNSTATE", &b[3], true);
   addStringVariable  ("SYNSTATEFILE", &SynStateFileName, true);
   addIntegerVariable ("SSPRESTART", &i[11], 0, INT_MAX, true);
   addIntegerVariable ("SSPREEND", &i[12], 0, INT_MAX, true);
   addIntegerVariable ("SSPOSTSTART", &i[13], 0, INT_MAX, true);
   addIntegerVariable ("SSPOSTEND", &i[14], 0, INT_MAX, true);

   addBooleanVariable ("OUTNEUSTATE", &b[4], true);
   addStringVariable  ("NEUSTATEFILE", &NeuStateFileName, true);
   addIntegerVariable ("NSSTART", &i[15], 0, INT_MAX, true);
   addIntegerVariable ("NSEND", &i[16], 0, INT_MAX, true);

   addBooleanVariable ("OUTSYNTRANS", &b[5], true);
   addStringVariable  ("SYNTRANSFILE", &SynTransFileName, true);
   addRealVariable    ("SYNTRANSSAMPLING", &r[45], (IVreal)1e-37, (IVreal)1e37, true);

   addBooleanVariable ("OUTSYNSTRUCT", &b[6], true);
   addStringVariable  ("SYNSTRUCTFILE", &SynStructFileName, true);

   addBooleanVariable ("OUTSYNDETTRANS", &b[7], true);
   addStringVariable  ("SYNDETTRANSFILE", &detailSynTransFileName, true);

   addBooleanVariable ("OUTAFFCURR", &b[8], true);
   addStringVariable  ("AFFCURRFILE", &CurrentFileName, true);
   addRealVariable    ("AFFCURRSAMPLING", &r[46], (IVreal)1e-37, (IVreal)1e37, true);
   addIntegerVariable ("ACNEURON", &i[17], 0, INT_MAX, true);

   /*** Declaration of parameters and error parsing. ***/
   if (readDefinitionFile(INIT_FILE)) exit(1);
   if (readCommandLineDefinition(ArgC, ArgV)) exit(1);
   if (isDefinitionCompleted()) exit(1);

   /*** Parameters setting. ***/
   addIntegerVariable ("NEURONSSEED", &i[2], -INT_MAX, INT_MAX, true);
   addIntegerVariable ("SYNAPSESSEED", &i[3], -INT_MAX, INT_MAX, true);

   DelayNumber = i[1];

   Life = r[0];

   /*** Seeds ***/
   if (isDefined("NEURONSSEED")) NeuronsSeed = i[2];
   if (isDefined("SYNAPSESSEED")) SynapsesSeed = i[3];

   /*** Command file not specified: the standard input will be ***
    *** assumed as input file.                                 ***/
   if (!isDefined("COMMANDFILE")) {
      CommandsFileName = getMemory(1,"(initParameters): Not enough memory\n");
      CommandsFileName[0] = '\0';
   }

   /*** Command file not specified: the standard input will be ***
    *** assumed as input file.                                 ***/
   if (!isDefined("LOGFILE")) {
      DocFileName = DEFAULT_LOG_FILENAME;
   }

   /***                                                       ***
    *** Sets constants and function pointers dependent on the ***
    *** type of choosen neuron.                               ***
    ***                                                       ***/
   if (setNeuronType()) {
      sprintf(sError, "Neuron type '%s' unknown .\n", NeuronType);
      printFatalError("initParameters", sError);
   }


   /***                                                  ***
    *** Sets the function pointers dependent on the      ***
    *** type of choosen transmission delay distribution. ***
    ***                                                  ***/
   if (setDelayDistributionType()) {
      sprintf(sError, "Delay distribution type '%s' unknown .\n", DelayDistribType);
      printFatalError("initParameters", sError);
   }


   /***                                                  ***
    *** Sets the function pointers dependent on the      ***
    *** type of synaptic extraction choosen.             ***
    ***                                                  ***/
   if (setSynapticExtractionType()) {
      sprintf(sError, "Synaptic extraction type '%s' unknown .\n", SynapticExtractionType);
      printFatalError("initParameters", sError);
   }


   /***                                                  ***/
   /*** loading populations and connectivity definition. ***/
   /***                                                  ***/
   if (readArrayDefinition (ModulesFileName, loadPopulationsDefinition))
      printFatalError("initParameters", "Bad population definition.\n");
   
   if (readArrayDefinition (ConnectivityFileName, loadConnectivityDefinition))
      printFatalError("initParameters", "Bad connectivity definition.\n");


   /***                                                   ***/
   /*** Reading parameters for monitoring the simulation. ***/
   /***                                                   ***/

   /*** Rates Output. ***/
   if (isDefined("OUTRATES")) {
      RatesResults = b[0];
      if (isDefined("RATESSAMPLING")) RatesSampling = r[23];
      if (isDefined("STARTTIME"))     RatesStartTime = r[24];
   } else
      RatesResults = false;

   /*** Synapses Output. ***/
   if (isDefined("OUTSYNAPSES")) {
      if (SynapsesResults = b[1]) {
         if (isDefined("PRESTART"))  SynapsePreStart = i[5];
         if (isDefined("PREEND"))    SynapsePreEnd = i[6];
         if (SynapsePreStart > SynapsePreEnd ||
             SynapsePreEnd >= NumNeurons)
            printFatalError("initParameters", "Bad neurons range in synapses (A).\n");
         if (isDefined("POSTSTART")) SynapsePostStart = i[7];
         if (isDefined("POSTEND"))   SynapsePostEnd = i[8];
         if (SynapsePostStart > SynapsePostEnd ||
             SynapsePostEnd >= NumNeurons)
            printFatalError("initParameters", "Bad neurons range in synapses (B).\n");
      }
   } else
      SynapsesResults = false;

   /*** Spikes Output. ***/
   if (isDefined("OUTSPIKES")) {
      if (SpikesResults = b[2]) {
         if (isDefined("NEURONSTART"))  SpikeStart = i[9];
         if (isDefined("NEURONEND"))    SpikeEnd = i[10];
         if (SpikeStart > SpikeEnd ||
             SpikeEnd >= NumNeurons)
            printFatalError("initParameters", "Bad neurons range in spikes.\n");
      }
   } else
      SpikesResults = false;


   /*** Asynchronous SYNAPTIC STATE logging. ***/
   if (isDefined("OUTSYNSTATE")) {
      if (SynStateResults = b[3]) {
         if (isDefined("SSPRESTART"))  SynStatePreStart = i[11];
         if (isDefined("SSPREEND"))    SynStatePreEnd = i[12];
         if (SynStatePreStart > SynStatePreEnd ||
             SynStatePreEnd >= NumNeurons)
            printFatalError("initParameters", "Bad neurons range in synapses (D).\n");
         if (isDefined("SSPOSTSTART")) SynStatePostStart = i[13];
         if (isDefined("SSPOSTEND"))   SynStatePostEnd = i[14];
         if (SynStatePostStart > SynStatePostEnd ||
             SynStatePostEnd >= NumNeurons)
            printFatalError("initParameters", "Bad neurons range in synapses (E).\n");
      }
   } else
      SynStateResults = false;

   /*** Asynchronous NEURONAL STATE logging. ***/
   if (isDefined("OUTNEUSTATE")) {
      if (NeuStateResults = b[4]) {
         if (isDefined("NSSTART"))  NeuStateStart = i[15];
         if (isDefined("NSEND"))    NeuStateEnd = i[16];
         if (NeuStateStart > NeuStateEnd ||
             NeuStateEnd >= NumNeurons)
            printFatalError("initParameters", "Bad neurons range in synapses (F).\n");
      }
   } else
      NeuStateResults = false;

   /*** Synaptic transition number logging. ***/
   if (isDefined("OUTSYNTRANS")) {
      if (SynTransResults = b[5])
         if (isDefined("SYNTRANSSAMPLING"))  SynTransSampling = r[45];
   } else
      SynTransResults = false;

   /*** Synaptic matrix prints at population level. ***/
   if (isDefined("OUTSYNSTRUCT")) {
      SynStructResults = b[6];
   } else
      SynStructResults = false;

   /*** Detailed synaptic transition logging. ***/
   if (isDefined("OUTSYNDETTRANS")) {
      detailSynTransResults = b[7];
   } else
      detailSynTransResults = false;

   /*** Afferent current logging. ***/
   if (isDefined("OUTAFFCURR")) {
      if (CurrentResults = b[8]) {
         if (isDefined("AFFCURRSAMPLING")) CurrentSampling = r[46];
         if (isDefined("ACNEURON")) CurrentNeuron = i[17];
         if (CurrentNeuron >= NumNeurons)            
            printFatalError("initParameters", "Incorrect neuron number (ACNEURON).\n");
      }
   } else
      CurrentResults = false;
}



#undef BUFFER_DIM

