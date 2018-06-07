//
// Gestionnaire de lien dynamique   -   R. Ansari 12/98
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA
//
#include <machdefs.h>

#include "sopnamsp.h"
#include "pdlmgr.h"
#include <stdio.h>
#include <stdlib.h>

#include <iostream>

#if defined(__APPLE__) //G.Barrand : to disconnect system().
#include <TargetConditionals.h>
#endif

// Extension de noms de fichiers Shared libs
static const char* sofext = ".so";
static const char* sofext_HPUX = ".sl";
// static const char* sofext_Darwin = ".dylib"; pas necessaire - Reza 02/2002

// Variables et methodes static
int PDynLinkMgr::numSO = 0;
string*  PDynLinkMgr::tmpDir = NULL;

/*!
  \class SOPHYA::PDynLinkMgr
  \ingroup SysTools
  This classes handles the run-time operations related to using shared
  libraries. The present version has been adapted for different Unix 
  flavours (Linux, HP/Compaq/Digital OSF-Tru64, SGI IRIX, IBM AIX, Sun Solaris,
  MacOS X/Darwin). \n
  For MacOS X/Darwin (version <= 10.3), the NSxxx API 
  <tt> (NSLinkModule , NSCreateObjectFileImageFromFile, ...) </tt> 
  is used. From MacOSX v >= 10.4, NSxxx API is deprecated.
  The example here shows the linking of shared library named "mylib.so" 
  containing a function \c double \c myfunction(double x). 
  \code
  #include "pdlmgr.h"
  typedef double (* AFunctionOfX) (double x);
  {
  // ...
  string soname = "mylib.so";
  string funcname = "myfunction";
  PDynLinkMgr dyl(son);
  AFunctionOfX f = (AFunctionOfX)dyl.GetFunction(funcname);
  double x = 3.1425;
  if (f != NULL) 
    cout << " X= " << x << " myfunction(x)=" << f(x) << endl;
  // ...
  }
  \endcode
*/

/* --Methode-Static-- */
/*! Sets the path for a temporary space where shared libraries are copied.
    The path is appended to \b LD_LIBRARY_PATH
*/
void PDynLinkMgr::SetTmpDir(string const & path)
{
if ( (path.length() > 0) && (path[path.length()-1] != '/')  ) GetTmpDir() = path + '/';
else GetTmpDir() = path;
#if defined(OSF1) || defined(Linux) || defined(SunOS) || defined(IRIX64)
  char* varenv=NULL;
#if !defined(IRIX64)
  string cmd = "LD_LIBRARY_PATH=";
  varenv=getenv("LD_LIBRARY_PATH");
#else
#ifdef SO_ARCH64
  string cmd = "LD_LIBRARYN64_PATH=";
  varenv=getenv("LD_LIBRARYN64_PATH");
#else
  string cmd = "LD_LIBRARYN32_PATH=";
  varenv=getenv("LD_LIBRARYN32_PATH");
#endif
#endif

  if (varenv == NULL) { 
    cmd += '.';  
    if (path.length() > 0)  cmd += ':' + path; 
    }
  else { 
    if (varenv[0] != '.') cmd += ".:";
    if (path.length() > 0)  cmd += path + ':';
    cmd += varenv;
    putenv(const_cast<char *>(cmd.c_str()));
    }
#elif defined(AIX)
  string cmd = "LIBPATH=";
  char* varenv=NULL;
  varenv=getenv("LIBPATH");
  if (varenv == NULL) { 
    cmd += '.';  
    if (path.length() > 0)  cmd += ':' + path; 
    cmd += ":/usr/lib:/lib";
    }
  else { 
    if (varenv[0] != '.') cmd += ".:";
    if (path.length() > 0)  cmd += path + ':';
    cmd += varenv;
    //AIX: l'argument de putenv n'est pas const (Reza Dec 2005)
    //On fait un new - mais qui fait le delete ?
    char * pevc = new char [cmd.length()+1];
    strcpy(pevc, cmd.c_str());
    putenv(pevc);
    }

#endif
return;
}

/* --Methode-Static-- */
/*! Returns the temporary space path */
string& PDynLinkMgr::GetTmpDir()
{
if (tmpDir == NULL) { 
  tmpDir = new string("");
  char* varenv;
  if ( (varenv=getenv("TMPDIR")) != NULL )  { 
    *tmpDir = varenv; 
    if ((*tmpDir)[tmpDir->length()-1] != '/') (*tmpDir) += '/';
  }
}
return(*tmpDir);
}

