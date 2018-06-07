// This may look like C code, but it is really -*- C++ -*-
// Class DataTable  (Row-Column data table in memory) 
//   R. Ansari -  Avril 2005
//   (C)  LAL-IN2P3/CNRS    CEA-DAPNIA  

#ifndef SWPPFDTABLE_H_SEEN
#define SWPPFDTABLE_H_SEEN

#include "basedtable.h"
#include "swppfsegdb.h"
#include "objfio.h"


namespace SOPHYA {

//! An implementation of BaseDataTable with using a PPF stream as swap space.
class SwPPFDataTable : public BaseDataTable {
public:
  SwPPFDataTable(size_t segsz=512);
  SwPPFDataTable(POutPersist & os, size_t segsz=512); 
  SwPPFDataTable(PPFDataSwapper<int_4>& swp, size_t segsz=512); 
  SwPPFDataTable(SwPPFDataTable const& a); 

  virtual ~SwPPFDataTable();

  virtual size_t AddColumn(FieldType ft, string const & cnom, size_t vsz=1, bool fgfcnet=false);

  //! Equal (copy) operator - Copies the data and the structure if necessary from \b a   
  inline  SwPPFDataTable& operator = (BaseDataTable const& a) 
  { CopyMerge(a, true) ; return *this ; } 
  //! Equal (copy) operator - Copies the structure and shares the data \b a   
  inline SwPPFDataTable&  operator = (SwPPFDataTable const& a) 
  { Clear(); Share(a); return *this ; } 

  //  Pour la gestion de persistance PPF
  friend class ObjFileIO<BaseDataTable> ;
  
  //! Reset(Clear) the table content and structure
  virtual void Clear();
  /*! This method should be called in order to write the data buffers to swap space, 
     before saving object to PPF stream 
  */
  void SwapOutAll() const;
protected:
  SwPPFDataTable(PInPersist & is, size_t segsz); 
  void Share(SwPPFDataTable const & a);

  PPFDataSwapper<int_4> mSwap_;  // Juste pour gerer l'objet swapper et les stream POutPersist / PInPersist 
  // Donnees des colonnes 
  std::vector< SwPPFSegDataBlock<int_4> > mICols; 
  PPFDataSwapper<int_4> mISwapper;
  std::vector< SwPPFSegDataBlock<int_8> > mLCols; 
  PPFDataSwapper<int_8> mLSwapper;
  std::vector< SwPPFSegDataBlock<r_4> > mFCols;
  PPFDataSwapper<r_4> mFSwapper;
  std::vector< SwPPFSegDataBlock<r_8> > mDCols; 
  PPFDataSwapper<r_8> mDSwapper;
  std::vector< SwPPFSegDataBlock< complex<r_4> > > mYCols;
  PPFDataSwapper< complex<r_4> > mYSwapper;
  std::vector< SwPPFSegDataBlock< complex<r_8> > > mZCols;
  PPFDataSwapper< complex<r_8> > mZSwapper;
  std::vector< SwPPFSegDataBlock< std::string > > mSCols;    
  PPFDataSwapper< std::string > mSSwapper;
  std::vector< SwPPFSegDataBlock< TimeStamp > > mTCols;    
  PPFDataSwapper< TimeStamp > mTSwapper;
  std::vector< SwPPFSegDataBlock< String16 > > mS16Cols;    
  PPFDataSwapper< String16 > mS16Swapper;
  std::vector< SwPPFSegDataBlock< String64 > > mS64Cols;    
  PPFDataSwapper< String64 > mS64Swapper;
  /*
  // Output swap stream doit etre cree avant l'appel au constructeur
  // et rester valide (non detruit) tant que l'objet SwPPFDataTable existe
  */

}; 
  
/*! Writes the object in the POutPersist stream \b os */
inline POutPersist& operator << (POutPersist& os, SwPPFDataTable & obj)
{ obj.SwapOutAll();  ObjFileIO<BaseDataTable> fio(&obj);  fio.Write(os);  return(os); }
/*! Reads the object from the PInPersist stream \b is */
inline PInPersist& operator >> (PInPersist& is, SwPPFDataTable & obj)
{  ObjFileIO<BaseDataTable> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }

} // namespace SOPHYA

#endif

