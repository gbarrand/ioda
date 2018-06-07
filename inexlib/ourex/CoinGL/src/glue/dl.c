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

/* Here's a posting I made on Usenet 2003-02-19 with queries about
   increasing the robustness of the symbol binding process on MSWin
   DLLs. I'm keeping it here for reference, just so we remember that
   there are potential problems:
  
   ------8<------[snip]------8<------[snip]------8<------[snip]----
      Newsgroups: comp.os.ms-windows.programmer.win32
      Subject: Possible to do calling convention query?
      Gcc: nnfolder+archive:out-news
      From: Morten Eriksen <mortene@sim.no>
      Organization: The Underpant Gnomes
      --text follows this line--
      Hi,
      
      I have a delicate problem with robustness which I would like to try to
      solve. The outline of my problem is as follows:
      
        * I'm using LoadLibrary() to bind to the symbols of a DLL at
          run-time.
      
        * This DLL was generated outside of my control.
      
        * I bind to function pointer symbols of the DLL with something like
          the following snippet of code:
      
            typedef void (__stdcall * nurbsProperty_t)(void *, int, float);
            nurbsProperty_t nurbsProperty =
                 (nurbsProperty_t)GetProcAddress(<dllhnd>, "nurbsProperty");
      
            // Now the function can be used like any other, e.g. like this:
            //
            // nurbsProperty(NULL, 0, 0.0f);
      
        * Now, notice the "__stdcall" part of the function signature
          typedef. As you all probably know, this specifies the _assumed_
          calling convention of the function, i.e. how arguments are pushed
          on the stack, and if it's the caller's or the callee's
          responsibility to clean up the stack afterwards.
      
      Now here's the problem I would like to solve: if the "nurbsProperty()"
      function of the DLL was built with a different calling convention than
      __stdcall (say, __cdecl), my subsequent invocation of the method will
      lead to either a corrupted set of input arguments, a corrupted stack
      upon return, or both. Therefore, I would like to *detect* whether the
      DLL function symbols actually matches the calling convention I expect.
      
      Can this be done through the Win32 API somehow? I.e. getting to know
      what calling convention an exported function in a DLL was built with?
      
      (I guess it can be done by loading the DLL file into memory "raw", and
      parsing its structures of the DLL / PE file, but that is _really_ the
      last resort.)
      
      Regards,
      Morten
      -- 
      Ees a sad an' beautiful world
   ------8<------[snip]------8<------[snip]------8<------[snip]----
  
   20030219 mortene.
  
   UPDATE 20030221 mortene: from the replies, it looks like this is
   impossible to do in a "clean" way (i.e. through the Win32
   API). Only suggestion that could be implemented so far is to check
   the value of the stack pointer before and after calling a function
   from the DLL.
  
   Additional suggestion from Lars: we could let the user control
   calling convention from the client side by setting an environment
   variable. That would be really helpful for remote debugging of
   problems suspected to be caused by mismatches.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <assert.h>
#include <stddef.h> /* NULL definition. */
#include <stdlib.h> /* atoi() */
#include <errno.h>
#include <string.h> /* strlen(), strcpy(), strerror() */
#include <stdio.h>  /* snprintf() */
#ifdef HAVE_LIBGEN_H
#include <libgen.h> /* dirname() */
#endif /* HAVE_LIBGEN_H */
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h> /* PATH_MAX */
#endif /* HAVE_SYS_PARAM_H */
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>  /* stat() */
#endif /* HAVE_SYS_STAT_H */

#ifdef HAVE_MACH_O_DYLD_H
#include <mach-o/dyld.h>
#include <mach-o/ldsyms.h>
#endif /* HAVE_MACH_O_DYLD_H */

#ifdef HAVE_DLFCN_H
#include <dlfcn.h> /* Programming interface to libdl. */
#endif /* HAVE_DLFCN_H */

#ifdef HAVE_DLD_LIB
#include <dl.h> /* Programming interface to libdld on HP-UX 10 & 11. */
#endif /* HAVE_DLD_LIB */

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif /* HAVE_WINDOWS_H */


#ifdef HAVE_WIN32_API
/* Conditional inclusion, as the functions in win32api.h will not be
   implemented unless the Win32 API is available. */
