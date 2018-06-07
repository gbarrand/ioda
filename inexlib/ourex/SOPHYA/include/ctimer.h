// This may look like C code, but it is really -*- C++ -*-
//
// $Id: ctimer.h,v 1.8 2009/05/26 21:39:55 ansari Exp $
//


#ifndef CTIMER_SEEN
#define CTIMER_SEEN

#include "machdefs.h"
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <string>
#include <iostream>

using namespace std;

// <summary> Permet de chronometrer des fonctions. </summary>
// Class Timer qui memorise l'heure de sa creation, et le temps CPU.
// A la fin du bloc ou de la procedure, l'objet est detruit,
// et son destructeur affiche le temps ecoule.

namespace SOPHYA {

//! Simple chronometer class
class Timer  {
public:
  Timer(bool prfg=true);
  // L'objet memorise le temps CPU et l'heure, et le nom donne
  Timer(const char* name, bool prfg=true);

  // Le destructeur appelle split sans parametre.
  virtual ~Timer();

  // Affiche le temps ecoule total/partiel, avec le nom eventuel.
  // Si pas de parametre affiche le nom donne a la creation.
  void Split(const char* comm=0, bool prfg=false);

  //! store partial CPU and elapsed time (no print)
  void SplitQ();

  // Sert a eviter que GNU ne pretende qu'on utilise pas l'objet...
  /*! To avoid not used object compiler warnings */
  void Nop() {}

  /*! \brief Return the total elapsed time (number of seconds), 
    between the object creation and the last call to Split(). 
  */ 
  inline int_8 TotalElapsedTime() { return elapSecT/1000 ; }
  /*! \brief Return the partial elapsed time (number of seconds).
      between the last two calls to Split().
  */ 
  inline int_8 PartialElapsedTime() { return elapSecP/1000 ; }
  
  /*! \brief Return the total elapsed time (in milli-seconds), 
    between the object creation and the last call to Split(). 
  */ 
  inline int_8 TotalElapsedTimems() { return elapSecT ; }
  /*! \brief Return the partial elapsed time (in milli-seconds).
      between the last two calls to Split().
  */ 
  inline int_8 PartialElapsedTimems() { return elapSecP ; }

  /*! \brief Return the total CPU time in seconds, 
    between the object creation and the last call to Split(). 
  */ 
  inline double TotalCPUTime() { return  cpuSecT; }
  /*! \brief Return the partial CPU time in seconds, 
      between the last two calls to Split().
  */ 
  inline double PartialCPUTime() { return  cpuSecP; }

//! Print the CPU and elapsed time on os (partial time corresponding to the last call to Split())
  ostream& Print(ostream& os) const ;
//! Print the CPU and elapsed time on cout 
  inline ostream& Print() const { return Print(cout); }

private:
  clock_t cpu0, cpuSplit;
  struct timeval elapse0, elapseSplit;
  double cpuSecT, cpuSecP;     // Total and partial CPU time 
  int_8 elapSecT, elapSecP;    // Total and partial elapsed time in ms
  string timerName;
  bool defprtflg;
};

/*! operator << overloading - Calls SplitQ() and Print() */
inline ostream& operator << (ostream& s, Timer& tm)
  {  tm.SplitQ(); tm.Print(s);  return(s);  }

} // namespace SOPHYA

#define TIMEF     Timer timer(__PRETTY_FUNCTION__); timer.Nop(); 

#endif // CTIMER_SEEN
