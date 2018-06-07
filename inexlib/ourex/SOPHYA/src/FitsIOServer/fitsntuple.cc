#include "sopnamsp.h"
#include <string.h>
#include "pexceptions.h"
#include "fitsntuple.h"
#include <algorithm> //G.Barrand : _MSC_VER : for max
///////////////////////////////////////////////////////////
//   Les objets delegues pour la gestion de persistance sur fichiers fits 
//    pout NTuple
///////////////////////////////////////////////////////////


#define LONNOM 8
#define LONNOM1  (LONNOM+1)


/*!
  \class SOPHYA::FITS_NTuple
  \ingroup FitsIOServer
  FITS format I/O handler for SOPHYA::NTuple objects.
*/

FITS_NTuple::FITS_NTuple()
{
  dobj_ = new NTuple;
  InitNull();
  ownobj_ = true;
}

FITS_NTuple::FITS_NTuple(char inputfile[],int hdunum)
{
  dobj_ = new NTuple;
  InitNull();
  ownobj_ = true; 

  Read(inputfile,hdunum);
}


FITS_NTuple::FITS_NTuple(const NTuple & obj) 
{ 
  dobj_ = new NTuple(obj);
  InitNull();
  ownobj_ = true; 
}
FITS_NTuple::FITS_NTuple(NTuple* obj) 
{ 
  dobj_ = obj;
  InitNull();
  ownobj_ = false; 
}


FITS_NTuple::~FITS_NTuple()
{
  if (ownobj_ && dobj_ != NULL) delete dobj_;
  //  if (column_ != NULL) delete [] column_;
}

int  FITS_NTuple::CheckReadability(FitsInOutFile& is)
{
  if (is.CurrentHDUType() == IMAGE_HDU ) return 0;
  string key = "SOPCLSNM"; 
  string clsnm = is.KeyValue(key);
  if (clsnm == "SOPHYA::NTuple")  return 2;
  key = "Content";
  if (is.KeyValue(key) == "NTuple") return 2;
  else return 1;
}

//void FITS_NTuple::Read(char inputfile[],int hdunum)
//{
//  ReadF(inputfile,hdunum);
//}
void FITS_NTuple::ReadLines(char inputfile[],int firstLine, int numberOfLines,int hdunum)
{
  fistLineToBeRead_ = firstLine;
  numberOfLinesToBeRead_ = numberOfLines;
  Read(inputfile,hdunum);
}



//void FITS_NTuple::Write(char outputfile[], bool OldFile)
//{
//  WriteF(outputfile, OldFile);
//}

