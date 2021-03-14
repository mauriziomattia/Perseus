/*--------------------------------------------------------------------*
 *                                                                    *
 *   invar.c                                                          *
 *                                                                    *
 *   Libreria di funzioni che permette la lettura di variabili con    *
 *   i valori di inizializzazione da un file di inizializzazione o    *
 *   da riga di comando nel seguente formato                          *
 *      <nome_variabile> = <valore_da_assegnare>                      *
 *   Ogni assegnazione deve trovarsi su una singola riga nel caso     *
 *   di file di inizializzazione e possono esserci degli spazi tra    *
 *   i vari token, mentre a riga di comando una definizione non       *
 *   deve contenere spazi e le definizioni sono separati da spazi.    *
 *   I nomi delle variabili non sono sensibili alla capitalizzazio-   *
 *   ne (case insensitive). Nel file di inizializzazione possono      *
 *   essere contenute righe vuote, tutti i caratteri successivi ad    *
 *   una definizione sono ignorati e il carattere '#' commenta una    *
 *   intera riga se disposto al suo inizio.                           *
 *   Le variabili posso essere dei soli seguenti tipi: reali, in-     *
 *   teri, booleani (interi con due soli stati: 0 e 1) o stringhe     *
 *   (delimitate da ').                                               *
 *                                                                    *
 *   Realizzato da Maurizio Mattia.                                   *
 *   Iniziato il 25 febbraio 1997.                                    *
 *                                                                    *
 *--------------------------------------------------------------------*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "invar.h"



                             /*----------*
                              *   TIPI   *
                              *----------*/
                              
typedef union {
   IVreal r;
   IVinteger i;
} number;

typedef enum {realT,              /* Tipo IVreal.    */
              integerT,           /* Tipo IVinteger. */
              booleanT,           /* Tipo boolean.   */
              stringT} vartype;   /* Tipo stringa.   */

typedef struct {
   char *  Name;      /* Nome nella definizione della variabile.     */
   void *  Variable;  /* Puntatore alla variabile.                   */
   number  MinValue;  /* Valore minimo della variabile.              */
   number  MaxValue;  /* Valore massimo assumibile dalla variabile.  */
   vartype Type;      /* Tipo della variabile.                       */
   boolean IsDefined; /* Se true e' stata definita almeno una volta. */
   boolean IsOptional;/* Se true la var. puo' non essere definita.   */
} definition;



                           /*---------------*
                            *   VARIABILI   *
                            *---------------*/

                      /* Insieme delle definizione dichiarate.    */
static definition * DefinitionSet = NULL;
                      /* Numero di definizione nel DefinitionSet. */
static int          DefinitionNumber = 0;
                      /* Messaggio di errore. Se e' "" non e'     *
                       * stato commesso alcun errore.             */
static char         ErrorMessage[256] = "";



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

char * strupr ( char * s )
{
   int i;

   for (i=0; i<strlen(s); i++)
      if (s[i] >= 'a' && s[i] <= 'z')
         s[i] += 'A' - 'a';

   return s;
}

#endif   /* _WIN32 */

#endif   /* __WATCOMC__ */



/*--------------------------------------------------------------------*
 *   add#Variable ()                                                  *
 *                                                                    *
 *   Aggiunge una variabile di tipo # (IVreal, IVinteger o Boolean) allo  *
 *   insieme delle definizioni.                                       *
 *--------------------------------------------------------------------*/

void addRealVariable (
                      char * Name,        /* Nome della variabile. */
                      IVreal * Variable,    /* Puntatore alla var.   */
                      IVreal MinValue,      /* Valore minimo.        */
                      IVreal MaxValue,      /* Valore massimo.       */
                      boolean IsOptional  /* La def. e' opzionale? */
                     )
{
   definition * d;
   
   /*** Crea spazio nell'insieme delle definizioni. ***/
   ++DefinitionNumber;
   if (DefinitionSet == NULL)
      DefinitionSet = malloc(sizeof(*DefinitionSet));
   else
      DefinitionSet = realloc(DefinitionSet,
                              sizeof(*DefinitionSet)*DefinitionNumber);
   d = &DefinitionSet[DefinitionNumber-1];

   /*** Inizializza il campo associato alla variabile. ***/
   d->Name       = Name;
   strupr(d->Name);
   d->Variable   = Variable;
   d->MinValue.r = MinValue;
   d->MaxValue.r = MaxValue;
   d->Type       = realT;
   d->IsDefined  = false;
   d->IsOptional = IsOptional;
}



