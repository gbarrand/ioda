// This may look like C code, but it is really -*- C++ -*-
// Gestion de block de donnees swapable
//                         R. Ansari  Mars 2005
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA
#ifndef PPFSWAPPER_H
#define PPFSWAPPER_H


#include "machdefs.h"
#include "swsegdb.h"
#include "ppersist.h"
#include "ppftpointerio.h"

#include "iostream"
using namespace std;

/*!
  \class SOPHYA::PPFDataSwapper
  \ingroup BaseTools

  Implementation of SOPHYA::DataSwapperInterface interface on PPF streams 
  (POutPersist , PInPersist) to be used with SOPHYA::SwSegDataBlock classes.

  PPFDataSwapper<T> implements reference counting and sharing to keep track 
  of POutPersist and PInPersist objects. 
  if PPF_TPointer_IO<T>::isFixed_IO_RecordSize() returns false, and in case of 
  re-writing to swap, the previous position is ignored, and the data is re-written 
  to swap and a new swap position is returned. If isFixed_IO_RecordSize() returns
  true, it is assumed that the PPF I/O record size is fixed and data is then 
  written to the same position in file.
*/

namespace SOPHYA {

////////////////////////////////////////////////////////////////
//// -------------- Class PPFDataSwapper<T> ----------------- //
////////////////////////////////////////////////////////////////

template <class T> 
class PPFDataSwapper : public DataSwapperInterface<T> {
public:
  //! Default constructor - no output, nor input streams
  PPFDataSwapper()   { Init(NULL,false,NULL,false); }
  //! Copy constructor
  PPFDataSwapper(const PPFDataSwapper<T> & a)  { mSRef=NULL;  Share(a); }
  //! Constructor from an output stream - swap out (write) only  
  PPFDataSwapper(POutPersist & os)  { Init(&os,false,NULL,false); }
  //! Constructor from an input stream - swap in (read) only  
  PPFDataSwapper(PInPersist & is)   { Init(NULL,false,&is,false); }
  //! Constructor from an output and an input stream - swap out (write) and in (read)   
  PPFDataSwapper(POutPersist & os, PInPersist & is)   { Init(&os,false,&is,false); }
  /*!
    \brief Constructor from an output stream - swap out (write) only 
 
    POutPersist delete handled by PPFDataSwapper if ados==true
  */
  PPFDataSwapper(POutPersist* os, bool ados=true)   { Init(os,ados,NULL,false); }
  /*!
    \brief Constructor from an input stream - swap in (read) only 
 
    PInPersist delete handled by PPFDataSwapper if adis==true
  */
  PPFDataSwapper(PInPersist* is, bool adis=true)   { Init(NULL,false,is,adis); }
  /*!
    \brief Constructor from an output and an input stream - swap out (write) and in (read) 
 
    POutPersist delete handled by PPFDataSwapper if ados==true
    PInPersist delete handled by PPFDataSwapper if adis==true.
  */
  PPFDataSwapper(POutPersist* os, PInPersist* is, bool ados=false, bool adis=false) 
    { Init(os,ados,is,adis); }

  //! enum to define swap mode : Out (write), In (read), InOut (read & write)
  
  //! constructor with specification of PPF file name and the swap mode 
  PPFDataSwapper(const char* inoutname, int mode)
  {
    Open(inoutname, mode);
  }
  //! constructor with specification of PPF file name and the swap mode 
  PPFDataSwapper(const string& inoutname, int mode)
  {
    Open(inoutname.c_str(), mode);
  }

  inline PInPersist*  InStream() { return mSRef->ppfis; }
  inline POutPersist* OutStream() { return mSRef->ppfos; }
  /*
  inline void SetInStream(PInPersist & is) { ppfis = &is; }
  inline void SetOutStream(POutPersist & os) { ppfos = &os; }
  */
  // Operateur = , on recopie les pointeurs des streams 
  inline PPFDataSwapper<T>& operator = (PPFDataSwapper<T> const & a) 
  { Share(a);  return *this; }

