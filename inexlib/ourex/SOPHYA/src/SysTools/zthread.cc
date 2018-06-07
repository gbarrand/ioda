// Interface C++ aux Thread POSIX  -   R. Ansari 02/2001
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA

#include "sopnamsp.h"
#include "zthread.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

/*!
  \class SOPHYA::ZThread
  \ingroup SysTools
  \brief Simple class for creating and controlling threads.

  This class provides an interface for creating and controlling threads.
  The implementation uses the POSIX thread interface.
  The ZThread objects can be sub classed with the redefinition of 
  the \c run() method which then performs the task.
  The default \c run() method of the base class can be used directly
  to perform computation through a function (see \b setAction() method)
  \sa SOPHYA::ZMutex
  The following sample code shows the usage of ZThread object 
  to run simultaneously two functions to perform computation.

  \code
  // The functions to perform computing
  void fun1(void *arg) { }
  void fun2(void *arg) { }
  // ...
  ZThread zt1;
  zt1.setAction(fun1, arg[1]);
  ZThread zt2;
  zt2.setAction(fun2, arg[1]);
  cout << " Starting threads ... " << endl;
  zt1.start();
  zt2.start();
  cout << " Waiting for threads to end ... " << endl;  
  zt1.join();
  zt2.join();
  \endcode
*/

#define CheckSt(st_, strg_)  if (st_ != 0)  perror(strg_); 

/*
extern "C" {
  void * zthr_run( void * xthr);
  void zthr_sig_exit(int s);
}
*/

static int _RCR_ = 99;
static void * zthr_run(void * xthr)
{
  ZThread * thr = (ZThread *)xthr;
  thr->run_p();
  return &_RCR_;
}

static void zthr_cleanup(void * xthr)
{
  ZThread * thr = (ZThread *)xthr;
  if(thr->_status == 2) thr->_status = 3;
  return;
}

static int _RCS_ = 97;
static void zthr_sig_exit(int s)
{
  printf("zthr_sig_exit(int s=%d)  signal received -> pthread_exit()\n",s);
  pthread_exit(&_RCS_);
}

/* ------ Classe ZThread  -------  */
/*!
  Constructor, with optional specification of the thread stack size in bytes.
  if not specified or zero, defaut system stack size is used.
*/
ZThread::ZThread(size_t stacksize)
{
  _status = 0;
  _fgstarted = false;
  _ssize = 0;
  _act = NULL;
  _usp = NULL;
  _rc = -99;
}

ZThread::~ZThread()
{
  // Que faut-il faire ?
}

/*!
  Method which starts the thread. 
  The thread object is created by a call to pthread_create() in this method.
*/
void ZThread::start()
{
  if ( _fgstarted ) throw ZThreadExc("ZThread::Start() - Already started thread !");
  _fgstarted = true;
  _status = 1;
  int rc;
  pthread_attr_t tha;
  rc = pthread_attr_init(&tha);
  CheckSt(rc,"ZThread::start() - Pb creating tha attribute object");
  if (_ssize > 0) 
    rc = pthread_attr_setstacksize(&tha, _ssize);
  rc = pthread_create(&_thr, &tha, zthr_run, this);
  CheckSt(rc,"ZThread::start() - Pb creating the thread object");     
  setRC(rc);      
}

/*!
  Calls the pthread_cancel. Can be used to stop a running thread.
*/
void ZThread::cancel()
{
  if ( !IfStarted() ) throw ZThreadExc("ZThread::cancel() - thread not started !");
  int rc = pthread_cancel(_thr);
  CheckSt(rc,"ZThread::cancel() - Pb pthread_cancel() ");  
  setRC(-77);      
}

/*!
  Calls the pthread_kill to deliver a signal to the corresponding thread
*/
void ZThread::kill(int sig)
{
  if ( !IfStarted() ) throw ZThreadExc("ZThread::kill() - thread not started !");
  int rc = pthread_kill(_thr, sig);
  CheckSt(rc,"ZThread::kill() - Pb pthread_kill() ");  
  setRC(-76);      
}


