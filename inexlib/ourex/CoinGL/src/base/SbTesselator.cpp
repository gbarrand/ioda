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
  \class SbTesselator SbTesselator.h Inventor/SbTesselator.h
  \brief The SbTesselator class is used to tessellate polygons into triangles.
  \ingroup base

  SbTesselator is used within Coin to split polygons into
  triangles. It handles concave polygons, does Delaunay triangulation
  and avoids generating self-intersecting triangles.

  Here's a simple example which shows how to tessellate a quad polygon
  with corners in <0, 0, 0>, <1, 0, 0>, <1, 1, 0> and <0, 1, 0>.

  \code

  // Callback function for the tessellator. Called once for each
  // generated triangle with the vertices.
  static void
  tess_cb(void * v0, void * v1, void * v2, void * cbdata)
  {
    SbVec3f * vtx0 = (SbVec3f *)v0;
    SbVec3f * vtx1 = (SbVec3f *)v1;
    SbVec3f * vtx2 = (SbVec3f *)v2;
    (void) fprintf(stdout, "triangle: <%f, %f, %f> <%f, %f, %f> <%f, %f, %f>\n",
      (*vtx0)[0], (*vtx0)[1], (*vtx0)[2],
      (*vtx1)[0], (*vtx1)[1], (*vtx1)[2],
      (*vtx2)[0], (*vtx2)[1], (*vtx2)[2]);

    // Do stuff with triangle here.
  }

  static SbVec3f vertices[] = {
    SbVec3f(1, 0, 0), SbVec3f(1, 1, 0),
    SbVec3f(0, 1, 0), SbVec3f(0, 0, 0)
  };

  SbTesselator mytessellator(tess_cb, NULL);
  mytessellator.beginPolygon();
  for (int i=0; i < 4; i++) {
    mytessellator.addVertex(vertices[i], &vertices[i]);
  }
  mytessellator.endPolygon();

  \endcode
  
  The call to SbTesselator::endPolygon() triggers the SbTesselator to
  spring into action, calling the tess_cb() function for each triangle
  it generates.
  
  The reason we use 2 arguments to SbTesselator::addVertex() and passes
  void pointers for the vertices to the callback function is to make it
  possible to have more complex structures than just the coordinates
  themselves (as in the example above), like material information,
  lighting information or whatever other attributes your vertices have.

  This class is not part of the original Open Inventor API.


  Another option for tessellating polygons is the tessellator of the
  GLU library. It has some features not part of SbTesselator (like
  handling hulls), but the GLU library is known to have bugs in
  various implementations and doesn't do Delaunay triangulation. If
  you however still prefer to use the GLU tessellator instead of this
  one, that can be forced by setting an environment variable:

  \code
  (void) coin_setenv("COIN_PREFER_GLU_TESSELLATOR", "1", 1);
  \endcode
*/


#include <Inventor/SbTesselator.h>
#include <Inventor/SbHeap.h>
#include <Inventor/C/base/heap.h>
#include <Inventor/SbBSPTree.h>
#include <Inventor/SbSphere.h>
#include <Inventor/lists/SbList.h>
#include <Inventor/errors/SoDebugError.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <float.h>

struct SbTVertex {
  float weight;
  int dirtyweight;
  SbTVertex *prev;
  SbTesselator *thisp;
  SbVec3f v;
  void *data;
  SbTVertex *next;
};

class SbTesselatorP {
public:
  SbTesselatorP(void) : bsptree(256) { }
  cc_heap * heap;
  SbBSPTree bsptree;
  SbList <int> clippablelist;
  float epsilon;
  SbBox3f bbox;
};

// FIXME: replace with standard pimpl member in Coin-3. pederb, 2003-10-06
#define PRIVATE(obj) ((SbTesselatorP*)(obj->heap))

int
SbTesselator::heap_compare(void * h0, void * h1)
{
  if (heap_evaluate(h0) > heap_evaluate(h1)) return -1;
  return 1;
}


float
SbTesselator::heap_evaluate(void *v)
{
  SbTVertex *vertex = (SbTVertex*)v;
  if (vertex->dirtyweight) {
    vertex->dirtyweight = 0;
    if ((vertex->thisp->area(vertex) > PRIVATE(vertex->thisp)->epsilon) &&
        vertex->thisp->isTriangle(vertex) &&
        vertex->thisp->clippable(vertex)) {
#if 0 // testing code to avoid empty triangles
      vertex->weight = vertex->thisp->circleSize(vertex);
      SbTVertex *v2 = vertex->next;
      if (vertex->weight != FLT_MAX &&
          v2->thisp->keepVertices &&
          v2->thisp->numVerts > 4 &&
          fabs(v2->thisp->area(v2)) < PRIVATE(thisp)->epsilon) {
        vertex->weight = 0.0f; // cut immediately!
      }

#else
      vertex->weight = vertex->thisp->circleSize(vertex);
#endif
    }
    else
      vertex->weight = FLT_MAX;
  }
  return vertex->weight;
}


