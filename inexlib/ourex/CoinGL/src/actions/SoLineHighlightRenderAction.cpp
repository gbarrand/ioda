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

/*!
  \class SoLineHighlightRenderAction SoLineHighlightRenderAction.h Inventor/actions/SoLineHighlightRenderAction.h
  \brief The SoLineHighlightRenderAction class renders selections with line highlighting.
  \ingroup actions

  See the documentation of SoBoxHighlightRenderAction.

  The only difference between SoBoxHighlightRenderAction and this
  action is that this action renders highlights by superposing a
  wireframe image onto each shape instead of the bounding box when
  drawing the highlight.

  \sa SoBoxHighlightRenderAction, SoSelection
*/

// *************************************************************************

#include <Inventor/actions/SoLineHighlightRenderAction.h>

#include <assert.h>

#include <Inventor/SbName.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoSubAction.h>
#include <Inventor/elements/SoDrawStyleElement.h>
#include <Inventor/elements/SoLazyElement.h>
#include <Inventor/elements/SoLinePatternElement.h>
#include <Inventor/elements/SoLineWidthElement.h>
#include <Inventor/elements/SoOverrideElement.h>
#include <Inventor/elements/SoPolygonOffsetElement.h>
#include <Inventor/elements/SoTextureOverrideElement.h>
#include <Inventor/elements/SoTextureQualityElement.h>
#include <Inventor/elements/SoMaterialBindingElement.h>
#include <Inventor/elements/SoNormalElement.h>
#include <Inventor/lists/SoEnabledElementsList.h>
#include <Inventor/lists/SoPathList.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/threads/SbStorage.h>

// *************************************************************************

/*!
  \var SoLineHighlightRenderAction::hlVisible

  Boolean which decides whether or not the highlights for selected
  nodes should be visible.
 */

// *************************************************************************

#define PRIVATE(p) (this->pimpl)
#define PUBLIC(p) (this->owner)

class SoLineHighlightRenderActionP {
public:
  SoLineHighlightRenderActionP(SoLineHighlightRenderAction * o) 
    : colorpacker_storage(sizeof(void*), alloc_colorpacker, free_colorpacker)
  {
    this->owner = o;

    PUBLIC(this)->hlVisible = TRUE;
    this->color = SbColor(1.0f, 0.0f, 0.0f);
    this->linepattern = 0xffff;
    this->linewidth = 3.0f;
    this->searchaction = NULL;

    // SoBase-derived objects should be dynamically allocated.
    this->postprocpath = new SoTempPath(32);
    this->postprocpath->ref();
  }

  ~SoLineHighlightRenderActionP() {
    this->postprocpath->unref();
    delete this->searchaction;
  }

  void drawBoxes(SoPath * pathtothis, const SoPathList * pathlist);

  SoSearchAction * searchaction;
  SbColor color;
  uint16_t linepattern;
  float linewidth;
  SoTempPath * postprocpath;
  SbStorage colorpacker_storage;

private:
  SoLineHighlightRenderAction * owner;

private:
  static void alloc_colorpacker(void * data) {
    SoColorPacker ** cptr = (SoColorPacker**) data;
    *cptr = new SoColorPacker;
  }
  static void free_colorpacker(void * data) {
    SoColorPacker ** cptr = (SoColorPacker**) data;
    delete *cptr;
  }
};

// *************************************************************************

SO_ACTION_SOURCE(SoLineHighlightRenderAction);

// *************************************************************************

// Override from parent class.
void
SoLineHighlightRenderAction::initClass(void)
{
  SO_ACTION_INTERNAL_INIT_CLASS(SoLineHighlightRenderAction, SoGLRenderAction);
}


/*!
  Default constructor. Note: passes a default SbViewportRegion to the
  parent constructor.
 */
SoLineHighlightRenderAction::SoLineHighlightRenderAction(void)
  : inherited(SbViewportRegion())
{
  SO_ACTION_CONSTRUCTOR(SoLineHighlightRenderAction);
  PRIVATE(this) = new SoLineHighlightRenderActionP(this);
}

/*!
  Constructor, taking an explicit \a viewportregion to render.
*/
SoLineHighlightRenderAction::SoLineHighlightRenderAction(const SbViewportRegion & viewportregion)
  : inherited(viewportregion)
{
  SO_ACTION_CONSTRUCTOR(SoLineHighlightRenderAction);
  PRIVATE(this) = new SoLineHighlightRenderActionP(this);
}

/*!
  The destructor.
*/
SoLineHighlightRenderAction::~SoLineHighlightRenderAction()
{
  delete PRIVATE(this);
}

// Documented in superclass. Overridden to add highlighting after the
// "ordinary" rendering.
void
SoLineHighlightRenderAction::apply(SoNode * node)
{
  SoGLRenderAction::apply(node);

  if (this->hlVisible) {
    if (PRIVATE(this)->searchaction == NULL) {
      PRIVATE(this)->searchaction = new SoSearchAction;
      PRIVATE(this)->searchaction->setType(SoSelection::getClassTypeId());
      PRIVATE(this)->searchaction->setInterest(SoSearchAction::ALL);
    }
    PRIVATE(this)->searchaction->apply(node);
    const SoPathList & pathlist = PRIVATE(this)->searchaction->getPaths();
    if ( pathlist.getLength() > 0 ) {
      int i;
      for ( i = 0; i < pathlist.getLength(); i++ ) {
        SoPath * path = pathlist[i];
        assert(path);
        SoSelection * selection = (SoSelection *) path->getTail();
        assert(selection->getTypeId().isDerivedFrom(SoSelection::getClassTypeId()));
        if ( selection->getNumSelected() > 0 )
          PRIVATE(this)->drawBoxes(path, selection->getList());
      }
    }
  }
}

