/*------------------------------------------------------------*
 *                                                            *
 *   queue.c                                                  *
 *                                                            *
 *      Libreria di funzioni che realizza un coda (FIFO) e    *
 *   la gestisce, struttura costituita da generici elementi   *
 *   omogenei.                                                *
 *                                                            *
 *   Realizzato da Maurizio Mattia.                           *
 *   Iniziato il 27 febbraio 1997.                            *
 *                                                            *
 *------------------------------------------------------------*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "queue.h"



                        /*--------------*
                         *   COSTANTI   *
                         *--------------*/

#define BUFFER_DIM 3  /* N. di elementi da aggungere ad ogni *
                       * ridimensionamento della coda.       */



                        /*---------------*
                         *   VARIABILI   *
                         *---------------*/

static char Error[255] = ""; /* Messaggio di errore, se e' "" alcun *
                              * errore e' stato rilevato.           */



                        /*--------------*
                         *   FUNZIONI   *
                         *--------------*/

/*--------------------------------------------------------*
 *   initQueue ()                                         *
 *                                                        *
 *   Inizializza fisicamente una coda vuota in memoria    *
 *   con una dimensione degli elementi da essa ospitati   *
 *   specificata come parametro.                          *
 *--------------------------------------------------------*/

void initQueue (
                queue * pQueue,     /* Ptr. alla coda da inizializ. */
                size_t  ElementSize /* Dim. degli elementi ospiti.  */
               )

{
   /*** La dimensione degli elementi e' corretta? ***/
   if (ElementSize <= 0) {
      sprintf(Error, "(initQueue) Dimensioni degli elementi non corretta: %i.",
                     (int)ElementSize);
      return;
   }

   /*** Inizializzazione. ***/
   pQueue->ElementSize = ElementSize;
   pQueue->ElementNum  = BUFFER_DIM;
   pQueue->First = pQueue->Last = -1;
   pQueue->Queue = malloc(pQueue->ElementSize*pQueue->ElementNum);

   /*** Coda allocata correttamente? ***/
   if (pQueue->Queue == NULL) {
      sprintf(Error, "(initQueue) Memoria insufficiente.");
      return;
   }
}



/*--------------------------------------------------------*
 *   putQueueElement ()                                   *
 *                                                        *
 *   Aggiunge un elemento alla coda. Se non c'e' spazio   *
 *   ridimensiona automaticamente la struttura. Il pun-   *
 *   tatore all'elemento da aggiungere e' fornito come    *
 *   parametro.                                           *
 *--------------------------------------------------------*/

void putQueueElement (
                      queue * Q, /* Ptr. alla coda interessata.   */
                      void * pE  /* Ptr. all'elem. da aggiungere. */
                     )

{
   /*** La coda e' vuota? ***/
   if (Q->First == -1) {

      /*** La coda non e' allocata fisicamente? ***/
      if (Q->ElementNum == 0) {
         
         Q->ElementNum += BUFFER_DIM;
         Q->Queue = malloc(Q->ElementSize*Q->ElementNum);

         /*** La coda e' allocata correttamente? ***/
         if (Q->Queue == NULL) {
            sprintf(Error, "(putQueue) Memoria insufficiente [1].");
            return;
         }
      }

      Q->Last = 0;
   } else 
      Q->Last += Q->ElementSize;

   /*** Occore operare una reindirizzazione ciclica? ***/
   if (Q->Last >= Q->ElementNum*Q->ElementSize)
      Q->Last = 0;

   /*** Occorre ridimensionare la coda? ***/
   if (Q->Last == Q->First) {

/***TEMPORANEO...
 *
 * La seguente procedura non e' il massimo dell'ottimizzazione,
 * ma il suo effetto, con un opportuno calibraggio di BUFFER_DIM
 * dovrebbe essere reso trascurabile, nelle prestazioni del
 * programma che fa uso della libreria.
 *
 ***/
      /*** Ridimensionamento. ***/
      Q->ElementNum += BUFFER_DIM;
      Q->Queue = realloc(Q->Queue, Q->ElementNum*Q->ElementSize);

      /*** La coda e' riallocata scorrettamente? ***/
      if (Q->Queue == NULL) {
         sprintf(Error, "(putQueue) Memoria insufficiente [2].");
         return;
      }

      /*** Slittamento dei dati, per mantenere integrita'. ***/
      memmove(&(Q->Queue[Q->First+BUFFER_DIM*Q->ElementSize]),
              &(Q->Queue[Q->First]),
              (Q->ElementNum-BUFFER_DIM)*Q->ElementSize-Q->First);
      Q->First += BUFFER_DIM*Q->ElementSize;
   } else
      /*** Si sta inserendo il primo elemento. ***/
      if (Q->First == -1)
         Q->First = 0;

   /*** Copia fisica dell'elemento nella coda. ***/
   memcpy(&(Q->Queue[Q->Last]), pE, Q->ElementSize);
}



