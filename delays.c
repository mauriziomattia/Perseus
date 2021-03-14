/*
 *
 *   delays.c
 *
 *   Library of functions to manage transmission
 *   delays distributions (UNIFORM, EXPONENTIAL, ...).
 *
 *   Project: PERSEO 2.0
 *
 */


#include <stdio.h>
#include <string.h>
#include <math.h>

#include "nalib.h"
#include "randdev.h"

#include "types.h"
#include "perseo.h"
#include "connectivity.h"
#include "delays.h"



#define STRING_SIZE 256



char *DelayDistribType = EMPTY_STRING; /* Delays distribution type 'UNIFORM', 'EXPONENTIAL', ... */
int        DelayNumber = 0;            /* Number of delays (layers) in synaptic matrix structure. */
real          DelayMax;                /* Maximum of all delays (max(connectivity.DMax)). */
real          DelayMin;                /* Minimum of all delays (min(connectivity.DMin)). */
real         DelayStep;                /* (DelayMax - DelayMin)/(DelayNumber - 1). */



/** 
 *  Returns the number of layer from a 
 *  given delay distribution. 
 */

int (*getRandomDelay)(connectivity * c);


/*----------------------------*
 *  setDelayDistributionType  *
 *----------------------------*/

/**
 *  Sets the function pointers dependent on the
 *  type of choosen transmission delay distribution.
 */

int setDelayDistributionType()
{
   if (strcmp(strupr(DelayDistribType), DDT_UNI) == 0)
   {
      getRandomDelay = &getRandomDelay_UNI;
      return 0;
   }
   if (strcmp(strupr(DelayDistribType), DDT_EXP) == 0)
   {
      getRandomDelay = &getRandomDelay_EXP;
      return 0;
   }

   return 1;
}


/*------------------*
 *  setDelayBounds  *
 *------------------*/

/**
 *  Sets bounds of the delays (DelayMax, DelayMin) and
 *  the sampling step of the distributions of delays 
 *  (DelayStep).
 */

void setDelayBounds()
{
   int i;

   DelayMax = 0;
   DelayMin = 1e36;

   for (i=0; i<NumConnectivityArray; i++)
   {
      if (ConnectivityArray[i].DMax > DelayMax) DelayMax = ConnectivityArray[i].DMax;
      if (ConnectivityArray[i].DMin < DelayMin) DelayMin = ConnectivityArray[i].DMin;
   }

//   DelayStep = (DelayMax - DelayMin)/(DelayNumber - 1);
   DelayStep = (DelayMax - DelayMin)/DelayNumber;
   DelayMax -= DelayStep/2;
   DelayMin += DelayStep/2;

   /*** Discretize the distribution delay range. ***/
   for (i=0; i<NumConnectivityArray; i++)
   {
      ConnectivityArray[i].DMax = roundr2i((ConnectivityArray[i].DMax - DelayMin)/DelayStep)*DelayStep+DelayMin;
      if (ConnectivityArray[i].DMax > DelayMax) 
         ConnectivityArray[i].DMax = DelayMax;
      else if (ConnectivityArray[i].DMax < DelayMin) 
              ConnectivityArray[i].DMax = DelayMin;

      ConnectivityArray[i].DMin = roundr2i((ConnectivityArray[i].DMin - DelayMin)/DelayStep)*DelayStep+DelayMin;
      if (ConnectivityArray[i].DMin > DelayMax) 
         ConnectivityArray[i].DMin = DelayMax;
      else if (ConnectivityArray[i].DMin < DelayMin) 
              ConnectivityArray[i].DMin = DelayMin;
   }
}



/*----------------------------------------*
 *                                        *
 *   UNI (Uniform) delays distribution.   *
 *                                        *
 *----------------------------------------*/


/** 
 *  Returns the number of layer from a 
 *  UNIFORM delay distribution. 
 */

int getRandomDelay_UNI(connectivity * c)
{
   return roundr2i((Random()*(c->DMax - c->DMin) + c->DMin - DelayMin) / DelayStep);
}



/*--------------------------------------------*
 *                                            *
 *   EXP (Exponential) delays distribution.   *
 *                                            *
 *--------------------------------------------*/


/** 
 *  Returns the number of layers from an 
 *  EXPONENTIAL delay distribution starting from
 *  DMin and cut at DMax in order to neglect a 
 *  portion TailNeglect to the exponential p.d.f.
 *  The mean transmission delay is given by
 *
 *   DMax TD - DMin     DMin - DMax
 *  ---------------- + -------------
 *       TD - 1           Log[TD]
 *
 *  where TD = TailNeglected.
 */

#define TailNeglected 0.05 /* p.d.f. tail of the exponential distribution to neglect. */

int getRandomDelay_EXP(connectivity * c)
{
   static real InvLogTN = 1.0;
   static real outfunc;

   if (InvLogTN > 0.0) InvLogTN = 1.0 / log(TailNeglected);

   outfunc = ((c->DMin + (c->DMax - c->DMin) * log(1.0 - Random() * (1.0 - TailNeglected)) * InvLogTN) - DelayMin) / DelayStep;

   return roundr2i(outfunc);
}

#undef TailNeglected



#undef STRING_SIZE