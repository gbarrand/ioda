/*    
   --- SOPHYA software - FitsIOServer module ---
   Guy Le Meur 09/2000    R. Ansari , 2006 
   (C) UPS+LAL IN2P3/CNRS     (C) DAPNIA-SPP/CEA 
*/
#ifndef FITSFILE_H
#define FITSFILE_H

#include "ndatablock.h"
#include "dvlist.h"
#include "fitsinoutfile.h" 
#include "fitshandler.h" 

#define OPENFILE    0
#define CREATEFILE  1
#define LEN_KEYWORD 9

// classes for saving/loading SOPHYA objects to/from FITS files...
// Guy le Meur (september 2000)


namespace SOPHYA {

  struct BnTblLine;
  class BufferLine;
  class FitsFile;
  class FitsInFile;
  class FitsOutFile;



class  FitsKeyword
{

 public:

FitsKeyword();
FitsKeyword(string comment);
FitsKeyword(string keyname, string value, string comment); 
FitsKeyword(string keyname, string value, string comment, char type); 
void writeOnFits(fitsfile* ptr);

void Print();

 private:

  char datatype_;
  string keyname_;
  double dvalue_;
  int ivalue_;
  string svalue_;
  string comment_;
};



//
//! Class for managing Interface for SOPHYA objects to FITS Format Files (uses cfitsio lib)

class FitsIOHandler : public FitsHandlerInterface {


 public:

   virtual ~FitsIOHandler() {}
   void   Read(const char flnm[],int hdunum= 0);
   void   Write(const char flnm[]) ;

   //Implementation par defaut de l'interface FitsHandlerInterface
   virtual AnyDataObj* DataObj() { return NULL; }
   virtual int         CheckHandling(AnyDataObj & o) { return 0; }
   virtual void        SetDataObj(AnyDataObj & o) { return; }
   virtual int         CheckReadability(FitsInOutFile& is) { return 0; }
   virtual FitsHandlerInterface* Clone() { return NULL; }

   virtual void   Read(FitsInOutFile& ifts);
   virtual void   Write(FitsInOutFile& ofts) ;

   virtual void   Read(FitsInFile& ifts, int hdunum=0);

 protected:
  
   virtual void    ReadFromFits(FitsInFile& is)=0;           
   virtual void    WriteToFits(FitsOutFile& os) =0;   
        
   friend class FitsInFile;
   friend class FitsOutFile;
  };


//! Class (virtual) for managing FITS format files
 class FitsFile : public FitsInOutFile {

 public:

   enum WriteMode {append, clear, unknown};

   enum FitsExtensionType {
     FitsExtensionType_NULL,
     FitsExtensionType_IMAGE,
     FitsExtensionType_ASCII_TBL,
     FitsExtensionType_BINARY_TBL,
     FitsExtensionType_EOF,
     FitsExtensionType_ERROR
   };
   enum FitsDataType {
     FitsDataType_NULL,
     FitsDataType_double,
     FitsDataType_float,
     FitsDataType_int,
     FitsDataType_char,
     FitsDataType_ASCII,
     FitsDataType_long,
     FitsDataType_byte,
     FitsDataType_short
   };


class BufferLine

  {
    public :

BufferLine() {;}
BufferLine(const vector<FitsFile::FitsDataType>& types);
inline const vector< pair<FitsFile::FitsDataType, int> >& identificateur() const {return id_;}
 inline r_8&    r_8Array(int k) { return ddata_[k];}
 inline const r_8&    r_8Array(int k) const { return ddata_[k];}
 inline r_4& r_4Array(int k) { return fdata_[k];}
 inline const r_4& r_4Array(int k) const { return fdata_[k];}


 inline int_2& int_2Array(int k) { return shdata_[k];}
 inline const int_2& int_2Array(int k) const { return shdata_[k];}
 inline int_4& int_4Array(int k) { return idata_[k];}
 inline const int_4& int_4Array(int k) const { return idata_[k];}


 inline int_8& int_8Array(int k) { return ldata_[k];}
 inline const int_8& int_8Array(int k) const { return ldata_[k];}
 inline string& stringArray(int k) { return cdata_[k];}
 inline const string& stringArray(int k) const { return cdata_[k];}
 inline unsigned char& u_charArray(int k) { return bdata_[k];}
 inline const unsigned char& u_charArray(int k) const { return bdata_[k];}


 void Print() const;


    private :
      // la paire contient le type de la variable et le rang dans le tableau
      // du type
   vector< pair<FitsFile::FitsDataType, int> > id_;
   vector<r_8>     ddata_;
   vector<r_4>     fdata_;
   vector<int_2>   shdata_; 
   vector<int_4>   idata_; 
   vector<int_8>   ldata_;
   vector<string>  cdata_;
   vector<unsigned char>   bdata_;
  };

