// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file exlib.license for terms.

#include "../swig/inlib_i"

#define LUA_COMPAT_ALL

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include <cstddef> //ptrdiff_t

#include "inlib_swig_lua.icc"

//exlib_build_use inlib lua csz kernel
