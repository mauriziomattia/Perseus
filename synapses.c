/*
 *
 *   synapses.c
 *
 *   Library of structures and functions to manage 
 *   different types of synapses (Fixed, ...).
 *
 *   Project: PERSEO 2.0
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "erflib.h"
#include "randdev.h"

#include "types.h"
#include "perseo.h"
#include "connectivity.h"
#include "synapses.h"
#include "results.h"
#include "modules.h"



/*---------------------*
 *  LOCAL DEFINITIONS  *
 *---------------------*/

#define BUFFER_SIZE          16 /* Size of the buffer for variable arrays. */ 



/*----------------------*
 *   GLOBAL VARIABLES   *
 *----------------------*/

real **JTabArray; /* Array of the LUT for the synaptic efficacies, an array of synaptic 
                     values for each stable state of the synapse. */
int NumJTabArray; /* Number of elements in the JTabArray. */



/*------------------------*
 *   FUNCTION DEFINITION  *
 *------------------------*/


/*----------------------*
 *  getSynapseType  *
 *----------------------*/

/**
 *  Returns the integer representation of the synapse type,
 *  if the string is among the known ones. Return -1 otherwise.
 */

int getSynapseType(char * SynapseType) 
{
   int out = -1;

//
// TO CUSTOMIZE (0)...
//
   if (strcmp(strupr(SynapseType), STS_FXD) == 0)
      out = ST_FXD;
   else if (strcmp(strupr(SynapseType), STS_AF) == 0)
      out = ST_AF;
   else if (strcmp(strupr(SynapseType), STS_TWAM) == 0)
      out = ST_TWAM;

   return out;
}


/*----------------------*
 *  parseSynapseParams  *
 *----------------------*/

/**
 *  Parses the synapse parameters read from the ConnectivityFile "connectivity.ini".
 *  Returns 1 upon error, 0 otherwise.
 */

int parseSynapseParams(int SynapseType, 
                       int NumSynapseParameters, 
                       IVreal * SynapseParams) 
{
   int out = 1;

//
// TO CUSTOMIZE (1)...
//
   if (SynapseType == ST_FXD)
      out = parseSynapseParams_FXD(NumSynapseParameters, SynapseParams);
   else if (SynapseType == ST_AF)
      out = parseSynapseParams_AF(NumSynapseParameters, SynapseParams);
   else if (SynapseType == ST_TWAM)
      out = parseSynapseParams_TWAM(NumSynapseParameters, SynapseParams);

   return out;
}


/*--------------------------*
 *  setSynapticEffaciesLUT  *
 *--------------------------*/

/**
 *  Makes or updates the LUT (Look-Up Table) for allowed 
 *  values of the synaptic efficacies.
 *  Returns 1 if there is not enough memory, 0 otherwise.
 */

int setSynapticEffaciesLUT(connectivity * c)
{
   static int JTabArraySize = 0;
          real     **newJTA;

   int i;
   int out = 1;

   if (c->JTab == NULL)
   {
//
// TO CUSTOMIZE (2) ...
//
       /*** Sets the number of stable states of the synapse. ***/
       if (c->SynapseType == ST_FXD)
           c->NumSynapseStableState = NSSS_FXD;
       else if (c->SynapseType == ST_AF)
           c->NumSynapseStableState = NSSS_AF;
       else if (c->SynapseType == ST_TWAM)
           c->NumSynapseStableState = NSSS_TWAM;

       /*** Allocate memory for the LUT. ***/
       NumJTabArray += c->NumSynapseStableState; 
       while (NumJTabArray > JTabArraySize) { // More memory is needed?
           JTabArraySize += BUFFER_SIZE;
           if (JTabArraySize == BUFFER_SIZE)
               JTabArray = getMemory(sizeof(real *) * JTabArraySize, "ERROR (setSynapticEffaciesLUT): Out of memory.");
           else {
               newJTA = realloc(JTabArray, sizeof(real *) * JTabArraySize);
               for (i=0; i<NumConnectivityArray; i++)
                   ConnectivityArray[i].JTab = (ConnectivityArray[i].JTab - JTabArray) + newJTA;
               JTabArray = newJTA;
               MemoryAmount += sizeof(real *) * BUFFER_SIZE;
               if (JTabArray == NULL)
                   printFatalError("setSynapticEffaciesLUT", "Out of memory.");
           }
       }
       c->JTab = &JTabArray[NumJTabArray - c->NumSynapseStableState];
       for (i=0; i<c->NumSynapseStableState; i++)
          c->JTab[i] = NULL;
   }

//
// TO CUSTOMIZE (3) ...
//
   /*** Makes or updates the LUT. ***/
   if (c->SynapseType == ST_FXD)
      out = setSynapticEffaciesLUT_FXD(c);
   else if (c->SynapseType == ST_AF)
      out = setSynapticEffaciesLUT_AF(c);
   else if (c->SynapseType == ST_TWAM)
      out = setSynapticEffaciesLUT_TWAM(c);

   return out;
}


