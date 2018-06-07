#include "sopnamsp.h"
#include "machdefs.h"
#include <stdlib.h>
#include <string.h>
#include "fitsfile.h"
#include "pexceptions.h"
#include "strutil.h"
#include "anydataobj.h" 

/*!
   \defgroup FitsIOServer FitsIOServer  module
   This module contains classes which handle FITS format I/O for 
   SOPHYA objects. This module uses cfitsio library.
*/

void BnTblLine::setFormat(int dc, int fc, int ic,  int lc, int bc,int cc, vector<string> names)
   {
    int nbcols = dc + fc + ic + cc + lc + bc;
    int maxName = names.size();
    if (nbcols != maxName)
      {
	cout << " WARNING: BnTblLine:: length of vector of column names not equal to total number of columns" << endl;
	maxName = nbcols < maxName ? nbcols : maxName;
      }
    ColName_ = vector<string>(nbcols);
    int k;
    for (k=0; k < maxName; k++) ColName_[k] = names[k];
    if (dc >0) ddata_ = vector<double>(dc);
    if (fc >0) fdata_ = vector<float>(fc);
    if (ic >0) idata_ = vector<int>(ic);
    if (lc >0) ldata_ = vector<long>(lc);
    if (bc >0) bdata_ = vector<unsigned char>(bc);
    if (cc >0) cdata_ = vector<string>(cc);
   }

bool BnTblLine::sameFormat(const BnTblLine& btl) const
   {
     if (btl.ddata_.size() == ddata_.size() && btl.fdata_.size() == fdata_.size() && btl.idata_.size() == idata_.size() && btl.cdata_.size() == cdata_.size() &&  btl.ldata_.size() == ldata_.size() &&  btl.bdata_.size() == bdata_.size()) return true;
     else return false;
   }

void BnTblLine::Print()
   {
     int k;
     cout << " ********* ligne ************* " << endl;
     cout << " *** noms de variables  " << endl;
     for (k=0; k < ColName_.size(); k++) cout << ColName_[k] << " ";
     cout << endl;
     cout << " *** variables doubles  " << endl;
     for (k=0; k < ddata_.size(); k++) cout << ddata_[k] << " ";
     cout << endl;
     cout << " *** variables float  " << endl;
     for (k=0; k < fdata_.size(); k++) cout << fdata_[k] << " ";
     cout << endl;
     cout << " *** variables int  " << endl;
     for (k=0; k < idata_.size(); k++) cout << idata_[k] << " ";
     cout << endl;
     cout << " *** variables string  " << endl;
     for (k=0; k < cdata_.size(); k++) cout << cdata_[k] << " ";
     cout << endl;
     cout << " *** variables long  " << endl;
     for (k=0; k < ldata_.size(); k++) cout << ldata_[k] << " ";
     cout << endl;
     cout << " *** variables byte  " << endl;
     for (k=0; k < bdata_.size(); k++) cout << (int)bdata_[k] << " ";
     cout << endl;
     cout << " ***************************** " << endl;
   }

FitsFile::BufferLine::BufferLine(const vector<FitsFile::FitsDataType>& types)
{


  int dc=0;
  int fc=0;
  int shc=0;
  int ic=0;
  int lc=0;
  int cc=0;
  int bc=0;
  id_  = vector< pair<FitsFile::FitsDataType, int> >(types.size());
  int k;
  for (k= 0; k < types.size(); k++)
    {
      switch( types[k] )
	{
	case FitsFile::FitsDataType_double :
	  {
            id_[k] = pair<FitsFile::FitsDataType, int>(FitsFile::FitsDataType_double, dc);
	    dc++;
	    break;
	  }
	case FitsFile::FitsDataType_float :
	  {
            id_[k] = pair<FitsFile::FitsDataType, int>(FitsFile::FitsDataType_float, fc);
	    fc++;
	    break;
	  }
	case  FitsFile::FitsDataType_short :
	  {
            id_[k] = pair<FitsFile::FitsDataType, int>(FitsDataType_short, shc);
	    shc++;
	    break;
	  }
	case  FitsFile::FitsDataType_int :
	  {
            id_[k] = pair<FitsFile::FitsDataType, int>(FitsDataType_int, ic);
	    ic++;
	    break;
	  }
	case  FitsFile::FitsDataType_long :
	  {
            id_[k] = pair<FitsFile::FitsDataType, int>(FitsFile::FitsDataType_long, lc);
	    lc++;
	    break;
	  }
	case  FitsFile::FitsDataType_byte :
	  {
            id_[k] = pair<FitsFile::FitsDataType, int>(FitsFile::FitsDataType_byte, bc);
	    bc++;
	    break;
	  }
	case FitsDataType_char :
	  {
            id_[k] = pair<FitsFile::FitsDataType, int>(FitsFile::FitsDataType_char, cc);
	    cc++;
	    break;
	  }
	default:
	  {
	    throw PException(" FitsFile::getHeaderWithSophyaObject() : unsupported FITS data type");
	  }
	}
    }

  if (dc >0) ddata_ = vector<r_8>(dc);
  if (fc >0) fdata_ = vector<r_4>(fc);
  if (shc >0) shdata_ = vector<int_2>(shc);
  if (ic >0) idata_ = vector<int_4>(ic);
  if (lc >0) ldata_ = vector<int_8>(lc);
  if (cc >0) cdata_ = vector<string>(cc);
  if (bc >0) bdata_ = vector<unsigned char>(bc);
}


void FitsFile::BufferLine::Print() const
{
  cout << " impression de la ligne: " << endl;

  cout << " doubles : " << endl;
  int k;
  for (k=0; k< ddata_.size(); k++)
    {
      cout << ddata_[k] << " " ;
    } 
  cout << endl;

  cout << " floats : " << endl;
  for (k=0; k< fdata_.size(); k++)
    {
      cout << fdata_[k] << " " ;
    } 
  cout << endl;

  cout << " entiers courts: " << endl;
  for (k=0; k< shdata_.size(); k++)
    {
      cout << shdata_[k] << " " ;
    } 
  cout << endl;
  cout << " entiers : " << endl;
  for (k=0; k< idata_.size(); k++)
    {
      cout << idata_[k] << " " ;
    } 
  cout << endl;

  cout << " entiers longs : " << endl;
  for (k=0; k< ldata_.size(); k++)
    {
      cout << ldata_[k] << " " ;
    } 
  cout << endl;

  cout << " chaines carac.  : " << endl;
  for (k=0; k< cdata_.size(); k++)
    {
      cout << cdata_[k] << " " ;
    } 
  cout << endl;

  cout << " bytes : " << endl;
  for (k=0; k< bdata_.size(); k++)
    {
      cout << (char)bdata_[k] << " " ;
    } 
  cout << endl;


}

/*!
  \class SOPHYA::FitsIOHandler
  \ingroup FitsIOServer
The class structure is analogous to Sophya-PPersist system :
Each SOPHYA object XXX is associated with a object of class FITS_XXX 
 (inheriting from FitsFileHandler), to which input/output operations with FITS 
 files are delegated (through a class Hierarchy : FitsFile (virtual), 
 FitsInFile, FitsOutFile) . A typical example of use is the following : 

\verbatim
  int m=... ;
  SphereHEALPix<r_8> sphere1(m);           // definition of the SOPHYA object
  .... fill the sphere ....

  FITS_SphereHEALPix<r_8> fits_sph1(sphere1); 
                                           // delegated object
  fits_sph.Write("myfile.fits");           // writing on FITS file

   FITS_SphereHEALPix<r_8> fits_sph2("myfile.fits"); 
                                           // load a delegated object 
                                           // from FITS file
   SphereHEALPix<r_8> sphere2=(SphereHEALPix<r_8>)fits_sph2; 
                                           // casting the delegated object
                                           // into a SOPHYA object
\endverbatim
  

*/

/*! \fn void  SOPHYA::FitsIOHandler::Read(const char flnm[],int hdunum)

this method is called from inherited objects : 

opens a file 'flnm' 

gets parameters in extension-header (hdunum)

calls the method 'ReadFromFits' from the inherited  object
*/ 
void   FitsIOHandler::Read(const char flnm[],int hdunum)
{
  FitsInFile ifts(flnm);
  Read(ifts, hdunum);
}

void FitsIOHandler::Read(FitsInOutFile& is) 
{
  FitsInFile fis(is);
  fis.ReadHeader(fis.CurrentHDU());
  ReadFromFits(fis);
}

  /*! \fn void SOPHYA::FitsIOHandler::Read(FitsInFile& is, int hdunum)
Read the data on extension hdunum (or primary header, if hdunum=1) from FitsInFIle. If hdunum is not addressed, , one reads the next extension, with respect to the current position. 
   */
void FitsIOHandler::Read(FitsInFile& is, int hdunum) 
{
  is.ReadHeader(hdunum);
  ReadFromFits(is);
}


/*! \fn void SOPHYA::FitsIOHandler::Write(char flnm[])
this method is called from inherited objects. 

for writing a new object in a new fits-extension :

\warning By convention, primary header may contain fits-image data.
For switching off this convention (i.e. to make sure that all data will be on fits-extensions) use the method : 

firstImageOnPrimaryHeader() (see below)

calls the method 'WriteToFits' from the inherited  object

*/ 
void FitsIOHandler::Write(const char flnm[]) 

{
  FitsOutFile of(flnm, FitsFile::unknown);
  WriteToFits(of);
}

void FitsIOHandler::Write(FitsInOutFile& os) 
{
  FitsOutFile fos(os);
  WriteToFits(fos);
}


FitsFile::FitsFile()
  : FitsInOutFile()
{
  InitNull();
}

FitsFile::FitsFile(FitsInOutFile const& fios)
  : FitsInOutFile(fios)
{
  InitNull();
  hdunum_ = fios.CurrentHDU();
}


/*RzDel 
FitsFile::~FitsFile()
{
  //  Close fait par le destructeur de FitsInOutFile - Reza , Dec 2005
  //  int status = 0;
  //  if( fptr_ != NULL)
  //    fits_close_file(fptr_,&status);
  // if( status )  printerror( status );
}
*/


void FitsFile::printerror(int &status) 
  //*****************************************************/
  //* Print out cfitsio error messages and exit program */
  //*****************************************************/
{
  if( status )
    {
      fits_report_error(stderr,status);
      throw IOExc("FitsFile:: error FITSIO status");
    }
  return;
}

void FitsFile::printerror(int& status, const char* texte) 
  //*****************************************************/
  //* Print out cfitsio error messages and exit program */
  //*****************************************************/
{
  // print out cfitsio error messages and exit program 
  // print error report  
     fits_report_error(stderr, status);
  cout << " erreur:: " << texte << endl;
  throw IOExc("FitsFile:: error FITSIO status");
}
void FitsFile::printerrorAndContinue(int& status, const char* texte) 
  //*****************************************************/
  //* Print out cfitsio error messages and exit program */
  //*****************************************************/
{
  // print out cfitsio error messages and exit program 
  // print error report  
     fits_report_error(stderr, status);
  cout << " erreur:: " << texte << endl;
  //  throw IOExc("FitsFile:: error FITSIO status");
}

