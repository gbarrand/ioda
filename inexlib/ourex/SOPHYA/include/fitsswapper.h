/*    
   --- SOPHYA software - FitsIOServer module ---
   R. Ansari,  C. Magneville,   Mai 2005 
   (C) UPS+LAL IN2P3/CNRS     (C) DAPNIA-SPP/CEA 
*/
#ifndef FITSSWAPPER_H
#define FITSSWAPPER_H


#include "machdefs.h"
#include "swsegdb.h"
#include "fitsinoutfile.h"
#include "fitsblkrw.h"

#include "basedtable.h"

/*!
  \class SOPHYA::FITSDataSwapper
  \ingroup FitsIOServer
  Implementation of SOPHYA::DataSwapperInterface interface on FITS files
  (FitsInOutFile) to be used with SOPHYA::SwSegDataBlock classes.
*/

namespace SOPHYA {

template <class T> 
class FITSDataSwapper : public DataSwapperInterface<T> {
public:
  FITSDataSwapper() 
    : fcol(0) , rowos(1) , repcnt(1), dtp(NULL)
  {
  }
  FITSDataSwapper(FitsInOutFile & ios, int col, int rcnt=1, BaseDataTable* dt=NULL) 
    : fios(ios) , fcol(col), rowos(1) , repcnt(rcnt), dtp(dt)
  {
  }

  inline FitsInOutFile & InOutStream() { return fios; }

  void SetInOutStream(FitsInOutFile & ios, int col, int rcnt=1) 
  {
    fios = ios;
    fcol = col;
    rowos = 1;
    repcnt = rcnt; 
    /*  On ne fait pas de check sur type HDU - Reza , 30/12/2005
    if ( fios != NULL) {
      fhdu = fios->CurrentHDU();
      if (fios->CurrentHDUType() != BINARY_TBL)
	throw FitsIOException("FITSDataSwapper<T>::SetInOutStream() CurrHDU not a BINARY_TBL");
    }    */
  }

  // Operateur = , on recopie les pointeurs des streams 
  FITSDataSwapper<T>& operator = (FITSDataSwapper<T> const & a)
  {
    fios = a.fios;
    fcol = a.fcol;
    rowos = a.rowos;
    repcnt = a.repcnt;
    dtp = a.dtp;
  }

  virtual int_8 WriteToSwap(const T * d, size_t sz, int_8 idx, int_8 oswp=0, bool osw=false)
  {
    /* fios->MoveAbsToHDU(fhdu);  On suppose qu'on est sur le bon HDU - Reza 30/12/2005 */
    LONGLONG row = rowos;
    if (osw)  row = oswp;
    if (dtp != NULL) 
      if ((row+sz/repcnt-1) > dtp->NRows() )  sz = (dtp->NRows()-row+1)*repcnt;
    FitsBlockRW<T>::WriteColumnData(fios, fcol, row, 1, d, sz);
    if (!osw) rowos += (sz/repcnt);
    return row;
  }

  virtual void  ReadFromSwap(int_8 idx, int_8 swp, T* d, size_t sz)
  {
    int_8 nrows = fios.GetNbRows();
    size_t szi = sz;
    if ((swp+sz/repcnt-1) > nrows) sz = (nrows-swp+1)*repcnt;  
    FitsBlockRW<T>::ReadColumnData(fios, fcol, swp, 1, d, sz);
    if (sz < szi) {
      T zz = d[sz-1];
      for(size_t k=sz; k<szi; k++)  d[k] = zz;
    }
  }

  virtual DataSwapperInterface<T>* Clone()
  {
    FITSDataSwapper<T> * rsw = new FITSDataSwapper<T>(fios, fcol, repcnt, dtp) ;
    rsw->rowos = rowos;
    rsw->dtp = dtp;    
    return rsw;
  }

protected:
  FitsInOutFile fios;    // Fichier FITS I/O 
  int fcol;              // Numero de colonne dans la table FITS 
  LONGLONG rowos;        // Numero de ligne (row) 
  LONGLONG repcnt;       // Nombre d'elements dans la colonne 
  BaseDataTable* dtp;
};

} // Fin du namespace

#endif