/*--------------------------------*
 *  setConnectivitySynapseFields  *
 *--------------------------------*/

/**
 *  Sets the field of connectivity related to the synaptic model
 *  like pointer to functions setting the state of the synapse and
 *  the size of the synapse itself.
 *  The synaptic size includes the indexes (byte) to the synaptic 
 *  efficacies LUT and the state variables (in general float to 
 *  save space). The value is NSV_# * sizeof(float) + NSSS_# * sizeof(byte)
 *  # stands for the synapse type (FXD, ...).
 */

void setConnectivitySynapseFields(connectivity * c)
{
//
// TO CUSTOMIZE (4) ...
//
   /*** Is it a FXD synapse?. ***/
   if (c->SynapseType == ST_FXD) {
      c->SynapseSize = sizeof(synapse_FXD);
      c->initSynapseState = &initSynapseState_FXD;
      c->updateSynapseState = &updateSynapseState_FXD;
      c->getSynapseState = &getSynapseState_FXD;
 
   /*** Is it a AF synapse?. ***/
   } else if (c->SynapseType == ST_AF) {
      c->SynapseSize = sizeof(synapse_AF);
      c->initSynapseState = &initSynapseState_AF;
      c->updateSynapseState = &updateSynapseState_AF;
      c->getSynapseState = &getSynapseState_AF;

   /*** Is it a TWAM synapse?. ***/
   } else if (c->SynapseType == ST_TWAM) {
      c->SynapseSize = sizeof(synapse_TWAM);
      c->initSynapseState = &initSynapseState_TWAM;
      c->updateSynapseState = &updateSynapseState_TWAM;
      c->getSynapseState = &getSynapseState_TWAM;
   }
}


/*--------------------*
 *  initSynapseState  *
 *--------------------*/

/**
 *  Initialize the state variables and the stable states of 
 *  the synapse addressed by the function parameters.
 */

void initSynapseState(indexn        i, // post-synaptic neuron.
                      indexn        j, // pre-synaptic neuron.
                      void         *s, // pointer to the synapse.
                      connectivity *c, // pointer to the synaptic population.
                      int           l) // Layer corresponding to the transmission delay.
{
   if (c->initSynapseState != NULL)
      (*(c->initSynapseState))(i,j,s,c,l);
}



//
// TO CUSTOMIZE (5)...
//

/*--------------------------*
 *                          *
 *   FXD (Fixed) synapse.   *
 *                          *
 *--------------------------*/


/*--------------------------*
 *  parseSynapseParams_FXD  *
 *--------------------------*

/**
 *  Parses the synapse parameters read from the ConnectivityFile "connectivity.ini"
 *  for the fixed instantaneous synapses.
 *  Returns 1 upon error, 0 otherwise.
 */

int parseSynapseParams_FXD(int NumSynapseParameters, 
                           IVreal * SynapseParams)
{
   if (NumSynapseParameters != NSP_FXD) {
      printError("parseSynapseParams_FXD", "Number of synaptic parameters is incorrect.\n");
      return 1;
   }

   /*** Semantic parsing. ***/
   if (SynapseParams[1] < 0.0) { 
      printError("parseSynapseParams_FXD", "Relative st.dev. of synaptic efficacy have to be non negative.\n");
      return 1;
   }

   return 0;
}


/*------------------------------*
 *  setSynapticEffaciesLUT_FXD  *
 *------------------------------*

/**
 *  Makes or updates the LUT (Look-Up Table) for allowed 
 *  values of the synaptic efficacies.
 *  Returns 1 if there is not enough memory, 0 otherwise.
 */

