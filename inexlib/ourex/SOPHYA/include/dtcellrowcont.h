// This may look like C code, but it is really -*- C++ -*-
//-----------------------------------------------------------
// Classes pour representer les contenus des cellules et 
// lignes des DataTables 
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// R. Ansari      UPS+LAL IN2P3/CNRS  2005-2013
//-----------------------------------------------------------

#ifndef DTCELLROWCONT_H_SEEN
#define DTCELLROWCONT_H_SEEN


#include "machdefs.h"
#include "mutyv.h"
#include "fmlstr.h"
#include <string> 
#include <vector> 
#include <iostream> 

namespace SOPHYA {

//--------------------------------------------------------------------------------   
//------------------------------ DataTableRow class ------------------------------
//--------------------------------------------------------------------------------   

// Class for representing and manipulating DataTable row content.
class DataTableRow {
public:
//! Constructor - For use by BaseDataTable class
  DataTableRow( vector<string> const & colnames, vector<size_t> const & colvsz);
//! Copy constructor
  DataTableRow( DataTableRow const & a );
//! Destructor 
  ~DataTableRow();
//! Return a reference to the value of column \b k . (or the first element of)
  inline MuTyV&     operator()  (size_t k, size_t iv=0)  { return getPtr(k)[iv]; }
//! Return a reference to the value of column \b colname. (or the first element of)
  inline MuTyV&     operator()  (string const colname, size_t iv=0)   { return getPtr(colname)[iv]; }
//! Return the MuTyV pointer for column number \b k. 
  MuTyV*     getPtr(size_t k);
//! Return the MuTyV pointer for column named \b colname. 
  MuTyV*     getPtr(string const& colname);
//! Return the \b k th value in the row. No bound checking performed 
  inline MuTyV      operator[]  (size_t k)  const { return mtv_[k]; }
//! Return a reference to the \b k th value in the row. No bound checking performed 
  inline MuTyV&     operator[]  (size_t k)  { return mtv_[k]; }
//! return the total number of elements 
  inline size_t  Size() const { return size_; }
//! return the total number of columns
  inline size_t  NCols() const { return colseqp_.size(); }
//! prints the object content on the output stream 
  ostream&   Print(ostream& os) const;
//! Access to the MuTyV pointer
  inline MuTyV const *     MTVPtr() const { return mtv_; }
//! Acces to the MuTyV pointer
  inline MuTyV*     MTVPtr() { return mtv_; }
protected:
  MuTyV * mtv_;
  size_t size_;
  vector< size_t > colseqp_;    // column index to sequential position in mtv_
  map<string , size_t> nm2idx_;    // column name to column index 
};

/*! Prints the DataTableRow object content on the output stream ( */
inline ostream& operator << (ostream& s, DataTableRow const & row)
  {  row.Print(s);  return(s);  }


//--------------------------------------------------------------------------------   
//---------------------------- DataTableRowPtr class -----------------------------
//--------------------------------------------------------------------------------   

/*!
   \ingroup HiStats

  \brief enum defining cell content types. Used by DataTableRowPtr class
*/
enum DataTableCellType { DTC_IntegerType, DTC_LongType, 
			 DTC_FloatType, DTC_DoubleType,
			 DTC_ComplexType, DTC_DoubleComplexType,
			 DTC_DateTimeType, DTC_StringType, 
			 DTC_FMLStr16Type, DTC_FMLStr64Type };

/*!
   \ingroup HiStats
   \brief  multi data type pointer for datatable column content access 
*/
union dtcellptr_u {
  void* p; 
  char* cp; 
  int_4 * i4;
  int_8 * i8;
  r_4 * f4;
  r_8 * f8;
  complex< r_4 > * z4;
  complex< r_8 > * z8;
  TimeStamp * tms;
  std::string * s;
  String16 * s16;
  String64 * s64;
};
/*!
   \ingroup HiStats
   \brief typedef for datatable cell content pointer 
*/
typedef union dtcellptr_u DTCellPtrU ; 

/*!
   \ingroup HiStats
   \brief class for accessing DataTable cell contents (access the cell memory area through hidden pointer) 

   provide some conversion operators and assignment operators
*/
class DTCell {
public:
  //! default constructor
  DTCell() 
  { typ_=DTC_DoubleType;  u_ptr_.p=NULL; sz_=8; }
  //! constructor
  DTCell(DataTableCellType typ, void* p, size_t sz) 
  { typ_=typ; u_ptr_.p=p; sz_=sz; }
  //! copy constructor
  DTCell(DTCell const& a) 
  { typ_=a.typ_; u_ptr_=a.u_ptr_; sz_=a.sz_; }
  //! copy constructor with offset (array like access)
  DTCell(DTCell const& a, size_t poff) 
  { typ_=a.typ_; u_ptr_=a.u_ptr_; sz_=a.sz_; u_ptr_.cp+=(sz_*poff); }

