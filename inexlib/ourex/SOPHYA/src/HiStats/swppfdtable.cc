#include "swppfdtable.h"
#include "ppfpio_fmls.h"
#include "pexceptions.h"
#include "thsafeop.h"

namespace SOPHYA { 

/*!
   \class SwPPFDataTable
   \ingroup HiStats
   This class can be used to organize data in table (row-column) form.
   Each column holds homogeneous data (same data type), while different
   columns can be used for different data types (integer, float, string ...). 
   A PPF stream is used as swap space. 

   \warning 
   - When creating a table, the output PPF stream (POutPersist) must not be closed
   (destroyed) before the SwPPFDataTable object is written to the stream.
   - It is not possible to make a complete (deep) copy of a SwPPFDataTable
   Copy constructor and equal operator shares the data.
   - The destructor  DOES NOT save the table object itself to the swap 
   stream. The << operator should be used on the output PPF stream being 
   used as swap in order to save the table. 
   - If the same SwPPFDataTable object is used in several parallel threads,
   do NOT use the methods which operate on DataTableRowPtr object

   \sa SOPHYA::MuTyV
   \sa SOPHYA::DataTableRow
   \sa SOPHYA::DataTableRowPtr
   \sa SOPHYA::DataTable
   \sa SOPHYA::SwSegDataBlock  SOPHYA::PPFDataSwapper

   \code
   #include "swppfdtable.h"
   // ...
   {
   // ---- Creation of the table
   // Create the swap stream 
   POutPersist so("myswtable.ppf");
   SwPPFDataTable dt(so, 64);
   // define table columns
   dt.AddFloatColumn("X0_f");
   dt.AddFloatColumn("X1_f");
   dt.AddDoubleColumn("X0X0pX1X1_d");
   // Fill the table
   r_8 x[5];
   for(int i=0; i<63; i++) {
     x[0] = (i%9)-4.;  x[1] = (i/9)-3.;  x[2] = x[0]*x[0]+x[1]*x[1];
     dt.AddRow(x);
   }
   // Printing table info
   cout << dt ;
   // Swap out all data and write the table structure to the PPF stream 
   so << dt ;
   // ....
   }
   {
   // ---- Accessing information from a previously created table
   SwPPFDataTable dt;
   PInPersist si("myswtable.ppf");
   si >> dt;
   // Printing table info
   cout << dt ;   
   }
   \endcode

   If one wants to have read operations (access to table data) while the table 
   is being filled, the PPF swapper should be created in read/write mode. 
   The example below shows how create a SwPPFDataTable in read/write mode
   \code 
   // Create the swap stream 
   POutPersist so("myswtable.ppf");
   // Open the swap stream for reading 
   PInPersist si("myswtable.ppf", false);
   // Create a read/write swapper 
   PPFDataSwapper< int_4 > swapper(so,si);
   SwPPFDataTable dt(swapper, 64);
//  start to fill the table 
    ...
//  Computing the min/max implies reading the table content 
    dt.SetShowMinMaxFlag(true);
    cout << dt;
   \endcode   
   implementation of PPF streams, read operations (acces to table data) cannot
   be performed when a table is being filled. 
*/
//! Default constructor with optional specification of block (or segment) size 
SwPPFDataTable::SwPPFDataTable(size_t segsz)
  : BaseDataTable(segsz)
    //    mSwOut(NULL), mSwIn(NULL),
{
}
/*! Constructor with the specification of the output swap stream -
    and optional specification of block (or segment) size
*/
SwPPFDataTable::SwPPFDataTable(POutPersist & os, size_t segsz)
  : BaseDataTable(segsz) ,
    mSwap_(os), 
    mISwapper(os), mLSwapper(os), mFSwapper(os), mDSwapper(os), 
    mYSwapper(os), mZSwapper(os), mSSwapper(os), mTSwapper(os),
    mS16Swapper(os), mS64Swapper(os)
{
}
/*! Constructor with the specification of a swapper object which manages 
    the POutPersist and PInPersist streams
*/
SwPPFDataTable::SwPPFDataTable(PPFDataSwapper<int_4>& swp, size_t segsz)
  : BaseDataTable(segsz) ,
    mSwap_(swp), 
    mISwapper(swp.OutStream(), swp.InStream()), mLSwapper(swp.OutStream(), swp.InStream()), 
    mFSwapper(swp.OutStream(), swp.InStream()), mDSwapper(swp.OutStream(), swp.InStream()), 
    mYSwapper(swp.OutStream(), swp.InStream()), mZSwapper(swp.OutStream(), swp.InStream()), 
    mSSwapper(swp.OutStream(), swp.InStream()), mTSwapper(swp.OutStream(), swp.InStream()),
    mS16Swapper(swp.OutStream(), swp.InStream()), mS64Swapper(swp.OutStream(), swp.InStream())
{
}

//! Protected constructor for creation from a swap stream 
SwPPFDataTable::SwPPFDataTable(PInPersist & is, size_t segsz)
  : BaseDataTable(segsz) ,
    mSwap_(is), 
    mISwapper(is), mLSwapper(is), mFSwapper(is), mDSwapper(is), 
    mYSwapper(is), mZSwapper(is), mSSwapper(is), mTSwapper(is), 
    mS16Swapper(is), mS64Swapper(is)
{
}

//! copy constructor - shares the data (and copies the thread safety state)

SwPPFDataTable::SwPPFDataTable(SwPPFDataTable const & a)
  : BaseDataTable(a.SegmentSize())
{
  Share(a);
}
//! Destructor  
SwPPFDataTable::~SwPPFDataTable()
{
}

void SwPPFDataTable::Share(SwPPFDataTable const & a)
{
  // Recopie des swappers  et les streams associes
  mSwap_ = a.mSwap_;
  mISwapper = a.mISwapper;
  mLSwapper = a.mLSwapper;
  mFSwapper = a.mFSwapper;
  mDSwapper = a.mDSwapper;
  mYSwapper = a.mYSwapper;
  mZSwapper = a.mZSwapper;
  mSSwapper = a.mSSwapper;
  mTSwapper = a.mTSwapper;
  mS16Swapper = a.mS16Swapper;
  mS64Swapper = a.mS64Swapper;
  
  // On recopie la taille de segment
  mSegSz = a.SegmentSize();
  if (a.NVar() == 0)  return;  // Table sans colonne 

  // On copie la structure de table 
  CopyStructure(a);
  if (a.IsThreadSafe())  SetThreadSafe(true);
  else SetThreadSafe(false);

  //
  // Update nombre d'entree, ...
  mNEnt = a.mNEnt;
  mNSeg = a.mNSeg;
  if (a.mInfo)  mInfo = new DVList(*(a.mInfo));

  // mis a jour des tableax min-max 
  mMin = a.mMin;
  mMax = a.mMax;
  mMinMaxNEnt = a.mMinMaxNEnt;

  // Et on partage les donnees des colonnes 
  for (size_t kk=0; kk<mNames.size(); kk++) {
    size_t sk = mNames[kk].ser;
    size_t ska = a.mNames[kk].ser;
    switch (mNames[kk].type) {
    case IntegerField :
      mICols[sk] = a.mICols[ska];
    break;
    case LongField :
      mLCols[sk] = a.mLCols[ska]; 
      break;
    case FloatField :
      mFCols[sk] = a.mFCols[ska]; 
      break;
    case DoubleField :
      mDCols[sk] = a.mDCols[ska]; 
      break;
    case ComplexField :
      mYCols[sk] = a.mYCols[ska]; 
      break;
    case DoubleComplexField :
      mZCols[sk] = a.mZCols[ska]; 
      break;
    case StringField :
      mSCols[sk] = a.mSCols[ska]; 
      break;
    case DateTimeField :
      mTCols[sk] = a.mTCols[ska]; 
      break;
    case FMLStr16Field :
      mS16Cols[sk] = a.mS16Cols[ska]; 
      break;
    case FMLStr64Field :
      mS64Cols[sk] = a.mS64Cols[ska]; 
      break;
    default:
      throw ForbiddenError("SwPPFDataTable::Share() : unknown column type ");
    break;
    }
  }  
}
void SwPPFDataTable::SwapOutAll() const
{
  // Et on vide les buffers de swap 
  for (size_t kk=0; kk<mNames.size(); kk++) {
    size_t sk = mNames[kk].ser;
    switch (mNames[kk].type) {
    case IntegerField :
      mICols[sk].SwapOutBuffer();
    break;
    case LongField :
      mLCols[sk].SwapOutBuffer();
      break;
    case FloatField :
      mFCols[sk].SwapOutBuffer();
      break;
    case DoubleField :
      mDCols[sk].SwapOutBuffer();
      break;
    case ComplexField :
      mYCols[sk].SwapOutBuffer();
      break;
    case DoubleComplexField :
      mZCols[sk].SwapOutBuffer();
      break;
    case StringField :
      mSCols[sk].SwapOutBuffer();
      break;
    case DateTimeField :
      mTCols[sk].SwapOutBuffer();
      break;
    case FMLStr16Field :
      mS16Cols[sk].SwapOutBuffer();
      break;
    case FMLStr64Field :
      mS64Cols[sk].SwapOutBuffer();
      break;
    default:
      throw ForbiddenError("SwPPFDataTable::Share() : unknown column type ");
    break;
    }
  }  
}

void SwPPFDataTable::Clear()
{
  if ( (NVar() == 0) && (NEntry() == 0)) return;
  ClearP();
  mICols.clear();
  mLCols.clear();
  mFCols.clear();
  mDCols.clear();
  mYCols.clear();
  mZCols.clear();
  mSCols.clear();
  mTCols.clear();
  mS16Cols.clear();
  mS64Cols.clear();
}


/*! 
  Implements the action defined in the BaseDataTable interface.
  if (fgfcnet==true) : force new column addition even if the table is not empty.
*/
  size_t SwPPFDataTable::AddColumn(FieldType ft, string const & cnom, size_t vsz, bool fgfcnet)
{
  if (!fgfcnet && (NEntry() > 0)) 
    throw ParmError("SwPPFDataTable::AddColumn() Table contains already data ");
  CheckColName(cnom);
  if (vsz < 1) vsz=1;
  size_t ser; 
  size_t idx = NVar(); 
  size_t segsz = mSegSz*vsz;
  /* ATTENTION: 
   Dans ce code qui suit, la reinitilisation complete de mXColsP  (X=I.L,F,D,Y,Z,S ...) 
   est imperatif. En effet, le push_back() ds mXCols  provoque des copies des objets 
   SwSegDataBlock contenu deja ds mXCols et le changement des pointeurs correspondant...
  */
  switch (ft) {
  case IntegerField :
    ser = mICols.size();
    mICols.push_back(SwPPFSegDataBlock<int_4>(mISwapper, segsz, 0, vsz));
    mIColIdx.push_back(idx);
    mIColsP.push_back(NULL);
    for(size_t kk=0; kk<mICols.size(); kk++)
      mIColsP[kk] = &(mICols[kk]);
    break;
  case LongField :
    ser = mLCols.size();
    mLCols.push_back(SwPPFSegDataBlock<int_8>(mLSwapper, segsz, 0, vsz));
    mLColIdx.push_back(idx);
    mLColsP.push_back(NULL);
    for(size_t kk=0; kk<mLCols.size(); kk++)
      mLColsP[kk] = &(mLCols[kk]);
    break;
  case FloatField :
    ser = mFCols.size();
    mFCols.push_back(SwPPFSegDataBlock<r_4>(mFSwapper, segsz, 0, vsz));
    mFColIdx.push_back(idx);
    mFColsP.push_back(NULL);
    for(size_t kk=0; kk<mFCols.size(); kk++)
      mFColsP[kk] = &(mFCols[kk]);
    break;
  case DoubleField :
    ser = mDCols.size();
    mDCols.push_back(SwPPFSegDataBlock<r_8>(mDSwapper, segsz, 0, vsz));
    mDColIdx.push_back(idx);
    mDColsP.push_back(NULL);
    for(size_t kk=0; kk<mDCols.size(); kk++)
      mDColsP[kk] = &(mDCols[kk]);
    break;
  case ComplexField :
    ser = mYCols.size();
    mYCols.push_back(SwPPFSegDataBlock< complex<r_4> >(mYSwapper, segsz, 0, vsz));
    mYColIdx.push_back(idx);
    mYColsP.push_back(NULL);
    for(size_t kk=0; kk<mYCols.size(); kk++)
      mYColsP[kk] = &(mYCols[kk]);
    break;
  case DoubleComplexField :
    ser = mZCols.size();
    mZCols.push_back(SwPPFSegDataBlock< complex<r_8> >(mZSwapper, segsz, 0, vsz));
    mZColIdx.push_back(idx);
    mZColsP.push_back(NULL);
    for(size_t kk=0; kk<mZCols.size(); kk++)
      mZColsP[kk] = &(mZCols[kk]);
    break;
  case StringField :
    ser = mSCols.size();
    mSCols.push_back(SwPPFSegDataBlock<string>(mSSwapper, segsz, 0, vsz));
    mSColIdx.push_back(idx);
    mSColsP.push_back(NULL);
    for(size_t kk=0; kk<mSCols.size(); kk++)
      mSColsP[kk] = &(mSCols[kk]);
    break;
  case DateTimeField :
    ser = mTCols.size();
    mTCols.push_back(SwPPFSegDataBlock<TimeStamp>(mTSwapper, segsz, 0, vsz));
    mTColIdx.push_back(idx);
    mTColsP.push_back(NULL);
    for(size_t kk=0; kk<mTCols.size(); kk++)
      mTColsP[kk] = &(mTCols[kk]);
    break;
  case FMLStr16Field :
    ser = mS16Cols.size();
    mS16Cols.push_back(SwPPFSegDataBlock< String16 >(mS16Swapper, segsz,0,vsz));
    mS16ColIdx.push_back(idx);
    mS16ColsP.push_back(NULL);
    for(size_t kk=0; kk<mS16Cols.size(); kk++)
      mS16ColsP[kk] = &(mS16Cols[kk]);
    break;
  case FMLStr64Field :
    ser = mS64Cols.size();
    mS64Cols.push_back(SwPPFSegDataBlock< String64 >(mS64Swapper, segsz,0,vsz));
    mS64ColIdx.push_back(idx);
    mS64ColsP.push_back(NULL);
    for(size_t kk=0; kk<mS64Cols.size(); kk++)
      mS64ColsP[kk] = &(mS64Cols[kk]);
    break;

  default:
    throw ParmError("SwPPFDataTable::AddColumn() unknown field type ");
    break;
  }

  return AddColumnBase(ft,cnom,vsz,ser);
}


} // FIN namespace SOPHYA 
