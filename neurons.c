/*
 *
 *   neurons.c
 *
 *   Library of structures and functions to manage 
 *   different types of neurons (LIF, VIF, ...).
 *
 *   Project: PERSEO 2.0
 *
 */

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "randdev.h"

#include "types.h"
#include "perseo.h"
#include "init.h"
#include "modules.h"
#include "connectivity.h"
#include "neurons.h"
#include "results.h"
#include "delays.h"



/*---------------------*
 *  LOCAL DEFINITIONS  *
 *---------------------*/

#define STRING_SIZE 256



/*--------------------*
 *  GLOBAL VARIABLES  *
 *--------------------*/

char       *NeuronType = EMPTY_STRING; /* Neurons type 'VIF', 'LIF', 'VIFCA', ... */
int NumNeuronVariables;                /* Number of state variables per neuron. */
int      NumParameters;                /* Number of parameters required by the model neuron to simulate. */


/**
 *  Called in initNeurons() set the initial
 *  conditions for the state variables of 
 *  the neurons.
 *  Return 1 if the initialization type is
 *  unknown, 0 otherwise.
 */

int (*initNeuronVariables)();


/**
 *  Updates the state variables of the <Post> generic neuron 
 *  assuming an arriving spike <*Sp> mediated by the 
 *  synapse <*s>. If <s> is NULL the spike comes from
 *  outside. The function is called in perseo.c.
 */

void (*updateNeuronState)(indexn Post,  // Neuron to update
                          void     *s,  // pointer to the synapse with the pre-synaptic neuron.
                          spike   *Sp); // Afferent spike to manage.


/**
 *  Returns, filling the neuron_state field <ns>, the state of
 *  the generic neuron <i>.
 *  StateVars composition: {Membrane potential, ...}.
 */

void (*getNeuronState)(indexn         i, // Neuron index.
                       timex          t, // Time to which compute the neuronal state.
                       neuron_state *ns); // Neuronal state to return.



/*-----------------------*
 *  FUNCTION DEFINITION  *
 *-----------------------*/


/**
 * Sets constants and function pointers dependent on the
 * type of choosen neuron.
 */

int setNeuronType()
{
//
// TO CUSTOMIZE (1)...
//
   if (strcmp(strupr(NeuronType), NT_LIF) == 0) // LIF neuron?
   { 
      setNeuronType_LIF();
      return 0;
   }
   else if (strcmp(strupr(NeuronType), NT_VIF) == 0) // VIF neuron?
   {
      setNeuronType_VIF();
      return 0;
   }
   else if (strcmp(strupr(NeuronType), NT_VIFCA) == 0) // VIFCA neuron?
   {
      setNeuronType_VIFCA();
      return 0;
   }   
   /* GUIDO GIGANTE 15_09_2006 - START */
   else if (strcmp(strupr(NeuronType), NT_LIFCA) == 0) // LIFCA neuron?
   {
      setNeuronType_LIFCA();
      return 0;
   }
   /* GUIDO GIGANTE 15_09_2006 - END */

   return 1;
}



/*-------------------*
 *  LOCAL FUNCTIONS  *
 *-------------------*/

//
// TO CUSTOMIZE (2)...
//

/*--------------------------------------------*
 *                                            *
 *   LIF (Leaky Integrate-and-Fire) neuron.   *
 *                                            *
 *--------------------------------------------*/


/**
 * Sets constants and function pointers for the LIF neuron.
 */

void setNeuronType_LIF()
{
   NumNeuronVariables = NNV_LIF;
   NumParameters = NP_LIF;
   initNeuronVariables = &initStateVariables_LIF;
   updateNeuronState = &updateNeuronState_LIF;
   getNeuronState = &getNeuronState_LIF;
}


/**
 *  Set initial conditions for the state variables 
 *  of all the LIF neuron in the network
 */

