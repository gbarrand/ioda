/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2007 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  ("GPL") version 2 as published by the Free Software Foundation.
 *  See the file LICENSE.GPL at the root directory of this source
 *  distribution for additional information about the GNU GPL.
 *
 *  For using Coin with software that can not be combined with the GNU
 *  GPL, and for taking advantage of the additional benefits of our
 *  support services, please contact Systems in Motion about acquiring
 *  a Coin Professional Edition License.
 *
 *  See http://www.coin3d.org/ for more information.
 *
 *  Systems in Motion, Postboks 1283, Pirsenteret, 7462 Trondheim, NORWAY.
 *  http://www.sim.no/  sales@sim.no  coin-support@coin3d.org
 *
\**************************************************************************/

/* *********************************************************************** */
/* Misc global Doxygen documentation stuff. ****************************** */

/*!
  \mainpage

  <a href="http://www.coin3d.org">Coin</a> is an OpenGL based,
  retained mode 3D graphics rendering library. It is implemented in
  C++ and publicly released with the source code open for your
  perusal. The application programmer's interface (API) is fully
  compatible with SGI's Open Inventor, the \e de \e facto standard 3D
  graphics API for complex visualization applications.

  For an excellently written, detailed, tutorial-style introduction to
  the Open Inventor API used by the Coin library, we highly recommend
  the book �The Inventor Mentor� (subtitle: �Programming
  Object-Oriented 3D Graphics with Open Inventor�), ISBN
  0-201-62495-8. It walks the Coin application programmer through all
  the principles applied in the API, richly illustrated and with
  numerous, well documented code examples.

  (The �Inventor Mentor� is getting a bit old, but don't let that put
  you off. We heartily recommend this book as it covers all the basic
  design principles, aswell as the major parts of the API, in an
  excellent manner.)

  For overviews of various selected features of the Coin library, see
  the "Related pages" document from the link in the upper right corner
  of this page.

  \link coin250 A summary of the new features for Coin v2.5.0 can be found here. \endlink

  Systems in Motion is working on providing libraries for interfacing
  Coin with a wide range of windowing systems and GUI toolkits.
  <ul>
  <li><a href="http://doc.coin3d.org/SoQt">SoQt</a> is for interfacing with Trolltech's cross-platform Qt toolkit
      (UNIX, Windows, Mac OS X).</li>
  <li><a href="http://doc.coin3d.org/SoWin">SoWin</a> is for interfacing with the Win32 API on Microsoft Windows
      platforms.</li>
  <li><a href="http://doc.coin3d.org/Sc21">Sc21</a> is for interfacing with Cocoa on Mac OS X.</li>
  <li><a href="http://doc.coin3d.org/SoXt">SoXt</a> is for interfacing with Xt/Motif on X Windows.</li>
  </ul>

  See <http://www.coin3d.org/> for more information about Coin and the
  GUI toolkit libraries.

  <b>IMPORTANT NOTE: the online documentation for the Coin library is
  a continuous work-in-progress.</b> Although the large majority of
  classes have been documented properly, there might still be some
  poorly documented items. If you happen upon an undocumented or
  poorly documented class and / or class method which you find hard to
  understand, please give us a notice so we can rectify the situation.
*/

// FIXME: should really list, explain and link to detail-doc on major
// features in the mainpage doc above. It should have screenshots and
// usage examples and all sorts of nice stuff for a good introduction
// to Coin.
//
// Should at least cover these major new features of Coin v2.0:
//
//     - big-image
//     - mp-rendering
//     - VRML97
//     - 3D sound
//
// 20030227 mortene.

/*! \defgroup actions Actions */
/*! \defgroup base Base classes */
/*! \defgroup bundles Bundles (mostly for internal use) */
/*! \defgroup caches Cache storage (mostly for internal use) */
/*! \defgroup collision Collision detections between scene geometry */
/*! \defgroup details Detail classes */
/*! \defgroup draggers Draggers */
/*! \defgroup elements Elements (mostly for internal use)
  See the class documentation of SoElement for a detailed description
  of the purpose and design of the element classes.
*/
/*! \defgroup engines Engine classes */
/*! \defgroup errors Error handling */
/*! \defgroup events Event classes */
/*! \defgroup fields Field classes */
/*! \defgroup general General classes */
/*! \defgroup hardcopy Support for vectorized printing

  See \ref hardcopy_overview for an overview of this functionality.
*/
/*! \defgroup manips Manipulators */
/*! \defgroup nodekits Node Kits */
/*! \defgroup nodes Nodes */
/*! \defgroup projectors Projector classes (mostly for internal use) */
/*! \defgroup sensors Sensor classes */
/*! \defgroup sound 3D sound support */
/*! \defgroup threads Portable thread-handling abstractions  */
/*! \defgroup VRMLnodes VRML97 classes */

/*!
  \class SbBool SbBasic.h Inventor/SbBasic.h
  \brief SbBool is a compiler portable boolean type.
  \ingroup base

  SbBool is meant to be a "compiler portable" way of defining a
  boolean type, since there are older compilers out there which don't
  support the ISO-standard C++ \c bool keyword.

  SbBool is not really a class, just a \c typedef.
*/


/* *********************************************************************** */

// FIXME: complete this to a first usable version -- it should evolve
// into a whitepaper-like document to educate people about exactly
// what Coin is and what features it has. As of now it is just a
// placeholder for misc explanations etc we drop on coin-discuss or
// other places.
//
// Note that the comment starts with "/*" instead of "/*!", so as not
// to activate this doc for Doxygen yet, as this is a work in
// progress.
//
// 20031105 mortene.
/*
  \page coin_technical_whitepaper Coin Technical Whitepaper

  * larsa on Performer vs Inventor:

    [...] both are scene-graph based.

    The main difference is that Open Inventor is data-driven (and
    event-based), while Performer is application-driven.  It basically
    means that Open Inventor will know when the program has changed
    something in the scene, and will redraw it for you, while for
    application-driven libraries it is up to the application to decide
    when to render the scene again, which usually ends up meaning
    continuous redrawing if you're not smart about it.  Also, the
    event-mechanism in Open Inventor makes it really easy to create
    interactive 3D components like draggers and manipulators, one of
    the major features that makes people choose to use Open Inventor.

  * larsa [unpublished] on Application-domain problems:

    [Someone once wrote the following]
    : I have a scene graph and I want to walk inside the scene
    : (like a city and I want to walk along the roads in the city). How can I put
    : the camera in a way so that I can only see the scene in front of me and
    : round me, just like the view you see when you walk around? Is there any
    : sample code so that I can use?

    Just thought I should explain why you haven't gotten any answers on this
    one, since it might clear up other generic issues you or anyone else are 
    wondering about.

    The Open Inventor library doesn't really know a whole lot.  It doesn't know
    what "up" is, it has no concept of what is a road, what is a wall, what is
    on the inside of something and what is on the outside.  It basically has
    coordinates and polygons and ways to render those.

    On the other hand, Open Inventor is helpful when it comes to organizing your
    models hierarchically or logically, making it easy for developers to know
    what is a road and what are walls, if that's what the developer wants to
    keep organized.  But to Open Inventor, it's still just coordinates and
    triangles.  Knowing anything more than that falls into The Application Domain.

    So what you have above is a problem relating to the application domain.
    The solution will be application specific.  For there to have been a
    "generic ready-made solution" for this, you would have to have a standardized
    way of organizing 3D models that the ready-made solution could utilize.
    But Open Inventor developers' needs are so diverse when it comes to what
    their 3D models should represent, so creating the above standard would
    be - if not impossible - certainly very suboptimal.

    So it's like this:
    - As the creator of the application, you are the holder of the knowledge
      on what a road is and what to do with it.
    - As the creator of the application, you have the power to control and
      monitor what the camera is doing, and enforcing your application specific
      rules on the camera movement.

*/

