/*    
   --- SOPHYA software - FitsIOServer module ---
   R. Ansari , 2005-2008 
   (C) UPS+LAL IN2P3/CNRS     (C) DAPNIA-SPP/CEA 
*/
#ifndef FITSINOUTFILE_H
#define FITSINOUTFILE_H

#include "machdefs.h"
#include "pexceptions.h"
#include "dvlist.h"
/* #include "FitsIO/fitsio.h" */
#include <fitsio.h> /*G.Barrand*/ 
#include <complex> 



namespace SOPHYA {

  class FitsInOutFile;
  //class FitsInStream;
  //class FitsOutStream;

/*! 
  \ingroup FitsIOServer
  \brief Exception class used by  FITS file wrapper classes in FitsIOserver module
*/
class FitsIOException : public IOExc {
  public:
  explicit FitsIOException(const char * m) throw() : IOExc(m) {}
  explicit FitsIOException(const string& m) throw() : IOExc(m) {}
};


/*! 
  \ingroup FitsIOServer
  \brief  for converting c/c++ types to FITS data types 
*/
class FitsTypes {
public:
 // Conversion de type en constante de type image FITS (XXX_IMG)
 static int ImageType(uint_1 d) { return BYTE_IMG; }
 static int ImageType(int_2  d) { return SHORT_IMG; }
 static int ImageType(int_4  d) { return LONG_IMG; }
#ifdef LONGLONG_IMG
 static int ImageType(int_8  d) { return LONGLONG_IMG; }
#else 
 static int ImageType(int_8  d)
   { throw FitsIOException("FitsImageType: Unsupported data type int_8"); }
#endif

 static int ImageType(r_4  d)   { return FLOAT_IMG; }
 static int ImageType(r_8  d)   { return DOUBLE_IMG; }

 // Conversion de type en constante datatype FITS 
 static int DataType(uint_1 d) { return TBYTE; }
 static int DataType(int_2  d) { return TSHORT; }
 static int DataType(uint_2 d) { return TUSHORT; }

 static int DataType(const int_4  d) 
   { return (sizeof(long)==4) ? TLONG: TINT; }
 static int DataType(const uint_4 d) 
   { return (sizeof(long)==4) ? TULONG: TUINT; }

#ifdef TLONGLONG
 static int DataType(int_8 d) { return TLONGLONG; }
#else 
 static int DataType(int_8 d) 
   { throw FitsIOException("FitsDataTypes: Unsupported data type int_8"); }
#endif

 static int DataType(r_4  d)   { return TFLOAT; }
 static int DataType(r_8  d)   { return TDOUBLE; }

 static int DataType(complex<r_4>  d)   { return TCOMPLEX; }
 static int DataType(complex<r_8>  d)   { return TDBLCOMPLEX; }

 static int DataType(char*  d)   { return TSTRING; }

 // Conversion entre type FITS et chaine - exemple TFLOAT -> r_4 
 static string ImageTypeToTypeString(int ityp);
 static string DataTypeToTypeString(int ityp);
};

//------ Wrapper class for cfitsio library functions
class FitsInOutFile {
public :
  //! File access mode (ReadOnly, ReadWrite, Create)
 enum FitsIOMode { Fits_RO, Fits_RW, Fits_Create };

                   FitsInOutFile(); 
                   FitsInOutFile(string const & name, FitsIOMode mode);
                   FitsInOutFile(const char* name, FitsIOMode mode);
                   FitsInOutFile(FitsInOutFile const& fios); 
 virtual           ~FitsInOutFile();


 void              Open(const char* name, FitsIOMode mode);
 void              Close();

 void              ShareFitsPtr(FitsInOutFile const& fios);
 
 inline fitsfile*  FitsPtr() const { return fptr_; }
 static float      cfitsioVersion();
 //! Return the SOPHYA FitsIOServer version 
  static float      Version();

 //! Return the file name as specified in the constructor (or Open) 
 inline string     FileName() { return fname_; }

 //---- Header manipulation methods
 //! Return total number of HDU's in file
 int               NbHDUs() const;
 //! Return current HDU number - starting from 1 , not zero
 int               CurrentHDU() const;
 //! Return current HDU Type as IMAGE_HDU or BINARY_TBL or ASCII_TBL 
 int               CurrentHDUType() const;
 //! Return current HDU Type as a string IMAGE_HDU or BINARY_TBL or ASCII_TBL 
 string            CurrentHDUTypeStr() const;
 
 //! Move to HDU specified by hdnum - Returns the newly opened HDU type
 int               MoveAbsToHDU(int hdunum);
 //! Move to HDU specified by relhdu , relative to the current HDU - Returns the newly opened HDU type
 int               MoveRelToHDU(int relhdu);
 //! Move to the next HDU specified by relhdu. Returns the newly opened HDU type (<0 at EOF)
 int               MoveToNextHDU();
 //! Skip the first HDU if it contains no data. 
 bool              SkipEmptyFirstHDU();

