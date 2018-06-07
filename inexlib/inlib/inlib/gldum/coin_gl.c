/* Copyright (C) 2010, Guy Barrand. All rights reserved. */
/* See the file inlib.license for terms.                 */

/* iOS : OpenGL functions not in GL-ES gl.h : */

#ifdef COIN_INTERNAL
#include "Inventor/system/gl-headers.h"
#else
#include "coin_gl.h"
#endif

#include <stdio.h>
#include <stddef.h>

void glGetTexLevelParameterfv(GLenum a_target,GLint a_level,GLenum a_pname,GLfloat* a_params) {
  fprintf(stderr,"coin_gl : glGetTexLevelParameterfv : dummy : target %d\n",a_target);
  (void)a_target;(void)a_level;(void)a_pname;(void)a_params;
}
void glGetTexLevelParameteriv(GLenum a_target,GLint a_level,GLenum a_pname,GLint* a_params) {
  fprintf(stderr,"coin_gl : glGetTexLevelParameteriv : dummy : target %d\n",a_target);
  (void)a_target;(void)a_level;(void)a_pname;(void)a_params;
}
void glDepthMask(GLboolean a_flag) {(void)a_flag;
  fprintf(stderr,"coin_gl : glDepthMask : dummy.\n");
}
void glTexGend(GLenum a_coord,GLenum a_pname,GLdouble a_param) {(void)a_coord;(void)a_pname;(void)a_param;
  fprintf(stderr,"coin_gl : glTexGend : dummy.\n");
}
void glTexGenf(GLenum a_coord,GLenum a_pname,GLfloat a_param) {(void)a_coord;(void)a_pname;(void)a_param;
  fprintf(stderr,"coin_gl : glTexGenf : dummy.\n");
}
void glTexGeni(GLenum a_coord,GLenum a_pname,GLint a_param) {(void)a_coord;(void)a_pname;(void)a_param;
  fprintf(stderr,"coin_gl : glTexGeni : dummy.\n");
}
void glTexGendv(GLenum a_coord,GLenum a_pname,const GLdouble* a_param) {(void)a_coord;(void)a_pname;(void)a_param;
  fprintf(stderr,"coin_gl : glTexGendv : dummy.\n");
}
void glTexGenfv(GLenum a_coord,GLenum a_pname,const GLfloat* a_param) {(void)a_coord;(void)a_pname;(void)a_param;
  fprintf(stderr,"coin_gl : glTexGenfv : dummy.\n");
}
void glTexGeniv(GLenum a_coord,GLenum a_pname,const GLint* a_param) {(void)a_coord;(void)a_pname;(void)a_param;
  fprintf(stderr,"coin_gl : glTexGeniv : dummy.\n");
}
void glAccum(GLenum a_op,GLfloat a_value) {(void)a_op;(void)a_value;
  fprintf(stderr,"coin_gl : glAccum : dummy.\n");
}

void glColorMaterial(GLenum a_face,GLenum a_mode) {
  fprintf(stderr,"coin_gl : glColorMaterial : dummy : face %x, mode %x.\n",a_face,a_mode);
}

void glMultMatrixd(const GLdouble* a_m) {(void)a_m;
  fprintf(stderr,"coin_gl : glMultMatrixd : dummy.\n");
}
void glMultMatrixf(const GLfloat* a_m) {(void)a_m;
  fprintf(stderr,"coin_gl : glMultMatrixf : dummy.\n");
}