/*
  FIXME: document all variables. pederb, 2004-03-22

  \page environment_variables Environment variables

  Debug related:

  COIN_DEBUG_GLU_INFO 
  COIN_DEBUG_FONTSUPPORT
  COIN_DEBUG_3DS
  COIN_DEBUG_AUDIO
  COIN_DEBUG_BREAK
  COIN_DEBUG_CACHING
  COIN_DEBUG_DL
  COIN_DEBUG_IMPORT
  COIN_DEBUG_LISTMODULES
  COIN_DEBUG_SOINPUT_FINDFILE
  COIN_DEBUG_SOOFFSCREENRENDERER
  COIN_DEBUG_WRITEREFS
  COIN_RANDOMIZE_RENDER_CACHING

  Font/Text rendering related:

  COIN_FREETYPE2_LIBNAME
  COIN_FORCE_FREETYPE_OFF
  COIN_FORCE_WIN32FONTS_OFF
  COIN_FONT_PATH

  COIN_GLU_LIBNAME
  COIN_AGLGLUE_NO_PBUFFERS
  COIN_DEBUG_DL
  COIN_SIMAGE_LIBNAME
  COIN_GLXGLUE_NO_GLX13_PBUFFERS
  COIN_GLXGLUE_NO_PBUFFERS
  COIN_ZLIB_LIBNAME
  COIN_BZIP2_LIBNAME
  COIN_WGLGLUE_NO_PBUFFERS
  COIN_DONT_MANGLE_OUTPUT_NAMES
  COIN_EXTSELECTION_SAVE_OFFSCREENBUFFER
  COIN_FORCE_TILED_OFFSCREENRENDERING
  COIN_GLERROR_DEBUGGING
  COIN_IDA_DEBUG
  COIN_OFFSCREENRENDERER_MAX_TILESIZE
  COIN_OFFSCREENRENDERER_TILEHEIGHT
  COIN_OFFSCREENRENDERER_TILEWIDTH
  COIN_OLDSTYLE_FORMATTING
  COIN_SEPARATE_DIFFUSE_TRANSPARENCY_OVERRIDE
  COIN_SOINPUT_SEARCH_GLOBAL_DICT
  COIN_SOOFFSCREENRENDERER_TILEPREFIX
  COIN_SORTED_LAYERS_USE_NVIDIA_RC

  Sound related:

  COIN_SOUND_BUFFER_LENGTH
  COIN_SOUND_DISABLE
  COIN_SOUND_DRIVER_NAME
  COIN_SOUND_ENABLE
  COIN_SOUND_INTRO_PAUSE
  COIN_SOUND_NUM_BUFFERS
  COIN_SOUND_THREAD_SLEEP_TIME
  COIN_OPENAL_LIBNAME

  Texture control related:

  COIN_TEX2_LINEAR_LIMIT
  COIN_TEX2_LINEAR_MIPMAP_LIMIT
  COIN_TEX2_MIPMAP_LIMIT
  COIN_TEX2_SCALEUP_LIMIT
  COIN_TEX2_USE_GLTEXSUBIMAGE
  COIN_MAXIMUM_TEXTURE2_SIZE
  COIN_MAXIMUM_TEXTURE3_SIZE

  Rendering (OpenGL) related:

  COIN_USE_GL_VERTEX_ARRAYS
  COIN_NORMALIZATION_CUBEMAP_SIZE
  OIV_NUM_SORTED_LAYERS_PASSES
  COIN_MAX_VBO_MEMORY
  COIN_NUM_SORTED_LAYERS_PASSES
  COIN_QUADMESH_PRECISE_LIGHTING
  COIN_ENABLE_CONFORMANT_GL_CLAMP

  IV_SEPARATOR_MAX_CACHES
  COIN_AUTOCACHE_LOCAL_MAX
  COIN_AUTOCACHE_LOCAL_MIN
  COIN_AUTOCACHE_REMOTE_MAX
  COIN_AUTOCACHE_REMOTE_MIN
  COIN_AUTO_CACHING
  COIN_ENABLE_VBO
  
  COIN_SOOFFSCREENRENDERER_ALLOW_RESOURCEHOG

  SO_DRAGGER_DIR
*/


/* *********************************************************************** */

/*!
  \class SoDB SoDB.h Inventor/SoDB.h
  \brief The SoDB class keeps track of internal global data.
  \ingroup general

  This class collects various methods for initializing, setting and
  accessing common global data from the Coin library.

  All methods on SoDB are static.

  Make sure you call SoDB::init() (either directly or indirectly
  through the init() method of the GUI glue library) before you use
  any of the other Coin classes.
*/

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#ifdef HAVE_UNISTD_H
#include <unistd.h> // fd_set (?)
#endif // HAVE_UNISTD_H

#include <Inventor/SoDB.h>

#include <Inventor/C/tidbits.h>
#include <Inventor/C/tidbitsp.h>
#include <Inventor/SoInput.h>
#include <Inventor/actions/SoAction.h>
#include <Inventor/details/SoDetail.h>
#include <Inventor/elements/SoElement.h>
#include <Inventor/engines/SoEngine.h>
#include <Inventor/engines/SoNodeEngine.h>
#include <Inventor/engines/SoEngineOutput.h>
#include <Inventor/errors/SoReadError.h>
#include <Inventor/events/SoEvent.h>
#include <Inventor/fields/SoGlobalField.h>
#include <Inventor/fields/SoSFTime.h>
#include <Inventor/misc/SbHash.h>
#include <Inventor/misc/SoGLBigImage.h>
#include <Inventor/misc/SoGLImage.h>
#include <Inventor/misc/SoProto.h>
#include <Inventor/misc/SoProtoInstance.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include <Inventor/annex/HardCopy/SoHardCopy.h>
#include <Inventor/errors/SoDebugError.h>
#include "SoVBO.h"
#include "../shaders/SoShader.h"
#include "../geo/SoGeo.h"
#include <Inventor/annex/FXViz/nodes/SoShadowGroup.h>

#include <coindefs.h> // COIN_STUB()


#ifdef HAVE_VRML97
#include <Inventor/VRMLnodes/SoVRML.h>
#include <Inventor/VRMLnodes/SoVRMLGroup.h>
#endif // HAVE_VRML97

#ifdef HAVE_THREADS
#include <Inventor/C/threads/threadp.h>
#endif // HAVE_THREADS

#ifdef COIN_THREADSAFE
#include <Inventor/threads/SbRWMutex.h>
#include <Inventor/C/threads/recmutexp.h>
static SbRWMutex * sodb_globalmutex = NULL;
#endif // COIN_THREADSAFE

#ifdef HAVE_3DS_IMPORT_CAPABILITIES
#include "../3ds/3dsLoader.h"
#endif // HAVE_3DS_IMPORT_CAPABILITIES

#include "CoinStaticObjectInDLL.h"
#include "PointerMap.h"
#include "systemsanity.icc"

static SbString * coin_versionstring = NULL;

// atexit callback
static void cleanup_func(void)
{
  delete coin_versionstring;
  coin_versionstring = NULL;
}

// *************************************************************************

class SoDB_HeaderInfo {
public:
  SoDB_HeaderInfo(const SbString & hs, const SbBool bin, const float ver,
                  SoDBHeaderCB * pre, SoDBHeaderCB * post, void * ud)
    : headerstring(hs), isbinary(bin), ivversion(ver),
        preload_cb(pre), postload_cb(post), userdata(ud)
    { }

  SbString headerstring;
  SbBool isbinary;
  float ivversion;
  SoDBHeaderCB * preload_cb, * postload_cb;
  void * userdata;
};

// *************************************************************************

typedef SbHash<int16_t, uint32_t> UInt32ToInt16Map;

// Private data class.
class SoDBP {
public:
  static void variableArgsSanityCheck(void);

  static void clean(void);
  static void removeRealTimeFieldCB(void);
  static void updateRealTimeFieldCB(void * data, SoSensor * sensor);
  static void listWin32ProcessModules(void);

  static SbList<SoDB_HeaderInfo *> * headerlist;
  static SoSensorManager * sensormanager;
  static SoTimerSensor * globaltimersensor;
  static UInt32ToInt16Map * converters;
  static int notificationcounter;
  static SbBool isinitialized;

  static SbBool is3dsFile(SoInput * in);
  static SoSeparator * read3DSFile(SoInput * in);

  struct ProgressCallbackInfo {
    SoDB::ProgressCallbackType * func;
    void * userdata;

    int operator==(const ProgressCallbackInfo & a) {
      return (a.func == this->func) && (a.userdata == this->userdata);
    }
  };
  static SbList<struct ProgressCallbackInfo> * progresscblist;
};

SbList<SoDB_HeaderInfo *> * SoDBP::headerlist = NULL;
SoSensorManager * SoDBP::sensormanager = NULL;
SoTimerSensor * SoDBP::globaltimersensor = NULL;
UInt32ToInt16Map * SoDBP::converters = NULL;
SbBool SoDBP::isinitialized = FALSE;
int SoDBP::notificationcounter = 0;
SbList<SoDBP::ProgressCallbackInfo> * SoDBP::progresscblist = NULL;

// *************************************************************************
// FIXME: this should be moved into a function in tidsbits.c. 20050509 mortene.

// A sanity check. Invoked from SoDB::init() below.
//
// This checks that the undefined behaviour of the system's
// vsnprintf() works the way we need it to when the (C99) va_copy()
// macro is not available on the system. See coin_vsnprintf() in
// tidbits.c for more information.
//
// The relevant part of ISO/IEC 9899:1999 ("C99") says (in section
// 7.19.6.12):
//
//  2] The vsnprintf function is equivalent to snprintf, with the
//  variable argument list replaced by arg, which shall have been
//  initialized by the va_start macro (and possibly subsequent va_arg
//  calls). The vsnprintf function does not invoke the va_end
//  macro. If copying takes place between objects that overlap, the
//  behavior is undefined.

