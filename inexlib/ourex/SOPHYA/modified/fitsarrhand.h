/*    
   --- SOPHYA software - FitsIOServer module ---
   R. Ansari , 2005-2008 
   (C) UPS+LAL IN2P3/CNRS     (C) DAPNIA-SPP/CEA 
*/
#ifndef FITSARRHAND_H
#define FITSARRHAND_H

#include "machdefs.h"
#include <string>
#include "tarray.h"
#include "tvector.h"

#include "fitshandler.h"
#include "fitsblkrw.h"

namespace SOPHYA {

/*! 
  \ingroup FitsIOServer
  \brief FITS I/O handler for array objects
*/

template <class T>
class FitsArrayHandler : public FitsHandlerInterface {
public :
  FitsArrayHandler() { dobj=NULL; ownobj=true; }
  FitsArrayHandler(TArray< T > & obj) { dobj = &obj; ownobj=false; }
  virtual   ~FitsArrayHandler() { if (ownobj && dobj) delete dobj; }
  virtual   AnyDataObj* DataObj() { return(dobj); }

  virtual   int         CheckHandling(AnyDataObj & o) 
  {  
    if ( (typeid(o) == typeid(TArray<T>)) ||
	 (typeid(o) == typeid(TMatrix<T>)) ||
	 (typeid(o) == typeid(TVector<T>)) )  return 2;
    TArray<T> * po = dynamic_cast< TArray<T> * >(& o); 
    if (po == NULL) return 0;
    else return 1;
  }

  virtual   void        SetDataObj(AnyDataObj & o) 
  {  
    TArray<T> * po = dynamic_cast< TArray<T> * >(& o); 
    if (po == NULL)  {
      string msg = "FitsHandler<T>::SetDataObj() Wrong object type: " ;
      msg += typeid(o).name(); 
      throw TypeMismatchExc(msg);    
    }
    if (ownobj && dobj) delete dobj;  dobj = po; ownobj = false; 
  }

  virtual int         CheckReadability(FitsInOutFile& is)
  {
    if (is.CurrentHDUType() != IMAGE_HDU)  return 0;
    T x = 0;
    LONGLONG naxes[BASEARRAY_MAXNDIMS];
    int naxis=BASEARRAY_MAXNDIMS;
    int imgtyp = is.GetImageHDUInfo(naxis, naxes);
    if (naxis < 1) return 0;
    if (FitsTypes::ImageType(x) == imgtyp)  return 2;
    else return 1;
  }

  virtual FitsHandlerInterface* Clone() 
    { return new FitsArrayHandler< T >(); }

  inline operator T&() { return(*dobj); }

  //----- Ecriture
  //! Writes the complete array as an IMAGE HDU
  virtual void     Write(FitsInOutFile& os)
    {
      if (( dobj == NULL) || (dobj->Size() < 1))  
  	  throw NullPtrError("FitsArrayHandler<T>::Write() dobj=NULL or dobj->Size()=0");
      if (!(dobj->IsPacked()))
	throw NotAvailableOperation("FitsArrayHandler<T>::Write() fitsI/O NOT supported for unpacked arrays");

      LONGLONG naxes[BASEARRAY_MAXNDIMS] = {0,0,0,0,0};
      for(int_4 id=0; id<dobj->NbDimensions(); id++) 
	naxes[id] = dobj->Size(id);
      int naxis=dobj->NbDimensions();
      T x = 0;
      TVector<T> * pvec = dynamic_cast< TVector<T> * >(dobj); 
      if (pvec!=NULL) {  naxis=1;  naxes[0]=pvec->NElts(); }
      os.CreateImageHDU(FitsTypes::ImageType(x), naxis, naxes);

      // Ecriture de l'unite si necessaire
      MuTyV mtv;
      bool sdone;
      Units un=dobj->GetUnits(sdone);
      if (sdone) {
	mtv = un.Name();
	os.WriteKey("PHYSUNIT",mtv,"data phys. unit"); 
	mtv = un.ToStringFullForm();
	os.WriteKey("SOPUNITS",mtv,"SOPHYA unit"); 
      }
      os.WriteHeaderRecords(dobj->Info());
      mtv = "SOPHYA::TArray<T>";
      os.WriteKey("SOPCLSNM",mtv," Object class name "); 
      FitsBlockRW<T>::WriteImageData(os, dobj->Data(), dobj->Size());
    }

