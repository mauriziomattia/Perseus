/*--------------------------------------------------------*
 *   timer.c                                              *
 *                                                        *
 *   Libreria di funzioni per la registrazione di tempi   *
 *   trascorsi tra due eventi (cronometro).               *
 *                                                        *
 *   Realizzato da Maurizio Mattia.                       *
 *   Iniziato il   15 marzo 1997.                         *
 *   Versione      0.0                                    *
 *--------------------------------------------------------*/



#include <stdio.h>
#include <time.h>



static clock_t Start = 0;  /* Tempo di partenza del cronometro. */
static clock_t Elapse = 0; /* Tempo intermedio.                 */



/*-------------------------------*
 *   startTimer()                *
 *                               *
 *   Fa partire il cronometro.   *
 *-------------------------------*/
 
void startTimer ()

{
   Start = clock();
}



/*-------------------------------*
 *   elapseTimer()               *
 *                               *
 *   Segna il tempo intermedio.  *
 *-------------------------------*/
 
void elapseTimer ()

{
   Elapse = clock();
}



/*----------------------------------------------*
 *   timer ()                                   *
 *                                              *
 *   Riporta in formato stringa il numero di    *
 *   secondi passati da quando e' partito il    *
 *   cronometro a quando e' stato interrotto.   *
 *----------------------------------------------*/
 
char * timer (char * TimeString)

{
   if (Elapse >= Start)
      sprintf(TimeString, "%g",
             (float)(Elapse-Start)/(float)CLOCKS_PER_SEC);
   else
      sprintf(TimeString, "%f", "ERRORE: cronometro non elapsed.");

   return TimeString;
}
