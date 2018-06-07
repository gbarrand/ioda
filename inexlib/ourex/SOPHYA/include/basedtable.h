// This may look like C code, but it is really -*- C++ -*-
// Class BaseDataTable 
//   R. Ansari -  Avril 2005  - MAJ : 2013 
//   (C)  LAL-IN2P3/CNRS , Univ. Paris Sud,  CEA-Irfu  

#ifndef BASEDTABLE_H_SEEN
#define BASEDTABLE_H_SEEN

#include "machdefs.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <complex>

#include "ntupintf.h"
#include "dvlist.h"
#include "segdatablock.h"
#include "timestamp.h"
#include "tvector.h"
#include "fmlstr.h"
#include "dtcellrowcont.h"

namespace SOPHYA {

//  Forward class declaration for Fits handler
template <class T>  class FitsHandler;
template <class T>  class HDF5_Handler;


//  Forward class declaration for Thread-safe operations
class ThSafeOp;

//! Interface definition for classes handling data in a table.
class BaseDataTable : public AnyDataObj , public NTupleInterface {
public:
  // ===> DO NOT CHANGE EXISTING enum type VALUES !!! 
  enum FieldType {IntegerField=1, LongField=2, 
		  FloatField=3, DoubleField=4,
		  ComplexField=5, DoubleComplexField=6,
		  StringField=7, DateTimeField=8,
                  FMLStr16Field=16, FMLStr64Field=64};
  // <=====================================================> 

  //! Return the column type as a string (long or short depending on fgl=true/false)
  static string     ColTypeToString(FieldType ft, bool fgl=false);
  //! Return the column type corresponding to \b sctyp
  static FieldType  StringToColType(string const & sctyp);
  //! Convert to the corresponding DataTableCellType enum value
  static DataTableCellType Convert2DTCellType(FieldType ft);
  // constructeur , destructeur 
  BaseDataTable(size_t segsz=512);
  virtual           ~BaseDataTable();

  // Activate/deactivate thread-safe AddRow()/GetRow() operation
  virtual void      SetThreadSafe(bool fg=true);
  //! Return true if AddRow()/GetRow()  operations are thread-safe
  inline bool       IsThreadSafe() const { return ((mThS)?true:false); }
  
