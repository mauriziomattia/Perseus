/*
 *
 *   commands.c
 *
 *     Library of commands recognised by the simulation,
 *   submitted as events to manage at a given time. Functions
 *   implements the commands and thei parsing from an input 
 *   file.
 *
 *   Project: Perseo 2.1.x
 *
 */



#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "types.h"
#include "events.h"
#include "stimuli.h"
#include "perseo.h"
#include "results.h"
#include "modules.h"
#include "connectivity.h"



/*----------------------*
 *  GLOBAL DEFINITIONS  *
 *----------------------*/

char   *CommandsFileName = EMPTY_STRING; /* File name containing the commands to submit to the simulation ("protocol.ini"). */



/*---------------------*
 *  LOCAL DEFINITIONS  *
 *---------------------*/

#define PROCESSED_COMMAND     1
#define PARSING_ERROR         0
#define UNPROCESSED_COMMAND  -1

#define BUFFER_DIM          256

#define SAMPLING_PERIOD       1.0  /* Period between two consecutive command reading in ms. */



/*-------------------*
 *  LOCAL FUNCTIONS  *
 *-------------------*/

//
// TO CUSTOMIZE (1)...
//

/*------------------*
 *  RANDOM_STIMULI  *
 *------------------*/

/**
 *  Random stimuli generation:
 *
 *    RANDOM_STIMULI <iN> <fCL>
 *
 *  where iN is the number of random stimuli to build,
 *  and fCL is the coding level (the probability to have
 *  a neuron stimulate by the single stimulus).
 *
 *  WARNING: NOT IMPLEMENTED...
 */

int processRandomStimuliCommand(char *InputLine, 
                                double *pTime)
{
   float      f[3];
   char Command[BUFFER_DIM];
   int ParamNum;

   ParamNum = sscanf(InputLine, "%s %f %f %f", Command, &(f[0]), &(f[1]), &(f[2]));

   if (strcmp(strupr(Command), "RANDOM_STIMULI") == 0)
      if (ParamNum == 3)
         createRandomStimuli((int)f[0], f[1]);
      else {
         printError("processRandomStimuliCommand", "parameters of 'RANDOM_STIMULI' differ from 2.\n");
         return PARSING_ERROR;
      }
   else
      return UNPROCESSED_COMMAND;

   return PROCESSED_COMMAND;
}



/*----------------------*
 *  CORRUPTED_STIMULUS  *
 *----------------------*/

/**
 *  Corrupted stimuli generation:
 *
 *     CORRUPTED_STIMULUS <iN> <fCL>
 *
 *  where iN is number of stimulus to corrupt,
 *  fCL is the corruption level of the stimulus,
 *  and range from 0 to 1: if it is 0 the stimulus
 *  is unchanged, while if it is 1 a random stimulus 
 *  is done.
 *
 *  WARNING: NOT IMPLEMENTED...
 */

int processCorruptedStimulusCommand(char *InputLine, double *pTime)
{
   float      f[3];
   char Command[BUFFER_DIM];
   int ParamNum;

   ParamNum = sscanf(InputLine, "%s %f %f %f", Command, &(f[0]), &(f[1]), &(f[2]));

   if (strcmp(strupr(Command), "CORRUPTED_STIMULUS") == 0)
      if (ParamNum == 3)
         createCorruptedStimulus((int)f[0], f[1]);
      else {
         printError("processCorruptedStimulusCommand", "parameters of 'CORRUPTED_STIMULUS' differ from 2.\n");
         return PARSING_ERROR;
      }
   else
      return UNPROCESSED_COMMAND;

   return PROCESSED_COMMAND;
}


/*-----------------------------*
 *  PRINT_DENDRITIC_STRUCTURE  *
 *-----------------------------*/

/**
 *  Print the a summary of the dendritic structure per population:
 *
 *     PRINT_DENDRITIC_STRUCTURE <dT>
 *
 *  <dT> time after which print the summary.
 */