void FitsFile::ResetStatus(int& status) 
{
  fits_status_ = status;
  status = 0;
  fits_clear_errmsg();
}

string FitsFile::GetErrStatus(int status) 
{
  char text[31];
  fits_get_errstatus(status, text);
  return string(text);
}

/*!
  \class SOPHYA::FitsInFile
  \ingroup FitsIOServer
class for reading  SOPHYA objects from FITS Format Files (uses cfitsio lib)
*/

FitsInFile::FitsInFile()
  : FitsFile()

{
  InitNull();
}

FitsInFile::FitsInFile(string const & flnm)
  : FitsFile()
{
  InitNull();
  Open(flnm.c_str(), Fits_RO);
  /*RZDEL
   int status = 0;
   fits_open_file(&fptr_,flnm.c_str(),READONLY,&status);
   if( status ) printerror( status );
  */
}

FitsInFile::FitsInFile(const char * flnm)
  : FitsFile()
{
  InitNull();
  Open(flnm, Fits_RO);
  /*RZDEL
  int status = 0;
   fits_open_file(&fptr_,flnm,READONLY,&status);
   if( status ) printerror( status );
  */
}

FitsInFile::FitsInFile(FitsInOutFile const& fios)
  : FitsFile(fios)

{
  InitNull();
  if (mode_ == Fits_Create)
    throw FitsIOException("FitsInFile::FitsInFile(FitsInOutFile const& fios) newly created fits file");
}


void FitsInFile::InitNull()
{
  imageDataType_ = FitsDataType_NULL;
  naxis_ = 0;
  nbData_ = 0;
  nrows_ = 0;
  nbcols_ = 0;
  naxisn_.clear();
  repeat_.clear();
  noms_.clear();
  taille_des_chaines_.clear();
  dvl_.Clear();

  dnull_ = 1.e-300;
  fnull_ = 1.e-33;
  inull_= 99999;
  cnull_= string("xxx");
  

}

//////////////////////////////////////////////////////////
//     methods with general purpose
/////////////////////////////////////////////////////////

int FitsInFile::NbBlocks(char flnm[])
{
  int status = 0;
  int nbhdu = 0; 
  fitsfile* fileptr; 
  fits_open_file(&fileptr,flnm,READONLY,&status);
  if( status ) printerror( status, "NbBlocks: erreur ouverture fichier" );
  fits_get_num_hdus(fileptr, &nbhdu, &status); 
  fits_close_file(fileptr,&status);
  return nbhdu;
}
int FitsInFile::NbBlocks()
{
  int status = 0;
  int nbhdu = 0; 
  fits_get_num_hdus(fptr_, &nbhdu, &status); 
  return nbhdu;
}

void FitsInFile::GetBlockType(char flnm[], int hdunum, FitsExtensionType& typeOfExtension, int& naxis, vector<int>& naxisn, FitsDataType& dataType, DVList& dvl )
{
  int status = 0;
  fitsfile* fileptr; 
  fits_open_file(&fileptr,flnm,READONLY,&status);
  if( status ) printerror( status, "GetBlockType: erreur ouverture fichier" );
  // move to the specified HDU number
  int hdutype = 0;
  fits_movabs_hdu(fileptr,hdunum,&hdutype,&status);
  if( status ) printerror( status,"GetBlockType: erreur movabs");
  if(hdutype == IMAGE_HDU) 
    {
      typeOfExtension = FitsExtensionType_IMAGE;
      GetImageParameters (fileptr, dataType, naxis, naxisn);
    }
  else
    if(hdutype == ASCII_TBL  || hdutype == BINARY_TBL) 
      {
	int nrows = 0;
	vector<string> noms; 
	vector<FitsDataType> types;   
	vector<int>  taille_des_chaines;
	GetBinTabParameters(fileptr, naxis, nrows, naxisn, noms, types, taille_des_chaines); 
	int k;
	for (k=0; k< naxisn.size(); k++) naxisn[k] *= nrows;
	if(hdutype == ASCII_TBL)
	  {
	    typeOfExtension = FitsExtensionType_ASCII_TBL; 
	    dataType = FitsDataType_ASCII;
	  }
	else
	  {
	    typeOfExtension = FitsExtensionType_BINARY_TBL;
	    dataType = types[0];
	  }
      }
    else
      {
	cout << " hdutype= " << hdutype << endl;
	throw IOExc("FitsFile::GetBlockType: this HDU type is unknown");
      }
  
  KeywordsIntoDVList(fileptr, dvl, hdunum);
  fits_close_file(fileptr,&status);
}


void FitsInFile::ReadHeader(int hdunum) 
{
  //  InitNull();
  int status = 0;
  if (hdunum<0) 
    {
      throw PException(" FitsInFile::ReadHeader : hdu number must be not negative");
    }
  if (hdunum != 0 ) hdunum_ = hdunum;

  // si le numero de header non precise
  else
    {
      // si c'est le premier objet a lire  
      if (hdunum_ == 0)
	{
	  // on calcule le numero de header a lire
	  if (imageOnPrimary_ == true ) hdunum_ = 1;
	  else hdunum_ = 2;
	}
      // sinon objet suivant  
      else hdunum_++;
    }
  getHeaderWithSophyaObject();
  if ( hdutype_ == FitsExtensionType_NULL )
    {
      if (hdunum == 0 && hdunum_ == 1)
	{
	  hdunum_++;
	  getHeaderWithSophyaObject();
	}
      else 
	{
	  cout << " WARNING (FitsInFile::ReadHeader) : no SOPHYA object on HDU number : " << hdunum_ << endl;
	}
    }
  if ( hdutype_ == FitsExtensionType_EOF )
    {
	throw PException("FitsFile::ReadHeader, attempt to read through EOF");
    }
}

string FitsInFile::getStringKeyword(int hdunum, string keyw, int& retStatus)
{
  string s;
  retStatus = 0;
  int status = 0;
  if (hdunum != hdunum_ )
    {
      int hdutype;
      fits_movabs_hdu(fptr_,hdunum,&hdutype,&status);
    }
  
  char value[FLEN_VALUE];
  char* keyname= const_cast<char*>(keyw.c_str());
  fits_read_key_str(fptr_,keyname,value,NULL,&status);
  if (status == 0)
    s = string(value);
  else retStatus = status;
  if (hdunum != hdunum_ )
    {
      int hdutype;
      if (hdunum_ != 0)
	fits_movabs_hdu(fptr_,hdunum_,&hdutype,&status);
      else 	     fits_movabs_hdu(fptr_,1,&hdutype,&status);
      
    }
  return s;
}
bool FitsInFile::hasKeyword(int hdunum, string keyw)
     {
       bool has=false;
       int status = 0;
       if (hdunum != hdunum_ )
	 {
	   int hdutype;
	   fits_movabs_hdu(fptr_,hdunum,&hdutype,&status);
	 }

       char value[FLEN_VALUE];
       char* keyname= const_cast<char*>(keyw.c_str());
       fits_read_keyword(fptr_,keyname,value,NULL,&status);
       if (status == 0)
	 has = true;
       else 
	 if (status == KEY_NO_EXIST ) status =0;
	 else fits_report_error(stderr,status); 
       if (hdunum != hdunum_ )
	 {
	   int hdutype;
	   if (hdunum_ != 0)
	     fits_movabs_hdu(fptr_,hdunum_,&hdutype,&status);
	   else 	     fits_movabs_hdu(fptr_,1,&hdutype,&status);

	 }
       return has;
     }

void FitsInFile::GetKeywordsFromHeader (int hdunum, list<FitsKeyword>& mots_cles) const
{
  int status = 0;
  int hdutype;
  fits_movabs_hdu(fptr_,hdunum,&hdutype,&status);
  if( status ) fits_report_error(stderr,status);

  // get number of keywords
  int nkeys,keypos;
  fits_get_hdrpos(fptr_, &nkeys, &keypos,&status);
  if( status ) fits_report_error(stderr,status);
  // shift with the number of mandatory keywords
  int num= 0;
  // if primary header
    if (hdunum == 1) 
      {
	// read NAXIS
	int naxis=0;
	fits_read_key(fptr_,TINT,"NAXIS",&naxis,NULL,&status);
	// number of mandatory keywords
	num = naxis+3;
      }
    // extensions
    else
      {
	if (hdutype == IMAGE_HDU)
	  {
	    // read NAXIS
	    int naxis=0;
	    fits_read_key(fptr_,TINT,"NAXIS",&naxis,NULL,&status);
	    // number of mandatory keywords
	    num = naxis+5;
	  }
	else
	  if(hdutype == ASCII_TBL || hdutype == BINARY_TBL)
	    {
	      // number of mandatory keywords
	      num = 8;
	    }
      }
  int j;
  char keyname[LEN_KEYWORD];  
  char value[FLEN_VALUE];
  char comment[FLEN_COMMENT];
  for(j = num+1; j <= nkeys; j++)
    {
      char dtype;
      fits_read_keyn(fptr_,j,keyname,value,comment,&status);
      if(status) 
	{
	  fits_report_error(stderr,status);
	  status=0;
	}
      string  kn(keyname);
      string  cm(comment);
      string  val(value);
      FitsKeyword kw(kn, val, cm);
      mots_cles.push_back(kw);
    }
  if (hdunum_ > 0) fits_movabs_hdu(fptr_,hdunum_,&hdutype,&status);

}
void FitsInFile::GetImageParameters (fitsfile* fileptr,FitsDataType& dataType,int& naxis,vector<int>& naxisn)
{
  int hdunum=0;
  //    cout << " Reading a FITS image in HDU : " << fits_get_hdu_num(fileptr,&hdunum)  << endl;
  int status= 0;

  // bits per pixels
  int bitpix=0;
  fits_read_key(fileptr,TINT,"BITPIX",&bitpix,NULL,&status);
  if( status )  printerror( status );
  if(bitpix == DOUBLE_IMG) dataType = FitsDataType_double;
  else if(bitpix == FLOAT_IMG) dataType = FitsDataType_float;
  else if(bitpix == LONG_IMG || bitpix == SHORT_IMG ) dataType = FitsDataType_int;
  else if (bitpix == BYTE_IMG) dataType = FitsDataType_char;
  else 
    {
      cout << " bitpix= " << bitpix << endl;
      throw PException(" FitsFile::GetImageParameters : unsupported FITS data type");
    }

  // number of dimensions in the FITS array
  naxis= 0;
  fits_read_key(fileptr,TINT,"NAXIS",&naxis,NULL,&status);
  if( status ) printerror( status );
  // read the NAXISn keywords to get image size
  long* naxes = new long[naxis] ;
  int nfound;
  fits_read_keys_lng(fileptr,"NAXIS",1,naxis,naxes,&nfound,&status);
  if( status ) printerror( status );
  if (nfound != naxis ) 
    cout << " WARNING : " << nfound << " axes found, expected naxis= " << naxis << endl;
  int k;
  for (k=0; k<naxis; k++) 
    {
      naxisn.push_back( (int)naxes[k] );
    }
  delete [] naxes;
}




  /*! \fn DVList SOPHYA::FitsInFile::DVListFromPrimaryHeader() const

   \return the keywords of primary header in a DVList 

*/
DVList  FitsInFile::DVListFromPrimaryHeader() const
   {
     int status;
     DVList dvl;
     KeywordsIntoDVList(fptr_, dvl, 1);
     int hdutype = 0;
     if (hdunum_ > 0) fits_movabs_hdu(fptr_,hdunum_,&hdutype,&status);
     return dvl;
   }

