//////////////////////////////////////////////////////////////////////////
//                        structure.h                                   //
//                       -------------                                  //
//    begin                : Tue Nov 4 2003                             //
//    copyright            : (C) 2003 by cbaudry (CEA/DSM/SPP)          //
//    email                : cedric.baudry@tremplin-utc.fr              //
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//   This program is free software; you can redistribute it and/or modify //
//   it under the terms of the GNU General Public License as published by //
//   the Free Software Foundation, version 2 of the License               //
//                                                                        //
////////////////////////////////////////////////////////////////////////////


#ifndef inlib_fog_structure_h
#define inlib_fog_structure_h

/** point index */
typedef int INDEX_POINT;
/** color index */
typedef int INDEX_COLOR;
/** face index */
typedef int INDEX_FACE;
/** object index */
typedef int INDEX_OBJECT;


/**
 * stores homogene coordinates
 */
class fog_vec4d
{
public :
 
  double x;
  double y;
  double z;
  double w;
};


/**
 * stores screen coordinates
 */
class fog_vec2i
{
public:
    int x,y;
};

/**
 * homogene fog_vec4f as a table of 4 float
 * could be replaced by vec4
 */
typedef float fog_vec4f[4];

/**
 * A way to store affine transformations with no scale
 * quite compact because there are only 6 doubles
 * but only rotation around origin are taken.
 */
class Transfo
{
public :
  double tx;               /* translation */
  double ty;
  double tz;
  double thetax;           /* rotation */
  double thetay;
  double thetaz;
};

/**
 * stores RGB color data
 */
class Rgb
{
public :
  double r;        /* red */
  double g;        /* green */
  double b;        /* blue */
};

/**
 * stores face data
 */
typedef struct
{
  int nbpt;               /* number of points in the face */
  INDEX_POINT *tabpt;     /* table of the index of points being in the face */
  fog_vec4d n;                /* face normal */
  INDEX_COLOR icol;       /* color index */
  void* extension;        /* G.Barrand : to store GL triangles. */
}
FACE;

/**
 * stores object data
 */
typedef struct
{
  int nbface;             /* number of faces being in the object */
  INDEX_FACE *tabface;    /* table of the index of faces being in the object */
}
OBJECT;

/**
 * stores super volume data
 */
typedef struct
{
  int nbobj;              /* number of objects being in the super volume */
  INDEX_OBJECT *tabobj;   /* table of the index of objects being in the super volume */
  int visible;            /* visible property initialised to 1 (everything visible) */
  Transfo transfo;        /* translation and rotation of the super volume */
  char * name;
  char * filename;
  int displayListNumber;
}
SVOLUME;

/**
 * stores scene data
 */
typedef struct
{
  int nbsv;        /* number of super volumes */
  SVOLUME *tabsv;  /* table of super volumes */
  int nbobj;       /* number of objects */
  OBJECT *tabobj;  /* table of the objects */
  int nbface;      /* number of faces */
  FACE *tabface;   /* table of the faces */
  int nbpt;        /* number of points */
  fog_vec4d *tabpt;    /* table of coordinates of the points */
  int nbcol;       /* number of colors */
  Rgb *tabcol;     /* table of the colors */
}
SCENE;

#endif
