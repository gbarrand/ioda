#include "swfitsdtable.h"
#include "fitshdtable.h"
#include "pexceptions.h"
#include "thsafeop.h"

namespace SOPHYA { 

/*!
   \class SwFitsDataTable
   \ingroup FitsIOServer
   This class can be used to organize data in table (row-column) form.
   Each column holds homogeneous data (same data type), while different
   columns can be used for different data types 
   (integer, float, string ...).
   A Fits file binary table is used as swap space.

   \warning 
   - When creating a table, the output fits file (FitsInOutFile) must not be closed
   (destroyed) before the SwFitsDataTable object is destroyed. The destructor writes
   out any pending buffer to the fits file.
   - It is not possible to make a complete (deep) copy of a SwFitsDataTable
   Copy constructor and equal operator shares the data.
   - The min/max values for column data are not updated when reading from a fits file.
   In this case, the Show() method (or ostream& << operator) will compute min/max 
   values. This operation will take some time for large tables.
   - The SwFitsDataTable operation (AddRow()/GetRow() ) is NOT thread safe

   \sa SOPHYA::MuTyV
   \sa SOPHYA::DataTableRow
   \sa SOPHYA::DataTable
   \sa SOPHYA::SwSegDataBlock  SOPHYA::FitsDataSwapper

   \code
   #include "swfitsdtable.h"
   // ...
   {
   // ---- Creation of the table
   // Create the swap stream 
   FitsInOutFile so("myswtable.fits", FitsInOutFile::Fits_Create);
   SwFitsDataTable dt(so, 16, true);
   // define table columns
   dt.AddFloatColumn("X0_f");
   dt.AddFloatColumn("X1_f");
   dt.AddDoubleColumn("X0X0pX1X1_d");
   // Fill the table
   r_8 x[5];
   for(int i=0; i<63; i++) {
     x[0] = (i%9)-4.;  x[1] = (i/9)-3.;  x[2] = x[0]*x[0]+x[1]*x[1];
     dt.AddLine(x);
   }
   // Printing table info
   cout << dt ;
   // The destructor will Swap out  data still in memory 
   }
   {
   // ---- Accessing information from a previously created table
   FitsInOutFile si("myswtable.fits", FitsInOutFile::Fits_ReadOnly);
   // Position the fits file on the first extension (BinTable)
   si.MoveAbsToHDU(2);  
   SwFitsDataTable dt(si, 512, false);
   // Printing table info 
   cout << dt ;   
   }
   \endcode
*/
//! Default constructor with optional specification of block (or segment) size 
SwFitsDataTable::SwFitsDataTable(size_t segsz)
  : BaseDataTable(segsz) , mFgCreate(false) , mFgDefDone(false)
{
}
/*!
  \brief Construcor with specification of the \b FitsInOutFile swap stream.
  if fgcreate == true, creates an empty table (the fits file should then be 
  opened for writing). 
  if fgcreate == false , the table is initialized (read in) from the current HDU.
*/
SwFitsDataTable::SwFitsDataTable(FitsInOutFile & os, size_t segsz, bool fgcreate)
  : BaseDataTable(segsz) ,
    mSwF(os) , mFgCreate(fgcreate) , mFgDefDone(false)
{
  if (!fgcreate) {   // Lecture de la table 
    FitsHandler<BaseDataTable> fio(*this);
    fio.Read(os);
    mFgDefDone = true;
  }
}

/*!
  \brief Construcor with specification of the FITS file name, as a Read-only file.
  The FITS file is opened and the table is initialized (read in) from HDU \b hdunum
*/
  SwFitsDataTable::SwFitsDataTable(string fitsname, int hdunum, size_t segsz)
  : BaseDataTable(segsz) ,
    mSwF(fitsname, FitsInOutFile::Fits_RO) , mFgCreate(false) , mFgDefDone(true)
{
  // Lecture de la table 
  FitsHandler<BaseDataTable> fio(*this);
  mSwF.MoveAbsToHDU(hdunum);
  fio.Read(mSwF);
}

//! copy constructor - shares the data (and copies the thread safety state)
SwFitsDataTable::SwFitsDataTable(SwFitsDataTable const & a)
  : BaseDataTable(a.SegmentSize()),
    mSwF(a.mSwF) , mFgCreate(a.mFgCreate) , mFgDefDone(a.mFgDefDone)
{
  // Attention(Reza Nov07): le mFgDefDone doit etre en principe partage entre les objets 
  Share(a);
}
//! Destructor - Deletes / closes the input PPF swap stream  
SwFitsDataTable::~SwFitsDataTable()
{
  if (mFgCreate) { 
    SwapOutAll();
    // Ecriture de SegmentSize et autre elements de DVList  
    mSwF.WriteHeaderRecords(Info());
    MuTyV mtv = (uint_8)SegmentSize();
    mSwF.WriteKey("SEGMSIZE",mtv," SOPHYA::DataTable SegmentSize");
    mtv = "SOPHYA::SwFitsDataTable";
    mSwF.WriteKey("SOPCLSNM",mtv," Object class name "); 
  }
}

void SwFitsDataTable::Share(SwFitsDataTable const & a)
{
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
    case DateTimeField :
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
    default:
      throw ForbiddenError("SwFitsDataTable::Share() : unknown column type ");
    break;
    }
  }  
}
void SwFitsDataTable::SwapOutAll() const
{
  if (NRows() < 1) return;
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
      throw ForbiddenError("SwFitsDataTable::SwapOutAll() : unknown column type ");
    break;
    }
  }  
}