#include <Inventor/C/glue/win32api.h>
#endif /* HAVE_WIN32_API */

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/C/glue/dl.h>
#include <Inventor/C/glue/dlp.h>
#include <Inventor/C/tidbits.h>
#include <Inventor/C/tidbitsp.h>
#include <Inventor/system/gl.h> /* for glGetString */

/* ********************************************************************** */

static cc_libhandle glsymbols_handle = NULL;
static SbBool attempted_glsymbols = FALSE;

static const char * NULL_STR = "(null)";

struct cc_libhandle_struct {
  const void * nativehnd;
  cc_string libname;
};

/* ********************************************************************** */

/* Return value of COIN_DEBUG_DL environment variable. */
static int
cc_dl_debugging(void)
{
  static int d = -1;
  if (d == -1) {
    const char * val = coin_getenv("COIN_DEBUG_DL");
    d = val ? atoi(val) : 0;
  }
  return (d > 0) ? 1 : 0;
}

/* ********************************************************************** */

#if defined (HAVE_DYLD_RUNTIME_BINDING)

static char *
cc_dirname(const char *path) {

#ifdef HAVE_LIBGEN_H

  /* Mac OS 10.2 and later have dirname() */
  return dirname(path);

#else

  /* FIXME: dirname() is not confirming to neither ISO/ANSI C nor
     POSIX, which means we could run into this problem on other
     platforms than just Mac OS 10.1. We should have it implemented in
     a manner compatible with DOS filenames aswell as UNIX-style
     paths, and move the implementation to either src/tidbits.c or
     (better) into a cc_file ADT. 20030804 mortene. */

  static char dirpath [MAXPATHLEN];
  const char * ptr;

  if (path == NULL || *path == '\0') return NULL;

  /* Get rid of trailing '/'s */ 
  ptr = path + strlen(path) - 1;
  while (*ptr == '/' && path <= ptr) ptr--;

  /* Skip last element in path */
  while (*ptr != '/' && path <= ptr) ptr--;

  /* Path is only '/' */
  if (ptr == path && *ptr == '/') {
    strcpy(dirpath, "/");
    return(dirpath);
  }

  /* No slashes in path... */
  if (ptr == path) {
    strcpy(dirpath, ".");
    return(dirpath);
  }

  if ((unsigned int)(ptr - path + 1) > sizeof(dirpath)) {
    return NULL;
  }

  strncpy(dirpath, path, ptr - path + 1);
  dirpath[ptr - path + 1] = '\0';
  return(dirpath);

#endif /* libgen.h */
}


/* Returns a string containing the search directories for
   dynamic libraries, separated by ':'. Needed since Mac OS X
   requires to have a full path to the library when loading it. */

static cc_string * 
cc_build_search_list(const char * libname)
{
  int i, image_count = _dyld_image_count();
  cc_string res_path, framework_path, dyld_path;
  cc_string * path = cc_string_construct_new();

  /* We search for libraries in 3 locations: 

    (1) We check if the library exists as a framework in 
        /Library/Frameworks/$libname.framework/$libname.     
        (This is actually quite an ugly hack, since frameworks 
        are not meant to be dlopen'ed -- but we need this for 
        dynamic loading of OpenAL symbols.)

    (2) the default search paths for libraries

    (3) If we cannot find the library on the system, we might have a
        fallback shipped with Coin / the Inventor.framework: Get the
        file system path to the actually loaded Inventor.framework,
        and look for the library in its Resources folder.
  */ 

  /* (1) check if library exists as framework */

  cc_string_construct(&framework_path);
  const char * framework_prefix = "/Library/Frameworks/";
  const char * framework_ext = ".framework";
  cc_string_sprintf(&framework_path, "%s%s%s%s", framework_prefix,
                    libname, framework_ext, ":");
  cc_string_append_string(path, &framework_path);
  cc_string_clean(&framework_path); 

  /* (2) default library search path  */

  cc_string_construct(&dyld_path);
  char * dyld_library_path = getenv("DYLD_LIBRARY_PATH"); 
  if (dyld_library_path) {
    cc_string_append_text(&dyld_path, dyld_library_path);
    if (dyld_library_path[strlen(dyld_library_path)-1] != ':') {
      cc_string_append_text(&dyld_path, ":");
    }
  } 
  char * dyld_fallback_library_path = getenv("DYLD_FALLBACK_LIBRARY_PATH");
  if (dyld_fallback_library_path) {
    cc_string_append_text(&dyld_path, dyld_fallback_library_path);
    if (dyld_fallback_library_path[strlen(dyld_fallback_library_path)-1] != ':') {
      cc_string_append_text(&dyld_path, ":");
    }
  } else {
    cc_string_append_text(&dyld_path, "/lib:/usr/lib:");
  }
  cc_string_append_string(path, &dyld_path);
  cc_string_clean(&dyld_path); 

  /* (3) the Resources folder of the Inventor framework */

  cc_string_construct(&res_path);
  for (i = 0; i < image_count; i++) {
    const char * p = _dyld_get_image_name(i);
    if (strstr(p, "Inventor.framework")) {
      /* We get /path/to/Foo.framework/Versions/A/Foo
         but want /path/to/Foo.framework/Versions/A/Resources */
      char * path_to_version_dir = cc_dirname(p);
      cc_string_sprintf(&res_path, "%s%s", 
                        path_to_version_dir, "/Resources");
      break;
    }
  }
  cc_string_append_string(path, &res_path);
  cc_string_clean(&res_path); 
  return path;
}

