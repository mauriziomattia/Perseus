/*--------------------------------------------------------------------*
 *   nalib.c                                                          *
 *                                                                    *
 *   Raccolta di funzioni da Numerical Recipes in C, per la ricerca   *
 *   di soluzioni di sistemi di equazioni trascendenti, l'integra-    *
 *   zione e derivazione numerica.                                    *
 *                                                                    *
 *   Iniziato il 22 luglio 1998,                                      *
 *--------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "nalib.h"



static int    MAXIT = 200;      /* Numero massimo di iterazioni per la ri-   *
                                 * cerca delle radici.                       */
static double EPS   = 1.0e-6;   /* Precisione numerica della macchina.       */
       char   naError = 0;      /* Se 0 una funzione in nalib ha commesso    *
                                 * un errore. Se 1 le funzioni hanno operato *
                                 * correttamente.                            */



/*--------------------------------------------------------*
 *   nrerror (msg)                                        *
 *                                                        *
 *  Interrompe l'esecuzione del programma, e riporta un   *
 *  messaggio d'errore msg.                               *
 *--------------------------------------------------------*/

 void nrerror (char * msg)

 {
    fprintf(stderr, "%s\n", msg);
    naError = 1;
/*    exit(1); */
 }



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
 
double rtsafe (funcd, x1, x2, xacc)

void (*funcd)(double, double *,double *);
double x1,x2,xacc;

{
   int j;
   double df,dx,dxold,f,fh,fl;
   double swap,temp,xh,xl,rts;

   naError = 0;
   (*funcd)(x1,&fl,&df);
   (*funcd)(x2,&fh,&df);
   if (fl*fh >= 0.0) {
      nrerror("[nalib.c:rtsafe] Root must be bracketed (f(x1)*f(x2)>=0).");
      goto Quit_rtsafe;
   }
   if (fl < 0.0) {
      xl = x1;
      xh = x2;
   } else {
      xh = x1;
      xl = x2;
      swap = fl;
      fl = fh;
      fh = swap;
   }
   rts = 0.5*(x1+x2);
   dxold = fabs(x2-x1);
   dx = dxold;
   (*funcd)(rts,&f,&df);
   for (j=1;j<=MAXIT;j++) {
      if ((((rts-xh)*df-f)*((rts-xl)*df-f) >= 0.0)
                        || (fabs(2.0*f) > fabs(dxold*df))) {
         dxold = dx;
         dx = 0.5*(xh-xl);
         rts = xl+dx;
         if (xl == rts) return rts;
      } else {
         dxold = dx;
         dx = f/df;
         temp = rts;
         rts -= dx;
         if (temp == rts) return rts;
      }
      if (fabs(dx) < xacc) return rts;
      (*funcd)(rts,&f,&df);
      if (f < 0.0) {
         xl = rts;
         fl = f;
      } else {
         xh = rts;
         fh = f;
      }
   }
   nrerror("[nalib.c:rtsafe] Maximum number of iterations exceeded.");
   Quit_rtsafe:
   return 0.; /* per far star zitto il compilatore. */
}



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
 
double zbrent (func, x1, x2, xacc)

double (*func)(double);
double x1, x2, xacc;

{
   int iter;
   double a = x1, b = x2, c, d, e, min1, min2;
   double fa = (*func)(a), fb = (*func)(b), fc, p, q, r, s, xacc1, xm;

   naError = 0;
   if (fb*fa > 0.0) {
      nrerror("[nalib.c:zbrent] Root must be bracketed (f(x1)*f(x2)>=0).");
      goto Quit_zbrent;
   }
   fc = fb;
   for (iter=1; iter<=MAXIT; iter++) {
      if (fb*fc > 0.0) {
         c = a;
         fc = fa;
         e = d = b-a;
      }
      if (fabs(fc) < fabs(fb)) {
         a = b;
         b = c;
         c = a;
         fa = fb;
         fb = fc;
         fc = fa;
      }
      xacc1 = 2.0*EPS*fabs(b)+0.5*xacc;
      xm = 0.5*(c-b);
      if (fabs(xm) <= xacc1 || fb == 0.0) return b;
      if (fabs(e) >= xacc1 && fabs(fa) > fabs(fb)) {
         s = fb/fa;
         if (a == c) {
            p = 2.0*xm*s;
            q = 1.0-s;
         } else {
            q = fa/fc;
            r = fb/fc;
            p = s*(2.0*xm*q*(q-r)-(b-a)*(r-1.0));
            q = (q-1.0)*(r-1.0)*(s-1.0);
         }
         if (p > 0.0)  q = -q;
         p = fabs(p);
         min1 = 3.0*xm*q-fabs(xacc1*q);
         min2 = fabs(e*q);
         if (2.0*p < (min1 < min2 ? min1 : min2)) {
            e = d;
            d = p/q;
         } else {
            d = xm;
            e = d;
         }
      } else {
         d = xm;
         e = d;
      }
      a = b;
      fa = fb;
      if (fabs(d) > xacc1)
         b += d;
      else
         b += (xm > 0.0 ? fabs(xacc1) : -fabs(xacc1));
      fb = (*func)(b);
   }
   nrerror("[nalib.c:zbrent] Maximum number of iterations exceeded.");
   Quit_zbrent:
   return 0.;
}



#define FUNC(x) ((*func)(x))


double trapzd(double (*func)(double), double a, double b, int n)
{

	double x,tnm,sum,del;
	static double s;
	static int it;
	int j;

	if (n == 1) {
		it=1;
		return (s=0.5*(b-a)*(FUNC(a)+FUNC(b)));
	} else {
		tnm=it;
		del=(b-a)/tnm;
		x=a+0.5*del;
		for (sum=0.0,j=1;j<=it;j++,x+=del) sum += FUNC(x);
		it *= 2;
		s=0.5*(s+(b-a)*sum/tnm);
	}
   return s;

}



#define JMAX 20


double qsimp(double (*func)(double), double a, double b)
{

	int j;
	double s,st,ost,os;


	ost = os =  -1.0e30;
   naError = 0;
	for (j=1;j<=JMAX;j++) {
		st=trapzd(func,b,a,j);
		s=(4.0*st-ost)/3.0;
      if (fabs(s-os) < EPS*fabs(os)) {
         return s;
      }
		os=s;
		ost=st;
	}
	nrerror("[nalib.c:qsimp] Too many steps in routine QSIMP");
	return 0.0;

}


#undef JMAX


/*
 *  Return the closest integer to <r>.
 */

int roundr2i (double r)

{
   static double i;

   i = floor(r);
   if (i+0.5 > r) 
      return (int)i;

   return (int)(i+1.0);
}


