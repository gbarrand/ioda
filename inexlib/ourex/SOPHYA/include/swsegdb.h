// This may look like C code, but it is really -*- C++ -*-
// Gestion de block de donnees swapable
//                         R. Ansari  Mars 2005
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA
#ifndef SWSEGDATABLOCK_H
#define SWSEGDATABLOCK_H

#include "machdefs.h"
#include "segdatablock.h"
#include "pexceptions.h"
#include <vector>
#include <typeinfo>

/*!
  \class SOPHYA::DataSwapperInterface
  \ingroup BaseTools
  Interface definition for  data swapper (pure virtual) classes to be used 
  with SOPHYA::SwSegDataBlock classes.
*/
/*!
  \class SOPHYA::SwSegDataBlock
  \ingroup BaseTools
  Segmented data structure with swap space management.
*/

namespace SOPHYA {

////////////////////////////////////////////////////////////////
//// ------------- Class DataSwapperInterface --------------- //
//// ---------------- Class SwSegDataBlock ------------------ //
////////////////////////////////////////////////////////////////

template <class T> 
class DataSwapperInterface {
public:
  DataSwapperInterface() : mode_(ios::in|ios::out) { }
  virtual ~DataSwapperInterface() { } 
  /*! Swap out the data array pointed by \b d with size \b sz
    Return the swap position which might be used later to retrieve the data from swap
    \param d : Pointer to the memory segment
    \param sz : Number of elements (type T) 
    \param idx : An integer which might be used to identify the data (optional)
    \param oswp : Old swap position, if the data has already been swapped
    \param osw : true -> data has already been swapped
  */
  virtual int_8 WriteToSwap(const T * d, size_t sz, int_8 idx, int_8 oswp=0, bool osw=false) = 0;
  /*! Swap in the data array pointed by \b d with size \b sz
    Retrieves the data from swap space and copies it to \b d
    \param idx : optional data identifier
    \param swp : swap position (obtained from a previous call to WriteToSwap()
    \param d : pointer to T , where the data will be copied from swap space
    \param sz : Number of data elements (type T) 
   */
  virtual void  ReadFromSwap(int_8 idx, int_8 swp, T* d, size_t sz) = 0;

  /*! Duplicate the swapper object and return the new object pointer.
      The returned pointer should be deleted when not needed any more.
      This method is used by SwSegDataBlock<T> 
  */
  virtual DataSwapperInterface<T>* Clone() = 0;
  //! Return the supported swap mode ios::in , ios::out , ios::in|ios::out
  virtual int getSwapMode()  { return mode_; }
protected:
  int mode_;
};

template <class T>
class SwSegDataBlock : public SegDBInterface<T> {
public:
  /*! \brief Default constructor 
    \warning SetSize(), SetSwapper() should be called after. SetSwapPosTagTable() might be called
  */
  SwSegDataBlock()
  {
    mSRef = NULL;  buff = NULL;
    bidx = -1;    fgcstbuff = true; 
  }
  //! Constructor - creation from swap position tags (values)
  SwSegDataBlock(DataSwapperInterface<T> & dsw, vector<int_8> const & swpos, size_t segsz, size_t nitem=1)
  {
    mSRef = NULL;
    SetSize(segsz, swpos.size(), nitem);
    SetSwapper(dsw);
    mSRef->gThsop.lock();   //  (ThreadSafe) - Start of atomic operation 
    mSRef->swp = swpos;
    for(size_t k=0; k<mSRef->fgwp.size(); k++)  mSRef->fgwp[k] = true; 
    mSRef->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation
    }
  //! Constructor - optional specification of segment size and number of segments
  SwSegDataBlock(DataSwapperInterface<T> & dsw, size_t segsz=32, size_t nbseg=0, size_t nitem=1)
  {
    mSRef = NULL;
    SetSize(segsz, nbseg, nitem);
    SetSwapper(dsw);
  }
  //! copy constructor - shares the data
  SwSegDataBlock(const SwSegDataBlock<T>& a)
  {
    a.mSRef->gThsop.lock();   //  (ThreadSafe) - Start of atomic operation 
    mSRef = a.mSRef;
    mSRef->nref++;
    size_t segsz = mSRef->segsize;
    a.mSRef->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation
    buff = new T[segsz];
    bidx = -1;        
    fgcstbuff = true; 
  }

