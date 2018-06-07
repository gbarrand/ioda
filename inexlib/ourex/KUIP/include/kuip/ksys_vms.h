/* G.Barrand : 
   put in a standalone file to rm of cpp of cxx on OSF1
   because of the '$' used in the name of variables.
*/

#  define OS_NAME "VMS"
#  ifdef __ALPHA
#    define ALPHA
#    define MACHINE_NAME "ALPHA"
#  else
#    define MACHINE_NAME "VAX"
#    define ConCat(con,cat) con/**/cat
#  endif
#  include <clidef.h>
#  include <descrip.h>
#  include <lib$routines.h>     /* lib$... prototypes */
#  include <processes.h>
#  include <rmsdef.h>
#  include <dvidef.h>
#  include <iodef.h>
#  include <lckdef.h>
#  include <lnmdef.h>
#  include <psldef.h>
#  include <ssdef.h>
#  include <starlet.h>          /* sys$... prototypes */
#  include <stsdef.h>
#  include <ttdef.h>
#  include <tt2def.h>
#  include <unixio.h>
#  include <unixlib.h>
#  ifndef R_OK                  /* no access() modes in unixio.h on VAX/VMS */
#    define F_OK 0
#    define X_OK 1
#    define W_OK 2
#    define R_OK 4
#  endif
#  define ARG_STYLE_VMS
#  define F77_EXTERN_LOWERCASE
#  define HAVE_MEMMOVE
#  define HAVE_STAT_H
#  define HAVE_VFORK            /* actually have only vfork */
#  define NO_FCNTL_H
#  define NO_UNISTD_H
#  define SIGNAL_BSD
#  define sigmask(sig) (1L << (sig-1)) /* should be in signal.h */
#  define USE_EDIT_SERVER       /* only for TPU/DISPLAY=MOTIF */

#  if defined(VAXC) && !defined(__DECC)
#    define STUPID_MALLOC
#  endif

#  define fix_descriptor(dsc,str,n) \
     do { \
       dsc.dsc$b_dtype   = DSC$K_DTYPE_T; \
       dsc.dsc$b_class   = DSC$K_CLASS_S; \
       dsc.dsc$a_pointer = str; \
       dsc.dsc$w_length  = n; \
     } while( 0 )

#  define var_descriptor(dsc,str) \
     do { \
       dsc.dsc$b_dtype   = DSC$K_DTYPE_T; \
       dsc.dsc$b_class   = DSC$K_CLASS_S; \
       dsc.dsc$a_pointer = str; \
       dsc.dsc$w_length  = strlen( dsc.dsc$a_pointer ); \
     } while( 0 )

