//////////////////////////////////////////////////////////////////////////
//                          material.h                                  //
//                         ------------                                 //
//    begin                : Thu Oct 09 2003                            //
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


#ifndef inlib_fog_material_h
#define inlib_fog_material_h

inline int NB_COLORS() {return 13;}

inline const double* get_col(unsigned int a_index) {
  static const unsigned int NB_COLORS = 13;
  static const double coltable[NB_COLORS][3] = {
    {1.00, 1.00, 1.00},         /* White */
    {0.00, 0.00, 0.00},         /* Black  */
    {1.00, 0.85, 0.70},         /* PeachPuff */
    {0.90, 0.50, 0.30},         /* Peach */
    {0.44, 0.50, 0.56},         /* SlateGrey */
    {0.39, 0.58, 0.93},         /* CornflowerBlue */
    {0.50, 0.50, 0.83},         /* Aquamarine */
    {0.49, 0.92, 0.00},         /* LawnGreen */
    {0.74, 0.71, 0.42},         /* DarkKhaki */
    {1.00, 0.84, 0.00},         /* Gold */
    {0.80, 0.36, 0.36},         /* IndianRed */
    {0.55, 0.27, 0.07},         /* SaddleBrown */
    {1.00, 0.08, 0.56}         /* DeepPink */
  };
  if(a_index>=NB_COLORS) {
    //we have some case of a_index = NB_COLORS (TGC.fog)
    a_index = 5;/*CornflowerBlue*/
  }
  return coltable[a_index];
}



/* #define NUM_MATERIALS 28
// Materials serves as an index with one entry for
// each of the NUM_MATERIALS materials, see material.h
enum materials
{

    EMERALD, JADE, OBSIDIAN, PEARL, RUBY, TURQUOISE,

    BRASS, BRONZE, CHROME, COPPER, GOLD, SILVER,

    BLACK_PLASTIC, CYAN_PLASTIC, GREEN_PLASTIC,

    RED_PLASTIC, WHITE_PLASTIC, YELLOW_PLASTIC,

    BLACK_RUBBER, CYAN_RUBBER, GREEN_RUBBER,

    RED_RUBBER, WHITE_RUBBER, YELLOW_RUBBER,

    BRIGHT_WHITE, LESS_BRIGHT_WHITE,

    WARM_WHITE, COOL_WHITE
};

// material names so we can get stringdescriptions

static char *matNames[NUM_MATERIALS][50] = {

    "EMERALD", "JADE", "OBSIDIAN", "PEARL", "RUBY", "TURQUOISE",

    "BRASS", "BRONZE", "CHROME", "COPPER", "GOLD", "SILVER",

    "BLACK_PLASTIC", "CYAN_PLASTIC", "GREEN_PLASTIC",

    "RED_PLASTIC", "WHITE_PLASTIC", "YELLOW_PLASTIC",

    "BLACK_RUBBER", "CYAN_RUBBER", "GREEN_RUBBER",

    "RED_RUBBER", "WHITE_RUBBER", "YELLOW_RUBBER",

    "BRIGHT_WHITE", "LESS_BRIGHT_WHITE",

    "WARM_WHITE", "COOL_WHITE"
};
*/

// description of materials, 10 floating values each:
// ambient(RGB), diffuse(RGB), specular(RGB), shininess

