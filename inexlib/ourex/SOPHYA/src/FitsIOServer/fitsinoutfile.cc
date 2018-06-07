#include "sopnamsp.h"
#include "machdefs.h"
#include "segdatablock.h"
#include "fitsinoutfile.h"
#include "strutil.h"
#include <stdio.h>
#include <string.h>
#include <iostream>

string FitsTypes::ImageTypeToTypeString(int ityp)
{
  switch (ityp) {
  case BYTE_IMG :
    return "uint_1";
    break;
  case SHORT_IMG :
    return "int_2";
    break;
  case LONG_IMG :
    return "int_4";
    break;
//  Support de cfitsio pour HDU_IMAGE avec data de type int_8 
#ifdef LONGLONG_IMG
  case LONGLONG_IMG :
    return "int_8";
    break;  
#endif
  case FLOAT_IMG :
    return "r_4";
    break;
  case DOUBLE_IMG :
    return "r_8";
    break;
  default:
    return "???" ;
    break;
  }
  return "";
}
string FitsTypes::DataTypeToTypeString(int ityp)
{
  switch (ityp) {
  case TBYTE :
    return "uint_1";
    break;
  case TSHORT :
    return "int_2";
    break;
  case TUSHORT :
    return "uint_2";
    break;
  case TINT :
    if (sizeof(int) == 4)  return "int_4";
    else if (sizeof(int) == 8)  return "int_8";
    else if (sizeof(int) == 2)  return "int_2";
    break;
  case TUINT :
    if (sizeof(int) == 4)  return "uint_4";
    else if (sizeof(int) == 8)  return "uint_8";
    else if (sizeof(int) == 2)  return "uint_2";
    break;
  case TINT32BIT :  /*  case TLONG : idem car TINT32BIT=TLONG=41 ds fitsio.h */ 
    return "int_4";
    break;
  case TULONG :
    if (sizeof(long) == 4)  return "uint_4";
    else if (sizeof(long) == 8)  return "uint_8";
    else if (sizeof(long) == 2)  return "uint_2";
    break;
#ifdef TLONGLONG
  case TLONGLONG :
    return "int_8";
    break;
#endif
  case TFLOAT :
    return "r_4";
    break;
  case TDOUBLE :
    return "r_8";
    break;
  case TCOMPLEX : 
    return "complex< r_4 >";
    break;
  case TDBLCOMPLEX : 
    return "complex< r_8 >";
    break;
  case TSTRING : 
    return "string";
    break;
  default:
    return "???" ;
    break;
  }
  return "";
}

/*! 
  \class SOPHYA::FitsInOutFile
  \ingroup FitsIOServer
  \brief  Wrapper class for cfitsio library functions 
*/

/*-- Methode --*/
//! Default constructor - The file should be opened subsequently using Open
FitsInOutFile::FitsInOutFile()
{
  fptr_ = NULL;
  mode_ = Fits_RO;
  SetDef_BinTable();
  SetDef_StrColWidth();
}

/*-- Methode --*/
//! Constructor with specification of file name and access mode
FitsInOutFile::FitsInOutFile(string const & name, FitsIOMode mode)
{
  //  cout << " DBG - FitsInOutFile(string name= " << name << ")" << endl;
  fptr_ = NULL;
  ownfptr = true;
  SetDef_BinTable();
  SetDef_StrColWidth();
  Open(name.c_str(), mode);
}

/*-- Methode --*/
//! Constructor with specification of file name and access mode
FitsInOutFile::FitsInOutFile(const char * name, FitsIOMode mode)
{
  //  cout << " DBG - FitsInOutFile(char* name= " << name << ")" << endl;
  fptr_ = NULL;
  ownfptr = true;
  SetDef_BinTable();
  SetDef_StrColWidth();
  Open(name, mode);
}

/*! \brief Copy constructor 
  \warning The fits file pointer is owned by the original FitsInOutFile object and 
  should not be closed as long as the new object is being used.
*/
FitsInOutFile::FitsInOutFile(FitsInOutFile const& fios)
{
  fptr_ = fios.fptr_;
  fname_ = fios.fname_;
  mode_ = fios.mode_;
  ownfptr = false;
  SetDef_BinTable();
  SetDef_StrColWidth();
}

