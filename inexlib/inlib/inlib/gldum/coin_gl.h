/* Copyright (C) 2010, Guy Barrand. All rights reserved. */
/* See the file inlib.license for terms.                 */

#ifndef inlib_gldum_coin_gl_h
#define inlib_gldum_coin_gl_h

/* dummy OpenGL for CoinGL. */

/*/////////////////////////////////////////////////*/
/*/////////////////////////////////////////////////*/
/*/////////////////////////////////////////////////*/
/*#include "gl.h"*/
#include "gldef.h"

#ifdef __cplusplus
extern "C"{
#endif

GLboolean glIsEnabled(GLenum a_0);
void glPointSize(GLfloat a_0);
void glClearColor(GLclampf a_0,GLclampf a_1,GLclampf a_2,GLclampf a_3);
void glReadPixels(GLint a_0,GLint a_1,GLsizei a_2,GLsizei a_3,GLenum a_4,GLenum a_5,GLvoid* a_6);
void glViewport(GLint a_0,GLint a_1,GLsizei a_2,GLsizei a_3);
void glPixelStorei(GLenum a_0,GLint a_1);
void glShadeModel(GLenum a_0);
void glClear(GLbitfield a_0);
void glFinish();
void glMatrixMode(GLenum a_0);
void glLoadIdentity();
void glLineWidth(GLfloat a_0);
/*void glNormal3f(GLfloat a_0,GLfloat a_1,GLfloat a_2);*/
void glEnable(GLenum a_0);
void glDisable(GLenum a_0);
void glLoadMatrixf(const GLfloat* a_0);
GLenum glGetError();
void glFrontFace(GLenum a_0);
/*void glColor4f(GLfloat a_0,GLfloat a_1,GLfloat a_2,GLfloat a_3);*/
void glBlendFunc(GLenum a_0,GLenum a_1);
void glVertexPointer(GLint a_0,GLenum a_1,GLsizei a_2,const GLvoid* a_3);
void glColorPointer(GLint a_0,GLenum a_1,GLsizei a_2,const GLvoid* a_3);
void glDrawArrays(GLenum a_0,GLint a_1,GLsizei a_2);
void glEnableClientState(GLenum a_0);
void glDisableClientState(GLenum a_0);
void glPolygonOffset(GLfloat a_0,GLfloat a_1);
void glTexImage2D(GLenum a_0,GLint a_1,GLint a_2,GLsizei a_3,GLsizei a_4,GLint a_5,GLenum a_6,GLenum a_7,const GLvoid* a_8);

const GLubyte* glGetString(GLenum a_name);
void glGetIntegerv(GLenum a_0,GLint* a_1);
void glBindTexture(GLenum a_0,GLuint a_1);
void glTexCoordPointer(GLint a_0,GLenum a_1,GLsizei a_2,const GLvoid* a_3);
void glDeleteTextures(GLsizei a_0,const GLuint* a_1);
GLboolean glIsTexture(GLuint a_texture);
void glColor4f(GLfloat red,GLfloat green,GLfloat blue,GLfloat alpha );
void glNormal3f(GLfloat nx,GLfloat ny,GLfloat nz );
void glNormalPointer(GLenum a_0,GLsizei a_1,const GLvoid* a_2);
void glLightfv(GLenum a_0,GLenum a_1,const GLfloat* a_2);
void glLightf(GLenum a_0,GLenum a_1,GLfloat a_2);
void glTexParameterf(GLenum a_0,GLenum a_1,GLfloat a_2);
void glGenTextures(GLsizei a_0,GLuint* a_1);

#ifdef __cplusplus
}
#endif
/*/////////////////////////////////////////////////*/
/*/////////////////////////////////////////////////*/
/*/////////////////////////////////////////////////*/

typedef char GLbyte;
#define GL_POINTS                         0x0000
#define GL_LINES                          0x0001
#define GL_LINE_LOOP                      0x0002
#define GL_LINE_STRIP                     0x0003
#define GL_TRIANGLES                      0x0004
#define GL_TRIANGLE_STRIP		  0x0005
#define GL_ZERO				  0x0
#define GL_ONE				  0x1
#define GL_COLOR_CLEAR_VALUE	          0x0C22
#define GL_DST_COLOR              1
#define GL_INVALID_VALUE          1
#define GL_INVALID_ENUM           2
#define GL_LINEAR_MIPMAP_LINEAR   1
#define GL_LINEAR_MIPMAP_NEAREST  1
#define GL_NEAREST_MIPMAP_LINEAR  1
#define GL_NEAREST_MIPMAP_NEAREST 1
#define GL_MAX_VIEWPORT_DIMS    1
#define GL_UNPACK_ALIGNMENT     1
#define GL_REPEAT               1
#define GL_RED_BITS             1
#define GL_GREEN_BITS           1
#define GL_BLUE_BITS            1
#define GL_SPOT_DIRECTION       1
#define GL_CURRENT_COLOR        1
#define GL_ALPHA_SCALE          1

