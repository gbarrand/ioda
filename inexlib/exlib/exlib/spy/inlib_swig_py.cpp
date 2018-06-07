// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file exlib.license for terms.

//
//WARNING : do not put the below in the SWIG.i file
//  in some %{... %}. Else it is going to be after Python.h
//  and then compilation problem may happen on some platform.
//

#include "../swig/inlib_i"

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

#include <cstddef> //ptrdiff_t

#include "inlib_swig_py.icc"

//exlib_build_use inlib Python csz kernel