void FitsInFile::getHeaderWithSophyaObject()
{
  // si hdunum_ > 1 lit le header correspondant
  // si hdunum_ = 1 se positionne au (et lit le) premier header qui 
  //                contient reellement un objet 
  int status=0;
  if (hdunum_ < 1)  throw PException(" attempt to read hdunum < 1");
   InitNull();
  if (hdunum_ == 1)
    {
      // presence of image ?
      int naxis= 0;
      fits_read_key(fptr_,TINT,"NAXIS",&naxis,NULL,&status);
      if( status ) printerror( status );
      if (naxis > 0 )       // there is an image
	{
	  hdutype_ = FitsExtensionType_IMAGE;
	  GetImageParameters (fptr_, imageDataType_, naxis_, naxisn_);
	  nbData_ =  1;
	  int k;
	  for (k=0; k<naxis_; k++) if (naxisn_[k] > 0) nbData_ *= naxisn_[k];
	  KeywordsIntoDVList(fptr_, dvl_,hdunum_);
	}
      else
	{
	  hdutype_ = FitsExtensionType_NULL;
	  KeywordsIntoDVList(fptr_, dvl_,hdunum_);
	}
    }
  else
    {
      int hdutype;
      fits_movabs_hdu(fptr_,hdunum_,&hdutype,&status);

      if( status )
	{
	  if (status == END_OF_FILE)
	    {
	      hdutype_= FitsExtensionType_EOF;
	      status =0;
	      return;
	    }
	  else
	    {
	      cout << "WARNING (FitsInFile::getHeaderWithSophyaObject) : error during movabs" << endl;
	      hdutype_= FitsExtensionType_ERROR;
	      status =0;
	      return;
	    }
	  // printerror( status,":FitsInFile::getHeader : erreur movabs");
	}
	

      if(hdutype == IMAGE_HDU) 
	{
	  hdutype_= FitsExtensionType_IMAGE;
	  GetImageParameters (fptr_, imageDataType_, naxis_, naxisn_);
	  nbData_ =  1;
	  int k;
	  for (k=0; k<naxis_; k++) if (naxisn_[k] > 0) nbData_ *= naxisn_[k];
	  KeywordsIntoDVList(fptr_, dvl_,hdunum_);
	}
      else if(hdutype == ASCII_TBL)
	{
	  hdutype_= FitsExtensionType_ASCII_TBL;
	  GetBinTabParameters(fptr_,nbcols_, nrows_,repeat_, noms_, types_, taille_des_chaines_);
	  KeywordsIntoDVList(fptr_, dvl_, hdunum_);
	}
      else if(hdutype == BINARY_TBL)
	{
	  hdutype_= FitsExtensionType_BINARY_TBL;
	  GetBinTabParameters(fptr_,nbcols_, nrows_,repeat_, noms_, types_, taille_des_chaines_);
	  KeywordsIntoDVList(fptr_, dvl_, hdunum_);
	}
      else 
	{
	  hdutype_= FitsExtensionType_NULL;
	  KeywordsIntoDVList(fptr_, dvl_, hdunum_);
	}

    }
  if (hdutype_ == FitsExtensionType_BINARY_TBL || hdutype_ == FitsExtensionType_ASCII_TBL)
    {
      bfl_ = BufferLine(types_);
    }
}


void FitsInFile::moveToFollowingHeader()
{
  int status = 0;
  hdunum_++;
  getHeaderWithSophyaObject();
  if ( hdutype_ == FitsExtensionType_NULL )
    {
      cout << " WARNING (FitsInFile::ReadHeader) : no SOPHYA object on HDU number : " << hdunum_ << endl;
	
    }
}





/*! \fn int  SOPHYA::FitsInFile::NbColsFromFits() const
\return number of columns (return 1 if IMAGE) 
*/
int  FitsInFile::NbColsFromFits() const
{
  if(hdutype_ == FitsExtensionType_BINARY_TBL) return nbcols_;
  else
    if(hdutype_ == FitsExtensionType_ASCII_TBL ||  hdutype_ == FitsExtensionType_IMAGE) return 1;
    else
      {
	cout << " hdutype= " << (int) hdutype_ << endl;
	throw PException("FitsFile::NbColsFromFits, HDU  not supported");
      }
}

/*! \fn  int SOPHYA::FitsInFile::NentriesFromFits(int nocol) const
\return number of data in the current IMAGE extension on FITS file, or number
 of data of column number 'nocol' of the current BINTABLE extension
*/
int FitsInFile::NentriesFromFits(int nocol) const
{
  if(hdutype_ == FitsExtensionType_BINARY_TBL)   return nrows_*repeat_[nocol];
  else
    if(hdutype_ == FitsExtensionType_ASCII_TBL) return nrows_;
    else
      if(hdutype_ == FitsExtensionType_IMAGE) return nbData_;
      else
	{
	  cout << "hdutype= " << (int) hdutype_ << endl;
	  throw PException("FitsFile::NentriesFromFits, this HDU is not supported");
	}
}

/*! \fn char SOPHYA::FitsInFile::ColTypeFromFits(int nocol) const

return a character denoting data type of column number 'nocol' in a BINTABLE :

D : double

E : float 

I : integer
    
S : character string 

  */

FitsFile::FitsDataType FitsInFile::ColTypeFromFits(int nocol) const
{
  if(hdutype_ != FitsExtensionType_ASCII_TBL && hdutype_ != FitsExtensionType_BINARY_TBL)
    {
      throw IOExc("FitsFile::TypeFromFits, this HDU is not an ASCII table nor a binary table");
    }
  return types_[nocol];
}


/*! \fn string SOPHYA::FitsInFile::ColNameFromFits(int nocol) const

\return name of the column number 'nocol' of the current BINTABLE extension
   */

string FitsInFile::ColNameFromFits(int nocol) const
{
  if(hdutype_ != FitsExtensionType_ASCII_TBL && hdutype_ != FitsExtensionType_BINARY_TBL)
    {
      throw IOExc("FitsFile::TypeFromFits, this HDU is not an ASCII table nor a binary table");
    }
  return noms_[nocol];
}

/*! \fn int DSOPHYA::FitsInFile::ColStringLengthFromFits(int nocol) const

 \return number of characters of each data  for the column number 'nocol' (if char* typed) of the current BINTABLE extension
*/

int FitsInFile::ColStringLengthFromFits(int nocol) const
{
  if(hdutype_ != FitsExtensionType_ASCII_TBL && hdutype_ != FitsExtensionType_BINARY_TBL)
    {
      throw IOExc("FitsFile::TypeFromFits, this HDU is not an ASCII table nor a binary table");
    }
  int index=-1;
  int k;
  for (k=0; k<=nocol; k++) 
    {
      if (types_[k] == FitsDataType_char) index++;
    }
  return  taille_des_chaines_[index];
}

const FitsFile::BufferLine&   FitsInFile::GetBufferLine(long NoLine) 
{
  int status= 0;
  int    anynul;
  double dnull= dnull_;
  float fnull= fnull_;
  int inull= inull_;
  char* cnull=  const_cast<char*>(cnull_.c_str());
  int ncol;
  long nels=1;
  double dval;
  float  fval;
  int    ival;

  // pas d'entier de longueur superieure a 32 bits, pour cfitsio
  int    lval;

  unsigned char usval; 
  int rang = 0;
  int ccount = 0;
  for (ncol=0; ncol<nbcols_; ncol++)
    {
      rang =  bfl_.identificateur()[ncol].second;
      //      cout << " fitsfile : relecture col " << ncol << " type " << bfl_.identificateur()[ncol].first << " rang " << rang << endl; 
      switch (bfl_.identificateur()[ncol].first)
	{ 
	case FitsDataType_double :
	  fits_read_col(fptr_,TDOUBLE,ncol+1,NoLine+1,1,1,&dnull, &dval,&anynul,&status);
	  bfl_.r_8Array(rang) = (r_8)dval;
	  break;
	case FitsDataType_float :
	  fits_read_col(fptr_,TFLOAT,ncol+1,NoLine+1,1,1,&fnull,&fval,&anynul,&status);
	  bfl_.r_4Array(rang)  = (r_4)fval;
	  break;
	case FitsDataType_short :
	  fits_read_col(fptr_,TSHORT,ncol+1,NoLine+1,1,1,&inull,&ival, &anynul,&status);
	  bfl_.int_2Array(rang) = (int_2)ival;
	  break;
	case FitsDataType_int :
	  fits_read_col(fptr_,TINT,ncol+1,NoLine+1,1,1,&inull,&ival, &anynul,&status);
	  bfl_.int_4Array(rang) = (int_4)ival;
	  break;
	case FitsDataType_long :
	  fits_read_col(fptr_,TLONG,ncol+1,NoLine+1,1,1,&inull,&lval, &anynul,&status);
	  bfl_.int_8Array(rang) = (int_8)lval;
	  break;
	case FitsDataType_byte :
	  fits_read_col(fptr_,TBYTE,ncol+1,NoLine+1,1,1,&inull, &usval, &anynul,&status);
	  bfl_.u_charArray(rang) = usval;
	  break;
	case FitsDataType_char : //G.Barrand : put the below in {}
         {char* chaine = new char[taille_des_chaines_[ccount++]];
	  fits_read_col(fptr_,TSTRING,ncol+1,NoLine+1,1,1,cnull,&chaine,&anynul,&status);
	  bfl_.stringArray(rang) = string(chaine);
          //G.Barrand : delete [] chaine; ???
          }
	  break;
        //G.Barrand : begin :
        case FitsDataType_NULL :
        case FitsDataType_ASCII :
          cerr << "FitsInFile::GetBufferLine : FitsDataType_[NULL,ASCII] not handled." << endl;
          break;
        //G.Barrand : end
	}
      if (status)
	{
	  ResetStatus(status);
	  break;
	}
    }
  //  cout << " fitsfile : ligne relue " << endl;
  //  bfl_.Print();
  return bfl_;
}



/*! \fn void  SOPHYA::FitsInFile::GetBinTabLine(int NoLine, double* ddata, float* fdata, int* idata, char ** cdata)

Get the NoLine-th 'line'  from the current BINTABLE extension on FITS file,
  */

