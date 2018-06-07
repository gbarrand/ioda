#ifndef  COIN_SOSHADERPROGRAMELEMENT_H
#define  COIN_SOSHADERPROGRAMELEMENT_H


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

#include <Inventor/elements/SoReplacedElement.h>
#include <Inventor/lists/SbList.h>

class SoGLShaderProgram;

// *************************************************************************

class COIN_DLL_API SoGLShaderProgramElement : public SoReplacedElement {
  typedef SoReplacedElement inherited;
  SO_ELEMENT_HEADER(SoGLShaderProgramElement);

public:
  virtual void init(SoState * state);

  static void set(SoState *const state, SoNode *const node,
		  SoGLShaderProgram * program);
  static SoGLShaderProgram * get(SoState * state);

  virtual void push(SoState *);
  virtual void pop(SoState * state, const SoElement *prevTopElement);

  virtual SbBool matches(const SoElement * element) const;
  virtual SoElement * copyMatchInfo(void) const;

SoINTERNAL public:
  static void initClass(void);
  SoGLShaderProgram *shaderProgram;

private:
  virtual ~SoGLShaderProgramElement();
  SbList <uint32_t> objectids;
};

#endif /* ! COIN_SOSHADERPROGRAMELEMENT_H */