int processPrintDendriticStructureCommand(char *InputLine, double *pTime)
{
   float      f[2];
   char Command[BUFFER_DIM];
   int ParamNum;

   ParamNum = sscanf(InputLine, "%s %f %f", Command, &(f[0]), &(f[1]));

   if (strcmp(strupr(Command), "PRINT_DENDRITIC_STRUCTURE") == 0)
      if (ParamNum == 2) {
         *pTime += f[0];
         newEvent(*pTime, &outDenStruct, NULL, 0);
      } else {
         printError("processPrintDendriticStructureCommand", "parameters of 'PRINT_DENDRITIC_STRUCTURE' differ from 1.\n");
         return PARSING_ERROR;
      }
   else
      return UNPROCESSED_COMMAND;

   return PROCESSED_COMMAND;
}


/*----------------------------*
 *  PRINT_SYNAPTIC_STRUCTURE  *
 *----------------------------*/

/**
 *  Print the a summary of the synaptic structure per population:
 *
 *     PRINT_SYNAPTIC_STRUCTURE <dT>
 *
 *  <dT> time after which print the summary.
 */

int processPrintSynapticStructureCommand(char *InputLine, double *pTime)
{
   float      f[2];
   char Command[BUFFER_DIM];
   int ParamNum;

   ParamNum = sscanf(InputLine, "%s %f %f", Command, &(f[0]), &(f[1]));

   if (strcmp(strupr(Command), "PRINT_SYNAPTIC_STRUCTURE") == 0)
      if (ParamNum == 2) {
         *pTime += f[0];
         if (SynStructResults)
            newEvent(*pTime, &outSynStruct, NULL, 0);
         else
            printError("processPrintSynapticStructureCommand", "'PRINT_SYNAPTIC_STRUCTURE' is neglected because 'OUTSYNSTRUCT' is 'NO'.\n");
      } else {
         printError("processPrintSynapticStructureCommand", "parameters of 'PRINT_SYNAPTIC_STRUCTURE' differ from 1.\n");
         return PARSING_ERROR;
      }
   else
      return UNPROCESSED_COMMAND;

   return PROCESSED_COMMAND;
}


/*-------------------------*
 *  START_SPIKE_RECORDING  *
 *-------------------------*/

/**
 *  Starts to print the spikes if OUT_SPIKES is YES
 *
 *     START_SPIKE_RECORDING <dT>
 *
 *  The neurons monitored are the ones specified in 
 *  the definition file. The dump start after a 
 *  relative time <dT>.
 */

int processStartSpikeRecordingCommand(char *InputLine, double *pTime)
{
   float      f[2];
   char Command[BUFFER_DIM];
   int ParamNum;

   ParamNum = sscanf(InputLine, "%s %f %f", Command, &(f[0]), &(f[1]));

   if (strcmp(strupr(Command), "START_SPIKE_RECORDING") == 0)
      if (ParamNum == 2) {
         *pTime += f[0];
         newEvent(*pTime, &startSpikeRecording, NULL, 0);
      } else {
         printError("processStartSpikeRecordingCommand", "parameters of 'START_SPIKE_RECORDING' differ from 1.\n");
         return PARSING_ERROR;
      }
   else
      return UNPROCESSED_COMMAND;

   return PROCESSED_COMMAND;
}


/*------------------------*
 *  STOP_SPIKE_RECORDING  *
 *------------------------*/

/**
 *  Stops to print the spikes if OUT_SPIKES is YES
 *
 *     STOP_SPIKE_RECORDING <dT>
 *
 *  The neurons monitored are the ones specified in 
 *  the definition file. The stop is at a relative 
 *  time <dT>.
 */

int processStopSpikeRecordingCommand(char *InputLine, double *pTime)
{
   float      f[2];
   char Command[BUFFER_DIM];
   int ParamNum;

   ParamNum = sscanf(InputLine, "%s %f %f", Command, &(f[0]), &(f[1]));

   if (strcmp(strupr(Command), "STOP_SPIKE_RECORDING") == 0)
      if (ParamNum == 2) {
         *pTime += f[0];
         newEvent(*pTime, &startSpikeRecording, NULL, 0);
      } else {
         printError("processStopSpikeRecordingCommand", "parameters of 'STOP_SPIKE_RECORDING' differ from 1.\n");
         return PARSING_ERROR;
      }
   else
      return UNPROCESSED_COMMAND;

   return PROCESSED_COMMAND;
}