#define GL_LUMINANCE_ALPHA      1
#define GL_INVALID_OPERATION    3
#define GL_STACK_UNDERFLOW      4
#define GL_STACK_OVERFLOW       5
#define GL_OUT_OF_MEMORY        6

#define GL_FRONT_AND_BACK       1
#define GL_FRONT                1
#define GL_BACK                 1
#define GL_DEPTH_BITS           1
#define GL_ALPHA_BITS           1
#define GL_ALPHA_TEST           1
#define GL_TEXTURE              1
#define GL_GREATER              1
#define GL_TEXTURE_ENV          1
#define GL_TEXTURE_ENV_MODE     1
#define GL_ALPHA                1
#define GL_LEQUAL               1
#define GL_UNSIGNED_SHORT       1
#define GL_TEXTURE_WRAP_S       1
#define GL_TEXTURE_WRAP_T       1
#define GL_CLIP_PLANE0          1
#define GL_MAX_CLIP_PLANES      1
#define GL_LIGHT_MODEL_AMBIENT  1
#define GL_FOG                  1
#define GL_FOG_MODE             1
#define GL_FOG_START            1
#define GL_FOG_END              1
#define GL_FOG_MODE             1
#define GL_EXP                  1
#define GL_FOG_DENSITY          1
#define GL_EXP2                 1
#define GL_FOG_COLOR            1

#define GL_SMOOTH               1
#define GL_DECAL                1
#define GL_MODULATE             1
#define GL_TEXTURE_ENV_COLOR    1
#define GL_LIGHT_MODEL_TWO_SIDE 1

#define GL_EMISSION      1
#define GL_SHININESS     1
#define GL_SCISSOR_TEST  1
#define GL_CURRENT_COLOR 1

#ifdef __cplusplus
extern "C"{
#endif
void glOrthof( GLfloat left,GLfloat right,GLfloat bottom,GLfloat top,GLfloat zNear,GLfloat zFar);

void glPushMatrix();
void glPopMatrix();

void glTexEnvf(GLenum a_target,GLenum a_pname,GLfloat a_param);
void glTexEnvi(GLenum a_target,GLenum a_pname,GLint a_param);
void glTexEnvfv(GLenum target,GLenum pname,const GLfloat *params);
void glTexEnviv(GLenum target,GLenum pname,const GLint *params );

void glTranslatef(GLfloat a_x,GLfloat a_y,GLfloat a_z);
void glScalef(GLfloat a_x,GLfloat a_y,GLfloat a_z);
void glAlphaFunc(GLenum a_func,GLclampf a_ref);

void glScissor( GLint x,GLint y,GLsizei width,GLsizei height );
void glFlush();

void glCopyTexSubImage2D(GLenum a_target,GLint a_level,
                         GLint a_xoffset,GLint a_yoffset,
			 GLint a_x,GLint a_y,GLsizei a_width,GLsizei a_height);
void glCopyTexImage2D(GLenum a_target,GLint a_level,GLenum a_internalformat,
                      GLint a_x,GLint a_y,GLsizei a_width,GLsizei a_height,GLint a_border);
void glTexParameteri(GLenum a_0,GLenum a_1,GLint a_2);

void glFogf( GLenum pname,GLfloat param );
void glFogfv( GLenum pname,const GLfloat *params );
void glLightModelf( GLenum pname,GLfloat param );
void glLightModelfv( GLenum pname,const GLfloat *params );

void glMaterialf( GLenum face,GLenum pname,GLfloat param );
void glMaterialfv( GLenum face,GLenum pname,const GLfloat *params );
void glRotatef( GLfloat angle,GLfloat x,GLfloat y,GLfloat z );

void glCullFace(GLenum a_mode);

void glDepthFunc(GLenum a_func);

void glGetFloatv(GLenum a_0,GLfloat* a_1);
void glGetBooleanv(GLenum a_0,GLboolean* a_1);

void glGenBuffers(GLsizei a_0,GLuint* a_1);
void glDeleteBuffers(GLsizei a_0,const GLuint* a_1);
#ifdef __cplusplus
}
#endif

#include "gl4coin.ic"

#endif