void glTexCoord1d( GLdouble s ) {(void)s;
  fprintf(stderr,"coin_gl : glTexCoord1d : dummy.\n");
}
void glTexCoord1f( GLfloat s ) {(void)s;
  fprintf(stderr,"coin_gl : glTexCoord1f : dummy.\n");
}
void glTexCoord1i( GLint s ) {(void)s;
  fprintf(stderr,"coin_gl : glTexCoord1i : dummy.\n");
}
void glTexCoord1s( GLshort s ) {(void)s;
  fprintf(stderr,"coin_gl : glTexCoord1s : dummy.\n");
}
void glTexCoord2d( GLdouble s,GLdouble t ) {
  fprintf(stderr,"coin_gl : glTexCoord2d : dummy.\n");
}
void glTexCoord2f( GLfloat s,GLfloat t ) {
  fprintf(stderr,"coin_gl : glTexCoord2f : dummy.\n");
}
void glTexCoord2i( GLint s,GLint t ) {
  fprintf(stderr,"coin_gl : glTexCoord2i : dummy.\n");
}
void glTexCoord2s( GLshort s,GLshort t ) {
  fprintf(stderr,"coin_gl : glTexCoord2s : dummy.\n");
}
void glTexCoord3d( GLdouble s,GLdouble t,GLdouble r ) {
  fprintf(stderr,"coin_gl : glTexCoord3d : dummy.\n");
}
void glTexCoord3f( GLfloat s,GLfloat t,GLfloat r ) {
  fprintf(stderr,"coin_gl : glTexCoord3f : dummy.\n");
}
void glTexCoord3i( GLint s,GLint t,GLint r ) {
  fprintf(stderr,"coin_gl : glTexCoord3i : dummy.\n");
}
void glTexCoord3s( GLshort s,GLshort t,GLshort r ) {
  fprintf(stderr,"coin_gl : glTexCoord3s : dummy.\n");
}
void glTexCoord4d( GLdouble s,GLdouble t,GLdouble r,GLdouble q ) {
  fprintf(stderr,"coin_gl : glTexCoord4d : dummy.\n");
}
void glTexCoord4f( GLfloat s,GLfloat t,GLfloat r,GLfloat q ) {
  fprintf(stderr,"coin_gl : glTexCoord4f : dummy.\n");
}
void glTexCoord4i( GLint s,GLint t,GLint r,GLint q ) {
  fprintf(stderr,"coin_gl : glTexCoord4i : dummy.\n");
}
void glTexCoord4s( GLshort s,GLshort t,GLshort r,GLshort q ) {
  fprintf(stderr,"coin_gl : glTexCoord4s : dummy.\n");
}

void glTexCoord1dv( const GLdouble *v ) {(void)v;
  fprintf(stderr,"coin_gl : glTexCoord1dv : dummy.\n");
}
void glTexCoord1fv( const GLfloat *v ) {(void)v;
  fprintf(stderr,"coin_gl : glTexCoord1fv : dummy.\n");
}
void glTexCoord1iv( const GLint *v ) {(void)v;
  fprintf(stderr,"coin_gl : glTexCoord1iv : dummy.\n");
}
void glTexCoord1sv( const GLshort *v ) {(void)v;
  fprintf(stderr,"coin_gl : glTexCoord1sv : dummy.\n");
}
void glTexCoord2dv( const GLdouble *v ) {(void)v;
  fprintf(stderr,"coin_gl : glTexCoord2dv : dummy.\n");
}
void glTexCoord2fv( const GLfloat *v ) {(void)v;
  fprintf(stderr,"coin_gl : glTexCoord2fv : dummy.\n");
}
void glTexCoord2iv( const GLint *v ) {(void)v;
  fprintf(stderr,"coin_gl : glTexCoord2iv : dummy.\n");
}
void glTexCoord2sv( const GLshort *v ) {(void)v;
  fprintf(stderr,"coin_gl : glTexCoord2sv : dummy.\n");
}
void glTexCoord3dv( const GLdouble *v ) {(void)v;
  fprintf(stderr,"coin_gl : glTexCoord3dv : dummy.\n");
}
void glTexCoord3fv( const GLfloat *v ) {(void)v;
  fprintf(stderr,"coin_gl : glTexCoord3fv : dummy.\n");
}
void glTexCoord3iv( const GLint *v ) {(void)v;
  fprintf(stderr,"coin_gl : glTexCoord3iv : dummy.\n");
}
void glTexCoord3sv( const GLshort *v ) {(void)v;
  fprintf(stderr,"coin_gl : glTexCoord3sv : dummy.\n");
}
void glTexCoord4dv( const GLdouble *v ) {(void)v;
  fprintf(stderr,"coin_gl : glTexCoord4dv : dummy.\n");
}
void glTexCoord4fv( const GLfloat *v ) {(void)v;
  fprintf(stderr,"coin_gl : glTexCoord4fv : dummy.\n");
}
void glTexCoord4iv( const GLint *v ) {(void)v;
  fprintf(stderr,"coin_gl : glTexCoord4iv : dummy.\n");
}
void glTexCoord4sv( const GLshort *v ) {(void)v;
  fprintf(stderr,"coin_gl : glTexCoord4sv : dummy.\n");
}