void  FitsInFile::GetBinTabLine(int NoLine, double* ddata, float* fdata, int* idata, char ** cdata) 
{
  int status= 0;
  int    anynul;
  double dnull= dnull_;
  float fnull= fnull_;
  int inull= inull_;
  char* cnull= const_cast<char*>(cnull_.c_str());
  int dcount = 0;
  int fcount = 0;
  int icount = 0;
  int ccount =0;
  int ncol;
  long nels=1;
  int ligneAsolue = NoLine+1;
  for (ncol=0; ncol<nbcols_; ncol++)
    {
      int repetition =repeat_[ncol]; 
      int ligneALire = ligneAsolue/repetition;
      int premierElement  = ligneAsolue-ligneALire*repetition;
      if (premierElement != 0 )
	{
	  ligneALire++;
	}
      else premierElement = repetition;

      switch (types_[ncol])
	{ 
	case FitsDataType_double :
	  {
	  fits_read_col(fptr_,TDOUBLE,ncol+1,ligneALire,premierElement,1,&dnull,&ddata[dcount++],&anynul,&status);
	    break;
	  }
	case FitsDataType_float :
	  {	  
	    fits_read_col(fptr_,TFLOAT,ncol+1,ligneALire,premierElement,1,&fnull,&fdata[fcount++],&anynul,&status);
	    break;
	  }
	case  FitsDataType_int :
	  {
	    fits_read_col(fptr_,TINT,ncol+1,ligneALire,premierElement,1,&inull,&idata[icount++],
			&anynul,&status);
	    break;
	  }
	case  FitsDataType_long :
	  {
	    fits_read_col(fptr_,TLONG,ncol+1,ligneALire,premierElement,1,&inull,&idata[icount++], &anynul,&status);
	    break;
	  }
	case  FitsDataType_byte :
	  {
	    unsigned char uschar = 0;
	    fits_read_col(fptr_,TBYTE,ncol+1,ligneALire,premierElement,1,&inull,&uschar, &anynul,&status);
	    idata[icount++] = (int)uschar;
	    break;
	  }
	case FitsDataType_char :
	  {
	    fits_read_col(fptr_,TSTRING,ncol+1,ligneALire,premierElement,1,cnull,&cdata[ccount++],&anynul,&status);
	    break;
	  }
	default:
	  {
	    throw PException(" FitsInFile::GetBinTabLine : unsupported FITS data type");
	  }
	}
      if (status)
	{
	  ResetStatus(status);
	  break;
	}
    }
}

/*! \fn void   SOPHYA::FitsInFile::GetBinTabLine(long NoLine,  BnTblLine& ligne)
Get the NoLine-th 'line'  from the current BINTABLE extension on FITS file,
*/
void   FitsInFile::GetBinTabLine(long NoLine,  BnTblLine& ligne) 
{
  int status= 0;
  int    anynul;
  double dnull= dnull_;
  float fnull= fnull_;
  int inull= inull_;
  char* cnull=  const_cast<char*>(cnull_.c_str());
  int dcount = 0;
  int fcount = 0;
  int icount = 0;
  int lcount = 0;
  int bcount = 0;
  int ccount =0;
  int ncol;
  long nels=1;
  for (ncol=0; ncol<nbcols_; ncol++)
    {
      switch (types_[ncol])
	{ 
	case FitsDataType_double :
	  {
	    fits_read_col(fptr_,TDOUBLE,ncol+1,NoLine+1,1,1,&dnull,&ligne.ddata_[dcount++],&anynul,&status);
	    break;
	  }
	case FitsDataType_float :
	  {
	    fits_read_col(fptr_,TFLOAT,ncol+1,NoLine+1,1,1,&fnull,&ligne.fdata_[fcount++],&anynul,&status);
	    break;
	  }
	case FitsDataType_int :
	  {
	    fits_read_col(fptr_,TINT,ncol+1,NoLine+1,1,1,&inull,&ligne.idata_[icount++], &anynul,&status);
	    break;
	  }
	case FitsDataType_long :
	  {
	    fits_read_col(fptr_,TLONG,ncol+1,NoLine+1,1,1,&inull,&ligne.ldata_[lcount++], &anynul,&status);
	    break;
	  }
	case FitsDataType_byte :
	  {
	    fits_read_col(fptr_,TBYTE,ncol+1,NoLine+1,1,1,&inull,&ligne.bdata_[bcount++], &anynul,&status);
	    break;
	  }
	case FitsDataType_char :
	  {
	    char* chaine = new char[taille_des_chaines_[ccount]];
	    fits_read_col(fptr_,TSTRING,ncol+1,NoLine+1,1,1,cnull,&chaine,&anynul,&status);
	    ligne.cdata_[ccount++] = string(chaine);
	    break;
	  }
	default:
	  {
	    throw PException(" FitsInFile::GetBinTabLine : unsupported FITS data type");
	  }
	}
      if (status)
	{
	  ResetStatus(status);
	  break;
	}
    }
}



/*! \fn void SOPHYA::FitsInFile::GetBinTabLine(int NoLine, float* fdata)

Get the NoLine-th float 'line'  from the current BINTABLE extension on FITS file,
*/
void FitsInFile::GetBinTabLine(int NoLine, float* fdata) 
{
  int status= 0;
  int    anynul;
  float fnull= fnull_;
  long nels=1;
  int ncol;
  for (ncol=0; ncol<nbcols_; ncol++)
    {
      fits_read_col(fptr_,TFLOAT,ncol+1,NoLine+1,1,1,&fnull,&fdata[ncol],&anynul,&status);
      if (status)
	{
	  ResetStatus(status);
	  break;
	}
    }
}

/*! 
Get the NoLine-th float 'line'  from the current BINTABLE extension on FITS file,
*/
void FitsInFile::GetBinTabLine(int NoLine, double* ddata) 
{
  int status= 0;
  int    anynul;
  double dnull= fnull_;
  long nels=1;
  int ncol;
  for (ncol=0; ncol<nbcols_; ncol++)
    {
      fits_read_col(fptr_, TDOUBLE, ncol+1,NoLine+1,1,1,&dnull,&ddata[ncol],&anynul,&status);
      if (status)
	{
	  ResetStatus(status);
	  break;
	}
    }
}


/*! \fn void SPOPHYA::FitsInFile::GetBinTabFCol(double* valeurs,int nentries, int NoCol) const

fill the array 'valeurs' with double data from the current BINTABLE extension on FITS file, from column number 'NoCol'

\param <nentries>  number of data to be read
*/
void FitsInFile::GetBinTabFCol(r_8* valeurs,int nentries, int NoCol) const
    {
      int status= 0;
      int DTYPE;
      long repeat,width;
            fits_get_coltype(fptr_, NoCol+1,&DTYPE,&repeat,&width,&status);
      if( DTYPE != TDOUBLE) 
	{
	  if (DTYPE == TFLOAT)  cout << " WARNING: reading double from float : conversion will be made by fitsio library" << endl;
	  else
	    throw IOExc("FitsFile::GetBinTabFCol, tentative de lecture non double");
	}
      long nels=nentries;
      int    anynul;
      double dnull= dnull_;

      fits_read_col(fptr_,TDOUBLE,NoCol+1,1,1,nels,&dnull,valeurs,
			    &anynul,&status);
      if( status )
	{
	  printerrorAndContinue( status,"erreur lecture de colonne" );
	}
    }

/*! \fn  void SOPHYA::FitsInFile::GetBinTabFCol(float* valeurs,int nentries, int NoCol) const

 same as previous method with float data 
*/
void FitsInFile::GetBinTabFCol(r_4* valeurs,int nentries, int NoCol) const
    {
      int status= 0;
      int DTYPE;
      long repeat,width;
      fits_get_coltype(fptr_, NoCol+1,&DTYPE,&repeat,&width,&status);
      if( DTYPE != TFLOAT) 
	{
	  if (DTYPE == TDOUBLE)  cout << " WARNING: reading float from double : conversion will be made by fitsio library" << endl;
	  else
	    throw IOExc("FitsFile::GetBinTabFCol, tentative de lecture non float");
	}
      long nels=nentries;
      int    anynul;
      float fnull= fnull_;
      fits_read_col(fptr_,TFLOAT,NoCol+1,1,1,nels,&fnull,valeurs,
			    &anynul,&status);
      if( status ) printerrorAndContinue( status,"erreur lecture de colonne" );
    }

/*! \fn void SOPHYA::FitsInFile::GetBinTabFCol(int* valeurs,int nentries, int NoCol) const

 same as previous method with int data 
*/

void FitsInFile::GetBinTabFCol(int_4* valeurs,int nentries, int NoCol) const
    {
      int status= 0;
      int DTYPE;
      long repeat,width;
      fits_get_coltype(fptr_, NoCol+1,&DTYPE,&repeat,&width,&status);
      if( DTYPE != TLONG &&  DTYPE != TINT) 
	{
	  throw IOExc("FitsFile::GetBinTabFCol, probleme de lecture d'entiers");
	}
      long nels=nentries;
      int    anynul;
      int inull= inull_;



      // voir commentaire dans putColToFits()
      fits_read_col(fptr_,TINT,NoCol+1,1,1,nels,&inull,valeurs,
			    &anynul,&status);
      if( status ) printerrorAndContinue( status,"erreur lecture de colonne" );
    }

/*! \fn void SOPHYA::FitsInFile::GetBinTabFCol(char** valeurs, int nentries, int NoCol) const

 same as previous method with char* data 
*/

void FitsInFile::GetBinTabFCol(char** valeurs, int nentries, int NoCol) const
    {
      int status= 0;
      int DTYPE;
      long repeat,width;
      fits_get_coltype(fptr_, NoCol+1,&DTYPE,&repeat,&width,&status);
      if( DTYPE != TSTRING &&  DTYPE != TBYTE) 
	{
	  throw IOExc("FitsFile::GetBinTabFCol, tentative de lecture non string");
	}
      long nels=nentries;
      int    anynul;
      char* cnull=  const_cast<char*>(cnull_.c_str());
      long frow=1;
      long felem=1;
      fits_read_col(fptr_,TSTRING,NoCol+1,frow,felem,nels,cnull,valeurs,
		    &anynul,&status);
      if( status ) printerrorAndContinue( status,"erreur lecture de colonne" );
    }

/*! \fn void SOPHYA::FitsInFile::GetSingleColumn(double* map, int nentries) const
fill the array 'map' with double data from the current extension on FITS file.
If the extension is BINTABLE, the first column is provided. 

\param <nentries>  number of data to be read
*/
void FitsInFile::GetSingleColumn(r_8* map, int nentries) const
{ 
  int status = 0;
  if(hdutype_ == FitsExtensionType_IMAGE) 
    {
  
      if(imageDataType_ != FitsDataType_double) 
	{
	  cout << " The data type on fits file is not double...";
	  cout << " Conversion to double achieved by cfitsio lib" << endl;
	}
  
      int    anynul;
      double dnull= dnull_;
  
      long nels= nentries;
      fits_read_img(fptr_,TDOUBLE,1,nels,&dnull,map,&anynul,&status);
      if( status ) printerror( status );
    }
  else
    if(hdutype_ == FitsExtensionType_ASCII_TBL || hdutype_ == FitsExtensionType_BINARY_TBL)
      {
	GetBinTabFCol(map,nentries, 0);
      }
    else
      {
      cout << " hdutype= " << (int) hdutype_ << endl;
      throw IOExc("FitsFile::GetSingleColumn, this HDU is unknown");
      }
}

