/*
 *
 *   init.h
 *
 *   Library of functions to read definition files to 
 *   initialize the data structures needed to the simulation
 *
 *   Project: PERSEO 2.x
 *
 */



#ifndef __INIT_H__
#define __INIT_H__



/**
 *   Crea e inizializza le strutture che ospitano la rete
 *   e stampa evenutali errori, in conseguenza dei quali
 *   viene terminato il programma.
 */

void initNeuralNetwork (void);



/**
 *   Inizializzazione di parametri ridondanti della rete a
 *   partire dai valori di default e da quelli specificati
 *   dall'utente.
 *   Le frequenze sono espresse in spikes/s, mentre i tempi
 *   in ms.
 */

void initParameters (int ArgC, char * ArgV[]);



#endif /* __INIT_H__ */