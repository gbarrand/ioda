/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2003 by Systems in Motion.  All rights reserved.
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
 *  See <URL:http://www.coin3d.org> for  more information.
 *
 *  Systems in Motion, Teknobyen, Abels Gate 5, 7030 Trondheim, NORWAY.
 *  <URL:http://www.sim.no>.
 *
\**************************************************************************/

// this :
#include <HEPVis/nodes/SoIndexedMarkerSet.h>

#include <Inventor/misc/SoState.h>

#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/actions/SoGetPrimitiveCountAction.h>
#include <Inventor/elements/SoGLCoordinateElement.h>
#include <Inventor/elements/SoMaterialBindingElement.h>
#include <Inventor/bundles/SoMaterialBundle.h>
#include <Inventor/elements/SoGLTextureEnabledElement.h>
#include <Inventor/elements/SoGLTexture3EnabledElement.h>
#include <Inventor/elements/SoLazyElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/elements/SoCullElement.h>
#include <math.h>
#include <string.h>

//#ifdef HAVE_CONFIG_H
//#include <config.h>
//#endif // HAVE_CONFIG_H

#include <HEPVis/SbGL.h>

#if COIN_DEBUG
#include <Inventor/errors/SoDebugError.h>
#endif // COIN_DEBUG

SO_NODE_SOURCE(HEPVis_SoIndexedMarkerSet);

/*!
  Constructor.
*/
HEPVis_SoIndexedMarkerSet::HEPVis_SoIndexedMarkerSet()
{
  SO_NODE_CONSTRUCTOR(HEPVis_SoIndexedMarkerSet);
  SO_NODE_ADD_FIELD(index, (0));
}

/*!
  Destructor.
*/
HEPVis_SoIndexedMarkerSet::~HEPVis_SoIndexedMarkerSet()
{
}
// doc in super
void
HEPVis_SoIndexedMarkerSet::initClass(void)
{
  SO_NODE_INIT_CLASS(HEPVis_SoIndexedMarkerSet, SoMarkerSet, SoMarkerSet);
}

// Internal method which translates the current material binding found
// on the state to a material binding for this node.  PER_PART,
// PER_FACE, PER_VERTEX and their indexed counterparts are translated
// to PER_VERTEX binding. OVERALL means overall binding for point set
// also, of course. The default material binding is OVERALL.
HEPVis_SoIndexedMarkerSet::Binding
HEPVis_SoIndexedMarkerSet::findMaterialBinding(SoState * const state) const
{
  Binding binding = OVERALL;
  if (SoMaterialBindingElement::get(state) !=
      SoMaterialBindingElement::OVERALL) binding = PER_VERTEX;
  return binding;
}

