// This may look like C code, but it is really -*- C++ -*-
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// Gestion de block de donnees avec partage de references
//       R. Ansari -  Mars 2005 , updated Nov 2012
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA
#ifndef SEGDATABLOCK_H
#define SEGDATABLOCK_H

#include "machdefs.h"
#include "anydataobj.h"
#include <string.h> 
#include <vector>
#include <iostream>
#include <iomanip>
#include <typeinfo>
#include "ppftpointerio.h"
#include "sunitpcst.h"
#include "thsafeop.h"  //  for ThreadSafe operations (Ref.Count/Share) 

/*!
   \class SOPHYA::SegDBInterface
   \ingroup BaseTools
   Interface definition for segmented data container (template class).
*/
/*!
   \class SOPHYA::SegDataBlock
   \ingroup BaseTools
   Template class impementing segmented data container in memory with 
   management of reference sharing. 
*/

namespace SOPHYA {

////////////////////////////////////////////////////////////////
//// ---------------- Class SegDBInterface ------------------ //
//// ----------------- Class SegDataBlock ------------------- //
////////////////////////////////////////////////////////////////

template <class T>
class SegDBInterface : public AnyDataObj {
public:
  virtual ~SegDBInterface() {}
  //! Changes the data segment size and reallocates the memory segments
  // segsz: Segment size - should be multiple of nbitem;  nbseg: Number of data segments; 
  // nbitem: number of consecutive of data items  
  virtual void SetSize(size_t segsz, size_t nbseg=0, size_t nbitem=1) = 0;
  //! Alias for SetSize()
  inline void  ReSize(size_t segsz, size_t nbseg=0, size_t nbitem=1) { SetSize(segsz, nbseg, nbitem); }
  //! Adds one segment to the data structure - returns the new number of segments.
  virtual size_t Extend() = 0;
  //! Return the segment size data structure, should be multiple of nbitem
  virtual  size_t SegmentSize() const = 0; 
  //! Return the number of data segments
  virtual size_t NbSegments() const = 0;
  //! Return the number of data items in each group (def=1). SegmentSize() is a multiple of NbItems();
  virtual size_t NbItems() const = 0;
  //! Return the current size of the segmented data structure
  inline size_t Size() const    { return  SegmentSize()*NbSegments(); }
  //! Return the pointer to data segment \b k
  virtual T* GetSegment(size_t k) = 0;   
  //! Return the const (read-only) pointer to data segment \b k      
  virtual T const * GetCstSegment(size_t k) const = 0; 
  //! Define or change the physical unit of the data 
  virtual void SetUnits(const Units& un) = 0;
  //! Return the physical unit of the data. Flag sdone is set to true if SetUnits() has been called.
  virtual Units GetUnits(bool& sdone) const = 0;
  //! Return the physical unit of the data 
  virtual Units GetUnits() const = 0;
  //! Fills the std::vector \b vec with the SegDBInterface object content
  virtual void CopyTo(std::vector<T> & vec)  const 
  {
    vec.resize(Size());
    size_t i=0;
    for(size_t k=0; k<NbSegments(); k++) {
      T const * ptr = GetCstSegment(k);
      for(size_t j=0; j<SegmentSize(); j++) {
	vec[i]=ptr[j]; i++;
      } 
    }
    return;
  }
  /*! 
    \brief Fills the SegDBInterface object content from std::vector \b vec 
    
    The \b vec size should be less or equal to the SegDBInterface object, otherwise an SzMismatchError is generated
  */
  virtual void CopyFrom(std::vector<T> const & vec)  
  {
    if (vec.size() > Size())  throw SzMismatchError("SegDBInterface<T>::CopyFrom(v) v.size() > this->Size()");
    size_t i=0;
    for(size_t k=0; k<NbSegments(); k++) {
      T * ptr = GetSegment(k);
      size_t jmax=SegmentSize();
      if (i+jmax > vec.size())  jmax=vec.size()-i; 
      for(size_t j=0; j<jmax; j++) {
	ptr[j]=vec[i]; i++;
      } 
    }
    return;
  }
};


// classe de container avec partage de reference
template <class T>
class SegDataBlock : public SegDBInterface<T> {

public:

