/*
 * $Id: ksig.h,v 1.1.1.1 2013/07/18 11:08:49 barrand Exp $
 *
 * $Log: ksig.h,v $
 * Revision 1.1.1.1  2013/07/18 11:08:49  barrand
 *
 *
 * Revision 1.2  2003/11/04 08:00:39  barrand
 * *** empty log message ***
 *
 * Revision 1.1.1.1  1999/05/25 13:17:44  barrand
 * KUIP
 *
 * Revision 1.1.1.1  1996/03/08 15:33:01  mclareni
 * Kuip
 *
 */
/* ksig.h: signal and break handling */

/*
 * Available signal handling package
 *
 *   #define SIGNAL_POSIX  ==>  sigaction()  for Unix
 *   #define SIGNAL_BSD    ==>  sigvec()     for VMS and NeXT
 *   #define SIGNAL_V7     ==>  signal()
 */
#if !defined(SIGNAL_BSD) && !defined(SIGNAL_V7)
#  define SIGNAL_POSIX
#else
#  define sigjmp_buf          jmp_buf
#  define sigsetjmp(buf,save) setjmp(buf)
#  define siglongjmp(buf,val) longjmp(buf,val)
#  ifdef vms
#    define sv_flags sv_onstack
#  endif
#endif

typedef struct {
  int         trap_enabled;     /* flag if exceptions should be trapped */
  int         intr_enabled;     /* flag if ^C delivery is allowed */
  int         intr_pending;     /* flag if ^C happened while disabled */
  int         intr_count;       /* count number of consecutive ^C interrupts */
  int         traceback;        /* print traceback on signal */
  char       *error_msg;        /* messages is handler cannot do print */
  int         soft_intr;        /* flag to stop at a convenient point */
  int         jump_set;         /* flag if stack has been setup */
  sigjmp_buf  stack;
  int         sockfd;           /* socket descriptor and routine to */
  void      (*piaf_sync)();     /* resynchronize Piaf communication */
} KcBreak;
EXTERN KcBreak kc_break;