/* -- Fonction utilitaire pour verifier fichier ouvert (pointeur non null) -- */
static inline void CheckFitsPtr(fitsfile* fptr) {
  if (fptr == NULL) throw  FitsIOException("FitsInOutFile/Error - NULL fitsfile pointer ");
  return;
}
/* -- Fonction utilitaire pour verifier le code d'erreur fitsio -- */
static inline bool FitsCheckStatus(int st, const char * emsg = NULL) {
  if (st) {
    fits_report_error(stderr, st);
    if (emsg) {
      char buff[FLEN_ERRMSG];
      fits_get_errstatus(st, buff);
      string msg = emsg;
      msg += buff;
      throw FitsIOException(msg);
    }
    else return true;
  }
  else return false;
}

/*-- Methode --*/
//! Destructor - Closes the fits file (if opened)
FitsInOutFile::~FitsInOutFile()
{
  Close();
}

/*-- Methode --*/
//! Opens the named fits file (see cfitsio routines fits_open_file and fits_create_file) 
void FitsInOutFile::Open(const char* name, FitsIOMode mode)
{
  if (fptr_ != NULL) 
    throw  FitsIOException("FitsInOutFile::Open() /Error - file already opened ");
  int status = 0;
  fptr_ = NULL;
  switch ( mode ) {
  case Fits_RO :
    fits_open_file(&fptr_, name, READONLY, &status);
    break;
  case Fits_RW :
    fits_open_file(&fptr_, name, READWRITE, &status);
    break;
  case Fits_Create :
    fits_create_file(&fptr_, name, &status);
    break;
  }
  FitsCheckStatus(status, "FitsInOutFile::Open() Error: ");
  fname_ = name;
  mode_ = mode;
  ownfptr = true;
  return;
}

/*-- Methode --*/
//! Opens the named fits file (see cfitsio routines fits_open_file and fits_create_file) 
void FitsInOutFile::Close()
{
  if (fptr_ == NULL) return;
  if (ownfptr == false) {
    fptr_ = NULL; return;
  }
  int status = 0;
  if (mode_ == Fits_Create) {
    status = 0;
    int hdutyp;
    fits_movabs_hdu(FitsPtr() , 1, &hdutyp, &status);
    status = 0;
    float sfv = Version();
    fits_write_key(FitsPtr(), TFLOAT, "SOPHYAFV", &sfv, 
		   "SOPHYA FitsIOServer module version", &status);
    fits_write_date(FitsPtr(), &status);
    status = 0;
    fits_write_comment(FitsPtr(), "--------------- SOPHYA  (http://www.sophya.org) ---------------", &status);
    fits_write_comment(FitsPtr(), "(C) Univ. Paris Sud, (C) LAL/IN2P3-CNRS, (C) IRFU/CEA  (FRANCE)", &status);
    fits_write_comment(FitsPtr(), "---------------------------------------------------------------", &status);
  }
  MoveAbsToHDU(1);
  status = 0;
  if (fptr_) fits_close_file(fptr_, &status);
  if (status) {
    cerr << " FitsInOutFile::Close - Error closing fits file !" << endl;
    fits_report_error(stderr, status);
  }
}

/*! \brief Closes the current fits file and uses \b fios file for subsequent operations.
  \warning The fits file pointer is owned by the original FitsInOutFile object and 
  should not be closed as long as the current object (this) is being used.
*/

void FitsInOutFile::ShareFitsPtr(FitsInOutFile const& fios)
{
  Close();
  fptr_ = fios.fptr_;
  fname_ = fios.fname_;
  mode_ = fios.mode_;
  ownfptr = false;
}

/*-- Methode --*/
float FitsInOutFile::cfitsioVersion() 
{
  float ver;
  fits_get_version(&ver);
  return ver;
}

/*-- Methode --*/
float FitsInOutFile::Version() 
{
  // DOIT ETRE IDENTIQUE A FitsIOServer_MOD_VERS de fiosinit.cc 
  return (float)2.22;
}

/*-- Methode --*/
int FitsInOutFile::NbHDUs()  const
{
  int status = 0;
  int nbhdu = 0;
  fits_get_num_hdus(FitsPtr() , &nbhdu, &status);
  FitsCheckStatus(status, "FitsInOutFile::NbHDUs() Error: ");
  return nbhdu;
}

