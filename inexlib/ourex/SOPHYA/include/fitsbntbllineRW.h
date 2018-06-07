// Guy Le Meur 06/2000


#ifndef FITSBntblLineRW_SEEN
#define FITSBntblLineRW_SEEN
#include "dvlist.h"
#include "fitsfile.h"

namespace SOPHYA {
///////////////////////////////////////////////////////////
//   Lecture ligne par ligne d'une BINTABLE sur fichier FITS 
//   
///////////////////////////////////////////////////////////

class  FITS_BntblLineReader : public FitsIOHandler  
{

public:
FITS_BntblLineReader();
FITS_BntblLineReader(const char inputfile[],int hdunum=2);
~FITS_BntblLineReader();

BnTblLine& ReadNextLine();
 inline  BnTblLine& GetBnTblLine()  {return ligne_;}
 inline long GetNextLineIndex() const {return nextLineToBeRead_;}
 inline  void SetStartingLineIndex(long n)  { nextLineToBeRead_ = n;}

 inline int status() const {return inFits_->statusF();};
 inline string GetStatus(int status) const {return inFits_->GetErrStatus(status);}; 

// Renvoie une reference sur l''objet DVList Associe
inline const DVList& DVListFromFits() const
  {
    return(inFits_->DVListFromFits());
  }


protected:

  // implementation de  FitsIOHandler
  virtual void  ReadFromFits(FitsInFile& is) {};
virtual void    WriteToFits(FitsOutFile& os) {};

 private :

inline void InitNull()
   {
     inFits_ = NULL;
     nextLineToBeRead_= 0;
   }
  // attributs de classe

 FitsInFile* inFits_;
 long nextLineToBeRead_;
 BnTblLine ligne_;
};


//////////////////////////////////////////////////////////////////


class  FITS_BntblLineWriter : public FitsIOHandler  
{


public:
 FITS_BntblLineWriter(const char inputfile[],int dc, int fc, int ic, int lc, int bc,int cc, vector<string> names, DVList* dvl=NULL, FitsFile::WriteMode wrm = FitsFile::clear);
~FITS_BntblLineWriter();
 void WriteNextLine( BnTblLine& WorkLine);

// Renvoie une reference sur l''objet DVList Associe 
 //inline const DVList& DVListFromFits() const
 //  {
 //   return(outFits_->DVListFromFits());
 // }

 private :

inline void InitNull()
   {
     outFits_ = NULL;
     nextLineToBeWritten_= 0;
   }

protected:

  // implementation de  FitsIOHandler
  virtual void  ReadFromFits(FitsInFile& is) {};
virtual void    WriteToFits(FitsOutFile& os) {};


  // attributs de classe

 FitsOutFile* outFits_;
  long nextLineToBeWritten_;
  BnTblLine ligne_;
};


} // Fin du namespace

#endif