/* Returns the absolute path to file if file can be found in the
   library and framework search path, NULL otherwise. It is the
   caller's responsibility to free the returned string. */

static cc_string *
cc_find_file(const char * file)
{
  int end_reached = 0;
  cc_string * path = cc_string_construct_new();
  cc_string * list = cc_build_search_list(file);
  const char * listptr = cc_string_get_text(list);

  while (!end_reached) {
    char * currententry = strsep((char **) &listptr, ":");
    end_reached = (listptr == NULL);
    if (currententry) {
      struct stat sbuf;
      cc_string_sprintf(path, "%s/%s", currententry, file);
      if (stat(cc_string_get_text(path), &sbuf) == 0) {
        break;
      } else {
        cc_string_clear(path);
      }
    }
  }
  cc_string_destruct(list);
  return path;
}

#endif /* HAVE_DYLD_RUNTIME_BINDING */

/* Returns TRUE if run-time linking to dynamic libraries can be
   done. So far, these ways of doing run-time linking are supported:

   \li libdl.so: for Linux, SGI IRIX, Solaris, and other *nix & *BSD systems
   \li LoadLibrary() et al: from the Win32 API
   \li libdld.so: for HP-UX (which also has libdl.so, BTW)
   \li dyld: for Mac OS X (with NSLookupAndBindSymbol() et al)
*/
SbBool
cc_dl_available(void)
{
#ifdef HAVE_DYNAMIC_LINKING
  return TRUE;
#else /* don't know how to do dynamic linking on this system */
  return FALSE;
#endif
}