  //! Adds a column holding integer, named \b cnom, vector column content with vsz if (vsz>1). 
  inline size_t  AddIntegerColumn(const char * cnom, size_t vsz=1) 
  { return AddColumn(IntegerField, cnom, vsz); }
  //! Adds a column holding integer, named \b cnom, vector column content with vsz if (vsz>1). 
  inline size_t  AddIntegerColumn(string const & cnom, size_t vsz=1) 
  { return AddColumn(IntegerField, cnom, vsz); }
  //! Adds a column holding long integer, named \b cnom, vector column content with vsz if (vsz>1). 
  inline size_t  AddLongColumn(const char * cnom, size_t vsz=1) 
  { return AddColumn(LongField, cnom, vsz); }
  //! Adds a column holding long integer, named \b cnom, vector column content with vsz if (vsz>1). 
  inline size_t  AddLongColumn(string const & cnom, size_t vsz=1) 
  { return AddColumn(LongField, cnom, vsz); }
  //! Adds a column holding floating values (r_4), named \b cnom, vector column content with vsz if (vsz>1). 
  inline size_t  AddFloatColumn(const char * cnom, size_t vsz=1) 
  { return AddColumn(FloatField, cnom, vsz); }
  //! Adds a column holding floating values (r_4), named \b cnom, vector column content with vsz if (vsz>1). 
  inline size_t  AddFloatColumn(string const & cnom, size_t vsz=1) 
  { return AddColumn(FloatField, cnom, vsz); }
  //! Adds a column holding double values (r_8), named \b cnom, vector column content with vsz if (vsz>1). 
  inline size_t  AddDoubleColumn(const char * cnom, size_t vsz=1) 
  { return AddColumn(DoubleField, cnom, vsz); }
  //! Adds a column holding double values (r_8), named \b cnom, vector column content with vsz if (vsz>1). 
  inline size_t  AddDoubleColumn(string const & cnom, size_t vsz=1) 
  { return AddColumn(DoubleField, cnom, vsz); }
  //! Adds a column holding single precision complex values (complex<r_4>), named \b cnom, vector column content with vsz if (vsz>1). 
  inline size_t  AddComplexColumn(const char * cnom, size_t vsz=1) 
  { return AddColumn(ComplexField, cnom, vsz); }
  //! Adds a column holding single precision complex values (complex<r_4>), named \b cnom, vector column content with vsz if (vsz>1). 
  inline size_t  AddComplexColumn(string const & cnom, size_t vsz=1) 
  { return AddColumn(ComplexField, cnom, vsz); }
  //! Adds a column holding double precision complex values (complex<r_8>), named \b cnom, vector column content with vsz if (vsz>1). 
  inline size_t  AddDoubleComplexColumn(const char * cnom, size_t vsz=1) 
  { return AddColumn(DoubleComplexField, cnom, vsz); }
  //! Adds a column holding double precision complex values (complex<r_8>), named \b cnom, vector column content with vsz if (vsz>1). 
  inline size_t  AddDoubleComplexColumn(string const & cnom, size_t vsz=1) 
  { return AddColumn(DoubleComplexField, cnom, vsz); }
  //! Adds a column holding character strings, named \b cnom, vector column content with vsz if (vsz>1). 
  inline size_t  AddStringColumn(const char * cnom, size_t vsz=1) 
  { return AddColumn(StringField, cnom, vsz); }
  //! Adds a column holding character strings, named \b cnom, vector column content with vsz if (vsz>1). 
  inline size_t  AddStringColumn(string const & cnom, size_t vsz=1) 
  { return AddColumn(StringField, cnom, vsz); }
  //! Adds a column holding Date/Time value, named \b cnom, vector column content with vsz if (vsz>1).  
  inline size_t  AddDateTimeColumn(const char * cnom, size_t vsz=1) 
  { return AddColumn(DateTimeField, cnom, vsz); }
  //! Adds a column holding Date/Time value, named \b cnom, vector column content with vsz if (vsz>1).  
  inline size_t  AddDateTimeColumn(string const & cnom, size_t vsz=1) 
  { return AddColumn(DateTimeField, cnom, vsz); }
  //! Adds a column holding character strings with maximum length 16, named \b cnom, vector column content with vsz if (vsz>1). 
  inline size_t  AddString16Column(const char * cnom, size_t vsz=1) 
  { return AddColumn(FMLStr16Field, cnom, vsz); }
  //! Adds a column holding character strings with maximum length 16, named \b cnom, vector column content with vsz if (vsz>1). 
  inline size_t  AddString16Column(string const & cnom, size_t vsz=1) 
  { return AddColumn(FMLStr16Field, cnom, vsz); }
  //! Adds a column holding character strings with maximum length 64, named \b cnom, vector column content with vsz if (vsz>1). 
  inline size_t  AddString64Column(const char * cnom, size_t vsz=1) 
  { return AddColumn(FMLStr64Field, cnom, vsz); }
  //! Adds a column holding character strings with maximum length 64, named \b cnom, vector column content with vsz if (vsz>1). 
  inline size_t  AddString64Column(string const & cnom, size_t vsz=1) 
  { return AddColumn(FMLStr64Field, cnom, vsz); }

  //! Method for adding a new column to the Data Table, vector column content with vsz if (vsz>1).   
  inline size_t  AddColumn(FieldType ft, const char * cnom, size_t vsz=1, bool fgfcnet=false)
  { string nom = cnom;  return AddColumn(ft, nom, vsz, fgfcnet); }  
  /*
    \brief Pure virtual method for adding a new column to the Data Table, vector column content with vsz if (vsz>1). 
    if fgfcnet==true, force column creation even if the table has been filled already.
  */
  virtual size_t AddColumn(FieldType ft, string const & cnom, size_t vsz=1, bool fgfcnet=false) = 0;