//projection enums
enum {OXY,OXZ,OYZ};

/*!
  Initializes a tessellator. The \a callback argument specifies a
  function which will be called for each triangle returned by the
  tessellator. The callback function will get three pointers to each
  vertex and the \a userdata pointer. The vertex pointers are
  specified in the SbTesselator::addVertex() method.
*/
SbTesselator::SbTesselator(SbTesselatorCB * func, void * data)
{
  this->setCallback(func, data);
  this->headV = this->tailV = NULL;
  this->currVertex = 0;  

  this->heap = (SbHeap *) new SbTesselatorP;
  PRIVATE(this)->heap =
    cc_heap_construct(256, (cc_heap_compare_cb *) heap_compare, TRUE);
  PRIVATE(this)->epsilon = FLT_EPSILON;
}

/*!
  Destructor.
*/
SbTesselator::~SbTesselator()
{
  cleanUp();
  int i, n = this->vertexStorage.getLength();
  for (i = 0; i < n; i++) { delete this->vertexStorage[i]; }
  
  cc_heap_destruct(PRIVATE(this)->heap);
  delete PRIVATE(this);
}

/*!
  Initializes new polygon.

  You can explicitly set the polygon normal if you know what it
  is. Otherwise it will be calculated internally.

  If \a keepVerts is \c TRUE, all vertices will be included in the
  returned triangles, even though this might lead to triangles without
  area.
*/
void
SbTesselator::beginPolygon(SbBool keepVerts, const SbVec3f &normal)
{
  this->cleanUp();
  this->keepVertices = keepVerts;
  if (normal != SbVec3f(0.0f, 0.0f, 0.0f)) {
    this->polyNormal = normal;
    this->hasNormal = TRUE;
  }
  else {
    this->hasNormal = FALSE;
  }
  this->headV = this->tailV = NULL;
  this->numVerts=0;
  PRIVATE(this)->bbox.makeEmpty();
}

/*!
  Adds a new vertex to the polygon. \a data will be returned as a
  vertex in the callback-function.
*/
void
SbTesselator::addVertex(const SbVec3f &v,void *data)
{
  if (this->tailV && !this->keepVertices && v == this->tailV->v) return;

  PRIVATE(this)->bbox.extendBy(v);

  SbTVertex *newv = this->newVertex();
  newv->v = v;
  newv->data = data;
  newv->next = NULL;
  newv->dirtyweight = 1;
  newv->weight = FLT_MAX;
  newv->prev = this->tailV;
  newv->thisp = this;
  if (!this->headV) this->headV = newv;
  if (this->tailV) this->tailV->next = newv;
  this->tailV = newv;
  this->numVerts++;
}