void FITS_NTuple::ReadFromFits(FitsInFile& is)
{
  if (!is.IsFitsTable())
    {
      throw PException("ReadFromFits: the fits file seems not to be a bintable nor ASCII table");
    }
  int nbcols, nbentries;
  nbcols = is.NbColsFromFits();
  nbentries = 0;
  int k;
  for (k=0; k<nbcols; k++) nbentries=max( nbentries, is.NentriesFromFits(k) );
  
  char ** ColName = new char*[nbcols];
  
  for (k=0; k<nbcols;k++) 
    {
      ColName[k] = new char[LONNOM1]; 
      strncpy(ColName[k], is.ColNameFromFits(k).c_str(),LONNOM);
      ColName[k][LONNOM] =  '\0';
    }
  // Reza Dec 2006 : Colonnes du NTuple de type double par defaut 
  for (k=0; k<nbcols;k++)
    {
      FitsFile::FitsDataType ss= is.ColTypeFromFits(k);
      string type;
      if ( (ss !=  FitsFile::FitsDataType_double) && (ss != FitsFile::FitsDataType_float) )
	{
	  if (ss == FitsFile::FitsDataType_int) type= string("integer");
	  else
	    if (ss == FitsFile::FitsDataType_char) type = string("char*");
	    else
	      type = string("unknown");
	  cout << " WARNING: the column " << k << " on fits file is not float but : " << type << endl;
	} 
    }
  bool fgdouble = true;
  if ( is.ColTypeFromFits(0) == FitsFile::FitsDataType_float ) fgdouble = false;
  if(dobj_ == NULL) 
    { 
      dobj_= new NTuple(nbcols,ColName,512,fgdouble);
      ownobj_ = true;      
    }
  else 
    {
      dobj_->Clean();
      (*dobj_) = NTuple(nbcols,ColName,512,fgdouble);
    }
  for (k=0; k<nbcols;k++) 
    {
      delete [] ColName[k];
    }
  delete [] ColName;

  // Reza Dec 2006 : Colonnes du NTuple de type double par defaut 
  double* dligne = NULL;
  float* fligne = NULL;
  if (fgdouble) dligne = new double[nbcols];
  else fligne = new float[nbcols];

  int firstln, lastln;
  if (numberOfLinesToBeRead_ > 0) 
    {
      firstln = fistLineToBeRead_;
      lastln = firstln + numberOfLinesToBeRead_;
    }
  else
    {
      firstln = 0;
      lastln  = nbentries;
    }
  int numLigne;
  if (fgdouble) 
    for (numLigne=firstln; numLigne < lastln; numLigne++) {
      is.GetBinTabLine(numLigne, dligne);
      dobj_->Fill((r_8*)dligne);
    }
  else 
    for (numLigne=firstln; numLigne < lastln; numLigne++) {
      is.GetBinTabLine(numLigne, fligne);
      dobj_->Fill((r_4*)fligne);
    }
  
  if (dligne) delete [] dligne;
  if (fligne) delete [] fligne;

  dobj_->Info()=is.DVListFromFits();
}

void FITS_NTuple::WriteToFits(FitsOutFile& os) 
{
  if(dobj_ == NULL) 
    {
      cout << " WriteToFits:: dobj_= null " << endl;
      return;
    }

  // table will have 'ncols'  columns
  int ncols = dobj_->NVar();

  // table will have 'nrows' rows
  int nentries = dobj_->NEntry();

  // get names and values from the join DVList object
  DVList dvl= dobj_->Info();
  dvl["Content"]= "NTuple";
  dvl.SetComment("Content", "name of SOPHYA object");
  dvl["SOPCLSNM"]= "SOPHYA::NTuple";
  dvl.SetComment("SOPCLSNM", "SOPHYA class name");
  // extension name
  // string extname("NTuple_Binary_tbl"); 
  string extname = os.NextExtensionName();

  vector<string> Noms(ncols);   
  int k;
  for (k=0; k< ncols; k++)
    {
      Noms[k]= dobj_->NomIndex(k);
    }

  // Gestion NTuple en double ou float / Reza Jan 2006
  if (dobj_->mFgDouble) {    // NTuple avec colonnes double
    string type(ncols, 'D');
    vector<int> dummy;
    // Reza 09/2006 : nentries -> 1  pour eviter les colonnes de type vecteur 
    os.makeHeaderBntblOnFits(type,Noms, 1, ncols, &dvl, extname, dummy);
    double* column =  new double[nentries];
    for (k=0; k<ncols;k++)
      {
	int j;
	for(j = 0; j < nentries; j++) column[j]= dobj_->GetCell(j,k);
	os.PutColToFits(k, nentries, column);
      }
    delete [] column;
  }
  else {   // NTuple avec colonnes float
    string type(ncols, 'E');
    vector<int> dummy;
    // Reza 09/2006 : nentries -> 1  pour eviter les colonnes de type vecteur 
    os.makeHeaderBntblOnFits(type,Noms, 1, ncols, &dvl, extname, dummy);
    float* column =  new float[nentries];
    for (k=0; k<ncols;k++)
      {
	int j;
	for(j = 0; j < nentries; j++) column[j]= dobj_->GetVal(j,k);
	os.PutColToFits(k, nentries, column);
      }
    delete [] column;
  }
}

