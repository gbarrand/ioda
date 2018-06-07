#include "sopnamsp.h"
#include "machdefs.h"
#include "pexceptions.h" 
#include "fitstarray.h" 
///////////////////////////////////////////////////////////
//   Les objets delegues pour la gestion de persistance sur fichiers fits 
//    pout TArray
///////////////////////////////////////////////////////////

/*!
  \class SOPHYA::FITS_TArray
  \ingroup FitsIOServer
  FITS format I/O handler for SOPHYA::TArray objects in Sophya.
*/

using namespace SOPHYA;

template <class T>
FITS_TArray<T>::FITS_TArray()
{
  dobj_= NULL;
  ownobj_=false;
}

template <class T>
FITS_TArray<T>::FITS_TArray(const char inputfile[],int hdunum)
{
  dobj_=NULL;
  ownobj_=false; 
  Read(inputfile,hdunum);
}

template <class T>
FITS_TArray<T>::FITS_TArray(const TArray<T> & obj) 
{ 
    dobj_ = new TArray<T>(obj);
  ownobj_=true; 
}

template <class T>
FITS_TArray<T>::FITS_TArray(TArray<T> *obj) 
{ 
  dobj_ = obj;
  ownobj_=false; 
}


template <class T>
FITS_TArray<T>::~FITS_TArray()
{
  if (ownobj_ && dobj_) delete dobj_;
}


template <class T>
AnyDataObj* FITS_TArray<T>::DataObj()
{
  return(dobj_);
}


template <class T>
void   FITS_TArray<T>::SetDataObj(AnyDataObj & o) 
  {   
    TArray<T>* po = dynamic_cast<  TArray<T>* >(& o); 
    if (po == NULL) return;
    if (ownobj_ && dobj_) delete dobj_;  
    dobj_ = po; 
    ownobj_ = false; 
  }    



template <class T>
void FITS_TArray<T>::ReadFromFits(FitsInFile& is)
{
  if (!is.IsFitsImage())
    {
      throw PException("ReadFromFits: the fits file seems not to be an image");
    }
  int dimension = is.nbDimOfImage();
  sa_size_t * siz = new sa_size_t[dimension];
  for (int k=0; k< dimension; k++) siz[k] = is.dimOfImageAxes()[k];
  if(dobj_ == NULL)
    {
      dobj_ = new TArray<T>(dimension,siz);
      ownobj_ = true;
    }
  else
    dobj_->ReSize(dimension,siz);
  
  delete [] siz;
  if (dobj_->Size() != is.nbOfImageData() )
  {
    cout << " total size of TArray: " << dobj_->Size() << endl;
    cout << " total size from fits file: " << is.nbOfImageData() << endl;
      throw PException("ReadFromFits: size conflict");
  }
  // On lit le tableau
  is.GetSingleColumn( dobj_->Data(),dobj_->Size());
    dobj_->Info()  = is.DVListFromFits();

}


template <class T>
void FITS_TArray<T>::WriteToFits(FitsOutFile& os)  
{
  if(dobj_ == NULL) return;
  uint_4 nbdim = dobj_->NbDimensions();
  if ( nbdim == 0) 
    {
      throw PException(" FITS_TARRAY: number of dimensions of the array = 0?");
    }
  //cout << "FITS_TARRAY: nombre de dimension du tableau a ecrire: " << nbdim << endl;
  int* naxisn = new int[nbdim];
  int k;
  for (k=0; k< nbdim; k++)
    {
      naxisn[k] = dobj_->Size(k);
     // cout << " nombre de donnees dans la la dimension " << k << " : " << naxisn[k] << endl;
    }
  char type;
  if ( typeid(T) == typeid(r_8) ) type='D';
  else 
    if ( typeid(T) == typeid(r_4) )  type='E';
    else
      if ( typeid(T) == typeid(int_4) )  type='J';
      else
	{
	  cout <<  " type du tableau= " <<  typeid(T).name() << endl;
	  throw IOExc("FITS_TArray:: unknown type");
	}

  int nbels = 1;
  for (k=0; k< nbdim; k++) 
    {
      if (naxisn[k] > 0) nbels *= naxisn[k];
    }
 // cout << " nombre total d'elements a copier " << nbels << endl;
  DVList dvl( dobj_->Info() );  
  dvl["Content"]= "TArray";
  dvl.SetComment("Content", "name of SOPHYA object");
  os.makeHeaderImageOnFits(type, nbdim, naxisn, &dvl);
  if (!dobj_->IsPacked()) 
    {
      cout << " IsPacked() = " << dobj_->IsPacked() << endl;
      throw PException(" FITS_TARRAY: this case is not yet programmed");
      
    }
  if (dobj_->MinStep() != 1) 
    {
      cout << " MinStep()) = " << dobj_->MinStep() << endl;
      throw PException(" FITS_TARRAY: this case is not yet programmed");
      
    }
  os.PutImageToFits(nbels, dobj_->Data());
  
  delete [] naxisn;
}


#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template FITS_TArray<r_8>
#pragma define_template FITS_TArray<r_4>
#pragma define_template FITS_TArray<int_4>
#endif
#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
namespace SOPHYA {
template class FITS_TArray<r_8>;
template class FITS_TArray<r_4>;
template class FITS_TArray<int_4>;
}
#endif
