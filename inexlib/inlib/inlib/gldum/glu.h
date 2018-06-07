/* Copyright (C) 2010, Guy Barrand. All rights reserved. */
/* See the file inlib.license for terms.                 */

#ifndef inlib_gldum_glu_h
#define inlib_gldum_glu_h

#include "gldef.h"

typedef void* GLUnurbs;
typedef void* GLUtesselator;
typedef unsigned int GLUenum;
typedef double GLUdouble;
typedef void GLUvoid;

#define GLU_TESS_BEGIN_DATA                100106
#define GLU_TESS_VERTEX_DATA               100107
#define GLU_TESS_END_DATA                  100108
#define GLU_TESS_ERROR_DATA                100109
#define GLU_TESS_COMBINE_DATA              100111
#define GLU_TESS_WINDING_RULE              100140
//#define GLU_TESS_TOLERANCE                 100142
#define GLU_TESS_WINDING_ODD               100130
#define GLU_TESS_WINDING_POSITIVE          100132
#define GLU_TESS_WINDING_NEGATIVE          100133

#ifdef __cplusplus
extern "C"{
#endif
GLint gluScaleImage(GLenum format,GLsizei wIn,GLsizei hIn,GLenum typeIn,const void *dataIn,GLsizei wOut,
                    GLsizei hOut,GLenum typeOut,GLvoid* dataOut );
const GLubyte* gluGetString(GLenum name );
const GLubyte* gluErrorString(GLenum error );

GLUnurbs* gluNewNurbsRenderer(void);
void gluDeleteNurbsRenderer(GLUnurbs* nurb);
void gluNurbsProperty(GLUnurbs* nurb,GLenum property,GLfloat value);
void gluLoadSamplingMatrices(GLUnurbs* nurb,const GLfloat *model,const GLfloat *perspective,const GLint *view);
void gluBeginSurface(GLUnurbs* nurb);
void gluEndSurface(GLUnurbs* nurb); 
void gluNurbsSurface(GLUnurbs* nurb,GLint sKnotCount,GLfloat* sKnots,GLint tKnotCount,
                     GLfloat* tKnots,GLint sStride,GLint tStride,
                     GLfloat* control,GLint sOrder,GLint tOrder,GLenum type);
void gluBeginTrim(GLUnurbs* nurb);
void gluEndTrim(GLUnurbs* nurb);
void gluBeginCurve(GLUnurbs* nurb);
void gluEndCurve(GLUnurbs* nurb);
void gluNurbsCurve(GLUnurbs* nurb,GLint knotCount,GLfloat *knots,GLint stride,
		   GLfloat *control,GLint order,GLenum type);
void gluPwlCurve(GLUnurbs* nurb,GLint count,GLfloat* data,GLint stride,GLenum type);
void gluNurbsCallback(GLUnurbs* nurb,GLenum which,GLvoid* CallBackFunc);

GLUtesselator* gluNewTess(void);
void gluDeleteTess (GLUtesselator* tess);
void gluTessBeginContour (GLUtesselator* tess);
void gluTessBeginPolygon (GLUtesselator* tess, GLUvoid* data);
void gluTessCallback (GLUtesselator* tess, GLUenum which, void(*CallBackFunc)());
void gluTessEndContour (GLUtesselator* tess);
void gluTessEndPolygon (GLUtesselator* tess);
void gluTessNormal (GLUtesselator* tess, GLUdouble valueX, GLUdouble valueY, GLUdouble valueZ);
void gluTessProperty (GLUtesselator* tess, GLUenum which, GLUdouble data);
void gluTessVertex (GLUtesselator* tess, GLUdouble *location, GLUvoid* data);

#ifdef __cplusplus
}
#endif

#endif