/*-- Methode --*/
int FitsInOutFile::CurrentHDU()  const
{
  //unused: int status = 0;
  int curhdu = 0;
  fits_get_hdu_num(FitsPtr() , &curhdu);
  return curhdu;
}

/*-- Methode --*/
int FitsInOutFile::CurrentHDUType()  const
{
  int status = 0;
  int hdutyp = 0;
  fits_get_hdu_type(FitsPtr() , &hdutyp, &status);
  FitsCheckStatus(status, "FitsInOutFile::CurrentHDUType() Error: ");
  return hdutyp;
}

/*-- Methode --*/
string FitsInOutFile::CurrentHDUTypeStr()  const
{
  int status = 0;
  int hdutyp = 0;
  fits_get_hdu_type(FitsPtr() , &hdutyp, &status);
  if ( FitsCheckStatus(status, "FitsInOutFile::CurrentHDUTypeStr() Error: ") ) 
    return "Unknown";
  else { 
    if (hdutyp == IMAGE_HDU) return "IMAGE_HDU";
    else if (hdutyp == BINARY_TBL) return "BINARY_TBL";
    else if (hdutyp == ASCII_TBL) return "ASCII_TBL";
    else return "Unknown";
  }
}

/*-- Methode --*/
int FitsInOutFile::MoveAbsToHDU(int hdunum)
{
  int status = 0;
  int hdutyp = 0;
  fits_movabs_hdu(FitsPtr() , hdunum, &hdutyp, &status);
  FitsCheckStatus(status, "FitsInOutFile::MoveAbsToHDU Error: ");
  return hdutyp;
}

/*-- Methode --*/
int FitsInOutFile::MoveRelToHDU(int hdunum)
{
  int status = 0;
  int hdutyp = 0;
  fits_movrel_hdu(FitsPtr() , hdunum, &hdutyp, &status);
  FitsCheckStatus(status, "FitsInOutFile::MoveRelToHDU Error: ");
  return hdutyp;
}

/*-- Methode --*/
int FitsInOutFile::MoveToNextHDU()
{
  if (CurrentHDU() < NbHDUs()) 
    return MoveRelToHDU(1);
  else return -1;
}

/*-- Methode --*/
/*! 
  Skip HDU 1 if NAXIS=0 (no data), on a file opened for reading.
  return true if moved to HDU No 2
*/
bool FitsInOutFile::SkipEmptyFirstHDU()
{
  if (fptr_ == NULL) return false;
  if (mode_ == Fits_Create) return false;
  if (CurrentHDU() != 1) return false;
  int naxis = 2;
  LONGLONG naxes[2];
  GetImageHDUInfo(naxis, naxes);
  if ((naxis == 0) &&  (NbHDUs() > 1)) {
    MoveRelToHDU(1);
    return true; 
  }
  return false; 
}

/*-- Methode --*/
void FitsInOutFile::CreateImageHDU(int bitpix, int naxis, LONGLONG* naxes)
{
  int status = 0;
  fits_create_imgll(fptr_, bitpix, naxis, naxes, &status);
  FitsCheckStatus(status,"FitsInOutFile::CreateImageHDU() Error: ");
  return;
}

/*-- Methode --*/
 /*!
   See cfitsio function fits_get_img_param() for more information 
   naxis : input=max naxes dimension / out=image dimension 
   Rc : return the image type (bitpix) 
 */
int FitsInOutFile::GetImageHDUInfo(int& naxis, LONGLONG* naxes) const
{
  int status = 0;
  int maxdim = naxis;
  int bitpix = 0;
  fits_get_img_paramll(fptr_, maxdim, &bitpix, &naxis, naxes, &status);
  FitsCheckStatus(status, "FitsInOutFile::GetImageHDUInfo() Error: ");
  return bitpix;
}
/*-- Methode --*/
 /*!
   See cfitsio function fits_set_bscale() for more information 
   Changes the BSCALE/BZERO values for read/write operations on the 
   primary HDU, without changing the keyword values in the fits header
 */