  //! Duplicate the data table structure from the source Table
  virtual size_t CopyStructure(BaseDataTable const & a);
  //! Checks if the two table have the same column structure.  
  virtual bool      CompareStructure(BaseDataTable const & a);
  //! Checks if the two table have the same column structure, with the same column names.  
  virtual bool      CompareStructAndColNames(BaseDataTable const & a);

  //! Define or change the physical unit of the data for the column \b k  of the table
  virtual void SetUnits(size_t k, const Units& un);  
  //! Define or change the physical unit of the data for the column of the table named \b cnom
  inline  void SetUnits(string cnom, const Units& un)
  { SetUnits(IndexNom(cnom), un); }
  /*!
    \brief Return the physical unit of the data for the column \b k  of the table
    Flag sdone is set to true if SetUnits() has been called for the column k
  */
  virtual Units GetUnits(size_t k, bool& sdone) const;
  //! Return the physical unit of the data for the column \b k  of the table
  inline Units GetUnits(size_t k) const  
  { bool fg; return GetUnits(k, fg); }
  /*!
    \brief Return the physical unit of the data for the column of the table named \b cnom
    Flag sdone is set to true if SetUnits() has been called for the column \b cnom
  */
  inline Units GetUnits(string cnom, bool& sdone) const
  {  return GetUnits(IndexNom(cnom), sdone); }
  //! Return the physical unit of the data for the column \b cnom  of the table
  inline Units GetUnits(string cnom) const  
  { bool fg; return GetUnits(IndexNom(cnom), fg); }

  // Verifie la validite du nom de colonne:envoie une exception
  // si nom en double ou non valide
  virtual bool      CheckColName(string const & cnom);
  // Verifie et corrige si besoin un nom de colonne
  virtual bool      CheckCorrectColName(string & cnom);

  // Access to various counts and parameters
  //! Return the number of lines (rows) in the table)
  inline size_t  NRows() const { return mNEnt ; }
  //! Return the number of lines (rows) in the table)
  inline size_t  NEntry() const { return mNEnt ; }
  //! Return the number of columns in the tables (number of cells in a row) - alias: NCols() NVar() NVars() 
  inline size_t  NColumns() const   { return mNames.size() ; } 
  //! Return the number of columns in the tables (number of cells in a row) - alias: NColumns() NVar() NVars() 
  inline size_t  NCols() const   { return mNames.size() ; } 
  //! Return the number of columns in the tables (number of cells in a row) - alias: NColumns() NCols() NVars()  
  inline size_t  NVar() const   { return mNames.size() ; } 
  //! Return the number of columns in the tables (number of cells in a row) - alias: NColumns() NCols() NVar()  
  inline size_t  NVars() const   { return mNames.size() ; } 
  //! Return the total number of items in a row ( = Sum_Columns [ vecsize ] )
  size_t         NItems_in_Row() const ;
  //! Return the segment size (SegDBInterface objects corresponding to columns)  
  inline size_t  SegmentSize() const   { return mSegSz ; } 
  //! Return the number of segments (SegDBInterface objects corresponding to columns)  
  inline size_t  NbSegments() const   { return mNSeg ; } 

  /*!
    \brief To define the number of rows 
    This method should only be used when filling the table through the FillColumn() methods. 
    It increases the table filled area and if necessary extends the table. 
    Does nothing if \b nrows is less than the current number of rows  
  */
  virtual void SetNRows(size_t nrows);
  // Pour etendre la table -      
  virtual size_t Extend();


  //! Return a compatible DataTableRow object (with table column names and vector sizes)
  virtual DataTableRow  EmptyRow() const ;
  //! Return a compatible DataTableRowPtr object 
  virtual DataTableRowPtr  EmptyRowPtr() const ;

