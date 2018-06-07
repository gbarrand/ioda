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
  \class SoGLLazyElement Inventor/elements/SoGLLazyElement.h
  \brief The SoGLLazyElement class is meant to optimize GL rendering.
  \ingroup elements

  This is just a wrap-around implementation for compatibility. It should
  (hopefully) work in the same way as the Inventor class though.
*/

// FIXME: is the above class doc comment still correct? Or do we have
// an implementation of this class in the same manner as the other
// Inventors now? 20040702 mortene.

// *************************************************************************

#include <Inventor/elements/SoGLLazyElement.h>

#include <assert.h>
#include <string.h>

#include <Inventor/C/glue/gl.h>
#include <Inventor/SbImage.h>
#include <Inventor/SoFullPath.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/bundles/SoMaterialBundle.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/elements/SoGLDisplayList.h>
#include <Inventor/elements/SoGLTexture3EnabledElement.h>
#include <Inventor/elements/SoGLTextureEnabledElement.h>
#include <Inventor/elements/SoGLTextureImageElement.h>
#include <Inventor/elements/SoShapeStyleElement.h>
#include <Inventor/elements/SoTextureCombineElement.h>
#include <Inventor/elements/SoGLShaderProgramElement.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/misc/SoGLImage.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/C/tidbits.h>
#include <coindefs.h> // COIN_OBSOLETED
#include "../../misc/SoVBO.h"
#include "../../shaders/SoGLShaderProgram.h"

// *************************************************************************

#define FLAG_FORCE_DIFFUSE      0x0001
#define FLAG_DIFFUSE_DEPENDENCY 0x0002

#if COIN_DEBUG
// #define GLLAZY_DEBUG(_x_) (SoDebugError::postInfo(COIN_STUB_FUNC, _x_))
#define GLLAZY_DEBUG(x)
#else
#define GLLAZY_DEBUG(x)
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

// Some data and functions to create Bayer dither matrices (used for
// screen door transparency)
static unsigned char stipple_patterns[64 + 1][32 * 4];
static uint32_t two_by_two[] = {0, 2, 3, 1};

//
// Used to generate a matrix twice the size of the input
//
static void
generate_next_matrix(uint32_t * old, int oldsize,
                     uint32_t * matrix)
{
  int i,j;
  int newsize = oldsize << 1;
  for (i = 0; i <  newsize; i++) {
    for (j = 0; j < newsize; j++) {
      matrix[i*newsize+j] = 4 * old[(i%oldsize)*oldsize + (j%oldsize)];
      matrix[i*newsize+j] += two_by_two[(i/oldsize)*2 + (j/oldsize)];
    }
  }
}

//
// Creates a matrix by starting with a 2x2 and doubling until size
//
static void
make_dither_matrix(uint32_t * ptr, int size)
{
  int currsize = 2;

  uint32_t * currmatrix = two_by_two;
  uint32_t * nextmatrix = NULL;
  int nextsize;

  while (currsize < size) {
    nextsize = currsize << 1;
    nextmatrix = new uint32_t[nextsize*nextsize];
    generate_next_matrix(currmatrix, currsize, nextmatrix);
    if (currmatrix != two_by_two) delete[] currmatrix;
    currmatrix = nextmatrix;
    currsize = nextsize;
  }
  // copy matrix
  int i;
  for (i = 0; i < size*size; i++)
    ptr[i] = currmatrix[i];

  if (currmatrix != two_by_two) delete[] currmatrix;
}

//
// Sets a bit bitnr bits from ptr
//
static void
set_bit(int bitnr, unsigned char * ptr)
{
  int byte = bitnr / 8;
  int bit = bitnr % 8;

  unsigned char mask = (unsigned char) (0x80 >> bit);

  ptr[byte] |= mask;
}

//
// Create a bitmap from a 32x32 matrix
//
static void
create_matrix_bitmap(int intensity, unsigned char * bitmap,
                     uint32_t *matrix, int size)
{
  int cnt = 0;
  int i,j;
  for (i = 0; i < 32*4; i++) bitmap[i] = 0;
  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
      if (matrix[i*size+j] > (uint32_t) intensity) {
        set_bit(i*32+j, bitmap);
        cnt++;
      }
    }
  }
}

// HACK. Replacement for GLState since we needed some more data
// members without breaking the ABI.  It's fixed properly in Coin-3
// (Coin development SVN).
typedef struct {
  // these are the same as GLState
  uint32_t cachebitmask;
  uint32_t diffuse;
  SbColor ambient;
  SbColor emissive;
  SbColor specular;
  float shininess;

  // pack these into one 32-bit number to save some mem
  int32_t blending : 2;
  int32_t culling : 2;
  int32_t twoside : 2;
  int32_t flatshading : 2;
  int32_t alphatest : 2;
  int32_t vertexordering : 22; // rest of the 32 bits

  int32_t lightmodel;
  int32_t stipplenum;
  int32_t blend_sfactor;
  int32_t blend_dfactor;
  int32_t glimageid;

  // extra data needed to fix a cache dependency bug.
  uint32_t diffusenodeid;
  uint32_t transpnodeid;
} SoInternalGLState;

/* FIXME: Horribly dirty void* cast added to calm down a gcc 4.1.1
 * type-punned pointer compiler warning. 20070308 tamer. */
// defines used to cast GLState to SoInternalGLState
#define UPDATE_GLSTATE(_ptr_, _name_, _val_) \
  ((SoInternalGLState*)(void*)&((_ptr_)->glstate))->_name_ = _val_

/* FIXME: Horribly dirty void* cast added to calm down a gcc 4.1.1
 * type-punned pointer compiler warning. 20070308 tamer. */