  //! assignment (copy) operator
  DTCell& operator= (DTCell const& a)
  { typ_=a.typ_; u_ptr_=a.u_ptr_; sz_=a.sz_; return *this;}

  //!  array element access operator 
  DTCell operator[](size_t i) { return DTCell(*this,i); }
  //!  array element access operator (const version)
  const DTCell operator[](size_t i) const { return DTCell(*this,i); }

  //! Assignment operators from integer int_4
  inline int_4 operator= (int_4 v)
  { 
    if (typ_==DTC_IntegerType) *(u_ptr_.i4)=v;
    else if (typ_==DTC_LongType) *(u_ptr_.i8)=(int_8)v;
    else if (typ_==DTC_FloatType) *(u_ptr_.f4)=(r_4)v;
    else if (typ_==DTC_DoubleType) *(u_ptr_.f8)=(r_8)v;
    return v;
  }
  //! Assignment operators from long integer int_8
  inline int_8 operator= (int_8 v)
  { 
    if (typ_==DTC_IntegerType) *(u_ptr_.i4)=(int_4)v;
    else if (typ_==DTC_LongType) *(u_ptr_.i8)=(int_8)v;
    else if (typ_==DTC_FloatType) *(u_ptr_.f4)=(r_4)v;
    else if (typ_==DTC_DoubleType) *(u_ptr_.f8)=(r_8)v;
    return v;
  }
  //! Assignment operators from float (r_4)
  inline r_4 operator= (r_4 v)
  { 
    if (typ_==DTC_IntegerType) *(u_ptr_.i4)=(int_4)v;
    else if (typ_==DTC_LongType) *(u_ptr_.i8)=(int_8)v;
    else if (typ_==DTC_FloatType) *(u_ptr_.f4)=(r_4)v;
    else if (typ_==DTC_DoubleType) *(u_ptr_.f8)=(r_8)v;
    return v;
  }
  //! Assignment operators from double precision float (r_8)
  inline r_8 operator= (r_8 v)
  { 
    if (typ_==DTC_IntegerType) *(u_ptr_.i4)=(int_4)v;
    else if (typ_==DTC_LongType) *(u_ptr_.i8)=(int_8)v;
    else if (typ_==DTC_FloatType) *(u_ptr_.f4)=(r_4)v;
    else if (typ_==DTC_DoubleType) *(u_ptr_.f8)=(r_8)v;
    return v;
  }
  //! Assignment operators from complex ( complex<r_4> )
  inline complex<r_4> operator= (complex<r_4> const& v)
  {
    if (typ_==DTC_ComplexType) *(u_ptr_.z4)=v;
    else if (typ_==DTC_DoubleComplexType) *(u_ptr_.z8)=complex<r_8>((r_8)v.real(), (r_8)v.imag());
    return v;
  }
  //! Assignment operators from double precision complex ( complex<r_8> )
  inline complex<r_8> operator= (complex<r_8> const& v)
  {
    if (typ_==DTC_ComplexType) *(u_ptr_.z4)=complex<r_4>((r_4)v.real(), (r_4)v.imag());
    else if (typ_==DTC_DoubleComplexType) *(u_ptr_.z8)=v;
    return v;
  }
  //! Assignment operators from TimeStamp
  inline TimeStamp& operator= (TimeStamp& v)
  {
    if (typ_==DTC_DateTimeType) *(u_ptr_.tms)=v; 
    return v;
  }
  //! Assignment operators from character string (std::string) 
  inline std::string& operator= (std::string& v)
  {
    if (typ_==DTC_StringType) *(u_ptr_.s)=v; 
    else if (typ_==DTC_FMLStr16Type) *(u_ptr_.s16)=v; 
    else if (typ_==DTC_FMLStr64Type) *(u_ptr_.s64)=v; 
    return v;
  }
  //! Assignment operators from character string (Fixed length String16) 
  inline String16 operator= (String16& v)
  {
    if (typ_==DTC_StringType) *(u_ptr_.s)=v.c_str(); 
    else if (typ_==DTC_FMLStr16Type) *(u_ptr_.s16)=v; 
    else if (typ_==DTC_FMLStr64Type) *(u_ptr_.s64)=v.c_str(); 
    return v;
  }
  //! Assignment operators from character string (Fixed length String64) 
  inline String64 operator= (String64& v)
  {
    if (typ_==DTC_StringType) *(u_ptr_.s)=v.c_str(); 
    else if (typ_==DTC_FMLStr16Type) *(u_ptr_.s16)=v.c_str(); 
    else if (typ_==DTC_FMLStr64Type) *(u_ptr_.s64)=v; 
    return v;
  }
  //! Assignment operators from character string (c string pointer) 
  inline const char* operator= (const char * v)
  {
    if (typ_==DTC_StringType) *(u_ptr_.s)=v; 
    else if (typ_==DTC_FMLStr16Type) *(u_ptr_.s16)=v; 
    else if (typ_==DTC_FMLStr64Type) *(u_ptr_.s64)=v; 
    return v;
  }