int initStateVariables_LIF()
{
   int i, out = 0;
   neuron_state_LIF *StateVariables_LIF;

   StateVariables_LIF = (neuron_state_LIF *)NeuronStateVars;
   for (i=0; i<(int)NumNeurons; i++)
      switch ((int)((neuron_params_LIF *)(Neurons[i].Pop->Parameters))->InitType) {

      case NIT_LIF_RESET_POTENTIAL:
           StateVariables_LIF[i].V = ((neuron_params_LIF *)(Neurons[i].Pop->Parameters))->H;
           break;

      case NIT_LIF_RESTING_POTENTIAL:
           StateVariables_LIF[i].V = 0.0;
           break;

      default: 
         out = 1;
         break;
      }

   return out;
}


/*--------------------------*
 *   updateNeuronState_LIF  *
 *--------------------------*/

/**
 *  Updates the state variables of the <Post> LIF neuron 
 *  assuming an arriving spike <*Sp> mediated by the 
 *  synapse <*s>. If <s> is NULL the spike comes from
 *  outside.
 */

void updateNeuronState_LIF(indexn Post, // Neuron to update
                           void     *s, // pointer to the synapse with the pre-synaptic neuron.
                           spike   *Sp) // Afferent spike to manage.
{
   static real ISI;
   static real r;
   static real J;
   static timex t;
   static neuron_state_LIF *SV;
   static neuron_params_LIF *P;
   static connectivity *C;

   /*** Initializes local variables. ***/
   t  = Sp->Emission;
   SV = (neuron_state_LIF *)Neurons[Post].StateVar;
   C  = Connectivity[Neurons[Post].Pop->ID][Neurons[Sp->Neuron].Pop->ID];
   P  = (neuron_params_LIF *)(Neurons[Post].Pop->Parameters);

   /*** Updates the neuron membrane potential just before the arrival of the spike. ***/
   if (diffTimex(t, Neurons[Post].Te) > P->Tarp) {

      /*** The leakage. ***/
      r = diffTimex(Neurons[Post].Tr, t) / P->Tau;
      if (-r < 0.17)
         SV->V *= 1.0 + r * (1.0 + 0.5 * r);
      else
         SV->V *= exp(r);

      /*** Updates the synaptic state if any and gets the synaptic efficacy. ***/
      if (s != NULL)
         J = (*(C->updateSynapseState))(Post, s, C, Sp);
      else
         J = Neurons[Post].Pop->JTab[(int)(Random()*ANALOG_DEPTH)];

      /*** TEMP: Some output... It should be managed using the event queue. ***/
      if (NeuStateResults) outNeuronalState(Post, t, 1, SV->V);

      /*** Updates the neuron membrane potential after the arrival of the spike. ***/
      SV->V += J;

      /*** Is a spike emitted? ***/
      if (SV->V >= P->Theta) {

         /*** TEMP: Some output... It should be managed using the event queue. ***/
         if (NeuStateResults)
            outNeuronalState(Post, t, 1, P->Theta*3.0);

         /*** Emits a spike and resets the membrane potential. ***/
         SV->V = P->H;
         ISI = diffTimex(t, Neurons[Post].Te);
         Neurons[Post].Te = t;
         Neurons[Post].Tr = t;
         Neurons[Post].Tr.Millis += P->Tarp;
         t.Millis += DelayMin;
         addNewSpike(Post, t, ISI, 0);
         t.Millis -= DelayMin;
      } else
         Neurons[Post].Tr = t;

      /*** TEMP: Some output... It should be managed using the event queue. ***/
      if (NeuStateResults) outNeuronalState(Post, t, 1, SV->V);

   /*** Updates the neuron state during the absolute refractory period (ARP). ***/
   } else
      /*** Updates the synaptic state if any and gets the synaptic efficacy. ***/
      if (s != NULL)
         J = (*(C->updateSynapseState))(Post, s, C, Sp);
      else
         J = Neurons[Post].Pop->JTab[(int)(Random()*ANALOG_DEPTH)];
   
   /*** TEMP: Some output... It should be managed using the event queue. ***/
   if (CurrentResults) 
      if (s != NULL)
         updateCurrent(Post, Sp->Neuron, J);
      else
         updateCurrent(Post, -1, J);
}