// doc in super
void
HEPVis_SoIndexedMarkerSet::GLRender(SoGLRenderAction * action)
{
  // FIXME: the marker bitmaps are toggled off when the leftmost pixel
  // is outside the left border, and ditto for the bottommost pixel
  // versus the bottom border. They should be drawn partly until they
  // are wholly outside the canvas instead. 20011218 mortene.

  SoState * state = action->getState();

  state->push();
  // We just disable lighting and texturing for markers, since we
  // can't see any reason this should ever be enabled.  send an angry
  // email to <pederb@coin3d.org> if you disagree.

  SoLazyElement::setLightModel(state, SoLazyElement::BASE_COLOR);
  SoGLTextureEnabledElement::set(state, this, FALSE);
  SoGLTexture3EnabledElement::set(state, this, FALSE);

  if (this->vertexProperty.getValue()) {
    this->vertexProperty.getValue()->GLRender(action);
  }

  const SoCoordinateElement * tmpcoord;
  const SbVec3f * dummy;
  SbBool needNormals = FALSE;

  SoVertexShape::getVertexData(state, tmpcoord, dummy, needNormals);

  if (!this->shouldGLRender(action)) {
    state->pop();
    return;
  }

  const SoGLCoordinateElement * coords = (SoGLCoordinateElement *)tmpcoord;

  Binding mbind = this->findMaterialBinding(action->getState());

  SoMaterialBundle mb(action);
  mb.sendFirst();
  
  int matnr = 0;
  const SbMatrix & mat = SoModelMatrixElement::get(state);
  const SbViewVolume & vv = SoViewVolumeElement::get(state);
  const SbViewportRegion & vp = SoViewportRegionElement::get(state);
  SbVec2s vpsize = vp.getViewportSizePixels();

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, vpsize[0], 0, vpsize[1], -1.0f, 1.0f);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // just in case

  int numidx = this->index.getNum();

  for (int i = 0; i < numidx; i++) {
    int cidx = SbMin(i, this->index.getNum()-1);
    int midx = SbMin(i, this->markerIndex.getNum() - 1);
#if COIN_DEBUG
    if (midx < 0 || (this->markerIndex[midx] >= markerlist->getLength())) {
      static SbBool firsterror = TRUE;
      if (firsterror) {
        SoDebugError::postWarning("HEPVis_SoIndexedMarkerSet::GLRender",
                                  "markerIndex %d out of bound",
                                  markerIndex[i]);
        firsterror = FALSE;
      }
      // Don't render, jump back to top of for-loop and continue with
      // next index.
      continue;
    }
    if (cidx < 0 || (this->index[cidx] >= tmpcoord->getNum())) {
      static int didwarn = 0;
      if (!didwarn) {
        SoDebugError::postWarning("HEPVis_SoIndexedMarkerSet::GLRender",
                                  "index %d out of bound",
                                  this->index[cidx]);
        didwarn = TRUE;
      }
      continue;
    }
#endif // COIN_DEBUG

    if (mbind == PER_VERTEX) mb.send(matnr++, TRUE);
    
    SbVec3f point = coords->get3(this->index[cidx]);
 
    if (this->markerIndex[midx] == NONE) { continue; }

    mat.multVecMatrix(point, point);  // to wcs

    // OpenGL's glBitmap() will not be clipped against anything but
    // the near and far planes. We want markers to also be clipped
    // against other clipping planes, to behave like the SoPointSet
    // superclass.
    const SbBox3f bbox(point, point);
    // FIXME: if there are *heaps* of markers, this next line will
    // probably become a bottleneck. Should really partition marker
    // positions in a oct-tree data structure and cull several at
    // the same time.  20031219 mortene.
    if (SoCullElement::cullTest(state, bbox, TRUE)) { continue; }

    vv.projectToScreen(point, point); // normalized screen coordinates
    point[0] = point[0] * float(vpsize[0]); // screen pixel position
    point[1] = point[1] * float(vpsize[1]);
    // change z range from [0,1] to [-1,1]
    point[2] *= 2.0f;
    point[2] -= 1.0f;

    SbVec2s size;
    const unsigned char * bytes;
    SbBool isLSBFirst; // ignored
        
    if (this->getMarker(this->markerIndex.getValues(0)[midx], 
                        size, bytes, isLSBFirst)) {
      // To have the exact center point of the marker drawn at the
      // projected 3D position.  (FIXME: I haven't actually checked that
      // this is what TGS' implementation of the HEPVis_SoIndexedMarkerSet node does
      // when rendering, but it seems likely. 20010823 mortene.)
      point[0] = point[0] - (size[0] - 1) / 2;
      point[1] = point[1] - (size[1] - 1) / 2;
      
      glRasterPos3f(point[0], point[1], -point[2]);
      glBitmap(size[0], size[1], 0, 0, 0, 0, bytes);
    }
  }

  // FIXME: this looks wrong, shouldn't we rather reset the alignment
  // value to what it was previously?  20010824 mortene.
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // restore default value
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  state->pop(); // we pushed, remember
}

// ----------------------------------------------------------------------------------------------------

// Documented in superclass.
void
HEPVis_SoIndexedMarkerSet::getPrimitiveCount(SoGetPrimitiveCountAction * action)
{
  // Overridden to add the number of markers to the number of images
  // in \a action.
  
  if (!this->shouldPrimitiveCount(action)) return;
  action->addNumImage(this->index.getNum());
}

void 
HEPVis_SoIndexedMarkerSet::computeBBox(SoAction * action, SbBox3f & box,
                                SbVec3f & center)
{
  const SoCoordinateElement *coordelem =
    SoCoordinateElement::getInstance(action->getState());

  SoVertexProperty * vp = (SoVertexProperty *) this->vertexProperty.getValue();
  assert(!vp ||
         vp->getTypeId().isDerivedFrom(SoVertexProperty::getClassTypeId()));
  SbBool vpvtx = vp && (vp->vertex.getNum() > 0);

  const int numCoords = vpvtx ?
    vp->vertex.getNum() :
    coordelem->getNum();

  center.setValue(0.0f, 0.0f, 0.0f);

  int cnt = 0;
  int n = this->index.getNum();
  for (int i = 0; i < n; i++) {
    int idx = this->index[i];
    if (idx < numCoords) {
      SbVec3f tmp = vpvtx ? vp->vertex.getValues(0)[idx] : coordelem->get3(idx);
      box.extendBy(tmp);
      center += tmp;
      cnt++;
    }
  }
  if (cnt) {
    center /= float(cnt);
  }
}