cc_libhandle
cc_dl_open(const char * filename)
{
  cc_libhandle h = (cc_libhandle) malloc(sizeof(struct cc_libhandle_struct));
  /* if (!h), FIXME: exception handling. 20020906 mortene. */
  h->nativehnd = NULL;

#ifdef HAVE_DL_LIB

  h->nativehnd = dlopen(filename, RTLD_LAZY);

#ifdef HAVE_DYLD_RUNTIME_BINDING
  /* Mac OS X: Search for library shipped with Inventor framework. */
  if (h->nativehnd == NULL) {
    cc_string * path = cc_find_file(filename);
    if (cc_string_length(path) > 0) {
      if (cc_dl_debugging()) {
        cc_debugerror_postinfo("cc_dlopen", "opening: %s", 
                               cc_string_get_text(path));
      }
      h->nativehnd = dlopen(cc_string_get_text(path), 
                            RTLD_LAZY);      
    }
    cc_string_destruct(path);
  }
#endif /* HAVE_DYLD_RUNTIME_BINDING */

  /*
    If dlopen() fails for any reason than not being able to find the
    dynamic link-library given by "filename" on disk, we should really
    detect it and report an error, whether we're running in debug mode
    or release mode.

    The libdl interface doesn't provide any means to do that, though,
    so we'll just /assume/ that a NULL return means the library
    couldn't be found.

    But if a special debugging environment variable is found, we'll
    spit out the error message, which could prove useful for remote
    debugging:
  */

  if (cc_dl_debugging() && (h->nativehnd == NULL)) {
    const char * e = dlerror();
    if (e) {
      cc_debugerror_post("cc_dl_open", "dlopen(\"%s\") failed with: '%s'", 
                         filename, e);
    }
  }

#elif defined (HAVE_DYLD_RUNTIME_BINDING) 

  if (filename == NULL) {

    /* 
       Simulate the behaviour of dlopen(NULL) by returning a handle to
       the first image loaded by the dynamic linker, which is the
       current process. See dyld(3).

       Note that this handle is not necessary for the dyld cc_dl_sym()
       implementation, but it makes it possible to use cc_dl_open() in
       the "classic" dlopen() style (where a NULL return value would
       indicate failure).
    */ 
    h->nativehnd = _dyld_get_image_header(0);

  } else {

    /* 
       Note that we must use NSAddImage, since we want to load a
       shared library, instead of NSCreateObjectFileImageFromFile()
       and NSLinkModule(), which work only with loadable
       modules/bundles. See NSModule(3), NSObjectFileImage(3) and
       http://fink.sourceforge.net/doc/porting/shared.php for details.
    */
    cc_string * path = cc_find_file(filename);
    if (cc_string_length(path) > 0) {
      if (cc_dl_debugging()) {
        cc_debugerror_postinfo("cc_dlopen", "opening: %s", 
                               cc_string_get_text(path));
      }

      h->nativehnd = (void *) NSAddImage(cc_string_get_text(path), 
                                         NSADDIMAGE_OPTION_RETURN_ON_ERROR);

      if (cc_dl_debugging() && !h->nativehnd) {
        NSLinkEditErrors c;
        int e;
        const char * file;
        const char * errstr;
        NSLinkEditError(&c, &e, &file, &errstr);
        cc_debugerror_post("cc_dlopen", "%s", errstr);
      }
      cc_string_destruct(path);
    } 
  }

#elif defined (HAVE_WINDLL_RUNTIME_BINDING)

  /* We don't want to call LoadLibrary(NULL) because this causes a
     crash on some Windows platforms (Crashes on Windows2000 has been
     reported). 20021101 thammer.
  */
  if (filename != NULL) {

    /* Don't use GetModuleHandle(): LoadLibrary() will *not* load a
       new image if the module is already loaded, it will only inc the
       reference count.

       Also, GetModuleHandle() doesn't inc the reference count, so it
       is dangerous in the sense that the module could be free'd from
       somewhere else between us opening it, and until it is used for
       resolving symbols.
    */
    h->nativehnd = LoadLibrary(filename);

    if (cc_dl_debugging() && (h->nativehnd == NULL)) {
      cc_string funcstr;
      cc_string_construct(&funcstr);
      cc_string_sprintf(&funcstr, "LoadLibrary(\"%s\")", filename ? filename : "(null)");
      cc_win32_print_error("cc_dl_open", cc_string_get_text(&funcstr), GetLastError());
      cc_string_clean(&funcstr);
    }
  }
  else {
    h->nativehnd = GetModuleHandle(NULL);
    if (cc_dl_debugging() && (h->nativehnd == NULL)) {
      cc_win32_print_error("cc_dl_open", "GetModuleHandle(NULL)", GetLastError());
    }
  }

#elif defined (HAVE_DLD_LIB)

  /* FIXME: there is a good reason to try to use shn_load() *first*,
     then dlopen() on HP-UX: according to a discussion on the libtool
     mailinglist, dlopen() for HP-UX was buggy in an official release,
     needing a patch to function properly. This would take some
     changes to the configure checks (we cut off further checking if
     libdl is found), and any code that depends on _either_
     HAVE_DL_LIB _or_ HAVE_DLD_LIB being defined, but not both at the
     same time.  20010626 mortene. */

  /* This define not available on older versions. */
#ifndef DYNAMIC_PATH
#define DYNAMIC_PATH 0
#endif /* DYNAMIC_PATH */

  /* Handle attempt to look at running executable image and already
     loaded dynamic libraries. */

  if (filename == NULL) {
    shl_t exehnd = (shl_t)0;
    void * dummy;
    int ret = shl_findsym(&exehnd, "main", TYPE_UNDEFINED, &dummy);
    if (ret != -1) {
      h->nativehnd = exehnd;
    }
    else {
      const char * e = strerror(errno);
      cc_debugerror_post("cc_dl_open",
                         "shl_findsym(&NULL, \"main\", ...) failed with: '%s'",
                         e);
    }
  }
  else {
    h->nativehnd = shl_load(filename, BIND_IMMEDIATE|BIND_NONFATAL|DYNAMIC_PATH, 0L);

    /*
      If a special debugging environment variable is found, we'll spit
      out the error message, which could prove useful for remote
      debugging.

      Note that if shl_load() fails for any reason than not being able
      to find the dynamic link-library given by "filename" on disk, we
      detect it and report an error, whether we're running in debug
      mode or release mode. ENOENT means "the specified library does
      not exist" -- all other errors should be warned about no matter
      what.
    */

    if ((h->nativehnd == NULL) && (cc_dl_debugging() || (errno != ENOENT))) {
      const char * e = strerror(errno);
      cc_debugerror_post("cc_dl_open", "shl_load(\"%s\") failed with: '%s'",
                         filename ? filename : "(null)", e);
    }
  }

#endif

  if (h->nativehnd == NULL) {
    free(h);
    h = NULL;
  }
  else {
    cc_string_construct(&h->libname);
    cc_string_set_text(&h->libname, filename ? filename : NULL_STR);

    if (cc_dl_debugging()) {
#ifdef HAVE_WINDLL_RUNTIME_BINDING
      char libpath[512];
      DWORD retval = GetModuleFileName((HINSTANCE) h->nativehnd, libpath, sizeof(libpath));
      assert(retval > 0 && "GetModuleFileName() failed");
      libpath[sizeof(libpath) - 1] = 0;
      cc_debugerror_postinfo("cc_dl_open", "Opened library '%s'", libpath);
#elif defined (HAVE_DL_LIB) || defined (HAVE_DLD_LIB)
      cc_debugerror_postinfo("cc_dl_open", "Opening library '%s'", cc_string_get_text(&h->libname));
#endif
    }
  }

  if (cc_dl_debugging() && h) {
    cc_debugerror_postinfo("cc_dl_open",
                           "\"%s\" success => cc_libhandle==%p, nativehnd==%p", 
                           cc_string_get_text(&h->libname), h, h->nativehnd);
  }

  return h;
}

