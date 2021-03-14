/*
 *
 *   neurons.h
 *
 *   Library of structures and functions to manage 
 *   different types of neurons (LIF, VIF, ...).
 *
 *   Project: PERSEO 2.0
 *
 */



#ifndef __NEURONS_H__
#define __NEURONS_H__



/*---------------------*
 *  GLOBAL DEFINITIONS  *
 *---------------------*/

/*** NeuronType ***/
#define NT_LIF   "LIF"
/* GUIDO GIGANTE 15_09_2006 - START */
#define NT_LIFCA "LIFCA"
/* GUIDO GIGANTE 15_09_2006 - END */
#define NT_VIF   "VIF"
#define NT_VIFCA "VIFCA"

/*** NumParameters ***/
#define NP_LIF    5
/* GUIDO GIGANTE 15_09_2006 - START */
#define NP_LIFCA  8
/* GUIDO GIGANTE 15_09_2006 - END */
#define NP_VIF    5
#define NP_VIFCA  8

/*** NumNeuronVariables ***/
#define NNV_LIF    1
/* GUIDO GIGANTE 15_09_2006 - START */
#define NNV_LIFCA  2
/* GUIDO GIGANTE 15_09_2006 - END */
#define NNV_VIF    1
#define NNV_VIFCA  2

/*** InitializationType ***/
#define NIT_LIF_RESET_POTENTIAL    0 /* All membrane potentials start from H. */
#define NIT_LIF_RESTING_POTENTIAL  1 /* All membrane potentials start from the resting potentials. */

/* GUIDO GIGANTE 15_09_2006 - START */
#define NIT_LIFCA_RESET_POTENTIAL    0 /* All membrane potentials start from H. */
#define NIT_LIFCA_RESTING_POTENTIAL  1 /* All membrane potentials start from the resting potentials. */
/* GUIDO GIGANTE 15_09_2006 - END */

#define NIT_VIF_RESET_POTENTIAL    0 /* All membrane potentials start from H. */
#define NIT_VIF_RESTING_POTENTIAL  1 /* All membrane potentials start from the resting potentials. */

#define NIT_VIFCA_RESET_POTENTIAL    0 /* All membrane potentials start from H. */
#define NIT_VIFCA_RESTING_POTENTIAL  1 /* All membrane potentials start from the resting potentials. */

/*** A struct for the generic neuron state. ***/
typedef struct neuron_state_struct {
   int NumStateVars; /* Number of state variables reported in the array StateVars. It is always greater than one. */
   real * StateVars; /* Array of state variables of the neuron.             *
                      * The first element has to be the membrane potential. */
} neuron_state;



/*--------------------*
 *  GLOBAL VARIABLES  *
 *--------------------*/

/* GUIDO GIGANTE 15_09_2006 - START */
extern char       *NeuronType; /* Neurons type 'VIF', 'LIF', 'LIFCA', 'VIFCA', ... */
/* GUIDO GIGANTE 15_09_2006 - END */
extern int NumNeuronVariables;   /* Number of state variables per neuron. */
extern int      NumParameters;   /* Number of parameters required by the model neuron to simulate. */


/**
 *  Called in initNeurons() set the initial
 *  conditions for the state variables of 
 *  the neurons.
 *  Return 1 if the initialization type is
 *  unknown, 0 otherwise.
 */

extern int (*initNeuronVariables)();

                
/**
 *  Updates the state variables of the <Post> generic neuron 
 *  assuming an arriving spike <*Sp> mediated by the 
 *  synapse <*s>. If <s> is NULL the spike comes from
 *  outside. The function is called in perseo.c.
 */

extern void (*updateNeuronState)(indexn Post,  // Neuron to update
                                 void     *s,  // pointer to the synapse with the pre-synaptic neuron.
                                 spike   *Sp); // Afferent spike to manage.


/**
 *  Returns, filling the neuron_state field <ns>, the state of
 *  the generic neuron <i>.
 *  StateVars composition: {Membrane potential, ...}.
 */

