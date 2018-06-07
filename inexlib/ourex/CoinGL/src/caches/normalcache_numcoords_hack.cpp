/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2007 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  ("GPL") version 2 as published by the Free Software Foundation.
 *  See the file LICENSE.GPL at the root directory of this source
 *  distribution for additional information about the GNU GPL.
 *
 *  For using Coin with software that can not be combined with the GNU
 *  GPL, and for taking advantage of the additional benefits of our
 *  support services, please contact Systems in Motion about acquiring
 *  a Coin Professional Edition License.
 *
 *  See http://www.coin3d.org/ for more information.
 *
 *  Systems in Motion, Postboks 1283, Pirsenteret, 7462 Trondheim, NORWAY.
 *  http://www.sim.no/  sales@sim.no  coin-support@coin3d.org
 *
\**************************************************************************/

// Hack used to circumvent the fact that the API interface for the
// public class SoNormalCache does not supply the number of
// coordinates as an argument.
//
// The SoNormalCache interface has already been fixed properly for
// Coin 3.0.

// *************************************************************************

#include "normalcache_numcoords_hack.h"

#include <assert.h>
#include <limits.h>
#include <Inventor/C/tidbitsp.h>
#include <Inventor/C/threads/threadsutilp.h>
#include <Inventor/misc/SbHash.h>

// *************************************************************************

typedef SbHash<unsigned int, const SoNormalCache *> NormalCache2UIntMap;
static NormalCache2UIntMap * numcoordshash_hack = NULL;

// *************************************************************************

static void
numcoords_hack_cleanup(void)
{
  delete numcoordshash_hack;
}

void
normalcache_set_num_coords_hack(const SoNormalCache * n, unsigned int nr)
{
  CC_GLOBAL_LOCK;
  if (!numcoordshash_hack) {
    numcoordshash_hack = new NormalCache2UIntMap;
    coin_atexit((coin_atexit_f*) numcoords_hack_cleanup, 0);
  }
  numcoordshash_hack->put(n, nr);
  CC_GLOBAL_UNLOCK;
}

unsigned int
normalcache_get_num_coords_hack(const SoNormalCache * n)
{
  // if SoNormalCache is used outside Coin it might happen that this
  // hack is not initialized. Just return UINT_MAX to make sure the
  // test in SoNormalCache never fails. pederb, 2005-04-20
  unsigned int ret = UINT_MAX;
  CC_GLOBAL_LOCK;
  if (numcoordshash_hack) {
    if (!numcoordshash_hack->get(n, ret)) { ret = UINT_MAX; }
  }
  CC_GLOBAL_UNLOCK;
  return ret;
}