void addIntegerVariable (
                         char * Name,        /* Nome della variabile. */
                         IVinteger * Variable, /* Puntatore alla var.   */
                         IVinteger MinValue,   /* Valore minimo.        */
                         IVinteger MaxValue,   /* Valore massimo.       */
                         boolean IsOptional  /* La def. e' opzionale? */
                        )
{
   definition * d;
   
   /*** Crea spazio nell'insieme delle definizioni. ***/
   ++DefinitionNumber;
   if (DefinitionSet == NULL)
      DefinitionSet = malloc(sizeof(*DefinitionSet));
   else
      DefinitionSet = realloc(DefinitionSet,
                              sizeof(*DefinitionSet)*DefinitionNumber);
   d = &DefinitionSet[DefinitionNumber-1];

   /*** Inizializza il campo associato alla variabile. ***/
   d->Name       = Name;
   strupr(d->Name);
   d->Variable   = Variable;
   d->MinValue.i = MinValue;
   d->MaxValue.i = MaxValue;
   d->Type       = integerT;
   d->IsDefined  = false;
   d->IsOptional = IsOptional;
}



void addBooleanVariable (
                         char * Name,        /* Nome della variabile. */
                         IVinteger * Variable, /* Puntatore alla var.   */
                         boolean IsOptional  /* La def. e' opzionale? */
                        )
{
   definition * d;
   
   /*** Crea spazio nell'insieme delle definizioni. ***/
   ++DefinitionNumber;
   if (DefinitionSet == NULL)
      DefinitionSet = malloc(sizeof(*DefinitionSet));
   else
      DefinitionSet = realloc(DefinitionSet,
                              sizeof(*DefinitionSet)*DefinitionNumber);
   d = &DefinitionSet[DefinitionNumber-1];

   /*** Inizializza il campo associato alla variabile. ***/
   d->Name       = Name;
   strupr(d->Name);
   d->Variable   = Variable;
   d->Type       = booleanT;
   d->IsDefined  = false;
   d->IsOptional = IsOptional;
}



void addStringVariable (
                        char * Name,        /* Nome della variabile.   */
                        char ** Variable,   /* Puntatore alla Stringa. */
                        boolean IsOptional  /* La def. e' opzionale?   */
                       )
{
   definition * d;
   
   /*** Crea spazio nell'insieme delle definizioni. ***/
   ++DefinitionNumber;
   if (DefinitionSet == NULL)
      DefinitionSet = malloc(sizeof(*DefinitionSet));
   else
      DefinitionSet = realloc(DefinitionSet,
                              sizeof(*DefinitionSet)*DefinitionNumber);
   d = &DefinitionSet[DefinitionNumber-1];

   /*** Inizializza il campo associato alla variabile. ***/
   d->Name       = Name;
   strupr(d->Name);
   d->Variable   = Variable;
   d->Type       = stringT;
   d->IsDefined  = false;
   d->IsOptional = IsOptional;
}



/*-------------------------------------------------------------------*
 *   defineVariable ()                                               *
 *                                                                   *
 *   Definisce la variabile (di tipo IVreal o IVinteger) il cui no-  *
 *   me e valore sono quelli specificati come parametri. Il valore   *
 *   deve essere specificato come una stringa.                       *
 *                                                                   *
 *   ERRORI:                                                         *
 *     a. se il valore non cade nell'intervallo [MinValue,MaxValue]  *
 *        viene generato un errore (se il tipo e' IVinteger o        * 
 *        IVreal).                                                   *
 *     b. se non esiste alcun nome di variabili uguale a quello      *
 *        specificato (il confronto e' case-insensitive) viene       *
 *        generato un errore.                                        *
 *     c. se il tipo del valore non corrisponde con il tipo della    *
 *        variabile.                                                 *
 *     d. se il tipo e' boolean viene generato un errore se il       *
 *        valore non e' una di queste due stringe {"YES", "NO"}.     *
 *-------------------------------------------------------------------*/
 