extern void (*getNeuronState)(indexn         i,  // Neuron index.
                              timex          t,  // Time to which compute the neuronal state.
                              neuron_state *ns); // Neuronal state to return.


                                 
/*---------------------------------*
 *  FUNCTION AND TYPE DECLARATION  *
 *---------------------------------*/

/**
 * Sets constants and function pointers dependent on the
 * type of choosen neuron.
 */

int setNeuronType();



/*--------------------------------------------*
 *                                            *
 *   LIF (Leaky Integrate-and-Fire) neuron.   *
 *                                            *
 *--------------------------------------------*/


typedef struct {
   real V; /* the membrane potential. */
} neuron_state_LIF;


typedef struct {
   real      Tau; /* Decay time constant for the membrane potential. */
   real    Theta; /* The emission threshold.                       */
   real        H; /* The reset potential.                          */
   real     Tarp; /* The absolute refractory period.               */
   real InitType; /* Type of initial conditions for the state variable. */
} neuron_params_LIF;


/**
 * Sets constants and function pointers for the LIF neuron.
 */

void setNeuronType_LIF();


/**
 *  Set initial conditions for the state variables 
 *  of all the LIF neuron in the network.
 *  Return 1 if the initialization type is unknown.
 */

int initStateVariables_LIF();


/**
 *  Updates the state variables of the <Post> LIF neuron 
 *  assuming an arriving spike <*Sp> mediated by the 
 *  synapse <*s>. If <s> is NULL the spike comes from
 *  outside.
 */

void updateNeuronState_LIF(indexn Post,  // Neuron to update
                           void     *s,  // pointer to the synapse with the pre-synaptic neuron.
                           spike   *Sp); // Afferent spike to manage.


/**
 *  Returns, filling the neuron_state field <ns>, the state of
 *  the LIF neuron <i>.
 *  StateVars composition: {Membrane potential}.
 */

void getNeuronState_LIF(indexn         i, // Neuron index.
                        timex          t, // Time to which compute the neuronal state.
                        neuron_state *ns); // Neuronal state to return.



/* GUIDO GIGANTE 15_09_2006 - START */
/*-----------------------------------------------------------*
 *                                                           *
 *   LIFCA (LIF with frequency adaptation due to Calcium).   *
 *                                                           *
 *-----------------------------------------------------------*/


typedef struct {
   real V; /* the membrane potential. */
   real C; /* the calcium concentration. */
} neuron_state_LIFCA;


typedef struct {
   real     Tau; /* Decay time constant for the membrane potential. */
   real    Theta; /* The emission threshold. */
   real        H; /* The reset potential. */
   real     Tarp; /* The absolute refractory period. */
   real   AlphaC; /* Increase of Ca concentration after the emission of a spike. */
   real     TauC; /* Characteristic time of Ca channel inactivation.*/
   real       gC; /* K-current due to a unitary Ca concentration. */
   real InitType; /* Type of initial conditions for the state variable. */
} neuron_params_LIFCA;


/**
 * Sets constants and function pointers for the LIFCA neuron.
 */

void setNeuronType_LIFCA();


/**
 *  Set initial conditions for the state variables 
 *  of all the LIFCA neuron in the network
 *  Return 1 if the initialization type is unknown.
 */

int initStateVariables_LIFCA();


/**
 *  Updates the state variables of the <Post> LIFCA neuron 
 *  assuming an arriving spike <*Sp> mediated by the 
 *  synapse <*s>. If <s> is NULL the spike comes from
 *  outside.
 */

void updateNeuronState_LIFCA(indexn Post,  // Neuron to update
                             void     *s,  // pointer to the synapse with the pre-synaptic neuron.
                             spike   *Sp); // Afferent spike to manage.


/**
 *  Returns, filling the neuron_state field <ns>, the state of
 *  the LIFCA neuron <i>.
 *  StateVars composition: {Membrane potential, Calcium concentration}.
 */

