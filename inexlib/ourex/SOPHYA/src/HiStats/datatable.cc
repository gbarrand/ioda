#include "datatable.h"
#include "strutil.h"
#include "pexceptions.h"
#include "fiosegdb.h"
#include "thsafeop.h"

namespace SOPHYA { 

/*!
   \class DataTable
   \ingroup HiStats
   This class can be used to organize data in table (row-column) form.
   Each column holds homogeneous data (same data type), while different
   columns can be used for different data types 
   (integer, float, string ...).
   The whole data set is kept in memory.
   \sa SOPHYA::MuTyV
   \sa SOPHYA::DataTableRow
   \sa SOPHYA::DataTableRowPtr
   \sa SOPHYA::BaseDataTable
   \sa SOPHYA::SegDataBlock

   \code
   #include "datatable.h"
   // ...
   DataTable dt(64);
   dt.AddFloatColumn("X0_f");
   dt.AddFloatColumn("X1_f");
   dt.AddDoubleColumn("X0X0pX1X1_d");
   double x[5];
   for(int i=0; i<63; i++) {
     x[0] = (i%9)-4.;  x[1] = (i/9)-3.;  x[2] = x[0]*x[0]+x[1]*x[1];
     dt.AddRow(x);
   }
   // Printing table info
   cout << dt ;
   // Saving object into a PPF file
   POutPersist po("dtable.ppf");
   po << dt ;
   \endcode

  DataTableRowPtr and DataTableRow objects can also be used to fill the table, 
  or access the data, as shown in the example below for DataTableRow:
  \code
  // We use the same table as in the example above:
  DataTableRow row = dt.EmptyRow();
  row[0] = 83.3; row[1] = 20.;   row[2] = 83.3*83.3+20.*20.;
  dt.AddRow(row);
  row["X0_f"] = 7.5; row["X1_f"] = 6.;  row["X0X0pX1X1_d"] = 7.5*7.5+6.*6.;
  dt.AddRow(row);  
  \endcode
*/


//! Default constructor - optional specification of block (or segment) size
DataTable::DataTable(size_t segsz)
  : BaseDataTable(segsz)
{
}

//! copy constructor 
/*!
  The copy constructur shares the data if \b share=true. 
  Otherwise, the Clone() method is called to make a complete copy.
  This constructor copies also the thread safety state from \b a .
*/
DataTable::DataTable(DataTable const & a, bool share)
  : BaseDataTable(a.SegmentSize())
{
  if (share) Share(a);
  else Clone(a);
  mNEnt = a.mNEnt;
  mNSeg = a.mNSeg;
  if (a.mInfo)  mInfo = new DVList(*(a.mInfo));
}
/*! 
  Copy the table structure from \b a and shares the data (columns content).
  The tread-safety state is copied from \b a .
*/
void DataTable::Share(DataTable const & a)
{
  // On copie la structure de table 
  CopyStructure(a);
  if (a.IsThreadSafe())  SetThreadSafe(true);
  else SetThreadSafe(false);
  // Et on partage les donnees des colonnes 
  for (size_t kk=0; kk<mNames.size(); kk++) {
    size_t sk = mNames[kk].ser;
    size_t ska = a.mNames[kk].ser;
    switch (mNames[kk].type) {
    case IntegerField :
      mICols[sk].Share(a.mICols[ska]); 
    break;
    case LongField :
      mLCols[sk].Share(a.mLCols[ska]); 
      break;
    case FloatField :
      mFCols[sk].Share(a.mFCols[ska]); 
      break;
    case DoubleField :
      mDCols[sk].Share(a.mDCols[ska]); 
      break;
    case ComplexField :
      mYCols[sk].Share(a.mYCols[ska]); 
      break;
    case DoubleComplexField :
      mZCols[sk].Share(a.mZCols[ska]); 
      break;
    case StringField :
      mSCols[sk].Share(a.mSCols[ska]); 
      break;
    case DateTimeField :
      mTCols[sk].Share(a.mTCols[ska]); 
      break;
    case FMLStr16Field :
      mS16Cols[sk].Share(a.mS16Cols[ska]); 
      break;
    case FMLStr64Field :
      mS64Cols[sk].Share(a.mS64Cols[ska]); 
      break;
    default:
      throw ForbiddenError("DataTable::Share() : unknown column type ");
    break;
    }
  }  
}

//! Copy the table structure from \b a and duplicate (copy) the data (columns content)
void DataTable::Clone(DataTable const & a)
{
  // On copie la structure de table 
  CopyStructure(a);
  if (a.IsThreadSafe())  SetThreadSafe(true);
  else SetThreadSafe(false);
  // Et on copie les donnees des colonnes 
  for (size_t kk=0; kk<mNames.size(); kk++) {
    size_t sk = mNames[kk].ser;
    size_t ska = a.mNames[kk].ser;
    switch (mNames[kk].type) {
    case IntegerField :
      mICols[sk].Clone(a.mICols[ska], true); 
    break;
    case LongField :
      mLCols[sk].Clone(a.mLCols[ska], true); 
      break;
    case FloatField :
      mFCols[sk].Clone(a.mFCols[ska], true); 
      break;
    case DoubleField :
      mDCols[sk].Clone(a.mDCols[ska], true); 
      break;
    case ComplexField :
      mYCols[sk].Clone(a.mYCols[ska], true); 
      break;
    case DoubleComplexField :
      mZCols[sk].Clone(a.mZCols[ska], true); 
      break;
    case StringField :
      mSCols[sk].Clone(a.mSCols[ska], true); 
      break;
    case DateTimeField :
      mTCols[sk].Clone(a.mTCols[ska], true); 
      break;
    case FMLStr16Field :
      mS16Cols[sk].Clone(a.mS16Cols[ska], true); 
      break;
    case FMLStr64Field :
      mS64Cols[sk].Clone(a.mS64Cols[ska], true); 
      break;
    default:
      throw ForbiddenError("DataTable::Clone() : unknown column type ");
    break;
    }
  }  
}
//! Reset (/clear) the table content and structure
void DataTable::Clear()
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
size_t DataTable::AddColumn(FieldType ft, string const & cnom, size_t vsz, bool fgfcnet)
{
  if (!fgfcnet && (NEntry() > 0)) 
    throw ParmError("DataTable::AddColumn() Table contains already data ");
  CheckColName(cnom);
  if (vsz < 1) vsz=1;
  size_t ser; 
  size_t idx = NVar(); 
  size_t segsz = mSegSz*vsz;
  /* ATTENTION: 
   Dans ce code qui suit, la reinitilisation complete de mXColsP  (X=I.L,F,D,Y,Z,S,T ...) 
   est imperatif. En effet, le push_back() ds mXCols  provoque des copies des objets 
   SegDataBlock contenu deja ds mXCols et le changement des pointeurs correspondant...
  */
  switch (ft) {
  case IntegerField :
    ser = mICols.size();
    mICols.push_back(SegDataBlock<int_4>(segsz, 0, vsz));
    mIColIdx.push_back(idx);
    mIColsP.push_back(NULL);
    for(size_t kk=0; kk<mICols.size(); kk++)
      mIColsP[kk] = &(mICols[kk]);
    break;
  case LongField :
    ser = mLCols.size();
    mLCols.push_back(SegDataBlock<int_8>(segsz, 0, vsz));
    mLColIdx.push_back(idx);
    mLColsP.push_back(NULL);
    for(size_t kk=0; kk<mLCols.size(); kk++)
      mLColsP[kk] = &(mLCols[kk]);
    break;
  case FloatField :
    ser = mFCols.size();
    mFCols.push_back(SegDataBlock<r_4>(segsz, 0, vsz));
    mFColIdx.push_back(idx);
    mFColsP.push_back(NULL);
    for(size_t kk=0; kk<mFCols.size(); kk++)
      mFColsP[kk] = &(mFCols[kk]);
    break;
  case DoubleField :
    ser = mDCols.size();
    mDCols.push_back(SegDataBlock<r_8>(segsz, 0, vsz));
    mDColIdx.push_back(idx);
    mDColsP.push_back(NULL);
    for(size_t kk=0; kk<mDCols.size(); kk++)
      mDColsP[kk] = &(mDCols[kk]);
    break;
  case ComplexField :
    ser = mYCols.size();
    mYCols.push_back(SegDataBlock< complex<r_4> >(segsz, 0, vsz));
    mYColIdx.push_back(idx);
    mYColsP.push_back(NULL);
    for(size_t kk=0; kk<mYCols.size(); kk++)
      mYColsP[kk] = &(mYCols[kk]);
    break;
  case DoubleComplexField :
    ser = mZCols.size();
    mZCols.push_back(SegDataBlock< complex<r_8> >(segsz, 0, vsz));
    mZColIdx.push_back(idx);
    mZColsP.push_back(NULL);
    for(size_t kk=0; kk<mZCols.size(); kk++)
      mZColsP[kk] = &(mZCols[kk]);
    break;
  case StringField :
    ser = mSCols.size();
    mSCols.push_back(SegDataBlock<string>(segsz, 0, vsz));
    mSColIdx.push_back(idx);
    mSColsP.push_back(NULL);
    for(size_t kk=0; kk<mSCols.size(); kk++)
      mSColsP[kk] = &(mSCols[kk]);
    break;
  case DateTimeField :
    ser = mTCols.size();
    mTCols.push_back(SegDataBlock<TimeStamp>(segsz, 0, vsz));
    mTColIdx.push_back(idx);
    mTColsP.push_back(NULL);
    for(size_t kk=0; kk<mTCols.size(); kk++)
      mTColsP[kk] = &(mTCols[kk]);
    break;
  case FMLStr16Field :
    ser = mS16Cols.size();
    mS16Cols.push_back(SegDataBlock< String16 >(segsz,0,vsz));
    mS16ColIdx.push_back(idx);
    mS16ColsP.push_back(NULL);
    for(size_t kk=0; kk<mS16Cols.size(); kk++)
      mS16ColsP[kk] = &(mS16Cols[kk]);
    break;
  case FMLStr64Field :
    ser = mS64Cols.size();
    mS64Cols.push_back(SegDataBlock< String64 >(segsz,0,vsz));
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