void defineVariable (
                     char * Name,   /* Nome della variabile */
                     char * Value   /* Valore da assegnare. */
                    )
{
   IVinteger    i;
   IVreal       r;
   definition * d;
   char *      ep = "";  /* Puntatore per valutare errori nelle convers. */
   
   /*** Ricerca della variabile interessata. ***/
   strupr(Name);
   for (i=0; i<DefinitionNumber; i++)
      if (strcmp(Name, DefinitionSet[i].Name) == 0)
         break;

   /*** La variabile e' presente nel DefinitionSet? ***/
   if (i == DefinitionNumber) {
      sprintf(ErrorMessage, "Il nome di variabile '%s' non esiste.",
                            Name);
      return;
   } else
       d = &DefinitionSet[i];

   /*** Converte il valore da assegnare nel tipo previsto. ***/
   switch (d->Type) {
      case realT:
              r = (IVreal)strtod(Value,&ep);
              break;
      case integerT:
              i = strtol(Value,&ep,10);
              break;
      case booleanT:
              strupr(Value);
              if (strcmp(Value, "YES") == 0)
                 i = 1;
              else if (strcmp(Value, "NO") == 0)
                      i = 0;
                   else
                      ep = "ERROR";
   }

   /*** C'e' stato un problema nella conversione? ***/
   if (strlen(ep) > 0) {
      switch (d->Type) {
         case realT:
                 sprintf(ErrorMessage,
                         "Il valore di '%s' deve essere un numero reale: '%s'.",
                         Name, Value);
                 break;
         case integerT:
                 sprintf(ErrorMessage,
                         "Il valore di '%s' deve essere un numero intero: '%s'.",
                          Name, Value);
                 break;
         case booleanT:
                 sprintf(ErrorMessage,
                         "Il valore di '%s' deve essere o YES o NO: '%s'.",
                         Name, Value);
      }
      return;
   }

   /*** Il valore rientra nell'intervallo di correttezza? ***/
   if (d->Type == realT)
      if (r < d->MinValue.r || r > d->MaxValue.r) {
         sprintf(ErrorMessage,
                 "Valore di '%s' non corretto: %g non appartiene a [%g,%g].",
                 Name, (float)r, (float)d->MinValue.r, (float)d->MaxValue.r);
         return;
      } else {}
   else if (d->Type == integerT)
           if (i < d->MinValue.i || i > d->MaxValue.i) {
              sprintf(ErrorMessage,
                      "Valore di '%s' non corretto: %li non appartiene a [%li,%li].",
                      Name, (long)i, (long)d->MinValue.i, (long)d->MaxValue.i);
              return;
           }

   /*** Assegnazione del valore. ***/
   switch (d->Type) {
      case realT:
              *(IVreal *)d->Variable = r;
              break;
      case integerT: 
              *(IVinteger *)(d->Variable) = i;
              break;
      case booleanT:
              *(IVinteger *)(d->Variable) = i;
              break;
      case stringT:
              *(char **)d->Variable = strdup(Value);
   }
   d->IsDefined = true;
}



/*---------------------------------------------------------------*
 *   readDefinitionFile (<sInFileName>)                          *
 *                                                               *
 *   Legge e assegna le definizioni dal file delle definizioni   *
 *   <sInFileName>. Riporta un valore non nullo se e' stato ri-  *
 *   levato un errore nelle definizioni.                         *
 *---------------------------------------------------------------*/

int readDefinitionFile (const char * sInFileName)

{
   FILE * tInFile;
   char sBuffer[1000];
   char sVarName[40],
        sVarValue[40];
   char * sToken;
   char * sDuplicate;
   int  iLine = 0;

   /*** Apertura del file delle definizioni. ***/
   if ((tInFile = fopen(sInFileName, "rt")) == NULL) {
      fprintf(stderr,
              "ERRORE... file delle definizioni '%s' inesistente.\n\n",
              sInFileName);
      return 1;
   }

   /*** Scansione del file. ***/
   while (fgets(sBuffer, 1000, tInFile) != NULL &&
          strlen(ErrorMessage) == 0) {
      iLine++;

      /*** Lettura del Nome della Variabile. ***/
      sDuplicate = strdup(sBuffer);
      sToken = strtok(sBuffer, " =\n");
      if (sToken == NULL) continue;     /* Riga vuota. */
      if (sToken[0] == '#') continue;   /* Commento.   */
      strcpy(sVarName, sToken);

      /*** Lettura del valore del parametro. ***/
      sToken = strtok(NULL, " =\n");
      if (sToken == NULL) {
         sprintf(ErrorMessage, "Valore della '%s' non specificato.",
                               sVarName);
         break;
      }

      /*** E' l'inizio di una stringa? ***/
      if (sToken[0] == 39) {
         sToken = strtok(sDuplicate, "'");
         sToken = strtok(NULL, "'\n");
      }
      strcpy(sVarValue, sToken);

      /*** Definizione della variabile. ***/
      defineVariable(sVarName, sVarValue);
   }

   /*** Chiusura del file d'ingresso. ***/
   fclose(tInFile);

   /*** Gestione degli errori. ***/
   if (strlen(ErrorMessage)>0) {
      fprintf(stderr, "ERRORE nel file di definizione '%s' (linea %i)...\n",
                      sInFileName, iLine);
      fprintf(stderr, "...%s\n\n", ErrorMessage);
      return 1;
   } else
      return 0;
}



