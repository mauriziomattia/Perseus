/**
 *
 *   modules.h
 *
 *   Library of structures and functions to manage 
 *   populations of the network.
 *
 *   Project: PERSEO 2.0
 *
 */



#ifndef __MODULES_H__
#define __MODULES_H__



#include "invar.h"

#include "types.h"
#include "stimuli.h"



/**
 *  The structure defining a generic neuron.
 */

typedef struct _neuron {
    struct _population *Pop; /* Population the neuron belongs to. */
           stimulus   *Stim; /* Stimulus, if any, affecting the neuron. */
           real   *StateVar; /* Array of neuron state variables (NumNeuronVariables elements). */
           timex         Tr; /* Arriving time of the last pre-synaptic spike. */
           timex         Te; /* Emission time of the last spike. */
           timex LastUpdate; /* Time of last update of the fields in the structure. */
        } neuron;


/**
 *  The structure defining a generic population.
 */

typedef struct _population {
        /*** Fields needed for making the simulation efficient. ***/
        real    InvNuExt; /* Reciprocal of the external spike frequency (1/NuExt). */
        timex   Emission; /* Time when the last external spike was received. */
        timex LastUpdate; /* Time of the last update of local variables (SpikeCounter). */
        int SpikeCounter; /* Number of spikes emitted . */
        real       *JTab; /* Look-up table for synaptic efficacy with external neurons. */
        int           ID; /* Corresponding index in the Populations array. */

        /*** Population parameters. ***/
        real *Parameters; /* Array of parameters needed for the evolution of neuron dynamics. */
        real        CExt;
        real       NuExt;
        real        JExt;
        real       DJExt;
        indexn         N; /* Number of neuron in population. */

        struct _neuron  *Neurons; /* Neuron array belonging to the population. */
}  population;



/*--------------------*
 *  GLOBAL VARIABLES  *
 *--------------------*/


/*-----------*
 *  Neurons  *
 *-----------*/

extern neuron       *Neurons;   /* All neurons in the network. */
extern real *NeuronStateVars;   /* Array containing all the state variables of the neurons in the network. */
extern indexn     NumNeurons;   /* Total number of neurons in the network. */


/*---------------*
 *  Populations  *
 *---------------*/

extern population *Populations; /* Array of basic populations defined in the network architecture. */
extern int      NumPopulations; /* Number of basic populations (length of Populations). */

extern char   *ModulesFileName; /* File name containing the definition of the populations ("modules.ini"). */



/*--------------------*
 *  GLOBAL FUNCTIONS  *
 *--------------------*/


/**
 *  Adds an element to the <Populations> array, and fills the fields of
 *  the structure <population>.
 */
int loadPopulationsDefinition(int NumRealParams, IVreal *RealParams, 
                              int NumStringParams, char **StringParams);


/**
 *  Functions called after the populations definition. 
 *  It allocates the array of neurons defining their fields.
 */
void createPopulations ();


/**
 *  Returns the value of the parameter ParamNum of the 
 *  population Pop, as defined in 'module.ini'.
 *
 *  WARNING: Only parameter n. 4 is managed (NuExt).
 */

double getPopulationParam(int      Pop,  /* Population to probe. */
                          int ParamNum); /* Number of parameter to read. */


/**
 *  Updates online the global parameter of a population, like
 *  the frequency \nu_ext of the external spikes.
 *
 *  WARNING: Only parameter n. 4 is managed (NuExt).
 */

void setPopulationParam(int           Pop,  /* Population to update. */
                        int      ParamNum,  /* Number of parameter to change. */
                        double ParamValue,  /* New value to assign to the parameter. */
                        double       Time); /* Time when the update occur. */



#endif /* __MODULES_H__ */