void FitsInOutFile::SetBScaleBZero(double bscale, double bzero)
{
  int status = 0;
  fits_set_bscale(fptr_, bscale, bzero, &status);
  FitsCheckStatus(status, "FitsInOutFile::SetBScaleBZero() Error: ");
}
/*-- Methode --*/
LONGLONG FitsInOutFile::GetNbRows()  const
{
  int status = 0;
  LONGLONG nbrow = 0;
  fits_get_num_rowsll(FitsPtr() , &nbrow, &status);
  FitsCheckStatus(status, "FitsInOutFile::GetNbRows() Error: " );
  return nbrow;
}

/*-- Methode --*/
int FitsInOutFile::GetNbCols()  const
{
  int status = 0;
  int nbcol = 0;
  fits_get_num_cols(FitsPtr() , &nbcol, &status);
  FitsCheckStatus(status, "FitsInOutFile::GetNbCols() Error: ");
  return nbcol;
}

/*-- Methode --*/
/*!
  Create a binary or ascii table - See cfitsio routine fits_create_tbl for more information.
  \param extname : extension name. NextExtensionName() will be used if extname == NULL or extname == "".
  \param ncols : Number of columns
  \param ttype : Column names 
  \param tform : Column data types J / V / K / E / D ...
  \param tunit : Column units
*/
void FitsInOutFile::CreateTable(int tbltyp, const char * extname, int ncols, 
				char * ttype[], char * tform[], 
				char * tunit[], long ininr)
{
  int status = 0;

  char * extn;
  if ( (extname != NULL) && (extname[0] != '\0') )  extn = const_cast<char *>(extname);
  else extn = const_cast<char *>(next_extname_.c_str());

  fits_create_tbl(FitsPtr(), tbltyp, ininr, ncols, ttype, 
		  tform, tunit, extn, &status);
  next_extname_ = "";
  FitsCheckStatus(status, "FitsInOutFile::CreateTable() Error: ");
  return;
}

/*-- Methode --*/
/*!
  Create a binary or ascii table - See cfitsio routine fits_create_tbl for more information.
  number of columns specified by colnames.size()
  \param extname : extension name
  \param colnames : Column names 
  \param tform : Column data types J / V / K / E / D ...
  \param tunit : Column units
*/
void FitsInOutFile::CreateTable(int tbltyp, const string & extname, 
				  const vector<string> & colnames, 
				  const vector<string> & tform, 
				  const vector<string> & tunit, 
				  long ininr)
{
  if ( (colnames.size() != tform.size() ) || 
       (colnames.size() != tunit.size() ) )
    throw SzMismatchError("FitsInOutFile::CreateTable(): different sizes for colnames,tform,tunit");
  
  // On utilise les SegDataBlock<T> pour eviter d'avoir a gerer les new/delete
  // en plus avec les exceptions ...
  size_t kk;
  int ncols = colnames.size();

  SegDataBlock<const char *> colnm(colnames.size(), 1);
  for(kk=0; kk<colnames.size(); kk++) colnm[kk] = colnames[kk].c_str();
  SegDataBlock<const char *> tfm(tform.size(), 1);
  for(kk=0; kk<tform.size(); kk++) tfm[kk] = tform[kk].c_str();
  SegDataBlock<const char *> tun(tunit.size(), 1);
  for(kk=0; kk<tunit.size(); kk++) tun[kk] = tunit[kk].c_str();
       
  CreateTable(tbltyp, const_cast<char *>(extname.c_str()), ncols, 
	      const_cast<char **>(colnm.GetSegment(0)), 
	      const_cast<char **>(tfm.GetSegment(0)), 
	      const_cast<char **>(tun.GetSegment(0)), 
	      ininr);
}