static void
forward_sprintf(char * dst, unsigned int realdstlen, const char * fmtstr, ...)
{
  va_list args;
  va_start(args, fmtstr);

  // This first call is just to get one invocation of va_arg() done
  // from the system's vsnprintf().
  int len = coin_vsnprintf(dst, (unsigned int)(strlen(fmtstr) - 1), fmtstr, args);
  assert(len == -1);

  // The next call is made to see whether or not additional
  // invocations of vsnprintf() on the system without an intervening
  // va_start()/va_end() pair will cause va_arg() to start picking up
  // arguments after the end of the actual argument list.
  len = coin_vsnprintf(dst, realdstlen, fmtstr, args);
  assert(len != -1);

  va_end(args);
}

void
SoDBP::variableArgsSanityCheck(void)
{
  const char * fmtstr = "abc%s";
  char block[128];

  // Notice that there's only a single "%s" in the format string, but
  // we pass in *two* string arguments to test whether or not the last
  // will be erroneously used.
  forward_sprintf(block, sizeof(block) - 1, fmtstr, "-XXX", "-YYY");

  if (strcmp(block, "abc-XXX") == 0) { return; } // all ok

  // Make sure the test itself is correct.
  assert((strcmp(block, "abc-YYY") == 0) && "variable args sanity check bogus");

  // SoDebugError not initialized yet, so use printf().
  (void)printf("Coin " COIN_VERSION ": Sanity Check Report\n\n");
  (void)printf("This system's vsnprintf() has a variable argument\n"
               "invocation which is not working properly with Coin.\n\n"
               "Application will continue to run, but be aware that this\n"
               "problem could cause obscure bugs.\n\n"
               "Please report this problem to <coin-support@coin3d.org>\n"
               "for further assistance.\n\n");
}

// *************************************************************************

// For sanity checking that our static variables in Coin has had a
// chance to init themselves before the invocation of SoDB::init().
//
// At least under Windows, it is possible to force the compiler /
// linker / system loader to init static application objects /
// variables before the ones in DLLs. If this is done (as we have seen
// with at least one external user), various hard-to-debug problems
// may crop up.

// FIXME: this is probably not initialized upon system start, but
// rather placed static in a thunk in the DLL/.so. Needs to fetch a
// value that can not have been compiled in. 20050506 mortene.

static uint32_t a_static_variable = 0xdeadbeef;

// *************************************************************************

/*!
  Initialize the Coin system. This needs to be done as the first
  thing before you start using the library, or you'll probably see an
  early crash.
 */
void
SoDB::init(void)
{
  // Sanity check that our static variables in Coin has had a chance
  // to init themselves before the first invocation of this function
  // happens. See above documentation on the variable for more
  // information.
  assert((a_static_variable == 0xdeadbeef) &&
         "SoDB::init() called before Coin DLL initialization!");

  if (SoDB::isInitialized()) return;

  // Releasing the mutex used for detecting multiple Coin instances in
  // the process image.
  CoinStaticObjectInDLL::init();

  // See systemsanity.icc
  SoDB_checkGCCBuiltinExpectSanity();

  // See SbBasic.h, notes about bug in the Sun CC 4.0 compiler vs
  // probable bug in the Intel compiler.
  assert(SoNode::getClassTypeId() == SoType::badType() && "Data init failed! Get in touch with maintainers at <coin-support@coin3d.org>");

  // Sanity check. Must be done early, as e.g.  SoDebugError::post*()
  // may fail if there are problems.
  SoDBP::variableArgsSanityCheck();


#ifdef HAVE_THREADS
  // initialize thread system first
  cc_thread_init();
#ifdef COIN_THREADSAFE
  sodb_globalmutex = new SbRWMutex(SbRWMutex::READ_PRECEDENCE);
#endif // COIN_THREADSAFE
#endif // HAVE_THREADS

  coin_init_tidbits();

  // Allocate our static members.
  SoDBP::headerlist = new SbList<SoDB_HeaderInfo *>;
  SoDBP::sensormanager = new SoSensorManager;
  SoDBP::converters = new UInt32ToInt16Map;

  // NB! There are dependencies in the order of initialization of
  // components below.

  // This obviously needs to be done first.
  SoType::init();

  // Error classes must be initialized before we do the type size
  // checking below, as we spit out warning messages if
  // inconsistencies are found.
  SoError::initClasses();


  // OBSOLETED asserts for 1.0 release. We should be ok. FIXME: I can
  // only think of possibilities for problems in the binary .iv import
  // and export code. 20010308 mortene.
#if 0
  // Sanity checks: if anything here breaks, either
  // include/Inventor/system/inttypes.h.in or the bitwidth define
  // configure tests need fixing. Keep these tests around.

  // Sanity check: if the int type is not equal to 32 bits everything
  // probably goes to hell. FIXME: remove this check when we are no
  // longer dependent on using native C types where we need to have a
  // particular bitwidth.
  assert(sizeof(int) == 4);

  if (sizeof(int) != 4) {
    SoDebugError::postWarning("SoDB::init",
                              "sizeof(int) != 4 "
                              "(Coin not tested on this platform)");
  }

  // Sanity check: if this breaks, the binary format import and export
  // routines will not work correctly. FIXME: the code should be fixed
  // to use the int16_t type, then we can remove this stoopid check.
  assert(sizeof(short) == 2);

  if (sizeof(short) != 2) {
    SoDebugError::postWarning("SoDB::init",
                              "sizeof(short) != 2 "
                              "(Coin not tested on this platform)");
  }

  // Sanity check: if the int type is unequal to the long type, things
  // could break -- but probably not.
  assert(sizeof(int) == sizeof(long));

  if (sizeof(int) != sizeof(long)) {
    SoDebugError::postWarning("SoDB::init",
                              "sizeof(int) != sizeof(long) "
                              "(Coin not tested on this platform)");
  }
#endif // OBSOLETED sanity checks

  // initialize the pimpl pointer mapper
  PointerMap::initClass();

  SoInput::init();
  SoBase::initClass();
  SoDetail::initClass();
  // SoPath inherits SoBase, so initialize it after SoBase.
  SoPath::initClass();
  SoFieldContainer::initClass();
  SoGlobalField::initClass(); // depends on SoFieldContainer init
  SoField::initClass();
  // Elements must be initialized before actions.
  SoElement::initClass();
  // Actions must be initialized before nodes (because of SO_ENABLE)
  SoAction::initClass();
  SoNode::initClass();
  SoEngine::initClass();
  SoEvent::initClass();
  SoSensor::initClass();

  SoProto::initClass();
  SoProtoInstance::initClass();

  SoGLImage::initClass();
  SoGLBigImage::initClass();

  SoHardCopy::init();

  SoShader::init();
  SoVBO::init();

  SoShadowGroup::init();
  SoGeo::init();

#ifdef HAVE_VRML97
  so_vrml_init();
#endif // HAVE_VRML97
 
  // Register all valid file format headers.
  SoDB::registerHeader(SbString("#Inventor V2.1 ascii   "), FALSE, 2.1f,
                       NULL, NULL, NULL);
  SoDB::registerHeader(SbString("#Inventor V2.1 binary  "), TRUE, 2.1f,
                       NULL, NULL, NULL);

  // FIXME: this is really only valid if the HAVE_VRML97 define is in
  // place. If it's not, we should register the header in a way so
  // that we spit out a /specific/ warning about why VRML97 is not
  // supported in the configuration of the compiled libCoin. 20020808 mortene.
  SoDB::registerHeader(SbString("#VRML V2.0 utf8"), FALSE, 2.1f,
                       NULL, NULL, NULL);

  // FIXME: there are nodes in TGS' later Inventor versions that we do
  // not support, so it's not really correct to register 2.4 and 2.5
  // headers.  20010925 mortene.
  SoDB::registerHeader(SbString("#Inventor V2.4 ascii   "), FALSE, 2.4f,
                       NULL, NULL, NULL);
  SoDB::registerHeader(SbString("#Inventor V2.4 binary  "), TRUE, 2.4f,
                       NULL, NULL, NULL);
  SoDB::registerHeader(SbString("#Inventor V2.5 ascii   "), FALSE, 2.5f,
                       NULL, NULL, NULL);
  SoDB::registerHeader(SbString("#Inventor V2.5 binary  "), TRUE, 2.5f,
                       NULL, NULL, NULL);
  // FIXME: TGS has released many more versions than this. There are
  // at least 2.6, 3.0, 3.1 and 4.0, as of now. What should we do with
  // those? Simply add them in the same manner? Should investigate
  // with someone holding a TGS license how the header look for output
  // written with these versions. 20040909 mortene.

  SoDB::registerHeader(SbString("#Inventor V2.0 ascii   "), FALSE, 2.0f,
                       NULL, NULL, NULL);
  SoDB::registerHeader(SbString("#Inventor V2.0 binary  "), TRUE, 2.0f,
                       NULL, NULL, NULL);

  // FIXME: this is erroneous, we don't _really_ support v1.x Inventor
  // files.  Should spit out a warning, and a helpful message on how
  // it is possible to convert old files, and then exit import.
  // 20010925 mortene.
  //
  // UPDATE 20040909 mortene: pederb has implemented the "upgraders"
  // mechanism, and at least a few upgrader nodes for V1.0
  // stuff. Should investigate the exact status of this.
  SoDB::registerHeader(SbString("#Inventor V1.0 ascii   "), FALSE, 1.0f,
                       NULL, NULL, NULL);
  SoDB::registerHeader(SbString("#Inventor V1.0 binary  "), TRUE, 1.0f,
                       NULL, NULL, NULL);

  // The VRML 1 standard was made from SGI's Inventor V2.1 (and should
  // be a pure sub-set), so that's what we pretend the format version
  // is, internally.
  SoDB::registerHeader(SbString("#VRML V1.0 ascii   "), FALSE, 2.1f,
                       NULL, NULL, NULL);


  // FIXME: should be more robust and accept a set of headers that
  // /almost/ match the exact specifications above. I have for
  // instance seen several VRML models from the web which starts with
  // "#VRML 2.0 utf8", which will cause SoDB::readAll() to fail with
  // "Not a valid Inventor file." We should rather spit out a warning
  // and read it anyway if we detect it's a close match. 20020920 mortene.
  
  SoDB::createGlobalField("realTime", SoSFTime::getClassTypeId());
  SoGlobalField::getGlobalFieldContainer("realTime")->ref();
  coin_atexit((coin_atexit_f *)SoDBP::removeRealTimeFieldCB, CC_ATEXIT_REALTIME_FIELD);

  SoDBP::globaltimersensor = new SoTimerSensor;
  SoDBP::globaltimersensor->setFunction(SoDBP::updateRealTimeFieldCB);
  // An interval of 1/12 is pretty low for today's standards, but this
  // won't result in e.g. jerky animations of SoRotor & Co, since the
  // SoSensorManager automatically and continually re-triggers scene
  // traversals when anything has been connected to the realTime
  // field.
  SoDBP::globaltimersensor->setInterval(SbTime(1.0/12.0));
  // FIXME: it would be better to not schedule unless something
  // actually attaches itself to the realtime field, or does this muck
  // up the code too much? 19990225 mortene.
  // FIXME: the globaltimersensor triggers will drift, and animations
  // will update slower (or less regular - not that we're talking
  // framerate, not animation motion speed) since we don't do a
  // setBaseTime() call on globaltimersensor.  Investigate if this is
  // the correct behaviour (what the SGI "reference" implementation
  // does), correct the behaviour if needed, and mark this spot with
  // info on that the sensor setup is intentional.  Do the same for
  // the Coin 1.* branch.  20021016 larsa
  SoDBP::globaltimersensor->schedule();

  // Force correct time on first getValue() from "realTime" field.
  SoDBP::updateRealTimeFieldCB(NULL, NULL);

  // This should prove helpful for debugging the pervasive problem
  // under Win32 with loading multiple instances of the same library.
  //
  // Note that this is done late in the init()-process, to make sure
  // all Coin-features used in SoDB::listWin32ProcessModules() have
  // been initialized.
  const char * env = coin_getenv("COIN_DEBUG_LISTMODULES");
  if (env && (atoi(env) > 0)) { SoDBP::listWin32ProcessModules(); }

  SoDBP::isinitialized = TRUE;

  // Debugging for memory leaks will be easier if we can clean up the
  // resource usage. This needs to be done last in init(), so we get
  // called _before_ clean() methods in other classes with the same
  // priority.

  // Make sure SoDBP::cleanup() is called _after_ normal cleanup
  // functions.
  coin_atexit((coin_atexit_f *)SoDBP::clean, CC_ATEXIT_SODB);
}