void SwFitsDataTable::Clear()
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
  size_t SwFitsDataTable::AddColumn(FieldType ft, string const & cnom, size_t vsz, bool fgfcnet)
{
  return AddColRd(ft, cnom, -1, NULL, vsz, fgfcnet);
}
/*! 
  Adds a column to the table. If swpos != NULL, the swapper is initialized 
  for reading data from the fits file. This method can be called
  by FitsHandler<BaseDataTable> with non zero swpos. 
*/
size_t SwFitsDataTable::AddColRd(FieldType ft, string const & cnom, 
				 int colidx, vector<int_8> const * swpos, size_t vsz, bool fgfcnet)
{
  if (!fgfcnet && (NEntry() > 0)) 
    throw ParmError("SwFitsDataTable::AddColumn() Table contains already data ");
  CheckColName(cnom);
  if (vsz < 1) vsz=1;
  size_t ser; 
  size_t idx = NVar(); 
  if (colidx < 1) colidx = idx+1; 
  size_t segsz = mSegSz*vsz;
  /* ATTENTION: 
   colidx est le numero de colonne ds le fichier FITS (colidx >= 1) 
   Dans ce code qui suit, la reinitilisation complete de mXColsP  (X=I.L,F,D,Y,Z,S ...) 
   est imperatif. En effet, le push_back() ds mXCols  provoque des copies des objets 
   SwSegDataBlock contenu deja ds mXCols et le changement des pointeurs correspondant...
  */

  switch (ft) {
  case IntegerField :
  {
    ser = mICols.size();
    FITSDataSwapper<int_4> ISwapper(mSwF, colidx, vsz, this);
    if (swpos)
      mICols.push_back(SwSegDataBlock<int_4>(ISwapper, *swpos, segsz, vsz));
    else 
      mICols.push_back(SwSegDataBlock<int_4>(ISwapper, segsz, 0, vsz));
    mIColIdx.push_back(idx);
    mIColsP.push_back(NULL);
    for(size_t kk=0; kk<mICols.size(); kk++)
      mIColsP[kk] = &(mICols[kk]);
    break;
  }
  case LongField :
  {
    ser = mLCols.size();
    FITSDataSwapper<int_8> LSwapper(mSwF, colidx, vsz, this);
    if (swpos)    
      mLCols.push_back(SwSegDataBlock<int_8>(LSwapper, *swpos, segsz, vsz));
    else 
      mLCols.push_back(SwSegDataBlock<int_8>(LSwapper, segsz, 0, vsz));
    mLColIdx.push_back(idx);
    mLColsP.push_back(NULL);
    for(size_t kk=0; kk<mLCols.size(); kk++)
      mLColsP[kk] = &(mLCols[kk]);
    break;
  }
  case FloatField :
  {
    ser = mFCols.size();
    FITSDataSwapper<r_4> FSwapper(mSwF, colidx, vsz, this);
    if (swpos)    
      mFCols.push_back(SwSegDataBlock<r_4>(FSwapper, *swpos, segsz, vsz));
    else
      mFCols.push_back(SwSegDataBlock<r_4>(FSwapper, segsz, 0, vsz));
    mFColIdx.push_back(idx);
    mFColsP.push_back(NULL);
    for(size_t kk=0; kk<mFCols.size(); kk++)
      mFColsP[kk] = &(mFCols[kk]);
    break;
  }
  case DoubleField :
  {
    ser = mDCols.size();
    FITSDataSwapper<r_8> DSwapper(mSwF, colidx, vsz, this);
    if (swpos)    
      mDCols.push_back(SwSegDataBlock<r_8>(DSwapper, *swpos, segsz, vsz));
    else 
      mDCols.push_back(SwSegDataBlock<r_8>(DSwapper, segsz, 0, vsz));      
    mDColIdx.push_back(idx);
    mDColsP.push_back(NULL);
    for(size_t kk=0; kk<mDCols.size(); kk++)
      mDColsP[kk] = &(mDCols[kk]);
    break;
  }
  case ComplexField :
  {
    ser = mYCols.size();
    FITSDataSwapper< complex<r_4> > YSwapper(mSwF, colidx, vsz, this);
    if (swpos)    
      mYCols.push_back(SwSegDataBlock< complex<r_4> >(YSwapper, *swpos, segsz, vsz));
    else
      mYCols.push_back(SwSegDataBlock< complex<r_4> >(YSwapper, segsz, 0, vsz));
    mYColIdx.push_back(idx);
    mYColsP.push_back(NULL);
    for(size_t kk=0; kk<mYCols.size(); kk++)
      mYColsP[kk] = &(mYCols[kk]);
    break;
  }
  case DoubleComplexField :
  {
    ser = mZCols.size();
    FITSDataSwapper< complex<r_8> > ZSwapper(mSwF, colidx, vsz, this);
    if (swpos)    
      mZCols.push_back(SwSegDataBlock< complex<r_8> >(ZSwapper, *swpos, segsz, vsz));
    else
      mZCols.push_back(SwSegDataBlock< complex<r_8> >(ZSwapper, segsz, 0, vsz));
    mZColIdx.push_back(idx);
    mZColsP.push_back(NULL);
    for(size_t kk=0; kk<mZCols.size(); kk++)
      mZColsP[kk] = &(mZCols[kk]);
    break;
  }
  case StringField :
  {
    ser = mSCols.size();
    FITSDataSwapper< string > SSwapper(mSwF, colidx, vsz, this);
    if (swpos)    
      mSCols.push_back(SwSegDataBlock< string >(SSwapper, *swpos, segsz, vsz));
    else 
      mSCols.push_back(SwSegDataBlock< string >(SSwapper, segsz, 0, vsz));
    mSColIdx.push_back(idx);
    mSColsP.push_back(NULL);
    for(size_t kk=0; kk<mSCols.size(); kk++)
      mSColsP[kk] = &(mSCols[kk]);
    break;
  }
  case DateTimeField :
  {
    ser = mTCols.size();
    FITSDataSwapper< TimeStamp > TSwapper(mSwF, colidx, vsz, this);
    if (swpos)    
      mTCols.push_back(SwSegDataBlock< TimeStamp >(TSwapper, *swpos, segsz, vsz));
    else
      mTCols.push_back(SwSegDataBlock< TimeStamp >(TSwapper, segsz, 0, vsz));
    mTColIdx.push_back(idx);
    mTColsP.push_back(NULL);
    for(size_t kk=0; kk<mTCols.size(); kk++)
      mTColsP[kk] = &(mTCols[kk]);
    break;
  }
  case FMLStr16Field :
  {
    ser = mS16Cols.size();
    FITSDataSwapper< String16 > SSwapper(mSwF, colidx, vsz, this);
    if (swpos)    
      mS16Cols.push_back(SwSegDataBlock< String16 >(SSwapper, *swpos, segsz, vsz));
    else 
      mS16Cols.push_back(SwSegDataBlock< String16 >(SSwapper, segsz, 0, vsz));
    mS16ColIdx.push_back(idx);
    mS16ColsP.push_back(NULL);
    for(size_t kk=0; kk<mS16Cols.size(); kk++)
      mS16ColsP[kk] = &(mS16Cols[kk]);
    break;
  }
  case FMLStr64Field :
  {
    ser = mS64Cols.size();
    FITSDataSwapper< String64 > SSwapper(mSwF, colidx, vsz, this);
    if (swpos)    
      mS64Cols.push_back(SwSegDataBlock< String64 >(SSwapper, *swpos, segsz, vsz));
    else 
      mS64Cols.push_back(SwSegDataBlock< String64 >(SSwapper, segsz, 0, vsz));
    mS64ColIdx.push_back(idx);
    mS64ColsP.push_back(NULL);
    for(size_t kk=0; kk<mS64Cols.size(); kk++)
      mS64ColsP[kk] = &(mS64Cols[kk]);
    break;
  }

  default:
    throw ParmError("SwFitsDataTable::AddColumn() unknown field type ");
    break;
  }

  return AddColumnBase(ft,cnom,vsz,ser);
}