// Documented in superclass.  This method will just call the
// SoGLRenderAction::apply() method (so no highlighting will be done).
//
// It has been overridden to avoid confusing the compiler, which
// typically want to see either all or none of the apply() methods
// overridden.
void
SoLineHighlightRenderAction::apply(SoPath * path)
{
  SoGLRenderAction::apply(path);
}

// Documented in superclass.  This method will just call the
// SoGLRenderAction::apply() method (so no highlighting will be done).
//
// It has been overridden to avoid confusing the compiler, which
// typically want to see either all or none of the apply() methods
// overridden.
void
SoLineHighlightRenderAction::apply(const SoPathList & pathlist,
                                   SbBool obeysrules)
{
  SoGLRenderAction::apply(pathlist, obeysrules);
}

/*!
  Sets if highlight wireframes should be \a visible when
  rendering. Defaults to \c TRUE.
*/
void
SoLineHighlightRenderAction::setVisible(const SbBool visible)
{
  this->hlVisible = visible;
}

/*!
  Return if selection wireframes should be visible.
*/
SbBool
SoLineHighlightRenderAction::isVisible(void) const
{
  return this->hlVisible;
}

/*!
  Sets the \a color of the wireframes. Defaults to red.
*/
void
SoLineHighlightRenderAction::setColor(const SbColor & color)
{
  PRIVATE(this)->color = color;
}

/*!
  Returns color of selection wireframes.
*/
const SbColor &
SoLineHighlightRenderAction::getColor(void)
{
  return PRIVATE(this)->color;
}

/*!
  Sets the line \a pattern used when drawing wireframes. Defaults to
  \c 0xffff (i.e. full, unstippled lines).
*/
void
SoLineHighlightRenderAction::setLinePattern(uint16_t pattern)
{
  PRIVATE(this)->linepattern = pattern;
}

/*!
  Returns line pattern used when drawing wireframe.
*/
uint16_t
SoLineHighlightRenderAction::getLinePattern(void) const
{
  return PRIVATE(this)->linepattern;
}

/*!
  Sets the line \a width used when drawing wireframe. Defaults to 3
  (measured in screen pixels).
*/
void
SoLineHighlightRenderAction::setLineWidth(const float width)
{
  PRIVATE(this)->linewidth = width;
}

/*!
  Returns the line width used when drawing wireframe.
*/
float
SoLineHighlightRenderAction::getLineWidth(void) const
{
  return PRIVATE(this)->linewidth;
}

void
SoLineHighlightRenderActionP::drawBoxes(SoPath * pathtothis,
                                        const SoPathList * pathlist)
{
  int i;
  int thispos = ((SoFullPath *)pathtothis)->getLength()-1;
  assert(thispos >= 0);
  this->postprocpath->truncate(0); // reset

  for (i = 0; i < thispos; i++)
    this->postprocpath->append(pathtothis->getNode(i));

  SoState * state = PUBLIC(this)->getState();
  state->push();

  // we need to disable accumulation buffer antialiasing while
  // rendering selected objects
  int oldnumpasses = PUBLIC(this)->getNumPasses();
  PUBLIC(this)->setNumPasses(1);

  SoLazyElement::setLightModel(state, SoLazyElement::BASE_COLOR);
  
  SoColorPacker ** cptr = (SoColorPacker**) this->colorpacker_storage.get();

  SoLazyElement::setDiffuse(state, pathtothis->getHead(), 1, &this->color, *cptr);
  // FIXME: we should check this versus the actual max line width
  // supported by the underlying OpenGL context. 20050610 mortene.
  SoLineWidthElement::set(state, this->linewidth);
  SoLinePatternElement::set(state, this->linepattern);
  SoTextureQualityElement::set(state, 0.0f);
  SoDrawStyleElement::set(state, SoDrawStyleElement::LINES);
  SoPolygonOffsetElement::set(state, NULL, -1.0f, 1.0f, SoPolygonOffsetElement::LINES, TRUE);
  SoMaterialBindingElement::set(state, NULL, SoMaterialBindingElement::OVERALL); 
  SoNormalElement::set(state, NULL, 0, NULL, FALSE);
 
  SoOverrideElement::setNormalVectorOverride(state, NULL, TRUE);
  SoOverrideElement::setMaterialBindingOverride(state, NULL, TRUE);
  SoOverrideElement::setLightModelOverride(state, NULL, TRUE);
  SoOverrideElement::setDiffuseColorOverride(state, NULL, TRUE);
  SoOverrideElement::setLineWidthOverride(state, NULL, TRUE);
  SoOverrideElement::setLinePatternOverride(state, NULL, TRUE);
  SoOverrideElement::setDrawStyleOverride(state, NULL, TRUE);
  SoOverrideElement::setPolygonOffsetOverride(state, NULL, TRUE);
  SoTextureOverrideElement::setQualityOverride(state, TRUE);

  for (i = 0; i < pathlist->getLength(); i++) {
    SoFullPath * path = (SoFullPath *)(*pathlist)[i];

    for (int j = 0; j < path->getLength(); j++) {
      this->postprocpath->append(path->getNode(j));
    }

    PUBLIC(this)->SoGLRenderAction::apply(this->postprocpath);
    this->postprocpath->truncate(thispos);
  }

  PUBLIC(this)->setNumPasses(oldnumpasses);
  state->pop();
}
