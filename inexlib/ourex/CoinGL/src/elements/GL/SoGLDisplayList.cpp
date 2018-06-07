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
  \class SoGLDisplayList Inventor/elements/SoGLCacheContextElement.h
  \brief The SoGLDisplayList class stores and manages OpenGL display lists.
  \ingroup elements

  The TEXTURE_OBJECT type is not directly supported in Coin. We handle
  textures differently in a more flexible class called SoGLImage,
  which also stores some information about the texture used when
  rendering. Old code which use this element should not stop
  working though. The texture object extension will just not be used,
  and the texture will be stored in a display list instead.
*/

// *************************************************************************

#include <Inventor/elements/SoGLDisplayList.h>

#include <string.h>
#include <assert.h>

#include <Inventor/C/glue/gl.h>
#include <Inventor/caches/SoGLRenderCache.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/elements/SoGLTexture3EnabledElement.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/misc/SoGL.h>

// *************************************************************************

// HACK WARNING: Please note that the "type" member is also used to store
// the texture target for a texture object, so it's only safe to check
// if "type" != DISPLAY_LIST. This is fixed properly in Coin-dev (using
// a pimpl implementation). pederb, 2005-10-18

// *************************************************************************

/*!
  Constructor.
*/
SoGLDisplayList::SoGLDisplayList(SoState * state, Type type, int allocnum,
                                 SbBool mipmaptexobj)
  : type(type),
    numalloc(allocnum),
    context(SoGLCacheContextElement::get(state)),
    refcount(0),
    mipmap(mipmaptexobj)
{
#if COIN_DEBUG && 0 // debug
  SoDebugError::postInfo("SoGLDisplayList::SoGLDisplayList", "%p", this);
#endif // debug

  // Check for known buggy OpenGL driver.
  const char * versionstr = (const char *)glGetString(GL_VERSION);
  assert(versionstr && "glGetString() returned 0 -- no valid GL context?");
  if (strcmp(versionstr, "1.3.1 NVIDIA 28.02") == 0) {
    // (From NVidia's changelog, it looks like the problem we've been
    // seeing with the 28.02 driver and displaylists *might* have been
    // fixed for the next version (28.80)).

    // Here's an Inventor file which can be used to reproduce the bug:
    // -----8<----- [snip] ----------8<----- [snip] ----------8<------
    // #Inventor V2.1 ascii
    //
    // # This dead simple scenegraph causes the examinerviewer to go blank
    // # on a Linux box with GeForce2 card and version 1.3.1 28.02 of the
    // # NVidia OpenGL drivers. The problem is gone for version 1.3.1 29.60
    // # of the drivers, so this seems _very_ much like a driver bug with
    // # OpenGL displaylists.
    // #
    // # The bug is also present for SGI Inventor.
    // #
    // # <mortene@sim.no>.
    //
    // Separator {
    //    renderCaching ON
    //    ShapeHints {
    //       vertexOrdering COUNTERCLOCKWISE
    //       faceType UNKNOWN_FACE_TYPE
    //    }
    //    Cube { }
    // }
    // -----8<----- [snip] ----------8<----- [snip] ----------8<------


    // FIXME: should be more robust, and rather just disable the use
    // of GL displaylists (but still issuing an
    // SoDebugError::postWarning()). This should be straightforward to
    // do when the FIXME below on better handling of the case where we
    // are not able to allocate displaylist indices is taken care
    // of. 20020911 mortene.

    static SbBool first = TRUE;
    if (first) {
      SoDebugError::post("SoGLDisplayList::SoGLDisplayList",
                         "This OpenGL driver ('%s') is known to contain serious "
                         "bugs in GL displaylist handling, and we strongly urge "
                         "you to upgrade! As long as you are using this driver, "
                         "GL rendering is likely to cause all sorts of nasty "
                         "problems.",
                         versionstr);
      first = FALSE;
    }
  }


  // Reserve displaylist IDs.

  if (this->type != DISPLAY_LIST) {
    assert(allocnum == 1 && "it is only possible to create one texture object at a time");
    const cc_glglue * glw = cc_glglue_instance(this->context);
    if (cc_glglue_has_texture_objects(glw)) {
      // use temporary variable, in case GLuint is typedef'ed to
      // something other than unsigned int
      GLuint tmpindex;
      cc_glglue_glGenTextures(glw, 1, &tmpindex);
      this->firstindex = (unsigned int )tmpindex;
    }
    else { // Fall back to display list, allocation happens further down below.
      this->type = DISPLAY_LIST;
    }
  }
  if (this->type == DISPLAY_LIST) {
    this->firstindex = (unsigned int) glGenLists(allocnum);
    if (this->firstindex == 0) {
      SoDebugError::post("SoGLDisplayList::SoGLDisplayList",
                         "Could not reserve %d displaylist%s. "
                         "Expect flawed rendering.",
                         allocnum, allocnum==1 ? "" : "s");
      // FIXME: be more robust in handling this -- the rendering will
      // gradually go bonkers after we hit this problem. 20020619 mortene.
    }
#if COIN_DEBUG && 0 // debug
    SoDebugError::postInfo("SoGLDisplayList::SoGLDisplayList",
                           "firstindex==%d", this->firstindex);
#endif // debug
  }
}

