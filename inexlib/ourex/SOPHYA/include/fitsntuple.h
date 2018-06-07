/*    
   --- SOPHYA software - FitsIOServer module ---
   Guy Le Meur 03/2000    R. Ansari , 2006 
   (C) UPS+LAL IN2P3/CNRS     (C) DAPNIA-SPP/CEA 
*/


#ifndef FITSNTuple_SEEN
#define FITSNTuple_SEEN
#include "machdefs.h"
#include "ntuple.h"
#include "anydataobj.h"
#include "ppersist.h"
#include "fitsfile.h"

namespace SOPHYA {
///////////////////////////////////////////////////////////////////
// Classe pour la gestion de persistance sur fichiers fits
// pout NTuple 
////////////////////////////////////////////////////////////////
class  FITS_NTuple : public FitsIOHandler  
{

public:

FITS_NTuple();
FITS_NTuple(char inputfile[],int hdunum=0);
FITS_NTuple(const NTuple & obj);
 FITS_NTuple(NTuple* obj); 
virtual ~FITS_NTuple();

virtual   AnyDataObj* DataObj() { return(dobj_); }
virtual   void        SetDataObj(AnyDataObj & o) 
  {   
    NTuple* po = dynamic_cast<  NTuple* >(& o); 
    if (po == NULL) return;
    if (ownobj_ && dobj_) delete dobj_;  
    dobj_ = po; 
    ownobj_ = false; 
  }    
virtual   int         CheckHandling(AnyDataObj & o) 
  {  
    if (typeid(o) == typeid(NTuple)) return 2;
    NTuple * po = dynamic_cast< NTuple * >(& o); 
    if (po == NULL) return 0;
    else return 1;
  }

virtual int         CheckReadability(FitsInOutFile& is);

virtual FitsHandlerInterface* Clone() 
    { return new FITS_NTuple() ; }

/*!
fill the NTuple only with lines from firstLine-th to (firstLine+numberOfLines-1)-th of the FITS-file inputfile.
\param <firstLine>  first line  to be read (the first line of the file is numbered 0)
\param <numberOfLines>  number of lines to be read
*/
 void ReadLines(char inputfile[],int firstLine, int numberOfLines,int hdunum=0);

inline operator NTuple() { return(*dobj_); }
inline NTuple * getObj() { return(dobj_); }

protected:

  // implementation de FitsFile
virtual void ReadFromFits(FitsInFile& is);
virtual void WriteToFits(FitsOutFile& os) ;

 private : 

inline void InitNull() { fistLineToBeRead_= -1; numberOfLinesToBeRead_= -1;}


  // attributs de classe
  NTuple* dobj_;
  bool ownobj_;
  int fistLineToBeRead_;
  int numberOfLinesToBeRead_;
};
//////////////////////////////////////////////////////////////////

inline FitsInOutFile& operator << (FitsInOutFile& fios, NTuple & nt)
  { FITS_NTuple fih(&nt); fih.Write(fios); return (fios); }

inline FitsInOutFile& operator >> (FitsInOutFile& fiis, NTuple & nt)
  { FITS_NTuple fih(&nt); fiis.SkipEmptyFirstHDU();
  fih.Read(fiis); fiis.MoveToNextHDU(); return (fiis); }


} // Fin du namespace

#endif