  //! Default constructor - optional specification of segment size and number of segments
  explicit SegDataBlock(size_t segsz=32, size_t nbseg=0, size_t nitem=1)
  {
    mSRef = NULL;
    SetSize(segsz, nbseg, nitem);
  }
  //! copy constructor - shares the data
  SegDataBlock(const SegDataBlock<T>& a)
  {
    a.mSRef->gThsop.lock();   //  (ThreadSafe) - Start of atomic operation 
    mSRef = a.mSRef;
    mSRef->nref++;
    a.mSRef->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation
  }
  //! copy constructor with specification of flags for data sharing and element value copy
  SegDataBlock(const SegDataBlock<T>& a, bool share, bool cpval=true)
  {
    if (share) {
      a.mSRef->gThsop.lock();   //  (ThreadSafe) - Start of atomic operation 
      mSRef = a.mSRef;
      mSRef->nref++;
      a.mSRef->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation
    }
    else {
      mSRef = NULL;
      Clone(a, cpval);
    }
  }
  //! copy constructor - shares the data if \b is a SegDataBlock, clones otherwise
  SegDataBlock(const SegDBInterface<T>& a)
  {
    SegDataBlock<T> * sdb = dynamic_cast< SegDataBlock<T> *>(&a);
    if (sdb != NULL) { 
      sdb->mSRef->gThsop.lock();   //  (ThreadSafe) - Start of atomic operation 
      mSRef = sdb->mSRef;
      mSRef->nref++;
      sdb->mSRefg->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation
    }
    else Clone(a, true);
  }
  //! Destructor. The memory is freed when the last object referencing the data segment is destroyed
  virtual ~SegDataBlock()
  {
    //DBG    cout << " DEBUG-~SegDataBlock() " << hex << mSRef << dec << " NRef()= " << NRef() << endl;
    Delete();
  }


  //! Adds one segment to the data structure - returns the number of allocated segments.
  virtual size_t Extend()
  {
    T * p = new T[mSRef->segsize];
    mSRef->gThsop.lock();   //  (ThreadSafe) - Start of atomic operation 
    mSRef->dseg.push_back(p);
    size_t rs = mSRef->dseg.size();
    mSRef->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation
    return( rs );
  }

  //! Changes the data segment size and reallocates the memory segments
  // segsz : Segment size ;  nbseg : Number of data segments
  virtual void SetSize(size_t segsz, size_t nbseg=0, size_t nitem=1)
  {
    if (segsz%nitem != 0) 
      throw ParmError("SegDataBlock<T>::SetSize() - segsz not a multiple of nitem");
    Delete();
    mSRef = new SDREF;
    mSRef->gThsop.lock();   //  (ThreadSafe) - Start of atomic operation 
    mSRef->nref = 1;
    mSRef->segsize = segsz;
    mSRef->nbitems = nitem;
    mSRef->unit_p = NULL;
    mSRef->dsid = AnyDataObj::getUniqueId();
    for(size_t k=0; k<nbseg; k++) Extend_P();
    mSRef->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation
  }
//! Shares the data between two SegDataBlock objects
  void Share(const SegDataBlock<T>& a)
  {
    Delete();
    a.mSRef->gThsop.lock();   //  (ThreadSafe) - Start of atomic operation 
    mSRef = a.mSRef;
    mSRef->nref++;
    a.mSRef->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation
  }

//! Makes a clone of the data structure and optionaly copie the data 
  void Clone(const SegDBInterface<T> & a, bool cpval=true)
  {
    Delete();
    mSRef = new SDREF;
    mSRef->gThsop.lock();   //  (ThreadSafe) - Start of atomic operation 
    mSRef->nref = 1;
    mSRef->segsize = a.SegmentSize();
    mSRef->nbitems = a.NbItems();
    bool fgud;
    Units un=a.GetUnits(fgud);
    if (fgud) mSRef->unit_p = new Units(un);
    else mSRef->unit_p = NULL;
    mSRef->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation
    for(size_t k=0; k<a.NbSegments(); k++) {
      Extend();
      if (cpval) {
	T * dst = GetSegment(k);
	const T * src = a.GetCstSegment(k);
	PPF_TPointer_IO<T>::Copy(dst, src, mSRef->segsize);
      }
    }   
  }


