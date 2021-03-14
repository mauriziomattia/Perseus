/*
 *
 *   synapses.h
 *
 *   Library of structures and functions to manage 
 *   different types of synapses (Fixed, ...).
 *
 *   Project: PERSEO 2.0
 *
 */



#ifndef __SYNAPSES_H__
#define __SYNAPSES_H__



#include "invar.h"



#define MAX_NSSS 128 // Maximum number of long-term synaptic state allowed.

//
// TO CUSTOMIZE (1)...
//

/*** SynapseType ***/
#define STS_FXD  "FIXED" /* Fixed instantaneous synapse. */
#define ST_FXD   0
#define STS_AF   "AF"    /* Plastic synapse Annunziato-Fusi. */
#define ST_AF    1
#define STS_TWAM "TWAM"  /* Plastic synapse with pre-post spike Time Window Anti-Markram. */ 
#define ST_TWAM  2

/*** NumSynapseParameters ***/
#define NSP_FXD    2
#define NSP_AF    13
#define NSP_TWAM  13

/*** NumSynapseVariables ***/
#define NSV_FXD    0
#define NSV_AF     1
#define NSV_TWAM   1

/*** NumSynapseStableStates ***/
#define NSSS_FXD   1
#define NSSS_AF    2
#define NSSS_TWAM  2

/*** SynapseInitializationType ***/
//#define SIT_..._RESTING_POTENTIAL    0 /* All internal variables start from the resting conditions. */


/*** A struct for the generic synapse state. ***/
typedef struct synapse_state_struct {
   int NumStateVars; /* Number of state variables reported in the array StateVars. It is always greater than one. */
   real * StateVars; /* Array of state variables of the synapse.                          *
                      * The first element has to be the long term synaptic efficacy.      *
                      * The second element, if any, has to be the logic value (0, 1, ...) *
                      * of the long-term synaptic state.                                  */
} synapse_state;



/*----------------------*
 *   GLOBAL VARIABLES   *
 *----------------------*/

extern real **JTabArray; /* Array of the LUT for the synaptic efficacies, an array of synaptic 
                            values for each stable state of the synapse. */
extern int NumJTabArray; /* Number of elements in the JTabArray. */


                  
                  
/**
 *  Returns the integer representation of the synapse type,
 *  if the string is among the known ones. Return -1 otherwise.
 */

int getSynapseType(char * SynapseType);


/**
 *  Parses the synapse parameters read from the ConnectivityFile "connectivity.ini".
 *  Returns 1 upon error, 0 otherwise.
 */

int parseSynapseParams(int SynapseType, 
                       int NumSynapseParameters, 
                       IVreal * SynapseParams);


/**
 *  Makes the LUT (Look-Up Table) for allowed values of 
 *  the synaptic efficacies.
 *  Returns 1 if there is not enough memory, 0 otherwise.
 */

int setSynapticEffaciesLUT(connectivity * c);


/**
 *  Sets the field of connectivity related to the synaptic model
 *  like pointer to functions setting the state of the synapse and
 *  the size of the synapse itself.
 *  The synaptic size includes the indexes (byte) to the synaptic 
 *  efficacies LUT and the state variables (in general float to 
 *  save space). The value is NSV_# * sizeof(float) + NSSS_# * sizeof(byte)
 *  # stands for the synapse type (FXD, ...).
 */

void setConnectivitySynapseFields(connectivity * c);


/**
 *  Initialize the state variables and the stable states of 
 *  the synapse addressed by the function parameters.
 */

void initSynapseState(indexn        i,  // post-synaptic neuron.
                      indexn        j,  // pre-synaptic neuron.
                      void         *s,  // pointer to the synapse.
                      connectivity *c,  // pointer to the synaptic population.
                      int           l); // Layer corresponding to the transmission delay.


//
// TO CUSTOMIZE (2)...
//

/*--------------------------*
 *                          *
 *   FXD (Fixed) synapse.   *
 *                          *
 *--------------------------*/