  //! Implementation of WriteToSwap on PPF streams 
  virtual int_8 WriteToSwap(const T * d, size_t sz, int_8 idx, int_8 oswp=0, bool osw=false)
  {
    if (mSRef->ppfos == NULL) 
      throw NotAvailableOperation("PPFDataSwapper<T>::WriteToSwap() - No POutPersist stream !");
    int_8 tag;
    int_8 cspos;
    bool fgrw=false;
    if (osw && PPF_TPointer_IO<T>::isFixed_IO_RecordSize() 
	&& (mSRef->ppfos->getStream().isSeekable()) )  fgrw=true;
    if (fgrw) {
      cspos=mSRef->ppfos->getStream().tellp();
      mSRef->ppfos->getStream().seekp(oswp); 
    }
    tag = mSRef->ppfos->WritePositionTag();	
    PPF_TPointer_IO<T>::Write(*(mSRef->ppfos), d, sz);
    if (fgrw) mSRef->ppfos->getStream().seekp(cspos); 
    mSRef->fglastopwrite=true;
    return tag;
  }
  //! Implementation of ReadFromSwap from PPF streams 
  virtual void  ReadFromSwap(int_8 idx, int_8 swp, T* d, size_t sz)
  {
    //DBG    cout << " *DBG*ReadFromSwap(idx="<<idx<<" ,swp="<<swp<<", d="<<hex<<d<<dec<<" ,sz="<<sz<<")"<<endl;
    if (mSRef->ppfis == NULL) 
      throw NotAvailableOperation("PPFDataSwapper<T>::ReadFromSwap() - No PInPersist stream !");
    if (mSRef->fglastopwrite)   { 
      mSRef->ppfos->getStream().flush();
      mSRef->fglastopwrite=false;
    }
    int_8 cspos; 
    if (mSRef->ppfis->getStream().isSeekable()) cspos=mSRef->ppfis->getStream().tellg();
    bool ok = mSRef->ppfis->GotoPositionTag(swp);
    PPF_TPointer_IO<T>::Read(*(mSRef->ppfis), d, sz);
    if (mSRef->ppfis->getStream().isSeekable()) mSRef->ppfis->getStream().seekg(cspos);
    return;
  }
  virtual DataSwapperInterface<T>* Clone()
  {
    PPFDataSwapper<T> * rsw = new PPFDataSwapper<T>(*this) ;
    //DBG    cout << "PPFDataSwapper<T>::Clone() po,pin: " << hex << rsw->ppfos << " , " << rsw->ppfis << endl;
    return rsw;
  }
  //! Open PPF file(s) and calls Init()
  void Open(const char* inoutname, int mode)
  {
    POutPersist* os=NULL;
    if (inoutname && (mode&ios::out))  os = new POutPersist(inoutname);
    PInPersist* is=NULL;
    if (inoutname && (mode&ios::in)) is = new PInPersist(inoutname,false);
    if (mSRef) Delete();
    Init(os, true, is, true);
  }
protected:
  //! Initializes the shared data structure
  void Init(POutPersist* os, bool ados, PInPersist* is, bool adis)
  {
    mSRef = new PSWREF;
    mSRef->nref = 1;
    mSRef->ppfos = os;
    mSRef->adfos = ados;
    mSRef->ppfis = is;
    mSRef->adfis = adis;
    mSRef->fglastopwrite = false;
    DataSwapperInterface<T>::mode_=0;
    if (os) DataSwapperInterface<T>::mode_ |= ios::out; 
    if (is) DataSwapperInterface<T>::mode_ |= ios::in; 
    return;
  }
  //! Initializes the shared data structure
  void Share(const PPFDataSwapper<T>& a)
  {
    if (mSRef) Delete();
    mSRef = a.mSRef;
    mSRef->nref++;
    DataSwapperInterface<T>::mode_ = a.DataSwapperInterface<T>::mode_;
  }
  //! Decrement the number of reference counts, and free the memory if NRef=0
  void Delete() 
  {
    if (mSRef == NULL) return;
    mSRef->nref--;
    if (mSRef->nref == 0)  { 
      if (mSRef->adfos && mSRef->ppfos) delete mSRef->ppfos;
      if (mSRef->adfis && mSRef->ppfis) delete mSRef->ppfis;
      delete mSRef;
    }
    mSRef = NULL; 
    return; 
  }
  /*! \cond 
    PSWREF structure for reference management - for internal use by PPFDataSwapper
  */
  typedef struct {
    size_t nref;      // Number of references to the data structure
    POutPersist* ppfos;
    bool adfos;
    PInPersist* ppfis;
    bool adfis;
    bool fglastopwrite;  // if true, last operation was write 
  } PSWREF;
  /*! \endcond */
  PSWREF *  mSRef;    //!< SDREF structure for reference sharing
};


} // Fin du namespace

#endif
