/*--------------------------------------------------------*
 *   timer.h                                              *
 *                                                        *
 *   Libreria di funzioni per la registrazione di tempi   *
 *   trascorsi tra due eventi (cronometro).               *
 *                                                        *
 *   Realizzato da Maurizio Mattia.                       *
 *   Iniziato il   15 marzo 1997.                         *
 *   Versione      0.0                                    *
 *--------------------------------------------------------*/



/*-------------------------------*
 *   startTimer()                *
 *                               *
 *   Fa partire il cronometro.   *
 *-------------------------------*/
 
void startTimer ();



/*-------------------------------*
 *   elapseTimer()               *
 *                               *
 *   Segna il tempo intermedio.  *
 *-------------------------------*/
 
void elapseTimer ();



/*----------------------------------------------*
 *   timer ()                                   *
 *                                              *
 *   Riporta in formato stringa il numero di    *
 *   secondi passati da quando e' partito il    *
 *   cronometro a quando e' stato interrotto.   *
 *----------------------------------------------*/
 
char * timer (char * TimeString);
