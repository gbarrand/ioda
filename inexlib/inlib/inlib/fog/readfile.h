//////////////////////////////////////////////////////////////////////////
//                          readFile.h                                  //
//                         ------------                                 //
//    begin                : Thu Sep 11 2003                            //
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


#ifndef inlib_fog_readfile_h
#define inlib_fog_readfile_h

#include "structure.h"

/**
 * @brief Reads modelisation files.
 *
 * Reads a file in the "dat" format and extracts
 * its information to the structure scene.
 *
 * Structure of the file containing the modelisation:
 * nb super-volumes  (int)
 * nb objects        (int)
 * nb faces          (int)
 * nb points         (int)
 * [points] definition for nb points lines (3 floats)
 * [faces] definition for 2*nb faces lines (2 ints - nbf imat -then nbf pts ints)
 * XXX -> nb points for this face XXX -> matter index
 * XX XX XX XX -> points index
 * [objects] definition for 2*nb objects lines (1 int then nbpo ints)
 * XXX -> nb faces for object
 * XX XX XX XX -> faces index
 * [super volumes] definition for 2*nsv lines (1 int then nbobjS ints)
 * XXX -> nb objects
 * XX XX XX XX -> objects index
 * NB: each piece of information is on NB_SPACES spaces
 *
 */
 

class ReadFile 
{
private:

  static int readInt (unsigned int nbchar, char * s, int line,int&);
    static double readDouble (unsigned int nbchar, char * s, int line,int&);
    static char * readString (unsigned int nbchar, char * s, int line,int&);
    static char * subString (char *str, unsigned int start, unsigned int end,int&);
 
public:

    static int readModelisationFile (const char name[], SCENE * scene);
    //static int addModelisationFile  (const char name[], SCENE * scene);
    static int addModelisationFile(unsigned int a_sz,const char* a_stream,
                                   const char name[],SCENE* scene);
    static void displayData (SCENE * scene,int detail);

};

#include "readfile.icc"

#endif