/*----------------------*
 *  getNeuronState_LIF  *
 *----------------------*

/**
 *  Returns, filling the neuron_state field <ns>, the state of
 *  the LIF neuron <i>.
 *  StateVars composition: {Membrane potential}.
 */

void getNeuronState_LIF(indexn         i, // Neuron index.
                        timex          t, // Time to which compute the neuronal state.
                        neuron_state *ns) // Neuronal state to return.
{
   static real r;
   static neuron_state_LIF *SV;
   static neuron_params_LIF *P;

   /*** Initializes local variables. ***/
   SV = (neuron_state_LIF *)Neurons[i].StateVar;
   P  = (neuron_params_LIF *)(Neurons[i].Pop->Parameters);
   ns->NumStateVars = 1;

   /*** Updates the neuron state at <t>. ***/
   if (diffTimex(t, Neurons[i].Te) > P->Tarp) {

      /*** The leakage. ***/
      r = diffTimex(Neurons[i].Tr, t) / P->Tau;
      if (-r < 0.17)
         ns->StateVars[0] = SV->V * (1.0 + r * (1.0 + 0.5 * r));
      else
         ns->StateVars[0] = SV->V * exp(r);

   } else 
      ns->StateVars[0] = P->H;
}



/* GUIDO GIGANTE 15_09_2006 - START */
/*-----------------------------------------------------------*
 *                                                           *
 *   LIFCA (LIF with frequency adaptation due to Calcium).   *
 *                                                           *
 *-----------------------------------------------------------*/


/**
 * Sets constants and function pointers for the LIFCA neuron.
 */

void setNeuronType_LIFCA()
{
   NumNeuronVariables = NNV_LIFCA;
   NumParameters = NP_LIFCA;
   initNeuronVariables = &initStateVariables_LIFCA;
   updateNeuronState = &updateNeuronState_LIFCA;
   getNeuronState = &getNeuronState_LIFCA;
}


/**
 *  Set initial conditions for the state variables 
 *  of all the LIFCA neuron in the network
 */

int initStateVariables_LIFCA()
{
   int i, out = 0;
   neuron_state_LIFCA *StateVariables_LIFCA;

   StateVariables_LIFCA = (neuron_state_LIFCA *)NeuronStateVars;
   for (i=0; i<(int)NumNeurons; i++)
      switch ((int)((neuron_params_LIFCA *)(Neurons[i].Pop->Parameters))->InitType) {

      case NIT_LIFCA_RESET_POTENTIAL:
           StateVariables_LIFCA[i].V = ((neuron_params_LIFCA *)(Neurons[i].Pop->Parameters))->H;
           StateVariables_LIFCA[i].C = 0.0;
           break;

      case NIT_LIFCA_RESTING_POTENTIAL:
           StateVariables_LIFCA[i].V = 0.0;
           StateVariables_LIFCA[i].C = 0.0;
           break;

      default: 
         out = 1;
         break;
      }

   return out;
}


/*----------------------------*
 *   updateNeuronState_LIFCA  *
 *----------------------------*/

/**
 *  Updates the state variables of the <Post> LIFCA neuron 
 *  assuming an arriving spike <*Sp> mediated by the 
 *  synapse <*s>. If <s> is NULL the spike come from
 *  outside.
 */