  //---- Methods to fill the table 
  // Filling data structures (adding lines/rows)
  virtual size_t AddRow(const r_8* data);
  // Filling data structures (adding lines/rows)
  virtual size_t AddRow(const MuTyV * data);
  // Filling data structures (adding lines/rows)
  virtual size_t AddRow(DataTableRow const & data);

  //! Alias for AddRow()
  inline size_t  AddLine(const r_8* data)
  { return AddRow(data); }
  //! Alias for AddRow()
  inline size_t  AddLine(const MuTyV * data)
  { return AddRow(data); }
  //! Alias for AddLine()
  inline size_t  AddLine(DataTableRow const & data)
  { return AddRow(data); }

  //! Alias for AddRow() - Kept for backward compatibility with NTuple class interface.
  inline size_t  Fill(const r_8* data)
  { return AddRow(data); }

  //--- Filling column  content without conversion - throw TypeMismatchExc if incompatible column type.
  //! Copies the \b v vector content to column \b k , if IntegerField type column. throw TypeMismatchExc otherwise.
  virtual void FillColumn(size_t k, vector<int_4> const & v);
  //! Copies the \b v vector content to column \b k , if LongField type column. throw TypeMismatchExc otherwise.
  virtual void FillColumn(size_t k, vector<int_8> const & v);
  //! Copies the \b v vector content to column \b k , if FloatField type column. throw TypeMismatchExc otherwise.
  virtual void FillColumn(size_t k, vector<r_4> const & v);
  //! Copies the \b v vector content to column \b k , if DoubleField type column. throw TypeMismatchExc otherwise.
  virtual void FillColumn(size_t k, vector<r_8> const & v);
  //! Copies the \b v vector content to column \b k , if ComplexField type column. throw TypeMismatchExc otherwise.
  virtual void FillColumn(size_t k, vector< complex<r_4> > const & v);
  //! Copies the \b v vector content to column \b k , if DoubleComplexField type column. throw TypeMismatchExc otherwise.
  virtual void FillColumn(size_t k, vector< complex<r_8> > const & v);
  //! Copies the \b v vector content to column \b k , if StringField type column. throw TypeMismatchExc otherwise.
  virtual void FillColumn(size_t k, vector< string > const & v);
  //! Copies the \b v vector content to column \b k , if DateTimeField type column. throw TypeMismatchExc otherwise.
  virtual void FillColumn(size_t k, vector< TimeStamp > const & v);
  //! Copies the \b v vector content to column \b k , if FMLStr16Field type column. throw TypeMismatchExc otherwise.
  virtual void FillColumn(size_t k, vector< String16 > const & v);
  //! Copies the \b v vector content to column \b k , if FMLStr64Field type column. throw TypeMismatchExc otherwise.
  virtual void FillColumn(size_t k, vector< String64 > const & v);


  //! Return the information stored in row \b n of the table in \b row object
  virtual DataTableRow&   GetRow(size_t n, DataTableRow& row) const ;
  //! Return the information stored in line (row) \b n of the table 
  virtual MuTyV *   GetRow(size_t n, MuTyV* mtvp=NULL) const ;
  //! Return the information stored in line \b n of the table. Alias of GetLine 
  inline  MuTyV *   GetLine(size_t n) const 
  { return GetRow(n); }

  //! set the data pointers of the DataTableRowPtr \b rowp to the data cells of row \b n of the table (R/W access)
  virtual DataTableRowPtr&   GetRowPtr(size_t n, DataTableRowPtr& rowp);
  //! set the data pointers of the DataTableRowPtr \b rowp to the data cells of row \b n of the table (RO access) 
  virtual DataTableRowPtr&   GetCstRowPtr(size_t n, DataTableRowPtr& rowp) const;
  //! 
  /*! Add a row to the table and sets \b rowp object to the newly created row cell pointers 
    \brief Adds a row to the table set the data pointers of the DataTableRowPtr \b rowp to the data cells of 
    row \b n of the table.

    No check on the DataTableRowPtr \b rowp object is performed, which should be created by 
    a call to the EmptyRowPtr() method. return the DataTableRowPtr provided as the argument.
  */
  virtual  DataTableRowPtr&   NextRowPtr(DataTableRowPtr& rowp);
  
