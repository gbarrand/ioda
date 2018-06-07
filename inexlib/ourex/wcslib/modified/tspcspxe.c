/*============================================================================

  WCSLIB 4.21 - an implementation of the FITS WCS standard.
  Copyright (C) 1995-2014, Mark Calabretta

  This file is part of WCSLIB.

  WCSLIB is free software: you can redistribute it and/or modify it under the
  terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 3 of the License, or (at your option)
  any later version.

  WCSLIB is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
  more details.

  You should have received a copy of the GNU Lesser General Public License
  along with WCSLIB.  If not, see http://www.gnu.org/licenses.

  Direct correspondence concerning WCSLIB to mark@calabretta.id.au

  Author: Mark Calabretta, Australia Telescope National Facility, CSIRO.
  http://www.atnf.csiro.au/people/Mark.Calabretta
  $Id: tspcspxe.c,v 4.21 2014/03/24 05:12:28 mcalabre Exp $
*=============================================================================
*
* tspcspxe tests function spcspxe().  Not part of the official test suite.
*
*---------------------------------------------------------------------------*/

#include <stdio.h>

#include "spc.h"
#include "wcserr.h"

int main()
{
  char   ptype, xtype;
  int    restreq;
  double crvalX, dXdS;
  struct wcserr *err;

  wcserr_enable(1);

  spcspxe("WAVE-F2W", 0.0, 1.420e9, 0.0, &ptype, &xtype, &restreq, &crvalX,
          &dXdS, &(err));

  printf(" P-type: %c\n", ptype);
  printf(" X-type: %c\n", xtype);
  printf("restreq: %d\n", restreq);
  printf(" crvalX: %f\n", crvalX);
  printf("   dXdS: %f\n", dXdS);
  /*G.Barrand : wcserr_prt(err, '\0');*/
  wcserr_prt(err, 0x0);

  return 0;
}
