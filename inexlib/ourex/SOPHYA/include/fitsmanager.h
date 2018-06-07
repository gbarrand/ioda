/*    
   --- SOPHYA software - FitsIOServer module ---
   R. Ansari , 2005 
   (C) UPS+LAL IN2P3/CNRS     (C) DAPNIA-SPP/CEA 
*/
#ifndef FITSMANAGER_H
#define FITSMANAGER_H

#include "machdefs.h"
#include <string>
#include <iostream>
#include "fitsinoutfile.h"

namespace SOPHYA {

class FitsHandlerInterface;

/*! 
  \ingroup FitsIOServer
  \brief Top level FITS I/O operations and handler management.
*/
class FitsManager {
 public:
  //! Called once for each handler during init phase.
  static int  RegisterHandler(FitsHandlerInterface * fhi, int glev=0, string desc="");
  //! Print list of handlers on cout 
  static int  ListHandlers();
  //! Finds the appropriate handler and writes the object \b o to fits file \b os 
  static void Write(FitsInOutFile& os, AnyDataObj & o);
  //! Finds the appropriate handler and reads the object \b o from fits file \b is 
  static void Read(FitsInOutFile& is, AnyDataObj & o);
  //! Finds the appropriate reader for the current HDU and reads the data form \b is
  static FitsHandlerInterface * Read(FitsInOutFile& is);
  //! Scans the fits file and prints information about each HDU on formatted stream \b os
  static int ScanFile(string filename, ostream& os, int slev=0);
  //! Scans the fits file and prints information about each HDU on \b cout
  static inline int ScanFile(string filename, int slev=0)
    { return ScanFile(filename, cout, slev); }

 protected:
  //! Finds the appropriate handler for the object \b o in the list of registered handlers.
  static FitsHandlerInterface* FindHandler(AnyDataObj & o);
  //! Finds the appropriate reader for the current HDU in the list of registered handlers.
  static FitsHandlerInterface* FindReader(FitsInOutFile& is);
};

} // Fin du namespace

#endif