void updateNeuronState_LIFCA(indexn Post, // Neuron to update
                             void     *s, // pointer to the synapse with the pre-synaptic neuron.
                             spike   *Sp) // Afferent spike to manage.
{
   static real ISI;
   static real J, c0, deltaT;
   static real rm, rc, erm, erc;
   static real TFLES; // Time From Last Emitted Spike
   static timex t;
   static neuron_state_LIFCA *SV;
   static neuron_params_LIFCA *P;
   static connectivity *C;

   /*** Initializes local variables. ***/
   t  = Sp->Emission;
   SV = (neuron_state_LIFCA *)Neurons[Post].StateVar;
   C  = Connectivity[Neurons[Post].Pop->ID][Neurons[Sp->Neuron].Pop->ID];
   P  = (neuron_params_LIFCA *)(Neurons[Post].Pop->Parameters);
   deltaT = diffTimex(t, Neurons[Post].Tr);
   TFLES = diffTimex(t, Neurons[Post].Te);

   /*** Updates the neuron state just before the arrival of the spike. ***/
   if (TFLES > P->Tarp) {

      /*** Deterministic dynamics between consecutive incoming spikes. ***/
      if (diffTimex(Neurons[Post].Tr, Neurons[Post].Te) < P->Tarp) {
         SV->C *= exp(-(deltaT - TFLES + P->Tarp) / P->TauC);
         deltaT = TFLES - P->Tarp;
      }
      c0     = SV->C;

	  rc = -deltaT / P->TauC;
	  rm = -deltaT / P->Tau;

	  /*
      if (-r < 0.17)
         SV->V *= 1.0 + r * (1.0 + 0.5 * r);
      else
         SV->V *= exp(r);
		 */

	  erm = exp(rm);
	  erc = exp(rc);
	  SV->V = SV->V * erm - P->gC * (P->TauC*P->Tau) / (P->TauC-P->Tau) * c0 * (erc-erm);
	  SV->C *= erc;

      /*** TEMP: Some output... It should be managed using the event queue. ***/
      if (NeuStateResults) outNeuronalState(Post, t, 2, SV->V, SV->C);

      /*** Updates the synaptic state if any and gets the synaptic efficacy. ***/
      if (s != NULL)
         J = (*(C->updateSynapseState))(Post, s, C, Sp);
      else
         J = Neurons[Post].Pop->JTab[(int)(Random()*ANALOG_DEPTH)];

      /*** Updates the neuron membrane potential after the arrival of the spike. ***/
      SV->V += J;

      /*** Is a spike emitted? ***/
      if (SV->V >= P->Theta) {

         /*** TEMP: Some output... It should be managed using the event queue. ***/
         if (NeuStateResults) outNeuronalState(Post, t, 2, P->Theta*3.0, SV->C);

         /*** Emits a spike and resets the membrane potential. ***/
         SV->V = P->H;
         SV->C += P->AlphaC;
         ISI = diffTimex(t, Neurons[Post].Te);
         Neurons[Post].Te = t;
         t.Millis += DelayMin;
         addNewSpike(Post, t, ISI, 0);
         t.Millis -= DelayMin;
      }

   /*** Updates the neuron state during the absolute refractory period (ARP). ***/
   } else {
      /*** Deterministic dynamics between consecutive incoming spikes. ***/
      SV->C *= exp(-deltaT / P->TauC);

      /*** Updates the synaptic state if any and gets the synaptic efficacy. ***/
      if (s != NULL)
         J = (*(C->updateSynapseState))(Post, s, C, Sp);
      else
         J = Neurons[Post].Pop->JTab[(int)(Random()*ANALOG_DEPTH)];
   }

   Neurons[Post].Tr = t;

   /*** TEMP: Some output... It should be managed using the event queue. ***/
   if (NeuStateResults) outNeuronalState(Post, t, 2, SV->V, SV->C);
   if (CurrentResults) 
      if (s != NULL)
         updateCurrent(Post, Sp->Neuron, J);
      else
         updateCurrent(Post, -1, J);
}


/*------------------------*
 *  getNeuronState_LIFCA  *
 *------------------------*

/**
 *  Returns, filling the neuron_state field <ns>, the state of
 *  the LIFCA neuron <i>.
 *  StateVars composition: {Membrane potential, Calcium concentration}.
 */