/*!
  \brief Waits for the thread to terminate (call pthread_join() )
*/
void ZThread::join()
{
  if ( !IfStarted() ) throw ZThreadExc("ZThread::join() - thread not started !");
  void * x;
  int rc = pthread_join(_thr, &x);
  CheckSt(rc,"ZThread::Join() - Pb pthread_join() ");
  return;  
}

/*!
  \brief Method which does the actual computation 
  This virtual method can be redefined in the derived class, in order 
  to perform the actual computation. The default implementation call
  the action function (if defined by setAction() )
  
*/
void ZThread::run()
{
  if (_act)  _act(_usp);
  exit(0);      
  return;
}

/*!
  \brief Method called by the function passed to pthread_create().
  This method sets the clean-up handler, changes the status variable
  and call the run() method, and pthread_exit() at the end.
*/
void ZThread::run_p()
{
  _status = 2;
  pthread_cleanup_push(zthr_cleanup, (void *)this);
  run();
  _status = 3;
  pthread_exit(&(_rc));
  pthread_cleanup_pop(0);
  return;
}


void ZThread::ActivateExitOnSignal(int sig)
{
  struct sigaction ae, ad;

  ae.sa_handler = zthr_sig_exit;
  ad.sa_handler = SIG_DFL;
  memset( &(ae.sa_mask), 0, sizeof(sigset_t) ); 
  ae.sa_flags = 0;
  memset( &(ad.sa_mask), 0, sizeof(sigset_t) ); 
  ad.sa_flags = 0;
#ifdef  OSF1
  ae.sa_flags = SA_RESTART;
  ad.sa_mask = 0;
#endif
  printf(" Activating signal %d  handling for threads -> pthread_exit()\n",sig);
  sigaction(sig, &ae, NULL);
}


/* ------ Classe ZMutex -------  */
/*!
  \class SOPHYA::ZMutex
  \ingroup SysTools
  \brief Wrapper for a pair of (Mutex,Cond.Var) objects.

  This class implements an interface to the Mutual Exclusion objects
  of the POSIX threads. 
  The ZMutex objects should be used to control acces from different threads 
  to common objects through the \b lock() and \b unlock() methods. 
  When the \b signal() method is called, one of the waiting threads
  on the corresponding mutex object (\b ZMutex::wait()) is awakened. 
  All waiting threads on the mutex object are awakened when \b broadcast()
  is called.
  \sa SOPHYA::ZSync
*/

/*!
  Constructor: Creates an associated pair of POSIX \c pthread_mutex_t 
  and \c pthread_cond_t objects
  \param : if fgd == true , the destructor call pthread_mutex_destroy/pthread_cond_destroy
*/
ZMutex::ZMutex(bool fgd)
{
  _mutx = new pthread_mutex_t;
  _condv = new pthread_cond_t;
  int rc;
  rc = pthread_mutex_init(_mutx, NULL);
  CheckSt(rc,"ZMutex::ZMutex() Pb pthread_mutex_init");
  rc = pthread_cond_init(_condv, NULL);
  CheckSt(rc,"ZMutex::ZMutex() Pb pthread_cond_init");
  _fgd = fgd;
}

ZMutex::~ZMutex()
{
  if (_fgd == false) return;
  int rc;
  rc = pthread_mutex_destroy(_mutx);
  CheckSt(rc,"ZMutex::~ZMutex() Pb pthread_mutex_destroy");
  rc = pthread_cond_destroy(_condv);
  CheckSt(rc,"ZMutex::~ZMutex() Pb pthread_cond_destroy");  
  delete _mutx;
  delete _condv;
}

/*!
  \class SOPHYA::ZSync
  \ingroup SysTools
  \brief Wrapper/utility class ensuring synchronised execution of an instruction bloc.

  This class can be used to insure that the execution of a given 
  part of the code is synchronised, i.e. only a single thread 
  goes through this at a given time.
  The constructor acquires a lock on given \b ZMutex object,
  which is released by the destructor.
  The ZSync object should then be instanciated at the beginning of 
  the synchronised instruction bloc.
  \sa SOPHYA::ZMutex
*/
