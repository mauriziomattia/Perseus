/*
 *
 *   commands.h
 *
 *     Library of commands recognised by the simulation,
 *   submitted as events to manage at a given time. Functions
 *   implements the commands and thei parsing from an input 
 *   file.
 *
 *   Project: Perseo 2.1.x
 *
 */



#ifndef __COMMANDS_H__
#define __COMMANDS_H__



/*----------------------*
 *  GLOBAL DEFINITIONS  *
 *----------------------*/

extern char *CommandsFileName; /* File name containing the commands to submit to the simulation ("protocol.ini"). */



/*--------------------*
 *  GLOBAL FUNCTIONS  *
 *--------------------*/

/**
 *  Reads and parses a command from lines, if available,
 *  in the command file, making correspondingly new events
 *  which will be processed at a given time of the simulation.
 *  If no command file is specified the stdin is assumed.
 *  If the pointer to the simulation time passed as parameter
 *  is NULL the static variables are initialized.
 *  The initialization have to occurre before any other call
 *  of the function.
 *  Note that the command file is not close once opened, in order
 *  to allow further command to be specified during the simulation.
 */

void readCommands (double *SimuTime); /* A ptr to the actual simulation time. */



#endif /* __COMMANDS_H__ */