/*! \fn void SOPHYA::FitsInFile::GetSingleColumn(float* map, int nentries) const
same as above with float data 
*/
void FitsInFile::GetSingleColumn(r_4* map, int nentries) const
{
  int status = 0;
  if(hdutype_ == FitsExtensionType_IMAGE) 
    {
      if(imageDataType_ != FitsDataType_float) 
	{
	  cout << " The data type on fits file is not float ";
	  cout << " Conversion to float achieved by cfitsio lib" << endl;
	}
      int   anynul;
      float fnull= fnull_;
      
      long nels= nentries;
      fits_read_img(fptr_,TFLOAT,1,nels,&fnull, map,&anynul,&status);
      if( status ) printerror( status );
    }
  else
    if(hdutype_ == FitsExtensionType_ASCII_TBL || hdutype_ == FitsExtensionType_BINARY_TBL)
      {
	GetBinTabFCol(map,nentries, 0);
      }
    else
      {
      cout << " hdutype= " << (int) hdutype_ << endl;
      throw IOExc("FitsFile::GetSingleColumn this HDU is unknown");
      }
}

/*! \fn void SOPHYA::FitsInFile::GetSingleColumn( int* map, int nentries) const
 same as above with int data 
*/
void FitsInFile::GetSingleColumn( int_4* map, int nentries) const
{
  int status = 0;
  if(hdutype_ == FitsExtensionType_IMAGE) 
    {
      if(imageDataType_ != FitsDataType_int) 
	{
	  cout << " The data type on fits file is not int ";
	  cout << " Conversion to float achieved by cfitsio lib" << endl;
	}
      int   anynul;
      float fnull= fnull_;
      
      long nels= nentries;
      fits_read_img(fptr_,TINT,1,nels,&fnull,map,&anynul,&status);
      if( status ) printerror( status );
    }
  else
    if(hdutype_ == FitsExtensionType_ASCII_TBL || hdutype_ == FitsExtensionType_BINARY_TBL)
      {
	GetBinTabFCol(map,nentries, 0);
      }
    else
      {
      cout << " hdutype= " << (int) hdutype_ << endl;
      throw IOExc("FitsFile::GetSingleColumn this HDU is unknown");
      }
}

void FitsInFile::GetBinTabParameters(fitsfile* fileptr, int& nbcols, int& nrows,
				  vector<int>& repeat,
				  vector<string>& noms, 
				  vector<FitsDataType>& types,   
				  vector<int>&  taille_des_chaines)
{
  int status= 0;
  int hdunum=0;
  int hdutype=0;
  fits_get_hdu_num(fileptr,&hdunum);
  fits_get_hdu_type(fileptr, &hdutype, &status);

  if(hdutype != ASCII_TBL && hdutype != BINARY_TBL)
    {
      throw IOExc("FitsFile::GetBinTabParameters this HDU is not an ASCII table nor a binary table");
    }
  //  if(hdutype == ASCII_TBL)
  //        cout << " Reading a FITS ascii table in HDU : " << hdunum << endl;  
  //  if(hdutype == BINARY_TBL)
  //        cout << " Reading a FITS binary table in HDU : " << hdunum << endl;

  // get the number of columns
  fits_get_num_cols(fileptr, &nbcols,&status);
  if( status ) printerror( status );

  // get the number of rows
  long naxis2= 0;
  fits_get_num_rows(fileptr,&naxis2,&status);
  if( status ) printerror( status );
  nrows = (int)naxis2;

  // get the datatype,  names and the repeat count
  noms.clear();
  noms.reserve(nbcols);
  types.clear();
  types.reserve(nbcols);
  repeat.clear();
  repeat.reserve(nbcols);
  taille_des_chaines.clear();
  char **ttype = new char*[nbcols];
  int ii;
  // 
  //
  for (ii=0; ii < nbcols; ii++) ttype[ii]=new char[FLEN_VALUE];
  int nfound;
  fits_read_keys_str(fileptr, "TTYPE",1,nbcols,ttype,&nfound, &status);
  if( status ) printerror( status,"erreur lecture des noms de colonne");
  int rept=0;
  if(hdutype == ASCII_TBL)
    {
      for(ii = 0; ii <  nbcols; ii++)
	{
	  int DTYPE;
	  long width;
	  long repete = 0;
	  fits_get_coltype(fileptr,ii+1,&DTYPE,&repete,&width,&status);
	  if( status ) printerror( status,"erreur lecture type de colonne");
	  rept = repete;
	  noms.push_back(string(ttype[ii]));
	  switch (DTYPE)
	    {
	    case TDOUBLE :
	      types.push_back(FitsDataType_double);
	      break;
	    case TFLOAT :
	      types.push_back(FitsDataType_float);
	      break;
	    case TLONG :
	      types.push_back(FitsDataType_long);
	      break;
	    case TINT :
	      types.push_back(FitsDataType_int);
	      break;
	    case TSHORT :
	      types.push_back(FitsDataType_short);
	      break;
	    case TSTRING :
	      types.push_back(FitsDataType_char);
	      taille_des_chaines.push_back(width);
	      rept/=width;
	      break;
	    default :
	      cout << " field " << ii+1 << " DTYPE= " << DTYPE << endl;
	      throw IOExc("FitsFile::GetBinTabParameters, unsupported data type of field, for ASCII table");
	    }
	  repeat.push_back(rept);
	}
    }
  else 
    {
      for(ii = 0; ii <  nbcols; ii++)
	{
	  int DTYPE;
	  long width;
	  long repete = 0;
	  fits_get_coltype(fileptr,ii+1,&DTYPE,&repete,&width,&status);
	  if( status ) printerror( status,"erreur lecture type de colonne");
	  rept = repete;
	  noms.push_back(string(ttype[ii]));
	  switch (DTYPE)
	    {
	    case TDOUBLE :
	      types.push_back(FitsDataType_double);
	      break;
	    case TFLOAT :
	      types.push_back(FitsDataType_float);
	      break;
	    case TLONG :
	      types.push_back(FitsDataType_long);
	      break;
	    case TINT :
	      types.push_back(FitsDataType_int);
	      break;
	    case TSHORT :
	      types.push_back(FitsDataType_short);
	      break;
	    case TSTRING :
	      types.push_back(FitsDataType_char);
	      taille_des_chaines.push_back(width);
	      rept/=width;
	      break;
	    case TBYTE :
	      types.push_back(FitsDataType_byte);
	      break;
	    default :
	      cout << " field " << ii+1 << " DTYPE= " << DTYPE << endl;
	      throw IOExc("FitsFile::GetBinTabParameters, unsupported data type of field, for BINTABLE");
	    }
	  repeat.push_back(rept);
	}
    }
  for (ii=0; ii < nbcols; ii++) delete [] ttype[ii];
  delete [] ttype;
}

void FitsInFile::KeywordsIntoDVList(fitsfile* fileptr, DVList& dvl, int hdunum)
{
  int status = 0;
  int hdutype;
  fits_movabs_hdu(fileptr,hdunum,&hdutype,&status);
  if( status ) printerror( status,":KeywordsIntoDVList : erreur movabs");
  // get number of keywords
  int nkeys,keypos;
  fits_get_hdrpos(fileptr,&nkeys,&keypos,&status);
  if( status ) printerror( status );

  // put  keywords in a DVList object
  char keyname[LEN_KEYWORD]= "";
  char strval[FLEN_VALUE]= "";
  char dtype;
  char card[FLEN_CARD];  
  const char *comkey = "COMMENT";
  char comment[FLEN_COMMENT];

  // shift with the number of mandatory keywords
  //  int num= 8;
    int num= 0;
    // primary header
    if (hdunum == 1) 
      {
	// read NAXIS
	int naxis=0;
	fits_read_key(fileptr,TINT,"NAXIS",&naxis,NULL,&status);
	// number of mandatory keywords
	num = naxis+3;
      }
    // extensions
    else
      {
	if (hdutype == IMAGE_HDU)
	  {
	    // read NAXIS
	    int naxis=0;
	    fits_read_key(fileptr,TINT,"NAXIS",&naxis,NULL,&status);
	    // number of mandatory keywords
	    num = naxis+5;
	  }
	else
	  if(hdutype == ASCII_TBL || hdutype == BINARY_TBL)
	    {
	      // number of mandatory keywords
	      num = 8;
	    }
      }
  int j;
  for(j = num+1; j <= nkeys; j++)
    {
      fits_read_keyn(fileptr,j,card,strval,NULL,&status);
      if(status) printerror(status);

      strncpy(keyname,card,LEN_KEYWORD-1);
      if(strncmp(keyname,comkey,LEN_KEYWORD-1) != 0 && strlen(keyname) != 0 
	 && strlen(strval) != 0)
	{
	  fits_get_keytype(strval,&dtype,&status);
	  if(status)  printerror(status);
	  
	  strip(keyname, 'B',' '); 
	  strip(strval, 'B',' ');
	  strip(strval, 'B','\'');
	  
	  switch( dtype )  
	    { 
	    case 'C':
	      fits_read_key(fileptr,TSTRING,keyname,strval,comment,&status);
              if ( strncmp(keyname,"TTYPE",5) == 0 || 
		   strncmp(keyname,"TFORM",5) == 0 ||
		   strncmp(keyname,"TBCOL",5) == 0 ) break;
	      dvl[keyname]= strval;
	      dvl.SetComment(keyname, comment);
	      break;
	    case 'I':
	      int ival;
	      fits_read_key(fileptr,TINT,keyname,&ival,comment,&status);
	        dvl[keyname]= (int_4) ival;		// Portage mac DY
		dvl.SetComment(keyname, comment);
	      break;
	    case 'L':
	      int ilog;
	      fits_read_key(fileptr,TLOGICAL,keyname,&ilog,comment,&status);
	      dvl[keyname]= (int_4) ilog;
	      dvl.SetComment(keyname, comment);
	      break;
	    case 'F':
	      double dval;
	      fits_read_key(fileptr,TDOUBLE,keyname,&dval,comment,&status);
	      dvl[keyname]= dval;
	      dvl.SetComment(keyname, comment);
	      break;
	    }
	  
	}
    } 
  //    dvl.Print();
} 


/*!
  \class SOPHYA::FitsOutFile
  \ingroup FitsIOServer
 Class for loading  SOPHYA objects from FITS Format Files (uses cfitsio lib)
*/

FitsOutFile::FitsOutFile()
  : FitsFile()
{
  InitNull(); 
}

   /*! \fn SOPHYA::FitsOutFile::FitsOutFile(char flnm[], WriteMode wrm)

\param <WriteMode>  enum , WriteMode = clear -> if alreadyy exists, the file will be overwritten (else created) ; WriteMode = append -> further objects will be appended to the file if it exists (else : file created). WriteMode = unknown -> file created if does not exist, else : exception. (the last situation is the default)

   */

FitsOutFile::FitsOutFile(string const & flnm, WriteMode wrm)
  : FitsFile()
{
  InitNull();
  openoutputfitsfile(flnm.c_str(), wrm);
}

FitsOutFile::FitsOutFile(const char * flnm, WriteMode wrm)
  : FitsFile()
{
  InitNull();
  openoutputfitsfile(flnm, wrm);
}

FitsOutFile::FitsOutFile(FitsInOutFile const& fios)
  : FitsFile(fios)
{
  InitNull(); 
  if (mode_ == Fits_RO)
    throw FitsIOException("FitsOutFile::FitsOutFile(FitsInOutFile const& ) ReadOnly Fits file");
}