// private destructor. Use ref()/unref()
SoGLDisplayList::~SoGLDisplayList()
{
#if COIN_DEBUG && 0 // debug
  SoDebugError::postInfo("SoGLDisplayList::~SoGLDisplayList", "%p", this);
#endif // debug

  if (this->type == DISPLAY_LIST) {
    glDeleteLists((GLuint) this->firstindex, this->numalloc);
  }
  else {
    const cc_glglue * glw = cc_glglue_instance(this->context);
    assert(cc_glglue_has_texture_objects(glw));

    // Use temporary variable in case GLUint != unsigned int.
    GLuint tmpindex = (GLuint) this->firstindex;
    // It is only possible to create one texture object at a time, so
    // there's only one index to delete.
    cc_glglue_glDeleteTextures(glw, 1, &tmpindex);
  }
}

/*!
  Increase reference count for this display list/texture object.
*/
void
SoGLDisplayList::ref(void)
{
  this->refcount++;
}

/*!
  Decrease reference count for this instance. When reference count
  reaches 0, the instence is deleted.
*/
void
SoGLDisplayList::unref(SoState * state)
{
  assert(this->refcount > 0);
  if (--this->refcount == 0) {
    // Let SoGLCacheContext delete this instance the next time context is current.
    SoGLCacheContextElement::scheduleDelete(state, this);
  }
}

/*!
  Open this display list/texture object.
*/
void
SoGLDisplayList::open(SoState * state, int index)
{
  if (type == DISPLAY_LIST) {
    // using GL_COMPILE here instead of GL_COMPILE_AND_EXECUTE will
    // lead to much higher performance on nVidia cards, and doesn't
    // hurt performance for other vendors.

    glNewList((GLuint) (this->firstindex+index), GL_COMPILE);
    // FIXME: using the mipmap member variable to temporarily store
    // the index while we compile the display list. Needed to avoid
    // breaking the ABI. This is done properly in Coin-dev.
    
    // It is safe to use the mipmap variable, since it's only used
    // when the type is TEXTURE_OBJECT
    this->mipmap = index; 
  }
  else {
    assert(index == 0);
    this->bindTexture(state);
  }
}

/*!
  Close this display list/texture object.
*/
void
SoGLDisplayList::close(SoState * state)
{
  if (this->type == DISPLAY_LIST) {
    glEndList();
    GLenum err = sogl_glerror_debugging() ? glGetError() : GL_NO_ERROR;
    if (err == GL_OUT_OF_MEMORY) {
      SoDebugError::post("SoGLDisplayList::close",
                         "Not enough memory resources available on system "
                         "to store full display list. Expect flaws in "
                         "rendering.");
    }
    // FIXME: we used this->mipmap to store the open index in open().
    glCallList((GLuint) (this->firstindex + this->mipmap));
    this->mipmap = FALSE;
  }
}

