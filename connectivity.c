/*
 *
 *   connectivity.c
 *
 *   Library of structures and functions to manage 
 *   the connectivity of the network: the populations
 *   of synapses at mean-field level.
 *
 *   Project    PERSEO 2.0
 *
 */



#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <limits.h>
#include <math.h>
#include <string.h>

#include "invar.h"
#include "nalib.h"
#include "randdev.h"

#include "types.h"
#include "perseo.h"
#include "modules.h"
#include "connectivity.h"
#include "delays.h"
#include "synapses.h"



/*---------------------*
 *  LOCAL DEFINITIONS  *
 *---------------------*/

#define STRING_SIZE         256 /* Max length of local strings. */
#define BASIC_REAL_PARAMETERS 5 /* Number of real parameters fixed per population definition. */
/* #define BUFFER_SIZE          16 /* Size of the buffer for variable arrays. */ 
#define BUFFER_SIZE       32768 /* 2^15 Size of the buffer for variable arrays. */ 
#define MAX_DISTANCE        255 /* Max distance between two consecutive post-synaptic neurons *
                                 * for compression purpose.                                   */


/*----------------------*
 *   GLOBAL VARIABLES   *
 *----------------------*/

synaptic_layer  *SynapticMatrix = NULL; /* Synaptic matrix decomposed in layers. */

connectivity    ***Connectivity = NULL; /* Matrix of the synaptic populations defining the network architecture 
                                           (matrix of pointers to connectivity structure). */
connectivity *ConnectivityArray = NULL; /* Each non NULL entry is the structure of an existing synaptic populations. */
int        NumConnectivityArray = 0;    /* Number of connectivity structures loaded in ConnectivityArray. */
real        *ConnectivityParams = NULL; /* A whole array of the variable parameters for the Connectivity. */
int       NumConnectivityParams = 0;    /* Number of reals loaded in ConnectivityParams. */

char      *ConnectivityFileName = EMPTY_STRING; /* File name containing the definition of the connectivity ("connectivity.ini"). */
char    *SynapticExtractionType = EMPTY_STRING; /* Type of synaptic random extraction 'RANDOM', 'FIXEDNUM', ... */



/*-----------------------*
 *  FUNCTION DEFINITION  *
 *-----------------------*/


/** 
 *  Returns the number of absent synapses between to consecutive
 *  synapses on the axon. 
 */

indexn (*getEmptySynapses)(int postPop, int prePop);



/*------------------------------*
 *  loadConnectivityDefinition  *
 *------------------------------*/

/**
 *  Adds an element to the <Connectivity> matrix, and fills the fields of
 *  the structure <connectivity>.
 */

