#ifndef CC_CONDVARP_H
#define CC_CONDVARP_H

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
#endif /* ! COIN_INTERNAL */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#define NO_IMPLEMENTATION

#ifdef USE_PTHREAD
#include <pthread.h>
#endif /* USE_PTHREAD */

#ifdef USE_W32THREAD
#include <windows.h>
#include <Inventor/C/threads/threadp.h>
#endif /* USE_W32THREAD */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ********************************************************************** */

struct cc_condvar {
#ifdef USE_PTHREAD
  struct cc_pthread_condvar_data {
    pthread_cond_t condid;
  } pthread;
#undef NO_IMPLEMENTATION
#endif /* USE_PTHREAD */

#ifdef USE_W32THREAD
  struct cc_w32thread_condvar_data {
    HANDLE eventhandle[2];
    uint32_t waiters_count;
    CRITICAL_SECTION waiters_count_lock;
  } w32thread;
#undef NO_IMPLEMENTATION
#endif /* USE_W32THREAD */

/*G.Barrand*/
#ifdef NO_IMPLEMENTATION
  int i;
#endif
};

#ifdef NO_IMPLEMENTATION
  /*G.Barrand
#error missing threads implementation support
  */
#endif /* NO_IMPLEMENTATION */

/* ********************************************************************** */

void cc_condvar_struct_init(cc_condvar * condvar_struct);
void cc_condvar_struct_clean(cc_condvar * condvar_struct);

/* ********************************************************************** */

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* ! CC_CONDVARP_H */