void getNeuronState_LIFCA(indexn         i, // Neuron index.
                          timex          t, // Time to which compute the neuronal state.
                          neuron_state *ns) // Neuronal state to return.
{
   static real c0, deltaT;
   static real rm, rc, erm, erc;
   static real TFLES; // Time From Last Emitted Spike
   static neuron_state_LIFCA *SV;
   static neuron_params_LIFCA *P;

   /*** Initializes local variables. ***/
   SV = (neuron_state_LIFCA *)Neurons[i].StateVar;
   P  = (neuron_params_LIFCA *)(Neurons[i].Pop->Parameters);
   ns->NumStateVars = NNV_LIFCA;
   deltaT = diffTimex(t, Neurons[i].Tr);
   TFLES = diffTimex(t, Neurons[i].Te);

   /*** Updates the neuron state at <t>. ***/
   if (TFLES > P->Tarp) {

      /*** Deterministic dynamics between consecutive incoming spikes. ***/
      if (diffTimex(Neurons[i].Tr, Neurons[i].Te) < P->Tarp) {
         ns->StateVars[1] = SV->C * exp(-(deltaT - TFLES + P->Tarp) / P->TauC);
         deltaT = TFLES - P->Tarp;
      } else
         ns->StateVars[1] = SV->C;
      c0     = ns->StateVars[1];

	  rc = -deltaT / P->TauC;
	  rm = -deltaT / P->Tau;
	  /*
      if (-r < 0.17)
         SV->V *= 1.0 + r * (1.0 + 0.5 * r);
      else
         SV->V *= exp(r);
		 */
	  erm = exp(rm);
	  erc = exp(rc);
	  ns->StateVars[0] = SV->V * erm - P->gC * (P->TauC*P->Tau) / (P->TauC-P->Tau) * c0 * (erc-erm);
	  ns->StateVars[1] *= erc;

   } else {
      ns->StateVars[0] = P->H;
      ns->StateVars[1] = SV->C * exp(-deltaT / P->TauC);
   }

}
/* GUIDO GIGANTE 15_09_2006 - END */



/*-------------------------------------------*
 *                                           *
 *   VIF (VLSI Integrate-and-Fire) neuron.   *
 *                                           *
 *-------------------------------------------*/


/**
 * Sets constants and function pointers for the LIF neuron.
 */

void setNeuronType_VIF()
{
   NumNeuronVariables = NNV_VIF;
   NumParameters = NP_VIF;
   initNeuronVariables = &initStateVariables_VIF;
   updateNeuronState = &updateNeuronState_VIF;
   getNeuronState = &getNeuronState_VIF;
}


/**
 *  Set initial conditions for the state variables 
 *  of all the VIF neuron in the network
 */

int initStateVariables_VIF()
{
   int i, out = 0;
   neuron_state_VIF *StateVariables_VIF;

   StateVariables_VIF = (neuron_state_VIF *)NeuronStateVars;
   for (i=0; i<(int)NumNeurons; i++)
      switch ((int)((neuron_params_VIF *)(Neurons[i].Pop->Parameters))->InitType) {

      case NIT_VIF_RESET_POTENTIAL:
           StateVariables_VIF[i].V = ((neuron_params_VIF *)(Neurons[i].Pop->Parameters))->H;
           break;

      case NIT_VIF_RESTING_POTENTIAL:
           StateVariables_VIF[i].V = 0.0;
           break;

      default: 
         out = 1;
         break;
      }

   return out;
}


/*--------------------------*
 *   updateNeuronState_VIF  *
 *--------------------------*/

/**
 *  Updates the state variables of the <Post> VIF neuron 
 *  assuming an arriving spike <*Sp> mediated by the 
 *  synapse <*s>. If <s> is NULL the spike come from
 *  outside.
 */