// This will free all resources which has been allocated by the SoDB
// class. This isn't really necessary in user applications (the
// underlying operating system will take care of making the resources
// available again), but it is useful for debugging purposes -- it
// makes it easier to find memory leaks.
void
SoDBP::clean(void)
{
  delete SoDBP::progresscblist;
  SoDBP::progresscblist = NULL;

  // Avoid having the SoSensorManager instance trigging the callback
  // into the So@Gui@ class -- not only have it possible "died", but
  // the whole GUI toolkit could have died until we come here.
  //
  // (This has already proven itself to be a source of problems with
  // the SoQt library, which wets its pants on the
  // SoDBP::globaltimersensor destruction under MSWindows if we don't
  // first nullify the callback function pointer.)
  SoDBP::sensormanager->setChangedCallback(NULL, NULL);

  delete SoDBP::globaltimersensor;
  delete SoDBP::converters;

  delete SoDBP::sensormanager;

  for (int i = 0; i < SoDBP::headerlist->getLength(); i++)
    delete (*SoDBP::headerlist)[i];
  delete SoDBP::headerlist;
  
#ifdef COIN_THREADSAFE
  delete sodb_globalmutex;
#endif // COIN_THREADSAFE
}

void
SoDBP::removeRealTimeFieldCB(void)
{
  SoGlobalField::getGlobalFieldContainer("realTime")->unref();
}

/*!
  Invoke this method as the last call of the application code, to
  trigger a clean-up of all static resources used by the Coin library.

  This is usually not necessary for stand-alone executable
  applications, as the operating system will take care of cleaning up
  after the process as it exits.

  It may be necessary to invoke this method to avoid leaks for
  "special" execution environments, though, like if the Coin library
  is used as e.g. a browser plug-in, or some other type of component
  which can be started, shut down and restarted multiple times.

  \since Coin 2.4
  \since TGS Inventor 5.0
*/
void
SoDB::finish(void)
{
  coin_atexit_cleanup();
  SoDBP::isinitialized = FALSE;
}

/*!
  This method was renamed from Coin version 2.4 onwards, to
  SoDB::finish(). Consider this name for the method obsolete.

  \since Coin 2.0
*/
void
SoDB::cleanup(void)
{
  SoDB::finish();
}

/*!
  Returns a text string containing the name of the library and version
  information.
*/
const char *
SoDB::getVersion(void)
{
  if (coin_versionstring == NULL) {
    coin_versionstring = new SbString("SIM Coin " COIN_VERSION);
    coin_atexit((coin_atexit_f *)cleanup_func, CC_ATEXIT_NORMAL);
  }
  return coin_versionstring->getString();
}

/*!
  Instantiates and reads an SoPath object from \a in and returns a
  pointer to it in \a path.

  The reference count of the SoPath object will initially be zero.

  Returns \c FALSE on error. Returns \c TRUE with \a path equal to \a
  NULL if we hit end of file instead of a new path specification in
  the file.
*/
SbBool
SoDB::read(SoInput * in, SoPath *& path)
{
  path = NULL;
  SoBase * baseptr;
  if (!SoDB::read(in, baseptr))  return FALSE;
  if (!baseptr) return TRUE; // eof

  if (!baseptr->isOfType(SoPath::getClassTypeId())) {
    SoReadError::post(in, "'%s' not derived from SoPath",
                      baseptr->getTypeId().getName().getString());
    baseptr->ref();
    baseptr->unref();
    return FALSE;
  }

  path = (SoPath *)baseptr;
  return TRUE;
}

SoSeparator *
SoDBP::read3DSFile(SoInput * in)
{
  assert(SoDBP::is3dsFile(in));

#ifdef HAVE_3DS_IMPORT_CAPABILITIES

  SoSeparator * b;
  const SbBool ok = coin_3ds_read_file(in, b);
  if (ok) { return b; }

#else // !HAVE_3DS_IMPORT_CAPABILITIES

  SoDebugError::postWarning("SoDB::read",
                            "It seems like the input file is in 3D Studio "
                            "format, but this configuration of Coin was "
                            "built without support for that file format.");

#endif // !HAVE_3DS_IMPORT_CAPABILITIES

  return NULL;
}

/*!
  Instantiates and reads an object of type SoBase from \a in and
  returns a pointer to it in \a base. \a base will be \c NULL on
  return if we hit end of file.

  The reference count of the base object will initially be zero.

  Returns \c FALSE on error.
*/
SbBool
SoDB::read(SoInput * in, SoBase *& base)
{
  if (SoDBP::is3dsFile(in)) {
    base = SoDBP::read3DSFile(in);
    return (base != NULL);
  }

  // Header is only required when reading from a stream, if reading from
  // memory no header is required
  if (!in->isValidFile()) return FALSE;
  return SoBase::read(in, base, SoBase::getClassTypeId());
}

/*!
  Instantiates and reads an object of type SoNode from \a in and returns
  a pointer to it in \a rootnode.

  The reference count of the node will initially be zero.

  Returns \c FALSE on error. Returns \c TRUE with \a rootnode equal to
  \c NULL if we hit end of file instead of a new node specification in
  the file.
 */
SbBool
SoDB::read(SoInput * in, SoNode *& rootnode)
{
  rootnode = NULL;
  SoBase * baseptr;

  if (SoDBP::is3dsFile(in)) {
    rootnode = SoDBP::read3DSFile(in);
    return (rootnode != NULL);
  }
  // allow engines at the top level of a file
  do {
    if (!SoDB::read(in, baseptr)) return FALSE;
    if (!baseptr) return TRUE; // eof
  } while (baseptr->isOfType(SoEngine::getClassTypeId()));

  if (!baseptr->isOfType(SoNode::getClassTypeId())) {
    SoReadError::post(in, "'%s' not derived from SoNode",
                      baseptr->getTypeId().getName().getString());
    baseptr->ref();
    baseptr->unref();
    return FALSE;
  }

  rootnode = (SoNode *)baseptr;
  return TRUE;
}

