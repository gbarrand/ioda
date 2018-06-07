/* Copyright (C) 2010, Guy Barrand. All rights reserved. */
/* See the file inlib.license for terms.                 */

/* OpenGL functions needed to link ioda built with CoinGL built by using Xcode.
*/

#include "gldef.h"

#include <stdio.h>

void glPassThrough( GLfloat a_token ){
  fprintf(stderr,"offscreen_gl : glPassThrough : dummy : token %g\n",a_token);
  (void)a_token;
}