int setSynapticEffaciesLUT_FXD(connectivity * c)
{
   synapse_params_FXD * sp;
   int outMGLUT;

   sp = (synapse_params_FXD *)(c->Parameters);

   /*** Definition of the JTab. ***/
   if (sp->J>0)
      outMGLUT = makeGaussianLUT(c->JTab, ANALOG_DEPTH, sp->J, sp->J*sp->DJ, 0.0, 2.0*sp->J);
   else
      outMGLUT = makeGaussianLUT(c->JTab, ANALOG_DEPTH, sp->J, -sp->J*sp->DJ, 2.0*sp->J, 0.0);
   if (outMGLUT)
      printFatalError("setSynapticEffaciesLUT_FXD", "Out of memory.");

   return 0;
}


/*------------------------*
 *  initSynapseState_FXD  *
 *------------------------*

/**
 *  Initializes the stable states of the FXD synapse addressed 
 *  by the function parameters.
 */

void initSynapseState_FXD(indexn        i, // post-synaptic neuron.
                          indexn        j, // pre-synaptic neuron.
                          void         *s, // pointer to the synapse.
                          connectivity *c, // pointer to the synaptic population.
                          int           l) // Layer corresponding to the transmission delay.
{
   synapse_FXD * ls = s;
   ls->Jndx = (int)(Random()*ANALOG_DEPTH);
}


/*--------------------------*
 *  updateSynapseState_FXD  *
 *--------------------------*

/**
 *  Return the synaptic efficacy of the fixed synapse.
 */

real updateSynapseState_FXD(indexn        i, // post-synaptic neuron.
                            void         *s, // pointer to the synapse.
                            connectivity *c, // pointer to the synaptic population.
                            spike       *sp) // The spike to transmit.
{
   synapse_FXD * ls = s;

   return c->JTab[0][ls->Jndx];
}


/*-----------------------*
 *  getSynapseState_FXD  *
 *-----------------------*

/**
 *  Returns, filling the synapse_state field <ss>, the state of
 *  the FXD synapse <s> having as pre- and post-synaptic neurons 
 *  <j> and <i> respectively. The synapse belong to the connectivity
 *  block address by <c> and transmits spikes with delay index <l>.
 *  StateVars composition: {Synaptic efficacy}.
 */

void getSynapseState_FXD(indexn          i, // post-synaptic neuron.
                         indexn          j, // pre-synaptic neuron.
                         void           *s, // pointer to the synapse.
                         connectivity   *c, // pointer to the synaptic population.
                         int             l, // Layer corresponding to the transmission delay.
                         timex           t, // Time to which compute the synaptic state.
                         synapse_state *ss) // Synaptic state to return.
{
   synapse_FXD * ls = s;
   ss->NumStateVars = 1;
   ss->StateVars[0] = c->JTab[0][ls->Jndx];
}



/*-----------------------------------*
 *                                   *
 *   AF (Annunziato-Fusi) synapse.   *
 *                                   *
 *-----------------------------------*/


/*-------------------------*
 *  parseSynapseParams_AF  *
 *-------------------------*

/**
 *  Parses the synapse parameters read from 
 *  the ConnectivityFile "connectivity.ini"
 *  for the Annunziato-Fusi synapse dynamical synapses.
 *  Returns 1 upon error, 0 otherwise.
 */

int parseSynapseParams_AF(int NumSynapseParameters, 
                          IVreal * SynapseParams)
{
   int  k;
   real ParamArray[NSP_AF];
   synapse_params_AF * sp;

   if (NumSynapseParameters != NSP_AF) {
      printError("parseSynapseParams_AF", "Number of synaptic parameters is incorrect.\n");
      return 1;
   }

   /*** Parameter conversion. ***/
   for (k=0; k<NSP_AF; k++) ParamArray[k] = (real)SynapseParams[k];
   sp = (synapse_params_AF *)ParamArray;

   /*** Semantic parsing. ***/
   if (sp->DJ0 < 0.0 || sp->DJ1 < 0.0) { 
      printError("parseSynapseParams_AF", "Relative st.dev. of synaptic efficacy have to be non negative.\n");
      return 1;
   }
   if (sp->J1 < sp->J0) { 
      printError("parseSynapseParams_AF", "Potentiated synaptic efficacy lower than the depressed one.\n");
      return 1;
   }
   if (!(sp->ThetaJ >= sp->RBdown && sp->RBup >= sp->ThetaJ)) { 
      printError("parseSynapseParams_AF", "Bad reflecting barriers position with respect to VJ threshold.\n");
      return 1;
   }
   if (sp->R0 < 0.0 || sp->R0 > 1.0) { 
      printError("parseSynapseParams_AF", "Fraction of initially potentiated synapses have to be between 0 and 1.\n");
      return 1;
   }

   return 0;
}