void *
cc_dl_sym(cc_libhandle handle, const char * symbolname)
{
  void * ptr = NULL;  

#ifdef HAVE_DL_LIB

  if ((handle == NULL) || (handle->nativehnd == NULL)) return NULL;
  ptr = dlsym((void *)handle->nativehnd, symbolname);

  if (cc_dl_debugging()) {
    const char * e = dlerror();
    if (e) {
      cc_debugerror_post("cc_dl_sym", "dlsym(\"%s\", \"%s\") failed with: '%s'",
                         cc_string_get_text(&handle->libname), symbolname, e);
    }
  }

#elif defined (HAVE_DYLD_RUNTIME_BINDING) 

  /* Note: The dlopen() version returns NULL here if handle or
     handle->nativehnd are NULL, but we do not need a handle for
     symbol lookup on Mac OS X - if we have one, it makes the lookup
     faster, but that's all, so we can get away with having no valid
     handle. */

  NSSymbol symbol = NULL;
  char * mangledname;
  NSLinkEditErrors c;
  int e;
  const char * file;
  const char * errstr;

  if (cc_dl_debugging()) {
    cc_debugerror_postinfo("cc_dl_sym", "Looking up symbol %s", 
                       symbolname);
  }

  mangledname = malloc(strlen(symbolname) + 2);
  strcpy(mangledname + 1, symbolname);
  mangledname[0] = '_';

  if (handle && handle->nativehnd) {
    if (NSIsSymbolNameDefinedInImage(handle->nativehnd, mangledname))
      symbol = NSLookupSymbolInImage(handle->nativehnd, mangledname,
               NSLOOKUPSYMBOLINIMAGE_OPTION_BIND |
               NSLOOKUPSYMBOLINIMAGE_OPTION_RETURN_ON_ERROR);
  } 

  /* If we did not specifically load the library ourselves
     (handle->nativehnd being NULL), or if the symbol could not be
     found in the library, let's try if we can find it in any of the
     loaded libs. */

  if (!symbol && NSIsSymbolNameDefined(mangledname)) {
    symbol = NSLookupAndBindSymbol(mangledname);
  } 

  if (cc_dl_debugging()) {
    if (symbol == NULL) {
      NSLinkEditError(&c, &e, &file, &errstr);
      cc_debugerror_post("cc_dl_sym", "symbol %s not found: %s", 
                         symbolname, errstr);
    }
  }

  free (mangledname);
  ptr = symbol ? NSAddressOfSymbol(symbol) : NULL;  
  
#elif defined (HAVE_WINDLL_RUNTIME_BINDING)

  if ((handle == NULL) || (handle->nativehnd == NULL)) return NULL;
  ptr = GetProcAddress((HINSTANCE) handle->nativehnd, symbolname);

  if (cc_dl_debugging() && (ptr == NULL)) {
    cc_string funcstr;
    cc_string_construct(&funcstr);
    cc_string_sprintf(&funcstr, "GetProcAddress(\"%s\", \"%s\")", cc_string_get_text(&handle->libname), symbolname);
    cc_win32_print_error("cc_dl_sym", cc_string_get_text(&funcstr), GetLastError());
    cc_string_clean(&funcstr);
  }

#elif defined (HAVE_DLD_LIB)

  {
    int retval = shl_findsym((shl_t *)(&handle->nativehnd), symbolname, TYPE_UNDEFINED, &ptr);

    if (cc_dl_debugging() && (retval == -1)) {
      const char * e = strerror(errno);
      cc_debugerror_post("cc_dl_sym", "shl_findsym(\"%s\", \"%s\", ...) failed with: '%s'",
                         cc_string_get_text(&handle->libname), symbolname, e);
    }
  }

#endif

  return ptr;
}