/*-- Methode --*/
/*!
  Return number of columns in table (See fits_get_colname and fits_get_coltype for more information)
  \param colnames : Column names
  \param coltypes : Column data types ( TSTRING / TSHORT / TFLOAT / ... )
  \param repcnt : Repeat count (for columns with vector data)
  \param width : The width (in bytes) of a single element in a column 
  
*/
long FitsInOutFile::GetColInfo(vector<string> & colnames, 
				 vector<int> & coltypes,
				 vector<LONGLONG> & repcnt,
				 vector<LONGLONG> & width)
{

  int status = 0;

  colnames.clear();
  coltypes.clear();
  width.clear();
  repcnt.clear();

  int colnum, typecode;
  LONGLONG repeat, colw;   // $CHECK$ LONGLONG ???
  //unused: int ncols = 0;
  char colname[128];  // longueur max d'un nom de colonne

  while (status != COL_NOT_FOUND) {
    char dum[2]  = {'*','\0'};
    fits_get_colname(FitsPtr(), CASEINSEN, dum, colname, &colnum, &status);
    if (status == COL_NOT_FOUND)  break;
    if ( (status != COL_NOT_UNIQUE) && (status != 0) ) {
      char buff[32];
      fits_get_errstatus(status, buff);
      string msg = "FitsInOutFile::GetColInfo() Error(1): " ;
      msg += buff;
      throw FitsIOException(msg);
    }
    int sta2 = 0;
    fits_get_coltypell(FitsPtr(), colnum, &typecode, &repeat, &colw, &sta2); // $CHECK$ LONGLONG ???fits_get_coltypell
    //DBG    cout << "*DBG*GetColInfo " << colnum << "," << colname << " ," << typecode << " ," << repeat << "," << colw << endl;
    FitsCheckStatus(sta2, "FitsInOutFile::GetColInfo() Error(2): ");

    colnames.push_back(colname);
    coltypes.push_back(typecode);
    repcnt.push_back(repeat);
    width.push_back(colw);
    if (status == 0)  break;
  }
  return colnames.size();
}

/*-- Methode --*/
void FitsInOutFile::InsertColumn(int numcol, const char* colname, const char* fmt)
{
  int status = 0;
  fits_insert_col(FitsPtr(), numcol, const_cast<char *>(colname), 
		  const_cast<char *>(fmt), &status);
  FitsCheckStatus(status, "FitsInOutFile::AddColumn() Error: ");

  return;
}

/*-- Methode --*/
/*!
  Return the value associated to the keyword \b key in the header as a string.
  If the keyword is not found in the fits header, an empty string is returned 
  and the \b nosk flag is set to true.
*/
string FitsInOutFile::KeyValue(string const & key, bool& nosk)
{
  nosk = false;
  int status = 0;
  char value[FLEN_VALUE], comm[FLEN_COMMENT];
  fits_read_key(FitsPtr(), TSTRING, const_cast<char *>(key.c_str()), value, comm, &status);
  if (status == KEY_NO_EXIST) {
    nosk = true;
    return "";
  }
  FitsCheckStatus(status, "FitsInOutFile::KeyValue() Error: ");
  return value;
}

/*-- Methode --*/
/*!
  Read the current fits header information as pairs of '(keyword,value)' appended
  to the DVList object \b dvl.
  \param dvl : DVList object containing filled with (keyword,value) pairs.
  \param stripkw : if true (default), remove leading and trailing spaces from keyword
  \param keepstkey : if true , keep keys of type TYP_STRUC_KEY 
*/
int FitsInOutFile::GetHeaderRecords(DVList& dvl, bool stripkw, bool keepstkey)
{
  int status = 0;
  int nkeys = 0;
  fits_get_hdrspace(FitsPtr(), &nkeys, NULL, &status);
  FitsCheckStatus(status, "FitsInOutFile::GetHeaderRecords() Error(1): ");
 
  char record[FLEN_CARD], value[FLEN_VALUE], comm[FLEN_COMMENT];
  string comment;
  int nok = 0;
  for(int kk=1; kk<=nkeys; kk++) {
    status = 0;
    fits_read_record(FitsPtr(), kk, record, &status);
    FitsCheckStatus(status, "FitsInOutFile::GetHeaderRecords() Error(2): ");
    int kclas = fits_get_keyclass(record);
    if ( (!keepstkey && (kclas ==  TYP_STRUC_KEY))
	 || (kclas == TYP_NULL_KEY) ) continue;
    int len = 0;
    status = 0;
    fits_get_keyname(record, value, &len, &status);
    if (status) continue;
    if (stripkw) strip(value, 'B', ' ');
    string keyname = value;
    status = 0;
    fits_parse_value(record, value, comm, &status);
    if (status) continue;
    if (kclas == TYP_COMM_KEY) {
      if (comment.length() > 0) comment += '\n';
      comment += value;
      continue;
    } 
    char ktyp;
    status = 0;
    complex<r_8> z;
    fits_get_keytype(value, &ktyp, &status);
    if (status) continue;
    switch (ktyp) {
    case 'C' :   // Chaine de caracteres 
      // Il faut enlever les quote ' ' de debut et de fin
      if ((strlen(value)>2)&&(value[0]=='\'')&&(value[strlen(value)-1])) {
	value[strlen(value)-1]='\0';
	dvl.SetS(keyname, value+1);
      }
      else dvl.SetS(keyname, value);
      break;
    case 'L' :   // valeurs logiques  TRUE / FALSE 
      dvl.SetS(keyname, value);
      break;
    case 'I' :   // Entiers 
      dvl.SetI(keyname, (int_8)atoll(value));
      break;
    case 'F' :   // Reels,  
      dvl.SetD(keyname, atof(value));
      break;
    case 'X' :   // Complexe
      dvl.SetZ(keyname, MuTyV(value).Convert(z));
    }
    if (strlen(comm) > 0) {
      string scom = comm;
      dvl.SetComment(keyname, scom);
    }
    nok++;
  }
  if (comment.length() > 0)  dvl.Comment() = comment; 
  return nok;
}