   FitsFile();
   FitsFile(FitsInOutFile const& fios);
   // RzDel virtual ~FitsFile();
   static string GetErrStatus(int status);
   inline  int   statusF() const { return fits_status_;}
   inline void firstImageOnPrimaryHeader(bool choice) {imageOnPrimary_=choice;}
   inline int currentHeaderIndex() {return hdunum_;}


 protected:
void         ResetStatus(int& status) ; 
static  void printerror(int&) ;
static  void printerror(int&,const char* texte) ;
static  void printerrorAndContinue(int& status,const char* texte);
inline void  InitNull() 
  {
    hdutype_= FitsExtensionType_NULL; 
    hdunum_ = 0; 
    fits_status_ = 0; imageOnPrimary_ = true;
  }
inline fitsfile* fitsfilePtr() const {return fptr_;}


   FitsExtensionType hdutype_;        /**<  image or bintable ? */
   int hdunum_;         /**<   index of header to be read/written */
   int fits_status_;    /**< last status returned by fitsio library. updated only by several methods */
   bool imageOnPrimary_;

   BufferLine bfl_;


 };

//! Class for saving  SOPHYA objects on FITS Format Files (uses cfitsio lib)

 class FitsInFile : public  FitsFile {

 public:
   FitsInFile();
   FitsInFile(string const & flnm);
   FitsInFile(const char * flnm);
   FitsInFile(FitsInOutFile const& fios);
   //   virtual ~FitsInFile();

   static int  NbBlocks(char flnm[]);
   int  NbBlocks();
   static void GetBlockType(char flnm[], int hdunum, FitsExtensionType& typeOfExtension, int& naxis, vector<int>& naxisn, FitsDataType& dataType, DVList& dvl  );

   void  ReadHeader(int hdunum);
bool hasKeyword(int hdunum, string keyw);
string getStringKeyword(int hdunum, string keyname,int& retStatus);

  /*! \return a reference on a DVList containing the keywords from FITS file */
  inline const DVList& DVListFromFits() const { return dvl_;}

  void GetKeywordsFromHeader (int hdunum, list<FitsKeyword>& mots_cles) const;


  DVList  DVListFromPrimaryHeader() const;
  void    moveToFollowingHeader();




       //////////////////////////////////////////////////////////
       ///////   methods for managing extensions ////////////////
       //////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
//       methods for managing FITS IMAGE extension
///////////////////////////////////////////////////



/*! \return true if the current header  corresponds to a FITS image extension */
inline bool IsFitsImage() const { return (hdutype_ == FitsExtensionType_IMAGE);}



  /*! \return number of dimensions of an image extension : NAXIS parameter (in FITS notations)   */
inline int nbDimOfImage() const {return naxis_;}

/*! \return a reference on a vector containing sizes of the NAXIS dimensions : NAXIS1, NAXIS2, NAXIS3 etc.  */
 inline const vector<int>& dimOfImageAxes() const { return naxisn_;}


/*! \return total number of data in the current IMAGE extension */
inline int nbOfImageData() const { return nbData_; }

/*! \return data type of the current IMAGE extension */
inline FitsFile::FitsDataType ImageType() const {return imageDataType_;}



//////////////////////////////////////////////////////////////////////////
//       methods for managing FITS BINARY TABLE or ASCII TABLE extension
////////////////////////////////////////////////////////////////////////




/*! \return true if the current header  corresponds to a FITS ASCII or BINTABLE extension */
inline bool IsFitsTable() const {return (hdutype_ == FitsExtensionType_ASCII_TBL || hdutype_ == FitsExtensionType_BINARY_TBL);}




 static  void GetBinTabParameters(fitsfile* fileptr, int& nbcols, int& nrows,
				  vector<int>& repeat,
				  vector<string>& noms, 
				  vector<FitsDataType>& types,   
				  vector<int>&  taille_des_chaines);
 FitsDataType   ColTypeFromFits(int nocol) const;
 string ColNameFromFits(int nocol) const;
 int    ColStringLengthFromFits(int nocol) const;
 const BufferLine& GetBufferLine(long NoLine); 
 void   GetBinTabLine(int NoLine, double* ddata, float* fdata, int* idata, char
** cdata) ;
 void   GetBinTabLine(long NoLine, BnTblLine& ligne) ;
 void   GetBinTabLine(int NoLine, float* fdata) ;
 void   GetBinTabLine(int NoLine, double* ddata) ;
 void   GetBinTabFCol(r_8* valeurs, int nentries, int NoCol) const;
 void   GetBinTabFCol(r_4* valeurs, int nentries, int NoCol) const;
 void   GetBinTabFCol(int_4* valeurs, int nentries,  int NoCol) const;
 void   GetBinTabFCol(char** valeurs,int nentries, int NoCol) const;

/////////////////////////////////////////////////////////////
//       methods for managing any type of FITS extension
////////////////////////////////////////////////////////

/*! \return true if the current header  is beyond the maximum */
inline bool IsFitsEOF() const {return (hdutype_ == FitsExtensionType_EOF);}
/*! \return true if the current header  is incorrect, following a cfitsio, movavs error */
inline bool IsFitsERROR() const {return (hdutype_ == FitsExtensionType_ERROR);}

