/*--------------------------------------------------------------------*
 *   nalib.h                                                          *
 *                                                                    *
 *   Raccolta di funzioni da Numerical Recipes in C, per la ricerca   *
 *   di soluzioni di sistemi di equazioni trascendenti, l'integra-    *
 *   zione e derivazione numerica.                                    *
 *                                                                    *
 *   Iniziato il 22 luglio 1998,                                      *
 *--------------------------------------------------------------------*/
 
 
 
#ifndef __NALIB_H__
#define __NALIB_H__



extern char naError;



/*----------------------------------------------------------------*
 *   InError()                                                    *
 *                                                                *
 *   Riporta un valore TRUE se una l'ultima funzione richiemata   *
 *   di ricerca di una radice ha fallito. FALSE se la funzione    *
 *   precedentemente chiamata non ha trovato la radice.           *
 *----------------------------------------------------------------*/

#define InError() (naError == 1)



/*------------------------------------------------------------*
 *   rtsafe (funcd,x1,x2,xacc)                                *
 *                                                            *
 *   Usando una combinazione dell'algoritmo Newton-Raphson    *
 *   e della bisezione trova la radice all'interno di un      *
 *   intervallo [x1,x2] in cui f(x1)<0 e f(x2)>0. La radice,  *
 *   riportata come valore dalla rtsafe, sara' rifinita fi-   *
 *   no a che la sua accuratrezza non supera in modulo la     *
 *   xacc. funcd e' una funzione definita dall'utente che     *
 *   fornisce sia il valore della funzione che della deri-    *
 *   vata prima [funcd(punto, funzione, derivata)].           *
 *------------------------------------------------------------*/
 
double rtsafe (void (*funcd)(double, double *,double *),
               double x1,
               double x2,
               double xacc);



/*------------------------------------------------------------------*
 *   zbrent (func,x1,x2,xacc)                                       *
 *                                                                  *
 *   Usando il metodo di Brent, si trova la radice di una funzione  *
 *   func(x) di cui e' noto che giace nell'intervallo [x1,x2]. La   *
 *   radice, riportata da zbrent(...), sara' rifinita fino a che    *
 *   la sua accuratezza non sara' xacc. Al contrario del metodo     *
 *   precedente non occorre conoscere la derivata prima del func,   *
 *   ma questo comporta un rallentamento del tempo di convergenza.  *
 *------------------------------------------------------------------*/
 
double zbrent (double (*func)(double),
               double x1,
               double x2,
               double xacc);


double trapzd(double (*func)(double), double a, double b, int n);


double qsimp(double (*func)(double), double a, double b);



/*
 *  Return the closest integer to <r>.
 */

int roundr2i (double r);



#endif /* __NALIB_H__ */