typedef struct {
   byte  Jndx; /* Index of the fixed synaptic efficacy in the LUT. */
} synapse_FXD;


typedef struct {
   real  J; /* Average of the fixed synaptic efficacies. */
   real DJ; /* Relative standard deviation of the fixed synaptic efficacies. */
} synapse_params_FXD;


/**
 *  Parses the synapse parameters read from the ConnectivityFile "connectivity.ini"
 *  for the fixed instantaneous synapses.
 *  Returns 1 upon error, 0 otherwise.
 */

int parseSynapseParams_FXD(int NumSynapseParameters, 
                           IVreal * SynapseParams);


/**
 *  Makes the LUT (Look-Up Table) for allowed values of 
 *  the synaptic efficacies.
 *  Returns 1 if there is not enough memory, 0 otherwise.
 */

int setSynapticEffaciesLUT_FXD(connectivity * c);


/**
 *  Initializes the stable states of the FXD synapse addressed 
 *  by the function parameters.
 */

void initSynapseState_FXD(indexn i, indexn j, void *s, connectivity *c, int l);


/**
 *  Return the synaptic efficacy of the fixed synapse.
 */

real updateSynapseState_FXD(indexn i, void *s, connectivity *c, spike *sp);


/**
 *  Returns, filling the synapse_state field <ss>, the state of
 *  the FXD synapse <s> having as pre- and post-synaptic neurons 
 *  <j> and <i> respectively. The synapse belong to the connectivity
 *  block address by <c>.
 *  StateVars composition: {Synaptic efficacy}.
 */

void getSynapseState_FXD(indexn i, indexn j, void *s, connectivity *c, int l, timex t, synapse_state *ss);


/*-------------------------------------------*
 *                                           *
 *   AF (Annunziato-Fusi) plastic synapse.   *
 *                                           *
 *-------------------------------------------*/

typedef struct {
   byte  J0ndx; /* Index of the depressed synaptic efficacy in the LUT.   */
   byte  J1ndx; /* Index of the potentiated synaptic efficacy in the LUT. */
   float    VJ; /* Internal synaptic variable.                            */
} synapse_AF;


typedef struct {
   real       J0; /* Average of the depressed synaptic efficacies.   */
   real       J1; /* Average of the potentiated synaptic efficacies. */
   real      DJ0; /* Relative standard deviation of the depressed synaptic efficacies.   */
   real      DJ1; /* Relative standard deviation of the potentiated synaptic efficacies. */
   real   AlphaJ; /* Refreshing drift toward RBdown for VJ. */
   real    BetaJ; /* Refreshing drift toward RBup for VJ. */
   real   ThetaJ; /* Threshold on VJ for the potentiated and depressed state. */
   real   ThetaV; /* Threshold on the post-synaptic potential setting the active of inactive state.  */
   real   JumpUp; /* Short-term potentiating jump for VJ at pre-synaptic spike time. */
   real JumpDown; /* Short-term depressing jump for VJ at presynaptic spike time. */
   real     RBup; /* Top reflecting barrier for VJ. */
   real   RBdown; /* Bottom reflecting barrier for VJ. */
   real       R0; /* Starting initial fraction of potentiated synapses in the element of connectivity. */
} synapse_params_AF;


/**
 *  Parses the synapse parameters read from 
 *  the ConnectivityFile "connectivity.ini"
 *  for the Annunziato-Fusi synapse dynamical synapses.
 *  Returns 1 upon error, 0 otherwise.
 */

int parseSynapseParams_AF(int NumSynapseParameters, 
                          IVreal * SynapseParams);


/**
 *  Makes the LUT (Look-Up Table) for allowed values of 
 *  the synaptic efficacies.
 *  Returns 1 if there is not enough memory, 0 otherwise.
 */

int setSynapticEffaciesLUT_AF(connectivity * c);


/**
 *  Initializes the stable states (long-term synaptic efficacies)
 *  of the AF synapse addressed by the function parameters.
 */

void initSynapseState_AF(indexn i, indexn j, void *s, connectivity *c, int l);


