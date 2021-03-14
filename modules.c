/**
 *
 *   modules.c
 *
 *   Library of structures and functions to manage 
 *   populations of the network.
 *
 *   Project    PERSEO 2.0
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


#include "erflib.h"
#include "randdev.h"

#include "types.h"
#include "perseo.h"
#include "neurons.h"
#include "modules.h"
#include "events.h"



#define STRING_SIZE 256



/*----------------------*
 *   GLOBAL VARIABLES   *
 *----------------------*/


/*-----------*
 *  Neurons  *
 *-----------*/

neuron       *Neurons = NULL; /* All neurons in the network. */
real *NeuronStateVars = NULL; /* Array containing all the state variables of the neurons in the network. */
indexn     NumNeurons = 0;    /* Total number of neurons in the network. */


/*---------------*
 *  Populations  *
 *---------------*/

population *Populations = NULL; /* Array of basic populations defined in the network architecture. */
real  *PopulationParams = NULL; /* A whole array of the variable parameters for the Populations. */
int      NumPopulations = 0;    /* Number of basic populations (length of Populations). */

char   *ModulesFileName = EMPTY_STRING; /* File name containing the definition of the populations ("modules.ini"). */



/*---------------------*
 *  LOCAL DEFINITIONS  *
 *---------------------*/


/* #define BUFFER_SIZE          16 /* Size of the buffer for variable arrays. */ 
#define BUFFER_SIZE         256 /* Size of the buffer for variable arrays. */ 
#define BASIC_REAL_PARAMETERS 5 /* Number of real parameters fixed per population definition. */



/*-----------------------*
 *  FUNCTION DEFINITION  *
 *-----------------------*/


/*-----------------------------*
 *  loadPopulationsDefinition  *
 *-----------------------------*/

/**
 *  Adds an element to the <Populations> array, and fills the fields of
 *  the structure <population>.
 */

int loadPopulationsDefinition(int NumRealParams, IVreal *RealParams, 
                              int NumStringParams, char **StringParams)
{
   static int PopulationsSize = 0;
   
   population *p;
   int k;

   /*** Makes room for the new population. ***/
   NumPopulations++;
   if (NumPopulations > PopulationsSize) { // More memory is needed?
      PopulationsSize += BUFFER_SIZE;
      if (PopulationsSize == BUFFER_SIZE) {
         Populations = (population *)getMemory(sizeof(population)*PopulationsSize, "ERROR (loadPopulationsDefinition): Out of memory.");
         PopulationParams = (real *)getMemory(sizeof(real)*PopulationsSize*NumParameters, "ERROR (loadPopulationsDefinition): Out of memory.");
      } else {
         Populations = (population *)realloc(Populations, sizeof(population)*PopulationsSize);
         MemoryAmount += sizeof(population) * BUFFER_SIZE;
         PopulationParams = (real *)realloc(PopulationParams, sizeof(real)*PopulationsSize*NumParameters);
         MemoryAmount += sizeof(real) * NumParameters * BUFFER_SIZE;
         if (Populations == NULL || PopulationParams == NULL)
            printFatalError("loadPopulationsDefinition", "Out of memory.");
         for (k=0; k<NumPopulations-1; k++)
            Populations[k].Parameters = &(PopulationParams[k*NumParameters]);
      }
   }
   p = &(Populations[NumPopulations-1]);

   /*** Definition of the population parameters. ***/
   if (NumRealParams != NumParameters + BASIC_REAL_PARAMETERS) { // Is number of parameters incorrect?
      return 1;
   }
   p->N = (indexn)RealParams[0];
   if (p->N <= 0) { // Is a positive integer?
      return 1;
   }
   p->JExt  = (real)RealParams[1];
   p->DJExt = (real)RealParams[2];
   p->CExt  = (real)RealParams[3];
   p->NuExt = (real)RealParams[4];
   p->Parameters = &(PopulationParams[(NumPopulations-1)*NumParameters]);
   for (k=0; k<NumParameters; k++)
      p->Parameters[k] = (real) RealParams[BASIC_REAL_PARAMETERS + k];

   /*** Definition of the fields needed for efficiency. ***/
   p->InvNuExt = 1000.0 / (p->NuExt*p->CExt*p->N);

   doubleToTimex(START_TIME_OFFSET, p->Emission);
   p->Emission.Millis -= p->InvNuExt * log(1.0-Random());
   if (p->Emission.Millis > 1.0) { // Optimization of time representation.
      k = (int)(p->Emission.Millis);
      p->Emission.Seconds += k;
      p->Emission.Millis -= (double)k;
   }

   doubleToTimex(START_TIME_OFFSET, p->LastUpdate);
   p->SpikeCounter = 0;

   p->ID = NumPopulations - 1;
   
   /*** Definition of the fields needed for efficiency. ***/
   p->JTab = NULL;
   if (makeGaussianLUT(&(p->JTab), ANALOG_DEPTH, p->JExt, p->JExt*p->DJExt, 0.0, 2.0*p->JExt))
      printFatalError("loadPopulationsDefinition", "Out of memory.");
   
   /*** Update the total number of neurons in the network. ***/
   NumNeurons += p->N;

   return 0;
}