FitsOutFile::~FitsOutFile()
{
  if (dvlToPrimary_ != NULL) delete dvlToPrimary_;
}

void FitsOutFile::openoutputfitsfile(const char * flnm, WriteMode wrm)
{
  int status = 0; 

  // create new FITS file
  fits_create_file(&fptr_,flnm,&status);
  if( status )    {
    switch (wrm) {
      // si on veut ecrire a la fin de ce fichier
    case append :
      status = 0;
      fits_clear_errmsg();
      fits_open_file(&fptr_,flnm,READWRITE,&status);
      if( status ) {
	cout << " error opening file: " << flnm << endl;
	printerror(status, "failure opening a file supposed to exist");
      }
      else cout << " file " << flnm << " opened, new objects will be appended " << endl;
      fits_get_num_hdus(fptr_, &hdunum_, &status);
      int hdutype;
      fits_movabs_hdu(fptr_,hdunum_,&hdutype,&status);
      if( status ) { 
	printerror( status,":FitsFile::WriteF : erreur movabs");
	throw FitsIOException("FitsOutFile::openoutputfitsfile()/fits movabs error");
	break;
	
      case clear :
	{
	  status = 0;
	  fits_clear_errmsg();
	  char* newname = new char[strlen(flnm)+2];
	  //
	  newname[0] = '!';
	  newname[1] = '\0';
	  strcat(newname, flnm);
	  fits_create_file(&fptr_,newname,&status);
	  delete [] newname;
	  if (status) {
	    cout << " error opening file: " << flnm << endl;
	    printerror(status, "unable to open file, supposed to exist");
	    throw FitsIOException("FitsOutFile::openoutputfitsfile()/fits open Error ");
	  }
	  else  cout << "  WARNING : file " << flnm << " is overwritten " << endl;
	  break;
	}
      case unknown : 
	printerror(status, " file seems already to exist");
	throw FitsIOException("FitsOutFile::openoutputfitsfile()/fits open Error - existing file");
	break;
      }
    }
  }
  fname_ = flnm;
  if ( wrm == append ) mode_ = Fits_RW;
  else mode_ = Fits_Create;
  ownfptr = true;
}



/*! \fn void SOPHYA::FitsOutFile::makeHeaderImageOnFits(char type, int nbdim, int* naxisn,  DVList &dvl) 

create an IMAGE header on FITS file.
\param <type> type of data (see method ColTypeFromFits)
\param <nbdim>  number of dimensions : 1D, 2D, 3D etc. = NAXIS
\param <naxisn>  array containind sizes of the different dimensions
*/
void FitsOutFile::makeHeaderImageOnFits(char type, int nbdim, int* naxisn,  DVList* ptr_dvl) 
{
  int status = 0;
  long naxis = nbdim;
  long* naxes = new long[nbdim];
  bool hdunfirst= (hdunum_ == 0);
  if (hdunfirst)
    {
      if (imageOnPrimary_ == false)
	{ 
	  hdunum_ = 1;
	  fits_create_img(fptr_,FLOAT_IMG,0,naxes,&status);
	}
    }
  int k;
  for (k=0; k< nbdim; k++) naxes[k] = (long)naxisn[k];
  if (type == 'D')
    fits_create_img(fptr_,DOUBLE_IMG,naxis,naxes,&status);
  else
    if  (type == 'E')
      fits_create_img(fptr_,FLOAT_IMG,naxis,naxes,&status);
    else
      if  (type == 'I')
	fits_create_img(fptr_,LONG_IMG,naxis,naxes,&status);
      else
	{
	  cout << " type of data: " << type << endl;
	  throw PException("FitsFile:::makeHeaderImageOnFits:unprogrammed type of data ");
	}

  // on ajoute eventuellement un dvlist prepare et la doc SOPHYA
  hdunum_++;
  if (hdunfirst) 
    {
      addDVListOnPrimary();
      writeSignatureOnFits(1);
    }

  // header format FITS
   
   writeAppendedHeaderOnFits();

  // write supplementary keywords (from SOPHYA)
  //  dvl.Print();
  if (ptr_dvl != NULL) addKeywordsOfDVList(*ptr_dvl);

  delete [] naxes;
  if( status ) printerror( status, "erreur creation HDU IMAGE"  );

}


/*! \fn void SOPHYA::FitsOutFile::PutImageToFits(int nbData, double* map) const

write double data from array 'map'on an IMAGE extension 
\param <nbData>  number of data to be written
*/
void FitsOutFile::PutImageToFits(int nbData, r_8* map) const
{
  int status = 0;
  long npix= nbData;
  fits_write_img(fptr_,TDOUBLE,1,npix,map,&status); 
  if( status ) printerror( status, "erreur ecriture PutImageToFits" );
}

/*! \fn void SOPHYA::FitsOutFile::PutImageToFits(int nbData, float* map) const

same as previous method with float data 
*/
void FitsOutFile::PutImageToFits(int nbData, r_4* map) const
{
  int status = 0;
  long npix= nbData;
  fits_write_img(fptr_,TFLOAT,1,npix, map,&status); 
  if( status ) printerror( status, "erreur ecriture PutImageToFits" );

}

  /*! \fn void SOPHYA::FitsOutFile::PutImageToFits( int nbData, int* map) const

 same as previous method with int data */
void FitsOutFile::PutImageToFits( int nbData, int_4* map) const
{
  int status = 0;

  long npix= nbData;
  fits_write_img(fptr_,TINT,1,npix,map,&status); 
  if( status ) printerror( status, "erreur ecriture PutImageToFits" );
}



/*! \fn void SOPHYA::FitsOutFile::makeHeaderBntblOnFits( string fieldType, vector<string> Noms, int nentries, int tfields, DVList &dvl, string extname, vector<int> taille_des_chaines)

create an BINTABLE header on FITS file.
\param <fieldType> array conta
ining characters denoting types of the different column (see method ColTypeFromFits)
\param <Noms>  array of the names of columns
\param <nentries>  number of data of each column
\param <tfields> number of columns
\param <dvl> a SOPHYA DVList containing keywords to be appended
\param <extname> keyword EXTNAME for FITS file
\param <taille_des_chaines> vector containing the number of characters of  data  for each char* typed column, with order of appearance in 'fieldType'
*/
void FitsOutFile::makeHeaderBntblOnFits(string fieldType, vector<string> Noms, int nentries, int tfields, DVList* ptr_dvl, string extname, vector<int> taille_des_chaines) 
{ 
  int k;
  int status = 0;
  long nrows;
  // verifications de coherences

  if (fieldType.length() != tfields)
    {
      cout << " nombre de champs :" << tfields << "nombre de types: " << fieldType.length() << endl;
      throw ParmError("FitsFile:: fields and types don't match");

    }
  if (tfields >  Noms.size())
    {
      cout << " WARNING: FitsOutFile::makeHeaderBntblOnFits, length of vector of column names not equal to total number of columns" << endl;
      for (k=0; k<(tfields-Noms.size()); k++) Noms.push_back( string(" "));
    }
  
  // nombre de variables "chaines de caracteres"
  int nbString = 0;
  for (k=0; k<tfields;k++) if (fieldType[k] == 'A') nbString++;
  // coherence de la longueur du vecteur des tailles 
  if (nbString >  taille_des_chaines.size())
    {
      cout << " WARNING: FitsOutFile::makeHeaderBntblOnFits, length of vector of string lengths  not equal to total number of columns" << endl;
      int strSz=0;
      for (k=0; k<taille_des_chaines.size(); k++) if ( taille_des_chaines[k] > strSz) strSz = taille_des_chaines[k];
      for (k=0; k<(nbString-taille_des_chaines.size()); k++) taille_des_chaines.push_back(strSz);
    }
  char ** ttype= new char*[tfields];
  char ** tform= new char*[tfields];
  char largeur[FLEN_VALUE];
  int noColString=0;
  for (k=0; k<tfields;k++)
    {
      char  format[FLEN_VALUE];
      
      if(nentries < 1024) 
	{
	  nrows= nentries;
	  if (fieldType[k] == 'A')
	    {
	      sprintf(largeur,"%d",taille_des_chaines[noColString++]);
	      strcpy(format,largeur);
	    }
	  else strcpy(format,"1");
	}
      else
	{
	  nrows = nentries/1024;
	  if(nentries%1024 != 0) nrows++;
	  if (fieldType[k] == 'A') 
	    {
	      char largaux[FLEN_VALUE];
	      sprintf(largeur,"%d",taille_des_chaines[noColString]);
	      sprintf(largaux,"%d",1024*taille_des_chaines[noColString]);
	      noColString++;
	      strcpy(format, largaux);    
	    }
	  else strcpy(format,"1024");
	}
      strncat(format,&fieldType[k],1);
      if (fieldType[k] == 'A') 
	{
	  strcat(format,largeur);
	}
      ttype[k] =  const_cast<char*>(Noms[k].c_str());
      tform[k]= new char[FLEN_VALUE];
      strcpy(tform[k],format);
    }
  char* extn = const_cast<char*>(extname.c_str());

  // create a new empty binary table onto the FITS file
  // physical units if they exist, are defined in the DVList object
  // so the NULL pointer is given for the tunit parameters.
  nrows=0;
  fits_create_tbl(fptr_,BINARY_TBL,nrows,tfields,ttype,tform,
		  NULL,extn,&status);
  if( status ) printerror( status );

  int ii;
  for(ii = 0; ii < tfields; ii++)
    {
      delete [] tform[ii];
    }
  delete [] ttype;
  delete [] tform;

  // on ajoute eventuellement des mots-cles

  if ( hdunum_ == 0 )
    {
      hdunum_ = 2;
      addDVListOnPrimary();
      writeSignatureOnFits(1);
    }
  else  hdunum_++;

  // header format FITS
   
   writeAppendedHeaderOnFits();

  // write SOPHYA keywords
  if (ptr_dvl != NULL) addKeywordsOfDVList(*ptr_dvl);
}

 

/*! \fn void SOPHYA::FitsOutFile::PutColToFits(int nocol, int nentries, double* donnees) const

write double data from array 'donnees ' on column number 'nocol' of a BINTABLE  extension.
\param <nentries>  number of data to be written
*/