#define GLSTATE(_ptr_) \
  ((SoInternalGLState*)(void*)&((_ptr_)->glstate))

#define OPENCACHEFLAGS(_ptr_) \
  (_ptr_)->forcediffuse


SO_ELEMENT_SOURCE(SoGLLazyElement);

/*!
  This static method initializes static data for the
  SoDiffuseColorElement class.
*/

void
SoGLLazyElement::initClass()
{
  // assert that the GLState hack is safe
  assert(sizeof(SoInternalGLState) <= sizeof(GLState));

  SO_ELEMENT_INIT_CLASS(SoGLLazyElement, inherited);

  // create stipple patterns
  int i;
  uint32_t matrix[32*32];
  make_dither_matrix((uint32_t*)matrix, 32);
  for (i = 0; i <= 64; i++) {
    int intensity = (32 * 32 * i) / 64 - 1;
    create_matrix_bitmap((intensity >= 0) ? intensity : 0,
                         stipple_patterns[i], (uint32_t*)matrix, 32);
  }
}

// ! FIXME: write doc

SoGLLazyElement::~SoGLLazyElement()
{
}

//! FIXME: write doc
SoGLLazyElement *
SoGLLazyElement::getInstance(const SoState *state)
{
  return (SoGLLazyElement*)
    state->getConstElement(classStackIndex);
}

inline void
SoGLLazyElement::sendPackedDiffuse(const uint32_t col) const
{
  glColor4ub((col>>24)&0xff, (col>>16)&0xff, (col>>8)&0xff, col&0xff);
  UPDATE_GLSTATE(this, diffuse, col);
  ((SoGLLazyElement*)this)->cachebitmask |= DIFFUSE_MASK;
}

inline void
SoGLLazyElement::sendLightModel(const int32_t model) const
{
  if (model == PHONG) glEnable(GL_LIGHTING);
  else glDisable(GL_LIGHTING);
  UPDATE_GLSTATE(this, lightmodel, model);
  ((SoGLLazyElement*)this)->cachebitmask |= LIGHT_MODEL_MASK;
}

inline void
SoGLLazyElement::sendFlatshading(const SbBool onoff) const
{
  if (onoff) glShadeModel(GL_FLAT);
  else glShadeModel(GL_SMOOTH);
  UPDATE_GLSTATE(this, flatshading, (int32_t)onoff);
  ((SoGLLazyElement*)this)->cachebitmask |= SHADE_MODEL_MASK;
}

void
SoGLLazyElement::sendGLImage(const uint32_t glimageid) const
{
  // sentimageid is set to 0 if texturing isn't enabled
  uint32_t sentimageid = glimageid;

  if (glimageid != 0) {
    sentimageid = 0;

    SoTextureImageElement::Model model;
    SbColor blendcolor;
    SoGLImage * glimage = SoGLTextureImageElement::get(this->state, model, blendcolor);

    if (glimage) {
      SbBool enabled =
        SoGLTextureEnabledElement::get(this->state) ||
        SoGLTexture3EnabledElement::get(this->state);
      // image data might be loaded on demand (SoVRMLImageTexture).
      // We trigger a load here by attempting to fetch the image data.
      if (!enabled && glimage->getImage()) {
        SbVec3s size;
        int nc;
        (void) glimage->getImage()->getValue(size, nc);
      }
      if (enabled) {
        SoGLDisplayList * dl = glimage->getGLDisplayList(this->state);
        if (dl) {
          // tag image (for GLImage LRU cache).
          SoGLImage::tagImage(this->state, glimage);
          if (SoTextureCombineElement::isDefault(this->state, 0)) {
            switch (model) {
            case SoTextureImageElement::DECAL:
              glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
              break;
            case SoTextureImageElement::MODULATE:
              glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
              break;
            case SoTextureImageElement::BLEND:
              glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
              glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, blendcolor.getValue());
              break;
            case SoTextureImageElement::REPLACE:
              // GL_REPLACE mode was introduced with OpenGL 1.1. It is
              // considered the client code's responsibility to check
              // that it can use this mode.
              //
              // FIXME: ..but we should do a sanity check anyway.
              // 20030901 mortene.
              glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
              break;
            default:
              assert(0 && "unknown model");
              break;
            }
          }
          else {
            SoTextureCombineElement::apply(this->state, 0);
          }
          dl->call(this->state);
          sentimageid = glimageid;
        }
      }
    }
  }
  ((SoGLLazyElement*)this)->cachebitmask |= GLIMAGE_MASK;
  UPDATE_GLSTATE(this, glimageid, (int32_t) sentimageid);
}

inline void
SoGLLazyElement::sendAlphaTest(const SbBool onoff) const
{
  if (onoff) {
    glAlphaFunc(GL_GREATER, 0.5f);
    glEnable(GL_ALPHA_TEST);
  }
  else {
    glDisable(GL_ALPHA_TEST);
  }
  ((SoGLLazyElement*)this)->cachebitmask |= ALPHATEST_MASK;
  UPDATE_GLSTATE(this, alphatest, onoff);
}


inline void
SoGLLazyElement::sendVertexOrdering(const VertexOrdering ordering) const
{
  glFrontFace(ordering == CW ? GL_CW : GL_CCW);
  UPDATE_GLSTATE(this, vertexordering, (int32_t) ordering);
  ((SoGLLazyElement*)this)->cachebitmask |= VERTEXORDERING_MASK;
}

inline void
SoGLLazyElement::sendTwosideLighting(const SbBool onoff) const
{
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, onoff ? GL_TRUE : GL_FALSE);
  UPDATE_GLSTATE(this,twoside, (int32_t) onoff);
  ((SoGLLazyElement*)this)->cachebitmask |= TWOSIDE_MASK;
}