void
cc_dl_close(cc_libhandle handle)
{
  if (cc_dl_debugging()) {
    cc_debugerror_postinfo("cc_dl_close",
                           "closing '%s', cc_libhandle==%p, nativehnd==%p",
                           cc_string_get_text(&handle->libname),
                           handle, handle->nativehnd);
  }

#ifdef HAVE_DL_LIB

  {
    int result = 0;
    int is_proc_img;

    /* Don't dlclose() on handle to the process image, as HP-UX 11's
       dlclose() will complain with an "invalid handle" error then. */

    is_proc_img =
      cc_string_compare_text(cc_string_get_text(&handle->libname),
                             NULL_STR) == 0;
    if (!is_proc_img) {
      result = dlclose((void *)handle->nativehnd);
    }
    
    if (result != 0) {
      const char * e = dlerror();
      if (e) {
        cc_debugerror_post("cc_dl_close", "dlclose(\"%s\") failed with: '%s'",
                           cc_string_get_text(&handle->libname), e);
      }
    }
  }

#elif defined (HAVE_DYLD_RUNTIME_BINDING) 

  /* Do nothing. */

  /* Unlike on ELF systems, you cannot unload unload Mach-O shared
     libraries. See man 3 NSModule, man 3 NSObjectFileImage and
     http://fink.sourceforge.net/doc/porting/shared.php for details. */

#elif defined (HAVE_WINDLL_RUNTIME_BINDING)
  { 
    BOOL result = FreeLibrary((HINSTANCE) handle->nativehnd);
    
    if (!result) {
      cc_string funcstr;
      cc_string_construct(&funcstr);
      cc_string_sprintf(&funcstr, "FreeLibrary(\"%s\")", cc_string_get_text(&handle->libname));
      cc_win32_print_error("cc_dl_close", cc_string_get_text(&funcstr), GetLastError());
      cc_string_clean(&funcstr);
    }
  }
#elif defined (HAVE_DLD_LIB)

  /* FIXME: lib unloading disabled, as on HP-UX PA32, no reference
     counter is held. And we don't want to unload a library that is in
     use. On HP-UX PA64, we should unload, as reference counting is
     done there. 20030305 mortene. */
#if 0
  int result = shl_unload((shl_t)handle->nativehnd);

  if (result == -1) {
    const char * e = strerror(errno);
    cc_debugerror_post("cc_dl_close", "shl_unload(\"%s\") failed with: '%s'",
                       cc_string_get_text(&handle->libname), e);
  }
#endif

#endif

  if (&handle->libname) cc_string_clean(&handle->libname);
  free(handle);
}