/*------------------------------*
 *  setSynapticEffaciesLUT_AF  *
 *------------------------------*

/**
 *  Makes or updates the LUT (Look-Up Table) for allowed 
 *  values of the synaptic efficacies.
 *  Returns 1 if there is not enough memory, 0 otherwise.
 */

int setSynapticEffaciesLUT_AF(connectivity * c)
{
   synapse_params_AF * sp = (synapse_params_AF *)(c->Parameters);

   /*** Definition of the efficacies LUT. ***/
   if (makeGaussianLUT(&(c->JTab[0]), ANALOG_DEPTH, sp->J0, sp->J0*sp->DJ0, 0.0, 2.0*sp->J0) |
       makeGaussianLUT(&(c->JTab[1]), ANALOG_DEPTH, sp->J1, sp->J1*sp->DJ1, 0.0, 2.0*sp->J1))
      printFatalError("setSynapticEffaciesLUT_AF", "Out of memory.");

   return 0;
}


/*-----------------------*
 *  initSynapseState_AF  *
 *-----------------------*

/**
 *  Initializes the stable states (long-term synaptic efficacies)
 *  of the AF synapse addressed by the function parameters.
 */

void initSynapseState_AF(indexn        i, // post-synaptic neuron.
                         indexn        j, // pre-synaptic neuron.
                         void         *s, // pointer to the synapse.
                         connectivity *c, // pointer to the synaptic population.
                         int           l) // Layer corresponding to the transmission delay.
{
   synapse_AF * ss = s;                   // Synaptic state.
   synapse_params_AF * sp = (synapse_params_AF *)c->Parameters; // Synaptic parameters.

   ss->J0ndx = (int)(Random()*ANALOG_DEPTH);
   ss->J1ndx = (int)(Random()*ANALOG_DEPTH);
   if (Random() < sp->R0)
      ss->VJ = (float)sp->RBup;
   else
      ss->VJ = (float)sp->RBdown;
}


/*-------------------------*
 *  updateSynapseState_AF  *
 *-------------------------*

/**
 *  Return the synaptic efficacy of the AF synapse after 
 *  the internal variables are updated, depending on the 
 *  pre- and post-synaptic neuronal activity.
 */