/*!
  Signals the tessellator to begin tessellating. The callback function
  specified in the constructor (or set using the
  SbTesselator::setCallback() method) will be called for each triangle
  before returning.
*/
void
SbTesselator::endPolygon()
{
  // check for special case when last point equals the first point
  if (!this->keepVertices && this->numVerts >= 3) {
    SbTVertex * first = this->headV;
    SbTVertex * last = this->tailV;
    if (first->v == last->v) {
      SbTVertex * newlast = last->prev;
      newlast->next = NULL;
      // don't delete old tail. We have some special memory handling
      // in this class
      this->tailV = newlast;
      this->numVerts--;
    }
  }
 
  SbTVertex *v;

  if (this->numVerts > 3) {
    this->calcPolygonNormal();

    // Find best projection plane
    int projection;
    if (fabs(polyNormal[0]) > fabs(polyNormal[1]))
      if (fabs(polyNormal[0]) > fabs(polyNormal[2]))
        projection=OYZ;
      else projection=OXY;
    else
      if (fabs(polyNormal[1]) > fabs(polyNormal[2]))
        projection=OXZ;
      else projection=OXY;

    switch (projection) {
    case OYZ:
      this->X=1;
      this->Y=2;
      polyDir=(int)(polyNormal[0]/fabs(polyNormal[0]));
      break;
    case OXY:
      this->X=0;
      this->Y=1;
      polyDir=(int)(polyNormal[2]/fabs(polyNormal[2]));
      break;
    case OXZ:
      this->X=2;
      this->Y=0;
      polyDir=(int)(polyNormal[1]/fabs(polyNormal[1]));
      break;
    }

    // find epsilon based on bbox
    SbVec3f d;
    PRIVATE(this)->bbox.getSize(d[0],d[1],d[2]);
    PRIVATE(this)->epsilon = SbMin(d[X], d[Y]) * FLT_EPSILON * FLT_EPSILON;

    //Make loop
    this->tailV->next = this->headV;
    this->headV->prev = this->tailV;

    // add all vertices to heap.
    cc_heap_clear(PRIVATE(this)->heap);
    PRIVATE(this)->bsptree.clear(256);

    // use two loops to add points to bsptree and heap, since the heap
    // requires that the bsptree is fully set up to evaluate
    // correctly.
    v = this->headV;
    do {
      PRIVATE(this)->bsptree.addPoint(SbVec3f(v->v[X],
                                              v->v[Y],
                                              0.0f), v);
      v = v->next;
    } while (v != this->headV);

    do {
      cc_heap_add(PRIVATE(this)->heap, v);
      v = v->next;
    } while (v != this->headV);
    
    while (this->numVerts > 4) {
      v = (SbTVertex*) cc_heap_get_top(PRIVATE(this)->heap);
      if (heap_evaluate(v) == FLT_MAX) break;
      cc_heap_remove(PRIVATE(this)->heap, v->next);
      PRIVATE(this)->bsptree.removePoint(SbVec3f(v->next->v[X],
                                                 v->next->v[Y],
                                                 0.0f));
      this->emitTriangle(v); // will remove v->next
      this->numVerts--;
      
      v->prev->dirtyweight = 1;
      v->dirtyweight = 1;

      (void) cc_heap_remove(PRIVATE(this)->heap, v->prev);
      (void) cc_heap_remove(PRIVATE(this)->heap, v);

      cc_heap_add(PRIVATE(this)->heap, v->prev);
      cc_heap_add(PRIVATE(this)->heap, v);
    }

    // remember that headV and tailV are not valid anymore!

    //
    // must handle special case when only four vertices remain
    //
    if (this->numVerts == 4) {
      float v0 = SbMax(heap_evaluate(v), heap_evaluate(v->next->next));
      float v1 = SbMax(heap_evaluate(v->next), heap_evaluate(v->prev));

      // abort if vertices should not be kept
      if (v0 == v1 && v0 == FLT_MAX && !this->keepVertices) return;

      if (v0 < v1) {
        this->emitTriangle(v);
        this->emitTriangle(v);
      }
      else {
        v = v->next;
        this->emitTriangle(v);
        this->emitTriangle(v);
      }
      this->numVerts -= 2;
    }

    // Emit the empty triangles that might lay around
    if (this->keepVertices) {
      while (numVerts>=3) {
        this->emitTriangle(v);
        this->numVerts--;
      }
    }
  }
  else if (this->numVerts==3) {   //only one triangle
    this->emitTriangle(headV);
  }
}

/*!
  Sets the callback function for this tessellator.
*/
void
SbTesselator::setCallback(SbTesselatorCB * func, void *data)
{
  this->callback = func;
  this->callbackData = data;
}


static SbBool
point_on_edge(const float x, const float y,
              const float * const v0, const float * const v1,
              const int X, const int Y, const float eps)
{
  if (x < v0[X] && x < v1[X]) return FALSE;
  if (x > v0[X] && x > v1[X]) return FALSE;
  if (y < v0[Y] && y < v1[Y]) return FALSE;
  if (y > v0[Y] && y > v1[Y]) return FALSE;

  if (v0[X] == v1[X]) {
    if (fabs(x-v0[X]) <= eps) return TRUE;
    return FALSE;
  }

  float ny = v0[Y] + (x-v0[X])*(v1[Y]-v0[Y])/(v1[X]-v0[X]);

  if (fabs(y-ny) <= eps) {
    return TRUE;
  }
  return FALSE;
}

//
// PRIVATE FUNCTIONS:
//