/* ********************************************************************** */

/*
  Returns a handle to the current process image, if one could be made.
*/
cc_libhandle
cc_dl_process_handle(void)
{
  return cc_dl_open(NULL);
}

/*
  Returns a handle to the Coin DLL in the current process image, if
  one could be made.
*/
cc_libhandle
cc_dl_coin_handle(void)
{
#ifndef COIN_SYSTEM_LIBRARY_NAME /* should usually be available in config.h */
#define COIN_SYSTEM_LIBRARY_NAME "libCoin.so"
#endif
#ifndef DYNAMIC_LIBRARY_EXTENSION /* should usually be available in config.h */
#define DYNAMIC_LIBRARY_EXTENSION ".so"
#endif

  cc_libhandle hnd = cc_dl_open(COIN_SYSTEM_LIBRARY_NAME);

  if (hnd) {
    /* For comparing with the known value, to make sure we e.g. don't
       get a different Coin DLL loaded from disk: */
    void * func = cc_dl_sym(hnd, "cc_dl_open");
    /* (instead of "cc_dl_open", we could use any other function in
       the public API) */

    if (func == NULL) {
      /* in case we're using the --enable-linkhack dev hack */
      cc_libhandle gluehnd = cc_dl_open("libglueLINKHACK" DYNAMIC_LIBRARY_EXTENSION);
      if (gluehnd) {
        func = cc_dl_sym(gluehnd, "cc_dl_open");
        cc_dl_close(gluehnd);
      }
    }

    if (func) {
      if (func == cc_dl_open) { return hnd; }

      if (cc_dl_debugging()) {
        cc_debugerror_post("cc_dl_coin_handle",
                           "function ptr from opened Coin image, %p, "
                           "does not match expected value from current "
                           "image; %p", func, cc_dl_open);
      }
    }
    else if (cc_dl_debugging()) {
      cc_debugerror_post("cc_dl_coin_handle",
                         "could not find function symbol of self");
    }
  }
  else if (cc_dl_debugging()) {
    cc_debugerror_post("cc_dl_coin_handle",
                       "was not able to open Coin image as '%s'",
                       COIN_SYSTEM_LIBRARY_NAME);
  }

  /* In case of errors when checking if we got a valid image, make
     sure to clean up to avoid resource leak. */
  if (hnd) { cc_dl_close(hnd); }
  return NULL;
}

/*
  Returns a handle to the OpenGL DLL in the current process image, if
  one could be made.
*/
cc_libhandle
cc_dl_opengl_handle(void)
{
#ifndef OPENGL_SYSTEM_LIBRARY_NAME /* should usually be available in config.h */
#define OPENGL_SYSTEM_LIBRARY_NAME "libGL.so"
#endif

  cc_libhandle hnd = cc_dl_open(OPENGL_SYSTEM_LIBRARY_NAME);
  if (hnd) {
    /* for comparing with the known value, to make sure we e.g. don't
       get a different OpenGL DLL loaded from disk: */
    void * func = cc_dl_sym(hnd, "glGetString");

    if (func) {
      if (func == glGetString) { return hnd; }

      if (cc_dl_debugging()) {
        cc_debugerror_post("cc_dl_opengl_handle",
                           "function ptr from opened OpenGL image, %p, "
                           "does not match expected value from current "
                           "image; %p", func, glGetString);
      }
    }
    else if (cc_dl_debugging()) {
      cc_debugerror_post("cc_dl_opengl_handle",
                         "could not find function symbol for 'glGetString'");
    }
  }
  else if (cc_dl_debugging()) {
    cc_debugerror_post("cc_dl_opengl_handle",
                       "was not able to open OpenGL image as '%s'",
                       OPENGL_SYSTEM_LIBRARY_NAME);
  }

  /* In case of errors when checking if we got a valid image, make
     sure to clean up to avoid resource leak. */
  if (hnd) { cc_dl_close(hnd); }
  return NULL;
}

