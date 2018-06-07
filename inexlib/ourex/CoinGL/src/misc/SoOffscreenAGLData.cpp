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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#ifdef HAVE_AGL

#include "SoOffscreenAGLData.h"

// Pixels-pr-mm.
SbVec2f
SoOffscreenAGLData::getResolution(void)
{
  return SbVec2f(72.0f / 25.4f, 72.0f / 25.4f);  //G.Barrand : FIXME
  /* G.Barrand : ScreenRes not found under SnowLeopard.
  SInt16 hr, vr;
  ScreenRes(&hr, &vr); 
  return SbVec2f((float)hr / 25.4f, (float)vr / 25.4f);
  */
}

#else
void CoinGL_SoOffscreenAGLData_dummy() {} //G.Barrand
#endif // HAVE_AGL