/*-------------------------------------------------------*
 *   getQueueElement ()                                  *
 *                                                       *
 *   Preleva un elemento dalla coda, secondo la speci-   *
 *   fica FIFO. L'elemento estratto viene copiato dal    *
 *   nella variabile indirizzata dal puntatore fornito   *
 *   come parametro.                                     *
 *-------------------------------------------------------*/

void getQueueElement (
                      queue * Q, /* Ptr. alla coda interessata.   */
                      void * pE  /* Ptr. all'elem. da aggiungere. */
                     )

{
   /*** La coda e' vuota? ***/
   if (Q->First == -1) {
      sprintf(Error, "(getQueueElement) La coda e' vuota.");
      return;
   }

   /*** Copia fisica dell'elemento dalla coda. ***/
   memcpy(pE, &(Q->Queue[Q->First]), Q->ElementSize);

   /*** La coda e' stata svuotata? ***/
   if (Q->First == Q->Last)
      Q->First = Q->Last = -1;
   else {
      /*** Aggiornamento della testa della coda. ***/
      Q->First += Q->ElementSize;
      if (Q->First >= Q->ElementNum*Q->ElementSize)
         Q->First = 0;
   }
}



/*---------------------------------------------------------*
 *   isQueueEmpty ()                                       *
 *                                                         *
 *   Riporta un valore non nullo se la coda non contiene   *
 *   alcun elemento.                                       *
 *---------------------------------------------------------*/

int isQueueEmpty (queue Q)

{
   return (Q.First == -1);
}



/*---------------------------------------------------------*
 *   elementNumber ()                                      *
 *                                                         *
 *   Riporta il numero di elementi realmente contenuti da  *
 *   dalla coda.                                           *
 *---------------------------------------------------------*/

int elementNumber (queue Q)

{
   if (isQueueEmpty(Q))
      return 0;
   if (Q.First <= Q.Last)
      return (Q.Last-Q.First)/Q.ElementSize+1;
   return (Q.ElementSize*Q.ElementNum-Q.First+Q.Last)/Q.ElementSize+1;
}



/*----------------------------------------------------------------*
 *   isError ()                                                   *
 *                                                                *
 *   Ritorna un valore non nullo se nelle precedenti operazioni   *
 *   sulla coda e' stato rilevato un errore. Stampa su "stdout"   *
 *   questo errore.                                               *
 *----------------------------------------------------------------*/

int isQueueError (void)

{
   if (strlen(Error) > 0) { 
      printf("ERRORE...%s\n", Error);
      sprintf(Error,"");
      return 1;
   }
   return 0;
}



/*
#define __TEST_QUEUE_C__
*/
#ifdef __TEST_QUEUE_C__

                             /*----------*
                              *   TEST   *
                              *----------*/

/*--------------------------------------------------------------*
 *   Chiamare il programma come                                 *
 *                                                              *
 *      queue <n>                                               *
 *                                                              *
 *   dove <n> e' la quantita' di numeri casuali da aggiungere   *
 *   alla coda.                                                 *
 *--------------------------------------------------------------*/
 
void main (
           int ArgC,      /* Numero di parametri.   */
           char * ArgV[]  /* Vettore dei parametri. */
          )

{
   int i, r;
   queue q;

   /*** Crea la coda. ***/
   initQueue (&q, sizeof(int));
   
   /*** Inserisce elementi casuali. ***/
   for (i=0; i<atoi(ArgV[1]); i++) {
      r = rand();
      putQueueElement(&q, &r);
      printf("[%i] %i\n", i, r);
      isQueueError();
   }
   printf("\n");

   /*** Ripesca gli elementi dalla coda. ***/
   i=0;
   while (!isQueueEmpty(q)) {
      getQueueElement(&q, &r);
      printf("[%i] %i\n", i++, r);
      isQueueError();
   }
}

#endif /* __TEST_QUEUE_C__ */