  //--- Accessing column information without conversion - throw TypeMismatchExc if incompatible column type.
  //! return the column \b k content copied to \b v , if IntegerField type column. throw TypeMismatchExc otherwise.
  virtual void GetColumn(size_t k, vector<int_4> & v) const ;
  //! return the column \b k content copied to \b v , if LongField type column. throw TypeMismatchExc otherwise.
  virtual void GetColumn(size_t k, vector<int_8> & v) const ;
  //! return the column \b k content copied to \b v , if FloatField type column. throw TypeMismatchExc otherwise.
  virtual void GetColumn(size_t k, vector<r_4> & v) const ;
  //! return the column \b k content copied to \b v , if DoubleField type column. throw TypeMismatchExc otherwise.
  virtual void GetColumn(size_t k, vector<r_8> & v) const ;
  //! return the column \b k content copied to \b v , if ComplexField type column. throw TypeMismatchExc otherwise.
  virtual void GetColumn(size_t k, vector< complex<r_4> > & v) const ;
  //! return the column \b k content copied to \b v , if DoubleComplexField type column. throw TypeMismatchExc otherwise.
  virtual void GetColumn(size_t k, vector< complex<r_8> > & v) const ;
  //! return the column \b k content copied to \b v , if StringField type column. throw TypeMismatchExc otherwise.
  virtual void GetColumn(size_t k, vector< string > & v) const ;
  //! return the column \b k content copied to \b v , if DateTimeField type column. throw TypeMismatchExc otherwise.
  virtual void GetColumn(size_t k, vector< TimeStamp > & v) const ;
  //! return the column \b k content copied to \b v , if FMLStr16Field type column. throw TypeMismatchExc otherwise.
  virtual void GetColumn(size_t k, vector< String16 > & v) const ;
  //! return the column \b k content copied to \b v , if FMLStr64Field type column. throw TypeMismatchExc otherwise.
  virtual void GetColumn(size_t k, vector< String64 > & v) const ;

  //! Return the information stored in column \b k of the table, converted to double 
  virtual TVector<r_8>  GetColumnD(size_t k) const ;
  //! Return the information stored in column named \b nom of the table, converted to double 
  inline TVector<r_8>  GetColumnD(char const * nom) const 
  { return  GetColumnD(IndexNom(nom)) ; }
  //! Return the information stored in column named \b nom of the table, converted to double 
  inline TVector<r_8>  GetColumnD(string const & nom) const 
  { return  GetColumnD(IndexNom(nom)) ; }

  //! Return the index for column name \b nom  
  size_t         IndexNom(char const* nom) const ; 
  //! Return the index for column name \b nom  
  inline size_t  IndexNom(string const & nom) const 
  { return IndexNom(nom.c_str()); }
  //! Return the column name for column index \b k
  string            NomIndex(size_t k) const ; 

  //! Return the column type for column \b k (no check on index range)
  inline FieldType  GetColumnType(size_t k) const
  { return mNames[k].type; }
  //! Return the column name  for column \b k (no check on index range)
  inline const string & GetColumnName(size_t k)  const
  { return mNames[k].nom; }
  //! Return the column content vector size for column \b k (no check on index range)
  inline size_t  GetColumnVecSize(size_t k) const
  { return mNames[k].vecsz; }
  
  
  //! Copy or merges the data (rows) from \b a into the data table (cp=true -> copy)
  virtual void  CopyMerge(BaseDataTable const& a, bool cp=false) ; 