/*--------------------------------------------------------------------*
 *   readCommandLineDefinition (<ArgC>, <ArgV>)                       *
 *                                                                    *
 *   Legge le definizioni dalla linea di comando, riporta un valore   *
 *   non nullo se si e' verificato un errore nelle definizioni.       *
 *--------------------------------------------------------------------*/

int readCommandLineDefinition (
                           int ArgC,      /* Numero di parametri.   */
                           char * ArgV[]  /* Vettore dei parametri. */
                          )
{
   int i;
   char sVarName[40],
        sVarValue[80];
   char * sToken;
   char * sDuplicate;

   /*** Scansione dei parametri a linea di comando. ***/
   for (i=1; i<ArgC && strlen(ErrorMessage)==0; i++) {

      /*** Lettura del nome della variabile. ***/
      sDuplicate = strdup(ArgV[i]);
      sToken = strtok(ArgV[i], "=");
      strcpy(sVarName, sToken);

      /*** Lettura del valore della variabile. ***/
      sToken = strtok(NULL, "=");
      if (sToken == NULL) {
         sprintf(ErrorMessage, "Valore della '%s' non specificato.",
                               sVarName);
         break;
      }

      /*** E' l'inizio di una stringa? ***/
      if (sToken[0] == 39) {
         sToken = strtok(sDuplicate, "'");
         sToken = strtok(NULL, "'");
      }
      strcpy(sVarValue, sToken);

      /*** Definizione della variabile. ***/
      defineVariable(sVarName, sVarValue);
   }

   /*** Gestione degli errori. ***/
   if (strlen(ErrorMessage)>0) {
      fprintf(stderr, "ERRORE a linea di comando...\n");
      fprintf(stderr, "...%s\n\n", ErrorMessage);
      return 1;
   } else
      return 0;
}



int readArrayDefinition (const char * sInFileName, 
                         int (*arrayProcessing)(int, IVreal *, int, char **))
/**
 *   Returns 1 if a fatal error occurs, 0 if parsing ends correctly.
 */

#define BUFFER_SIZE      1024
#define TOKENARRAY_SIZE    64
#define STRING_SIZE       256
{
   FILE * tInFile;
   char sBuffer[BUFFER_SIZE];
   char * sToken;
   char * ep = "";
   char sCAT[STRING_SIZE];
   int  iLine = 0;
   int  iST = 0;
   int  iRT = 0;
   char **StringTokens;
   IVreal RealTokens[TOKENARRAY_SIZE];
   
   /*** Opens the definition file... ***/
   if ((tInFile = fopen(sInFileName, "rt")) == NULL) {
      fprintf(stderr,
              "ERROR: population definition file '%s' missing.\n\n",
              sInFileName);
      return 1;
   }

   /*** Dynamic allocation of memory hosting string tokens... ***/
   StringTokens = (char **)malloc(sizeof(char *) * TOKENARRAY_SIZE);
   for (iST=0; iST<TOKENARRAY_SIZE; iST++)
      StringTokens[iST] = (char *)malloc(sizeof(char) * STRING_SIZE);

   /*** File parsing... ***/
   while (fgets(sBuffer, BUFFER_SIZE, tInFile) != NULL &&
          strlen(ErrorMessage) == 0) {
      iLine++;
      iST = 0;
      iRT = 0;

      sToken = strtok(sBuffer, " \n");
      sCAT[0] = '\0';
      while (sToken != NULL) {

         /*** Line parsing... ***/
         if (sToken[0] == '#') break;  // A comment.

         /*** Is it a string? ***/
         if (sCAT[0] == '\0') {
            if (sToken[0] == 39)             
               strcat(sCAT,sToken);
         } else 
            strcat(sCAT,sToken);
            
         if (sCAT[0] != '\0') {
            if (sCAT[strlen(sCAT)-1] == 39) {
               if (iST>TOKENARRAY_SIZE-1) {
                  sprintf(ErrorMessage, "Too many string tokens per line.");
                  break;
               }

               strncpy(StringTokens[iST],&(sCAT[1]),strlen(sCAT)-2);
               StringTokens[iST++][strlen(sCAT)-2] = '\0';
               sCAT[0] = '\0';
            }

         /*** It is a real... ***/
         } else {
            if (iRT>TOKENARRAY_SIZE-1) {
               sprintf(ErrorMessage, "Too many real tokens per line.");
               break;
            }
            RealTokens[iRT++] = (IVreal)strtod(sToken,&ep);
            if (strlen(ep) > 0) {
               strcpy(ErrorMessage,ep);
               break;
            }
         }
         sToken = strtok(NULL, " \n");
      }

      /*** Processing of the element read... ***/
      if (iRT>0 || iST>0)
         if (strlen(ErrorMessage) == 0)
            if ((*arrayProcessing)(iRT, RealTokens, iST, StringTokens))
               sprintf(ErrorMessage,"Semantic problems.");
   }

   /*** Closes the definition file... ***/
   fclose(tInFile);

   /*** Management of parsing errors... ***/
   if (strlen(ErrorMessage)>0) {
      fprintf(stderr, "ERROR in the population definition file  '%s' (line %i)...\n",
                      sInFileName, iLine);
      fprintf(stderr, "...%s\n\n", ErrorMessage);
      return 1;
   } else
      return 0;
}