real updateSynapseState_AF(indexn        i, // post-synaptic neuron.
                           void         *s, // pointer to the synapse.
                           connectivity *c, // pointer to the synaptic population.
                           spike       *sp) // The spike to transmit.
{
   static int JflagBefore, /* Long term efficacy state before and ... */ 
               JflagAfter; /* after the update of the synapse.        */
   static real     DeltaT; /* Time from the last spikes received (ISI). */
   static timex        tp; /* Time when a reflecting barrier is crossed. */
   static timex         t; /* Emission time of the spike to manage. */

   synapse_AF * ss = s;                    // Synaptic state.
   synapse_params_AF * spar = (synapse_params_AF * )c->Parameters; // Synaptic parameters.

   /*** Initialize local variables. ***/
   JflagBefore = ss->VJ > spar->ThetaJ;
   t = sp->Emission;
// TEMP: if the synaptic failure is implemented the following statement is 
//       misleading and incorrect.
   DeltaT = sp->ISI;

   /*** Constant decay or increase of VJ and check if reflecting barriers are crossed. ***/
   if (JflagBefore) {
      ss->VJ += (float)(spar->BetaJ * DeltaT);
      if (ss->VJ > spar->RBup) {
         /*** TEMP: Some output... It should be managed using the event queue. ***/
         if (SynStateResults) {
            tp = t;
            tp.Millis -= (ss->VJ - spar->RBup) / spar->BetaJ;
            outSynapticState(i, sp->Neuron, tp, 3, c->JTab[1][ss->J1ndx], 1, spar->RBup);
         }
         ss->VJ = (float)spar->RBup;
      }
   } else {
      ss->VJ -= (float)(spar->AlphaJ * DeltaT);
      if (ss->VJ < spar->RBdown) {
         /*** TEMP: Some output... It should be managed using the event queue. ***/
         if (SynStateResults) {
            tp = t;
            tp.Millis -= (spar->RBdown - ss->VJ) / spar->AlphaJ;
            outSynapticState(i, sp->Neuron, tp, 3, c->JTab[0][ss->J0ndx], 0, spar->RBdown);
         }
         ss->VJ = (float)spar->RBdown;
      }
   }

   /*** TEMP: Some output... It should be managed using the event queue. ***/
   if (SynStateResults)
      if (ss->VJ > spar->ThetaJ)
         outSynapticState(i, sp->Neuron, tp, 3, c->JTab[1][ss->J1ndx], 1, ss->VJ);
      else
         outSynapticState(i, sp->Neuron, tp, 3, c->JTab[0][ss->J0ndx], 0, ss->VJ);

   /*** VJ jump probing post-synaptic membrane potential. ***/
   if (Neurons[i].StateVar[0] > spar->ThetaV) {
      ss->VJ += (float)spar->JumpUp;
      if (ss->VJ > spar->RBup) ss->VJ = (float)spar->RBup;
   } else {
      ss->VJ -= (float)spar->JumpDown;
      if (ss->VJ < spar->RBdown) ss->VJ = (float)spar->RBdown;
   }

   /*** TEMP: Some output... It should be managed using the event queue. ***/
   if (SynStateResults)
      if (ss->VJ > spar->ThetaJ)
         outSynapticState(i, sp->Neuron, tp, 3, c->JTab[1][ss->J1ndx], 1, ss->VJ);
      else
         outSynapticState(i, sp->Neuron, tp, 3, c->JTab[0][ss->J0ndx], 0, ss->VJ);

   /*** What follows is needed to monitor the transition number. ***/
   JflagAfter  = ss->VJ > spar->ThetaJ;

   /*** TEMP: Some output... It should be managed using the event queue. ***/
   if(SynTransResults) updateSynTrans(JflagBefore,JflagAfter);
   if(detailSynTransResults) detailSynTrans(i, sp->Neuron, JflagBefore, JflagAfter, t);

   /*** TEMP: STD, short term depression. ***/
//   if (TauRec > 0.0) {                      /* Flag a run-time. */
//      S->r = (float)(1.0 + ((1.0 - Use) * S->r - 1.0)*exp(-DeltaT/TauRec));
//
//      if (JflagAfter)
//         return J1Tab[(*S).J.EE.J1]*Use*S->r;
//      else
//         return J0Tab[(*S).J.EE.J0]*Use*S->r;
//   }

   /*** Return the actual long term synaptic efficacy. ***/
   if (JflagAfter)
      return c->JTab[1][ss->J1ndx];

   return c->JTab[0][ss->J0ndx];
}


/*----------------------*
 *  getSynapseState_AF  *
 *----------------------*

/**
 *  Returns, filling the synapse_state field <ss>, the state of
 *  the AF synapse <s> having as pre- and post-synaptic neurons 
 *  <j> and <i> respectively. The synapse belong to the connectivity
 *  block address by <c> and transmits spikes with delay index <l>.
 *  StateVars composition: {Syn. efficacy, 0-Depressed/1-Potentiated, 
 *  Hidden synaptic potential VJ}.
 */