int loadConnectivityDefinition(int NumRealParams, IVreal *RealParams, 
                               int NumStringParams, char **StringParams)
{
   static int ConnectivityParamsSize = 0;
   static int  ConnectivityArraySize = 0;
   real         *newCP;
   connectivity *newCA;
   int     SynapseType = -1;

   int i, pre, post;

   if (Connectivity == NULL) {

      /*** Allocate memory for the connectivity matrix. ***/
      Connectivity = (connectivity ***)getMemory(sizeof(connectivity**) * NumPopulations, 
                               "ERROR (loadConnectivityDefinition): Out of memory.");
      for (post=0; post<NumPopulations; post++)
         Connectivity[post] = (connectivity **)getMemory(sizeof(connectivity*) * NumPopulations, 
                                        "ERROR (loadConnectivityDefinition): Out of memory.");

      /*** Initialize the connectivity matrix. ***/
      for (post=0; post<NumPopulations; post++)
         for (pre=0; pre<NumPopulations; pre++) 
            Connectivity[post][pre] = NULL;
   }

   /*** Synapse parameters parsing. ***/
   if (NumStringParams >= 1) {
      SynapseType = getSynapseType(StringParams[0]);
      if (SynapseType >= 0)
      {
         if (parseSynapseParams(SynapseType, NumRealParams - BASIC_REAL_PARAMETERS, &RealParams[BASIC_REAL_PARAMETERS])) {
            printError("loadConnectivityDefinition", "Uncorrect synapse parameters.\n");
            return 1;
         }
      } else {
         printError("loadConnectivityDefinition", "Unknown synapse type.\n");
         return 1;
      }
   } else {
      printError("loadConnectivityDefinition", "Synapse type unspecified.\n");
      return 1;
   }

   /*** Allocate memory for synapse parameters. ***/
   NumConnectivityParams += NumRealParams - BASIC_REAL_PARAMETERS;
   if (NumConnectivityParams > ConnectivityParamsSize) { // More memory is needed?
      ConnectivityParamsSize += BUFFER_SIZE;
      if (ConnectivityParamsSize == BUFFER_SIZE)
          ConnectivityParams = (real *)getMemory(sizeof(real) * ConnectivityParamsSize, "ERROR (loadConnectivityDefinition): Out of memory.");
      else {
         newCP = (real *)realloc(ConnectivityParams, sizeof(real) * ConnectivityParamsSize);
         for (i=0; i<NumConnectivityArray; i++)
            ConnectivityArray[i].Parameters = (ConnectivityArray[i].Parameters - ConnectivityParams) + newCP;
         ConnectivityParams = newCP;
         MemoryAmount += sizeof(real) * BUFFER_SIZE;
         if (ConnectivityParams == NULL)
            printFatalError("loadConnectivityDefinition", "Out of memory.");
      }
   }

   /*** Allocate memory for the connectivity structure. ***/
   NumConnectivityArray++; 
   if (NumConnectivityArray > ConnectivityArraySize) { // More memory is needed?
      ConnectivityArraySize += BUFFER_SIZE;
      if (ConnectivityParamsSize == BUFFER_SIZE)
         ConnectivityArray = (connectivity *)getMemory(sizeof(connectivity) * ConnectivityArraySize, "ERROR (loadConnectivityDefinition): Out of memory.");
      else {
         newCA = (connectivity *)realloc(ConnectivityArray, sizeof(connectivity) * ConnectivityArraySize);
         for (post=0; post<NumPopulations; post++)
            for (pre=0; pre<NumPopulations; pre++) 
               if (Connectivity[post][pre] != NULL)
                  Connectivity[post][pre] = (Connectivity[post][pre] - ConnectivityArray) + newCA;
         ConnectivityArray = newCA;
         MemoryAmount += sizeof(connectivity) * BUFFER_SIZE;
         if (ConnectivityArray == NULL)
            printFatalError("loadConnectivityDefinition", "Out of memory.");
      }
   }

   /*** Defines the connectivity element. ***/
   post = (int) (RealParams[0]);
   pre = (int) (RealParams[1]);
   if (post < 0 || post >= NumPopulations ||
      pre < 0 || pre >= NumPopulations) {
      printError("loadConnectivityDefinition", "Element of the connectivity matrix out of range.\n");
      return 1;
   }
   Connectivity[post][pre] = &(ConnectivityArray[NumConnectivityArray-1]);
   Connectivity[post][pre]->CProb = (real)RealParams[2];
   Connectivity[post][pre]->DMin = (real)RealParams[3];
   Connectivity[post][pre]->DMax = (real)RealParams[4];
   Connectivity[post][pre]->SynapseType = SynapseType;
   Connectivity[post][pre]->Parameters = &(ConnectivityParams[NumConnectivityParams - NumRealParams + BASIC_REAL_PARAMETERS]);
   Connectivity[post][pre]->ID = NumConnectivityArray - 1;
   for (i=0; i < NumRealParams - BASIC_REAL_PARAMETERS; i++)
      Connectivity[post][pre]->Parameters[i] = (real) RealParams[BASIC_REAL_PARAMETERS + i];
   Connectivity[post][pre]->NumParameters = NumRealParams - BASIC_REAL_PARAMETERS;

   /*** Sets synapses fields like pointer to functions and the size of the synapse. ***/
   setConnectivitySynapseFields(Connectivity[post][pre]);

   /*** Look up tables for synaptic efficacies. ***/
   Connectivity[post][pre]->JTab = NULL;
   if (setSynapticEffaciesLUT(Connectivity[post][pre]))
      printFatalError("loadConnectivityDefinition", "Out of memory.");
   
   return 0;
}


/*------------------------*
 *  getEmptySynapses_RAN  *
 *------------------------*/

/**
 *  Return the number plus one of non-existent (empty) synapses
 *  between two consecutive post-synaptic neurons, following 
 *  a Bernoulli distribution with probability p.
 */