void updateNeuronState_VIF(indexn Post, // Neuron to update
                           void     *s, // pointer to the synapse with the pre-synaptic neuron.
                           spike   *Sp) // Afferent spike to manage.
{
   static real ISI;
   static real J;
   static timex t;
   static neuron_state_VIF *SV;
   static neuron_params_VIF *P;
   static connectivity *C;

   /*** Initializes local variables. ***/
   t  = Sp->Emission;
   SV = (neuron_state_VIF *)Neurons[Post].StateVar;
   C  = Connectivity[Neurons[Post].Pop->ID][Neurons[Sp->Neuron].Pop->ID];
   P  = (neuron_params_VIF *)(Neurons[Post].Pop->Parameters);

   /*** Updates the neuron membrane potential just before the arrival of the spike. ***/
   if (diffTimex(t, Neurons[Post].Te) > P->Tarp) {

      /*** The constant leakage. ***/
      SV->V -= diffTimex(t, Neurons[Post].Tr) * P->Beta;
      if (SV->V < 0.0) SV->V = 0.0; // The reflecting barrier.

      /*** Updates the synaptic state if any and gets the synaptic efficacy. ***/
      if (s != NULL)
         J = (*(C->updateSynapseState))(Post, s, C, Sp);
      else
         J = Neurons[Post].Pop->JTab[(int)(Random()*ANALOG_DEPTH)];

      /*** TEMP: Some output... It should be managed using the event queue. ***/
      if (NeuStateResults) outNeuronalState(Post, t, 1, SV->V);

      /*** Updates the neuron membrane potential after the arrival of the spike. ***/
      SV->V += J;
      if (SV->V < 0.0) SV->V = 0.0; // The reflecting barrier.

      /*** Is a spike emitted? ***/
      if (SV->V >= P->Theta) {

         /*** TEMP: Some output... It should be managed using the event queue. ***/
         if (NeuStateResults) outNeuronalState(Post, t, 1, P->Theta*3.0);

         /*** Emits a spike and resets the membrane potential. ***/
         SV->V = P->H;
         ISI = diffTimex(t, Neurons[Post].Te);
         Neurons[Post].Te = t;
         Neurons[Post].Tr = t;
         Neurons[Post].Tr.Millis += P->Tarp;
         t.Millis += DelayMin;
         addNewSpike(Post, t, ISI, 0);
         t.Millis -= DelayMin;
      } else
         Neurons[Post].Tr = t;

      /*** TEMP: Some output... It should be managed using the event queue. ***/
      if (NeuStateResults) outNeuronalState(Post, t, 1, SV->V);

   /*** Updates the neuron state during the absolute refractory period (ARP). ***/
   } else
      /*** Updates the synaptic state if any and gets the synaptic efficacy. ***/
      if (s != NULL)
         J = (*(C->updateSynapseState))(Post, s, C, Sp);
      else
         J = Neurons[Post].Pop->JTab[(int)(Random()*ANALOG_DEPTH)];

   /*** TEMP: Some output... It should be managed using the event queue. ***/
   if (CurrentResults) 
      if (s != NULL)
         updateCurrent(Post, Sp->Neuron, J);
      else
         updateCurrent(Post, -1, J);

}


/*----------------------*
 *  getNeuronState_VIF  *
 *----------------------*

/**
 *  Returns, filling the neuron_state field <ns>, the state of
 *  the VIF neuron <i>.
 *  StateVars composition: {Membrane potential}.
 */

void getNeuronState_VIF(indexn         i, // Neuron index.
                        timex          t, // Time to which compute the neuronal state.
                        neuron_state *ns) // Neuronal state to return.
{
   static neuron_state_VIF *SV;
   static neuron_params_VIF *P;

   /*** Initializes local variables. ***/
   SV = (neuron_state_VIF *)Neurons[i].StateVar;
   P  = (neuron_params_VIF *)(Neurons[i].Pop->Parameters);
   ns->NumStateVars = 1;

   /*** Updates the neuron state at <t>. ***/
   if (diffTimex(t, Neurons[i].Te) > P->Tarp) {

      /*** The leakage. ***/
      ns->StateVars[0] = SV->V - diffTimex(t, Neurons[i].Tr) * P->Beta;
      if (ns->StateVars[0] < 0.0) ns->StateVars[0] = 0.0; // The reflecting barrier.

   } else 
      ns->StateVars[0] = P->H;
}