/*!
  Reads all graphs from \a in and returns them under an SoSeparator
  node. If the file contains only a single graph under an SoSeparator
  node (which is the most common way of constructing and exporting
  scene graphs), no \e extra SoSeparator root node will be made, but
  the returned root node will be the top-most node from the file.

  The reference count of the root separator returned from this method
  will be zero. Other nodes in the returned scene graph will have
  reference counts according to the number of parent-child
  relationships, as usual.

  The common layout for how to load, work with and then finally
  destruct and return memory resources of scenegraphs usually goes
  like this:

  \code
  // [snip]
  SoInput in;
  if (!in.openFile(filename)) { exit(1); }

  SoSeparator * root = SoDB::readAll(&in);
  if (!root) { exit(1); }

  // root-node returned from SoDB::readAll() has initial zero
  // ref-count, so reference it before we start using it to
  // avoid premature destruction.
  root->ref();

  // [do your thing here -- attach the scene to a viewer or whatever]

  // Bring ref-count of root-node back to zero to cause the
  // destruction of the scene.
  root->unref();
  // (Upon self-destructing, the root-node will also de-reference
  // it's children nodes, so they also self-destruct, and so on
  // recursively down the scenegraph hierarchy until the complete
  // scenegraph has self-destructed and thereby returned all
  // memory resources it was using.)
  \endcode

  Returns \c NULL on any error.

  Tip: a common operation to do after importing a scene graph is to
  pick out the memory pointers to one or more of the imported nodes
  for further handling. This can be accomplished by using either the
  SoNode::getByName() function (which is the easier approach) or by
  using an instance of the SoSearchAction class (which is the more
  complex but also more flexible approach).
*/
SoSeparator *
SoDB::readAll(SoInput * in)
{
  return (SoSeparator*) 
    SoDB::readAllWrapper(in, SoSeparator::getClassTypeId());
}

/*!
  Same as SoDB::readAll(), except it return an SoVRMLGroup instead
  of an SoSeparator.

  \sa SoDB::readAll()
  \since Coin 2.0
*/
SoVRMLGroup * 
SoDB::readAllVRML(SoInput * in)
{
#ifdef HAVE_VRML97
  return (SoVRMLGroup*) 
    SoDB::readAllWrapper(in, SoVRMLGroup::getClassTypeId());
#else // HAVE_VRML97
  return NULL;
#endif // ! HAVE_VRML97
}

/*!
  Check if \a testString is a valid file format header identifier string.

  \sa getHeaderData(), registerHeader()
 */
SbBool
SoDB::isValidHeader(const char * teststring)
{
  SbBool isbinary;
  float ivversion;
  SoDBHeaderCB * preload_cb, * postload_cb;
  void * userdata;

#if COIN_DEBUG
  if (!teststring) {
    SoDebugError::postWarning("SoDB::isValidHeader",
                              "Passed a NULL string pointer.");
    return FALSE;
  }
#endif // COIN_DEBUG

  return SoDB::getHeaderData(SbString(teststring), isbinary, ivversion,
                             preload_cb, postload_cb, userdata, TRUE);
}

/*!
  Register a header string which should be recognized by SoInput upon
  file import. This is a convenient way for library users to register
  their own VRML or Coin derived file formats.

  Set \a isbinary to \c TRUE if the file should be read as binary
  data, and set \a ivversion to indicate which Coin library version is
  needed to read the file.

  Callbacks \a precallback and \a postcallback will be called before
  and after importing the custom format.

  If \a headerstring can not be accepted as a valid file format header
  for Coin files, \c FALSE will be returned. A valid header \e must
  start with a '#' character, and not be more than 80 characters long.

  \sa getHeaderData()
*/
SbBool
SoDB::registerHeader(const SbString & headerstring,
                     SbBool isbinary, float ivversion,
                     SoDBHeaderCB * precallback, SoDBHeaderCB * postcallback,
                     void * userdata)
{
  // Must start with '#'.
  if ((headerstring.getLength() == 0) ||
      (headerstring.getString()[0] != '#')) {
#if COIN_DEBUG
    SoDebugError::postWarning("SoDB::registerHeader",
                              "File header string must begin with '#', '%s'"
                              " invalid.", headerstring.getString());
#endif // COIN_DEBUG
    return FALSE;
  }

  // No more than 80 characters.
  if (headerstring.getLength() > 80) {
#if COIN_DEBUG
    SoDebugError::postWarning("SoDB::registerHeader",
                              "File header string must be 80 characters "
                              "or less, so '%s' is invalid.",
                              headerstring.getString());
#endif // COIN_DEBUG
    return FALSE;
  }

  SoDB_HeaderInfo * newheader =
    new SoDB_HeaderInfo(headerstring, isbinary, ivversion,
                        precallback, postcallback, userdata);
  SoDBP::headerlist->append(newheader);
  return TRUE;
}

/*!
  Returns the settings for the given \a headerstring, if \a headerstring
  is a valid header.

  If \a substringok is \c TRUE, ignore trailing characters in \a headerstring
  when checking for validity.

  If no valid header string by this name is found, \c FALSE is returned,
  otherwise \c TRUE will be returned, and the other input arguments will
  be set to their respective values.

  \sa isValidHeader(), registerHeader()
 */
SbBool
SoDB::getHeaderData(const SbString & headerstring, SbBool & isbinary,
                    float & ivversion, SoDBHeaderCB *& precallback,
                    SoDBHeaderCB *& postcallback, void *& userdata,
                    SbBool substringok)
{
  unsigned int hslen = headerstring.getLength();
  if (hslen == 0) return FALSE;

  // Disregard whitespace.
  while ((headerstring[hslen-1] == ' ') || (headerstring[hslen-1] == '\t')) {
    hslen--;
    if (hslen == 0) return FALSE;
  }

  SbString tryheader = headerstring.getSubString(0, hslen-1);

  SbBool hit = FALSE;
  for (int i=0; (i < SoDB::getNumHeaders()) && !hit; i++) {
    SoDB_HeaderInfo * hi = (*SoDBP::headerlist)[i];
    SbString & s = hi->headerstring;
    unsigned int reglen = s.getLength();
    assert(reglen > 0);
    // Disregard whitespace.
    while ((s[reglen-1] == ' ') || (s[reglen-1] == '\t')) reglen--;
    assert(reglen > 0);

    SbString regheader = s.getSubString(0, reglen-1);

    if (regheader == tryheader)
      hit = TRUE;
    else if (substringok && (hslen > reglen) &&
             (regheader == tryheader.getSubString(0, reglen - 1)))
      hit = TRUE;

    if (hit) {
      isbinary = hi->isbinary;
      ivversion = hi->ivversion;
      precallback = hi->preload_cb;
      postcallback = hi->postload_cb;
      userdata = hi->userdata;
    }
  }

  return hit;
}

/*!
  Returns number of registered file headers.

  \sa getHeaderString()
 */
int
SoDB::getNumHeaders(void)
{
  return SoDBP::headerlist->getLength();
}

/*!
  Returns the identifier header string of index \a i.

  \sa getNumHeaders(), getHeaderData()
 */
SbString
SoDB::getHeaderString(const int i)
{
#if COIN_DEBUG
  if ((i < 0) || (i >= SoDBP::headerlist->getLength())) {
    SoDebugError::post("SoDB::getHeaderString", "Index %d out of range.", i);
    return SbString("");
  }
#endif // COIN_DEBUG

  return (*SoDBP::headerlist)[i]->headerstring;
}

/*!
  Create a new global field by the given \a type, and identified in
  subsequent accesses to getGlobalField() by \a name. If a global
  field by the name and type already exists, returns a pointer to it.
  If a global field with the same name but a different type exists,
  returns \c NULL.

  A global field can be deallocated by calling
  SoDB::renameGlobalField(), with the second argument set to an empty
  string.

  \sa getGlobalField(), renameGlobalField()
 */
SoField *
SoDB::createGlobalField(const SbName & name, SoType type)
{
  assert(name != "" && "invalid name for a global field");

  SoField * f = SoDB::getGlobalField(name);
  if (f) {
    if (f->getTypeId() == type) return f;
    else return NULL;
  }

#if COIN_DEBUG
  if (!type.canCreateInstance()) {
    SoDebugError::postWarning("SoDB::createGlobalField",
                              "Can't create instance of field type ``%s''.",
                              type.getName().getString());
    return NULL;
  }
#endif // COIN_DEBUG

  // (Deallocation of the field happens in the SoGlobalField
  // destructor.)
  SoField * newfield = (SoField *)type.createInstance();
  (void) new SoGlobalField(name, newfield);
  return newfield;
}