//
// Checks if the point p lies inside the triangle
// starting with t.
// Algorithm from comp.graphics.algorithms FAQ
//
SbBool
SbTesselator::pointInTriangle(SbTVertex *p, SbTVertex *t)
{
  float x,y;
  SbBool tst = FALSE;

  x = p->v[X];
  y = p->v[Y];

  const float *v1 = t->v.getValue();
  const float *v2 = t->next->next->v.getValue();

  if ((((v1[Y] <= y) && (y < v2[Y])) || ((v2[Y] <= y)  && (y < v1[Y]))) &&
      (x < (v2[X] - v1[X]) * (y - v1[Y]) / (v2[Y] - v1[Y]) + v1[X])) {
    tst = !tst;
  }

  v2 = v1;
  v1 = t->next->v.getValue();

  if ((((v1[Y] <= y) && (y < v2[Y])) || ((v2[Y] <= y)  && (y < v1[Y]))) &&
      (x < (v2[X] - v1[X]) * (y - v1[Y]) / (v2[Y] - v1[Y]) + v1[X])) {
    tst = !tst;
  }

  v2 = v1;
  v1 = t->next->next->v.getValue();

  if ((((v1[Y] <= y) && (y < v2[Y])) || ((v2[Y] <= y)  && (y < v1[Y]))) &&
      (x < (v2[X] - v1[X]) * (y - v1[Y]) / (v2[Y] - v1[Y]) + v1[X])) {
    tst = !tst;
  }

  // the pointInTriangle test might fail for vertices that are on one
  // of the triangle edges. Do a point_on_edge test for all three
  // edges to handle this case. Example model that fails without this
  // test:
  // 
  //  ShapeHints { faceType UNKNOWN_FACE_TYPE vertexOrdering CLOCKWISE }
  //  IndexedFaceSet {
  //    vertexProperty
  //    VertexProperty {
  //      vertex [ -0.3 0.05 0.0,
  //                0.3   0.05  0.0,
  //                0.3   0.1   0.0,
  //                0.5   0.0   0.0,
  //                0.3  -0.1   0.0,
  //                0.3  -0.05  0.0,
  //               -0.3  -0.05  0.0,
  //               -0.3  -0.1   0.0,
  //               -0.5   0.0   0.0,
  //               -0.3   0.1   0.0 ]
  //    }
  //    coordIndex [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1]
  if (!tst) {
    if (point_on_edge(x, y, t->v.getValue(),
                      t->next->v.getValue(), X, Y, PRIVATE(this)->epsilon)) {
      return TRUE;
    }
    if (point_on_edge(x, y, t->next->v.getValue(),
                      t->next->next->v.getValue(), X, Y, PRIVATE(this)->epsilon)) {
      return TRUE;
    }
    if (point_on_edge(x, y, t->next->next->v.getValue(),
                      t->v.getValue(), X, Y, PRIVATE(this)->epsilon)) {
      return TRUE;
    }
  }
  return tst;
}


//
// Check if v points to a legal triangle (normal is right way)
// (i.e convex or concave corner)
//
SbBool
SbTesselator::isTriangle(SbTVertex *v)
{
  return (((v->next->v[X]-v->v[X]) * (v->next->next->v[Y]-v->v[Y]) -
	   (v->next->v[Y]-v->v[Y]) * (v->next->next->v[X]-v->v[X])) *
	  this->polyDir > 0.0) ? TRUE : FALSE;
}

//
// Check if there are no intersection to the triangle
// pointed to by v. (no other vertices are inside the triangle)
//
SbBool
SbTesselator::clippable(SbTVertex *v)
{
  SbBox3f bbox;
  bbox.makeEmpty();
  bbox.extendBy(SbVec3f(v->v[X], v->v[Y], 0.0f));
  bbox.extendBy(SbVec3f(v->next->v[X], v->next->v[Y], 0.0f));
  bbox.extendBy(SbVec3f(v->next->next->v[X], v->next->next->v[Y], 0.0f));
  
  SbSphere sphere;
  sphere.circumscribe(bbox);
  
  SbList <int> & l = PRIVATE(this)->clippablelist;
  l.truncate(0);
  PRIVATE(this)->bsptree.findPoints(sphere, l);
  for (int i = 0; i < l.getLength(); i++) {
    SbTVertex * vtx = (SbTVertex*) PRIVATE(this)->bsptree.getUserData(l[i]);
    if (vtx != v && vtx != v->next && vtx != v->next->next) {
      if (pointInTriangle(vtx, v)) { return FALSE; }
    }
  }
  return TRUE;
}



//
// Call the callback-function for the triangle starting with t
//
void
SbTesselator::emitTriangle(SbTVertex *t)
{
  assert(t);
  assert(t->next);
  assert(t->next->next);
  assert(this->callback);

  this->callback(t->data, t->next->data, t->next->next->data,
                 this->callbackData);
  this->cutTriangle(t);
}

// Cuts t->next out of the triangle vertex list.
//
// FIXME: bad design, this should have been a method on
// SbTVertex. 20031007 mortene.
void
SbTesselator::cutTriangle(SbTVertex * t)
{
  t->next->next->prev = t;
  t->next = t->next->next;
}