void glVertex2d( GLdouble x,GLdouble y ) {
  fprintf(stderr,"coin_gl : glVertex2d : dummy.\n");
}
void glVertex2f( GLfloat x,GLfloat y ) {
  fprintf(stderr,"coin_gl : glVertex2f : dummy.\n");
}
void glVertex2i( GLint x,GLint y ) {
  fprintf(stderr,"coin_gl : glVertex2i : dummy.\n");
}
void glVertex2s( GLshort x,GLshort y ) {
  fprintf(stderr,"coin_gl : glVertex2s : dummy.\n");
}
void glVertex3d( GLdouble x,GLdouble y,GLdouble z ) {
  fprintf(stderr,"coin_gl : glVertex3d : dummy.\n");
}
void glVertex3f( GLfloat x,GLfloat y,GLfloat z ) {
  fprintf(stderr,"coin_gl : glVertex3f : dummy.\n");
}
void glVertex3i( GLint x,GLint y,GLint z ) {
  fprintf(stderr,"coin_gl : glVertex3i : dummy.\n");
}
void glVertex3s( GLshort x,GLshort y,GLshort z ) {
  fprintf(stderr,"coin_gl : glVertex3s : dummy.\n");
}
void glVertex4d( GLdouble x,GLdouble y,GLdouble z,GLdouble w ) {
  fprintf(stderr,"coin_gl : glVertex4d : dummy.\n");
}
void glVertex4f( GLfloat x,GLfloat y,GLfloat z,GLfloat w ) {
  fprintf(stderr,"coin_gl : glVertex4f : dummy.\n");
}
void glVertex4i( GLint x,GLint y,GLint z,GLint w ) {
  fprintf(stderr,"coin_gl : glVertex4i : dummy.\n");
}
void glVertex4s( GLshort x,GLshort y,GLshort z,GLshort w ) {
  fprintf(stderr,"coin_gl : glVertex4s : dummy.\n");
}

void glVertex2dv( const GLdouble *v ) {(void)v;
  fprintf(stderr,"coin_gl : glVertex2dv : dummy.\n");
}
void glVertex2fv( const GLfloat *v ) {(void)v;
  fprintf(stderr,"coin_gl : glVertex2fv : dummy.\n");
}
void glVertex2iv( const GLint *v ) {(void)v;
  fprintf(stderr,"coin_gl : glVertex2iv : dummy.\n");
}
void glVertex2sv( const GLshort *v ) {(void)v;
  fprintf(stderr,"coin_gl : glVertex2sv : dummy.\n");
}
void glVertex3dv( const GLdouble *v ) {(void)v;
  fprintf(stderr,"coin_gl : glVertex3dv : dummy.\n");
}
void glVertex3iv( const GLint *v ) {(void)v;
  fprintf(stderr,"coin_gl : glVertex3iv : dummy.\n");
}
void glVertex3sv( const GLshort *v ) {(void)v;
  fprintf(stderr,"coin_gl : glVertex3sv : dummy.\n");
}
void glVertex4dv( const GLdouble *v ) {(void)v;
  fprintf(stderr,"coin_gl : glVertex4dv : dummy.\n");
}
void glVertex4fv( const GLfloat *v ) {(void)v;
  fprintf(stderr,"coin_gl : glVertex4fv : dummy.\n");
}
void glVertex4iv( const GLint *v ) {(void)v;
  fprintf(stderr,"coin_gl : glVertex4iv : dummy.\n");
}
void glVertex4sv( const GLshort *v ) {(void)v;
  fprintf(stderr,"coin_gl : glVertex4sv : dummy.\n");
}

