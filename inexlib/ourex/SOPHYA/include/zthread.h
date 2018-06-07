// This may look like C code, but it is really -*- C++ -*-
// Interface C++ aux Thread POSIX  -   R. Ansari 02/2001
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA

#ifndef ZTHREAD_SEEN
#define ZTHREAD_SEEN

#include "machdefs.h"
#include <pthread.h>
#include <signal.h>
#include "pexceptions.h"


namespace SOPHYA {

typedef void (* ZThreadAction) (void *);

/*!
  \class ZThreadExc
  \ingroup SysTools
  \brief Exception class used by ZThread and ZMutex classes.
*/  
class ZThreadExc : public PException {
public:
  explicit ZThreadExc(const string& m, int id=0) throw() : PException(m,id) {}
  explicit ZThreadExc(const char* m, int id=0) throw() : PException(m,id) {}
};

class ZThread {
public:
  explicit	ZThread(size_t stacksize=0);
  virtual	~ZThread();
  //! Change the thread stack-size, specified in bytes. Can only be called before the thread is started
  inline void   setStackSize(size_t ssz) { _ssize=ssz; }
  //! Change the thread stack-size, specified in kilobytes. Can only be called before the thread is started
  inline void   setStackSizekb(size_t sszk) { _ssize=sszk*1024; }

  void		start();
  void		cancel();
  void          kill(int sig);
  void          join();

  virtual void  run();

  //! Returns true if the thread has been created / started
  bool IfStarted() { return (((_fgstarted)||(_status>0)) ? true : false); }
  //! Returns true if the thread is running 
  bool IfRunning() { return ((_status == 2) ? true : false); }
  //! Returns true if the thread has ended normally 
  bool IfEnded() { return ((_status == 3) ? true : false); }
  //! Returns true if the thread has been stopped (killed/canceled) 
  bool IfStopped() { return ((_status == 5) ? true : false); }

  //! Sets the return code for the thread object
  inline void   setRC(int rc) { _rc = rc; }
  //! Return the value of the return code for the thread object
  inline int    getRC() { return(_rc);
 }
  //! Defines the function which is executed by the default run() method
  inline void	setAction(ZThreadAction act, void * usp=NULL)
       {  _act = act;  _usp = usp; }

  //! Call this method to activate the stop/exit handler on signal \b sig
  static void ActivateExitOnSignal(int sig=SIGUSR1);

  /*!
    \brief Set/changes the current thread cancelability state 
    see pthread_setcancelstate()
    \b st = PTHREAD_CANCEL_ENABLE  or PTHREAD_CANCEL_DISABLE
  */
  static inline void setCancelState(int st= PTHREAD_CANCEL_ENABLE) 
       { int ocs; pthread_setcancelstate(st, &ocs); }
  // PTHREAD_CANCEL_ENABLE  ou PTHREAD_CANCEL_DISABLE

  /*!
    \brief Set/changes the current thread cancelability state 
    see pthread_setcanceltype()
    \b ct = PTHREAD_CANCEL_DEFERRED or PTHREAD_CANCEL_ASYNCHRONOUS
  */
  static inline void setCancelType(int ct= PTHREAD_CANCEL_DEFERRED) 
       { int oct; pthread_setcanceltype(ct, &oct); }
  // PTHREAD_CANCEL_DEFERRED  ou  PTHREAD_CANCEL_ASYNCHRONOUS

  /*! 
    \brief This method is only for internal use by zthread.cc and should not be called by users
  */ 
  virtual void  run_p();

  // ---- Attribute variables ----
  pthread_t _thr;
  size_t _ssize;
  int _rc;
  int _status; // 0 : not started, 1 : started, 2: running, 3: ended, 5:stopped 
  bool _fgstarted;   // false -> true par appel a start()
  ZThreadAction _act;
  void * _usp;
};


class ZMutex {
public:
  explicit	ZMutex(bool fgd=false);
  virtual	~ZMutex();
  //! Locks the mutex object
  inline void	lock()
    { pthread_mutex_lock(_mutx); } 
  //! Unlocks the mutex object
  inline void	unlock()
    { pthread_mutex_unlock(_mutx); }
  //! Waits for a condition change
  inline void	wait()
    { pthread_cond_wait(_condv, _mutx); }
  //! Signal a condition change on the mutex object. 
  inline void	signal()
  { pthread_cond_signal(_condv); }
  //! Broadcasts a condition change on the mutex object
  inline void	broadcast()
  { pthread_cond_broadcast(_condv); }

protected:
  ZMutex(ZMutex const & a) 
    { throw ForbiddenError("ZMutex::ZMutex(ZMutex const & ) forbidden"); }
  ZMutex& operator = (ZMutex const & a) 
    { throw ForbiddenError("ZMutex:: operator = (ZMutex const & a) forbidden"); }

  // Attributes
  pthread_mutex_t *  _mutx;
  pthread_cond_t *  _condv;
  bool _fgd;
};


class ZSync {
public:
/*!
  Constructor. Locks the associated ZMutex. 
  
  - <tt> sigbr==0 </tt> No calls to signal() or broadcast() 
  in destructor 
  - <tt> sigbr==1 </tt> destructor calls \c signal on 
  the associated ZMutex 
  - <tt> sigbr==2 </tt> destructor calls \c broadcast on 
  the associated ZMutex 
*/
  explicit inline  ZSync(ZMutex & mtx, int sigbr=0) 
  {_mtx = &mtx; _sigbr = sigbr; _mtx->lock(); }

/*!
  Constructor from ZMutex pointer. 
  Locks the associated ZMutex object if non null pointer 
*/ 
  explicit inline  ZSync(ZMutex * mtxp, int sigbr=0) 
  {_mtx = mtxp; _sigbr = sigbr; if (_mtx) _mtx->lock(); }
  inline	   ~ZSync() 
  { 
    if (_mtx) { 
      if (_sigbr == 1) _mtx->signal(); 
      else  if (_sigbr == 2) _mtx->broadcast(); 
      _mtx->unlock(); 
    }
  }
//! Calls the wait method on the associated ZMutex object
  inline void wait() { if (_mtx) _mtx->wait(); }
//! To avoid warnings about unused variables 
  inline int NOp() { return _sigbr; } 

private:
  ZMutex * _mtx;
  int _sigbr;
  inline	ZSync() {_mtx = NULL; _sigbr = 0; }

};

} // namespace SOPHYA

#endif