  //! Return the segment size of the data structure
  virtual size_t SegmentSize() const  { return mSRef->segsize; }
  //! Return the number of data segments
  virtual size_t NbSegments() const  { return mSRef->dseg.size(); }
  //! Return the number of data items in each group (def=1). SegmentSize() is a multiple of NbItems().
  virtual size_t NbItems() const  { return mSRef->nbitems; }
  //! Return the current size of the segmented data structure
  inline size_t Size() const    { return mSRef->segsize*mSRef->dseg.size(); }

  //! Return the pointer to data segment \b k      
  virtual T* GetSegment(size_t k)              { return mSRef->dseg[k]; } 
  //! Return the const (read-only) pointer to data segment \b k      
  virtual T const * GetCstSegment(size_t k) const { return mSRef->dseg[k]; } 

  //! Define or change the physical unit of the data 
  virtual void SetUnits(const Units& un)
  {
    if (!mSRef)  return;
    mSRef->gThsop.lock();   //  (ThreadSafe) - Start of atomic operation 
    if (mSRef->unit_p) *(mSRef->unit_p)=un; 
    else  mSRef->unit_p=new Units(un);
    mSRef->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation    
  }
  //! Return the physical unit of the data. Flag sdone is set to true if SetUnits() has been called.
  virtual Units GetUnits(bool& sdone) const
  {
    if (mSRef && mSRef->unit_p)  { sdone=true;  return *(mSRef->unit_p);  }
    sdone=false;
    return Units();
  }
  //! Return the physical unit of the data. 
  virtual Units GetUnits() const
  {
    if (mSRef && mSRef->unit_p)  return *(mSRef->unit_p);
    else return Units();
  }

  //! Return the segment index for  element \b i
  inline size_t SegIndex(size_t i) {  return i/mSRef->segsize; }
  //! Return the offset (in data segment)  for  element \b i
  inline size_t EltOffset(size_t i) {  return i%mSRef->segsize; }

  //! Return the \b i th element of the segmented data structure   
  inline T& operator()(size_t i)       { return *(mSRef->dseg[SegIndex(i)]+EltOffset(i));}
  //! Return the \b i th element of  the data structure
  inline T& operator()(size_t i) const { return *(mSRef->dseg[SegIndex(i)]+EltOffset(i));}
  //! Return the \b i th element of the segmented data structure   
  inline T& operator[](size_t i)       { return *(mSRef->dseg[SegIndex(i)]+EltOffset(i));}
  //! Return the \b i th element of  the data structure
  inline T& operator[](size_t i) const { return *(mSRef->dseg[SegIndex(i)]+EltOffset(i));}


  //! Return the number of references to the data structure
  inline size_t NRef() const { return mSRef->nref; }

  //! Equal operator. Set all element values to \b v 
  SegDataBlock<T>& operator = (T const & v)
  {
    for(size_t k=0; k<NbSegments(); k++) {
      T * p = mSRef->dseg[k];
      for(size_t j=0; j<SegmentSize(); j++) p[j] = v;
    }
    return (*this);
  }
  //! Equal operator. Clones and copie values from \b a
  inline SegDataBlock<T>& operator = (const SegDBInterface<T> & a)
  {
    Clone(a, true);
    return (*this);
  }
  //! Equal operator. Clones and copie values from \b a
  inline SegDataBlock<T>& operator = (const SegDataBlock<T> & a)
  {
    Clone(a, true);
    return (*this);
  }