/*
static const float Material[NUM_MATERIALS][10] = {

    //EMERALD

   { 0.0215f, 0.1745f, 0.0215f,

    0.07568f, 0.61424f, 0.07568f,

    0.633f, 0.727811f, 0.633f,

    0.6f},

    //JADE

   { 0.135f, 0.2225f, 0.1575f,

    0.54f, 0.89f, 0.63f,

    0.316228f, 0.316228f, 0.316228f,

     0.1f},

    //OBSIDIAN

   { 0.05375f, 0.05f, 0.06625f,

    0.18275f, 0.17f, 0.22525f,

    0.332741f, 0.328634f, 0.346435f,

    0.3f},

    //PEARL

   { 0.25f, 0.20725f, 0.20725f,

    1.0f, 0.829f, 0.829f,

    0.296648f, 0.296648f, 0.296648f,

    0.088f},

    //RUBY

   { 0.1745f, 0.01175f, 0.01175f,

    0.61424f, 0.04136f, 0.04136f,

    0.727811f, 0.626959f, 0.626959f,

    0.6f},

    //TURQUOISE

   { 0.1f, 0.18725f, 0.1745f,

    0.396f, 0.74151f, 0.69102f,

    0.297254f, 0.30829f, 0.306678f,

    0.1f},

    //BRASS

   { 0.329412f, 0.223529f, 0.027451f,

    0.780392f, 0.568627f, 0.113725f,

    0.992157f, 0.941176f, 0.807843f,

    0.21794872f},

    //BRONZE

   { 0.2125f, 0.1275f, 0.054f,

    0.714f, 0.4284f, 0.18144f,

    0.393548f, 0.271906f, 0.166721f,

    0.2f},

    //CHROME

   { 0.25f, 0.25f, 0.25f,

    0.4f, 0.4f, 0.4f,

    0.774597f, 0.774597f, 0.774597f,

    0.6f},

    //COPPER

   { 0.19125f, 0.0735f, 0.0225f,

    0.7038f, 0.27048f, 0.0828f,

    0.256777f, 0.137622f, 0.086014f,

    0.1f},

    //GOLD

   { 0.24725f, 0.1995f, 0.0745f,

    0.75164f, 0.60648f, 0.22648f,

    0.628281f, 0.555802f, 0.366065f,

    0.4f},

    //SILVER

    {0.19225f, 0.19225f, 0.19225f,

    0.50754f, 0.50754f, 0.50754f,

    0.508273f, 0.508273f, 0.508273f,

    0.4f},

    //BLACK PLASTIC

    {0.0f, 0.0f, 0.0f,

    0.01f, 0.01f, 0.01f,

    0.50f, 0.50f, 0.50f,

    0.25f},

    //CYAN PLASTIC

   { 0.0f, 0.1f, 0.06f,

    0.0f, 0.50980392f, 0.50980392f,

    0.50196078f, 0.50196078f, 0.50196078f,

    0.25f},

    //GREEN PLASTIC

   { 0.0f, 0.0f, 0.0f,

    0.1f, 0.35f, 0.1f,

    0.45f, 0.55f, 0.45f,

    0.25f},

    //RED PLASTIC

   { 0.0f, 0.0f, 0.0f,

    0.5f, 0.0f, 0.0f,

    0.7f, 0.6f, 0.6f,

    0.25f},

    //WHITE PLASTIC

   { 0.0f, 0.0f, 0.0f,

    0.55f, 0.55f, 0.55f,

    0.70f, 0.70f, 0.70f,

    0.25f},

    //YELLOW PLASTIC

   { 0.0f, 0.0f, 0.0f,

    0.5f, 0.5f, 0.0f,

    0.60f, 0.60f, 0.50f,

    0.25f},

    //BLACK RUBBER

   { 0.02f, 0.02f, 0.02f,

    0.01f, 0.01f, 0.01f,

    0.4f, 0.4f, 0.4f,

    0.078125f},

    //CYAN RUBBER

   { 0.0f, 0.05f, 0.05f,

    0.4f, 0.5f, 0.5f,

    0.04f, 0.7f, 0.7f,

    0.078125f},

    //GREEN RUBBER

   { 0.0f, 0.05f, 0.0f,

    0.4f, 0.5f, 0.4f,

    0.04f, 0.7f, 0.04f,

    0.078125f},

    //RED RUBBER

   { 0.05f, 0.0f, 0.0f,

    0.5f, 0.4f, 0.4f,

    0.7f, 0.04f, 0.04f,

    0.078125f},

    //WHITE RUBBER

   { 0.05f, 0.05f, 0.05f,

    0.5f, 0.5f, 0.5f,

    0.7f, 0.7f, 0.7f,

    0.078125f},

    //YELLOW RUBBER

   { 0.05f, 0.05f, 0.0f,

    0.5f, 0.5f, 0.4f,

    0.7f, 0.7f, 0.04f,

    0.078125f},

    // bright white

   { 0.2f, 0.2f, 0.2f,

    1.0f, 1.0f, 1.0f,

    0.8f, 0.8f, 0.8f,

    0.4f},

    // less bright white

   { 0.2f, 0.2f, 0.2f,

    0.8f, 0.8f, 0.8f,

    0.5f, 0.5f, 0.5f,

    0.35f},

    // warmish white

   { 0.3f, 0.2f, 0.2f,

    1.0f, 0.9f, 0.8f,

    0.4f, 0.2f, 0.2f,

    0.35f},

    // coolish white

   { 0.2f, 0.2f, 0.3f,

    0.8f, 0.9f, 1.0f,

    0.2f, 0.2f, 0.4f,

     0.35f}
};
*/

//G.Barrand void doMaterial (GLfloat * pMat, GLenum gSide);
/*
inline void doMaterial (float * pMat, GLenum gSide)
{

    glMaterialfv (gSide, GL_AMBIENT, pMat);

    pMat += 3;

    glMaterialfv (gSide, GL_DIFFUSE, pMat);

    pMat += 3;

    glMaterialfv (gSide, GL_SPECULAR, pMat);

    pMat++;

    glMaterialf (gSide, GL_SHININESS, *pMat * 128.0f);

}
*/

#endif
