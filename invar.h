/*--------------------------------------------------------------------*
 *                                                                    *
 *   invar.h                                                          *
 *                                                                    *
 *      File delle dichiarazioni delle funzioni nella librearia       *
 *   "invar.c" che fornisce gli strumenti per le leggere da file e    *
 *   da linea di comando la definizione di variabili.                 *
 *      Una definizione di variabile consiste nella seguente sin-     *
 *   tassi                                                            *
 *         <nome_variabile> = <valore_da_assegnare>                   *
 *      Ogni assegnazione deve trovarsi su una singola riga nel ca-   *
 *   so di file di inizializzazione e possono esserci degli spazi     *
 *   tra i vari token, mentre a riga di comando una definizione non   *
 *   deve contenere spazi e sono le diverse definizioni ad essere     *
 *   separate da spazi.                                               *
 *      I nomi delle variabili non sono sensibili alla capitalizza-   *
 *   zione (case insensitive). Nel file di inizializzazione possono   *
 *   essere contenute righe vuote, tutti i caratteri successivi ad    *
 *   una definizione sono ignorati e il carattere '#' commenta una    *
 *   intera riga se disposto al suo inizio.                           *
 *      Le variabili posso essere di soli due tipi numerici: reali,   *
 *   interi o booleani (interi con due soli stati: 0 e 1, associati   *
 *   in input agli stati "NO" e "YES" rispettivamente).               *
 *                                                                    *
 *      La modalita' di utilizzo delle funzioni della libreria e'     *
 *   piuttosto lineare e puo' essere riassunta in modo algoritmico    *
 *   come segue:                                                      *
 *      1. aggiungere nell'insieme delle definizioni riconoscibili    *
 *         le variabili e le loro caratteristiche attraverso l'uso    *
 *         delle funzioni "add#Variable(...)", costruendo una base    *
 *         dati per un'eventuale successiva lettura di definizioni;   *
 *      2. leggere esternamente da riga di comando o da file di       *
 *         inizializzazione le definizioni, facendo uso delle fun-    *
 *         zioni "readDefinitionFile(...)" e "readCommandLineDefi-    *
 *         nition(...)", le quali segualeranno eventuali errori di    *
 *         analisi sintattica e semantica;                            *
 *      3. verificare che tutte le definizioni obbligatorie siano     *
 *         state effettura con la funzione "isDefinitionCompleted".   *
 *   Un esempio pratico di utilizzio e' riportato nel file "invar.c"  *
 *   nella funzione "main" utilizzata come test della libreria.       *
 *                                                                    *
 *   Realizzato da Maurizio Mattia.                                   *
 *   Iniziato il 25 febbraio 1997.                                    *
 *                                                                    *
 *--------------------------------------------------------------------*/



#ifndef __INVAR_H__
#define __INVAR_H__



#include <limits.h>



                             /*----------*
                              *   TIPI   *
                              *----------*/
                              
typedef int         IVinteger;
typedef float       IVreal;
typedef enum {false = 0,
              true} boolean;



                           /*--------------*
                            *   FUNZIONI   *
                            *--------------*/



#ifndef __WATCOMC__

#ifndef _WIN32

/*-------------------------------------------------------*
 *   strupr ( <s> )                                      *
 *                                                       *
 *   Converte le lettere minuscuole in maiuscole nella   *
 *   stringa <s>, modificandola.                         *
 *-------------------------------------------------------*/

char * strupr ( char * s );

#endif   /* _WIN32 */

#endif   /* __WATCOMC__ */



/*--------------------------------------------------------------------*
 *   add#Variable ()                                                  *
 *                                                                    *
 *   Aggiunge una variabile di tipo # (Real, Integer o Boolean) allo  *
 *   insieme delle definizioni.                                       *
 *--------------------------------------------------------------------*/

void addRealVariable (
                      char * Name,        /* Nome della variabile. */
                      IVreal * Variable,    /* Puntatore alla var.   */
                      IVreal MinValue,      /* Valore minimo.        */
                      IVreal MaxValue,      /* Valore massimo.       */
                      boolean IsOptional  /* La def. e' opzionale? */
                     );


void addIntegerVariable (
                         char * Name,        /* Nome della variabile. */
                         IVinteger * Variable, /* Puntatore alla var.   */
                         IVinteger MinValue,   /* Valore minimo.        */
                         IVinteger MaxValue,   /* Valore massimo.       */
                         boolean IsOptional  /* La def. e' opzionale? */
                        );


void addBooleanVariable (
                         char * Name,        /* Nome della variabile. */
                         IVinteger * Variable, /* Puntatore alla var.   */
                         boolean IsOptional  /* La def. e' opzionale? */
                        );


void addStringVariable (
                        char * Name,        /* Nome della variabile.   */
                        char ** Variable,   /* Puntatore alla Stringa. */
                        boolean IsOptional  /* La def. e' opzionale?   */
                       );



/*---------------------------------------------------------------*
 *   readDefinitionFile (<sInFileName>)                          *
 *                                                               *
 *   Legge e assegna le definizioni dal file delle definizioni   *
 *   <sInFileName>. Riporta un valore non nullo se e' stato ri-  *
 *   levato un errore nelle definizioni.                         *
 *---------------------------------------------------------------*/

int readDefinitionFile (const char * sInFileName);



/*--------------------------------------------------------------------*
 *   readCommandLineDefinition (<ArgC>, <ArgV>)                       *
 *                                                                    *
 *   Legge le definizioni dalla linea di comando, riporta un valore   *
 *   non nullo se si e' verificato un errore nelle definizioni.       *
 *--------------------------------------------------------------------*/

int readCommandLineDefinition (
                           int ArgC,      /* Numero di parametri.   */
                           char * ArgV[]  /* Vettore dei parametri. */
                          );


/**
 *   Returns 1 if a fatal error occurs, 0 if parsing ends correctly.
 */

int readArrayDefinition (const char * sInFileName, 
                         int (*arrayProcessing)(int, IVreal *, int, char **));


/*-----------------------------------------------------------*
 *   isDefinitionCompleted ()                                *
 *                                                           *
 *   Determina se tutte le definizioni non opzionali sono    *
 *   state specificate. Riporta un valore non nullo se il    *
 *   test e' fallito, quindi se qualche definizione non e'   *
 *   stata specificata.                                      *
 *-----------------------------------------------------------*/

int isDefinitionCompleted ();



/*-----------------------------------------------------------------*
 *   isDefined ()                                                  *
 *                                                                 *
 *   Risponde alla domanda: la variabile il cui nome e' passato    *
 *   come parametro e' stata definita o a linea di comando o nel   *
 *   file di inizializzazione?                                     *
 *-----------------------------------------------------------------*/

int isDefined (char * Name);



#endif /* __INVAR_H__ */