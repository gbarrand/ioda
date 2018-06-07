// This may look like C code, but it is really -*- C++ -*-
// Gestionnaire de lien dynamique   -   R. Ansari 12/98
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA

#ifndef PDYNLINKMGR_SEEN
#define PDYNLINKMGR_SEEN

#include "machdefs.h"
#include <string>


#if defined(HPUX)
#include <dl.h>
/* Les fonction NSCreateObjectFile...() sont deprecated - Il faut utiliser les dlopen()...
   qui sont plus efficaces a partir de 10.4  - On se cale donc sur les versions de GCC */
#elif defined(Darwin) && ( GCC_VERSION <= 3350 )
#include <mach-o/dyld.h>
#else
#include <dlfcn.h>
#endif

namespace SOPHYA {

typedef void (* DlFunction) (void);

//! Dynamic Link Manager. 
class PDynLinkMgr {
public:
  // ---- Methodes globales (static)
  static PDynLinkMgr*   BuildFromCFile(string const & fname);  // Compil+link fichier C
  static void		SetTmpDir(string const & path);        // Repertoire des fichiers temporaires

  // ---- Constructeur et methodes 
  // Constructeur a partir d'un shared lib - copie en zone temporaire si cp==true
			PDynLinkMgr(string& soname, bool cp=true);  
  virtual		~PDynLinkMgr();

  virtual DlFunction	GetFunction(string const & funcname);              
  inline  DlFunction	GetFunction(const char * funcname) 
    { string fname = funcname;   return GetFunction(fname); }            

  inline  string        GetName() const { return soName; }

protected :
  static string&	GetTmpDir();
  static int	  numSO;
  static string*  tmpDir;

#if defined(HPUX)
  shl_t dlhandle;
#elif defined(Darwin) && ( GCC_VERSION <= 3350 )
  NSObjectFileImage nsobjfile;
  NSModule nsmod;
#else 
  void * dlhandle;
#endif
  bool dylok;
  string soName;
  bool copy;
};

} // namespace SOPHYA

#endif