/*---------------------*
 *  START_STIMULATION  *
 *---------------------*/

/**
 *  Starts the update of afferent spike frequency from the outside
 *  to the neuron in the population to stimulate.
 *
 *    START_STIMULATION <dT> <iN> <fC>
 *
 *  Stimulation starts after a relative time <dT>. The stimulus to 
 *  consider between the one defined is the <iN>-th. <fC> is the 
 *  "contrast" of the stimulation: the percentage of variation in
 *  the afferent spike frequency from outside (\nu_ext \to 
 *  \nu_ext*(1 + fC).
 *
 *  WARNING: NOT IMPLEMENTED...
 */

int processStartStimulationCommand(char *InputLine, double *pTime)
{
   float      f[4];
   char Command[BUFFER_DIM];
   int ParamNum;

   ParamNum = sscanf(InputLine, "%s %f %f %f %f", Command, &(f[0]), &(f[1]), &(f[2]), &(f[3]));

   if (strcmp(strupr(Command), "START_STIMULATION") == 0)
      if (ParamNum == 4) {
         *pTime += f[0];
         newEvent(*pTime, &startStimulation, NULL, 2, f[1], f[2]);
      } else {
         printError("processStartStimulationCommand", "parameters of 'START_STIMULATION' differ from 3.\n");
         return PARSING_ERROR;
      }
   else
      return UNPROCESSED_COMMAND;

   return PROCESSED_COMMAND;
}


/*--------------------*
 *  STOP_STIMULATION  *
 *--------------------*/

/**
 *  Stops the last activated stimulation
 *
 *    STOP_STIMULATION <dT>
 *
 *  <dT> relative time after which interrupt the stimulation.
 *
 *  WARNING: NOT IMPLEMENTED...
 */

int processStopStimulationCommand(char *InputLine, double *pTime)
{
   float      f[2];
   char Command[BUFFER_DIM];
   int ParamNum;

   ParamNum = sscanf(InputLine, "%s %f %f", Command, &(f[0]), &(f[1]));

   if (strcmp(strupr(Command), "STOP_STIMULATION") == 0)
      if (ParamNum == 2) {
         *pTime += f[0];
         newEvent(*pTime, &stopStimulation, NULL, 0);
      } else {
         printError("processStopStimulationCommand", "parameters of 'STOP_STIMULATION' differ from 1.\n");
         return PARSING_ERROR;
      }
   else
      return UNPROCESSED_COMMAND;

   return PROCESSED_COMMAND;
}


/*-------------*
 *  SET_PARAM  *
 *-------------*/

/**
 *  Hook function called to manage the command which allow to 
 *  modify a population parameter like the external spike
 *  frequency.
 *
 *  WARNING: Only parameter n. 4 is managed (NuExt).
 */

int setPopulationParamFromEvent(event *Event)
{
   setPopulationParam((int)(Event->Param[0]), (int)(Event->Param[1]), Event->Param[2], Event->Time);
   return 1;
}


/**
 *  Hook function called to manage the command which allow to 
 *  modify a synaptic population parameter like the JumpUp or 
 *  JumpDown of the hidden synaptic variable.
 *
 *  WARNING: Only parameter n. 13 and 14 are managed (JumpUp and JumpDown).
 */

int setConnectivityParamFromEvent(event *Event)
{
   setConnectivityParam((int)(Event->Param[0]), (int)(Event->Param[1]), (int)(Event->Param[2]), Event->Param[3], Event->Time);
   return 1;
}


/**
 *  Sets a neural or synaptic population parameter to a new value.
 *
 *    SET_PARAM <time> <population> <param_num> <value>
 *
 *  <param_num> number of parameter to update (see modules.ini) of the.
 *  <population> after a period <time>, from the last event or the 
 *  simulation beginning, with the new value <value>.
 *  The parameter 4 is NuExt.
 *
 *  Other usage:
 *
 *    SET_PARAM <time> <post-pop.> <pre-pop.> <param_num> <value>
 *
 *  <param_num> number of parameter to update (see connectivity.ini) of the
 *  synaptic population having <post-pop.> and <pre-pop.> respectively as
 *  post and pre-synaptic population. The update is after <time> ms from the 
 *  last event or the simulation beginning. <value> is the new value of the 
 *  parameter.
 *  The parameters 13 and 14 are respectively <JumpUp> and <JumpDown>.
 */

