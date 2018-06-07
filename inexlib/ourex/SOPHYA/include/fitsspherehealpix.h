/*    
   --- SOPHYA software - FitsIOServer module ---
   Guy Le Meur 03/2000    R. Ansari , 2006 
   (C) UPS+LAL IN2P3/CNRS     (C) DAPNIA-SPP/CEA 
*/
#ifndef FITSSPHEREHEALPIX_SEEN
#define FITSSPHEREHEALPIX_SEEN



#include "spherehealpix.h"
#include "anydataobj.h"
#include "fitsfile.h"

namespace SOPHYA {
///////////////////////////////////////////////////////////////////////
// ------------- Classe pour la gestion de persistance I/O format fits--
// spherehealpix
//////////////////////////////////////////////////////////////////////
template <class T>
class FITS_SphereHEALPix : public FitsIOHandler  
{
public:

FITS_SphereHEALPix();
FITS_SphereHEALPix(const char inputfile[],int hdunum=0); 
FITS_SphereHEALPix(const SphereHEALPix<T>& obj);
FITS_SphereHEALPix(SphereHEALPix<T>* obj);
virtual ~FITS_SphereHEALPix();
virtual AnyDataObj* DataObj();
virtual void        SetDataObj(AnyDataObj & o);

virtual int         CheckHandling(AnyDataObj & o) ;
virtual int         CheckReadability(FitsInOutFile& is);
virtual FitsHandlerInterface* Clone();

void Mollweide_picture_projection(const char flnm[]);
void sinus_picture_projection(const char flnm[]);
//void ReWriteFromFitsToFits(FitsInFile& headerin, FitsOutFile& os); 

inline operator SphereHEALPix<T>() { return(*dobj_); }

protected:

//virtual void ReadFromFits(FitsFile& fn);
virtual void ReadFromFits(FitsInFile& is);
virtual void WriteToFits(FitsOutFile& os) ;
SphereHEALPix<T>* dobj_;
bool ownobj_;
};


//////////////////////////////////////////////////////////////////

template <class T>
inline FitsInOutFile& operator << (FitsInOutFile& fios, SphereHEALPix<T> & sph)
  { FITS_SphereHEALPix<T> fih(&sph); fih.Write(fios); return (fios); }

template <class T>
inline FitsInOutFile& operator >> (FitsInOutFile& fiis, SphereHEALPix<T> & sph)
  { FITS_SphereHEALPix<T> fih(&sph); fiis.SkipEmptyFirstHDU();
  fih.Read(fiis); fiis.MoveToNextHDU(); return (fiis); }


} // Fin du namespace

#endif