/**
 *  Return the synaptic efficacy of the AF synapse after 
 *  the internal variables are updated, depending on the 
 *  pre- and post-synaptic neuronal activity.
 */

real updateSynapseState_AF(indexn i, void *s, connectivity *c, spike *sp);


/**
 *  Returns, filling the synapse_state field <ss>, the state of
 *  the AF synapse <s> having as pre- and post-synaptic neurons 
 *  <j> and <i> respectively. The synapse belong to the connectivity
 *  block address by <c> and transmits spikes with delay index <l>.
 *  StateVars composition: {Syn. efficacy, 0-Depressed/1-Potentiated, 
 *  Hidden synaptic potential VJ}.
 */

void getSynapseState_AF(indexn i, indexn j, void *s, connectivity *c, int l, timex t, synapse_state *ss);


/*------------------------------------------------------*
 *                                                      *
 *   TWAM (Time Window Anti-Markram) plastic synapse.   *
 *                                                      *
 *------------------------------------------------------*/

typedef struct {
   byte  J0ndx; /* Index of the depressed synaptic efficacy in the LUT. */
   byte  J1ndx; /* Index of the potentiated synaptic efficacy in the LUT. */
   float    VJ; /* Internal synaptic variable. */
} synapse_TWAM;


typedef struct {
   real        J0; /* Average of the depressed synaptic efficacies. */
   real        J1; /* Average of the potentiated synaptic efficacies. */
   real       DJ0; /* Relative standard deviation of the depressed synaptic efficacies.   */
   real       DJ1; /* Relative standard deviation of the potentiated synaptic efficacies. */
   real    AlphaJ; /* Refreshing drift toward RBdown for VJ. */
   real     BetaJ; /* Refreshing drift toward RBup for VJ. */
   real    ThetaJ; /* Threshold on VJ for the potentiated and depressed state. */
   real PotWindow; /* Time window separating pre- and post-synaptic spikes to have short-term potentiation. */
   real    JumpUp; /* Short-term potentiating jump for VJ at pre-synaptic spike time. */
   real  JumpDown; /* Short-term depressing jump for VJ at presynaptic spike time. */
   real      RBup; /* Top reflecting barrier for VJ. */
   real    RBdown; /* Bottom reflecting barrier for VJ. */
   real        R0; /* Starting initial fraction of potentiated synapses in the element of connectivity. */
} synapse_params_TWAM;


/**
 *  Parses the synapse parameters read from 
 *  the ConnectivityFile "connectivity.ini"
 *  for the Time Window Anti-Markram dynamical synapses.
 *  Returns 1 upon error, 0 otherwise.
 */

int parseSynapseParams_TWAM(int NumSynapseParameters, 
                            IVreal * SynapseParams);


/**
 *  Makes the LUT (Look-Up Table) for allowed values of 
 *  the synaptic efficacies.
 *  Returns 1 if there is not enough memory, 0 otherwise.
 */

int setSynapticEffaciesLUT_TWAM(connectivity * c);


/**
 *  Initializes the stable states (long-term synaptic efficacies)
 *  of the TWAM synapse addressed by the function parameters.
 */

void initSynapseState_TWAM(indexn i, indexn j, void *s, connectivity *c, int l);


/**
 *  Return the synaptic efficacy of the TWAM synapse after 
 *  the internal variables are updated, depending on the 
 *  pre- and post-synaptic neuronal activity.
 */

real updateSynapseState_TWAM(indexn i, void *s, connectivity *c, spike *sp);


/**
 *  Returns, filling the synapse_state field <ss>, the state of
 *  the TWAM synapse <s> having as pre- and post-synaptic neurons 
 *  <j> and <i> respectively. The synapse belong to the connectivity
 *  block address by <c>.
 *  StateVars composition: {Syn. efficacy, 0-Depressed/1-Potentiated, 
 *  Hidden synaptic potential VJ}.
 */

void getSynapseState_TWAM(indexn i, indexn j, void *s, connectivity *c, int l, timex t, synapse_state *ss);



#endif /* __SYNAPSES_H__ */
