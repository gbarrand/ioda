#ifndef THSAFEOP_H_SEEN
#define THSAFEOP_H_SEEN

// Classe ThSafeOp : pour permettre l'execution d'operations atomiques en 
// multithread, lors de gestion de comptage de reference en particulier.
//  
// R. Ansari      UPS+LAL IN2P3/CNRS  2007


#include "machdefs.h"
#include "pexceptions.h"

#ifndef SO_NOTHSAFE
#include <pthread.h>
#endif

namespace SOPHYA {


/*! \brief 
    Auxiliary class for insuring thread safe operations in SOPHYA, for reference 
    counting operations 
*/ 
class ThSafeOp {
public:
#if defined(SO_NOTHSAFE)
  explicit ThSafeOp() { }
  ~ThSafeOp() { }
  inline void	lock() { return; }
  inline void	unlock() { return; }
#else
  explicit ThSafeOp() 
    { 
      int rc;
      rc = pthread_mutex_init(&_mutx, NULL);
      if (rc != 0) throw PException("ThSafeOp::ThSafeOp() / Error pthread_mutex_init");
    }
  ~ThSafeOp() 
    {
      int rc;
      rc = pthread_mutex_destroy(&_mutx);
      if (rc != 0) throw PException("ThSafeOp::~ThSafeOp() / Error pthread_mutex_destroy");
    }
  //! Locks the mutex object
  inline void	lock()
    { pthread_mutex_lock(&_mutx); } 
  //! Unlocks the mutex object
  inline void	unlock()
    { pthread_mutex_unlock(&_mutx); }
protected:
  pthread_mutex_t _mutx;
#endif
};
} // namespace SOPHYA

#endif