/*-----------------------------------------------------------*
 *   isDefinitionCompleted ()                                *
 *                                                           *
 *   Determina se tutte le definizioni non opzionali sono    *
 *   state specificate. Riporta un valore non nullo se il    *
 *   test e' fallito, quindi se qualche definizione non e'   *
 *   stata specificata.                                      *
 *-----------------------------------------------------------*/

int isDefinitionCompleted ()

{
   int i;

   /*** Visiona lo stato delle definizioni. ***/
   for (i=0; i<DefinitionNumber; i++)
      if (!DefinitionSet[i].IsOptional &&
          !DefinitionSet[i].IsDefined) {
         fprintf(stderr,
                 "ERRORE... la variabile '%s' non e' stata definita.\n\n",
                 DefinitionSet[i].Name);
         return 1;
      }

   return 0;
}



/*-----------------------------------------------------------------*
 *   isDefined ()                                                  *
 *                                                                 *
 *   Risponde alla domanda: la variabile il cui nome e' passato    *
 *   come parametro e' stata definita o a linea di comando o nel   *
 *   file di inizializzazione?                                     *
 *-----------------------------------------------------------------*/

int isDefined (char * Name)

{
   int i;

   /*** Ricerca della variabile interessata. ***/
   strupr(Name);
   for (i=0; i<DefinitionNumber; i++)
      if (strcmp(Name, DefinitionSet[i].Name) == 0)
         break;

   /*** La variabile e' presente nel DefinitionSet? ***/
   if (i == DefinitionNumber) {
      sprintf(ErrorMessage, "Il nome di variabile '%s' non esiste.",
                            Name);
      return 0;
   } else

   return DefinitionSet[i].IsDefined;
}



/*
#define __TEST_INVAR_C__
*/
#ifdef __TEST_INVAR_C__

                     /*--------------------------*
                      *   TEST DELLA LIBRERIA.   *
                      *--------------------------*/

int main (
            int ArgC,      /* Numero di parametri.   */
            char * ArgV[]  /* Vettore dei parametri. */
         )

