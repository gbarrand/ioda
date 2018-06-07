/* Copyright (C) 2010, Guy Barrand. All rights reserved. */
/* See the file inlib.license for terms.                 */

/* OpenGL functions used in ourex/CoinGL,HEPVis but are not in coin_gl.c.
   This file is used to link "ioda -offscreen" that uses ourex/[CoinGL,HEPVis]
   that are themselves built with -no_gl.
*/

#ifdef COIN_INTERNAL
#include "Inventor/system/gl-headers.h"
#else
#include "gldef.h"
#endif

#include <stdio.h>
#include <stddef.h>

void glFlush() {
  fprintf(stderr,"offscreen_gl : glFlush : dummy\n");
}
void glGetFloatv(GLenum a_pname,GLfloat* a_1){
  fprintf(stderr,"offscreen_gl : glGetFloatv : dummy : pname %d\n",a_pname);
  (void)a_1;
}
void glGetIntegerv(GLenum a_pname,GLint* a_1){
  fprintf(stderr,"offscreen_gl : glGetIntegerv : dummy : pname %d\n",a_pname);
  (void)a_1;
}
void glGetBooleanv(GLenum a_pname,GLboolean* a_1){
  fprintf(stderr,"offscreen_gl : glGetBooleanv : dummy : pname %d\n",a_pname);
  (void)a_1;
}

void glPassThrough( GLfloat a_token ){
  fprintf(stderr,"offscreen_gl : glPassThrough : dummy : token %g\n",a_token);
  (void)a_token;
}

/*GLboolean glIsEnabled( GLenum cap ){return 0;}*/
void glFeedbackBuffer(GLsizei size,GLenum type,GLfloat *buffer ){}
/*const GLubyte * glGetString( GLenum name ) {return 0;}*/
GLint glRenderMode( GLenum mode ){return 0;}
/*
void glReadPixels( GLint x,
                   GLint y,
                   GLsizei width,
                   GLsizei height,
                   GLenum format,
                   GLenum type,
		   GLvoid *pixels ){}
*/
void glOrthof( GLfloat left,GLfloat right,GLfloat bottom,GLfloat top,GLfloat zNear,GLfloat zFar){}
void glFogf( GLenum pname,GLfloat param ) {}
void glFogfv( GLenum pname,const GLfloat *params ) {}

void glDepthFunc(GLenum a_func) {(void)a_func;}
void glAlphaFunc(GLenum a_func,GLclampf a_ref) {(void)a_func;(void)a_ref;}

void glCopyTexSubImage2D(GLenum a_target,GLint a_level,
                                GLint a_xoffset,GLint a_yoffset,
                                GLint a_x,GLint a_y,GLsizei a_width,GLsizei a_height) {
  (void)a_target;
  (void)a_level;
  (void)a_xoffset;(void)a_yoffset;
  (void)a_x;(void)a_y;
  (void)a_width;(void)a_height;
}
void glCopyTexImage2D(GLenum a_target,GLint a_level,GLenum a_internalformat,
                             GLint a_x,GLint a_y,GLsizei a_width,GLsizei a_height,GLint a_border) {
  (void)a_target;
  (void)a_level;
  (void)a_internalformat;
  (void)a_x;(void)a_y;
  (void)a_width;(void)a_height;
  (void)a_border;
}

void glCullFace(GLenum a_mode) {(void)a_mode;}

void glLightModelfv( GLenum pname,const GLfloat *params ) {}

void glMaterialf( GLenum face,GLenum pname,GLfloat param ) {}
void glMaterialfv( GLenum face,GLenum pname,const GLfloat *params ) {}

void glPushMatrix() {}
void glPopMatrix() {}

void glRotatef( GLfloat angle,GLfloat x,GLfloat y,GLfloat z ) {}

void glScalef(GLfloat a_x,GLfloat a_y,GLfloat a_z) {(void)a_x;(void)a_y;(void)a_z;}
void glScissor( GLint x,GLint y,GLsizei width,GLsizei height ) {}

void glTexEnvf(GLenum a_target,GLenum a_pname,GLfloat a_param) {(void)a_target;(void)a_pname;(void)a_param;}
void glTexEnvi(GLenum a_target,GLenum a_pname,GLint a_param) {(void)a_target;(void)a_pname;(void)a_param;}
void glTexEnvfv( GLenum target,
                        GLenum pname,
                        const GLfloat *params ) {}
void glTexParameteri(GLenum a_0,GLenum a_1,GLint a_2){
  (void)a_0;(void)a_1;(void)a_2;
}
void glTranslatef(GLfloat a_x,GLfloat a_y,GLfloat a_z) {(void)a_x;(void)a_y;(void)a_z;}

/*//////////////////////////////////////////////////////////*/
/*//////////////////////////////////////////////////////////*/
/*//////////////////////////////////////////////////////////*/
GLboolean glIsEnabled(GLenum a_0){(void)a_0;return GL_FALSE;}

