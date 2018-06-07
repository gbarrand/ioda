#include "sopnamsp.h"
#include "pexceptions.h"
#include "fitsbntbllineRW.h"
#include "utils.h"
#include <algorithm> //G.Barrand : _MSC_VER : for max
///////////////////////////////////////////////////////////
//    
//    
///////////////////////////////////////////////////////////


#define LONNOM 31 


FITS_BntblLineReader::FITS_BntblLineReader()
{
  InitNull();
}

FITS_BntblLineReader::FITS_BntblLineReader(const char inputfile[],int hdunum) 
                                                                 
{
  InitNull();
  inFits_  = new FitsInFile (inputfile);
  inFits_->ReadHeader(hdunum);

  //  if (!fn->IsFitsTable())
  if (!inFits_->IsFitsTable())
    {
      throw PException("FITS_BntblLineReader: the fits file seems not to be a bintable nor ASCII table");
    }

  //
  int nbcols, nbentries;
  //  nbcols = fn->NbColsFromFits();
  nbcols = inFits_->NbColsFromFits();
  nbentries = 0;
  int k;
  //  for (k=0; k<nbcols; k++) nbentries=max( nbentries, fn->NentriesFromFits(k) );
  for (k=0; k<nbcols; k++) nbentries=max( nbentries, inFits_->NentriesFromFits(k) );

  //
  // pour mettre les colonnes dans l'ordre double, float, int, char :
  // tableau de correspondance 
  // DfitsCol(j)= numero dans le fichier fits de la  jeme variable double du 
  // xntuple;
  // FfitsCol(j)= numero dans le fichier fits de la  jeme variable float du 
  // xntuple;
  // etc.
  vector<int> DfitsCol;
  vector<int> FfitsCol;
  vector<int> IfitsCol;
  vector<int> SfitsCol;
  vector<int> LfitsCol;
  vector<int> BfitsCol;
  for (k=0; k<nbcols;k++)
    {
       FitsFile::FitsDataType ss= inFits_->ColTypeFromFits(k);

      if (ss == FitsFile::FitsDataType_double) DfitsCol.push_back(k);
      else if (ss == FitsFile::FitsDataType_float) FfitsCol.push_back(k);
      else if (ss == FitsFile::FitsDataType_int) IfitsCol.push_back(k);
      else if (ss == FitsFile::FitsDataType_long) LfitsCol.push_back(k);
      else if (ss == FitsFile::FitsDataType_byte) BfitsCol.push_back(k);
      else if (ss == FitsFile::FitsDataType_char) SfitsCol.push_back(k);
      else {
	cout << "FITS_BntblLineReader: colonne fits " << k << " type= " << (int) ss << endl;
	throw  IOExc("type de champ inconnu");
      }
    }
  vector<string> ColN(nbcols);
  int compt=0;
  for (k=0; k<DfitsCol.size(); k++)
    {
      ColN[compt++] = inFits_->ColNameFromFits(DfitsCol[k]);
    }
  for (k=0; k<FfitsCol.size(); k++)
    {
      ColN[compt++] =  inFits_->ColNameFromFits(FfitsCol[k]);
    }
  for (k=0; k<IfitsCol.size(); k++)
    {
      ColN[compt++] =  inFits_->ColNameFromFits(IfitsCol[k]);
    }
  for (k=0; k<LfitsCol.size(); k++)
    {
      ColN[compt++] =  inFits_->ColNameFromFits(LfitsCol[k]);
    }
  for (k=0; k<BfitsCol.size(); k++)
    {
      ColN[compt++] =  inFits_->ColNameFromFits(BfitsCol[k]);
    }
  for (k=0; k<SfitsCol.size(); k++)
    {
      ColN[compt++] = inFits_->ColNameFromFits(SfitsCol[k]);
    }

  ligne_.setFormat(DfitsCol.size(), FfitsCol.size(), IfitsCol.size(), LfitsCol.size(), BfitsCol.size(),SfitsCol.size(),  ColN);
}


FITS_BntblLineReader::~FITS_BntblLineReader()
{
  if (inFits_ != NULL) delete inFits_;
}

BnTblLine& FITS_BntblLineReader::ReadNextLine()
{
  inFits_->GetBinTabLine(nextLineToBeRead_++, ligne_);
  return ligne_;
}




FITS_BntblLineWriter::FITS_BntblLineWriter(const char inputfile[],int dc, int fc,int ic, int lc, int bc, int cc,  vector<string> names,DVList* ptr_dvl,  FitsFile::WriteMode wrm)
{
  int k;
  int nbcols = dc+fc+ic+cc+lc+bc;
  if (nbcols !=  (int)names.size())
    {
      cout << " WARNING: BnTblLineWriter:: length of vector of column names not equal to total number of columns" << endl;
    }
  InitNull();
  outFits_  = new FitsOutFile (inputfile, wrm);
  string types;
  vector<int> StringSizes(cc);
  for (k=0; k<dc;k++)
    {
      types+='D';
    }
  for (k=0; k<fc;k++)
    {
      types+='E';
    }
  for (k=0; k<ic;k++)
    {
      types+='J';
    }
  for (k=0; k<lc;k++)
    {
      types+='J';
    }
  for (k=0; k<bc;k++)
    {
      types+='B';
    }
  for (k=0; k<cc;k++)
    {
      types+='A';
      StringSizes[k] = names[dc+fc+ic+lc+bc+k].length();
    }            
  string extname("Lines_on_Binary_tbl"); 

  outFits_->makeHeaderBntblOnFits(types, names, 1, nbcols, ptr_dvl, extname,StringSizes); 
    ligne_.setFormat(dc, fc, ic,lc,bc,cc, names);
    
}

FITS_BntblLineWriter::~FITS_BntblLineWriter()
{
  //  if (dvl_ != NULL) delete dvl_;
  if (outFits_ != NULL)  delete outFits_;
}

void FITS_BntblLineWriter::WriteNextLine( BnTblLine& WorkLine)
{
  if (! ligne_.sameFormat(WorkLine) )
    {
    	      throw PException("  FITS_BntblLineWriter:: line to be written does not match the header");
    }
  outFits_->PutBinTabLine(nextLineToBeWritten_++, WorkLine);

}