/*!
  If there exist a global field with the given \a name, return a
  pointer to it. If there is no field with this name, return \c NULL.

  Of particular interest is the \c realTime global field set up by the
  library on initialization. This field is used as a source field to
  all the autonomous animation objects within the library, like for
  instance the SoTimeCounter engine or the SoRotor node.

  If you want to control the speed of "action" of a scene with
  animating / moving components (for instance for doing fixed
  frame-time snapshots for generating a movie), grab the global field
  named "realTime" and use it in the manner depicted in the class
  documentation of the SoOffscreenRenderer class.

  \sa createGlobalField(), renameGlobalField()
*/
SoField *
SoDB::getGlobalField(const SbName & name)
{
  SoGlobalField * gf = SoGlobalField::getGlobalFieldContainer(name);
  return gf ? gf->getGlobalField() : NULL;
}

/*!
  Rename a global field. If \a to is an empty name, the \a from field
  gets deleted. If another global field already goes by the name \a
  to, that field will get deleted before the rename operation.

  \sa getGlobalField(), createGlobalField()
*/
void
SoDB::renameGlobalField(const SbName & from, const SbName & to)
{
  SoGlobalField * gf = SoGlobalField::getGlobalFieldContainer(from);

#if COIN_DEBUG
  if (gf == NULL) {
    SoDebugError::postWarning("SoDB::renameGlobalField",
                              "Couldn't find global field '%s' to rename.",
                              from.getString());
    return;
  }
#endif // COIN_DEBUG

  if (to == "") { // Empty string is a special case, remove field.
    assert(gf->getRefCount() == 1);
    SoGlobalField::removeGlobalFieldContainer(gf);
    return;
  }

  // Existing entry by the same "to"-name? If so, remove it.
  SoGlobalField * old = SoGlobalField::getGlobalFieldContainer(to);
  if (old) {
    assert(old->getRefCount() == 1);
    SoGlobalField::removeGlobalFieldContainer(old);
  }

  gf->setName(to);
}

// This is the timer sensor callback which updates the realTime global
// field.
void
SoDBP::updateRealTimeFieldCB(void * /* data */, SoSensor * /* sensor */)
{
  SoField * f = SoDB::getGlobalField("realTime");
  if (f && (f->getTypeId() == SoSFTime::getClassTypeId())) {
    ((SoSFTime *)f)->setValue(SbTime::getTimeOfDay());
  }
}

/*!
  Set the time interval between updates for the \c realTime global field
  to \a interval. Default value is 1/12 s.

  The low update rate is due to historical reasons, to be compatible
  with application code written for SGI Inventor.

  Setting the interval to a zero time will disable automatic updates
  of the realTime field.

  \sa getRealTimeInterval(), getGlobalField()
 */
void
SoDB::setRealTimeInterval(const SbTime & interval)
{
#if COIN_DEBUG
  if (interval < SbTime::zero()) {
    SoDebugError::postWarning("SoDB::setRealTimeInterval",
                              "Tried to set negative interval.");
    return;
  }
#endif // COIN_DEBUG

  SbBool isscheduled = SoDBP::globaltimersensor->isScheduled();
  if (isscheduled) SoDBP::globaltimersensor->unschedule();
  SoDBP::globaltimersensor->setInterval(interval);
  if (isscheduled && interval != SbTime::zero()) 
    SoDBP::globaltimersensor->schedule();
}

/*!
  Returns the current trigger interval for the global \a realTime SbTime
  field.

  \sa setRealTimeInterval(), getGlobalField()
 */
const SbTime &
SoDB::getRealTimeInterval(void)
{
  return SoDBP::globaltimersensor->getInterval();
}

/*!
  This is just a wrapper for the method in SoSensorManager by the
  same name.

  \sa getDelaySensorTimeout(), SoSensorManager
 */
void
SoDB::setDelaySensorTimeout(const SbTime & t)
{
  SoDB::getSensorManager()->setDelaySensorTimeout(t);
}

/*!
  This is just a wrapper for the method in SoSensorManager by the
  same name.

  \sa setDelaySensorTimeout(), SoSensorManager
 */
const SbTime &
SoDB::getDelaySensorTimeout(void)
{
  return SoDB::getSensorManager()->getDelaySensorTimeout();
}

/*!
  Returns a pointer to the global sensor manager. The sensor manager keeps
  track of the sensor queues.
 */
SoSensorManager *
SoDB::getSensorManager(void)
{
  return SoDBP::sensormanager;
}

/*!
  NOTE: THIS METHOD IS OBSOLETED. DON'T USE IT.

  This is a wrapper around the POSIX \c select() call. It is provided
  so you can do synchronous I/O while Coin continues to handle sensor
  events, rendering, etc. The parameters are the same as for \c
  select(), so check your system documentation on how to use them.

  The void* arguments must be valid pointers to fd_set
  structures. We've changed this from the original SGI Inventor API to
  avoid messing up the header file with system-specific includes.

  NOTE: THIS METHOD IS OBSOLETED. DON'T USE IT.
*/
int
SoDB::doSelect(int nfds, void * readfds, void * writefds,
               void * exceptfds, struct timeval * usertimeout)
{
  assert(FALSE && "obsoleted method");
  return 0;
}

/*!
  Notify SoDB that there exists a way to convert data from the \a from
  SoField type to the \a to SoField type, by connecting them with an
  instance of the \a converter SoFieldConverter type.

  By doing this, SoDB::getConverter() will later be able to
  automatically return the type of the correct conversion class when
  requested.

  Coin internally provides conversion between most field types, so
  application programmers should usually not need to use this
  function. The exception is if you are writing your own field type
  classes, and want to be able to connect them to the internal field
  types (or other extensions field types).

  \sa createConverter(), SoFieldConverter
 */
void
SoDB::addConverter(SoType from, SoType to, SoType converter)
{
  const uint32_t linkid = (((uint32_t)from.getKey()) << 16) + to.getKey();
  SbBool nonexist = SoDBP::converters->put(linkid, converter.getKey());
  if (!nonexist) {
#if COIN_DEBUG
    SoDebugError::postWarning("SoDB::addConverter",
                              "Conversion from ``%s'' to ``%s'' is already "
                              "handled by instances of ``%s''",
                              from.getName().getString(),
                              to.getName().getString(),
                              converter.getName().getString());
#endif // COIN_DEBUG
  }
}

/*!
  Return the type of an SoFieldConverter class which is able to
  convert data between fields of type \a from to the data field(s) of
  field type \a to.

  If no conversion between the given field types is possible, returns
  SoType::badType().

  \sa addConverter()
 */
SoType
SoDB::getConverter(SoType from, SoType to)
{
  uint32_t val = (((uint32_t)from.getKey()) << 16) + to.getKey();
  int16_t key;
  if (!SoDBP::converters->get(val, key)) { return SoType::badType(); }
  return SoType::fromKey(key);
}

/*!
  Returns \c TRUE if init() has been called.

  \sa init()
 */
SbBool
SoDB::isInitialized(void)
{
  return SoDBP::isinitialized;
}

/*!
  \COININTERNAL
 */
void
SoDB::startNotify(void)
{
#ifdef COIN_THREADSAFE
  (void) cc_recmutex_internal_notify_lock();
#endif // COIN_THREADSAFE
  SoDBP::notificationcounter++;
}

/*!
  \COININTERNAL
 */
SbBool
SoDB::isNotifying(void)
{
  return SoDBP::notificationcounter > 0;
}

/*!
  \COININTERNAL
 */
void
SoDB::endNotify(void)
{
  SoDBP::notificationcounter--;
  if (SoDBP::notificationcounter == 0) {
    // Process zero-priority sensors after notification has been done.
    SoSensorManager * sm = SoDB::getSensorManager();
    if (sm->isDelaySensorPending()) sm->processImmediateQueue();
  }
#ifdef COIN_THREADSAFE
  (void) cc_recmutex_internal_notify_unlock();
#endif // COIN_THREADSAFE

}

/*!
  Turn on or off the realtime sensor.

  The most common use for turning the realtime sensor off is to
  control the realTime global field from the user application. This is
  for instance handy when you want to take screen snapshots at fixed
  intervals. See the class documentation of SoOffscreenRenderer for
  further information.

  \sa setRealTimeInterval(), getGlobalField()
*/
void
SoDB::enableRealTimeSensor(SbBool on)
{
  assert(SoDB::isInitialized());

  SbBool isscheduled = SoDBP::globaltimersensor->isScheduled();
  if (isscheduled && !on) SoDBP::globaltimersensor->unschedule();
  else if (!isscheduled && on && 
           SoDBP::globaltimersensor->getInterval() != SbTime::zero()) 
    SoDBP::globaltimersensor->schedule();
#if COIN_DEBUG
  else SoDebugError::postWarning("SoDB::enableRealTimeSensor",
                                 "realtime sensor already %s",
                                 on ? "on" : "off");
#endif // COIN_DEBUG
}