int processSetParamCommand(char *InputLine, double *pTime)
{
   float      f[5];
   char Command[BUFFER_DIM];
   int ParamNum;

   ParamNum = sscanf(InputLine, "%s %f %f %f %f %f %f", Command, &(f[0]), &(f[1]), &(f[2]), &(f[3]), &(f[4]));

   if (strcmp(strupr(Command), "SET_PARAM") == 0)
      if (ParamNum == 5) {
         *pTime += f[0];
         newEvent(*pTime, &setPopulationParamFromEvent, NULL, 3, f[1], f[2], f[3]);
      } else 
         if (ParamNum == 6) {
            *pTime += f[0];
            newEvent(*pTime, &setConnectivityParamFromEvent, NULL, 4, f[1], f[2], f[3], f[4]);
         } else {
            printError("processSetParamCommand", "parameters of 'SET_PARAM' differ from 4.\n");
            return PARSING_ERROR;
         }
   else
      return UNPROCESSED_COMMAND;

   return PROCESSED_COMMAND;
}


/*------------------*
 *  SET_PARAM_FROM  *
 *------------------*/

/**
 *  Sets a population parameter to a set of new values read from a file.
 *
 *    SET_PARAM_FROM <in_file_name> <population> <param_num>
 *
 *  Updates the parameter <param_num> (see modules.ini) of the 
 *  population <population> reading from the file <in_file_name> as
 *  many times as the number of rows in the file: each row has to
 *  provide two values, the first is the time after which to update
 *  the parameters with the value specified in the second column.
 */

int processSetParamFromCommand(char *InputLine, double *pTime)
{
   float         f[5];
   char    Command[BUFFER_DIM];
   char   FileName[BUFFER_DIM];
   char     sError[BUFFER_DIM];
   FILE *ParamFile;
   int    ParamNum;

   ParamNum = sscanf(InputLine, "%s %s %f %f %f", Command, FileName, &(f[0]), &(f[1]), &(f[2]));

   if (strcmp(strupr(Command), "SET_PARAM_FROM") == 0)
      if (ParamNum == 4) {
         if ((ParamFile = fopen(FileName, "rt")) == NULL) {
            sprintf(sError, "unable to open parameter file '%s'.", FileName);
            printError("processSetParamFromCommand", sError);
            return PARSING_ERROR;
         }
         while (fscanf(ParamFile, "%f %f", &(f[3]), &(f[4])) == 2) {
            *pTime += f[3];
            newEvent(*pTime, &setPopulationParamFromEvent, NULL, 3, f[0], f[1], f[4]);
         }
         fclose(ParamFile);
      } else {
         printError("processSetParamFromCommand", "parameters of 'SET_PARAM_FROM' differ from 3.\n");
         return PARSING_ERROR;
      }
   else
      return UNPROCESSED_COMMAND;

   return PROCESSED_COMMAND;
}


/*--------------------------*
 *  SINUSOIDAL_STIMULATION  *
 *--------------------------*/

/**
 *  The hook function called to manage an event associated to
 *  the sinusoidal stimulation. The event to manage is the one
 *  passed as parameters.
 *  The return value is 1 if the event has to be disposed from
 *  the heap, 0 if it has to be reused.
 */

int manageSinusoidalStimulation(event *Event)
{
   typedef struct {
              double Pop;
              double Num;
              double T0;
              double Dt;
              double T;
              double Phi;
              double A;
              double tmp;
           } SinStimParam; 
   static SinStimParam *p;

   p = (SinStimParam *)(Event->Param);

   if (Event->Time <= p->T0)
      p->tmp = getPopulationParam((int)p->Pop, (int)p->Num);

   setPopulationParam((int)p->Pop, (int)p->Num,
                      p->tmp * (1.0 + (p->A * sin(6.283185307179586477 * (Event->Time - p->T0) / p->T) + p->Phi)), 
                      Event->Time);
   Event->Time += p->Dt;
   putEvent(Event);

   return 0;
}