inline void
SoGLLazyElement::sendBackfaceCulling(const SbBool onoff) const
{
  if (onoff) glEnable(GL_CULL_FACE);
  else glDisable(GL_CULL_FACE);
  UPDATE_GLSTATE(this, culling, onoff);
  ((SoGLLazyElement*)this)->cachebitmask |= CULLING_MASK;
}

static inline void
send_gl_material(GLenum pname, const SbColor & color)
{
  GLfloat col[4];
  color.getValue(col[0], col[1], col[2]);
  col[3] = 1.0f;
  glMaterialfv(GL_FRONT_AND_BACK, pname, col);
}


inline void
SoGLLazyElement::sendAmbient(const SbColor & color) const
{
  send_gl_material(GL_AMBIENT, color);
  UPDATE_GLSTATE(this, ambient, color);
  ((SoGLLazyElement*)this)->cachebitmask |= AMBIENT_MASK;
}

inline void
SoGLLazyElement::sendEmissive(const SbColor & color) const
{
  send_gl_material(GL_EMISSION, color);
  UPDATE_GLSTATE(this, emissive, color);
  ((SoGLLazyElement*)this)->cachebitmask |= EMISSIVE_MASK;
}

inline void
SoGLLazyElement::sendSpecular(const SbColor & color) const
{
  send_gl_material(GL_SPECULAR, color);
  UPDATE_GLSTATE(this, specular, color);
  ((SoGLLazyElement*)this)->cachebitmask |= SPECULAR_MASK;
}

inline void
SoGLLazyElement::sendShininess(const float shine) const
{
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine*128.0f);
  UPDATE_GLSTATE(this, shininess, shine);
  ((SoGLLazyElement*)this)->cachebitmask |= SHININESS_MASK;
}

inline void
SoGLLazyElement::sendTransparency(const int stipplenum) const
{
  if (stipplenum == 0) {
    glDisable(GL_POLYGON_STIPPLE);
  }
  else {
    if (GLSTATE(this)->stipplenum <= 0) glEnable(GL_POLYGON_STIPPLE);
    glPolygonStipple(stipple_patterns[stipplenum]);
  }
  UPDATE_GLSTATE(this, stipplenum, stipplenum);
  ((SoGLLazyElement*)this)->cachebitmask |= TRANSPARENCY_MASK;
}

inline void
SoGLLazyElement::enableBlending(const int sfactor, const int dfactor) const
{
  glEnable(GL_BLEND);
  glBlendFunc((GLenum) sfactor, (GLenum) dfactor);
  UPDATE_GLSTATE(this, blending, TRUE);
  UPDATE_GLSTATE(this, blend_sfactor, sfactor);
  UPDATE_GLSTATE(this, blend_dfactor, dfactor);  
  ((SoGLLazyElement*)this)->cachebitmask |= BLENDING_MASK;
}

inline void
SoGLLazyElement::disableBlending(void) const
{
  glDisable(GL_BLEND);
  UPDATE_GLSTATE(this, blending , FALSE);
  ((SoGLLazyElement*)this)->cachebitmask |= BLENDING_MASK;
}

void
SoGLLazyElement::init(SoState * stateptr)
{
  inherited::init(stateptr);
  this->state = stateptr; // needed to send GL texture
  this->colorindex = FALSE;
  GLSTATE(this)->ambient.setValue(-1.0f, -1.0f, -1.0f);
  GLSTATE(this)->emissive.setValue(-1.0f, -1.0f, -1.0f);
  GLSTATE(this)->specular.setValue(-1.0f, -1.0f, -1.0f);
  GLSTATE(this)->shininess = -1.0f;
  GLSTATE(this)->lightmodel = -1;
  GLSTATE(this)->blending = -1;
  GLSTATE(this)->blend_sfactor = -1;
  GLSTATE(this)->blend_dfactor = -1;
  GLSTATE(this)->stipplenum = -1;
  GLSTATE(this)->vertexordering = -1;
  GLSTATE(this)->twoside = -1;
  GLSTATE(this)->culling = -1;
  GLSTATE(this)->flatshading = -1;
  GLSTATE(this)->glimageid = -1;
  GLSTATE(this)->alphatest = -1;
  GLSTATE(this)->diffuse = 0xccccccff;
  this->packedpointer = NULL;
  this->transpmask = 0xff;
  this->colorpacker = NULL;
  this->precachestate = NULL;
  this->postcachestate = NULL;
  OPENCACHEFLAGS(this) = 0;

  // initialize this here to avoid UMR reports from
  // Purify. cachebitmask is updated even when there are no open
  // caches. It is only used (and properly initialized) when recording
  // a cache though.
  this->cachebitmask = 0;

  glDisable(GL_POLYGON_STIPPLE);
  GLboolean rgba;
  glGetBooleanv(GL_RGBA_MODE, &rgba);
  if (!rgba) this->colorindex = TRUE;
  else {
    this->sendPackedDiffuse(0xccccccff);
  }
}

void
SoGLLazyElement::push(SoState * stateptr)
{
  inherited::push(stateptr);
  SoGLLazyElement * prev = (SoGLLazyElement*) this->getNextInStack();
  this->state = stateptr; // needed to send GL texture
  memcpy(GLSTATE(this), GLSTATE(prev), sizeof(SoInternalGLState));
  this->colorindex = prev->colorindex;
  this->transpmask = prev->transpmask;
  this->colorpacker = prev->colorpacker;
  this->precachestate = prev->precachestate;
  this->postcachestate = prev->postcachestate;
  this->didsetbitmask = prev->didsetbitmask;
  this->didntsetbitmask = prev->didntsetbitmask;
  this->cachebitmask = prev->cachebitmask;
  OPENCACHEFLAGS(this) = OPENCACHEFLAGS(prev);
}