void glColor3b( GLbyte red,GLbyte green,GLbyte blue ) {
  fprintf(stderr,"coin_gl : glColor3b : dummy.\n");
}
void glColor3d( GLdouble red,GLdouble green,GLdouble blue ) {
  fprintf(stderr,"coin_gl : glColor3d : dummy.\n");
}
void glColor3f( GLfloat red,GLfloat green,GLfloat blue ) {
  fprintf(stderr,"coin_gl : glColor3f : dummy.\n");
}
void glColor3i( GLint red,GLint green,GLint blue ) {
  fprintf(stderr,"coin_gl : glColor3i : dummy.\n");
}
void glColor3s( GLshort red,GLshort green,GLshort blue ) {
  fprintf(stderr,"coin_gl : glColor3s : dummy.\n");
}
void glColor3ub( GLubyte red,GLubyte green,GLubyte blue ) {
  fprintf(stderr,"coin_gl : glColor3ub : dummy.\n");
}
void glColor3ui( GLuint red,GLuint green,GLuint blue ) {
  fprintf(stderr,"coin_gl : glColor3ui : dummy.\n");
}
void glColor3us( GLushort red,GLushort green,GLushort blue ) {
  fprintf(stderr,"coin_gl : glColor3us : dummy.\n");
}

/* a glColor4f exists for the below : */
void glColor4b( GLbyte red,GLbyte green,GLbyte blue,GLbyte alpha ) {
  fprintf(stderr,"coin_gl : glColor4b : dummy.\n");
}
void glColor4d( GLdouble red,GLdouble green,GLdouble blue,GLdouble alpha ) {
  fprintf(stderr,"coin_gl : glColor4d : dummy.\n");
}
void glColor4i( GLint red,GLint green,GLint blue,GLint alpha ) {
  fprintf(stderr,"coin_gl : glColor4i : dummy.\n");
}
void glColor4s( GLshort red,GLshort green,GLshort blue,GLshort alpha ) {
  fprintf(stderr,"coin_gl : glColor4s : dummy.\n");
}
void glColor4ub( GLubyte red,GLubyte green,GLubyte blue,GLubyte alpha ) {
  fprintf(stderr,"coin_gl : glColor4ub : dummy.\n");
}
void glColor4ui( GLuint red,GLuint green,GLuint blue,GLuint alpha ) {
  fprintf(stderr,"coin_gl : glColor4ui : dummy.\n");
}
void glColor4us( GLushort red,GLushort green,GLushort blue,GLushort alpha ) {
  fprintf(stderr,"coin_gl : glColor4us : dummy.\n");
}
void glColor3bv( const GLbyte *v ) {(void)v;
  fprintf(stderr,"coin_gl : glColor3bv : dummy.\n");
}
void glColor3dv( const GLdouble *v ) {(void)v;
  fprintf(stderr,"coin_gl : glColor3dv : dummy.\n");
}
void glColor3fv( const GLfloat *v ) {(void)v;
  fprintf(stderr,"coin_gl : glColor3fv : dummy.\n");
}
void glColor3iv( const GLint *v ) {(void)v;
  fprintf(stderr,"coin_gl : glColor3iv : dummy.\n");
}
void glColor3sv( const GLshort *v ) {(void)v;
  fprintf(stderr,"coin_gl : glColor3sv : dummy.\n");
}
void glColor3ubv( const GLubyte *v ) {(void)v;
  fprintf(stderr,"coin_gl : glColor3ubv : dummy.\n");
}
void glColor3uiv( const GLuint *v ) {(void)v;
  fprintf(stderr,"coin_gl : glColor3uiv : dummy.\n");
}
void glColor3usv( const GLushort *v ) {(void)v;
  fprintf(stderr,"coin_gl : glColor3usv : dummy.\n");
}
void glColor4bv( const GLbyte *v ) {(void)v;
  fprintf(stderr,"coin_gl : glColor4bv : dummy.\n");
}
void glColor4dv( const GLdouble *v ) {(void)v;
  fprintf(stderr,"coin_gl : glColor4dv : dummy.\n");
}
void glColor4fv( const GLfloat *v ) {(void)v;
  fprintf(stderr,"coin_gl : glColor4fv : dummy.\n");
}
void glColor4iv( const GLint *v ) {(void)v;
  fprintf(stderr,"coin_gl : glColor4iv : dummy.\n");
}
void glColor4sv( const GLshort *v ) {(void)v;
  fprintf(stderr,"coin_gl : glColor4sv : dummy.\n");
}
void glColor4ubv( const GLubyte *v ) {(void)v;
  fprintf(stderr,"coin_gl : glColor4ubv : dummy.\n");
}
void glColor4uiv( const GLuint *v ) {(void)v;
  fprintf(stderr,"coin_gl : glColor4uiv : dummy.\n");
}
void glColor4usv( const GLushort *v ) {(void)v;
  fprintf(stderr,"coin_gl : glColor4usv : dummy.\n");
}