/*-- Methode --*/
void FitsInOutFile::WriteKey(const char * kname, MuTyV const & mtv, 
			     const char *comment)
{
  CheckFitsPtr(FitsPtr()); 
  char keyname[FLEN_KEYWORD], comm[FLEN_COMMENT], sval[FLEN_VALUE];
  LONGLONG lval;
  double dval;
  complex<double> zval;
  string s;
  
  keyname[0] = '\0';
  strncpy(keyname, kname, FLEN_KEYWORD); keyname[FLEN_KEYWORD-1] = '\0';
  comm[0] = '\0';
  /*G.Barrand : if (comm != NULL) */
    strncpy(comm, comment, FLEN_COMMENT); comm[FLEN_COMMENT-1] = '\0';
  int status = 0;
  switch (mtv.Type()) {
  case MuTyV::MTVInteger :
    lval = mtv.GetIntPart();
    fits_write_key(FitsPtr(), TLONGLONG, keyname, &lval, comm, &status);
    break;
  case MuTyV::MTVFloat : 
    dval = mtv.GetRealPart();
    fits_write_key(FitsPtr(), TDOUBLE, keyname, &dval, comm, &status);
    break;
  case MuTyV::MTVComplex : 
    zval = complex<double>(mtv.GetRealPart(),mtv.GetImagPart());
    fits_write_key(FitsPtr(), TDBLCOMPLEX, keyname, &zval, comm, &status);
    break;
  case MuTyV::MTVString : 
    strncpy(sval, mtv.GetStringPointer()->c_str(), FLEN_VALUE);  
    keyname[FLEN_VALUE-1] = '\0';
    fits_write_key(FitsPtr(), TSTRING, keyname, sval, comm, &status);
    break;
  case MuTyV::MTVTimeStamp : 
  default :
    mtv.Convert(s);
    strncpy(sval, s.c_str(), FLEN_VALUE);  
    keyname[FLEN_VALUE-1] = '\0';
    fits_write_key(FitsPtr(), TSTRING, keyname, sval, comm, &status);
    break;
  }
  FitsCheckStatus(status, "FitsInOutFile::WriteKey() Error: ");
  return;
}

/*-- Methode --*/
int FitsInOutFile::WriteHeaderRecords(DVList & dvl)
{

  CheckFitsPtr(FitsPtr()); 
  //unused: int status = 0;
  DVList::ValList::const_iterator it;
  for(it = dvl.Begin(); it != dvl.End(); it++)  
    WriteKey( (*it).first, (*it).second.elval, (*it).second.elcomm);
  // Ecriture commentaires
  return 0;

}

/*-- Methode --*/
void FitsInOutFile::Print(ostream& os, int lev) const
{
  string mode;
  if (mode_ == Fits_Create)  mode = "Create";
  else if (mode_ == Fits_RO)  mode = "ReadOnly";
  else mode = "ReadWrite";
  os << " FitsInOutFile(FileName= " << fname_  << " Mode=" 
     << mode  << ") fitsioVers= " << cfitsioVersion() << endl;
  os << " TotalNumberHDU= " << NbHDUs() << " CurrentHDU: Num= " 
     << CurrentHDU() << " Type= " << CurrentHDUTypeStr() << endl;

  return;
}