SbBool
SoDBP::is3dsFile(SoInput * in)
{
  if (in->getHeader().getLength() > 0) { return FALSE; }

  char c1, c2;
  if (!in->get(c1)) { return FALSE; }
  if (!in->get(c2)) { in->putBack(c1); return FALSE; }
  in->putBack(c2);
  in->putBack(c1);

  // FIXME: this seems like a rather weak test for 3D Studio format
  // files (1 out of every 65536 files with random data would return
  // TRUE). Should check up on the format spec, and improve
  // it. 20031117 mortene.
  if (c1 != 0x4d) { return FALSE; }
  if (c2 != 0x4d) { return FALSE; }

  return TRUE;
}

// private wrapper for readAll() and readAllVRML()
SoGroup *
SoDB::readAllWrapper(SoInput * in, const SoType & grouptype)
{
  assert(SoDB::isInitialized() && "you forgot to initialize the Coin library");

  assert(grouptype.canCreateInstance());
  assert(grouptype.isDerivedFrom(SoGroup::getClassTypeId()));

  if (SoDBP::is3dsFile(in)) {
    SoSeparator * root3ds = SoDBP::read3DSFile(in);
    if (root3ds == NULL) { return NULL; }

    if (!SoSeparator::getClassTypeId().isDerivedFrom(grouptype)) {
      SoGroup * root = (SoGroup *)grouptype.createInstance();
      root->addChild(root3ds);
      return root;
    }
    else {
      return root3ds;
    }
  }

  if (!in->isValidFile()) {
    SoReadError::post(in, "Not a valid Inventor file.");
    return NULL;
  }

  const int stackdepth = in->filestack.getLength();

  SoGroup * root = (SoGroup *)grouptype.createInstance();
  SoNode * topnode;
  do {
    if (!SoDB::read(in, topnode)) {
      root->ref();
      root->unref();
      return NULL;
    }
    if (topnode) { root->addChild(topnode); }
  } while (topnode && in->skipWhiteSpace());

  if (!in->eof()) {
    // All  characters  may not  have  been  read  from the  current
    // stream.  The reading stops when no more valid identifiers are
    // found, so we have to read until the current file on the stack
    // is at the end.  All non-whitespace characters from now on are
    // erroneous.
    static uint32_t readallerrors_termination = 0;
    char dummy = -1; // Set to -1 to make sure the variable has been
                     // read before an error is output
    char buf[2];
    buf[1] = '\0';  
    while (!in->eof() && in->read(dummy)) {
      if (readallerrors_termination < 1) {
        buf[0] = dummy;
        SoReadError::post(in, "Erroneous character(s) after end of scenegraph: \"%s\". "
                          "This message will only be shown once for this file, "
                          "but more errors might be present", dummy != '\0' ? buf : "\\0");
      }
      
      readallerrors_termination++;
    }
    assert(in->eof());

    if (dummy == '\0' && !in->isBinary()) {
      SoReadError::post(in, "It appears that your iv-file ends with a null-character ('\\0') "
                        "This could happen if you use the SoInput::setBuffer method "
                        "with a character-string argument and the size of the string "
                        "was one character too long.  A typical reason for the problem "
                        "is if you use sizeof to measure the string length; not taking into "
                        "account that strings end with '\\0', which should not be "
                        "input to the setBuffer-method. To correct this, use strlen "
                        "instead of sizeof.");
    }
  }

  // Make sure the current file (which is EOF) is popped off the stack.
  in->popFile(); // No popping happens if there is just one file on
                 // the stack

  // Detect problems with missing pops from the SoInput file stack.
  assert((stackdepth == 1 && in->filestack.getLength() == 1) ||
         (stackdepth - 1 == in->filestack.getLength())); 

  // Strip off extra root group node if it was unnecessary (i.e. if
  // the file only had a single top-level root, and it was of the same
  // type as is requested returned).
  if ((root->getNumChildren() == 1) && (root->getChild(0)->isOfType(grouptype))) {
    SoNode * n = root->getChild(0);
    n->ref();
    root->ref();
    root->unref();
    n->unrefNoDelete();
    return (SoGroup *)n;
  }

  return root;
}

/* *********************************************************************** */

/*!
  \typedef SbBool SoDB::ProgressCallbackType(const SbName & itemid, float fraction, SbBool interruptible, void * userdata)

  Client code progress callback function must be static functions of
  this type.

  The \a itemid argument is a unique text identifier which says what
  is being processed (use this for any GUI progress bar informational
  text), and \a fraction is a value in the range [0, 1] which
  indicates how far the process has got. If the task is successfully
  aborted, the callback will be invoked a last time with \a fraction
  set to -1.0.

  The return value is an abort flag indication from the client
  code. Note that the process that is being run can only be aborted if
  the \a interruptible flag is set.
  
  See SoDB::addProgressCallback() for full documentation of how the
  progress notification mechanism works.
*/

/*!
  The concept behind progress information in Coin is that any internal
  process which may take a long time to complete (like e.g. file
  import for huge scenes) can pass on progress information by calling
  back to a progress callback set up by the code of the client
  application.

  The client's progress callback's function signature must match the
  SoDB::ProgressCallbackType.

  The mechanism works by enforcing that all progress notification from
  within Coin must

  <ol>

  <li> Use a unique text id to identify the "progress-informing"
       process (e.g. "File import" for SoDB::readAll() / SoInput file
       reading, "File export" for SoOutput / SoWriteAction, etc). This
       is the \a itemid name passed on to the progress callback.</li>

  <li>The first invocation of the user callback will be done with an
      exact 0.0 \a fraction value.</li>

  <li>The last invocation will be done with an exact 1.0 fraction
      value.</li>

  <li>An exception to the last point is that if the process is
      aborted, a final invocation with a -1.0 fraction value will be
      made.</li>

  </ol>

  One important thing to note about the mechanism is that processes
  with progress callbacks can be running within \e other processes
  using the progress callback functionality.  Progress information
  will then have to be considered to be "stacked", and client code
  must be aware of and treat this properly.

  \COIN_FUNCTION_EXTENSION

  \since Coin 2.2
*/
void
SoDB::addProgressCallback(ProgressCallbackType * func, void * userdata)
{
  if (SoDBP::progresscblist == NULL) {
    SoDBP::progresscblist = new SbList<SoDBP::ProgressCallbackInfo>;
  }

  const SoDBP::ProgressCallbackInfo newitem = { func, userdata };
  SoDBP::progresscblist->append(newitem);
}

/*!
  Removes a progress callback function, which will no longer be
  invoked.
*/
void
SoDB::removeProgressCallback(ProgressCallbackType * func, void * userdata)
{
  assert(SoDBP::progresscblist);

  const SoDBP::ProgressCallbackInfo item = { func, userdata };
  const int idx = SoDBP::progresscblist->find(item);
  assert(idx != -1);
  SoDBP::progresscblist->remove(idx);
}

/*!  
  Returns \c TRUE if this is a thread safe version of Coin
  (i.e. it was configured and built with --enable-threadsafe).
*/
SbBool 
SoDB::isMultiThread(void)
{
#ifdef COIN_THREADSAFE
  return TRUE;
#else // COIN_THREADSAFE
  return FALSE;
#endif // !COIN_THREADSAFE
}

// Note that the function names of the next four functions below are
// all lowercase to be compatible with client code written on TGS
// Inventor.

/*!

  Places a read lock on the global SoDB mutex. This can be used to
  synchronize between threads that are reading/writing Coin scene
  graphs.

  If you call this function, you must make sure that you also call
  SoDB::readunlock(). If you fail to do this, you might experience
  that your application locks up.

  All Coin actions has a read-lock on the global SoDB mutex while
  traversing the scene graph.

  \sa SoDB::readunlock(), SoDB::writelock()

  \since Coin 2.3
  \since TGS Inventor 3.0
*/
void
SoDB::readlock(void) 
{ 
#ifdef COIN_THREADSAFE
  sodb_globalmutex->readLock(); 
#endif // COIN_THREADSAFE 
}

/*!
  Unlocks the read lock on the global SoDB mutex.

  \sa SoDB::readlock()
  \since Coin 2.3
  \since TGS Inventor 3.0
*/ 
void 
SoDB::readunlock(void)
{
#ifdef COIN_THREADSAFE
  sodb_globalmutex->readUnlock();
#endif // COIN_THREADSAFE
}

/*!  
  Places a write lock on the global SoDB mutex. This can be used to
  prevent that the scene graph is read or traversed while you modify
  the scene graph.

  If you call this function, you must make sure that you also call
  SoDB::writeunlock(). If you fail to do this, you might experience
  that your application locks up.

  \sa SoDB::readlock()
  \since Coin 2.3  
  \since TGS Inventor 3.0
*/
void 
SoDB::writelock(void)
{
#ifdef COIN_THREADSAFE
  sodb_globalmutex->writeLock();
#endif // COIN_THREADSAFE
}

/*!
  Unlocks the write lock on the global SoDB mutex.

  \sa SoDB::writelock()
  \since Coin 2.3  
  \since TGS Inventor 3.0
*/
void 
SoDB::writeunlock(void)
{
#ifdef COIN_THREADSAFE
  sodb_globalmutex->writeUnlock();
#endif // COIN_THREADSAFE
}

// *************************************************************************

