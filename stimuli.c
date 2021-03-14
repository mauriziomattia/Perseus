/*
 *
 *   stimuli.c
 *
 *   Library of structures and function to 
 *   handle composite stimulations of the 
 *   neurons in the network.
 *
 *   Project: PERSEO 2.0
 *
 */



#include "randdev.h"
#include "types.h"
#include "events.h"
#include "stimuli.h"



stimulus *SingleStimuli;     /* Array of stimuli involving neurons of the same population. */
int    NumSingleStimuli = 0; /* Number of single stimuli (length of SingleStimuli). */

multistimuli *Stimuli;     /* Array of single and/or multiple stimuli. */
int        NumStimuli = 0; /* Number of stimuli (length of Stimuli). */

int    ActiveStimulus = 0; /* Addresses an element of Stimuli representing the 
                            * active stimulus, if any. Is negative if no stimuli 
                            * are active.                                        */

/**
 *   TO DO...
 *
 *   Questa funzione crea 'Nstim' stimoli casuali con lo
 *   stesso livello di codifica (CodLev): un neurone ha
 *   probabilita' CodLev di essere stimolato da un pattern
 *   cui appartiene (PERSEO 1.0).
 */

void createRandomStimuli(int Nstim, float CodLev)
{
}



/**
 *  TO DO...
 *
 *  Crea una popolazione di neuroni da stimolare a partire dai
 *  neuroni che vengono stimolati dallo stimolo Stim: se un neurone 
 *  appartiene allo stimolo Stim ha probabilita' 1-CL/2 di 
 *  appartenere al pattern corrotto, se non vi appartiene ha 
 *  probabilita' 1-CL/2 di non appartenere al pattern corrotto. 
 *  CL e' il livello di corruzione compreso tra 0 e 1 (CorruptionLevel): 
 *  0 = assenza di corruzione, 1 completo rimescolamento del pattern
 *  (PERSEO 1.0).
 */

void createCorruptedStimulus (int Stim, float CL)
{
}



/**
 *   TO DO...
 *
 *   Avvia la stimolazione dello stimolo 'Stim' con un'intensita'
 *   contrasto 'Contrast' al tempo 'TimeStim' della simulazione
 *   (PERSEO 1.0).
 */

int startStimulation(event *Event)
{
   return 1;
}



/**
 *   TO DO...
 *
 *   Riporta il numero di popolazioni attive a due: eccitatori,
 *   e inibitori (PERSEO 1.0).
 */

int stopStimulation (event *Event)
{
   return 1;
}