void getSynapseState_AF(indexn          i, // post-synaptic neuron.
                        indexn          j, // pre-synaptic neuron.
                        void           *s, // pointer to the synapse.
                        connectivity   *c, // pointer to the synaptic population.
                        int             l, // Layer corresponding to the transmission delay.
                        timex           t, // Time to which compute the synaptic state.
                        synapse_state *ss) // Synaptic state to return.
{
   static real DeltaT; /* Time from the last spikes received (ISI). */
   static timex    tt; /* Time as timex. */

   synapse_AF         *ls = s;                                   // Synaptic state.
   synapse_params_AF *lsp = (synapse_params_AF * )c->Parameters; // Synaptic parameters.

   ss->NumStateVars = 3;
   ss->StateVars[1] = ls->VJ > lsp->ThetaJ ? 1.0 : 0.0;

// TEMP: if the synaptic failure is implemented the following statement is 
//       misleading and incorrect.
   DeltaT = diffTimex(t, Neurons[j].Te) - SynapticMatrix[l].Delay;

   /*** Constant decay or increase of VJ and check if reflecting barriers are crossed. ***/
   if (ls->VJ > lsp->ThetaJ) {
      ss->StateVars[2] = ls->VJ + lsp->BetaJ * DeltaT;
      if (ss->StateVars[2] > lsp->RBup) 
         ss->StateVars[2] = lsp->RBup;
      ss->StateVars[0] = c->JTab[1][ls->J1ndx];
   } else {
      ss->StateVars[2] = ls->VJ - lsp->AlphaJ * DeltaT;
      if (ss->StateVars[2] < lsp->RBdown)
         ss->StateVars[2] = lsp->RBdown;
      ss->StateVars[0] = c->JTab[0][ls->J0ndx];
   }
}



/*----------------------------------------------*
 *                                              *
 *   TWAM (Time-Window Anti-Markram) synapse.   *
 *                                              *
 *----------------------------------------------*/


/*---------------------------*
 *  parseSynapseParams_TWAM  *
 *---------------------------*

/**
 *  Parses the synapse parameters read from 
 *  the ConnectivityFile "connectivity.ini"
 *  for the Time Window Anti-Markram dynamical synapses.
 *  Returns 1 upon error, 0 otherwise.
 */

int parseSynapseParams_TWAM(int NumSynapseParameters, 
                            IVreal * SynapseParams)
{
   int  k;
   real ParamArray[NSP_TWAM];
   synapse_params_TWAM * sp;

   if (NumSynapseParameters != NSP_TWAM) {
      printError("parseSynapseParams_TWAM", "Number of synaptic parameters is incorrect.\n");
      return 1;
   }

   /*** Parameter conversion. ***/
   for (k=0; k<NSP_TWAM; k++) ParamArray[k] = (real)SynapseParams[k];
   sp = (synapse_params_TWAM *)ParamArray;

   /*** Semantic parsing. ***/
   if (sp->DJ0 < 0.0 || sp->DJ1 < 0.0) { 
      printError("parseSynapseParams_TWAM", "Relative st.dev. of synaptic efficacy have to be non negative.\n");
      return 1;
   }
   if (sp->J1 < sp->J0) { 
      printError("parseSynapseParams_TWAM", "Potentiated synaptic efficacy lower than the depressed one.\n");
      return 1;
   }
   if (!(sp->ThetaJ >= sp->RBdown && sp->RBup >= sp->ThetaJ)) { 
      printError("parseSynapseParams_TWAM", "Bad reflecting barriers position with respect to VJ threshold.\n");
      return 1;
   }
   if (sp->R0 < 0.0 || sp->R0 > 1.0) { 
      printError("parseSynapseParams_TWAM", "Fraction of initially potentiated synapses have to be between 0 and 1.\n");
      return 1;
   }
   if (sp->PotWindow <= 0.0) { 
      printError("parseSynapseParams_TWAM", "Time window for potentiation have to be positive.\n");
      return 1;
   }

   return 0;
}


/*-------------------------------*
 *  setSynapticEffaciesLUT_TWAM  *
 *-------------------------------*

/**
 *  Makes or updates the LUT (Look-Up Table) for allowed 
 *  values of the synaptic efficacies.
 *  Returns 1 if there is not enough memory, 0 otherwise.
 */

int setSynapticEffaciesLUT_TWAM(connectivity * c)
{
   synapse_params_TWAM * sp = (synapse_params_TWAM *)(c->Parameters);

   /*** Definition of the efficacies LUT. ***/
   if (makeGaussianLUT(&(c->JTab[0]), ANALOG_DEPTH, sp->J0, sp->J0*sp->DJ0, 0.0, 2.0*sp->J0) |
       makeGaussianLUT(&(c->JTab[1]), ANALOG_DEPTH, sp->J1, sp->J1*sp->DJ1, 0.0, 2.0*sp->J1))
      printFatalError("setSynapticEffaciesLUT_TWAM", "Out of memory.");

   return 0;
}


