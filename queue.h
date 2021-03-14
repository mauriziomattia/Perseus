/*------------------------------------------------------------*
 *                                                            *
 *   queue.h                                                  *
 *                                                            *
 *      Libreria di funzioni che realizza un coda (FIFO) e    *
 *   la gestisce, struttura costituita da generici elementi   *
 *   omogenei.                                                *
 *                                                            *
 *   Realizzato da Maurizio Mattia.                           *
 *   Iniziato il 27 febbraio 1997.                            *
 *                                                            *
 *------------------------------------------------------------*/



#ifndef __QUEUE_H__
#define __QUEUE_H__



                          /*----------*
                           *   TIPI   *
                           *----------*/

/* typedef unsigned char byte; */

/*-----------------------------------------------------------------*
 *      La coda e' una struttura dati circolare e il ridimensio-   *
 *   namento ha luogo solo quando Last raggiunge First.            *
 *   Lo stato della coda e' il seguente:                           *
 *      a. se First e Last sono -1, la coda e' vuota, ma non e'    *
 *         detto che non occupi spazio.                            *
 *-----------------------------------------------------------------*/
 
typedef struct {
           byte * Queue;       /* La coda: vettore di elementi.      */
           size_t First;       /* Addr. al primo elemento della coda.*/
           size_t Last;        /* Addr. all'ultimo elem. della coda. */
           size_t ElementSize; /* Dim. di un elemento della coda.    */
           int    ElementNum;  /* Numero di elementi ospitabili.     */
        } queue;



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
               );



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
                     );



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
                     );



/*---------------------------------------------------------*
 *   isQueueEmpty ()                                       *
 *                                                         *
 *   Riporta un valore non nullo se la coda non contiene   *
 *   alcun elemento.                                       *
 *---------------------------------------------------------*/

int isQueueEmpty (queue Q);



/*---------------------------------------------------------*
 *   elementNumber ()                                      *
 *                                                         *
 *   Riporta il numero di elementi realmente contenuti da  *
 *   dalla coda.                                           *
 *---------------------------------------------------------*/

int elementNumber (queue Q);



/*----------------------------------------------------------------*
 *   isError ()                                                   *
 *                                                                *
 *   Ritorna un valore non nullo se nelle precedenti operazioni   *
 *   sulla coda e' stato rilevato un errore. Stampa su "stdout"   *
 *   questo errore.                                               *
 *----------------------------------------------------------------*/

int isQueueError (void);



#endif /* __QUEUE_H__ */