indexn getEmptySynapses_RAN  (int postPop, int prePop)
{
   static indexn n;
   static real  r, P, C;

   /*** Frees the memory allocated for local structures, it's a needed fake block of code. ***/
   if (postPop < 0 && prePop < 0 )
      return 0;

   if (Connectivity[postPop][prePop]->CProb <= 0.0)
       n = INT_MAX;
   else {
      r = Random();
      P = Connectivity[postPop][prePop]->CProb;
      C = P;
      n = 1;
      while (C < r) {
         n++;
         P *= 1 - Connectivity[postPop][prePop]->CProb;
         C += P;
      }
      if (n<=0) {
         n = INT_MAX;
      }
   }

   return n;
}


/*------------------------*
 *  getEmptySynapses_FIX  *
 *------------------------*/

/**
 *  Return the number plus one of non-existent (empty) synapses
 *  between two consecutive post-synaptic neurons, in order to
 *  provide a fixed number of synapses given by the product of
 *  the connectivity and the number of neurons in the pre-
 *  synaptic population.
 *  NOTE: if the parameters are both negatives, the local
 *  memory structure are freed.
 */

indexn getEmptySynapses_FIX (int postPop, int prePop)
{
   static indexn n, m, s, k, SynNum;
   static int lastPost, lastPre, npre;
   static indexn MaxDendSize = 0;
   static int p, q;                  /* Cursor to scan populations. */
   static double r, dx, offset;
   static int prePopMask = -1;
   static int postPopMask = -1;
   static indexn **SynMatMask = NULL; /* A mask of pre-synaptic addresses. Zero element have to 
                                       * be negelcted. Note that for computational convenience
                                       * the rows refer to presynaptic elements and the 
                                       * columns the postsynaptic neurons. */
   static double *SynExtraction = NULL; /* Support array in which are collected the poissonian
                                         * extractions which set the post-synaptic neurons with
                                         * a synaptic contact. */
   static indexn *PopOffset = NULL; /* The index of the first neuron in a module. */
   static indexn *PreSynCursor = NULL; /* The index of the last element visited in a given column
                                        * of the SynMatMask. */
   static unsigned int SupportMemoryAmount = 0; /* The amount of memory used in local  
                                                 * structures located in the heap. */

   /***                                                               ***/
   /*** Frees the memory allocated for local structures, is required. ***/
   /***                                                               ***/
   if (postPop < 0 && prePop < 0 )
   {
      if (SynMatMask != NULL)
      {
         free(PopOffset);
         free(SynExtraction);
         free(PreSynCursor);
         for (n=0; n<MaxDendSize; n++)
            free(SynMatMask[n]);
         free(SynMatMask);

         /*** Prints the status of the SynapticMatrix creation. ***/
         MemoryAmount -= SupportMemoryAmount;
#ifdef PRINT_STATUS
         fprintf(stderr, "Initializing Synaptic Matrix... 100.0%% (Memory: %g Mbytes)    \r", (real)MemoryAmount/1024.0/1024.0);
#endif
      }

      return 0;
   }


   /***                                ***/
   /*** Has the mask to be recomputed? ***/
   /***                                ***/
   if (prePop != prePopMask)
   {
      prePopMask = prePop;

      /*** Makes room, if needed for the synaptic matrix mask. ***/
      if (SynMatMask == NULL)
      {
         SupportMemoryAmount = MemoryAmount;

         /*** Allocates and set the offset neurons of the populations. ***/
#ifdef PRINT_DEBUG
         fprintf(stderr, "\na. Allocates memory for offset neurons of the population...");
#endif
         PopOffset = (indexn *)getMemory(sizeof(indexn) * NumPopulations, 
                               "ERROR (getEmptySynapses_FIX): Out of memory (5).\n");

         /*** Finds the size of the largest dendritic tree branch connecting ***
          *** one population to a post-synaptic neuron.                      ***/
         for (p=0; p<NumPopulations; p++)
         {
            PopOffset[p] = p == 0 ? 0 : PopOffset[p-1] + Populations[p-1].N;
            for (q=0; q<NumPopulations; q++)
               if (Connectivity[p][q] != NULL)
               {
                  SynNum = roundr2i(Populations[q].N * Connectivity[p][q]->CProb);
                  if (MaxDendSize < SynNum)
                     MaxDendSize = SynNum;
               }
         }

         /*** Allocates memory for the synaptic matrix mask. ***/
#ifdef PRINT_DEBUG
         fprintf(stderr, "\nb. Allocates memory for the synaptic matrix mask...");
#endif

         SynMatMask = (indexn **)getMemory(sizeof(indexn *) * MaxDendSize, 
                                "ERROR (getEmptySynapses_FIX): Out of memory (1).\n");
         for (n=0; n<MaxDendSize; n++)
            SynMatMask[n] = (indexn *)getMemory(sizeof(indexn) * NumNeurons, 
                                      "ERROR (getEmptySynapses_FIX): Out of memory (2).\n");

         /*** Allocates memory for the synaptic extraction support array. ***/
         SynExtraction = (double *)getMemory(sizeof(double) * (MaxDendSize + 1), 
                                "ERROR (getEmptySynapses_FIX): Out of memory (3).\n");

         /*** Allocates and cursor for pre-synaptic neurons. ***/
         PreSynCursor = (indexn *)getMemory(sizeof(indexn) * NumNeurons, 
                               "ERROR (getEmptySynapses_FIX): Out of memory (4).\n");

         SupportMemoryAmount = MemoryAmount - SupportMemoryAmount;

#ifdef PRINT_STATUS
         fprintf(stderr, "\nSupporting memory for fixed connectivity... %g Mbytes\n", (real)SupportMemoryAmount/1024.0/1024.0);
#endif
      }

      /***                                                                     ***/
      /*** Initializes with all zeros the synaptic matrix mask and the cursor. ***/
      /***                                                                     ***/
      for (n=0; n<MaxDendSize; n++)
         memset(SynMatMask[n], 0xFF, sizeof(indexn) * NumNeurons);
      memset(PreSynCursor, 0, sizeof(indexn) * NumNeurons);

      /***                                     ***/
      /*** Populates the synaptic matrix mask. ***/
      /***                                     ***/
#ifdef PRINT_DEBUG
         fprintf(stderr, "\nc. Populates the synaptic matrix mask...");
#endif
      for (n=0, p=0; p<NumPopulations; p++)
         if (Connectivity[p][prePop] == NULL)
            n += Populations[p].N;
         else
         {
            SynNum = roundr2i(Populations[prePop].N * Connectivity[p][prePop]->CProb);

            /*** Is the subnetwork fully connected? ***/
            if (SynNum == Populations[prePop].N)
            {
               for (m=0; m<Populations[p].N; m++, n++)
               {
                  for (s=0; s<SynNum; s++)
                     SynMatMask[s][n] = s;
               }

            /*** ... sparse connectivity. ***/
            } else if (SynNum > 0) {            // It can be 0 even if Connectivity is not NULL
               for (m=0; m<Populations[p].N; m++, n++)
               {
                  for (s=0, r=0.0; s<SynNum; s++)
                  {
                     r += ExpDev();
                     SynExtraction[s] = r;
                  }
                  dx = SynExtraction[SynNum-1] / (Populations[prePop].N - SynNum);
                  offset = -Random() * (SynExtraction[SynNum-1] + SynNum*dx);
                  for (s=0, k=0; s<SynNum; s++)
                  {
                     npre = (int)floor(s+(offset+SynExtraction[s])/dx);
                     if (npre >= 0)
                        SynMatMask[k++][n] = npre;
                  }
                  for (s=0; k<SynNum; k++, s++)
                  {
                     npre = Populations[prePop].N + (int)floor(s+(offset+SynExtraction[s])/dx);
                     SynMatMask[k][n] = npre;
                  }
               }
            }
         }

      lastPre = 0;
   } else
      if (lastPost == NumNeurons || postPop < postPopMask)
      {
         lastPre++;
         lastPost = -1;
      }

   /*** Has the post-synaptic population changed? ***/
   if (postPop != postPopMask)
   {
      postPopMask = postPop;
      lastPost = PopOffset[postPop] - 1;
   }

   /*** Return the empty synapses plus one read from the synaptic matrix mask. ***/
   p = lastPost;
   for (lastPost++; (indexn)lastPost<PopOffset[postPop]+Populations[postPop].N; lastPost++)
      if (PreSynCursor[lastPost]<MaxDendSize)
         if (SynMatMask[PreSynCursor[lastPost]][lastPost] == lastPre) 
         {
            PreSynCursor[lastPost] = PreSynCursor[lastPost] + 1;
            break;
         }

   return (indexn)(lastPost - p);
}