  int     NbColsFromFits() const;
  int     NentriesFromFits(int nocol) const;


  void    GetSingleColumn(r_8* map, int nentries) const;

  void    GetSingleColumn(r_4*  map, int nentries) const;

  void    GetSingleColumn(int_4* map, int nentries) const;





  private :

void InitNull();

void getHeaderWithSophyaObject();
static void KeywordsIntoDVList(fitsfile* fileptr, DVList& dvl, int hdunum); 
static  void GetImageParameters (fitsfile* fileptr,FitsDataType& dataType,int& naxis,vector<int>& naxisn);

 FitsDataType imageDataType_;          /**< fits-Image parameter (bitpix)*/
 int naxis_;           /**< fits-Image parameter */
 vector<int> naxisn_;  /**< fits-Image parameters : sizes of dimensions */
 int nbData_;          /*< fits-Image parameter: number of data */
 int nrows_;           /**< Bintable parameter */
 vector<int> repeat_;  /**< Bintable parameter */
 int nbcols_;          /**< Bintable parameter */
 vector<string> noms_; /**< Bintable parameter: column names */
 vector<FitsDataType> types_;  /**< Bintable parameters: types of columns (D: double, E: float, I: integers,  A: char*) */
 DVList dvl_;          /**< DVList for transferring keywords */
 vector<int>  taille_des_chaines_; /**< Bintable parameters:   length of the char* variables */

  double dnull_;
  float fnull_;
  int inull_;
  string cnull_;

 

 };

//! Class for loading  SOPHYA objects from FITS Format Files (uses cfitsio lib)

 class FitsOutFile : public  FitsFile {

 public:

   FitsOutFile();
   FitsOutFile(string const & flnm, WriteMode wrm = unknown );
   FitsOutFile(const char * flnm, WriteMode wrm = unknown );
   FitsOutFile(FitsInOutFile const& fios);
   virtual ~FitsOutFile();
   inline void InitNull() {dvlToPrimary_ = NULL;}

       //////////////////////////////////////////////////////////
       ///////   methods for managing extensions ////////////////
       //////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////
//       methods for managing FITS IMAGE extension
///////////////////////////////////////////////////


   void makeHeaderImageOnFits(char type, int nbdim, int* naxisn, DVList* dvl) ;
   void PutImageToFits( int nbData, r_8* map) const;
   void PutImageToFits(int nbData, r_4* map ) const;
   void PutImageToFits(int nbData, int_4* map) const;



//////////////////////////////////////////////////////////////////////////
//       methods for managing FITS BINARY TABLE or ASCII TABLE extension
////////////////////////////////////////////////////////////////////////



   void makeHeaderBntblOnFits ( string fieldType, vector<string> Noms, int nentries, int tfields, DVList* dvl, string extname,  vector<int> taille_des_chaines) ;
   void appendInputHeader(FitsInFile& headerin, int hdunum);
   void writeAppendedHeaderOnFits();
   void PrintHeaderToBeAppended();
   void insertCommentLineOnHeader(string comment);
   void insertKeywordOnHeader(string keyname, double value, string comment); 
   void insertKeywordOnHeader(string keyname, int value, string comment); 
   void insertKeywordOnHeader(string keyname, string value, string comment); 
   void PutColToFits(int nocol, int nentries, r_8* donnees) const;
   void PutColToFits(int nocol, int nentries, r_4* donnees) const;
   void PutColToFits(int nocol, int nentries, int_4* donnees) const;
   void PutColToFits(int nocol, int nentries, char** donnees) const;
   void PutBinTabLine(long NoLine,  BnTblLine& ligne) const;


/////////////////////////////////////////////////////////////
//       methods for managing any type of FITS extension
////////////////////////////////////////////////////////


void  DVListIntoPrimaryHeader(DVList& dvl) ;



  private :

  void openoutputfitsfile(const char * flnm, WriteMode wrm);
  void writeSignatureOnFits(int hdunum) const; 
  void addKeywordsOfDVList( DVList& dvl) const;
  void addDVListOnPrimary();

  DVList* dvlToPrimary_; /**< for transferring keywords when creating primary header */
  list<FitsKeyword>  mots_cles_;

 };

 struct BnTblLine 
 {
   BnTblLine() {}
   void setFormat(int dc, int fc, int ic, int lc, int bc, int cc, vector<string> names);
   bool sameFormat(const BnTblLine& btl) const;

   void Print();

   vector<double> ddata_;
   vector<float>  fdata_;
   vector<int>    idata_; 
   vector<string>  cdata_;
   vector<string> ColName_;
   vector<long>   ldata_;
   vector<unsigned char>   bdata_;

 };



} // Fin du namespace

#endif
