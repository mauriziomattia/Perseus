/*
 *
 *   stimuli.h
 *
 *   Library of structures and function to 
 *   handle composite stimulations of the 
 *   neurons in the network.
 *
 *   Project: PERSEO 2.0
 *
 */


#ifndef __STIMULI_H__
#define __STIMULI_H__



#include "types.h"
#include "events.h"


 
/**
 *  A stimulus can be associated only to a subset of a population.
 *  Many stimuli can be simultaneously active.
 */
typedef struct {
        /*** Fields needed for making the simulation efficient. ***/
        real        InvNu; /* 1 / (Contrast * NuExt). */
        timex    Emission; /* Emission time of the last spike emittedTempo di ricezione dell'ultimo impulso esterno. */
        timex  LastUpdate; /* Time of the last update of local variables (SpikeCounter). */
        int  SpikeCounter; /* Conta gli impulsi emessi dalla pop. in un periodo. */

        indexn          N; /* Number of neuron to stimulate. */
        indexn *   Neuron; /* Array of stimulated neuron index. */
}  stimulus;


extern stimulus *SingleStimuli; /* Array of stimuli involving neurons of the same population. */
extern int    NumSingleStimuli; /* Number of single stimuli (length of SingleStimuli). */

typedef struct {
   stimulus *StimSet; /* A set of single stimuli (points to SingleStimuli). */
   int    NumStimSet; /* Elements in StimSet, composing a stimulus (multiple or single). */
} multistimuli;

extern multistimuli *Stimuli; /* Array of single and/or multiple stimuli. */
extern int        NumStimuli; /* Number of stimuli (length of Stimuli). */

extern int    ActiveStimulus; /* Addresses an element of Stimuli representing the 
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

void createRandomStimuli(int Nstim, float CodLev);



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

void createCorruptedStimulus (int Stim, float CL);



/**
 *   TO DO...
 *
 *   Avvia la stimolazione dello stimolo 'Stim' con un'intensita'
 *   contrasto 'Contrast' al tempo 'TimeStim' della simulazione
 *   (PERSEO 1.0).
 */

int startStimulation(event *Event);



/**
 *   TO DO...
 *
 *   Riporta il numero di popolazioni attive a due: eccitatori,
 *   e inibitori (PERSEO 1.0).
 */

int stopStimulation (event *Event);



/**
 * isStimulated()
 *
 * Is 1 if at least one stimulus is active, 0 otherwise.
 */

#define isStimulated() (ActiveStimulus >= 0)



#endif /* __STIMULI_H__ */