/*! 
  \brief Writes table definition (column name/type) to the FITS file

  It is not possible to add columns once the table definition is 
  written to the fits file. Returns number of table columns.
  Although AddRow() checks and perform this operation automatically,
  it is advised to do it explicitely, before using an SwFitsDataTable
  for filling in multi-thread programs.
*/
size_t SwFitsDataTable::WriteTableDefinitionToFits()
{
  if (!mFgDefDone && (NRows() == 0)) {  
    FitsHandler<BaseDataTable> fio(*this);
    fio.Write(mSwF);
    mFgDefDone = true;
  }
  return NVar();
}

//! Adds a line (or row to the table) with r_8* input data.
/*! 
  The min/max values for each column is updated, in addition 
  to the actions performed by the base class AddRow().
  The table is also created on the FITS file at the first call to AddRow(), 
  by calling WriteTableDefinitionToFits() , if not already done.
*/
size_t SwFitsDataTable::AddRow(const r_8* data)
{
  if (mThS) mThS->lock();   // tread-safety
  if (!mFgDefDone && (NRows() == 0)) { // On cree la definition de la table FITS au premier appel  
    FitsHandler<BaseDataTable> fio(*this);
    fio.Write(mSwF);
    mFgDefDone = true;
  }
  // On est oblige de calculer les min-max lors du remplissage
  // On ne peut pas en effet 'relire' le swap pendant l'ecriture
  for(size_t k=0; k<NVar(); k++) {
    double x = data[k];
    if (x < mMin[k])  mMin[k] = x;
    if (x > mMax[k])  mMax[k] = x;
    mMinMaxNEnt[k]++;
  }
  if (mThS) mThS->unlock(); // tread-safety
  return BaseDataTable::AddRow(data);
}


