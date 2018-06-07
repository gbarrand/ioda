/*
 * $Id: kutrue.c,v 1.1.1.1 2013/07/18 11:08:49 barrand Exp $
 *
 * $Log: kutrue.c,v $
 * Revision 1.1.1.1  2013/07/18 11:08:49  barrand
 *
 *
 * Revision 1.2  1999/12/03 15:42:31  barrand
 * *** empty log message ***
 *
 * Revision 1.1.1.1  1999/05/25  13:09:15  barrand
 * KUIP
 *
 * Revision 1.1.1.1  1996/03/08 15:32:58  mclareni
 * Kuip
 *
 */
/*CMZ :  2.05/16 17/08/94  12.26.34  by  Alfred Nathaniel*/
/*-- Author :    Alfred Nathaniel   14/06/92*/

#include "kuip/kuip.h"
#include "kuip/kfor.h"

static LOGICAL f77_true = ~0;
static LOGICAL f77_false = 0;

/*
 * set Fortran's notion of .TRUE. and .FALSE.
 */
void Kutrue( LOGICAL *ftrue, LOGICAL *ffalse )
{
  f77_true = *ftrue;
  f77_false = *ffalse;
}

LOGICAL ku_true( int return_true )
{
  return return_true ? f77_true : f77_false;
}
