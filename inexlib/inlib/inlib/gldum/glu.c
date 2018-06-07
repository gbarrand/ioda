/* Copyright (C) 2010, Guy Barrand. All rights reserved. */
/* See the file inlib.license for terms.                 */

/* iOS : OpenGL functions not in GL-ES gl.h : */

#ifdef COIN_INTERNAL
#include "Inventor/system/glu-headers.h"
#else
#include "glu.h"
#endif

#include <stdio.h>

GLint gluScaleImage(GLenum format,GLsizei wIn,GLsizei hIn,GLenum typeIn,const void *dataIn,GLsizei wOut,
                    GLsizei hOut,GLenum typeOut,GLvoid* dataOut ) {
  fprintf(stderr,"coin_gl : gluScaleImage : dummy.\n");
  return 0;
}
const GLubyte* gluGetString(GLenum name) {return 0;}
const GLubyte* gluErrorString(GLenum error) {return 0;}

GLUnurbs* gluNewNurbsRenderer(void) {
  fprintf(stderr,"coin_gl : gluNewNurbsRenderer : dummy.\n");
  return 0;
}
void gluDeleteNurbsRenderer(GLUnurbs* nurb) {}
void gluNurbsProperty(GLUnurbs* nurb,GLenum property,GLfloat value) {}
void gluLoadSamplingMatrices(GLUnurbs* nurb,const GLfloat *model,const GLfloat *perspective,const GLint *view) {}
void gluBeginSurface(GLUnurbs* nurb) {}
void gluEndSurface(GLUnurbs* nurb) {}
void gluNurbsSurface(GLUnurbs* nurb,GLint sKnotCount,GLfloat* sKnots,GLint tKnotCount,
                     GLfloat* tKnots,GLint sStride,GLint tStride,
                     GLfloat* control,GLint sOrder,GLint tOrder,GLenum type) {}
void gluBeginTrim(GLUnurbs* nurb) {}
void gluEndTrim(GLUnurbs* nurb) {}
void gluBeginCurve(GLUnurbs* nurb) {}
void gluEndCurve(GLUnurbs* nurb) {}
void gluNurbsCurve(GLUnurbs* nurb,GLint knotCount,GLfloat *knots,GLint stride,
		   GLfloat *control,GLint order,GLenum type) {}
void gluPwlCurve(GLUnurbs* nurb,GLint count,GLfloat* data,GLint stride,GLenum type) {}
void gluNurbsCallback(GLUnurbs* nurb,GLenum which,GLvoid* CallBackFunc) {}

GLUtesselator* gluNewTess(void) {
  fprintf(stderr,"coin_gl : gluNewTess : dummy.\n");
  return 0;
}
void gluDeleteTess (GLUtesselator* tess) {}
void gluTessBeginContour (GLUtesselator* tess) {}
void gluTessBeginPolygon (GLUtesselator* tess, GLUvoid* data) {}
void gluTessCallback (GLUtesselator* tess, GLUenum which, void(*CallBackFunc)()) {}
void gluTessEndContour (GLUtesselator* tess) {}
void gluTessEndPolygon (GLUtesselator* tess) {}
void gluTessNormal (GLUtesselator* tess, GLUdouble valueX, GLUdouble valueY, GLUdouble valueZ) {}
void gluTessProperty (GLUtesselator* tess, GLUenum which, GLUdouble data) {}
void gluTessVertex (GLUtesselator* tess, GLUdouble *location, GLUvoid* data) {}