/* a glNormal3f exists for the below : */
void glNormal3b( GLbyte nx,GLbyte ny,GLbyte nz ) {
  fprintf(stderr,"coin_gl : glNormal3b : dummy.\n");
}
void glNormal3d( GLdouble nx,GLdouble ny,GLdouble nz ) {
  fprintf(stderr,"coin_gl : glNormal3d : dummy.\n");
}
void glNormal3i( GLint nx,GLint ny,GLint nz ) {
  fprintf(stderr,"coin_gl : glNormal3i : dummy.\n");
}
void glNormal3s( GLshort nx,GLshort ny,GLshort nz ) {
  fprintf(stderr,"coin_gl : glNormal3s : dummy.\n");
}

void glNormal3bv( const GLbyte *v ) {(void)v;
  fprintf(stderr,"coin_gl : glNormal3bv : dummy.\n");
}
void glNormal3dv( const GLdouble *v ) {(void)v;
  fprintf(stderr,"coin_gl : glNormal3dv : dummy.\n");
}
void glNormal3iv( const GLint *v ) {(void)v;
  fprintf(stderr,"coin_gl : glNormal3iv : dummy.\n");
}
void glNormal3sv( const GLshort *v ) {(void)v;
  fprintf(stderr,"coin_gl : glNormal3sv : dummy.\n");
}

void glPolygonMode(GLenum a_face,GLenum a_mode) {(void)a_face;(void)a_mode;
  fprintf(stderr,"coin_gl : glPolygonMode : dummy.\n");
}
void glPolygonStipple( const GLubyte *mask ) {(void)mask;
  fprintf(stderr,"coin_gl : glPolygonStipple : dummy.\n");
}
void glLineStipple( GLint a_factor,GLushort a_pattern) {(void)a_factor;(void)a_pattern;
  fprintf(stderr,"coin_gl : glLineStipple : dummy.\n");
}

void glIndexd( GLdouble c ) {(void)c;
  fprintf(stderr,"coin_gl : glIndexd : dummy.\n");
}
void glIndexf( GLfloat c ) {(void)c;
  fprintf(stderr,"coin_gl : glIndexf : dummy.\n");
}
void glIndexi( GLint c ) {(void)c;
  fprintf(stderr,"coin_gl : glIndexi : dummy.\n");
}
void glIndexs( GLshort c ) {(void)c;
  fprintf(stderr,"coin_gl : glIndexs : dummy.\n");
}
void glIndexub( GLubyte c ) {(void)c;
  fprintf(stderr,"coin_gl : glIndexub : dummy.\n");
}

void glIndexdv( const GLdouble *c ) {(void)c;
  fprintf(stderr,"coin_gl : glIndexdv : dummy.\n");
}
void glIndexfv( const GLfloat *c ) {(void)c;
  fprintf(stderr,"coin_gl : glIndexfv : dummy.\n");
}
void glIndexiv( const GLint *c ) {(void)c;
  fprintf(stderr,"coin_gl : glIndexiv : dummy.\n");
}
void glIndexsv( const GLshort *c ) {(void)c;
  fprintf(stderr,"coin_gl : glIndexsv : dummy.\n");
}
void glIndexubv( const GLubyte *c ) {(void)c;
  fprintf(stderr,"coin_gl : glIndexubv : dummy.\n");
}

void glPushAttrib( GLbitfield a_mask ) {(void)a_mask;
  fprintf(stderr,"coin_gl : glPushAttrib : dummy.\n");
}
void glPopAttrib() {
  fprintf(stderr,"coin_gl : glPopAttrib : dummy.\n");
}