/*-------------------------*
 *  initSynapseState_TWAM  *
 *-------------------------*

/**
 *  Initializes the stable states (long-term synaptic efficacies)
 *  of the TWAM synapse addressed by the function parameters.
 */

void initSynapseState_TWAM(indexn        i, // post-synaptic neuron.
                           indexn        j, // pre-synaptic neuron.
                           void         *s, // pointer to the synapse.
                           connectivity *c, // pointer to the synaptic population.
                           int           l) // Layer corresponding to the transmission delay.
{
   synapse_TWAM * ss = s;                   // Synaptic state.
   synapse_params_TWAM * sp = (synapse_params_TWAM *)c->Parameters; // Synaptic parameters.

   ss->J0ndx = (int)(Random()*ANALOG_DEPTH);
   ss->J1ndx = (int)(Random()*ANALOG_DEPTH);
   if (Random() < sp->R0)
      ss->VJ = (float)sp->RBup;
   else
      ss->VJ = (float)sp->RBdown;
}


/*---------------------------*
 *  updateSynapseState_TWAM  *
 *---------------------------*

/**
 *  Return the synaptic efficacy of the TWAM synapse after 
 *  the internal variables are updated, depending on the 
 *  pre- and post-synaptic neuronal activity.
 */

real updateSynapseState_TWAM(indexn        i, // post-synaptic neuron.
                             void         *s, // pointer to the synapse.
                             connectivity *c, // pointer to the synaptic population.
                             spike       *sp) // The spike to transmit.
{
   static int JflagBefore, /* Long term efficacy state before and ... */ 
               JflagAfter; /* after the update of the synapse.        */
   static real     DeltaT; /* Time from the last spikes received (ISI). */
   static timex        tp; /* Time when a reflecting barrier is crossed. */
   static timex         t; /* Emission time of the spike to manage. */

   synapse_TWAM * ss = s;                    // Synaptic state.
   synapse_params_TWAM * spar = (synapse_params_TWAM * )c->Parameters; // Synaptic parameters.

   /*** Initialize local variables. ***/
   JflagBefore = ss->VJ > spar->ThetaJ;
   t = sp->Emission;
// TEMP: if the synaptic failure is implemented the following statement is 
//       misleading and incorrect.
   DeltaT = sp->ISI;

   /*** Constant decay or increase of VJ and check if reflecting barriers are crossed. ***/
   if (JflagBefore) {
      ss->VJ += (float)(spar->BetaJ * DeltaT);
      if (ss->VJ > spar->RBup) {
         /*** TEMP: Some output... It should be managed using the event queue. ***/
         if (SynStateResults) {
            tp = t;
            tp.Millis -= (ss->VJ - spar->RBup) / spar->BetaJ;
            outSynapticState(i, sp->Neuron, tp, 3, c->JTab[1][ss->J1ndx], 1, spar->RBup);
         }
         ss->VJ = (float)spar->RBup;
      }
   } else {
      ss->VJ -= (float)(spar->AlphaJ * DeltaT);
      if (ss->VJ < spar->RBdown) {
         /*** TEMP: Some output... It should be managed using the event queue. ***/
         if (SynStateResults) {
            tp = t;
            tp.Millis -= (spar->RBdown - ss->VJ) / spar->AlphaJ;
            outSynapticState(i, sp->Neuron, tp, 3, c->JTab[0][ss->J0ndx], 0, spar->RBdown);
         }
         ss->VJ = (float)spar->RBdown;
      }
   }

   /*** TEMP: Some output... It should be managed using the event queue. ***/
   if (SynStateResults)
      if (ss->VJ > spar->ThetaJ)
         outSynapticState(i, sp->Neuron, tp, 3, c->JTab[1][ss->J1ndx], 1, ss->VJ);
      else
         outSynapticState(i, sp->Neuron, tp, 3, c->JTab[0][ss->J0ndx], 0, ss->VJ);

   /*** VJ jump related to the time distance between pre- and post-synaptic spikes. ***/
   if (diffTimex(t, Neurons[i].Te) < spar->PotWindow) {
      ss->VJ += (float)spar->JumpUp;
      if (ss->VJ > spar->RBup) ss->VJ = (float)spar->RBup;
   } else {
      ss->VJ -= (float)spar->JumpDown;
      if (ss->VJ < spar->RBdown) ss->VJ = (float)spar->RBdown;
   }

   /*** TEMP: Some output... It should be managed using the event queue. ***/
   if (SynStateResults)
      if (ss->VJ > spar->ThetaJ)
         outSynapticState(i, sp->Neuron, tp, 3, c->JTab[1][ss->J1ndx], 1, ss->VJ);
      else
         outSynapticState(i, sp->Neuron, tp, 3, c->JTab[0][ss->J0ndx], 0, ss->VJ);

   /*** What follows is needed to monitor the transition number. ***/
   JflagAfter  = ss->VJ > spar->ThetaJ;

   /*** TEMP: Some output... It should be managed using the event queue. ***/
   if(SynTransResults) updateSynTrans(JflagBefore,JflagAfter);
   if(detailSynTransResults) detailSynTrans(i, sp->Neuron, JflagBefore, JflagAfter, t);

   /*** TEMP: STD, short term depression. ***/
//   if (TauRec > 0.0) {                      /* Flag a run-time. */
//      S->r = (float)(1.0 + ((1.0 - Use) * S->r - 1.0)*exp(-DeltaT/TauRec));
//
//      if (JflagAfter)
//         return J1Tab[(*S).J.EE.J1]*Use*S->r;
//      else
//         return J0Tab[(*S).J.EE.J0]*Use*S->r;
//   }

   /*** Return the actual long term synaptic efficacy. ***/
   if (JflagAfter)
      return c->JTab[1][ss->J1ndx];

   return c->JTab[0][ss->J0ndx];
}


