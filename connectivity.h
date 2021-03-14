/*
 *
 *   connectivity.h
 *
 *   Library of structures and functions to manage 
 *   the connectivity of the network: the populations
 *   of synapses at mean-field level.
 *
 *   Project    PERSEO 2.0
 *
 */



#ifndef __CONNECTIVITY_H__
#define __CONNECTIVITY_H__



#include "invar.h"
#include "queue.h"

#include "types.h"



/**
 *  Segment of the axon of a neuron, containing the set 
 *  of synapes with the same transmission delay.
 */

typedef struct {
   void     *Synapses; /* The set of synapses in the axon segment.  *
                        * It is a void pointer because the elements *
                        * of the array can have different size.     */
   byte        *DPost; /* Number of non-existent synapses preceding *
                        * the synapse in the axon.                  */
   indexn  *Exception; /* Index of postsynaptic neurons farther     *
                        * than 255 elements from the preceding      *
                        * postsynaptic neuron in the axon.          */
   indexn NumSynapses; /* Number of synapses in the axon segment    *
                        * (length of DPost).                        */
} axon_segment;


/**
 *  A layer composing the synaptic matrix 
 *  corresponding to a transmission delay.
 *  It contantains also the queue of the spikes 
 *  to manage.
 */

typedef struct {
   axon_segment *Pre; /* Array of axon segments of pre-synaptic neurons. */
   real        Delay; /* The transmission delay of synapse in the layer. */
   queue       Queue; /* Queue of the spikes to be managed.              */
   spike       Spike; /* The last spike extracted from the queue and to  *
                       * to manage (the top of the queue).               */
   boolean     Empty; /* It is true if no spikes have to be managed.     */
} synaptic_layer;


/**
 *  Element of the connectivity matrix defined in 'connectivity.ini'.
 */

typedef struct connectivity_struct{
        /*** Fields needed for making the simulation efficient.     ***/
        real               **JTab; /* Look-up table for synaptic efficacies.    */
        int           SynapseSize; /* Size in byte of a single synapse. It is NSV * sizeof(float) + NSSS * sizeof(byte) (see synapse.h). */
        int NumSynapseStableState; /* Number of long-term synaptic states. Length of JTab array. */
        int                    ID; /* Corresponding index in the ConnectivityArray array. */
        void (*initSynapseState)(indexn, 
                                 indexn, 
                                 void *, 
                                 struct connectivity_struct *, 
                                 int);
        real (*updateSynapseState)(indexn, 
                                   void *, 
                                   struct connectivity_struct *, 
                                   spike *);
        void (*getSynapseState)(indexn,
                                indexn,
                                void *,
                                struct connectivity_struct *,
                                int,
                                timex,
/*                                synapse_state *); */
                                struct synapse_state_struct *);

        /*** Population parameters. ***/
        int  NumParameters; /* Size of Parameters array. */
        real   *Parameters; /* Array of parameters needed also for the evolution of synapse dynamics. */
        int    SynapseType; /* Type of synapse. */
        real          DMin; /* Minimum transmission delay .*/
        real          DMax; /* Maximum transmission delay .*/
        real         CProb; /* Probability to have a synapse. */
}  connectivity;


/**
 *  Type of function pointer of the call-back function used by the 
 *  scanSynapticMatrix loop.
 */

typedef void (*InspectFuncPtr)(indexn,         // post-synaptic neuron.
                               indexn,         // pre-synaptic neuron.
                               void *,         // pointer to the synapse.
                               connectivity *, // pointer to the synaptic population.
                               int);           // Layer corresponding to the transmission delay.


/*** SynapticExtractionType ***/
#define SET_RAN "RANDOM"
#define SET_FIX "FIXEDNUM"


/*--------------------*
 *  GLOBAL VARIABLES  *
 *--------------------*/

extern synaptic_layer  *SynapticMatrix; /* Synaptic matrix decomposed in layers. */

extern connectivity    ***Connectivity; /* Matrix of the synaptic populations defining the network architecture 
                                           (matrix of pointers to connectivity structure). */
extern connectivity *ConnectivityArray; /* Each non NULL entry is the structure of an existing synaptic populations. */
extern int        NumConnectivityArray; /* Number of connectivity structures loaded in ConnectivityArray. */
extern real        *ConnectivityParams; /* A whole array of the variable parameters for the Connectivity. */
extern int       NumConnectivityParams; /* Number of reals loaded in ConnectivityParams. */

extern char      *ConnectivityFileName; /* File name containing the definition of the connectivity ("connectivity.ini"). */
extern char    *SynapticExtractionType; /* Type of synaptic random extraction 'RANDOM', 'FIXEDNUM', ... */



/*--------------------*
 *  GLOBAL FUNCTIONS  *
 *--------------------*/

/**
 *  Adds an element to the <Connectivity> matrix, and fills the fields of
 *  the structure <connectivity>.
 */

int loadConnectivityDefinition(int NumRealParams, IVreal *RealParams, 
                               int NumStringParams, char **StringParams);


/**
 *  Functions called after the connectivity definition. 
 *  It allocates the layered structure of the synaptic matrix.
 */

void createSynapticMatrix ();


/**
 *  Visits a portion of the synaptic matrix defined by an 
 *  interval of post-synaptic neurons [PostStart,PostEnd] 
 *  and an interval of pre-synaptic neurons [PreStart,PreEnd].
 *  For each synapse met a user-defined hook function 
 *  (InspectFunc) is called. The synapse are visited in 
 *  ascending order of pre-synaptic neuron, transmission 
 *  delay and post-synaptic neuron.
 */

void scanSynapticMatrix (indexn PostStart, 
                         indexn   PostEnd, 
                         indexn  PreStart, 
                         indexn    PreEnd,
                         InspectFuncPtr InspectFunc);



/**
 *  Updates online the global parameter of a synaptic population,
 *  like the JumpUp or JumpDown of the hidden synaptic variable.
 *
 *  WARNING: Only parameter n. 14 and 15 are managed (JumpUp and JumpDown).
 */

void setConnectivityParam(int          Post, /* Post-synaptic population. */
                          int           Pre, /* Post-synaptic population. */
                          int      ParamNum, /* Number of parameter to change. */
                          double ParamValue, /* New value to assign to the parameter. */
                          double       Time); /* Time when the update occur. */



/**
 *  Sets the function pointers dependent on the
 *  type of synaptic extraction choosen.
 */

int setSynapticExtractionType();



#endif /* __CONNECTIVITY_H__ */