  //! Destructor. The memory is freed when the last object referencing the data segment is destroyed
  virtual ~SwSegDataBlock() 
  { 
    Delete(); 
  }
  //! Adds one segment to the data structure - returns the pointer to the allocated segment.
  virtual size_t Extend() 
  {
    size_t rs = 0;
    mSRef->gThsop.lock();   //  (ThreadSafe) - Start of atomic operation 
    mSRef->swp.push_back(0);
    mSRef->fgwp.push_back(false);
    rs = mSRef->swp.size();
    mSRef->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation
    return rs;
  }
  /*! \brief Changes the data segment size and reallocates the memory segments

    \warning SetSwapper() and SetSwapPosTagTable() should be called as necessary 
    be called if an existing SwSegDataBlock<T> object is being initialized. 
  */
  // segsz : Segment size ;  nbseg : Number of data segments
  virtual void SetSize(size_t segsz, size_t nbseg=0, size_t nitem=1)
  {
    if (segsz%nitem != 0) 
      throw ParmError("SwSegDataBlock<T>::SetSize() - segsz not a multiple of nitem");
    DataSwapperInterface<T> * dswp=NULL;
    if (mSRef)  dswp = mSRef->swapper->Clone();
    DataSwapperInterface<T> * rswp = Delete(true);
    mSRef = new SWSDREF;
    mSRef->gThsop.lock();   //  (ThreadSafe) - Start of atomic operation 
    mSRef->nref = 1;
    mSRef->segsize = segsz;
    mSRef->nbitems = nitem;
    mSRef->unit_p = NULL;
    mSRef->dsid = AnyDataObj::getUniqueId();
    for(size_t k=0; k<nbseg; k++) {
      mSRef->swp.push_back(0);
      mSRef->fgwp.push_back(false);
    }
    mSRef->swapper = rswp;
    mSRef->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation
    buff = new T[segsz];
    bidx = -1;        
    fgcstbuff = true; 
  }

  //! Define the data swapper object. Should only be called if SetSize() is called
  void SetSwapper(DataSwapperInterface<T> & dsw)
  {
    if (mSRef == NULL) return;
    mSRef->gThsop.lock();   //  (ThreadSafe) - Start of atomic operation 
    if (mSRef->swapper) delete mSRef->swapper;  
    mSRef->swapper = dsw.Clone();
    mSRef->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation
  }

  /*! \brief Defines the vector of swap positions 
    \warning the vector size should be equal to NbSegments()
  */
  void SetSwapPosTagTable(const std::vector< int_8> & postag)
  {
    if (postag.size()!=mSRef->swp.size()) 
      throw SzMismatchError("SwSegDataBlock::SetSwapPosTagTable(postag) postag.size() != NbSegments() ");
    mSRef->gThsop.lock();   //  (ThreadSafe) - Start of atomic operation 
    mSRef->swp=postag;
    for(size_t k=0; k<mSRef->fgwp.size(); k++)  mSRef->fgwp[k] = true; 
    mSRef->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation
  }

  //! Return the segment size data structure
  virtual  size_t SegmentSize() const  { return mSRef->segsize; }
  //! Return the number of data segments
  virtual size_t NbSegments() const { return mSRef->swp.size(); } ;
  //! Return the number of data items in each group (def=1). SegmentSize() is a multiple of NbItems().
  virtual size_t NbItems() const  { return mSRef->nbitems; }
  //! Return the current size of the segmented data structure
  inline size_t Size() const    { return  mSRef->swp.size()*mSRef->segsize; }
  //! Return the pointer to data segment \b k
  virtual T* GetSegment(size_t k) 
  {
    getSeg(k);
    fgcstbuff = false;
    return buff;
  }
  //! Return the const (read-only) pointer to data segment \b k         
  virtual T const * GetCstSegment(size_t k) const 
  {
    if (getSeg(k))  fgcstbuff = true;
    return buff;    
  }

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

  //! Equal operator. Shares the data with \b a
  inline SwSegDataBlock<T>&  operator = (const SwSegDataBlock<T>& a)
  {
    Delete();
    a.mSRef->gThsop.lock();   //  (ThreadSafe) - Start of atomic operation 
    mSRef = a.mSRef;
    mSRef->nref++;
    size_t segsz = mSRef->segsize;
    a.mSRef->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation
    buff = new T[segsz];
    bidx = -1;        
    fgcstbuff = true; 
    return *this;
  }