static void
glsymbols_handle_cleanup(void)
{
  if (glsymbols_handle) {
    /* FIXME: the cc_dl_close() below doesn't work on my Linux system,
       at least. Causes a segfault on exit.

       Closing a handle on our own process is a no-no..? Or is there
       perhaps something else hiding behind this error? Should
       investigate.

       The call-stack back-trace when it segfaults:

       Program received signal SIGSEGV, Segmentation fault.
       [Switching to Thread 46912629269472 (LWP 11931)]
       0x00002aaaaaabb610 in calloc () from /lib64/ld-linux-x86-64.so.2
       (gdb) bt
       #0  0x00002aaaaaabb610 in calloc () from /lib64/ld-linux-x86-64.so.2
       #1  0x00002aaaaaab5f38 in _dl_rtld_di_serinfo () from /lib64/ld-linux-x86-64.so.2
       #2  0x00002aaaaf898e4c in _dl_close () from /lib/libc.so.6
       #3  0x00002aaaaaab6140 in _dl_rtld_di_serinfo () from /lib64/ld-linux-x86-64.so.2
       #4  0x00002aaaae006542 in dlerror () from /lib/libdl.so.2
       #5  0x00002aaaae0060cf in dlclose () from /lib/libdl.so.2
       #6  0x00002aaaac17e361 in cc_dl_close (handle=0x79b630) at /home/mortene/code/Coin/src/glue/dl.c:699
       #7  0x00002aaaac17e4f3 in glsymbols_handle_cleanup () at /home/mortene/code/Coin/src/glue/dl.c:874
       #8  0x00002aaaaaff6dd3 in coin_atexit_cleanup () at /home/mortene/code/Coin/src/tidbits.c:1195
       #9  0x00002aaaac566d29 in SoDB::finish () at /home/mortene/code/Coin/src/misc/SoDB.cpp:675
       
       20070105 mortene.
    */
    /* cc_dl_close(glsymbols_handle); */

    glsymbols_handle = NULL;
  }

  attempted_glsymbols = FALSE;
}

/* Using the process handle to get at OpenGL symbols is not always
   workable, there are cases where the process handle will not "lead
   us" to Coin symbols or OpenGL symbols. (Like e.g. when running
   under the Pivy Coin-in-Python binding's interpreter.)

   Another example is Windows platforms, where GetProcAddress() will
   *only* resolve symbols from the specific DLL module handle, so
   there it seems like we will have to use the OpenGL DLL handle
   directly.

   Therefore, various handles are tried in sequence: first the process
   handle (which usually works for UNIX- and UNIX-like systems), then
   the Coin handle (which usually works for the special cases, as when
   Coin is running within a dynamic language interpreter), then a
   handle directly to OpenGL (which should work for Windows
   platforms).

   Testing is done in that succession because chances are better at
   getting a valid handle for the process, than for Coin, which again
   is more likely to be available than one for OpenGL.

   The returned handle will be closed on application exit, so client
   code should not clean it up.
*/
cc_libhandle
cc_dl_handle_with_gl_symbols(void)
{
  typedef cc_libhandle handlefetch(void);
  cc_libhandle hnd;
  size_t i;

  handlefetch * f[] = {
    cc_dl_process_handle, cc_dl_coin_handle, cc_dl_opengl_handle
  };

  /*G.Barrand
  if (attempted_glsymbols) { return glsymbols_handle; }

  attempted_glsymbols = TRUE;
  */

  coin_atexit((coin_atexit_f *)glsymbols_handle_cleanup, CC_ATEXIT_NORMAL);

  for (i = 0; i < (sizeof(f) / sizeof(f[0])); i++) {
    hnd = (*f[i])();
    if (hnd) {
      void * glchk = cc_dl_sym(hnd, "glGetString");
      if (cc_dl_debugging()) {
        cc_debugerror_postinfo("cc_dl_handle_with_gl_symbols",
                               "successfully found image handle for '%s', "
                               "testing OpenGL symbol access: "
                               "cc_dl_sym(..., \"glGetString\") == %p",
                               cc_string_get_text(&hnd->libname),
                               glchk);
      }
      if (glchk) {
        /*G.Barrand glsymbols_handle = hnd; */
        return hnd;
      }
      cc_dl_close(hnd); /* OpenGL symbol not found, close again */
    }
  }
  return NULL;
}

/* ********************************************************************** */
