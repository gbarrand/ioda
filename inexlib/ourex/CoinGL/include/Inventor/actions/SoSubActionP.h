#ifndef COIN_SOSUBACTIONP_H
#define COIN_SOSUBACTIONP_H

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

// This file contains macros which are private to the
// implementation. The header file is not installed for the Coin
// development system.

#include <Inventor/actions/SoSubAction.h>
#include <Inventor/C/tidbitsp.h>

// Internally, we use this macro instead of SO_ACTION_ADD_METHOD(). It
// should be used from the constructor, and can not be used from
// within initClass() (as the action classes are initialized before
// the node classes).

#define SO_ACTION_ADD_METHOD_INTERNAL(nodeclass, method) \
  do { \
    SO_ACTION_ADD_METHOD(nodeclass, method); \
  } while (0)

#define SO_ACTION_INTERNAL_INIT_CLASS(_classname_, _parentclassname_) \
   do { \
     SO_ACTION_INIT_CLASS(_classname_, _parentclassname_); \
     coin_atexit((coin_atexit_f*) _classname_::atexit_cleanup, CC_ATEXIT_NORMAL); \
   } while (0)

#endif // !COIN_SOSUBACTIONP_H