/* --Methode-Static-- */
/*! Compiles the C source file named \b fname and creates the 
    corresponding shared library linking against the standard 
    C library (-lc) and the math library (-lm).
    Returns a pointer to the created PDynLinkMgr object (by new).
    Returns the NULL pointer in case of errors. 
*/
PDynLinkMgr* PDynLinkMgr::BuildFromCFile(string const & fname)
{
#if TARGET_OS_IPHONE //G.Barrand :
  cerr << "PDynLinkMgr::BuildFromCFile() : iOS : system() not available." << endl; 
  return(NULL);
#else
size_t l = fname.length();
if (l < 1)  return(NULL);
string fnameobj = GetTmpDir()+"tmp_pdl.o";

string cmd;
int rc;

string compopt = "cc -c -o ";
#if defined(__GNUG__) 
#ifndef SO_NOFPIC
compopt = "cc -fPIC -c -o " ;
#endif
#endif
#if defined(IRIX64) && defined(SO_ARCH64)
compopt = "cc -64 -c -o " ;
#endif
#if defined(AIX) && defined(SO_ARCH64)
compopt = "cc -q64 -c -o " ;
#endif

// Compilation du fichier
cmd = compopt + fnameobj + " " + fname;

rc = system(cmd.c_str());
if (rc != 0)  { 
  cerr << "PDynLinkMgr::BuildFromCFile() Error Rc(" << cmd <<")= "<< rc << endl; 
  return(NULL);
  }

char buff[32];
numSO++;
#ifndef HPUX
sprintf(buff,"pdlmgr%d%s", numSO,sofext);
#endif
string fnameso = GetTmpDir()+buff;

// Creation du shared-lib
#if defined(OSF1) 
  cmd = "ld -shared -o " + fnameso + " -all " + fnameobj + " -none -lm -lc";
#elif defined(Linux)
  cmd = "ld -shared -o " + fnameso + " " + fnameobj + " -lm -lc";
#elif defined(SunOS)
  cmd = "ld -G -o " + fnameso + " " + fnameobj + " -lm -lc";
#elif defined(IRIX64)
#ifdef SO_ARCH64
  cmd = "ld -64 -shared -o " + fnameso + " " + fnameobj + " -lm -lc";
#else
  cmd = "ld -shared -o " + fnameso + " " + fnameobj + " -lm -lc";
#endif
#elif defined(AIX)
  //  cmd = "ld -G -bnogc -bexpall -bM:1L -o " + fnameso + " " + fnameobj;
#ifdef SO_ARCH64
  cmd = "cc -brtl -qmkshrobj -q64 -o " + fnameso + " " + fnameobj + " -lm -lc";;
#else
  cmd = "cc -brtl -qmkshrobj -o " + fnameso + " " + fnameobj + " -lm -lc";;
#endif
#elif defined(HPUX)
  cmd = "ld -b -o " + fnameso + " " + fnameobj + " -lm -lc";
#elif defined(Darwin)
  // cmd = "cc -bundle -flat_namespace -undefined suppress -o " + fnameso + " " + fnameobj + " -lm  -lcc_dynamic -lSystem ";
  cmd = "cc -bundle -o " + fnameso + " " + fnameobj + " -lSystem -lm";
#else 
  cmd = "ld -o " + fnameso + " " + fnameobj + " -lm -lc";
#endif
rc = system(cmd.c_str());
if (rc != 0)  { 
  cerr << "PDynLinkMgr::BuildFromCFile() Error Rc(" << cmd <<")= "<< rc << endl; 
  return(NULL);
  }
PDynLinkMgr* rdyn = new PDynLinkMgr(fnameso, false);
rdyn->copy = true;
return(rdyn);
#endif //TARGET_OS_IPHONE
}