void glPushClientAttrib( GLbitfield a_mask ) {(void)a_mask;
  fprintf(stderr,"coin_gl : glPushClientAttrib : dummy.\n");
}
void glPopClientAttrib() {
  fprintf(stderr,"coin_gl : glPopClientAttrib : dummy.\n");
}

void glPixelTransferf( GLenum pname,GLfloat param ) {
  fprintf(stderr,"coin_gl : glPixelTransferf : dummy.\n");
}
void glPixelTransferi( GLenum pname,GLint param ) {
  fprintf(stderr,"coin_gl : glPixelTransferi : dummy.\n");
}

void glPixelMapfv( GLenum map,GLint mapsize,const GLfloat *values ) {
  fprintf(stderr,"coin_gl : glPixelMapfv : dummy.\n");
}
void glPixelMapuiv( GLenum map,GLint mapsize,const GLuint *values ) {
  fprintf(stderr,"coin_gl : glPixelMapuiv : dummy.\n");
}
void glPixelMapusv( GLenum map,GLint mapsize,const GLushort *values ) {
  fprintf(stderr,"coin_gl : glPixelMapusv : dummy.\n");
}

void glClearIndex( GLfloat c ) {(void)c;
  fprintf(stderr,"coin_gl : glClearIndex : dummy.\n");
}

void glPixelZoom( GLfloat xfactor,GLfloat yfactor ) {
  fprintf(stderr,"coin_gl : glPixelZoom : dummy.\n");
}

void glRasterPos2d( GLdouble x,GLdouble y ) {
  fprintf(stderr,"coin_gl : glRa : dummy.\n");
}
void glRasterPos2f( GLfloat x,GLfloat y ) {
  fprintf(stderr,"coin_gl : glRa : dummy.\n");
}
void glRasterPos2i( GLint x,GLint y ) {
  fprintf(stderr,"coin_gl : glRa : dummy.\n");
}
void glRasterPos2s( GLshort x,GLshort y ) {
  fprintf(stderr,"coin_gl : glRa : dummy.\n");
}
void glRasterPos3d( GLdouble x,GLdouble y,GLdouble z ) {
  fprintf(stderr,"coin_gl : glRa : dummy.\n");
}
void glRasterPos3f( GLfloat x,GLfloat y,GLfloat z ) {
  fprintf(stderr,"coin_gl : glRa : dummy.\n");
}
void glRasterPos3i( GLint x,GLint y,GLint z ) {
  fprintf(stderr,"coin_gl : glRa : dummy.\n");
}
void glRasterPos3s( GLshort x,GLshort y,GLshort z ) {
  fprintf(stderr,"coin_gl : glRa : dummy.\n");
}
void glRasterPos4d( GLdouble x,GLdouble y,GLdouble z,GLdouble w ) {
  fprintf(stderr,"coin_gl : glRa : dummy.\n");
}
void glRasterPos4f( GLfloat x,GLfloat y,GLfloat z,GLfloat w ) {
  fprintf(stderr,"coin_gl : glRa : dummy.\n");
}
void glRasterPos4i( GLint x,GLint y,GLint z,GLint w ) {
  fprintf(stderr,"coin_gl : glRa : dummy.\n");
}
void glRasterPos4s( GLshort x,GLshort y,GLshort z,GLshort w ) {
  fprintf(stderr,"coin_gl : glRa : dummy.\n");
}