  //! Empties all memory buffers to swap stream 
  void SwapOutBuffer() const
  {
    mSRef->gThsop.lock();   //  (ThreadSafe) - Start of atomic operation 
    if ((bidx >= 0) && !fgcstbuff) {
      int_8 nswp = mSRef->swapper->WriteToSwap(buff, mSRef->segsize, bidx, 
					       mSRef->swp[bidx], mSRef->fgwp[bidx]);
      mSRef->swp[bidx] = nswp;
      mSRef->fgwp[bidx] = true;
      bidx = -1;
      fgcstbuff = true;  
   }
    mSRef->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation
  }
  //! Return the position tag (swap position) table
  std::vector< int_8 > & GetSwapPosTagTable() const  {  return  mSRef->swp; } 
  //! Returns the unique object identifier 
  inline uint_8 DRefId() const { return ((mSRef)?mSRef->dsid:0); }
  //! Return pointer to the swapper object 
  DataSwapperInterface<T> * GetSwapper() 
  {
    if (mSRef) return mSRef->swapper;
    else return NULL;
  } 
protected: 
  DataSwapperInterface<T> * Delete(bool fgc=false)   
  {
    DataSwapperInterface<T> * rswp = NULL;
    if (mSRef == NULL) return rswp;
    mSRef->gThsop.lock();   //  (ThreadSafe) - Start of atomic operation 
    if (fgc && mSRef->swapper) rswp = mSRef->swapper->Clone();
    mSRef->nref--;
    if (mSRef->nref > 0)  { 
      mSRef->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation
      delete[] buff;
      mSRef = NULL; 
      return rswp; 
    }
    if (mSRef->swapper) delete mSRef->swapper;  
    if (mSRef->unit_p)  delete mSRef->unit_p;
    mSRef->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation
    delete mSRef;
    delete[] buff;
    mSRef = NULL;
    return rswp;
  } 
  bool getSeg(size_t k) const
  {
    if ((bidx >= 0) && (k == bidx))  return false;
    if (k >= NbSegments()) 
      throw RangeCheckError("SwSegDataBlock<T>::getSeg(k) k>=NbSegments()");
    mSRef->gThsop.lock();   //  (ThreadSafe) - Start of atomic operation 
    if ((bidx >= 0) && !fgcstbuff) {
      int_8 nswp = mSRef->swapper->WriteToSwap(buff, mSRef->segsize, bidx, 
					       mSRef->swp[bidx], mSRef->fgwp[bidx]);
      mSRef->swp[bidx] = nswp;
      mSRef->fgwp[bidx] = true;
    }
    if (mSRef->fgwp[k]) {
      //DBG cout << "SwSegDataBlock<T>::getSeg(k)*DBG ReadFromSwap(k="<<k<<" pos="<<mSRef->swp[k]<<endl;
      mSRef->swapper->ReadFromSwap(k, mSRef->swp[k], buff, mSRef->segsize);
    }
    //DEL-02022007    else { delete[] mSRef->buff;  mSRef->buff = new T[mSRef->segsize]; }  
    mSRef->gThsop.unlock();   //  (ThreadSafe) - End of atomic operation
    bidx = k;
    return true;
  }

  //! \cond
  typedef struct {
    size_t nref;      // Number of references to the data structure  
    uint_8 dsid;      // Data structure id
    size_t segsize;   // data segment size - is a multiple of nbitems 
    size_t nbitems;   // number of data items in each group
    Units* unit_p;    // Optional physical unit of the data 
    mutable std::vector< int_8 > swp;  // swap position tag for each segment
    mutable std::vector< bool > fgwp;  // swap flag (true = already swapped)  for each segment
    DataSwapperInterface<T> * swapper;   // Data swapper
    ThSafeOp gThsop;         // Mutex for thread safe operation one / SWSDREF struct 
  } SWSDREF;
  //! \endcond
  SWSDREF *  mSRef;    //!< SWSDREF structure for reference sharing
  mutable T * buff;                // Data buffer   
  mutable int_8 bidx;              // segment index (number) corresponding to buffer 
  mutable bool fgcstbuff;          // true : this is a constant T * buff

};


} // Fin du namespace

#endif