/**
 *  Sinusoidal modulation of a population parameter.
 *
 *     SINUSOIDAL_STIMULATION <iP> <iN> <fT0> <fDt> <fT> <fPhi> <fA>
 *
 *  The parameter Param(iP, iN) at time <t> becomes:
 *     Param(iP, iN) (1 + fA sin(2 \pi (t-fT0)/fT + fPhi))
 *  where:
 *     <iP> population to which deliver the modulation;
 *     <iN> parameter number to modulate (4 for \nu_ext);
 *     <fT0> relative time when the modulation starts;
 *     <fDt> the sampling period of the modulation;
 *     <fT> period of the sinusoid;
 *     <fPhi> phase shift of the sinusoid.
 *     <fA> amplitude of the sinusoid.
 */

int processSinusoidalStimulationCommand(char *InputLine, double *pTime)
{
   float      f[8];
   char Command[BUFFER_DIM];
   int ParamNum;

   ParamNum = sscanf(InputLine, "%s %f %f %f %f %f %f %f %f", Command, &(f[0]), &(f[1]), &(f[2]), &(f[3]), &(f[4]), &(f[5]), &(f[6]), &(f[7]));

   if (strcmp(strupr(Command), "SINUSOIDAL_STIMULATION") == 0)
      if (ParamNum == 8) {
         *pTime += f[2];
         newEvent(*pTime, &manageSinusoidalStimulation, NULL, 8, f[0], f[1], *pTime, f[3], f[4], f[5], f[6], 0.0);
      } else {
         printError("processSinusoidalStimulationCommand", "parameters of 'SINUSOIDAL_STIMULATION' differ from 7.\n");
         return PARSING_ERROR;
      }
   else
      return UNPROCESSED_COMMAND;

   return PROCESSED_COMMAND;
}


/*--------------------*
 *  EXTERNAL_COMMAND  *
 *--------------------*/

/**
 *  The hook function called to manage an event associated to
 *  the launch of an external command. The event to manage is the one
 *  passed as parameters.
 *  The return value is 1 if the event has to be disposed from
 *  the heap, 0 if it has to be reused.
 */

int manageExternalCommand(event *Event)
{
   flushOutputFiles();
   system(Event->ParamStr);
   return 1;
}


/**
 *  External command launch.
 *
 *     EXTERNAL_COMMAND <fT> <sCommand>
 *
 *  <fT> relative time when the external command <sCommand> will be 
 *  launched.
 */

int processExternalCommand(char *InputLine, double *pTime)
{
   float         f;
   char    Command[BUFFER_DIM];
   char ExtCommand[BUFFER_DIM];
   int    ParamNum;

   ParamNum = sscanf(InputLine, "%s %f %s", Command, &f, ExtCommand);

   if (strcmp(strupr(Command), "EXTERNAL_COMMAND") == 0)
      if (ParamNum == 3) {
         *pTime += f;
         newEvent(*pTime, &manageExternalCommand, strstr(InputLine, ExtCommand), 0);
      } else {
         printError("processExternalCommand", "parameters of 'EXTERNAL_COMMAND' differ from 2.\n");
         return PARSING_ERROR;
      }
   else
      return UNPROCESSED_COMMAND;

   return PROCESSED_COMMAND;
}


/*-------------------------*
 *  parseAndSubmitCommand  *
 *-------------------------*/

/**
 *  Given an input string, parses it and recognises a
 *  possible command to submit to the simulation creating
 *  an associated event.
 *  The time at which the event will be processed is
 *  a relative time to add to the time pointed by <pTime>
 *  which is provided from outside and will be updated by
 *  this function.
 *  Return 1 if a command is recognized and stored, 0 
 *  otherwise.
 */