void
SoGLLazyElement::pop(SoState *stateptr, const SoElement * prevtopelement)
{
  inherited::pop(stateptr, prevtopelement);
  SoGLLazyElement * prev = (SoGLLazyElement*) prevtopelement;
  memcpy(GLSTATE(this), GLSTATE(prev), sizeof(SoInternalGLState));
  this->colorindex = prev->colorindex;
  this->didsetbitmask = prev->didsetbitmask;
  this->didntsetbitmask = prev->didntsetbitmask;
  this->cachebitmask = prev->cachebitmask;
  OPENCACHEFLAGS(this) = OPENCACHEFLAGS(prev);
}

//! FIXME: write doc

void
SoGLLazyElement::sendAllMaterial(SoState * state)
{
  SoGLLazyElement * elem = getInstance(state);
  elem->send(state, ALL_MASK);
}

//! FIXME: write doc

void
SoGLLazyElement::sendNoMaterial(SoState * state)
{
  SoGLLazyElement * elem = getInstance(state);
  elem->send(state, NO_COLOR_MASK);
}

//! FIXME: write doc

void
SoGLLazyElement::sendOnlyDiffuseColor(SoState * state)
{
  SoGLLazyElement * elem = getInstance(state);
  elem->send(state, DIFFUSE_ONLY_MASK);
}

//! FIXME: write doc

void
SoGLLazyElement::sendDiffuseByIndex(const int index) const
{
  int safeindex = index;
#if COIN_DEBUG
  if (index < 0 || index >= this->coinstate.numdiffuse) {
    static int first = 1;
    if (first) {
      SoAction * action = this->state->getAction();
      SoFullPath * path = (SoFullPath*) this->state->getAction()->getCurPath();
      SoNode * tail = path->getTail();
      SbName name = tail->getName();
      SoDebugError::postWarning("SoGLLazyElement::sendDiffuseByIndex",
                                "index %d out of bounds [0, %d] in node %p: %s "
                                "(this warning will only be printed once, but there "
                                "might be more errors)",
                                index,
                                this->coinstate.numdiffuse-1,
                                tail, name != SbName("") ? name.getString() : "<noname>");
      first = 0;
    }

    safeindex = SbClamp((long) index, (long) 0, (long) (this->coinstate.numdiffuse-1));
  }
#endif // COIN_DEBUG

  if (this->colorindex) {
    glIndexi((GLint)this->coinstate.colorindexarray[safeindex]);
  }
  else {
    uint32_t col = this->packedpointer[safeindex] | this->transpmask;
    // this test is really not necessary. SoMaterialBundle does the
    // same test.  We also need to send the color here to work around
    // an nVIDIA bug
    // if (col != GLSTATE(this)->diffuse) 
    this->sendPackedDiffuse(col);
  }
}

//! FIXME: write doc

SbBool
SoGLLazyElement::isColorIndex(SoState * state)
{
  SoGLLazyElement * elem = getInstance(state);
  return elem->colorindex;
}

//! FIXME: write doc

void
SoGLLazyElement::send(const SoState * stateptr, uint32_t mask) const
{
  SoInternalGLState * theprecachestate = (SoInternalGLState*)
    this->precachestate;

  if (this->colorpacker) {
    if (!this->colorpacker->diffuseMatch(this->coinstate.diffusenodeid) ||
        !this->colorpacker->transpMatch(this->coinstate.transpnodeid)) {
      this->packColors(this->colorpacker);
    }
    ((SoGLLazyElement*)this)->packedpointer = this->colorpacker->getPackedColors();
  }
  else ((SoGLLazyElement*)this)->packedpointer = this->coinstate.packedarray;

  assert(this->packedpointer);

  int stipplenum;

  for (int i = 0; (i < LAZYCASES_LAST)&&mask; i++, mask>>=1) {
    if (mask&1) {
      switch (i) {
      case LIGHT_MODEL_CASE:
        if (this->coinstate.lightmodel != GLSTATE(this)->lightmodel) {
          SoGLShaderProgram * prog = SoGLShaderProgramElement::get((SoState*) stateptr);
          if (prog) prog->updateCoinParameter((SoState*)stateptr, SbName("coin_light_model"), this->coinstate.lightmodel);
          this->sendLightModel(this->coinstate.lightmodel);
        }
        break;
      case DIFFUSE_CASE:
        if (theprecachestate) {
          // we are currently building a cache. Check if we're using
          // colors from a material node outside the cache.
          if ((theprecachestate->diffusenodeid == this->coinstate.diffusenodeid) ||
              (theprecachestate->transpnodeid == this->coinstate.transpnodeid)) {
            OPENCACHEFLAGS((SoGLLazyElement*)this) |= FLAG_DIFFUSE_DEPENDENCY;
          }
        }
        if (OPENCACHEFLAGS(this) & FLAG_FORCE_DIFFUSE) {
          // we always send the first diffuse color for the first
          // material in an open cache
          if (this->colorindex) {
            glIndexi((GLint)this->coinstate.colorindexarray[0]);
          }
          else {
            this->sendPackedDiffuse(this->packedpointer[0]|this->transpmask);
          }
          OPENCACHEFLAGS((SoGLLazyElement*)this) &= ~FLAG_FORCE_DIFFUSE;
        }
        else {
          this->sendDiffuseByIndex(0);
        }
        break;
      case AMBIENT_CASE:
        if (this->coinstate.ambient != GLSTATE(this)->ambient) {
          this->sendAmbient(this->coinstate.ambient);
        }
        break;
      case SPECULAR_CASE:
        if (this->coinstate.specular != GLSTATE(this)->specular) {
          this->sendSpecular(this->coinstate.specular);
        }
        break;
      case EMISSIVE_CASE:
        if (this->coinstate.emissive != GLSTATE(this)->emissive) {
          this->sendEmissive(this->coinstate.emissive);
        }
        break;
      case SHININESS_CASE:
        if (this->coinstate.shininess != GLSTATE(this)->shininess) {
          this->sendShininess(this->coinstate.shininess);
        }
        break;
      case BLENDING_CASE:
        if (this->coinstate.blending) {
          if (GLSTATE(this)->blending != this->coinstate.blending || 
              this->coinstate.blend_sfactor != GLSTATE(this)->blend_sfactor ||
              this->coinstate.blend_dfactor != GLSTATE(this)->blend_dfactor) {
            this->enableBlending(this->coinstate.blend_sfactor, this->coinstate.blend_dfactor);
          }
        }
        else {
          if (this->coinstate.blending != GLSTATE(this)->blending) {
            this->disableBlending();
          }
        }
        break;
      case TRANSPARENCY_CASE:
        stipplenum =
          this->coinstate.transptype == SoGLRenderAction::SCREEN_DOOR ?
          this->coinstate.stipplenum : 0;

        if (stipplenum != GLSTATE(this)->stipplenum) {
          this->sendTransparency(stipplenum);
        }
        break;
      case VERTEXORDERING_CASE:
        if (GLSTATE(this)->vertexordering != this->coinstate.vertexordering) {
          this->sendVertexOrdering(this->coinstate.vertexordering);
        }
        break;
      case CULLING_CASE:
        if (GLSTATE(this)->culling != this->coinstate.culling) {
          this->sendBackfaceCulling(this->coinstate.culling);
        }
        break;
      case TWOSIDE_CASE:
        if (GLSTATE(this)->twoside != this->coinstate.twoside) {
          this->sendTwosideLighting(this->coinstate.twoside);
        }
        break;
      case SHADE_MODEL_CASE:
        if (GLSTATE(this)->flatshading != this->coinstate.flatshading) {
          this->sendFlatshading(this->coinstate.flatshading);
        }
        break;
      case GLIMAGE_CASE:
        if (GLSTATE(this)->glimageid != (int32_t) this->coinstate.glimageid) {
          this->sendGLImage(this->coinstate.glimageid);
        }
        break;
      case ALPHATEST_CASE:
        if (GLSTATE(this)->alphatest != (int32_t) this->coinstate.alphatest) {
          this->sendAlphaTest(this->coinstate.alphatest);
        }
        break;
      }

    }
  }
}