  //--- conversion to value operators (useful at right hand side of = ) 
  //! Conversion (cast) operator to int_4
  inline operator int_4() const 
  {
    if (typ_==DTC_IntegerType) return *(u_ptr_.i4);
    else if (typ_==DTC_LongType) return (int_4)(*(u_ptr_.i8)); 
    else if (typ_==DTC_FloatType) return (int_4)(*(u_ptr_.f4)); 
    else if (typ_==DTC_DoubleType) return (int_4)(*(u_ptr_.f8)); 
    return 0;
  }
  //! Conversion (cast) operator to int_8
  inline operator int_8() const 
  {
    if (typ_==DTC_IntegerType) return (int_8)(*(u_ptr_.i4));
    else if (typ_==DTC_LongType) return (int_8)(*(u_ptr_.i8)); 
    else if (typ_==DTC_FloatType) return (int_8)(*(u_ptr_.f4)); 
    else if (typ_==DTC_DoubleType) return (int_8)(*(u_ptr_.f8)); 
    return 0;
  }
  //! Conversion (cast) operator to float (r_4)
  inline operator r_4() const 
  {
    if (typ_==DTC_IntegerType) return (r_4)(*(u_ptr_.i4));
    else if (typ_==DTC_LongType) return (r_4)(*(u_ptr_.i8)); 
    else if (typ_==DTC_FloatType) return (r_4)(*(u_ptr_.f4)); 
    else if (typ_==DTC_DoubleType) return (r_4)(*(u_ptr_.f8)); 
    return (r_4)0.;
  }
  //! Conversion (cast) operator to double precision float (r_8)
  inline operator r_8() const 
  {
    if (typ_==DTC_IntegerType) return (r_8)(*(u_ptr_.i4));
    else if (typ_==DTC_LongType) return (r_8)(*(u_ptr_.i8)); 
    else if (typ_==DTC_FloatType) return (r_8)(*(u_ptr_.f4)); 
    else if (typ_==DTC_DoubleType) return (r_8)(*(u_ptr_.f8)); 
    return (r_8)0.;
  }
  //! Conversion (cast) operator to complex ( complex<r_4> )
  inline operator complex<r_4> () const 
  {
    if (typ_==DTC_ComplexType) return *(u_ptr_.z4);
    else if (typ_==DTC_DoubleComplexType) return complex<r_4>(u_ptr_.z8->real(), u_ptr_.z8->imag());
    return complex<r_4>(0.,0.);
  }
  //! Conversion (cast) operator to double precision complex ( complex<r_8> )
  inline operator complex<r_8> () const 
  {
    if (typ_==DTC_ComplexType) return complex<r_8>(u_ptr_.z4->real(), u_ptr_.z4->imag());
    else if (typ_==DTC_DoubleComplexType) return *(u_ptr_.z8); 
    return complex<r_4>(0.,0.);
  }
  //! Conversion (cast) operator to TimeStamp
  inline operator TimeStamp () const  
  {
    if (typ_==DTC_DateTimeType) return(*(u_ptr_.tms)); 
    return TimeStamp();
  }
  //! Conversion (cast) operator to c-style character string 
  inline operator const char * () const 
  {
    if (typ_==DTC_StringType) return (u_ptr_.s->c_str()); 
    else if (typ_==DTC_FMLStr16Type) return (u_ptr_.s16->c_str()); 
    else if (typ_==DTC_FMLStr64Type) return (u_ptr_.s64->c_str()); 
    return "";
  }