//! Adds a row (or line to the table) with input data as an array of MuTyV
/*! 
  The min/max values for each column is updated, in addition 
  to the actions performed by the base class AddRow().
  The table is also created on the FITS file at the first call to AddRow() 
*/
size_t SwFitsDataTable::AddRow(const MuTyV * data)
{
  if (mThS) mThS->lock();   // tread-safety
  if (!mFgDefDone && (NRows() == 0)) { // On cree la definition de la table FITS au premier appel  
    FitsHandler<BaseDataTable> fio(*this);
    fio.Write(mSwF);
    mFgDefDone = true;
  }
  // On est oblige de calculer les min-max lors du remplissage
  // On ne peut pas en effet 'relire' le swap pendant l'ecriture
  for(size_t k=0; k<NVar(); k++) {
    double x = (double)data[k];
    if (x < mMin[k])  mMin[k] = x;
    if (x > mMax[k])  mMax[k] = x;
    mMinMaxNEnt[k]++;
  }
  if (mThS) mThS->unlock();   // tread-safety
  return BaseDataTable::AddRow(data);
}

//! Adds a row (or line) to the table with input data as DataTableRow object
/*! 
  The table is  created on the FITS file at the first call to AddRow() 
*/
size_t SwFitsDataTable::AddRow(DataTableRow const& data)
{
  if ( data.Size() != NCols() )
    throw SzMismatchError(" SwFitsDataTable::AddRow() - data.Size() != NCols() ");
  return AddRow(data.MTVPtr());
}

//! Adds a row (or line) to the table and fills \b rowp with the corresponding data pointers 
DataTableRowPtr&   SwFitsDataTable::NextRowPtr(DataTableRowPtr& rowp)
{
  if (mThS) mThS->lock();   // tread-safety
    if (!mFgDefDone && (NRows() == 0)) { // On cree la definition de la table FITS au premier appel  
    FitsHandler<BaseDataTable> fio(*this);
    fio.Write(mSwF);
    mFgDefDone = true;
  }
  if (mThS) mThS->unlock();   // tread-safety
  return BaseDataTable::NextRowPtr(rowp);
}

} // FIN namespace SOPHYA 