//! FIXME: write doc

void
SoGLLazyElement::sendVPPacked(SoState* stateptr, const unsigned char* pcolor)
{
  assert(0 && "Not implemented yet. Provided for API compatibility.");
}

/*!
  Reset element GL state (set state to invalid). Use this method to
  notify this element when you use your own GL code that changes the
  OpenGL state.
*/
void
SoGLLazyElement::reset(SoState * stateptr,  uint32_t mask) const
{
  SoGLLazyElement * elem = getInstance(stateptr);

  if (stateptr->isCacheOpen()) {
    elem->cachebitmask |= mask;
  }

  for (int i = 0; (i < LAZYCASES_LAST)&&mask; i++, mask>>=1) {
    if (mask&1) {
      switch (i) {
      case LIGHT_MODEL_CASE:
        GLSTATE(elem)->lightmodel = -1;
        break;
      case DIFFUSE_CASE:
        elem->sendPackedDiffuse(0xccccccff);
        break;
      case AMBIENT_CASE:
        GLSTATE(elem)->ambient = SbColor(-1.f, -1.0f, -1.0f);
        break;
      case SPECULAR_CASE:
        GLSTATE(elem)->specular = SbColor(-1.0f, -1.0f, -1.0f);
        break;
      case EMISSIVE_CASE:
        GLSTATE(elem)->emissive = SbColor(-1.0f, -1.0f, -1.0f);
        break;
      case SHININESS_CASE:
        GLSTATE(elem)->shininess = -1.0f;
        break;
      case BLENDING_CASE:
        GLSTATE(elem)->blending = -1;
        GLSTATE(elem)->blend_sfactor = -1;
        GLSTATE(elem)->blend_dfactor = -1;
        break;
      case TRANSPARENCY_CASE:
        GLSTATE(elem)->stipplenum = -1;
        break;
      case VERTEXORDERING_CASE:
        GLSTATE(elem)->vertexordering = -1;
        break;
      case CULLING_CASE:
        GLSTATE(elem)->culling = -1;
        break;
      case TWOSIDE_CASE:
        GLSTATE(elem)->twoside = -1;
        break;
      case SHADE_MODEL_CASE:
        GLSTATE(elem)->flatshading = -1;
        break;
      case GLIMAGE_CASE:
        GLSTATE(elem)->glimageid = -1;
        break;
      case ALPHATEST_CASE:
        GLSTATE(elem)->alphatest = -1;
        break;
      }
    }
  }
}

void
SoGLLazyElement::sendPackedDiffuse(SoState * state, const uint32_t diffuse)
{
  SbBool cacheopen = state->isCacheOpen();
  SoGLLazyElement * elem = getInstance(state);
  if (GLSTATE(elem)->diffuse != diffuse) {
    elem->sendPackedDiffuse(diffuse);
    if (cacheopen) elem->lazyDidSet(DIFFUSE_MASK|TRANSPARENCY_MASK);
  }
  else if (cacheopen) {
    elem->lazyDidntSet(DIFFUSE_MASK|TRANSPARENCY_MASK);
  }
}