//
// Return the area of the triangle starting with v
//
float
SbTesselator::area(SbTVertex *v)
{
  return (float)fabs(((v->next->v[X]-v->v[X])*(v->next->next->v[Y]-v->v[Y])-
                     (v->next->v[Y]-v->v[Y])*(v->next->next->v[X]-v->v[X])));
}

//
// Returns the center of the circle through points a, b, c.
//
SbBool
SbTesselator::circleCenter(const SbVec3f &a, const SbVec3f &b,
                           const SbVec3f &c, float &cx, float &cy)
{
  float d1, d2, d3, c1, c2, c3;
  SbVec3f tmp1, tmp2;

  tmp1 = b - a;
  tmp2 = c - a;
  d1 = dot2D(tmp1, tmp2);

  tmp1 = b - c;
  tmp2 = a - c;
  d2 = dot2D(tmp1, tmp2);

  tmp1 = a - b;
  tmp2 = c - b;
  d3 = dot2D(tmp1, tmp2);

  c1 = d2 * d3;
  c2 = d3 * d1;
  c3 = d1 * d2;

  SbVec3f tmp4(c);
  tmp1 = a;
  tmp2 = b;
  tmp1 *= (c2+c3);
  tmp2 *= (c3+c1);
  tmp4 *= (c1+c2);
  tmp4 += tmp1;
  tmp4 += tmp2;

  float div = 2.0f*(c1+c2+c3);
  if (div != 0.0f) {
    float val = 1.0f / div;
    cx = tmp4[X] * val;
    cy = tmp4[Y] * val;
    return TRUE;
  }
  return FALSE;
}

//
// Returns the square of the radius of the circle through a, b, c
//
float
SbTesselator::circleSize(const SbVec3f &a, const SbVec3f &b, const SbVec3f &c)
{
  float cx, cy;
  if (circleCenter(a, b, c, cx, cy)) {
    float t1, t2;
    t1 = a[X] - cx;
    t2 = a[Y] - cy;
    return t1*t1+t2*t2;
  }
  return FLT_MAX;
}

float
SbTesselator::circleSize(SbTVertex *v)
{
  return circleSize(v->v, v->next->v, v->next->next->v);
}

float
SbTesselator::dot2D(const SbVec3f &v1, const SbVec3f &v2)
{
  return v1[X] * v2[X] + v1[Y] * v2[Y];
}

void
SbTesselator::calcPolygonNormal()
{
  assert(this->numVerts > 3);

  this->polyNormal.setValue(0.0f, 0.0f, 0.0f);
  SbVec3f vert1, vert2;
  SbTVertex *currvertex = this->headV;
  vert2 = currvertex->v;

  while (currvertex->next != NULL && currvertex != tailV) {
    vert1 = vert2;
    vert2 = currvertex->next->v;
    this->polyNormal[0] += (vert1[1] - vert2[1]) * (vert1[2] + vert2[2]);
    this->polyNormal[1] += (vert1[2] - vert2[2]) * (vert1[0] + vert2[0]);
    this->polyNormal[2] += (vert1[0] - vert2[0]) * (vert1[1] + vert2[1]);
    currvertex = currvertex->next;
  }
  vert1 = vert2;
  vert2 = headV->v;
  polyNormal[0] += (vert1[1] - vert2[1]) * (vert1[2] + vert2[2]);
  polyNormal[1] += (vert1[2] - vert2[2]) * (vert1[0] + vert2[0]);
  polyNormal[2] += (vert1[0] - vert2[0]) * (vert1[1] + vert2[1]);
  
  if (polyNormal.normalize() == 0.0f) {
#if COIN_DEBUG
    SoDebugError::postWarning("SbTesselator::calcPolygonNormal",
                              "Polygon has no normal.");
#endif // COIN_DEBUG
  }
}

//
// makes sure SbTVertexes are not allocated and deallocated
// all the time, by storing them in a growable array. This
// way, the SbTVertexes will not be deleted until the tessellator
// is destructed, and SbTVertexes can be reused.
//
struct SbTVertex *
SbTesselator::newVertex()
{
  assert(this->currVertex <= this->vertexStorage.getLength());
  if (this->currVertex == this->vertexStorage.getLength()) {
    struct SbTVertex *v = new SbTVertex;
    this->vertexStorage.append(v);
  }
  return this->vertexStorage[currVertex++];
}

void
SbTesselator::cleanUp()
{
  this->headV = this->tailV = NULL;
  this->currVertex = 0;
  this->numVerts = 0;
}