int parseAndSubmitCommand(char *InputLine, /* The string to process. */
                          double *pTime)   /* A pointer to the actual time. */
{
   char sBuffer[BUFFER_DIM];
   char *pDest;
   int    rval;

   /* Remove comments if any. */
   pDest = strstr(InputLine, "#");
   if (pDest != NULL) pDest[0] = '\0';

   /* Parses and submit the command. */
   if (sscanf(InputLine, "%s", sBuffer) == EOF) // Is it an empty line?
      return 1;

//
// TO CUSTOMIZE (2)...
//
  if ((rval = processRandomStimuliCommand(InputLine, pTime)) == UNPROCESSED_COMMAND)
     if ((rval = processCorruptedStimulusCommand(InputLine, pTime)) == UNPROCESSED_COMMAND)
        if ((rval = processPrintDendriticStructureCommand(InputLine, pTime)) == UNPROCESSED_COMMAND)
           if ((rval = processStartSpikeRecordingCommand(InputLine, pTime)) == UNPROCESSED_COMMAND)
              if ((rval = processStopSpikeRecordingCommand(InputLine, pTime)) == UNPROCESSED_COMMAND)
                 if ((rval = processStartStimulationCommand(InputLine, pTime)) == UNPROCESSED_COMMAND)
                    if ((rval = processStopStimulationCommand(InputLine, pTime)) == UNPROCESSED_COMMAND)
                       if ((rval = processSinusoidalStimulationCommand(InputLine, pTime)) == UNPROCESSED_COMMAND)
                          if ((rval = processSetParamFromCommand(InputLine, pTime)) == UNPROCESSED_COMMAND)
                             if ((rval = processSetParamCommand(InputLine, pTime)) == UNPROCESSED_COMMAND)
                                if ((rval = processExternalCommand(InputLine, pTime)) == UNPROCESSED_COMMAND)
                                   if ((rval = processPrintSynapticStructureCommand(InputLine, pTime)) == UNPROCESSED_COMMAND)
                                      rval = 0;

   return rval;
}



/*--------------------*
 *  GLOBAL FUNCTIONS  *
 *--------------------*/

/*----------------*
 *  readCommands  *
 *----------------*/
 
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
 *  After the first initialization reading, the following readings
 *  occur each SAMPLING_PERIOD ms.
 */

void readCommands (double *SimuTime) /* A ptr to the actual simulation time. */
{
   static FILE *CommandFile = NULL;      /* The command file. */
   static int       LineNum = 0;         /* The number of scanning line. */
   static double   LastTime;             /* Time of the last command reading. */
   char                Line[BUFFER_DIM]; /* The string cointening the line under parsing. */
   double              Time;             /* Local time to compute absolute time when the events will be managed. */

   /*** Notify the first read of the command file. ***/
#ifdef PRINT_STATUS
   if (SimuTime == NULL)
      fprintf(stderr, "\nReading commands... (Memory: %g Mbytes)\r", (real)MemoryAmount/1024.0/1024.0);
#endif
 
   /*** File opening. ***/
   if (SimuTime == NULL) {
      if (strlen(CommandsFileName)>0) {
         if ((CommandFile = fopen(CommandsFileName, "rt")) == NULL) {
            sprintf(Line, "Unable to open command file '%s'.", CommandsFileName);
            printError("readCommands", Line);
//            CommandFile = stdin;
         }
      } else {
//         CommandFile = stdin;
      }
      Time = START_TIME_OFFSET;
      LastTime = Time;
      initEventManager();
   } else
      /*** Is a new read required? ***/
      if (LastTime + SAMPLING_PERIOD > *SimuTime)
         return;
      else
         LastTime = Time = *SimuTime;

   if (CommandFile == NULL) return;

   /*** Parsing of the commands: one per line. ***/
   while (fgets(Line, BUFFER_DIM, CommandFile) != NULL) {
      parseAndSubmitCommand(Line, &Time);
      LineNum++;      
   }

   /*** Notify the first read of the command file. ***/
#ifdef PRINT_STATUS
   if (SimuTime == NULL)
      fprintf(stderr, "Reading commands... (Memory: %g Mbytes)    \r", (real)MemoryAmount/1024.0/1024.0);

#endif
}



#undef SAMPLING_PERIOD
#undef BUFFER_DIM
#undef UNPROCESSED_COMMAND
#undef PARSING_ERROR
#undef PROCESSED_COMMAND