//
// helper function for createRoute(). First test the actual fieldname,
// then set set_<fieldname>, then <fieldname>_changed.
//
static SoField *
find_route_field(SoNode * node, const SbName & fieldname)
{
  SoField * field = node->getField(fieldname);

  if (!field) {
    if (strncmp(fieldname.getString(), "set_", 4) == 0) {
      SbName newname = fieldname.getString() + 4;
      field = node->getField(newname);
    }
    else {
      SbString str = fieldname.getString();
      int len = str.getLength();
      const char CHANGED[] = "_changed";
      const int changedsize = sizeof(CHANGED) - 1;

      if (len > changedsize && strcmp(str.getString()+len-changedsize,
                                      CHANGED) == 0) {
        SbString substr = str.getSubString(0, len-(changedsize+1));
        SbName newname = substr.getString();
        field = node->getField(newname);
      }
    }
  }
  return field;
}

/*!
  Create a connection from one VRML97 node field to another.

  ("Routes" are what field-to-field connections are called for the
  VRML97 standard.)

  Connections made in this manner will be persistent upon file export.

  \sa SoDB::removeRoute()
  \sa SoField::connectFrom(SoField*)

  \since Coin 2.4
  \since TGS Inventor 2.6
*/
void
SoDB::createRoute(SoNode * fromnode, const char * eventout,
                  SoNode * tonode, const char * eventin)
{
  assert(fromnode && tonode && eventout && eventin);

  SbName fromfieldname(eventout);
  SbName tofieldname(eventin);

  SoField * from = find_route_field(fromnode, fromfieldname);
  SoField * to = find_route_field(tonode, tofieldname);

  SbName fromnodename = fromnode->getName();
  if (fromnodename == "") {
    fromnodename = "<noname>";
  }
  SbName tonodename = tonode->getName();
  if (tonodename == "") {
    tonodename = "<noname>";
  }
  SoEngineOutput * output = NULL;
  if (from == NULL && fromnode->isOfType(SoNodeEngine::getClassTypeId())) {
    output = ((SoNodeEngine*) fromnode)->getOutput(fromfieldname);
  }

  if (to && (from || output)) {
    SbBool notnotify = FALSE;
    SbBool append = FALSE;
    if (output || from->getFieldType() == SoField::EVENTOUT_FIELD) {
      notnotify = TRUE;
    }
#if 0 // seems like (reading the VRML97 spec.) fanIn in allowed even to regular fields
    if (to->getFieldType() == SoField::EVENTIN_FIELD) append = TRUE;
#else // fanIn
    append = TRUE;
#endif // fanIn fix
    
    // Check if we're already connected.
    SoFieldList fl;
    if (from) from->getForwardConnections(fl);
    else output->getForwardConnections(fl);
    int idx = fl.find(to);
    if (idx != -1) {
#if COIN_DEBUG
      SoDebugError::postWarning("SoDB::createRoute",
                                "Tried to connect a ROUTE multiple times "
                                "(from %s.%s to %s.%s)",
                                fromnodename.getString(), fromfieldname.getString(),
                                tonodename.getString(), tofieldname.getString());
#endif // COIN_DEBUG
      return;
    }

    // Check that there exists a field converter, if one is needed.
    SoType totype = to->getTypeId();
    SoType fromtype = from ? from->getTypeId() : output->getConnectionType();
    if (totype != fromtype) {
      SoType convtype = SoDB::getConverter(fromtype, totype);
      if (convtype == SoType::badType()) {
#if COIN_DEBUG
        SoDebugError::postWarning("SoDB::createRoute",
                                  "Tried to connect a ROUTE between entities "
                                  "that can not be connected (due to lack of "
                                  "field type converter): %s.%s is of type "
                                  "%s, and %s.%s is of type %s",
                                  fromnodename.getString(), fromfieldname.getString(),
                                  fromtype.getName().getString(),
                                  tonodename.getString(), tofieldname.getString(),
                                  totype.getName().getString());
#endif // COIN_DEBUG
        return;
      }
    }
    
    SbBool ok;
    if (from) ok = to->connectFrom(from, notnotify, append);
    else ok = to->connectFrom(output, notnotify, append);
    // Both known possible failure points are caught above.
    assert(ok && "unexpected connection error");
  }
#if COIN_DEBUG
  else {
    SoDebugError::postWarning("SoDB::createRoute",
                              "Unable to create route: %s.%s TO %s.%s",
                              fromnodename.getString(), eventout,
                              tonodename.getString(), eventin);
  }
#endif  // COIN_DEBUG
}

/*!
  Removes a field-to-field connection.

  \sa SoDB::createRoute()
  \sa SoField::disconnect(SoField*)

  \since Coin 2.4
  \since TGS Inventor 2.6
*/
void
SoDB::removeRoute(SoNode * fromnode, const char * eventout,
                  SoNode * tonode, const char * eventin)
{
  assert(fromnode && tonode && eventout && eventin);

  SbName fromfieldname(eventout);
  SbName tofieldname(eventin);
  
  SoField * from = find_route_field(fromnode, fromfieldname);
  SoField * to = find_route_field(tonode, tofieldname);
  
  SoEngineOutput * output = NULL;
  if (from == NULL && fromnode->isOfType(SoNodeEngine::getClassTypeId())) {
    output = ((SoNodeEngine*) fromnode)->getOutput(fromfieldname);
  }

  SbName fromnodename = fromnode->getName();
  if (fromnodename == "") {
    fromnodename = "<noname>";
  }
  SbName tonodename = tonode->getName();
  if (tonodename == "") {
    tonodename = "<noname>";
  }

  if (to && (from || output)) {
    if (from) to->disconnect(from);
    else to->disconnect(output);
  }
#if COIN_DEBUG
  else { // some error occured
    SoDebugError::postWarning("SoDB::removeRoute",
                              "Unable to remove route: %s.%s TO %s.%s",
                              fromnodename.getString(), eventout,
                              tonodename.getString(), eventin);
  }
#endif // COIN_DEBUG
}

// *************************************************************************

#if defined(HAVE_WINDLL_RUNTIME_BINDING) && defined(HAVE_TLHELP32_H)

#include <tlhelp32.h>


typedef HANDLE (WINAPI * CreateToolhelp32Snapshot_t)(DWORD, DWORD);
typedef BOOL (WINAPI * Module32First_t)(HANDLE, LPMODULEENTRY32);
typedef BOOL (WINAPI * Module32Next_t)(HANDLE, LPMODULEENTRY32);

static CreateToolhelp32Snapshot_t funCreateToolhelp32Snapshot;
static Module32First_t funModule32First;
static Module32Next_t funModule32Next;


void
SoDBP::listWin32ProcessModules(void)
{
  BOOL ok;

  HINSTANCE kernel32dll = LoadLibrary("kernel32.dll");
  assert(kernel32dll && "LoadLibrary(''kernel32.dll'') failed");

  funCreateToolhelp32Snapshot = (CreateToolhelp32Snapshot_t)
    GetProcAddress(kernel32dll, "CreateToolhelp32Snapshot");
  funModule32First = (Module32First_t)
    GetProcAddress(kernel32dll, "Module32First");
  funModule32Next = (Module32Next_t)
    GetProcAddress(kernel32dll, "Module32Next");

  if (!funCreateToolhelp32Snapshot || !funModule32First || !funModule32Next) {
    SoDebugError::postWarning("SoDBP::listWin32ProcessModules",
                              "Tool Help Library not available (NT4?)");
  }
  else {
    HANDLE tool32snap = funCreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    assert((tool32snap != (void *)-1) && "CreateToolhelp32Snapshot() failed");

    MODULEENTRY32 moduleentry;
    moduleentry.dwSize = sizeof(MODULEENTRY32);
    ok = funModule32First(tool32snap, &moduleentry);
    assert(ok && "Module32First() failed");

    SoDebugError::postInfo("SoDBP::listWin32ProcessModules",
                           "MODULEENTRY32.szModule=='%s', .szExePath=='%s'",
                           moduleentry.szModule, moduleentry.szExePath);

    while (funModule32Next(tool32snap, &moduleentry)) {
      SoDebugError::postInfo("SoDBP::listWin32ProcessModules",
                             "MODULEENTRY32.szModule=='%s', .szExePath=='%s'",
                             moduleentry.szModule, moduleentry.szExePath);
    }

    assert(GetLastError()==ERROR_NO_MORE_FILES && "Module32Next() failed");

    ok = CloseHandle(tool32snap);
    assert(ok && "CloseHandle() failed");
  }

  ok = FreeLibrary(kernel32dll);
  assert(ok && "FreeLibrary() failed");
}


#else // !HAVE_WINDLL_RUNTIME_BINDING || !HAVE_TLHELP32_H

void
SoDBP::listWin32ProcessModules(void)
{
  SoDebugError::postWarning("SoDBP::listWin32ProcessModules",
                            "Tool Help Library not available "
                            "(non-win32 platform?)");
}

#endif // !HAVE_WINDLL_RUNTIME_BINDING || !HAVE_TLHELP32_H

/* *********************************************************************** */