void glRasterPos2dv( const GLdouble *v ) {
  fprintf(stderr,"coin_gl : glRa : dummy.\n");
}
void glRasterPos2fv( const GLfloat *v ) {
  fprintf(stderr,"coin_gl : glRa : dummy.\n");
}
void glRasterPos2iv( const GLint *v ) {
  fprintf(stderr,"coin_gl : glRa : dummy.\n");
}
void glRasterPos2sv( const GLshort *v ) {
  fprintf(stderr,"coin_gl : glRa : dummy.\n");
}
void glRasterPos3dv( const GLdouble *v ) {
  fprintf(stderr,"coin_gl : glRa : dummy.\n");
}
void glRasterPos3fv( const GLfloat *v ) {
  fprintf(stderr,"coin_gl : glRa : dummy.\n");
}
void glRasterPos3iv( const GLint *v ) {
  fprintf(stderr,"coin_gl : glRa : dummy.\n");
}
void glRasterPos3sv( const GLshort *v ) {
  fprintf(stderr,"coin_gl : glRa : dummy.\n");
}
void glRasterPos4dv( const GLdouble *v ) {
  fprintf(stderr,"coin_gl : glRa : dummy.\n");
}
void glRasterPos4fv( const GLfloat *v ) {
  fprintf(stderr,"coin_gl : glRa : dummy.\n");
}
void glRasterPos4iv( const GLint *v ) {
  fprintf(stderr,"coin_gl : glRa : dummy.\n");
}
void glRasterPos4sv( const GLshort *v ) {
  fprintf(stderr,"coin_gl : glRa : dummy.\n");
}
void glBitmap( GLsizei width,GLsizei height,GLfloat xorig,GLfloat yorig,GLfloat xmove,GLfloat ymove,const GLubyte *bitmap ) {
  fprintf(stderr,"coin_gl : glBitmap : dummy.\n");
}
void glDrawPixels( GLsizei width,GLsizei height,GLenum format,GLenum type,const GLvoid *pixels ) {
  fprintf(stderr,"coin_gl : glDrawPixels : dummy.\n");
}

GLuint glGenLists(GLsizei a_range) {
  fprintf(stderr,"coin_gl : glGenLists : dummy : range %d\n",a_range);
  (void)a_range;
  return 1;
}
void glDeleteLists(GLuint a_list,GLsizei a_range) {(void)a_list;(void)a_range;
  fprintf(stderr,"coin_gl : glDeleteLists : dummy.\n");
}
void glNewList(GLuint a_list,GLenum a_mode) {(void)a_list;(void)a_mode;
  fprintf(stderr,"coin_gl : glNewList : dummy.\n");
}
void glEndList() {
  fprintf(stderr,"coin_gl : glEndList : dummy.\n");
}
GLboolean glIsList(GLuint a_list) {(void)a_list;return GL_FALSE;
  fprintf(stderr,"coin_gl : glIsList : dummy.\n");
}
void glCallList(GLuint a_list) {(void)a_list;
  fprintf(stderr,"coin_gl : glCallList : dummy.\n");
}

/* a f version exists for the below : */
void glOrtho( GLdouble left,GLdouble right,GLdouble bottom,GLdouble top,GLdouble zNear,GLdouble zFar ) {
  glOrthof((GLfloat)left,(GLfloat)right,(GLfloat)bottom,(GLfloat)top,(GLfloat)zNear,(GLfloat) zFar);
}

void glRotated( GLdouble angle,GLdouble x,GLdouble y,GLdouble z ) {
  fprintf(stderr,"coin_gl : glRotated : dummy.\n");
}

void glMateriali( GLenum face,GLenum pname,GLint param ) {
  fprintf(stderr,"coin_gl : glMateriali : dummy.\n");
}

void glMaterialiv( GLenum face,GLenum pname,const GLint *params ) {
  fprintf(stderr,"coin_gl : glMaterialiv : dummy.\n");
}

void glFogi( GLenum pname,GLint param ) {
  fprintf(stderr,"coin_gl : glFogi : dummy.\n");
}
void glFogiv( GLenum pname,const GLint *params ) {
  fprintf(stderr,"coin_gl : glFogiv : dummy.\n");
}

void glLightModeli(GLenum a_pname,GLint a_param) {
  fprintf(stderr,"coin_gl : glLightModeli : dummy : pname %x, param %x.\n",a_pname,a_param);
}
void glLightModeliv( GLenum pname,const GLint *params ) {
  fprintf(stderr,"coin_gl : glLightModeliv : dummy.\n");
}

void glClipPlane(GLenum a_plane,const GLdouble* a_equation){(void)a_plane;(void)a_equation;
  fprintf(stderr,"coin_gl : glClipPlane : dummy.\n");
}