/* --Methode-- */
/*! The constructor. 
    \param soname : Name of the shared library. ".so" is appended
    to the name if no dot "." is found in the name.
    \param cp : if true, copies the shared library in the temporary space.
*/ 
PDynLinkMgr::PDynLinkMgr(string& soname, bool cp)
{
#if TARGET_OS_IPHONE //G.Barrand :
  cerr << "PDynLinkMgr::PDynLinkMgr() : iOS : system() not available." << endl; 
#else
dylok = false;
soName = "";

if (soname.find_last_of(".") > soname.length())  
#ifdef HPUX
  soname += sofext_HPUX;
#else
  soname += sofext;
#endif

string fnameso;
if (cp) {
  numSO++;
  char buff[32];
#ifndef HPUX
  sprintf(buff,"pdlmgr%d%s", numSO,sofext);
#elif defined(Darwin)
  sprintf(buff,"pdlmgr%d%s", numSO,sofext_Darwin);
#else
  sprintf(buff,"pdlmgr%d%s", numSO,sofext_HPUX);
#endif
  fnameso = GetTmpDir()+buff;
  string cmd = "cp " + soname + " " + fnameso;
  int rc = system(cmd.c_str());
  if (rc != 0)  { 
    cerr << "PDynLinkMgr::PDynLinkMgr() Error Rc(" << cmd <<")= "<< rc << endl; 
    return;
    }
  } 
else fnameso = soname;
copy = cp;
soName = fnameso;

string sdylerr;
#if defined(HPUX)
cerr << "PDynLinkMgr::PDynLinkMgr() Not yet available on HP-UX " << endl;
return;
#elif defined(Darwin) && ( GCC_VERSION <= 3350 )
NSObjectFileImageReturnCode nsrc = NSCreateObjectFileImageFromFile(fnameso.c_str(), &nsobjfile);
if (nsrc == NSObjectFileImageSuccess) {
  nsmod = NSLinkModule(nsobjfile, fnameso.c_str(),
		       NSLINKMODULE_OPTION_BINDNOW | NSLINKMODULE_OPTION_PRIVATE | 
		       NSLINKMODULE_OPTION_RETURN_ON_ERROR);
  // The second argument of NSLinkModule is the module name
  // We might associate later a name for the module in the PDynLinkMgr object
  if (nsmod != NULL) dylok = true;
  else cerr << "PDynLinkMgr::PDynLinkMgr()/ Error from NSLinkModule ... " << endl;
}
else { 
  cerr << "PDynLinkMgr::PDynLinkMgr()/ Error from NSCreateObjectFileImageFromFile ... " << endl;
  if (nsrc == NSObjectFileImageFailure) cerr << " ErrCode= NSObjectFileImageFailure " << endl;
  else if (nsrc == NSObjectFileImageInappropriateFile) cerr << " ErrCode= NSObjectFileImageInappropriateFile" << endl;
  else if (nsrc == NSObjectFileImageArch ) cerr << " ErrCode= NSObjectFileImageArch" << endl;
  else if (nsrc == NSObjectFileImageFormat) cerr << " ErrCode= NSObjectFileImageFormat" << endl;
  else if (nsrc == NSObjectFileImageAccess) cerr << " ErrCode= NSObjectFileImageAccess" << endl;
  }
#else
dlhandle = NULL;
dlhandle =  dlopen(fnameso.c_str(), RTLD_NOW);
if (dlhandle != NULL) dylok = true;
else { 
  sdylerr = "Loader Error (dlerror()):";
  sdylerr += dlerror();
  }
#endif

if (!dylok)  {
  cerr << "PDynLinkMgr::PDynLinkMgr(): Error opening SO " << fnameso 
       << " (" << soname << ")" << endl; 
  if (sdylerr.length() > 0) cerr << sdylerr;
  return; 
  }
#endif //TARGET_OS_IPHONE
}

/* --Methode-- */
/*! Destructor. Closes the shared library. Removes the file if it had been
    copied in the temporary space, or generated by \b BuildFromCFile */
PDynLinkMgr::~PDynLinkMgr()
{
#if TARGET_OS_IPHONE //G.Barrand :
  cerr << "PDynLinkMgr::~PDynLinkMgr() : iOS : system() not available." << endl; 
#else
#if defined(HPUX)
cerr << "PDynLinkMgr::~PDynLinkMgr() Not yet available on HP-UX " << endl;
// return;
#elif defined(Darwin) && ( GCC_VERSION <= 3350 )
if (dylok) { 
  if (nsmod != NULL)  NSUnLinkModule(nsmod, NSUNLINKMODULE_OPTION_NONE);
  NSDestroyObjectFileImage(nsobjfile);
  }
#else
if (dylok) {
  if (dlhandle) dlclose(dlhandle);    dlhandle = NULL;
  }
#endif
if (copy) {
  string cmd = "rm -f " + soName;
  system(cmd.c_str());
  }
#endif //TARGET_OS_IPHONE 
}

/* --Methode-- */
/*! Returns a handle to the function named \b funcname.
    Returns the NULL pointer in case of error          */
DlFunction PDynLinkMgr::GetFunction(string const & funcname)
{
DlFunction f = NULL;
if (!dylok) {
  cerr << "PDynLinkMgr::GetFunction() Error:sharedobjet/dynamic library not open -> f=NULL" << endl;
  return f;
}
#if defined(HPUX)
cerr << "PDynLinkMgr::GetFunction() Not yet available on HP-UX " << endl;
return f;
#endif
#if defined(Darwin) && ( GCC_VERSION <= 3350 )
string funame = "_" + funcname;
NSSymbol nsf = NSLookupSymbolInModule(nsmod, funame.c_str());
f = (DlFunction)NSAddressOfSymbol(nsf);
#else 
string const & funame = funcname;
if (dlhandle != NULL)  
  f = (DlFunction)dlsym(dlhandle, funame.c_str());
#endif

if (f == NULL) cerr << "PDynLinkMgr::GetFunction(): Error linking " << funcname << endl;
return(f);
}


