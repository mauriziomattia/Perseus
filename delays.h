/*
 *
 *   delays.h
 *
 *   Library of functions to manage transmission
 *   delays distributions (UNIFORM, EXPONENTIAL, ...).
 *
 *   Project: PERSEO 2.0
 *
 */



#ifndef __DELAYS_H__
#define __DELAYS_H__



#include "types.h"
#include "connectivity.h"



/*** DelayDistribType ***/
#define DDT_UNI "UNIFORM"
#define DDT_EXP "EXPONENTIAL"



extern char *DelayDistribType; /* Delays distribution type 'UNIFORM', 'EXPONENTIAL', ... */
extern int        DelayNumber; /* Number of delays (layers) in synaptic matrix structure. */
extern real          DelayMax; /* Maximum of all delays (max(connectivity.DMax)). */
extern real          DelayMin; /* Minimum of all delays (min(connectivity.DMin)). */
extern real         DelayStep; /* (DelayMax - DelayMin)/(DelayNumber - 1). */

                       
/** 
 *  Returns the number of layer from a 
 *  given delay distribution. 
 */

extern int (*getRandomDelay)(connectivity * c);



/**
 *  Sets the function pointers dependent on the
 *  type of choosen transmission delay distribution.
 */

int setDelayDistributionType();



/**
 *  Sets bounds of the delays (DelayMax, DelayMin) and
 *  the sampling step of the distributions of delays 
 *  (DelayStep).
 */

void setDelayBounds();




/*----------------------------------------*
 *                                        *
 *   UNI (Uniform) delays distribution.   *
 *                                        *
 *----------------------------------------*/


/** 
 *  Returns the number of layer from a 
 *  UNIFORM delay distribution. 
 */

int getRandomDelay_UNI(connectivity * c);



/*--------------------------------------------*
 *                                            *
 *   EXP (Exponential) delays distribution.   *
 *                                            *
 *--------------------------------------------*/


/** 
 *  Returns the number of layer from a 
 *  EXPONENTIAL delay distribution. 
 */

int getRandomDelay_EXP(connectivity * c);



#endif /* __DELAYS_H__ */