/*------------------------*
 *  createSynapticMatrix  *
 *------------------------*/

/**
 *  Functions called after the connectivity definition. 
 *  It allocates the layered structure of the synaptic matrix.
 */

void createSynapticMatrix ()
{
   int           l;       /* Index for the layer array. */
   indexn        i, j, D; 
   int   prePop, postPop;
   int              Post; /* A cursor to identify the existent synapses. */
   connectivity       *c; /* A cursor for the connectivity matrix. */
   axon_segment *Support; /* Support structure hosting a maximally populated *
                           * axon.                                           */
   indexn *NumExceptions; /* An array of number of exceptions per layer. */
   indexn      *LastPost; /* Index array of the last post-synaptic neuron *
                           * extracted per layer.                         */
   int      *SynapseSize; /* Array of the size in byte of the memory segments   *
                           * that will host the synapses of the SynapticMatrix. */
   unsigned int SupportMemoryAmount; /* Local variable to compute the memory allocated. */

#ifdef PRINT_STATUS
   real    Status = 0.0; /* Processing status cursor. */
   real IncStatus = 0.1; /* Visible increment of the cursor. */
#endif

   /*** Allocates memory for the layers. ***/
#ifdef PRINT_DEBUG
   fprintf(stderr, "\na. Allocates memory for delay layers...");
#endif
   SynapticMatrix = (synaptic_layer *)getMemory(sizeof(synaptic_layer)*DelayNumber, "ERROR (createSynapticMatrix): Out of memory (1).");

   /*** Initializes the layers and allocates memory for ***
    *** pre-synaptic neuron axon segment.               ***/
#ifdef PRINT_DEBUG
   fprintf(stderr, "\nb. Allocates memory for pre-synaptic axon segments...");
#endif
   for (l=0; l<DelayNumber; l++) {
      initQueue(&(SynapticMatrix[l].Queue), sizeof(spike));
      SynapticMatrix[l].Empty = true;
      SynapticMatrix[l].Delay = DelayMin + DelayStep * l;
      SynapticMatrix[l].Pre = (axon_segment *)getMemory(sizeof(axon_segment)*NumNeurons, "ERROR (createSynapticMatrix): Out of memory (2).");
      for (j=0; j<NumNeurons; j++) {
         SynapticMatrix[l].Pre[j].DPost = NULL;
         SynapticMatrix[l].Pre[j].Exception = NULL;
         SynapticMatrix[l].Pre[j].Synapses = NULL;
         SynapticMatrix[l].Pre[j].NumSynapses = 0;
      }
   }

   /*** Allocates memory for the support structures. ***/
#ifdef PRINT_DEBUG
   fprintf(stderr, "\nc. Allocates memory for support structures...");
#endif
   SupportMemoryAmount = MemoryAmount;
   Support = (axon_segment *)getMemory(sizeof(axon_segment)*DelayNumber, "ERROR (createSynapticMatrix): Out of memory (3).\n");
   NumExceptions = (indexn *)getMemory(sizeof(indexn)*DelayNumber, "ERROR (createSynapticMatrix): Out of memory (3).\n");
   LastPost = (indexn *)getMemory(sizeof(indexn)*DelayNumber, "ERROR (createSynapticMatrix): Out of memory (3).\n");
   SynapseSize = (int *)getMemory(sizeof(int)*DelayNumber, "ERROR (createSynapticMatrix): Out of memory (3).\n");
   for (l=0; l<DelayNumber; l++) {
       Support[l].DPost = (byte *)getMemory(sizeof(byte)*NumNeurons, "ERROR (createSynapticMatrix): Out of memory (4).");
       Support[l].Exception = (indexn *)getMemory(sizeof(indexn)*NumNeurons, "ERROR (createSynapticMatrix): Out of memory (4).");
   }
   SupportMemoryAmount = MemoryAmount  - SupportMemoryAmount;
#ifdef PRINT_STATUS
   fprintf(stderr, "\nSupporting memory... %g Mbytes", (real)SupportMemoryAmount/1024.0/1024.0);
#endif


   /*** Loop on the presynapic neurons. ***/
#ifdef PRINT_STATUS
   fprintf(stderr, "\nInitializing Synaptic Matrix... \r");
//   fflush(stderr);
#endif

   for (j=0; j<NumNeurons; j++) {
      prePop = Neurons[j].Pop->ID;

      /*** Initializes the support structure. ***/
      for (l=0; l<DelayNumber; l++) {
         Support[l].NumSynapses = 0;
         NumExceptions[l] = 0;
         LastPost[l] = -1;
         SynapseSize[l] = 0;
      }

      /*** Loop on the existing connectivity elements population by population. ***/
      for (i=postPop=0; postPop<NumPopulations; i+=Populations[postPop].N, postPop++) {

         if (Connectivity[postPop][prePop] != NULL) {

            c = Connectivity[postPop][prePop];
            Post = i - 1;

            /*** Loop on the post-synaptic neurons connected to the pre-synaptic j. ***/
            while ((Post += (*getEmptySynapses)(postPop, prePop)) < (int)(i + Populations[postPop].N))
            {
               /*** May happens when empty synapses is too large, related to low connectivity... ***/
               if (Post < 0)
                  break;

               /*** No self-coupling is allowed. ***/
//              if (Post != j) {
                  l = (*getRandomDelay)(c);
                  D = Post - LastPost[l];
                  if (D > MAX_DISTANCE) {
                     D = EXCEPTION;
                     Support[l].Exception[NumExceptions[l]++] = Post;
                  }
                  Support[l].DPost[Support[l].NumSynapses++] = D;
                  SynapseSize[l] += c->SynapseSize;
                  LastPost[l] = Post;
//               }
            }
         }
      }

      /*** Copies the support structure to the SynapticMatrix. ***/
      for (l=0; l<DelayNumber; l++) {

         /*** Allocates memory for the j-th axon. ***/
         SynapticMatrix[l].Pre[j].DPost = (byte *)getMemory(sizeof(byte)*Support[l].NumSynapses, "ERROR (createSynapticMatrix): Out of memory (5).");
         SynapticMatrix[l].Pre[j].Exception = (indexn *)getMemory(sizeof(indexn)*NumExceptions[l], "ERROR (createSynapticMatrix): Out of memory (5).");
         SynapticMatrix[l].Pre[j].Synapses = getMemory(SynapseSize[l], "ERROR (createSynapticMatrix): Out of memory (5).");

         /*** Initializes fields and copies the contents. ***/
         SynapticMatrix[l].Pre[j].NumSynapses = Support[l].NumSynapses;
         memcpy(SynapticMatrix[l].Pre[j].DPost, Support[l].DPost, sizeof(byte)*Support[l].NumSynapses);
         memcpy(SynapticMatrix[l].Pre[j].Exception, Support[l].Exception, sizeof(indexn)*NumExceptions[l]);
      }

      /*** Prints the status of the SynapticMatrix creation. ***/
#ifdef PRINT_STATUS
     if ((100.0*j)/NumNeurons > Status + IncStatus) {
        Status = (int)((100.0*j)/NumNeurons / IncStatus) * IncStatus;
        fprintf(stderr, "Initializing Synaptic Matrix... %5.1f%% (Memory: %g Mbytes)    \r", Status, (real)MemoryAmount/1024.0/1024.0);
     }
#endif

      /*** Did an interrupt signal occur? ***/
      if (QuitSimulation) break;
   }

   /*** Frees the memory occupied by the support structures. ***/
   (*getEmptySynapses)(-1, -1);
   for (l=0; l<DelayNumber; l++) {
      free(Support[l].DPost);
      free(Support[l].Exception);
   }
   free(Support);
   free(NumExceptions);
   free(LastPost);
   free(SynapseSize);
   MemoryAmount -= SupportMemoryAmount;

   /*** Prints the status of the SynapticMatrix creation. ***/
#ifdef PRINT_STATUS
        fprintf(stderr, "Initializing Synaptic Matrix... 100.0%% (Memory: %g Mbytes)    \r", (real)MemoryAmount/1024.0/1024.0);
#endif
}