  inline DTCellPtrU& Ptr() { return u_ptr_; }
  inline DataTableCellType Type() { return typ_; }
  inline size_t DataSize() { return sz_; }

  DataTableCellType typ_;
  DTCellPtrU u_ptr_;
  size_t sz_;
};

//---- Class for direct access to data table row content through pointer 
class DataTableRowPtr {
public:
//! Constructor - For use by BaseDataTable class
  DataTableRowPtr( vector< DataTableCellType > const & coltypes, vector<string> const & colnames, vector<size_t> const & colvsz);
//! Copy constructor
  DataTableRowPtr( DataTableRowPtr const & a );
//! Destructor 
  ~DataTableRowPtr();
//! Return a reference to the DTCellPtrU of column \b k . No bound checking performed 
  inline DTCellPtrU&  operator[]  (size_t k) { return cellptr_[k].Ptr(); }
//! Return a const reference to the DTCellPtrU of column \b k . No bound checking performed 
  inline const DTCellPtrU&  operator[]  (size_t k) const { return cellptr_[k].Ptr(); }

/*! 
  \brief Return a DTCell object corresponding to elemts \b i of column \b k . 
  
  Use this operator with the Index(colname) method to get access to the cell named colname.
*/
  inline DTCell  operator()  (size_t k, size_t i=0)  { return getCell(k,i); }
//! Return a const DTCell object corresponding to elemts \b i of column \b k . 
  inline const DTCell  operator()  (size_t k, size_t i=0) const { return getCell(k,i); }

//! Return the row size (number of cells or columns) 
  inline size_t Size() const { return cellsizes_.size(); }
//! Return the row size (number of cells or columns) - alias for Size()
  inline size_t  NCols() const { return cellsizes_.size(); }
//! Copy content from the DataTableRowPtr \b a. No compatibility check is performed 
  void   CopyContentFrom(DataTableRowPtr const & a);
//! Copy content from the DataTableRowPtr \b a and column pairs identified by cid. No compatibility check is performed. 
  void   CopyContentFrom(DataTableRowPtr const & a, vector< pair<size_t, size_t> > const & cid);
//! prints the object content on the output stream \b os 
  ostream&   Print(ostream& os, const char* csep=" ; ", const char* vsep=",") const;
//! Access to the DTCell pointer 
  inline DTCell * CellPtr() { return cellptr_; }
//! get cell/column index from its name - can throw NotFoundExc exception 
  size_t Index(string const & colname) const ;
//! get cell/column type
  inline DataTableCellType CellType(size_t k) const { return cellptr_[k].Type(); }
//! get cell/column vector size
  inline size_t CellVSize(size_t k) const { return cellsizes_[k]; }
//! return data size as a function of the data type DataTableCellType
  static size_t getDataSize(DataTableCellType typ);
protected:
//! return the DTCell object corresponding to element \b i of column \b k
  DTCell  getCell(size_t k, size_t i) const
  {
    if ((k>=Size())||(i>=cellsizes_[k]))  throw RangeCheckError("DataTableRowPtr::getCell(k,i) Out of range k or i");
    return DTCell(cellptr_[k],i);
  }

  DTCell * cellptr_;
  vector< size_t > cellsizes_;      // cell vector size 
  map<string , size_t> nm2idx_;    // column/cell name to column index 
};

/*! Prints the DataTableRow object content on the output stream ( */
inline ostream& operator << (ostream& s, DataTableRowPtr const & row)
  {  row.Print(s);  return(s);  }


} // namespace SOPHYA

#endif
