#ifndef COIN_POINTERMAP_H
#define COIN_POINTERMAP_H

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

#ifndef COIN_INTERNAL
#error this is a private header file
#endif /* !COIN_INTERNAL */

#include <Inventor/misc/SbHash.h>
#include <Inventor/threads/SbMutex.h>
#include <assert.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

class PointerMap {
public:
  static void initClass(void);

  static void add(const void * ptr, void * dst) {
#ifdef COIN_THREADSAFE
    mutex->lock();
#endif
    SbBool ret = ptrmap->put(ptr, dst);
#ifdef COIN_THREADSAFE
    mutex->unlock();
#endif
    assert(ret == TRUE);
  }
  static void remove(const void * ptr) {
#ifdef COIN_THREADSAFE
    mutex->lock();
#endif
    SbBool ret = ptrmap->remove(ptr);
#ifdef COIN_THREADSAFE
    mutex->unlock();
#endif
    assert(ret == TRUE);    
  }
  static void * get(const void * ptr) {
    void * dst = NULL;
#ifdef COIN_THREADSAFE
    mutex->lock();
#endif
    SbBool ret = ptrmap->get(ptr, dst);
#ifdef COIN_THREADSAFE
    mutex->unlock();
#endif
    assert(ret == TRUE);
    return dst;
  }
private:
  static void cleanup(void);
  static SbHash <void *, const void *> * ptrmap;
  static SbMutex * mutex;
};

#endif // COIN_POINTERMAP_H