/*----------------------*
 *  scanSynapticMatrix  *
 *----------------------*/

/**
 *  Visits a portion of the synaptic matrix defined by an 
 *  interval of post-synaptic neurons [PostStart,PostEnd] 
 *  and an interval of pre-synaptic neurons [PreStart,PreEnd].
 *  For each synapse met a user-defined hook function 
 *  (InspectFunc) is called. The synapse are visited in 
 *  ascending order of pre-synaptic neuron, transmission 
 *  delay and post-synaptic neuron.
 */

void scanSynapticMatrix (indexn           PostStart, 
                         indexn             PostEnd, 
                         indexn            PreStart, 
                         indexn              PreEnd,
                         InspectFuncPtr InspectFunc)
{
   int        l, n;
   indexn  i, j, k;
   byte         *s;
   connectivity *c;

   /*** Scans the presynaptic neurons. ***/
   for (j=PreStart; j<=PreEnd; j++)

      /*** Scans the layer of transmission delay. ***/
      for (l=0; l<DelayNumber; l++) {
         i = -1; 
         s = (byte *)(SynapticMatrix[l].Pre[j].Synapses);
         n = 0;

         /*** Scans the axon segment. ***/
         for (k=0; k<SynapticMatrix[l].Pre[j].NumSynapses; k++) {
            if (SynapticMatrix[l].Pre[j].DPost[k] != EXCEPTION)
               i += SynapticMatrix[l].Pre[j].DPost[k];
            else
               i = SynapticMatrix[l].Pre[j].Exception[n++];

            c = Connectivity[Neurons[i].Pop->ID][Neurons[j].Pop->ID];

            if (i >= PostStart)
               if (i <= PostEnd)
                  (*InspectFunc)(i, j, (void *)s, c, l);
               else
                  break;

            s += c->SynapseSize;
         }
      }
}



