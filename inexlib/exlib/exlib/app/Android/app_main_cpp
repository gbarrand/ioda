// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file EXLIB_APP.license for terms.

#ifdef APP_USE_PYTHON
#undef Py_BUILD_CORE
#endif

#include "../EXLIB_APP/main"

#include <exlib/app/Android/main_cpp>

void android_main(struct android_app* a_state) {
  exlib_main<EXLIB_APP::context,EXLIB_APP::main>("EXLIB_APP","fr.in2p3.lal",a_state);
#ifdef EXLIB_ANDROID_MAIN_DO_STD_EXIT
  // for g4exa, g4view, ioda. Mainly all apps that need to unfold some zipped resource file.
  // If doing a first exit by using the Android back button, then
  // a second app startup crashes when creating G4RunManager. We suspect that
  // we reuse the same process context (a getpid() gives the same id) and something
  // in G4 (some static ?) are not cleaned/reset by the first exit.
  ::exit(0);
#endif
}

//exlib_build_use inlib expat
//exlib_build_use exlib png jpeg zlib inlib_glutess freetype
//exlib_build_use GL Android


