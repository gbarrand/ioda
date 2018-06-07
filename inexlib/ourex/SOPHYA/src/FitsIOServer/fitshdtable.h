/*    
   --- SOPHYA software - FitsIOServer module ---
   R. Ansari , 2005
   (C) UPS+LAL IN2P3/CNRS     (C) DAPNIA-SPP/CEA 
*/
#ifndef FITSDTABLEHAND_H
#define FITSDTABLEHAND_H

#include "machdefs.h"
#include <string>
#include "datatable.h"

#include "fitshandler.h"

namespace SOPHYA {

/*! 
  \ingroup FitsIOServer
  \brief FITS I/O handler for DataTable objects
*/



inline FitsInOutFile& operator << (FitsInOutFile& os, BaseDataTable const & obj)
{ FitsHandler<BaseDataTable> fio(const_cast<BaseDataTable &>(obj));  fio.Write(os);  return os; }

inline FitsInOutFile& operator >> (FitsInOutFile& is, BaseDataTable & obj)
  { FitsHandler<BaseDataTable> fio(obj); is.SkipEmptyFirstHDU();
  fio.Read(is); is.MoveToNextHDU(); return(is); }


} // Fin du namespace

#endif