  //----- Lecture
  //! Resize the array and reads the complete IMAGE HDU to the array
  virtual void     Read(FitsInOutFile& is)
    {
      LONGLONG naxes[BASEARRAY_MAXNDIMS];
      int naxis=BASEARRAY_MAXNDIMS;
      is.GetImageHDUInfo(naxis, naxes);
      if ( dobj == NULL) {
	if (naxis == 1) dobj = new TVector<T>;
	else if (naxis == 2) { 
	  if ( (naxes[0] == 1) || (naxes[1] == 1) )  
	    dobj = new TVector<T>;  
	  else dobj = new TMatrix<T>;
	}
	else dobj = new TArray<T>;
	ownobj = true;
      }
      sa_size_t sz[BASEARRAY_MAXNDIMS];
      if (naxis > BASEARRAY_MAXNDIMS) { 
      	cerr << "FitsArrayHandler<T>::Read()/Warning FITS NAXIS=" << naxis 
      		 << " > BASEARRAY_MAXNDIMS=" << BASEARRAY_MAXNDIMS << endl;
      	naxis = BASEARRAY_MAXNDIMS;
      }
      //G.Barrand : _MSC_VER : cast (sa_size_t).
      for(int_4 id=0; id<naxis; id++) sz[id] = (sa_size_t)naxes[id]; //G.Barrand
      TMatrix<T> * pmx = dynamic_cast< TMatrix<T> * >(dobj); 
      if ((naxis==1)&&(pmx!=NULL)) {
	TVector<T> * pvec = dynamic_cast< TVector<T> * >(dobj); 
	if (pvec!=NULL) pvec->SetSize((sa_size_t)naxes[0]); //G.Barrand
	else pmx->SetSize(1,(sa_size_t)naxes[0]); //G.Barrand
      }
      else dobj->SetSize(naxis, sz, 1, false);
      FitsBlockRW<T>::ReadImageData(is, dobj->Data(), dobj->Size());
      // read header records 
      is.GetHeaderRecords(dobj->Info());
      // and manage the  associated Unit ...
      string kuns="SOPUNITS";
      if (dobj->Info().HasKey(kuns)) {
	Units un(dobj->Info().GetS(kuns));
	dobj->SetUnits(un);
	dobj->Info().DeleteKey(kuns);
	kuns="PHYSUNIT";
	dobj->Info().DeleteKey(kuns);	
      }
    }

  //----- Lecture avec specification d'offset et taille donnee par le tableau
  /*!  \brief Reads from the fits file into the array, at the specified offset 
  
     The array should be allocated and its size is used to define the number of 
     pixels to be read. 
     \warning The offset should be specified with SOPHYA/C array index convention 
     (starting at zero) - and NOT the cfitsio/fortran convention. 
     The offset should contain as many elements as the fits NAXIS keyword.
  */
  virtual void     ReadAtOffset(FitsInOutFile& is, sa_size_t* offset)
    {
    if (( dobj == NULL) || (dobj->Size() < 1))  
  	  throw NullPtrError("FitsArrayHandler<T>::ReadAtOffset() dobj=NULL or dobj->Size()=0");
	
    LONGLONG naxes[BASEARRAY_MAXNDIMS];
    int naxis=BASEARRAY_MAXNDIMS;
    is.GetImageHDUInfo(naxis, naxes);
    LONGLONG fpix[15]; 
    int namx = (naxis < 15) ? naxis : 15;
    for(int i=0; i<namx; i++) fpix[i] = offset[i]+1;
    FitsBlockRW<T>::ReadImageData(is, dobj->Data(), dobj->Size(), fpix);
    // read header records 
    is.GetHeaderRecords(dobj->Info());
    // and manage the  associated Unit ...
    string kuns="SOPUNITS";
    if (dobj->Info().HasKey(kuns)) {   
      Units un(dobj->Info().GetS(kuns));
      dobj->SetUnits(un);
      dobj->Info().DeleteKey(kuns);
      kuns="PHYSUNIT";
      dobj->Info().DeleteKey(kuns);	
    }
    }

  //----- Ecriture avec specification d'offset et taille donnee par le tableau
  /*!  \brief Writes the array to the fits file, at the specified offset 
  
     The array should be allocated and its size is used to define the number of 
     pixels to be written. 
     \warning The offset should be specified with SOPHYA/C array index convention 
     (starting at zero) - and NOT the cfitsio/fortran convention. 
     The offset should contain as many elements as the fits NAXIS keyword.
     \warning The IMAGE HDU should be already created using FitsInOutFile::CreateImageHDU().
  */
  virtual void     WriteAtOffset(FitsInOutFile& os, sa_size_t* offset)
    {
    if (( dobj == NULL) || (dobj->Size() < 1))  
  	  throw NullPtrError("FitsArrayHandler<T>::ReadAtOffset() dobj=NULL or dobj->Size()=0");

    LONGLONG naxes[BASEARRAY_MAXNDIMS];
    int naxis=BASEARRAY_MAXNDIMS;
    os.GetImageHDUInfo(naxis, naxes);
    LONGLONG fpix[15]; 
    int namx = (naxis < 15) ? naxis : 15;
    for(int i=0; i<namx; i++) fpix[i] = offset[i]+1;
    FitsBlockRW<T>::WriteImageData(os, dobj->Data(), dobj->Size(), fpix);	
    }

protected :
  TArray<T>  * dobj;
  bool ownobj;       // True si dobj obtenu par new
};


//! operator << overload to write a TArray<T> to a fits IMAGE HDU
template <class T>
inline FitsInOutFile& operator << (FitsInOutFile& os, TArray<T> const & obj)
{ FitsArrayHandler<T> fio(const_cast< TArray<T> &>(obj));  fio.Write(os);  return os; }

//! operator >> overload to read a TArray<T> from a fits IMAGE HDU
template <class T>
inline FitsInOutFile& operator >> (FitsInOutFile& is, TArray<T> & obj)
{ FitsArrayHandler<T> fio(obj); fio.Read(is); is.MoveToNextHDU(); return(is); }


} // Fin du namespace

#endif

