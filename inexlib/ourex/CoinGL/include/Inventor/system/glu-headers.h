#ifndef COIN_GLU_HEADERS_H
#define COIN_GLU_HEADERS_H

/*G.Barrand : for Nurbs. Used in HEPVis/SoPlotter and OpenPAW.*/

#ifdef OUREX_COINGL_INLIB_GL
#include <inlib/gldum/coin_gl.h>
#include <inlib/gldum/glu.h>
#else

#include "gl-headers.h"

#if TARGET_OS_IPHONE
#elif defined(ANDROID)
#elif defined(_WIN32)
#include <GL/glu.h> 
#undef GL_EXT_paletted_texture 
#elif defined(__APPLE__)
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#endif

#endif