/*---------------------*
 *  createPopulations  *
 *---------------------*/

/**
 *  Functions called after the populations definition. 
 *  It allocates the array of neurons defining their fields.
 */

void createPopulations ()
{
   int n, k, p;
   indexn i;

   /*** Allocates memory for global variables. ***/
   Neurons = (neuron *)getMemory(sizeof(neuron) * NumNeurons, "ERROR (createBasicPopulations): Out of memory.");
   NeuronStateVars = (real *)getMemory(sizeof(real)*NumNeurons * NumNeuronVariables, "ERROR (createBasicPopulations): Out of memory.");

   /*** Definition of the single neurons of the network. ***/
   n = 0;
   k = 0;
   for (p=0; p<NumPopulations; p++) 
   {
      Populations[p].Neurons = &(Neurons[n]);
      for (i=0; i<Populations[p].N; i++) 
      {
         Populations[p].Neurons[i].Pop = &(Populations[p]);
         Populations[p].Neurons[i].Stim = NULL;
         Populations[p].Neurons[i].StateVar = &(NeuronStateVars[k]);
         k += NumNeuronVariables;
         /*** The other fields have to be initialized when the initial ***
          *** conditions of the network are set (see init.c).          ***/
      }

      n += Populations[p].N;
   }

}



/*----------------------*
 *  getPopulationParam  *
 *----------------------*/

/**
 *  Returns the value of the parameter ParamNum of the 
 *  population Pop, as defined in 'module.ini'.
 *
 *  WARNING: Only parameter n. 4 is managed (NuExt).
 */

double getPopulationParam(int      Pop, /* Population to probe. */
                          int ParamNum) /* Number of parameter to read. */
{
   static population *p;
   static double    ret;

   if (Pop >= 0 && Pop < NumPopulations) {
      p = &Populations[Pop];

      if (ParamNum == 4) {// NuExt
         ret = p->NuExt;

      } else if (ParamNum == 10) { // TauC in the case of VIFCA and LIFCA neurons.
         if (strcmp(strupr(NeuronType), NT_VIFCA) == 0 || 
            strcmp(strupr(NeuronType), NT_LIFCA) == 0) { 
            ret = p->Parameters[ParamNum - BASIC_REAL_PARAMETERS];
         }
      }


   }

   return ret;
}


/*----------------------*
 *  setPopulationParam  *
 *----------------------*/

/**
 *  Updates online the global parameter of a population, like
 *  the frequency \nu_ext of the external spikes.
 *
 *  WARNING: Only parameter n. 4 is managed (NuExt).
 */

void setPopulationParam(int           Pop, /* Population to update. */
                        int      ParamNum, /* Number of parameter to change. */
                        double ParamValue, /* New value to assign to the parameter. */
                        double       Time) /* Time when the update occur. */
{
   static population *p;

   if (Pop >= 0 && Pop < NumPopulations) {
      p = &Populations[Pop];

      if (ParamNum == 4) { // NuExt
         p->NuExt = ParamValue;
         if (p->NuExt <= 0.0) {
            p->NuExt = 0.0;
            p->InvNuExt = -1.0; // It is a non sense value.
            doubleToTimex(Life, p->Emission);
         } else {
            p->InvNuExt = 1000.0 / (p->NuExt * p->CExt * p->N);
            doubleToTimex(Time - p->InvNuExt * log(1-Random()), p->Emission);
         }

      } else if (ParamNum == 10) { // Is TauC in the case of VIFCA and LIFCA neurons.
         if ((strcmp(strupr(NeuronType), NT_VIFCA) == 0 || 
            strcmp(strupr(NeuronType), NT_LIFCA) == 0) && 
            ParamValue > 0.) { 
            p->Parameters[ParamNum - BASIC_REAL_PARAMETERS] = ParamValue;
         }
      }
   }
}



#undef BASIC_REAL_PARAMETERS
#undef BUFFER_SIZE
#undef STRING_SIZE