/*-----------------------------------------------------------*
 *                                                           *
 *   VIFCA (VIF with frequency adaptation due to Calcium).   *
 *                                                           *
 *-----------------------------------------------------------*/


/**
 * Sets constants and function pointers for the VIFCA neuron.
 */

void setNeuronType_VIFCA()
{
   NumNeuronVariables = NNV_VIFCA;
   NumParameters = NP_VIFCA;
   initNeuronVariables = &initStateVariables_VIFCA;
   updateNeuronState = &updateNeuronState_VIFCA;
   getNeuronState = &getNeuronState_VIFCA;
}


/**
 *  Set initial conditions for the state variables 
 *  of all the VIFCA neuron in the network
 */

int initStateVariables_VIFCA()
{
   int i, out = 0;
   neuron_state_VIFCA *StateVariables_VIFCA;

   StateVariables_VIFCA = (neuron_state_VIFCA *)NeuronStateVars;
   for (i=0; i<(int)NumNeurons; i++)
      switch ((int)((neuron_params_VIFCA *)(Neurons[i].Pop->Parameters))->InitType) {

      case NIT_VIFCA_RESET_POTENTIAL:
           StateVariables_VIFCA[i].V = ((neuron_params_VIFCA *)(Neurons[i].Pop->Parameters))->H;
           StateVariables_VIFCA[i].C = 0.0;
           break;

      case NIT_VIFCA_RESTING_POTENTIAL:
           StateVariables_VIFCA[i].V = 0.0;
           StateVariables_VIFCA[i].C = 0.0;
           break;

      default: 
         out = 1;
         break;
      }

   return out;
}


/*----------------------------*
 *   updateNeuronState_VIFCA  *
 *----------------------------*/

/**
 *  Updates the state variables of the <Post> VIFCA neuron 
 *  assuming an arriving spike <*Sp> mediated by the 
 *  synapse <*s>. If <s> is NULL the spike come from
 *  outside.
 */

void updateNeuronState_VIFCA(indexn Post, // Neuron to update
                             void     *s, // pointer to the synapse with the pre-synaptic neuron.
                             spike   *Sp) // Afferent spike to manage.
{
   static real ISI;
   static real J, c0, deltaT;
   static real TFLES; // Time From Last Emitted Spike
   static timex t;
   static neuron_state_VIFCA *SV;
   static neuron_params_VIFCA *P;
   static connectivity *C;

   /*** Initializes local variables. ***/
   t  = Sp->Emission;
   SV = (neuron_state_VIFCA *)Neurons[Post].StateVar;
   C  = Connectivity[Neurons[Post].Pop->ID][Neurons[Sp->Neuron].Pop->ID];
   P  = (neuron_params_VIFCA *)(Neurons[Post].Pop->Parameters);
   deltaT = diffTimex(t, Neurons[Post].Tr);
   TFLES = diffTimex(t, Neurons[Post].Te);

   /*** Updates the neuron state just before the arrival of the spike. ***/
   if (TFLES > P->Tarp) {

      /*** Deterministic dynamics between consecutive incoming spikes. ***/
      if (diffTimex(Neurons[Post].Tr, Neurons[Post].Te) < P->Tarp) {
         SV->C *= exp(-(deltaT - TFLES + P->Tarp) / P->TauC);
         deltaT = TFLES - P->Tarp;
      }
      c0     = SV->C;
      SV->C *= exp(-deltaT / P->TauC);
      SV->V -= P->Beta * deltaT + P->gC * P->TauC * (c0 - SV->C);
      if (SV->V < 0.0) SV->V = 0.0; /* Reflecting barrier. */

      /*** TEMP: Some output... It should be managed using the event queue. ***/
      if (NeuStateResults) outNeuronalState(Post, t, 2, SV->V, SV->C);

      /*** Updates the synaptic state if any and gets the synaptic efficacy. ***/
      if (s != NULL)
         J = (*(C->updateSynapseState))(Post, s, C, Sp);
      else
         J = Neurons[Post].Pop->JTab[(int)(Random()*ANALOG_DEPTH)];

      /*** Updates the neuron membrane potential after the arrival of the spike. ***/
      SV->V += J;
      if (SV->V < 0.0) SV->V = 0.0; // The reflecting barrier.

      /*** Is a spike emitted? ***/
      if (SV->V >= P->Theta) {

         /*** TEMP: Some output... It should be managed using the event queue. ***/
         if (NeuStateResults) outNeuronalState(Post, t, 2, P->Theta*3.0, SV->C);

         /*** Emits a spike and resets the membrane potential. ***/
         SV->V = P->H;
         SV->C += P->AlphaC;
         ISI = diffTimex(t, Neurons[Post].Te);
         Neurons[Post].Te = t;
         t.Millis += DelayMin;
         addNewSpike(Post, t, ISI, 0);
         t.Millis -= DelayMin;
      }

   /*** Updates the neuron state during the absolute refractory period (ARP). ***/
   } else {
      /*** Deterministic dynamics between consecutive incoming spikes. ***/
      SV->C *= exp(-deltaT / P->TauC);

      /*** Updates the synaptic state if any and gets the synaptic efficacy. ***/
      if (s != NULL)
         J = (*(C->updateSynapseState))(Post, s, C, Sp);
      else
         J = Neurons[Post].Pop->JTab[(int)(Random()*ANALOG_DEPTH)];
   }

   Neurons[Post].Tr = t;

   /*** TEMP: Some output... It should be managed using the event queue. ***/
   if (NeuStateResults) outNeuronalState(Post, t, 2, SV->V, SV->C);
   if (CurrentResults) 
      if (s != NULL)
         updateCurrent(Post, Sp->Neuron, J);
      else
         updateCurrent(Post, -1, J);
}


