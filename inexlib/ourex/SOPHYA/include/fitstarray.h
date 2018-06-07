// Guy Le Meur 04/2000


#ifndef FITSTARRAY_SEEN
#define FITSTARRAY_SEEN


#include "pexceptions.h" 
#include "tarray.h"
#include "fitsfile.h"

namespace SOPHYA {


/////////////////////////////////////////////////////////////////////////
// Classe pour la gestion de persistance sur fichiers fits
/////////////////////////////////////////////////////////////////////////



template <class T>
class FITS_TArray : public FitsIOHandler {

public:

FITS_TArray();
FITS_TArray(const char inputfile[],int hdunum=0);
FITS_TArray(const TArray<T> & obj);
FITS_TArray(TArray<T> *obj);
virtual ~FITS_TArray();
virtual AnyDataObj* DataObj();
virtual   void      SetDataObj(AnyDataObj & o); 
inline operator TArray<T>() { return(*dobj_); }

protected :

void ReadFromFits(FitsInFile& is);
void WriteToFits(FitsOutFile& os) ;

  TArray<T> * dobj_;
  bool ownobj_;
};
//////////////////////////////////////////////////////////////////

template <class T>
inline FitsOutFile& operator << (FitsOutFile& fios, TArray<T> & arr)
  { FITS_TArray<T> fih(&arr); fih.Write(fios); return (fios); }

template <class T>
inline FitsInFile& operator >> (FitsInFile& fiis, TArray<T> & arr)
  { FITS_TArray<T> fih(&arr); fih.Read(fiis); return (fiis); }

} // Fin du namespace

#endif