  //! ASCII formatted output (print)
  ostream& Print(ostream& os, int lev=0, const char * sep=NULL) const
  {
    os << "SegDataBlock< " << typeid(T).name() << "> mSRef= " << hex << mSRef
       << " NRef=" << dec << NRef() << " DSId= " <<  DRefId() << endl;
    os << " ... SegSize= " << SegmentSize() << " NbSeg= " << NbSegments() 
       << " NbItems=" << NbItems() << " Size= " << Size();
    bool fgud;
    Units un=GetUnits(fgud);
    if (fgud) os << " Units: " << un.Name() << endl;
    else os << endl;
    if (sep == NULL) sep = "  ";
    if (lev > 0) {  
      for(size_t k=0; k<NbSegments(); k++) {
	T * p = mSRef->dseg[k];
	os << " ..... DataSeg[ " << k << " ] : " << hex << p << dec << endl;
	if (lev > 1) 
	  for(size_t j=0; j<SegmentSize(); j++)  os << p[j] << sep;
	os << endl;     
      }
    }
    return os;
  }
  //! ASCII formatted output (print) on cout
  inline ostream& Print(int lev=0, const char * sep=NULL) const
  {
    return Print(cout, lev, sep);
  }

  //! Returns the unique object identifier 
  inline uint_8 DRefId() const { return ((mSRef)?mSRef->dsid:0); }

protected:
  //! NON-thread safe: Decrement the number of reference counts, and free the memory if NRef=0
  void Delete() 
  {
    if (mSRef == NULL) return;
    mSRef->gThsop.lock();
    mSRef->nref--;
    if (mSRef->nref > 0)  { 
     mSRef->gThsop.unlock();
     mSRef = NULL; 
      return; 
    }
    //DBG    cout << " DEBUG-SegDataBlock::Delete() NbSegments() = " << dec << NbSegments() << endl;
    for(size_t k=0; k<NbSegments(); k++) {
      //DBG  cout << " DEBUG-SegDataBlock::Delete() Segment["<< k <<"] = " << hex << mSRef->dseg[k] << endl;
      delete[] mSRef->dseg[k];
      mSRef->dseg[k] = NULL;
    }
    if (mSRef->unit_p)  { 
      //DBG cout << " DEBUG-SegDataBlock::Delete() unit_p= " << hex << mSRef->unit_p << endl;
      delete mSRef->unit_p;      

    }
    mSRef->gThsop.unlock();
    delete mSRef;
    mSRef = NULL;
  }
  //! NON-thread safe, version of Extend() : Adds one segment to the data structure 
  size_t Extend_P()
  {
    T * p = new T[mSRef->segsize];
    mSRef->dseg.push_back(p);
    return( mSRef->dseg.size());
  }

  /*! \cond 
    SDREF structure for reference management - for internal use by SegDataBlock
  */
  typedef struct {
    size_t nref;      // Number of references to the data structure
    uint_8 dsid;      // Data structure Id - Used by FIO_SegDataBlock
    std::vector<T *> dseg;
    size_t segsize;   // data segment size - is a multiple of nbitems 
    size_t nbitems;   // number of data items in each group
    Units* unit_p;    // Optional physical unit of the data 
    ThSafeOp gThsop;  // Mutex for thread safe operation 
  } SDREF;
  /*! \endcond */
  SDREF *  mSRef;    //!< SDREF structure for reference sharing

};


//! Definition of operator \<\< for ascii formatted output of SegDataBlock
template<class T> 
inline ostream& operator << (ostream& os, const SegDataBlock<T>& a)
  {  return a.Print(os); }

} // Fin du namespace

#endif