void
SoGLLazyElement::sendLightModel(SoState * state, const int32_t model)
{
  SbBool cacheopen = state->isCacheOpen();
  SoGLLazyElement * elem = getInstance(state);
  if (GLSTATE(elem)->lightmodel != model) {
    elem->sendLightModel(model);
    if (cacheopen) elem->lazyDidSet(LIGHT_MODEL_MASK);
  }
  else if (cacheopen) {
    elem->lazyDidntSet(LIGHT_MODEL_MASK);
  }
}

void
SoGLLazyElement::sendFlatshading(SoState * state, const SbBool onoff)
{
  SbBool cacheopen = state->isCacheOpen();
  SoGLLazyElement * elem = getInstance(state);
  if (GLSTATE(elem)->flatshading != onoff) {
    elem->sendFlatshading(onoff);
    if (cacheopen) elem->lazyDidSet(SHADE_MODEL_MASK);
  }
  else if (cacheopen) {
    elem->lazyDidntSet(SHADE_MODEL_MASK);
  }
}

void
SoGLLazyElement::sendVertexOrdering(SoState * state, const VertexOrdering ordering)
{
  SbBool cacheopen = state->isCacheOpen();
  SoGLLazyElement * elem = getInstance(state);
  if (GLSTATE(elem)->vertexordering != (int32_t) ordering) {
    elem->sendVertexOrdering(ordering);
    if (cacheopen) elem->lazyDidSet(VERTEXORDERING_MASK);
  }
  else if (cacheopen) {
    elem->lazyDidntSet(VERTEXORDERING_MASK);
  }
}

void
SoGLLazyElement::sendTwosideLighting(SoState * state, const SbBool onoff)
{
  SbBool cacheopen = state->isCacheOpen();
  SoGLLazyElement * elem = getInstance(state);
  if (GLSTATE(elem)->twoside != (int32_t) onoff) {
    elem->sendTwosideLighting(onoff);
    if (cacheopen) elem->lazyDidSet(TWOSIDE_MASK);
  }
  else if (cacheopen) {
    elem->lazyDidntSet(TWOSIDE_MASK);
  }
}

void
SoGLLazyElement::sendBackfaceCulling(SoState * state, const SbBool onoff)
{
  SbBool cacheopen = state->isCacheOpen();
  SoGLLazyElement * elem = getInstance(state);
  if (GLSTATE(elem)->culling != (int32_t) onoff) {
    elem->sendBackfaceCulling(onoff);
    if (cacheopen) elem->lazyDidSet(CULLING_MASK);
  }
  else if (cacheopen) {
    elem->lazyDidntSet(CULLING_MASK);
  }
}

void
SoGLLazyElement::setDiffuseElt(SoNode * node,  int32_t numcolors,
                               const SbColor * colors, SoColorPacker * packer)
{
  inherited::setDiffuseElt(node, numcolors, colors, packer);
  this->colorpacker = packer;
}

void
SoGLLazyElement::setPackedElt(SoNode * node, int32_t numcolors,
                              const uint32_t * colors, const SbBool packedtransparency)
{
  inherited::setPackedElt(node, numcolors, colors, packedtransparency);
  this->colorpacker = NULL;
  this->packedpointer = colors;
}

void
SoGLLazyElement::setColorIndexElt(SoNode * node, int32_t numindices,
                                  const int32_t * indices)
{
  inherited::setColorIndexElt(node, numindices, indices);
}

void
SoGLLazyElement::setTranspElt(SoNode * node, int32_t numtransp,
                              const float * transp, SoColorPacker * packer)
{
  inherited::setTranspElt(node, numtransp, transp, packer);
  this->colorpacker = packer;
}


void
SoGLLazyElement::setTranspTypeElt(int32_t type)
{
  inherited::setTranspTypeElt(type);
  this->transpmask = type == SoGLRenderAction::SCREEN_DOOR ? 0xff : 0x00;
}

void
SoGLLazyElement::setAmbientElt(const SbColor* color)
{
  inherited::setAmbientElt(color);
}

void
SoGLLazyElement::setEmissiveElt(const SbColor* color)
{
  inherited::setEmissiveElt(color);
}

void
SoGLLazyElement::setSpecularElt(const SbColor* color)
{
  inherited::setSpecularElt(color);
}

void
SoGLLazyElement::setShininessElt(float value)
{
  inherited::setShininessElt(value);
}

void
SoGLLazyElement::setColorMaterialElt(SbBool value)
{
  inherited::setColorMaterialElt(value);
}

void
SoGLLazyElement::enableBlendingElt(int sfactor, int dfactor)
{
  inherited::enableBlendingElt(sfactor, dfactor);
}

void
SoGLLazyElement::disableBlendingElt(void)
{
  inherited::disableBlendingElt();
}

void
SoGLLazyElement::setLightModelElt(SoState * stateptr, int32_t model)
{
  inherited::setLightModelElt(stateptr, model);
}

void
SoGLLazyElement::setMaterialElt(SoNode * node, uint32_t bitmask,
                                SoColorPacker * packer,
                                const SbColor * diffuse, const int numdiffuse,
                                const float * transp, const int numtransp,
                                const SbColor & ambient,
                                const SbColor & emissive,
                                const SbColor & specular,
                                const float shininess,
                                const SbBool istransparent)
{
  inherited::setMaterialElt(node, bitmask,
                            packer, diffuse, numdiffuse,
                            transp, numtransp, ambient,
                            emissive, specular, shininess, istransparent);
  this->colorpacker = packer;
}

void
SoGLLazyElement::setVertexOrderingElt(VertexOrdering ordering)
{
  inherited::setVertexOrderingElt(ordering);
}

void
SoGLLazyElement::setBackfaceCullingElt(SbBool onoff)
{
  inherited::setBackfaceCullingElt(onoff);
}