void getNeuronState_LIFCA(indexn         i, // Neuron index.
                          timex          t, // Time to which compute the neuronal state.
                          neuron_state *ns); // Neuronal state to return.

/* GUIDO GIGANTE 15_09_2006 - END */



/*-------------------------------------------*
 *                                           *
 *   VIF (VLSI Integrate-and-Fire) neuron.   *
 *                                           *
 *-------------------------------------------*/


typedef struct {
   real V; /* the membrane potential. */
} neuron_state_VIF;


typedef struct {
   real  Beta; /* Constant decaying current. */
   real Theta; /* The emission threshold. */
   real     H; /* The reset potential. */
   real  Tarp; /* The absolute refractory period. */
   real InitType; /* Type of initial conditions for the state variable. */
} neuron_params_VIF;


/**
 * Sets constants and function pointers for the LIF neuron.
 */

void setNeuronType_VIF();


/**
 *  Set initial conditions for the state variables 
 *  of all the VIF neuron in the network
 *  Return 1 if the initialization type is unknown.
 */

int initStateVariables_VIF();


/**
 *  Updates the state variables of the <Post> VIF neuron 
 *  assuming an arriving spike <*Sp> mediated by the 
 *  synapse <*s>. If <s> is NULL the spike come from
 *  outside.
 */

void updateNeuronState_VIF(indexn Post,  // Neuron to update
                           void     *s,  // pointer to the synapse with the pre-synaptic neuron.
                           spike   *Sp); // Afferent spike to manage.


/**
 *  Returns, filling the neuron_state field <ns>, the state of
 *  the VIF neuron <i>.
 *  StateVars composition: {Membrane potential}.
 */

void getNeuronState_VIF(indexn         i, // Neuron index.
                        timex          t, // Time to which compute the neuronal state.
                        neuron_state *ns); // Neuronal state to return.



/*-----------------------------------------------------------*
 *                                                           *
 *   VIFCA (VIF with frequency adaptation due to Calcium).   *
 *                                                           *
 *-----------------------------------------------------------*/


typedef struct {
   real V; /* the membrane potential. */
   real C; /* the calcium concentration. */
} neuron_state_VIFCA;


typedef struct {
   real     Beta; /* Constant decaying current. */
   real    Theta; /* The emission threshold. */
   real        H; /* The reset potential. */
   real     Tarp; /* The absolute refractory period. */
   real   AlphaC; /* Increase of Ca concentration after the emission of a spike. */
   real     TauC; /* Characteristic time of Ca channel inactivation.*/
   real       gC; /* K-current due to a unitary Ca concentration. */
   real InitType; /* Type of initial conditions for the state variable. */
} neuron_params_VIFCA;


/**
 * Sets constants and function pointers for the VIFCA neuron.
 */

void setNeuronType_VIFCA();


/**
 *  Set initial conditions for the state variables 
 *  of all the VIFCA neuron in the network
 *  Return 1 if the initialization type is unknown.
 */

int initStateVariables_VIFCA();


/**
 *  Updates the state variables of the <Post> VIFCA neuron 
 *  assuming an arriving spike <*Sp> mediated by the 
 *  synapse <*s>. If <s> is NULL the spike comes from
 *  outside.
 */

void updateNeuronState_VIFCA(indexn Post,  // Neuron to update
                             void     *s,  // pointer to the synapse with the pre-synaptic neuron.
                             spike   *Sp); // Afferent spike to manage.


/**
 *  Returns, filling the neuron_state field <ns>, the state of
 *  the VIFCA neuron <i>.
 *  StateVars composition: {Membrane potential, Calcium concentration}.
 */

void getNeuronState_VIFCA(indexn         i, // Neuron index.
                          timex          t, // Time to which compute the neuronal state.
                          neuron_state *ns); // Neuronal state to return.



#endif /* __NEURONS_H__ */