  //! Copy a selection of rows from a source table
  virtual void  SelectFrom(BaseDataTable const& src, std::vector<size_t> const & rows);
  //! Copy a selection of rows and columns from table \b src
  virtual void  SelectFrom(BaseDataTable const& src, std::vector<size_t> const & rows, std::vector<size_t> const & cols);
  //! Merges selected columns data identified by \b cols from \b src table. Row numbers in \b src is given by \b rows 
  virtual void  MergeColumn(BaseDataTable const& src, std::vector<size_t> const & rows,  std::vector<size_t> const & cols,
			    int_8 defval=0, const char* defstr="");
   

  //! Clear/reset the table content and structure.
  virtual void Clear() = 0;

  //! Prints  the table content (ascii dump)
  virtual ostream&  Print(ostream& os, size_t lstart, size_t lend, 
			  size_t lstep=1) const ;
  //! Prints  the table content (ascii dump) on stream \b os
  inline ostream&   Print(ostream& os) const
  { return Print(os, 0, NEntry(), 1); }
  //! Prints  the table content (ascii dump) on stream \b os
  inline ostream&   WriteASCII(ostream& os) const 
  { return Print(os, 0, NEntry(), 1); }

  
  //! Prints table definition and number of entries 
  void              Show(ostream& os) const ; 
  //! Prints table definition and number of entries on the standard output stream \b cout
  inline void       Show() const { Show(cout) ; } 
    //! If set (fgsmmx=true)  GetMinMax() method is called by the Show() method 
  inline  void      SetShowMinMaxFlag(bool fgsmmx=false)  const { mShowMinMaxFg=fgsmmx;}

  //! Return the associated DVList (info) object. 
  DVList&           Info() const ; 
  
  // Remplissage depuis fichier ASCII 
  size_t	    FillFromASCIIFile(istream& is, char clm='#', const char* sep=" \t");

  //! Compute min-max and sum-squared of values for column \b b   
  virtual void      ComputeMinMaxSum(size_t k, bool fgforce=false) const;
  //! Compute min-max and sum, sum-squared of values for all columns   
  virtual void      ComputeMinMaxSumAll(bool fgforce=false) const;
  //! Return a table row (line), formatted and converted to a string
  virtual string    TableRowToString(size_t n, bool qstr, const char* sep=" ", int fw=12) const; 

  //-------------------------------------------------------------
  //-----------  Declaration de l interface NTuple --------------

  virtual size_t    NbLines() const ;
  virtual size_t    NbColumns() const ;
  virtual r_8 *     GetLineD(size_t n) const ;
  virtual r_8	    GetCell(size_t n, size_t k) const ;
  inline  r_8	    GetCell(size_t n, std::string const & nom) const 
  { return GetCell(n, IndexNom(nom)); }
  virtual string    GetCelltoString(size_t n, size_t k) const ;
  inline  string    GetCelltoString(size_t n, string const & nom) const 
  { return GetCelltoString(n, IndexNom(nom)); }
  virtual void      GetMinMax(size_t k, double& min, double& max)   const ; 
  inline  void	    GetMinMax(string const & nom, double& min, double& max)   const 
  { return GetMinMax(IndexNom(nom), min, max); }
  virtual void	    GetSum(size_t k, double& sum, double& sumsq)   const ; 
  inline  void	    GetSum(string const & nom, double& sum, double& sumsq)   const 
  { return GetSum(IndexNom(nom), sum, sumsq); }

  virtual size_t    ColumnIndex(string const & nom)  const ;
  virtual string    ColumnName(size_t k) const;
  virtual string    VarList_C(const char* nomx=NULL) const ;
  virtual string    LineHeaderToString() const;
  virtual string    LineToString(size_t n) const;  
//-- Extended interface 
  virtual bool          ImplementsExtendedInterface() const;
  virtual NTupMTPointer * GetLineMTP(size_t n) const ; 
  virtual string	VarListMTP_C(const char* nomx=NULL) const ;