void
SoGLLazyElement::setTwosideLightingElt(SbBool onoff)
{
  inherited::setTwosideLightingElt(onoff);
}

void
SoGLLazyElement::setShadeModelElt(SbBool flatshading)
{
  inherited::setShadeModelElt(flatshading);
}

void
SoGLLazyElement::setGLImageIdElt(uint32_t glimageid, SbBool alphatest)
{
  inherited::setGLImageIdElt(glimageid, alphatest);
}

void
SoGLLazyElement::setAlphaTestElt(SbBool onoff)
{
  inherited::setAlphaTestElt(onoff);
}

void
SoGLLazyElement::packColors(SoColorPacker * packer) const
{
  const int n = this->coinstate.numdiffuse;
  const SbColor * diffuse = this->coinstate.diffusearray;
  const int numtransp = this->coinstate.numtransp;
  const float * transp = this->coinstate.transparray;

  if (packer->getSize() < n) packer->reallocate(n);
  uint32_t * ptr = packer->getPackedColors();

  int ti = 0;

  for (int i = 0; i < n; i++) {
    ptr[i] = diffuse[i].getPackedValue(transp[ti]);
    if (ti < numtransp-1) ti++;
  }

  packer->setNodeIds(this->coinstate.diffusenodeid,
                     this->coinstate.transpnodeid);
}

void
SoGLLazyElement::beginCaching(SoState * state, GLState * prestate,
                              GLState * poststate)
{
  SoGLLazyElement * elem = getInstance(state);
  elem->send(state, ALL_MASK); // send lazy state before starting to build cache
  memcpy(prestate, GLSTATE(elem), sizeof(SoInternalGLState));
  ((SoInternalGLState*)prestate)->diffusenodeid = elem->coinstate.diffusenodeid;
  ((SoInternalGLState*)prestate)->transpnodeid = elem->coinstate.transpnodeid;

  elem->precachestate = prestate;
  elem->postcachestate = poststate;
  ((SoInternalGLState*)elem->precachestate)->cachebitmask = 0;
  ((SoInternalGLState*)elem->postcachestate)->cachebitmask = 0;
  
  elem->didsetbitmask = 0;
  elem->didntsetbitmask = 0;
  elem->cachebitmask = 0;
  OPENCACHEFLAGS(elem) = 0;
}

void
SoGLLazyElement::endCaching(SoState * state)
{
  SoGLLazyElement * elem = getInstance(state);
  
  SoInternalGLState * precachestate = (SoInternalGLState*) elem->precachestate;
  SoInternalGLState * postcachestate = (SoInternalGLState*) elem->postcachestate;
  
  memcpy(elem->postcachestate, GLSTATE(elem), sizeof(SoInternalGLState));
  postcachestate->cachebitmask = elem->cachebitmask;
  precachestate->cachebitmask = elem->didntsetbitmask;

  // unset diffuse mask since it's used by the dependency test
  precachestate->cachebitmask &= ~DIFFUSE_MASK;

  // set diffuse mask if this cache depends on a material outside the
  // cache.
  if (OPENCACHEFLAGS(elem) & FLAG_DIFFUSE_DEPENDENCY) {
    precachestate->cachebitmask |= DIFFUSE_MASK;
  }

  elem->precachestate = NULL;
  elem->postcachestate = NULL;
  OPENCACHEFLAGS(elem) = 0;
}

void
SoGLLazyElement::postCacheCall(SoState * state, GLState * poststate_ptr)
{
  SoInternalGLState * poststate = (SoInternalGLState*) poststate_ptr;
  SoGLLazyElement * elem = getInstance(state);
  uint32_t mask = poststate->cachebitmask;

  for (int i = 0; (i < LAZYCASES_LAST)&&mask; i++, mask>>=1) {
    if (mask&1) {
      switch (i) {
      case LIGHT_MODEL_CASE:
        GLSTATE(elem)->lightmodel = poststate->lightmodel;
        break;
      case DIFFUSE_CASE:
        GLSTATE(elem)->diffuse = poststate->diffuse;
        break;
      case AMBIENT_CASE:
        GLSTATE(elem)->ambient = poststate->ambient;
        break;
      case SPECULAR_CASE:
        GLSTATE(elem)->specular = poststate->specular;
        break;
      case EMISSIVE_CASE:
        GLSTATE(elem)->emissive = poststate->emissive;
        break;
      case SHININESS_CASE:
        GLSTATE(elem)->shininess = poststate->shininess;
        break;
      case BLENDING_CASE:
        GLSTATE(elem)->blending = poststate->blending;
        GLSTATE(elem)->blend_sfactor = poststate->blend_sfactor;
        GLSTATE(elem)->blend_dfactor = poststate->blend_dfactor;
        break;
      case TRANSPARENCY_CASE:
        GLSTATE(elem)->stipplenum = poststate->stipplenum;
        break;
      case VERTEXORDERING_CASE:
        GLSTATE(elem)->vertexordering = poststate->vertexordering;
        break;
      case CULLING_CASE:
        GLSTATE(elem)->culling = poststate->culling;
        break;
      case TWOSIDE_CASE:
        GLSTATE(elem)->twoside = poststate->twoside;
        break;
      case SHADE_MODEL_CASE:
        GLSTATE(elem)->flatshading = poststate->flatshading;
        break;
      case GLIMAGE_CASE:
        GLSTATE(elem)->glimageid = poststate->glimageid;
        break;
      case ALPHATEST_CASE:
        GLSTATE(elem)->alphatest = poststate->alphatest;
        break;
      }
    }
  }
}