void glPointSize(GLfloat a_0){(void)a_0;}
void glClearColor(GLclampf a_0,GLclampf a_1,GLclampf a_2,GLclampf a_3){
  (void)a_0;(void)a_1;(void)a_2;(void)a_3;
}
void glReadPixels(GLint a_0,GLint a_1,GLsizei a_2,GLsizei a_3,GLenum a_4,GLenum a_5,GLvoid* a_6){
  (void)a_0;(void)a_1;(void)a_2;(void)a_3;(void)a_4;(void)a_5;(void)a_6;
}
void glViewport(GLint a_0,GLint a_1,GLsizei a_2,GLsizei a_3){
  (void)a_0;(void)a_1;(void)a_2;(void)a_3;
}
void glPixelStorei(GLenum a_0,GLint a_1){
  (void)a_0;(void)a_1;
}
void glShadeModel(GLenum a_0){(void)a_0;}
void glClear(GLbitfield a_0){(void)a_0;}
void glFinish(){}
void glMatrixMode(GLenum a_0){(void)a_0;}
void glLoadIdentity(){}
void glLineWidth(GLfloat a_0){(void)a_0;}
  /*void glNormal3f(GLfloat a_0,GLfloat a_1,GLfloat a_2){
  (void)a_0;(void)a_1;(void)a_2;
  }*/
void glEnable(GLenum a_0){(void)a_0;}
void glDisable(GLenum a_0){(void)a_0;}
void glLoadMatrixf(const GLfloat* a_0){(void)a_0;}
GLenum glGetError(){return GL_NO_ERROR;}
void glFrontFace(GLenum a_0){(void)a_0;}
  /*void glColor4f(GLfloat a_0,GLfloat a_1,GLfloat a_2,GLfloat a_3){
  (void)a_0;(void)a_1;(void)a_2;(void)a_3;
  }*/
void glBlendFunc(GLenum a_0,GLenum a_1){
  (void)a_0;(void)a_1;
}
void glVertexPointer(GLint a_0,GLenum a_1,GLsizei a_2,const GLvoid* a_3){
  (void)a_0;(void)a_1;(void)a_2;(void)a_3;
}
void glColorPointer(GLint a_0,GLenum a_1,GLsizei a_2,const GLvoid* a_3){
  (void)a_0;(void)a_1;(void)a_2;(void)a_3;
}
void glDrawArrays(GLenum a_0,GLint a_1,GLsizei a_2){
  (void)a_0;(void)a_1;(void)a_2;
}
void glEnableClientState(GLenum a_0){(void)a_0;}
void glDisableClientState(GLenum a_0){(void)a_0;}
void glPolygonOffset(GLfloat a_0,GLfloat a_1){
  (void)a_0;(void)a_1;
}
void glTexImage2D(GLenum a_0,GLint a_1,GLint a_2,GLsizei a_3,GLsizei a_4,GLint a_5,GLenum a_6,GLenum a_7,const GLvoid* a_8){
  (void)a_0;(void)a_1;(void)a_2;(void)a_3;(void)a_4;(void)a_5;(void)a_6;(void)a_7;(void)a_8;
}

const GLubyte* glGetString(GLenum a_name) {(void)a_name;return 0;}
/*void glGetIntegerv(GLenum a_0,GLint* a_1){(void)a_0;(void)a_1;}*/
void glBindTexture(GLenum a_0,GLuint a_1){(void)a_0;(void)a_1;}
void glTexCoordPointer(GLint a_0,GLenum a_1,GLsizei a_2,const GLvoid* a_3){(void)a_0;(void)a_1;(void)a_2;(void)a_3;}
void glDeleteTextures(GLsizei a_0,const GLuint* a_1){(void)a_0;(void)a_1;}
GLboolean glIsTexture(GLuint a_texture) {(void)a_texture;return 0;}
void glColor4f(GLfloat red,GLfloat green,GLfloat blue,GLfloat alpha ) {}
void glNormal3f(GLfloat nx,GLfloat ny,GLfloat nz ) {}
void glNormalPointer(GLenum a_0,GLsizei a_1,const GLvoid* a_2){(void)a_0;(void)a_1;(void)a_2;}
void glLightfv(GLenum a_0,GLenum a_1,const GLfloat* a_2){(void)a_0;(void)a_1;(void)a_2;}
void glLightf(GLenum a_0,GLenum a_1,GLfloat a_2){(void)a_0;(void)a_1;(void)a_2;}
void glTexParameterf(GLenum a_0,GLenum a_1,GLfloat a_2){(void)a_0;(void)a_1;(void)a_2;}
void glGenTextures(GLsizei a_0,GLuint* a_1){(void)a_0;(void)a_1;}