 //---- IMAGE_HDU manipulation methods
 //! Creates a new HDU of type image (see fits_create_img)
 void              CreateImageHDU(int bitpix, int naxis, LONGLONG* naxes);
 //! Get information about the current image HDU. - return the image type TBYTE,TINT ...
 int               GetImageHDUInfo(int& naxis, LONGLONG* naxes) const;

 //! Change BSCALE/BZERO when reading/writing primary HDU
 void              SetBScaleBZero(double bscale=1., double bzero=0.);
	
 //---- BINARY_TBL or ASCII_TBL 
 //! Return number of rows in a table HDU
 LONGLONG          GetNbRows() const;
 //! Return number of columns in a table HDU
 int               GetNbCols() const;
 //! Creation of a new table - tbltyp = BINARY_TBL or ASCII_TBL
 void              CreateTable(int tbltyp, const char * extname, int ncols, 
			       char * colnames[], char * tform[], 
			       char * tunit[], long ininr=0);
 //! Creation of a new table - tbltyp = BINARY_TBL or ASCII_TBL
 void              CreateTable(int tbltyp, const string & extname, 
			       const vector<string> & colnames, 
			       const vector<string> & tform, 
			       const vector<string> & tunit, 
			       long ininr=0);
 //! Return number of columns, names, types and repeat count
 long              GetColInfo(vector<string> & colnames, 
			      vector<int> & coltypes,
			      vector<LONGLONG> & repcnt,
			      vector<LONGLONG> & width);

 //! Defines the extension name for the next table creation 
 inline void        SetNextExtensionName(string const & extname)
   { next_extname_ = extname; }
 //! Defines the extension name for the next table creation 
 inline void        SetNextExtensionName(const char * extname)
   { next_extname_ = extname; }
 //! Return the default extension name 
 inline string      NextExtensionName() const
   { return next_extname_; }

 //! Defines default table type for created tables as BINARY_TBL
 inline void        SetDef_BinTable() { def_tbltype = BINARY_TBL; }
 //! Defines default table type for created tables as ASCII_TBL
 inline void        SetDef_AscTable() { def_tbltype = ASCII_TBL; }
 //! Return default table type
 inline int         GetDef_TableType() { return def_tbltype; }

 //! Defines default column width for strings (Aw) 
 inline void        SetDef_StrColWidth(long w=16) { def_strcolw = w; }
 //! Return default column width for strings (Aw)
 inline long        GetDef_StrColWidth() { return def_strcolw; } 

 //! Insert (add) a new column
 void              InsertColumn(int numcol, const char* colname, const char* fmt);
 //! Insert (add) a new column
 inline void       InsertColumn(int numcol, const string& colname, const char* fmt)
   { InsertColumn(numcol, colname.c_str(), fmt); }


 // Manipulation des informations de l'entete 
 //! Retrieve a keyword value from the header 
  inline string    KeyValue(string const & key)
    { bool nosk; return KeyValue(key, nosk); }
 //! Retrieve a keyword value from the header 
  string           KeyValue(string const & key, bool& nosk);
 //! Read header records and appends the information to dvl
 int               GetHeaderRecords(DVList& dvl, 
				    bool stripkw= true, bool keepstkey=false);
 //! Appends a keyword to FITS header
 void	           WriteKey(const char * kname, MuTyV const & val, 
			    const char *comm=NULL);
 inline void	   WriteKey(string const & kname, MuTyV const & val, string const & comm) 
   { WriteKey(kname.c_str(), val, comm.c_str()); }
 //! Write dvl information to fits header 
 int               WriteHeaderRecords(DVList & dvl);

 //! Prints information about the fits file on standard output stream (cout)
 inline  void      Print(int lev=0) const  { Print(cout, lev); }  
 //! Prints information about the fits file on stream os
 virtual void      Print(ostream& os, int lev=0)  const;

protected:
 fitsfile *fptr_;     //  pointer to the FITS file, defined in fitsio.h 
 string fname_;       //  File name as passed to creator
 FitsIOMode mode_;    
 bool ownfptr;        //  If true, owns the FitsPointer, which will be closed by the destructor

 // Default extension name 
 string next_extname_;
 // Default table type 
 int def_tbltype;
 // default column width for strings 
 long def_strcolw;
};

/*! Prints FITS file information on stream \b s ( fio.Print(s) ) */

inline ostream& operator << (ostream& s, FitsInOutFile const & fio)
  {  fio.Print(s);  return(s);  }

} // Fin du namespace

#endif
