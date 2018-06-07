/*    
   --- SOPHYA software - FitsIOServer module ---
   R. Ansari , 2005-2008 
   (C) UPS+LAL IN2P3/CNRS     (C) DAPNIA-SPP/CEA 
*/

#ifndef SWFITSDTABLE_H_SEEN
#define SWFITSDTABLE_H_SEEN

#include "basedtable.h"
#include "swsegdb.h"
#include "fitsswapper.h"

#include "fitshandler.h"

namespace SOPHYA {

//! An implementation of BaseDataTable  using FITS files as swap space.
class SwFitsDataTable : public BaseDataTable {
public:
  SwFitsDataTable(size_t segsz=512);
  SwFitsDataTable(FitsInOutFile & os, size_t segsz=512, bool fgcreate=true); 
  SwFitsDataTable(string fitsname, int hdunum=2, size_t segsz=512); 
  SwFitsDataTable(SwFitsDataTable const& a); 

  virtual ~SwFitsDataTable();

  virtual size_t AddColumn(FieldType ft, string const & cnom, size_t sz=1, bool fgfcnet=false);

  virtual size_t WriteTableDefinitionToFits();

  // Filling data structures (adding lines)
  virtual size_t AddRow(const r_8* data);
  virtual size_t AddRow(const MuTyV * data);
  virtual size_t AddRow(DataTableRow const& data);
  virtual  DataTableRowPtr&   NextRowPtr(DataTableRowPtr& rowp);

   //! Equal (copy) operator - Copies the data and the structure from \b a   
  inline  SwFitsDataTable& operator = (BaseDataTable const& a) 
  { CopyMerge(a, true) ; return *this ; } 
 //! Equal (copy) operator - Copies the structure and shares the data \b a   
  inline SwFitsDataTable& operator = (SwFitsDataTable const& a) 
  { Clear(); Share(a); return *this ; } 

  //! Acces the FitsInOutFile swap stream
  inline FitsInOutFile& FitsSwapFile() { return mSwF; } 
  
  //! Reset(Clear) the table content and structure
  virtual void Clear();
  /*! This method should be called in order to empty the swapout buffer, 
     before saving object to PPF stream 
  */
  void SwapOutAll() const;

  //  Pour la gestion I/O FITS
  friend class FitsHandler<BaseDataTable> ; 

protected:
  void Share(SwFitsDataTable const & a);
  // Methode pour utilisation par FitsHandler<BaseDataTable>
  size_t AddColRd(FieldType ft, string const & cnom, int colidx=-1, 
		  vector<int_8> const * swpos=NULL, size_t sz=1, bool fgfcnet=false);


  // Donnees (SegDataBlock) et leurs swapper
  std::vector< SwSegDataBlock<int_4> > mICols; 
  std::vector< SwSegDataBlock<int_8> > mLCols; 
  std::vector< SwSegDataBlock<r_4> > mFCols;
  std::vector< SwSegDataBlock<r_8> > mDCols; 
  std::vector< SwSegDataBlock< complex<r_4> > > mYCols;
  std::vector< SwSegDataBlock< complex<r_8> > > mZCols;
  std::vector< SwSegDataBlock<std::string> > mSCols;    
  std::vector< SwSegDataBlock<TimeStamp> > mTCols;    
  std::vector< SwSegDataBlock<String16> > mS16Cols;    
  std::vector< SwSegDataBlock<String64> > mS64Cols;    

  // In/Out swap file doit etre cree avant l'appel au constructeur
  // et rester valide (non detruit) tant que l'objet SwFitsDataTable existe
  FitsInOutFile mSwF;   
  bool mFgCreate; // true -> creation de table, false -> lecture
  bool mFgDefDone; // true -> Definition de bin-table effectuee 
}; 

  
} // namespace SOPHYA

#endif