/*!
  Execute this display list/texture object.
*/
void
SoGLDisplayList::call(SoState * state, int index)
{
  if (this->type == DISPLAY_LIST) {
    glCallList((GLuint) (this->firstindex + index));
  }
  else {
    assert(index == 0);
    this->bindTexture(state);
  }
  this->addDependency(state);
}

/*!
  Create a dependency on the display list.
*/
void
SoGLDisplayList::addDependency(SoState * state)
{
  if (state->isCacheOpen()) {
    SoGLRenderCache * cache = (SoGLRenderCache*)
      SoCacheElement::getCurrentCache(state);
    if (cache) cache->addNestedCache(this);
  }
}

/*!
  Returns whether the texture object stored in this instance
  was created with mipmap data. This method is an extension
  versus the Open Inventor API.
*/
SbBool
SoGLDisplayList::isMipMapTextureObject(void) const
{
  return this->mipmap;
}

/*!
  Return type. Display list or texture object.
*/
SoGLDisplayList::Type
SoGLDisplayList::getType(void) const
{
  return this->type;
}

/*!
  Return number of display lists/texture objects allocated.
*/
int
SoGLDisplayList::getNumAllocated(void) const
{
  return this->numalloc;
}

/*!
  Return first GL index for this display list.
*/
unsigned int
SoGLDisplayList::getFirstIndex(void) const
{
  return this->firstindex;
}

/*!
  Return an id for the current context.
*/
int
SoGLDisplayList::getContext(void) const
{
  return this->context;
}

/*!
  \COININTERNAL

  \COIN_FUNCTION_EXTENSION

  \since Coin 2.0
*/
void
SoGLDisplayList::bindTexture(SoState *state)
{
  const cc_glglue * glw = cc_glglue_instance(this->context);
  assert(cc_glglue_has_texture_objects(glw));

  GLenum target;
  switch (this->type) {
  case TEXTURE_OBJECT:
  case DISPLAY_LIST:
    target = GL_TEXTURE_2D;
    break;
  default:
    target = (GLenum) this->type;
    break;
  }

  // this is a quite ugly, since it will not work with multi
  // texturing.  In Coin 2.5 this test will be removed, and the
  // SoGLImage instance will set the correct texture target.
  // pederb, 2005-10-18
  if (SoGLTexture3EnabledElement::get(state)) {
    assert(cc_glglue_has_3d_textures(glw));
    target = GL_TEXTURE_3D;
  }

  cc_glglue_glBindTexture(glw, target, (GLuint)this->firstindex);
}

/*!
  Sets the texture target for a texture object. The texture
  target should be the OpenGL texture target: GL_TEXTURE_2D,
  GL_TEXTURE_3D, GL_TEXTURE_CUBE_MAP or GL_TEXTURE_RECTANGLE_EXT
  (more targets might apply in the future).

  \since Coin 2.5
*/
void
SoGLDisplayList::setTextureTarget(int target)
{
  // just reuse the type member. This assumes that the GL enums are !=
  // DISPLAY_LIST (== 0), which should be safe. This is handled
  // properly in Coin-dev using a pimpl-implementation.
  this->type = (Type) target;
}

/*!
  Returns the OpenGL texture target.

  \sa setTextureTarget().
  \since Coin 2.5
*/
int
SoGLDisplayList::getTextureTarget(void) const
{
  GLenum target;
  switch (this->type) {
  case TEXTURE_OBJECT:
  case DISPLAY_LIST:
    target = GL_TEXTURE_2D;
    break;
  default:
    target = (GLenum) this->type;
    break;
  }
  return (int) target;
}
