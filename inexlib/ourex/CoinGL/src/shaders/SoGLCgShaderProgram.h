#ifndef COIN_SOGLCGSHADERPROGRAM_H
#define COIN_SOGLCGSHADERPROGRAM_H

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
#endif

// *************************************************************************

class SoGLCgShaderObject;

// *************************************************************************

class SoGLCgShaderProgram
{
public:
  SoGLCgShaderProgram(void);
  ~SoGLCgShaderProgram();

  void addShaderObject(SoGLCgShaderObject * shaderObject);
  void removeShaderObjects(void);

  void enable(void);
  void disable(void);

#if defined(SOURCEPROGRAM_HINT)
  SbString getSourceHint(void) const;
#endif

private:
  SoGLCgShaderObject * fragmentShader;
  SoGLCgShaderObject * vertexShader;
};

#endif /* ! COIN_SOGLCGSHADERPROGRAM_H */