/*------------------------*
 *  setConnectivityParam  *
 *------------------------*/

/**
 *  Updates online the global parameter of a synaptic population,
 *  like the JumpUp or JumpDown of the hidden synaptic variable.
 *
 *  WARNING: Only parameters n. 13 and 14 are managed (JumpUp and 
 *  JumpDown) or the ones from 5 to 8 (the synaptic efficacies and
 *  their relative standard deviations).
 */

void setConnectivityParam(int          Post, /* Post-synaptic population. */
                          int           Pre, /* Post-synaptic population. */
                          int      ParamNum, /* Number of parameter to change. */
                          double ParamValue, /* New value to assign to the parameter. */
                          double       Time) /* Time when the update occur. */
{
   static connectivity *c;

   if (Post >= 0 && Post < NumPopulations &&
       Pre >= 0 && Pre < NumPopulations)

 /* TEMP: Such a solution is not the final one. In general the synaptic module will be *
  *       the one capable to update the synaptic parameter whose meaning depends on    *
  *       the synapse type.                                                            */
      if ((c = Connectivity[Post][Pre]) != NULL)
         if (c->NumParameters > ParamNum-BASIC_REAL_PARAMETERS)
            if (ParamNum == 13 || ParamNum == 14) // JumpUp or JumpDown
            { 
               c->Parameters[ParamNum-BASIC_REAL_PARAMETERS] = ParamValue;
            } else {
                if (ParamNum >= 5 && ParamNum <= 8) // J* or DJ*
                {
                    c->Parameters[ParamNum-BASIC_REAL_PARAMETERS] = ParamValue;
                    setSynapticEffaciesLUT(c);
                }
            }
}


/*-----------------------------*
 *  setSynapticExtractionType  *
 *-----------------------------*/

/**
 *  Sets the function pointers dependent on the
 *  type of synaptic extraction choosen.
 */

int setSynapticExtractionType()
{
   if (strcmp(strupr(SynapticExtractionType), SET_RAN) == 0)
   {
      getEmptySynapses = &getEmptySynapses_RAN;
      return 0;
   }
   if (strcmp(strupr(SynapticExtractionType), SET_FIX) == 0)
   {
      getEmptySynapses = &getEmptySynapses_FIX;
      return 0;
   }
   if (strcmp(strupr(SynapticExtractionType), EMPTY_STRING) == 0)
   {
      getEmptySynapses = &getEmptySynapses_RAN;
      return 0;
   }
   
   return 1;
}


#undef MAX_DISTANCE
#undef BUFFER_SIZE
#undef BASIC_REAL_PARAMETERS
#undef STRING_SIZE
