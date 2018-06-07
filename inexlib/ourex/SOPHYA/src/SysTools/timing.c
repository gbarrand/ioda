/*    Fonction d'impression de temps de calcul et de temps passe  */
/*                               R.Ansari    Juin 93              */

/*
++
  Module	Temps CPU, passé (C)
  Lib	LibsUtil
  include	timing.h

	Fonctions permettant d'imprimer le temps CPU consommé et le temps 
	passé.
--
*/ 

/*
++
  void InitTim()
	Initialisation des chronomètres
  void PrtTim(char *comm)
	Imprime le temps CPU, et le temps passé depuis 
	le dernier appel à "PrtTim()" avec le commentaire
	"comm". Imprime aussi le cumul du temps CPU et du 
	temps passé depuis l'appel à "InitTim()".
--
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "timing.h"

static clock_t CPUT0=0, CPUT=0;
static time_t ELT0=0, ELT=0;
/*-------  modifs Christophe 30/09/04
On somme nous memes les temps partiels pour avoir une autre
mesure du temps CPU total. En effet, pour des jobs longs
clock()-CPUT0 depasse la possibilite de stockage d'un entier
32 bits car clock() renvoit des microsecondes.
Par exemple pour un job de 20h = 72e+9 micro-secondes > 2^32
(Ca ne marche plus pour des jobs > 2146 sec ~= 2^32 microsec)
Seul un entier 64 bits pourrait donner un resultat correct
mais il n'existe pas sur toutes les plateformes.
-------*/ 
static double tcalt_sum = 0.; 
/* Flag ajutes par Reza le 7 Fev 2005 */ 
static int prttim_debug=0;  /* Pour controler l'impression de la somme des temps partiels */
static int prttim_usesum=0;  /* >0 compteur de clock a fait un tour, on utilise tcalt_sum */
 
/* Nouvelle-Fonction */
/*!
   \ingroup SysTools
   Initializes CPU and elapsed time timer (C function).
   The values of the CPU and elapsed time can then be printed 
   using \b PrtTim() 
*/
void InitTim(void)
{
CPUT0 = CPUT = clock();
ELT0 = ELT = time(NULL);
tcalt_sum = 0.;
prttim_usesum = 0;
return;
}

/* Nouvelle-Fonction */
/*!
   \ingroup SysTools
   Actvates (dbg>0) or deactivates (dbg=0) printing of sum of partial elapsed times.
*/
void PrtTimSetDebug(int dbg)
{
  prttim_debug = dbg;
} 
/* Nouvelle-Fonction */
/*!
   \ingroup SysTools
   Prints the values of the CPU and elapsed time, since call to \b InitTim().
*/
void PrtTim(const char * Comm)
{
double tcal,tcalt;
clock_t cput;
time_t elt;
unsigned long etm,etmt;

cput = clock();
tcalt = ( (double)(cput) - (double)(CPUT0) ) / (double)(CLOCKS_PER_SEC);
tcal = ( (double)(cput) - (double)(CPUT) ) / (double)(CLOCKS_PER_SEC);

elt = time(NULL);
etm = elt - ELT;
etmt = elt - ELT0;
/*-------  modifs Christophe 30/09/04
- tcalt_sum en 1/100 ieme de seconde:
  On imprime des Secondes que l'on somme N fois
  -> on se donne une precision a 1/100 de Seconde
- Au moment ou clock()>2^32 tcal devient negatif
  ==> On ne somme donc pas tcal et on TRICHE en sommant "etm"
      (pour etre vrai il faut que le process ait 100% du CPU !)
-------*/
if(tcal>0.) { 
  tcalt_sum += tcal*100.; 
  if (prttim_debug > 0) printf("PrtTim/Warning - tcalt_sum will now be used ...\n");
}
else  tcalt_sum += (double)etm*100.;

if (prttim_usesum)  tcalt = tcalt_sum/100.;
if (prttim_debug > 0) 
  printf("%s CPUTime: Total= %g  (Sum~= %.1f) (Partial= %g) Sec. \n",
         Comm, tcalt, tcalt_sum/100., tcal);
else 
  printf("%s CPUTime: Total= %g   (Partial= %g) Sec. \n",
          Comm, tcalt, tcal);

printf("ElapsedTime(hh:mm:ss): Total= %02ld:%02ld:%02ld ",
       etmt/3600, (etmt%3600)/60, etmt%60);
printf(" (Partial= %02ld:%02ld:%02ld)\n",
       etm/3600, (etm%3600)/60, etm%60);

ELT = elt;
CPUT = cput;
return;
}
