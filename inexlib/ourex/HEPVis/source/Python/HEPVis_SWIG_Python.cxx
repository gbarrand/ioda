//
//WARNING : do not put the below in the SWIG.i file
//  in some %{... %}. Else it is going to be after Python.h
//  and then compilation problem may happen on some platform.
//  (For example clash with Coin-2.x code on a slc3_gcc323).
//

#include <CoinPython/Inventor.h>

#include <HEPVis/SbAll.h>
#include <HEPVis/Utils.h>
#include <HEPVis/Detector.h>
#include <HEPVis/Geometry.h>
#include <HEPVis/Plotter.h>
#include <HEPVis/GUI.h>

#include <HEPVis/misc/SoTools.h>
#include <HEPVis/misc/SoStyleCache.h>

inline SbBool SbConvertToUnsignedLong(const SbString& aString,unsigned long& aValue){
  aValue = 0L;
  if(::sscanf(aString.getString(),"%lx",&aValue)!=1) {
    if(::sscanf(aString.getString(),"%lu",&aValue)!=1) {
      aValue = 0L;
      return FALSE;
    }
  }
  return TRUE;
}

extern "C" {
  void HEPVisUtils_initClasses();
  void HEPVisGeometry_initClasses();
  void HEPVisDetector_initClasses();
  void HEPVisPlotter_initClasses();
  void HEPVisGUI_initClasses();

  void SoPlotter_initClasses();
}

#include <HEPVis/inlib_plot>

#if defined(__linux)
// Clash between os_defines.h (coming from <string>) and pyconfig.h
#if defined(_POSIX_C_SOURCE)
#undef _POSIX_C_SOURCE
#endif
#if defined(_XOPEN_SOURCE)
#undef _XOPEN_SOURCE
#endif
#endif

#include <Python.h>

#ifdef _WIN32
#include <windows.h>
#undef max
#undef ERROR
#undef DELETE
#endif

//#undef ANY

//If wrapping full Inventor too.
typedef SoGLLazyElement::GLState GLState;

#include "HEPVis_SWIG_Python.ic"