/*------------------------*
 *  getNeuronState_VIFCA  *
 *------------------------*

/**
 *  Returns, filling the neuron_state field <ns>, the state of
 *  the VIFCA neuron <i>.
 *  StateVars composition: {Membrane potential, Calcium concentration}.
 */

void getNeuronState_VIFCA(indexn         i, // Neuron index.
                          timex          t, // Time to which compute the neuronal state.
                          neuron_state *ns) // Neuronal state to return.
{
   static real c0, deltaT;
   static real TFLES; // Time From Last Emitted Spike
   static neuron_state_VIFCA *SV;
   static neuron_params_VIFCA *P;

   /*** Initializes local variables. ***/
   SV = (neuron_state_VIFCA *)Neurons[i].StateVar;
   P  = (neuron_params_VIFCA *)(Neurons[i].Pop->Parameters);
   ns->NumStateVars = NNV_VIFCA;
   deltaT = diffTimex(t, Neurons[i].Tr);
   TFLES = diffTimex(t, Neurons[i].Te);

   /*** Updates the neuron state at <t>. ***/
   if (TFLES > P->Tarp) {

      /*** Deterministic dynamics between consecutive incoming spikes. ***/
      if (diffTimex(Neurons[i].Tr, Neurons[i].Te) < P->Tarp) {
         ns->StateVars[1] = SV->C * exp(-(deltaT - TFLES + P->Tarp) / P->TauC);
         deltaT = TFLES - P->Tarp;
      } else
         ns->StateVars[1] = SV->C;
      c0     = ns->StateVars[1];
      ns->StateVars[1] *= exp(-deltaT / P->TauC);
      ns->StateVars[0] = SV->V - (P->Beta * deltaT + P->gC * P->TauC * (c0 - ns->StateVars[1]));
      if (ns->StateVars[0] < 0.0) ns->StateVars[0] = 0.0; /* Reflecting barrier. */

   } else {
      ns->StateVars[0] = P->H;
      ns->StateVars[1] = SV->C * exp(-deltaT / P->TauC);
   }

}



#undef STRING_SIZE