/*------------------------*
 *  getSynapseState_TWAM  *
 *------------------------*

/**
 *  Returns, filling the synapse_state field <ss>, the state of
 *  the TWAM synapse <s> having as pre- and post-synaptic neurons 
 *  <j> and <i> respectively. The synapse belong to the connectivity
 *  block address by <c> and transmits spikes with delay index <l>.
 *  StateVars composition: {Syn. efficacy, 0-Depressed/1-Potentiated, 
 *  Hidden synaptic potential VJ}.
 */

void getSynapseState_TWAM(indexn          i, // post-synaptic neuron.
                          indexn          j, // pre-synaptic neuron.
                          void           *s, // pointer to the synapse.
                          connectivity   *c, // pointer to the synaptic population.
                          int             l, // Layer corresponding to the transmission delay.
                          timex           t, // Time to which compute the synaptic state.
                          synapse_state *ss) // Synaptic state to return.
{
   static real DeltaT; /* Time from the last spikes received (ISI). */
   static timex    tt; /* Time as timex. */

   synapse_TWAM         *ls = s;                                     // Synaptic state.
   synapse_params_TWAM *lsp = (synapse_params_TWAM * )c->Parameters; // Synaptic parameters.

   ss->NumStateVars = 3;
   ss->StateVars[1] = ls->VJ > lsp->ThetaJ ? 1.0 : 0.0;

// TEMP: if the synaptic failure is implemented the following statement is 
//       misleading and incorrect.
   DeltaT = diffTimex(t, Neurons[j].Te) - SynapticMatrix[l].Delay;

   /*** Constant decay or increase of VJ and check if reflecting barriers are crossed. ***/
   if (ls->VJ > lsp->ThetaJ) {
      ss->StateVars[2] = ls->VJ + lsp->BetaJ * DeltaT;
      if (ss->StateVars[2] > lsp->RBup) 
         ss->StateVars[2] = lsp->RBup;
      ss->StateVars[0] = c->JTab[1][ls->J1ndx];
   } else {
      ss->StateVars[2] = ls->VJ - lsp->AlphaJ * DeltaT;
      if (ss->StateVars[2] < lsp->RBdown)
         ss->StateVars[2] = lsp->RBdown;
      ss->StateVars[0] = c->JTab[0][ls->J0ndx];
   }
}



#undef BUFFER_SIZE