void FitsOutFile::PutColToFits(int nocol, int nentries, r_8* donnees) const
{
  int status = 0;
  int hdutype;
  fits_movabs_hdu(fptr_,hdunum_,&hdutype,&status);
  if( status ) printerror(status,"PutColToFits: le movabs a foire");
  fits_get_hdu_type(fptr_, &hdutype, &status);
    if(hdutype != ASCII_TBL && hdutype != BINARY_TBL)
        {
            cout << " hdunum= " << hdunum_ << " hdutype= " << hdutype << endl;
     throw IOExc("FitsFile::PutColToFits, this HDU is not an ASCII table nor a binary table");
          }
  int code; 
  long repeat, width;
  fits_get_coltype(fptr_, nocol+1, &code, &repeat,&width, &status);
  if( code != TDOUBLE)
    {
      cout << " WARNING : types don't match (PutColToFits) : on fits file= " << code << " to be written= DOUBLE " << endl;
    } 
  //  cout << " 10 elements de colonne " << endl;
  //  for (int toto=0; toto < 10; toto++) cout << donnees[toto] << endl;
  fits_write_col(fptr_,TDOUBLE,nocol+1,1,1,nentries, donnees ,&status);
  if( status )  printerror( status,"erreur ecriture col. double, dans fichier fits" );
}



  /*! \fn void SOPHYA::FitsOutFile::PutColToFits(int nocol, int nentries, float* donnees) const

same as previous method with float data 
*/
void FitsOutFile::PutColToFits(int nocol, int nentries, r_4* donnees) const
{
  int status = 0;
  int hdutype;
  fits_movabs_hdu(fptr_,hdunum_,&hdutype,&status);
  if( status ) printerror(status,"PutColToFits: le movabs a foire");
  fits_get_hdu_type(fptr_, &hdutype, &status);
  if(hdutype != ASCII_TBL && hdutype != BINARY_TBL)
    {
      cout << " hdunum= " << hdunum_ << " hdutype= " << hdutype << endl;
      throw IOExc("FitsFile::PutColToFits, this HDU is not an ASCII table nor a binary table");
    }
  if(hdutype == ASCII_TBL && nocol>0)
    {
      throw IOExc("FitsFile::PutColToFits, this HDU is an ASCII table, nocol>0  forbidden");
    }
  int code; 
  long repeat, width;
  fits_get_coltype(fptr_, nocol+1, &code, &repeat,&width, &status);
  if( code != TFLOAT)
    {
      cout << " WARNING : types don't match (PutColToFits) : on fits file= " << code << " (FITS code), to be written= FLOAT " << endl;
    } 
  fits_write_col(fptr_,TFLOAT,nocol+1,1,1,nentries, donnees ,&status);
  if( status )  printerror( status,"erreur ecriture col. floats, dans fichier fits" );
}


/*! \fn void FitsOutFile::PutColToFits(int nocol, int nentries, int* donnees) const

same as previous method with int data 
*/
void FitsOutFile::PutColToFits(int nocol, int nentries, int_4* donnees) const
{
  int status = 0;
  int hdutype;
  fits_movabs_hdu(fptr_,hdunum_,&hdutype,&status);
  if( status ) printerror(status,"PutColToFits: le movabs a foire");
  fits_get_hdu_type(fptr_, &hdutype, &status);
  if(hdutype != ASCII_TBL && hdutype != BINARY_TBL)
    {
      cout << " hdunum= " << hdunum_ << " hdutype= " << hdutype << endl;
      throw IOExc("FitsFile::PutColToFits, this HDU is not an ASCII table nor a binary table");
    }
  if(hdutype == ASCII_TBL && nocol>0)
    {
      throw IOExc("FitsFile::PutColToFits, this HDU is an ASCII table, nocol>0  forbidden");
    }
  int code; 
  long repeat, width;
  fits_get_coltype(fptr_, nocol+1, &code, &repeat,&width, &status);
    

  
  if (code == TINT || code == TLONG)
    {
      // cfitsio n'a que des entiers de longueur inferieure a 32 bits.
      // ici, a l'ecriture TLONG impliquerait que le tableau de donnees
      // soit un tableau int_8. Donc c'est toujours TINT qu;il faut mettre
      // De plus, j'ai l'impression que TINT va devenir obsolete dans cfitsio
      // (GLM)
      fits_write_col(fptr_,TINT,nocol+1,1,1,nentries, donnees ,&status);
    }
  else
    {
      cout << " WARNING : types don't match (PutColToFits) : on fits file= " << code << " (FITS code), to be written= integers " << endl;
    } 
  if( status )  printerror( status,"erreur ecriture col. entiers, dans fichier fits" );
}


/*! \fn void SOPHYA::FitsOutFile::PutColToFits(int nocol, int nentries, char** donnees) const
same as previous method with char* data 
*/
void FitsOutFile::PutColToFits(int nocol, int nentries, char** donnees) const
{
  int status = 0;
  int hdutype;
  fits_movabs_hdu(fptr_,hdunum_,&hdutype,&status);
  if( status ) printerror(status,"PutColToFits: le movabs a foire");
  fits_get_hdu_type(fptr_, &hdutype, &status);
  if(hdutype != ASCII_TBL && hdutype != BINARY_TBL)
    {
      cout << " hdunum= " << hdunum_ << " hdutype= " << hdutype << endl;
      throw IOExc("FitsFile::PutColToFits, this HDU is not an ASCII table nor a binary table");
    }
  if(hdutype == ASCII_TBL && nocol>0)
    {
      throw IOExc("FitsFile::PutColToFits, this HDU is an ASCII table, nocol>0  forbidden");
    }
  int code; 
  long repeat, width;
  fits_get_coltype(fptr_, nocol+1, &code, &repeat,&width, &status);
  if( code != TSTRING)
    {
      cout << " WARNING : types don't match (PutColToFits) : on fits file= " << code << " (FITS code), to be written= char** " << endl;
    } 
  fits_write_col(fptr_,TSTRING,nocol+1,1,1,nentries, donnees ,&status);
  if( status )  printerror( status,"erreur ecriture col. chars, dans fichier fits" );
}

void FitsOutFile::PutBinTabLine(long NoLine,  BnTblLine& ligne) const
{
  // on ne fait pas de verification de type, ni de dimension ici, pour 
  // des raisons de performances
  int k;
  int status= 0;
  int    anynul;
  int ncol=0;
  long nels=1;
  //  int nbcols;
  //  fits_get_num_cols(fptr_, &nbcols,&status);
  for (k=0; k<ligne.ddata_.size(); k++, ncol++)
    {
      fits_write_col(fptr_,TDOUBLE,ncol+1,NoLine+1,1,1, &ligne.ddata_[k] ,&status);
      if( status ) printerror( status, "PutBinTabLine : erreur ecriture double"  );
    }
  for (k=0; k<ligne.fdata_.size(); k++, ncol++)
    {
      fits_write_col(fptr_,TFLOAT,ncol+1,NoLine+1,1,1, &ligne.fdata_[k] ,&status);
      if( status ) printerror( status, "PutBinTabLine : erreur ecriture float"  );
    }
  for (k=0; k<ligne.idata_.size(); k++, ncol++)
    {
      fits_write_col(fptr_,TINT,ncol+1,NoLine+1,1,1, &ligne.idata_[k] ,&status);
      if( status ) printerror( status, "PutBinTabLine : erreur ecriture entier"  );
    }
  for (k=0; k<ligne.ldata_.size(); k++, ncol++)
    {
      fits_write_col(fptr_,TLONG,ncol+1,NoLine+1,1,1, &ligne.ldata_[k] ,&status);
      if( status ) printerror( status, "PutBinTabLine : erreur ecriture entier long"  );
    }
  for (k=0; k<ligne.bdata_.size(); k++, ncol++)
    {
      fits_write_col(fptr_,TBYTE,ncol+1,NoLine+1,1,1, &ligne.bdata_[k] ,&status);
      if( status ) printerror( status, "PutBinTabLine : erreur ecriture byte"  );
    }
  
  for (k=0; k<ligne.cdata_.size(); k++, ncol++)
    {
      fits_write_col(fptr_,TSTRING,ncol+1,NoLine+1,1,1, (void*)ligne.cdata_[k].c_str() ,&status);
      if( status ) printerror( status, "PutBinTabLine : erreur ecriture caracteres"  );
    }
}


/* \fn void  SOPHYA::FitsOutFile::DVListIntoPrimaryHeader(DVList& dvl) const

Put keywords from a DVList into the primary header of the fits-file 
*/
void  FitsOutFile::DVListIntoPrimaryHeader(DVList& dvl) 
{
  int status = 0;
  int hdutype;
  if (hdunum_ == 0) 
    {
      if (dvlToPrimary_ == NULL) dvlToPrimary_ = new DVList(dvl);
      else dvlToPrimary_->Merge(dvl);
    }
  else
    {
      fits_movabs_hdu(fptr_,1,&hdutype,&status);
      addKeywordsOfDVList(dvl);
      fits_movabs_hdu(fptr_,hdunum_,&hdutype,&status);
    }
}


void FitsOutFile::writeSignatureOnFits(int hdunum) const
{
  int status = 0;
  int hdutype;
  char keyname[LEN_KEYWORD];
  char strval[FLEN_VALUE];
  char comment[FLEN_COMMENT];
  if (hdunum_ == 0) 
    {
      cerr << " WARNING : can't write keywords on non existing primary header" << endl;
      return;
    }
  fits_movabs_hdu(fptr_,1,&hdutype,&status);
  //
  strncpy(keyname, "CREATOR", LEN_KEYWORD); 
  keyname[7] = '\0';
  strcpy(strval, "SOPHYA");
  strcpy(comment," SOPHYA Package - FITSIOServer ");
  fits_write_key(fptr_, TSTRING, keyname, &strval, comment, &status);
  if( status ) printerror( status );
  fits_write_date(fptr_, &status);
  fits_write_comment(fptr_,"..............................................", &status);
  fits_write_comment(fptr_, " SOPHYA package - FITSIOSever ", &status);
  fits_write_comment(fptr_, " (C) LAL/IN2P3-CNRS Orsay,  FRANCE 2000", &status);
  fits_write_comment(fptr_, " (C) DAPNIA/CEA     Saclay, FRANCE 2000", &status);
  fits_write_comment(fptr_,"..............................................", &status);
  if( status ) printerror( status, "erreur writeSignatureOnFits"  );
  //
  fits_movabs_hdu(fptr_,hdunum_,&hdutype,&status);
}


void FitsOutFile::addKeywordsOfDVList( DVList& dvl) const
{
  int status = 0;
  fits_write_comment(fptr_,"---------- keywords from SOPHYA ---------", &status);
  DVList::ValList::const_iterator it;
  for(it = dvl.Begin(); it != dvl.End(); it++)
    {
      MuTyV::MTVType keytype= (*it).second.elval.Type();
      char keyname[LEN_KEYWORD];
      strncpy(keyname,(*it).first.substr(0,64).c_str(),LEN_KEYWORD);
      int bout = ((*it).first.substr(0,64).length() < LEN_KEYWORD) ? (*it).first.substr(0,64).length() : LEN_KEYWORD-1;
      keyname[bout] = '\0';
      string key((*it).first.substr(0,64));
	//      string key(keyname);
      char comment[FLEN_COMMENT];
      char strval[FLEN_VALUE]= "";
      const char *comkey = "COMMENT";
      //      fits_read_keyword(fptr_, keyname, strval, NULL, &status);
      //  if (status != 0 || strncmp(keyname,comkey,LEN_KEYWORD-1) == 0 ) 
	{
	  string coco = dvl.GetComment(key);
	  coco.copy( comment, FLEN_COMMENT-1);
	  int bout = (coco.length() < FLEN_COMMENT) ? coco.length() : FLEN_COMMENT-1;
	  comment[bout]= '\0';
	  status = 0;
	  switch (keytype)
	    {
	    case MuTyV::MTVInteger :
	      {
		int ival = (int)dvl.GetI(key);
		fits_write_key(fptr_,TINT,keyname,&ival, comment,&status);
		break;
	      }
	    case MuTyV::MTVFloat :
	      {
		double dval= (double)dvl.GetD(key);
		fits_write_key(fptr_,TDOUBLE,keyname,&dval,comment,&status);
		break;
	      }
	    case MuTyV::MTVString :
	      {
		char strvaleur[FLEN_VALUE]= "";
		string valChaine = dvl.GetS(key);
		valChaine.copy(strvaleur, FLEN_VALUE-1);
		int fin =  (valChaine.length() < FLEN_VALUE) ? valChaine.length() : FLEN_VALUE-1;
		strvaleur[fin]= '\0';

		fits_write_key(fptr_,TSTRING,keyname,&strvaleur,comment,&status);
		break;
	      }
            //G.Barrand : begin :
	    case MuTyV::MTVComplex :
	    case MuTyV::MTVTimeStamp :
              cerr << "FitsOutFile::addKeywordsOfDVList : MTVComplex, MTVTimeStamp not handled." << endl;
              break;
            //G.Barrand : end
	    } 
	}
  if( status )  printerror( status,"fitsfile: probleme ecriture mot-cle du dvlist" );
    }
  fits_write_comment(fptr_,"--------------------------------------", &status);
}