{
   int     n;
   IVinteger i[20];
   IVinteger b[20];
   IVreal    r[20];

   /*** Creazione della lista delle definizioni. ***/
   addIntegerVariable ("NE", &i[0], 1, INT_MAX, false);
   addIntegerVariable ("NI", &i[1], 1, INT_MAX, false);
   addRealVariable    ("F", &r[0], 0., 1., false);
   addIntegerVariable ("P", &i[2], 1, INT_MAX, false);

   addRealVariable    ("CXE", &r[1], 0., 1., false);
   addRealVariable    ("CXI", &r[2], 0., 1., false);
   addRealVariable    ("X", &r[3], 0., 1., false);

   addRealVariable    ("BE", &r[4], -1e37, 1e37, false);
   addRealVariable    ("BI", &r[5], -1e37, 1e37, false);

   addRealVariable    ("JP", &r[6], 1e-37, 1e37, false);
   addRealVariable    ("JD", &r[7], 1e-37, 1e37, false);
   addRealVariable    ("JEEXT", &r[8], 1e-37, 1e37, false);
   addRealVariable    ("JEE", &r[9], 1e-37, 1e37, false);
   addRealVariable    ("JEI", &r[10], 1e-37, 1e37, false);
   addRealVariable    ("JIE", &r[11], 1e-37, 1e37, false);
   addRealVariable    ("JII", &r[12], 1e-37, 1e37, false);
   addRealVariable    ("DXY", &r[13], 0., 1e37, false);

   addRealVariable    ("F0E", &r[14], 0., 1., false);
   addRealVariable    ("F0I", &r[15], 0., 1., false);
   addRealVariable    ("FEXT", &r[16], 0., 1., false);

   addIntegerVariable ("TARP", &i[3], 1, INT_MAX, true);
   addIntegerVariable ("MINDELAY", &i[4], 1, INT_MAX, true);
   addIntegerVariable ("MD", &i[4], 1, INT_MAX, true);
   addIntegerVariable ("SAMPLEDELAY", &i[5], 1, INT_MAX, true);
   addIntegerVariable ("SD", &i[5], 1, INT_MAX, true);
   addIntegerVariable ("SPIKETIME", &i[6], 0, INT_MAX, true);
   addIntegerVariable ("ST", &i[6], 0, INT_MAX, true);
   addIntegerVariable ("LIFE", &i[7], 1, INT_MAX, true);

   addIntegerVariable ("PPATTERN", &i[8], 1, INT_MAX, true);
   addIntegerVariable ("PP", &i[8], 1, INT_MAX, true);
   addIntegerVariable ("PPC", &i[9], 1, INT_MAX, true);
   addRealVariable    ("IEXT", &r[17], 0., 1e37, true);
   addIntegerVariable ("PTIEXT", &i[10], 1, INT_MAX, true);

   addIntegerVariable ("SAMPLING", &i[11], 1, INT_MAX, true);
   addIntegerVariable ("SEED", &i[12], 0, INT_MAX, true);

   addBooleanVariable ("SHOW_GRAPHIC", &b[0], true);
   addBooleanVariable ("SHOW_PARAMS", &b[1], true);
   addBooleanVariable ("DUMP_SCREEN", &b[2], true);
   addBooleanVariable ("OUT_SD", &b[3], true);
   addBooleanVariable ("OUT_STATUS", &b[4], true);
   addBooleanVariable ("OUT_PD", &b[5], true);
   addBooleanVariable ("OUT_AC", &b[6], true);
   addBooleanVariable ("OUT_TPP", &b[7], true);
   addBooleanVariable ("WAIT_END", &b[8], true);

   /*** Lettura delle definizioni e rilevazione degli errori. ***/
   if (readDefinitionFile("invar.ini")) return 1;
   if (readCommandLineDefinition(ArgC, ArgV)) return 1;
   if (isDefinitionCompleted()) return 1;

   /*** Stampa dei valori registrati. ***/
   for (n=0; n<13; n++)
       printf("[%i] %i\n", n, (IVinteger)i[n]);
   for (n=0; n<18; n++)
       printf("[%i] %g\n", n, (float)r[n]);
   for (n=0; n<9; n++)
       printf("[%i] %s\n", n, b[n]==1?"YES":"NO");
      
   return 0;
}

#endif


/*
#define __TEST2_INVAR_C__
*/
#ifdef __TEST2_INVAR_C__

                     /*--------------------------*
                      *   TEST DELLA LIBRERIA.   *
                      *--------------------------*/


int myArrayProcessing(int NumRealToken, IVreal * RealToken, int NumStringToken, char **StringToken)
{
   static int RowNum = 0;
   int i;

   RowNum++;
   printf("Array n. %i\n",RowNum);
   for (i=0; i<NumRealToken; i++)
      printf("Real token [%i]: %g\n", i, RealToken[i]);
   for (i=0; i<NumStringToken; i++)
      printf("String token [%i]: %s\n",i, StringToken[i]);
   printf("\n");

   return 0;
}



int main (
            int ArgC,      /* Numero di parametri.   */
            char * ArgV[]  /* Vettore dei parametri. */
         )
{

   readArrayDefinition ("connectivity.ini", myArrayProcessing);

}

#endif