SbBool
SoGLLazyElement::preCacheCall(SoState * state, GLState * prestate_ptr)
{
  SoInternalGLState * prestate = (SoInternalGLState*) prestate_ptr;
  SoGLLazyElement * elem = getInstance(state);

  struct CoinState & curr = elem->coinstate;
  uint32_t mask = prestate->cachebitmask;

  for (int i = 0; (i < LAZYCASES_LAST)&&mask; i++, mask>>=1) {
    if (mask&1) {
      switch (i) {
      case LIGHT_MODEL_CASE:
        if (curr.lightmodel != prestate->lightmodel) {
          GLLAZY_DEBUG("light model failed");
          return FALSE;
        }
        break;
      case DIFFUSE_CASE:
        // this is a special case, since we can have multiple diffuse
        // and transparency values. Check the node ids.
        if ((prestate->diffusenodeid != curr.diffusenodeid) ||
            (prestate->transpnodeid != curr.transpnodeid)) {
          GLLAZY_DEBUG("material dependency failed");
          return FALSE;
        }
        break;
      case AMBIENT_CASE:
        if (curr.ambient != prestate->ambient) {
          GLLAZY_DEBUG("ambient failed");
          return FALSE;
        }
        break;
      case SPECULAR_CASE:
        if (curr.specular != prestate->specular) {
          GLLAZY_DEBUG("specular failed");
          return FALSE;
        }
        break;
      case EMISSIVE_CASE:
        if (curr.emissive != prestate->emissive) {
          GLLAZY_DEBUG("emissive failed");
          return FALSE;
        }
        break;
      case SHININESS_CASE:
        if (curr.shininess != prestate->shininess) {
          GLLAZY_DEBUG("shininess failed");
          return FALSE;
        }
        break;
      case BLENDING_CASE:
        if (curr.blending != prestate->blending) {
          GLLAZY_DEBUG("blending failed");
          return FALSE;
        }
        if (prestate->blending) {
          if (curr.blend_sfactor != prestate->blend_sfactor ||
              curr.blend_dfactor != prestate->blend_dfactor) {
            GLLAZY_DEBUG("blending failed");
            return FALSE;
          }
        }
        break;
      case TRANSPARENCY_CASE:
        if (curr.stipplenum != prestate->stipplenum) {
          GLLAZY_DEBUG("transparency failed");
          return FALSE;
        }
        break;
      case VERTEXORDERING_CASE:
        if (curr.vertexordering != prestate->vertexordering) {
          GLLAZY_DEBUG("vertexordering failed");
          return FALSE;
        }
        break;
      case CULLING_CASE:
        if (curr.culling != prestate->culling) {
          GLLAZY_DEBUG("culling failed");
          return FALSE;
        }
        break;
      case TWOSIDE_CASE:
        if (curr.twoside != prestate->twoside) {
          GLLAZY_DEBUG("twoside failed");
          return FALSE;
        }
        break;
      case SHADE_MODEL_CASE:
        if (curr.flatshading != prestate->flatshading) {
          GLLAZY_DEBUG("shade model failed");
          return FALSE;
        }
        break;
      case GLIMAGE_CASE:
        if ((int32_t) curr.glimageid != prestate->glimageid) {
          GLLAZY_DEBUG("glimageid failed");
          return FALSE;
        }
        break;
      case ALPHATEST_CASE:
        if (curr.alphatest != prestate->alphatest) {
          GLLAZY_DEBUG("alphatest failed");
          return FALSE;
        }
        break;
      }
    }
  }
  return TRUE;
}


void
SoGLLazyElement::lazyDidSet(uint32_t mask)
{
  if (mask & DIFFUSE_MASK) {
    if (!(this->didsetbitmask & DIFFUSE_MASK)) {
      // to be safe, always send first diffuse when a cache is open
      OPENCACHEFLAGS(this) |= FLAG_FORCE_DIFFUSE;
    }
  }
  this->didsetbitmask |= mask;
}

void
SoGLLazyElement::lazyDidntSet(uint32_t mask)
{
  if (mask & DIFFUSE_MASK) {
    if (!(this->didsetbitmask & DIFFUSE_MASK)) {
      // to be safe, always send first diffuse when a cache is open
      this->didsetbitmask |= DIFFUSE_MASK;
      OPENCACHEFLAGS(this) |= FLAG_FORCE_DIFFUSE;
    }
  }
  this->didntsetbitmask |= mask&(~this->didsetbitmask);
}

void 
SoGLLazyElement::updateColorVBO(SoVBO * vbo)
{
  if (this->colorpacker) {
    uint32_t maxid = this->colorpacker->getDiffuseId();
    uint32_t tid = this->colorpacker->getTranspId();
    if (tid > maxid) {
      maxid = tid;
    }
    uint32_t vboid = vbo->getBufferDataId();
    if (vboid != maxid) {
      const int n = this->coinstate.numdiffuse;
      // need to update the VBO
      const uint32_t * src = this->colorpacker->getPackedColors();
      if (coin_host_get_endianness() == COIN_HOST_IS_BIGENDIAN) {
        vbo->setBufferData(src, n * sizeof(uint32_t),
                           maxid);
      }
      else {
        uint32_t * dst = (uint32_t*)
          vbo->allocBufferData(n * sizeof(uint32_t),
                               maxid);
        for (int i = 0; i < n; i++) {
          uint32_t tmp = src[i];
          dst[i] = 
            (tmp << 24) |
            ((tmp & 0xff00) << 8) |
            ((tmp & 0xff0000) >> 8) |
            (tmp >> 24);
        }
      }
    }
  }
}

#undef UPDATE_GLSTATE
#undef GLSTATE
#undef OPENCACHEFLAGS
#undef FLAG_FORCE_DIFFUSE
#undef FLAG_DIFFUSE_DEPENDENCY