void FitsOutFile::addDVListOnPrimary()
    {
      int status = 0;
      int hdutype;
      if (hdunum_ == 0) 
	{
	  cerr << " WARNING : can't write keywords on non existing primary header" << endl;
	  return;
	}
      if (dvlToPrimary_ != NULL)
	{
	  fits_movabs_hdu(fptr_,1,&hdutype,&status);
	  addKeywordsOfDVList(*dvlToPrimary_);
	  delete dvlToPrimary_;
	  dvlToPrimary_ = NULL;
	  fits_movabs_hdu(fptr_,hdunum_,&hdutype,&status);
	}
    }


/*! \fn void  FitsOutFile::appendInHeader(FitsInFile& infits, int hdunum)

get a header from FitsInFile and append to the header beeing built
(shifting mandatory keywords)
*/ 

void FitsOutFile::appendInputHeader(FitsInFile& infits, int hdunum)
{

  infits.GetKeywordsFromHeader(hdunum, mots_cles_);
  /*
  int status = 0;
  int hdutype;
  fitsfile* fptr=infits.fitsfilePtr();
  fits_movabs_hdu(fptr,hdunum,&hdutype,&status);
  if( status ) fits_report_error(stderr,status);

  // get number of keywords
  int nkeys,keypos;
  fits_get_hdrpos(fptr,&nkeys,&keypos,&status);
  if( status ) fits_report_error(stderr,status);
  // shift with the number of mandatory keywords
  int num= 0;
  // if primary header
    if (hdunum == 1) 
      {
	// read NAXIS
	int naxis=0;
	fits_read_key(fptr,TINT,"NAXIS",&naxis,NULL,&status);
	// number of mandatory keywords
	num = naxis+3;
      }
    // extensions
    else
      {
	if (hdutype == IMAGE_HDU)
	  {
	    // read NAXIS
	    int naxis=0;
	    fits_read_key(fptr,TINT,"NAXIS",&naxis,NULL,&status);
	    // number of mandatory keywords
	    num = naxis+5;
	  }
	else
	  if(hdutype == ASCII_TBL || hdutype == BINARY_TBL)
	    {
	      // number of mandatory keywords
	      num = 8;
	    }
      }
  int j;
  char keyname[LEN_KEYWORD];  
  char value[FLEN_VALUE];
  char comment[FLEN_COMMENT];
  for(j = num+1; j <= nkeys; j++)
    {
      char dtype;
      fits_read_keyn(fptr,j,keyname,value,comment,&status);
      if(status) 
	{
	  fits_report_error(stderr,status);
	  status=0;
	}
      string  kn(keyname);
      string  cm(comment);
      string  val(value);
      FitsKeyword kw(kn, val, cm);
      mots_cles_.push_back(kw);
    }
  */
}
void FitsOutFile::writeAppendedHeaderOnFits()
{
  for (list<FitsKeyword>::iterator it=mots_cles_.begin(); it !=mots_cles_.end(); it++)
    {
      (*it).writeOnFits(fptr_);
    } 
  mots_cles_.clear();
}

void FitsOutFile::insertKeywordOnHeader(string keyname, double value, string comment)
{
  char cvalue[16];
  sprintf(cvalue,"%e",value);
  FitsKeyword kw(keyname, string(cvalue), comment, 'F');
  mots_cles_.push_back(kw);
}
void FitsOutFile::insertKeywordOnHeader(string keyname, int value, string comment)
{
  char cvalue[16];
  sprintf(cvalue,"%d",value);
  FitsKeyword kw(keyname, string(cvalue), comment, 'I');
  mots_cles_.push_back(kw);
}
void FitsOutFile::insertKeywordOnHeader(string keyname, string value, string comment)
{
  FitsKeyword kw(keyname, value , comment, 'C');
  mots_cles_.push_back(kw);
}

void FitsOutFile::insertCommentLineOnHeader(string comment)
{
  FitsKeyword kw(comment);
  mots_cles_.push_back(kw);
}

void FitsOutFile::PrintHeaderToBeAppended()
{
  cout << " contenu du header en cours de fabrication " << endl;
  for (list<FitsKeyword>::iterator it=mots_cles_.begin(); it !=mots_cles_.end(); it++)
    {
      (*it).Print();
    } 
}


FitsKeyword::FitsKeyword()
  {
    datatype_=' ';
    keyname_ = string("");
    dvalue_=0.;
    ivalue_=1;
    svalue_=string("");
    comment_=string("");
  }

FitsKeyword::FitsKeyword(string comment)
  {
    datatype_=' ';
    keyname_=string("COMMENT");
    comment_=comment;
  } 

FitsKeyword::FitsKeyword(string keyname, string value, string comment) :  keyname_(keyname), comment_(comment)
  {
    int status=0;
    char dtype;
    const char* val= value.c_str();
    char* valk = const_cast<char*>(val);
    fits_get_keytype(valk,&dtype,&status);
    if(status) 
      {
	status=0;
	if (status == VALUE_UNDEFINED) cout << "WARNING (FitsKeyword) : undefined keyword value " << endl;
	datatype_=' ';
      }
    else     datatype_=dtype;
    
    switch( datatype_ )  
      { 
      case 'C':
	{
	  strip(valk, 'B','\'');
	  svalue_ = string(valk);
	    break;
	}
      case 'I':
	{
	  ivalue_ = atoi(val);
	  break;
	}
      case 'L':
	{
	  bool bb = value.c_str();
	  ivalue_ = (int)bb;
	  break;
	  }
      case 'F':
	{
	  dvalue_ = atof(val);
	  break;
	}
      case 'X':
	{
	  throw IOExc("FitsKeyword , complex keyword value not supported");
	}
      }
  }

// constructeur pour les mots-cles maison (ne prvenant pas de la lecture d'un fichier fits)
FitsKeyword::FitsKeyword(string keyname, string value, string comment, char type) :  keyname_(keyname), comment_(comment), datatype_(type)
  {
    char dtype;
    const char* val= value.c_str();
    char* valk = const_cast<char*>(val);
    switch( datatype_ )  
      { 
      case 'C':
	{
	  strip(valk, 'B','\'');
	  svalue_ = string(valk);
	    break;
	}
      case 'I':
	{
	  ivalue_ = atoi(val);
	  break;
	}
      case 'L':
	{
	  bool bb = value.c_str();
	  ivalue_ = (int)bb;
	  break;
	  }
      case 'F':
	{
	  dvalue_ = atof(val);
	  break;
	}
      case 'X':
	{
	  throw IOExc("FitsKeyword , complex keyword value not supported");
	}
      }
  }

void FitsKeyword::writeOnFits(fitsfile* ptr) 
  {
    int status=0;
    char keyname[LEN_KEYWORD];
    char comment[FLEN_COMMENT];
    keyname_.copy(keyname, LEN_KEYWORD); 
    int bout = (keyname_.length() < LEN_KEYWORD) ? keyname_.length() : LEN_KEYWORD-1;
    keyname[bout] = '\0';
    comment_.copy( comment, FLEN_COMMENT);
    bout = (comment_.length() < FLEN_COMMENT) ? comment_.length() : FLEN_COMMENT-1;
    comment[bout]= '\0';

  int nkeys,keypos;
  fits_get_hdrpos(ptr,&nkeys,&keypos,&status);
    switch( datatype_ )  
      { 
      case 'C':
	{
	  char value[FLEN_VALUE]="";
	  svalue_.copy(value, FLEN_VALUE-1);
	  int fin =  (svalue_.length() < FLEN_VALUE) ? svalue_.length() : FLEN_VALUE-1;
	  value[fin]= '\0';
	  fits_write_key(ptr,TSTRING,keyname,&value, comment,&status);
	  fits_report_error(stderr,status);
	  break;
	}
      case 'I':
	{
	  fits_write_key(ptr,TINT,keyname,&ivalue_, comment,&status);
	  fits_report_error(stderr,status);
	  break;
	}
      case 'L':
	{
	  fits_write_key(ptr,TLOGICAL,keyname,&ivalue_, comment,&status);
	  fits_report_error(stderr,status);
	  break;
	}
      case 'F':
	{
	  fits_write_key(ptr,TDOUBLE,keyname,&dvalue_, comment,&status);
	  fits_report_error(stderr,status);
	  break;
	}
      case 'X':
	{
	  cout << "FitsKeyword : complex keyword value not supported" << endl;;
	  break;
	}
      default :
	{
	  const char *comkey = "COMMENT";
	  if(strncmp(keyname,comkey,LEN_KEYWORD-1) == 0)
	    {
	      fits_write_comment(ptr,comment,&status);
	      fits_report_error(stderr,status);
	    }
	  else
	    {
	      cout << " WARNING  (FitsKeyword::writeOnFits) : unrecognized keyword : " << keyname_ << endl;
	    }
	}
      }
  }

void FitsKeyword::Print()
  {
    switch( datatype_ )  
      { 
      case 'C':
	{
	  cout << " mot cle : " << keyname_ << " valeur : " << svalue_ << " commentaire : " << comment_ <<endl;
	  break;
	}
      case 'I':
	{
	  cout << " mot cle : " << keyname_ << " valeur : " << ivalue_ << " commentaire : " << comment_ <<endl;
	  break;
	}
      case 'L':
	{
	  cout << " mot cle : " << keyname_ << " valeur : " << ivalue_ << " commentaire : " << comment_ <<endl;
	  break;
	}
      case 'F':
	{
	  cout << " mot cle : " << keyname_ << " valeur : " << dvalue_ << " commentaire : " << comment_ <<endl;
	  break;
	}
      case 'X':
	{
	  cout << "FitsKeyword : complex keyword value not supported" << endl;;
	}
      default :
	{
	  cout << " mot cle : " << keyname_ << " commentaire : "  << comment_ <<endl;
	}
      }
  }