void glLoadMatrixd(const GLdouble* a_m) {(void)a_m;
  fprintf(stderr,"coin_gl : glLoadMatrixd : dummy.\n");
}
void glTranslated(GLdouble a_x,GLdouble a_y,GLdouble a_z) {(void)a_x;(void)a_y;(void)a_z;
  fprintf(stderr,"coin_gl : glTranslated : dummy.\n");
}
void glScaled(GLdouble a_x,GLdouble a_y,GLdouble a_z) {(void)a_x;(void)a_y;(void)a_z;
  fprintf(stderr,"coin_gl : glScaled : dummy.\n");
}

/*////////////////////////////////////////////////////////////////////*/
/*/// a (not thread safe) implementation of glBegin/glEnd :///////////*/
/*////////////////////////////////////////////////////////////////////*/
/*////////////////////////////////////////////////////////////////////*/
#define MAX_PTS 1000000
static float s_xyzs[3*MAX_PTS];
static float s_nms[3*MAX_PTS];

static int s_mode = -1;
static float* s_xyzs_pos = 0;
static float* s_nms_pos = 0;

void glBegin(GLenum a_mode){
  if(s_mode!=(-1)) {
    fprintf(stderr,"coin_gl : glBegin(%d) : already active !\n",a_mode);
    return;
  }
  s_mode = (int)a_mode;
  s_xyzs_pos = s_xyzs;
  s_nms_pos = s_nms;
}
void glVertex3fv(const GLfloat* a_xyzs) {
  ptrdiff_t n_xyzs = 0;
  if(s_mode==(-1)) {
    fprintf(stderr,"coin_gl : glVertex3fv : glBegin not active.\n");
    return;
  }
  n_xyzs = s_xyzs_pos-s_xyzs;
  if((n_xyzs+3)>=(3*MAX_PTS)) {
    fprintf(stderr,"coin_gl : glVertex3fv : s_xyzs overflow.\n");
    return;
  }
  *s_xyzs_pos = a_xyzs[0];s_xyzs_pos++;
  *s_xyzs_pos = a_xyzs[1];s_xyzs_pos++;
  *s_xyzs_pos = a_xyzs[2];s_xyzs_pos++;
}
void glNormal3fv(const GLfloat* a_nms) {
  ptrdiff_t n_nms = 0;
  if(s_mode==(-1)) {
    /*fprintf(stderr,"coin_gl : glNormal3fv : glBegin not active : do a glNormal3f.\n");*/
    glNormal3f(a_nms[0],a_nms[1],a_nms[2]);
    return;
  }
  n_nms = s_nms_pos-s_nms;
  if((n_nms+3)>=(3*MAX_PTS)) {
    fprintf(stderr,"coin_gl : glNormal3fv : s_nms overflow.\n");
    return;
  }
  *s_nms_pos = a_nms[0];s_nms_pos++;
  *s_nms_pos = a_nms[1];s_nms_pos++;
  *s_nms_pos = a_nms[2];s_nms_pos++;
}
void glEnd(){
  ptrdiff_t n_nms,n_xyzs;
  GLsizei elems;
  GLenum mode;

  if(s_mode==(-1)) {
    fprintf(stderr,"coin_gl : glEnd : glBegin not active.\n");
    return;
  }
  
  n_nms = s_nms_pos-s_nms;
  n_xyzs = s_xyzs_pos-s_xyzs;

  /*
  if(n_nms!=n_xyzs) {
    fprintf(stderr,"coin_gl : glEnd : n_nms (%lu) != n_xyzs (%lu).\n",n_nms,n_xyzs);
    return;
  }
  */

  glColor4f(1,0,0,1);

  elems = (GLsizei)(n_xyzs/3);
  if(!elems) return;

/*fprintf(stderr,"coin_gl : glEnd : elems %d.\n",elems);*/

  mode = GL_POINTS;
  /*
  mode = GL_LINES;
  mode = GL_TRIANGLES;
  */

  if(n_nms) {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glVertexPointer(3,GL_FLOAT,0,s_xyzs);
    glNormalPointer(GL_FLOAT,0,s_nms);
    glDrawArrays(mode,0,elems);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
  } else {
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3,GL_FLOAT,0,s_xyzs);
    glDrawArrays(mode,0,elems);
    glDisableClientState(GL_VERTEX_ARRAY);
  }

  s_mode = -1;
}