  //  Pour la gestion de persistance PPF
  friend class ObjFileIO<BaseDataTable> ;
  // pour fichiers FITS
  friend class FitsHandler<BaseDataTable>;
  friend class HDF5_Handler<BaseDataTable>;
  
protected:
  //! clear methods to be called by the Clear() method of the derived classes 
  virtual void ClearP();
  //! This method should be called by AddColumn() of derived classes 
  virtual size_t AddColumnBase(FieldType ft, string const & cnom, size_t vsz, size_t ser);

  uint_8   mNEnt ;   // nb total d'entrees 
  uint_8   mSegSz ;  // taille bloc/segment 
  uint_8   mNSeg;    // Nb total de segments
  mutable r_8 *  mVarD;    // Pour retourner une ligne de la table
  mutable MuTyV *  mVarMTV;    // Pour retourner une ligne de la table en MuTyV
  mutable NTupMTPointer * mVarMTP;  // Pour retourner une ligne de la table en pointeurs ( NTupMTPointer )
  mutable r_8 * mVarTMS;     // pour gerer les double provenant de la conversion des DateTime.ToDays()
  mutable const char ** mVarStrP;  // pour gerer les chaines associees a mVarMTP

  //! \cond   Pour pas inclure ds la documentation doxygen
  typedef struct { 
    string nom;      // nom de la colonne
    FieldType type;  // type de la colonne 
    size_t vecsz;    // vsz>1 : column contains a vector with size vsz 
    size_t ser;      // numero de sequence ds le vecteur de SegDataBlock<type> 
    size_t item_in_row;  // numero d'element dans la ligne , une colonne compte pour vecsz elements
  } colst;
  //! \endcond  
  std::vector<colst> mNames;

  mutable bool mFgVecCol;       // true -> il y a des colonnes avec contenu vecteur 
  mutable DVList* mInfo;        // Infos (variables) attachees au DataTable 

  // Pour calculer et garder le min/max  et Somme_Colonne et Somme_Colonne^2
  mutable std::vector<r_8> mMin;     
  mutable std::vector<r_8> mMax;
  mutable std::vector<r_8> mSum;
  mutable std::vector<r_8> mSumSq;
  mutable std::vector<uint_8> mMinMaxNEnt;

  mutable bool mShowMinMaxFg;     // if true -> appel a GetMin/Max et affichage valeurs ds methode Show

  // Les pointeurs des SegDataBlock ...  
  std::vector< SegDBInterface<int_4> * > mIColsP; 
  std::vector< size_t > mIColIdx;
  std::vector< SegDBInterface<int_8> * > mLColsP; 
  std::vector< size_t > mLColIdx;
  std::vector< SegDBInterface<r_4> * > mFColsP;
  std::vector< size_t > mFColIdx; 
  std::vector< SegDBInterface<r_8> * > mDColsP; 
  std::vector< size_t > mDColIdx;
  std::vector< SegDBInterface< complex<r_4> > * > mYColsP;
  std::vector< size_t > mYColIdx; 
  std::vector< SegDBInterface< complex<r_8> > * > mZColsP; 
  std::vector< size_t > mZColIdx;
  std::vector< SegDBInterface< std::string > * > mSColsP; 
  std::vector< size_t > mSColIdx;
  std::vector< SegDBInterface< TimeStamp > * > mTColsP; 
  std::vector< size_t > mTColIdx;
  std::vector< SegDBInterface< String16 > * > mS16ColsP; 
  std::vector< size_t > mS16ColIdx;
 std::vector< SegDBInterface< String64 > * > mS64ColsP; 
  std::vector< size_t > mS64ColIdx;

  ThSafeOp* mThS;       // != NULL -> Thread safe operations
  
}; 
/*! Prints table information (Column name and type, min/max) on stream \b s (bd.Show(s)) */
inline ostream& operator << (ostream& s, BaseDataTable const & bd)
  {  bd.Show(s);  return(s);  }
  
} // namespace SOPHYA

#endif
