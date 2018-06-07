//
// $Id: ctimer.cc,v 1.11 2010/01/04 17:06:58 ansari Exp $
//

#include "machdefs.h"
#include "ctimer.h"

#include <iostream>
#include <stdio.h>
#include <string>

//++
// Class	Timer
// Lib  	SysTools
// include	ctimer.h
//
//	Chronométrage de programmes. Le constructeur mémorise l'heure et
//      le temps CPU, ainsi qu'un message éventuel.
//
//	Split affiche le temps partiel.
//
//	Le destructeur affiche le temps total (CPU, et écoulé).
//    
//      Des macros permettent une utilisation simplifiée :
//      * TIMEF   crée un objet de type Timer, avec le nom de la function 
//	courante comme message. Le temps écoulé sera affiché à la sortie 
//	de la function.
//--



/*!
  \class SOPHYA::Timer
  \ingroup SysTools
  \brief Simple chronometer for CPU and elapsed time measurements.
  This class implements a simple chronometer which can be used for
  measuring the CPU and elapsed time in functions. The constructor
  keeps the start time (and CPU time) and an optional message. 
  The \b Split updates partial times and optionaly displays partial and 
  total CPU and elapsed time. 
  The destructor calls Split() if the object has been created with prfg=true.
  displays the total CPU and elapsed time since timer creation.
  The macro \b  TIMEF create a timer object with the function name.
  The elapsed and CPU time are displayed at the end of the bloc or function.
*/

namespace SOPHYA {
//! Default constructor with optional default print flag 
Timer::Timer(bool prfg)
  : defprtflg(prfg)
{
  cpu0 = cpuSplit = clock();
  gettimeofday(&elapse0, NULL);
  elapseSplit = elapse0;
//  elapse0 = elapseSplit = time(0);
  cpuSecT = cpuSecP = 0.;
  elapSecT = elapSecP = 0;
}
/*! 
  \brief Constructor with the specification of a name or message 
  and optional default print flag.
  if \b prfg==true , a call to Split() causes the display of partial and 
  total CPU and elapsed time.
*/
Timer::Timer(const char* name, bool prfg)
  : timerName(((name!=NULL)?name:"")) , defprtflg(prfg)
{
  cpu0 = cpuSplit = clock();
  gettimeofday(&elapse0, NULL);
  elapseSplit = elapse0;
//  elapse0 = elapseSplit = time(0);
  cpuSecT = cpuSecP = 0.;
  elapSecT = elapSecP = 0;
}

//! The destructor call Split() if the object has been created with prtflag=true.
Timer::~Timer()
{
  if (defprtflg) Split();
}


/*! This method updates the CPU and elapsed time. 
    If the prfg==true or the constructor prfg==true, it also displays (prints) 
    the partial CPU and elapsed time
    An optional message can be passed to be used instead of the
    timer name
*/
void Timer::SplitQ()
{
  struct timeval elapse; 
  gettimeofday(&elapse, NULL);

  clock_t cpu   = clock();

  uint_8 deltacpu;
  if (cpu < cpuSplit) {
  	clock_t cpumax = 0;   cpumax = ~cpumax;
  	deltacpu = (uint_8)(cpumax-cpuSplit)+cpu;
  }
  else deltacpu = cpu - cpuSplit;
  
  cpuSecT += ((double)deltacpu) / (double)(CLOCKS_PER_SEC);
  cpuSecP = ((double)deltacpu) / (double)(CLOCKS_PER_SEC);
  
  elapSecP  = (elapse.tv_sec - elapseSplit.tv_sec)*1000  
              + (elapse.tv_usec - elapseSplit.tv_usec)/1000;
  elapSecT = (elapse.tv_sec - elapse0.tv_sec)*1000  
              + (elapse.tv_usec - elapse0.tv_usec)/1000;


  //  cumlcpu += (double)cpuSecP;
  //  cumulelapse += (int_8)etm;

  elapseSplit = elapse;
  cpuSplit    = cpu;
}

void Timer::Split(const char* comm, bool prfg)
{
  SplitQ();
  if ( !defprtflg && !prfg )  return;
  cout << "***Timing " << (comm ? comm : timerName.c_str()) << endl;
  Print(cout);
}

// Pour des formats comme ca, la syntaxe printf est plus agreable.
// Pour ne pas melanger stdio/iostream (pb de desynchronisation sur
// autres C++ que GNU), on fait un cout << chaine.


ostream& Timer::Print(ostream& os) const
{

  int_4 etm  = elapSecP/1000;
  int_4 etmt = elapSecT/1000; 

  char out[200],outp[64];
  sprintf(out,"CPU     Time: Total= %lg (Partial= %lg) s",
	  cpuSecT, cpuSecP);
  os << out << endl;

  if (etmt<60) {
    int_4 etmtms = elapSecT%1000; 
    sprintf(out,"%02d.%03d s", etmt, etmtms);
  }
  else
    sprintf(out,"%02d:%02d:%02d", etmt/3600, (etmt%3600)/60, etmt%60);

  if (etm<60) {
    int_4 etmms = elapSecT%1000; 
    sprintf(outp,"%02d.%03d s", etm, etmms);
  }
  else
    sprintf(outp,"%02d:%02d:%02d", etm/3600, (etm%3600)/60, etm%60);
        
  os << "Elapsed Time: Total=" << out << " (Partial= " << outp << ")" << endl;

  return os;
}

} // FIN namespace SOPHYA 
