#ifndef COIN_DEFS_H
#define COIN_DEFS_H

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

/*
  This file contains definitions which should _only_ be used during
  library build. It is not installed for use by the application
  programmer.
*/

#ifndef COIN_INTERNAL
#error this is a private header file
#endif /* !COIN_INTERNAL */

#ifdef HAVE_CONFIG_H
#include <config.h> /* for HAVE_* defines */
#endif /* HAVE_CONFIG_H */


#ifdef __FILE__
#define COIN_STUB_FILE __FILE__
#else
#define COIN_STUB_FILE ((const char *)0L)
#endif

#ifdef __LINE__
#define COIN_STUB_LINE __LINE__
#else
#define COIN_STUB_LINE 0
#endif

#ifdef __cplusplus
#ifdef HAVE_CPP_COMPILER_FUNCTION_NAME_VAR
#define COIN_STUB_FUNC HAVE_CPP_COMPILER_FUNCTION_NAME_VAR
#else
#define COIN_STUB_FUNC ((const char *)0L)
#endif
#else /* !__cplusplus */
#ifdef HAVE_C_COMPILER_FUNCTION_NAME_VAR
#define COIN_STUB_FUNC HAVE_C_COMPILER_FUNCTION_NAME_VAR
#else
#define COIN_STUB_FUNC ((const char *)0L)
#endif
#endif /* !__cplusplus */


/*
  COIN_STUB(): this is the method which prints out stub
  information. Used where there is functionality missing.

  COIN_OBSOLETED: this is the method which prints out obsoleted
  information. Used where there is an obsoleted or unsupported
  function. Typically a function that we feel should have been private
  in Open Inventor.
*/

#if COIN_DEBUG

#include <Inventor/errors/SoDebugError.h>

#define COIN_STUB() \
  do { \
    SbString s; \
    s.sprintf("%s:%u:%s", \
              COIN_STUB_FILE ? COIN_STUB_FILE : "<>", \
              COIN_STUB_LINE, \
              COIN_STUB_FUNC ? COIN_STUB_FUNC : "<>"); \
    SoDebugError::postWarning(s.getString(), \
                              "STUB: functionality not yet completed"); \
  } while (0)

#define COIN_STUB_ONCE() \
  do { \
    static int first = 1; \
    if (first) { \
      SbString s; \
      s.sprintf("%s:%u:%s", \
                COIN_STUB_FILE ? COIN_STUB_FILE : "<>", \
                COIN_STUB_LINE, \
                COIN_STUB_FUNC ? COIN_STUB_FUNC : "<>"); \
      SoDebugError::postWarning(s.getString(), \
                                "STUB: functionality not yet completed"); \
      first = 0; \
    } \
  } while (0)

#define COIN_OBSOLETED() \
  do { \
    SbString s; \
    s.sprintf("%s:%u:%s", \
              COIN_STUB_FILE ? COIN_STUB_FILE : "<>", \
              COIN_STUB_LINE, \
              COIN_STUB_FUNC ? COIN_STUB_FUNC : "<>"); \
    SoDebugError::post(s.getString(), \
                       "OBSOLETED: functionality not supported (any more)"); \
  } while (0)

#else /* !COIN_DEBUG */

#define COIN_STUB()       do { } while (0)
#define COIN_OBSOLETED()  do { } while (0)
#define COIN_STUB_ONCE()  do { } while (0)

#endif /* !COIN_DEBUG */

/* COIN_CT_ASSERT() - a macro for doing compile-time asserting */
#define COIN_CT_ASSERT(expr) \
  do { switch ( 0 ) { case 0: case (expr): break; } } while ( 0 )

#endif /* !COIN_DEFS_H */
