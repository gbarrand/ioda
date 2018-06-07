#include "machdefs.h"

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <typeinfo>

#include "histos.h"
#include "hisprof.h"
#include "histos2.h"

#include "fitsblkrw.h"
#include "fitshandler.h"

namespace SOPHYA {

////////////////////////////////////////////////////////////////
///////////////////////// Histo , HProf ////////////////////////
////////////////////////////////////////////////////////////////

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
int FitsHandler<Histo>::CheckReadability(FitsInOutFile& is)
{
  if (is.CurrentHDUType() == IMAGE_HDU ) return 0;
  string key = "SOPCLSNM";
  string clsnm = is.KeyValue(key);
  if ( (clsnm == "SOPHYA::Histo")
     || (clsnm == "SOPHYA::HProf") ) return 2;
  return 0;
}

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
int FitsHandler<Histo>::CheckHandling(AnyDataObj & o) 
{  
 if (typeid(o) == typeid(Histo)) return 2;
 Histo * po = dynamic_cast< Histo * >(& o); 
 if (po != NULL) return 2;
 return 0;
}

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void FitsHandler<Histo>::Write(FitsInOutFile& os)
{
  if(dobj==NULL)   
    throw NullPtrError("FitsHandler<Histo>::Write() NULL dobj pointer ");

  //--- Le type d'objet et son pointeur
  Histo*    h  = dynamic_cast< Histo *> (dobj);
  HProf*    hp = dynamic_cast< HProf *> (dobj);

  //--- Les noms de colonnes
  int tbltyp = os.GetDef_TableType();
  vector<string> colnames, tform, tunit;
  // les valeurs du bin
  if(tbltyp==ASCII_TBL) tform.push_back("D15.8"); else tform.push_back("D");
  colnames.push_back("val");
  tunit.push_back("");
  // Les erreurs
  if(h->mErr2) {
    if(tbltyp==ASCII_TBL) tform.push_back("D15.8"); else tform.push_back("D");
    colnames.push_back("e2");
    tunit.push_back("");
  }
  // Le nombre d'entrees dans le bin
  if(hp!=NULL) {
    if(tbltyp==ASCII_TBL) tform.push_back("D15.8"); else tform.push_back("D");
    colnames.push_back("nb");
    tunit.push_back("");
  }

  //--- On cree la table  
  string extname = os.NextExtensionName();
  os.CreateTable(os.GetDef_TableType(),extname,colnames,tform, tunit);

  // Ecriture des donnees des colonnes
  int n = h->NBins();
  if(n>0) {
    if(hp) hp->UpdateHisto();
    FitsBlockRW<r_8>::WriteColumnData(os,1,1,1,h->mData,n);
    if(h->mErr2) FitsBlockRW<r_8>::WriteColumnData(os,2,1,1,h->mErr2,n);
    if(hp!=NULL) FitsBlockRW<r_8>::WriteColumnData(os,3,1,1,hp->mSumW,n);
  }

  // Ecriture des clefs fits
  MuTyV mtv;

  mtv = "SOPHYA::Histo";
  if(hp) mtv = "SOPHYA::HProf";
  os.WriteKey("SOPCLSNM",mtv," SOPHYA class name");

  mtv = "Histo";
  if(hp) mtv = "HProf";
  os.WriteKey("CONTENT",mtv," name of SOPHYA object");

  mtv = h->mBins;
  os.WriteKey("NBIN",mtv," number of bins");

  mtv = h->mMin;
  os.WriteKey("XMIN",mtv," absc of beginning of 1srt bin");

  mtv = h->mMax;
  os.WriteKey("XMAX",mtv," absc of end of last bin");

  mtv = h->binWidth;
  os.WriteKey("WBIN",mtv," bin width");

  mtv = h->mUnder;
  os.WriteKey("UNDER",mtv," number of bins");

  mtv = h->mOver;
  os.WriteKey("OVER",mtv," underflow");

  mtv = h->nHist;
  os.WriteKey("NHIST",mtv," entries weighted somme");

  double x = h->nEntries; mtv = x;
  os.WriteKey("NENTRIES",mtv," number of entries");

  int_4 haserr =(h->mErr2) ? 1: 0;
  mtv = haserr;
  os.WriteKey("HASERR2",mtv," square errors associated");

  if(hp) {
    int_4 ok = (hp->mOk)? 1: 0;
    mtv = ok;
    os.WriteKey("UPDOK",mtv," update status flag");

    mtv = (int_4)hp->mOpt;
    os.WriteKey("SIGOPT",mtv," sigma statistic flag");

    mtv = hp->mYMin;
    os.WriteKey("YMIN",mtv," sum low limit");

    mtv = hp->mYMax;
    os.WriteKey("YMAX",mtv," sum high limit");
  }
  
  return;
}


DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void FitsHandler<Histo>::Read(FitsInOutFile& is)
{
 int hdutyp = is.CurrentHDUType();
 if( (hdutyp != BINARY_TBL ) && (hdutyp != ASCII_TBL) )
    throw FitsIOException("FitsHandler<Histo>::Read() Not a binary or ascii table HDU");

  //--- Nb de lignes et de colonnes 
  vector<string> colnames;
  vector<int> coltypes;
  vector<LONGLONG> repcnt, width;
  is.GetColInfo(colnames,coltypes,repcnt,width);
  long ncol = colnames.size();
  if(ncol<=0)
    throw FitsIOException("FitsHandler<Histo>::Read() bad number of table columns");
  long nbrows = is.GetNbRows();
  if(nbrows<=0)
    throw FitsIOException("FitsHandler<Histo>::Read() number of rows is zero, no reading");

  //--- Lecture entete FITS
  string key = "SOPCLSNM"; string clsnm = is.KeyValue(key);
  DVList dvl; is.GetHeaderRecords(dvl,true,false);

  int_4 nbin = dvl.GetI("NBIN",-1);
  if(nbin<=0 && nbin!=nbrows)
    throw FitsIOException("FitsHandler<Histo>::Read() number of bins is zero or bad, no reading");

  r_8 xmin = dvl.GetD("XMIN",-1.);
  r_8 xmax = dvl.GetD("XMAX",+1.);

  //--- Creation de l'objet si necessaire
  if(dobj == NULL) {
    if(clsnm == "SOPHYA::Histo") dobj = new Histo;
    else if(clsnm == "SOPHYA::HProf") dobj = new HProf;
  }

  //--- Type de l'histo
  Histo*    h  = dynamic_cast< Histo *> (dobj);
  HProf*    hp = dynamic_cast< HProf *> (dobj);

  //--- Allocation pour histo
  if(hp&& (clsnm=="SOPHYA::HProf")) {
    if(ncol<3)
      throw FitsIOException("FitsHandler<Histo>::Read() wrong number of columns for HProf");
    r_8 ymin = dvl.GetD("YMIN",1.);
    r_8 ymax = dvl.GetD("YMAX",-1.);
    hp->CreateOrResize(xmin,xmax,nbin,ymin,ymax);
  } else if(h && clsnm == "SOPHYA::Histo" ) {
    h->CreateOrResize(xmin,xmax,nbin);
    int_4 haserr2 = dvl.GetI("HASERR2",0);
    if(ncol>1 && haserr2>0) h->Errors();
  } else {
    throw FitsIOException("FitsHandler<Histo>::Read() No assocaition classe/fits_header");
  }

  //--- remplissage des variables d'entete
  h->mUnder = dvl.GetD("UNDER",0.);
  h->mOver = dvl.GetD("OVER",0.);
  h->nHist = dvl.GetD("NHIST",0.);
  double x = dvl.GetD("NENTRIES",0.); h->nEntries = uint_8(x);

  if(hp) {
    int_4 ok = dvl.GetI("UPDOK",0); hp->mOk = (ok)? true : false;
    hp->mOpt = (uint_2)dvl.GetI("SIGOPT",0);
    hp->mYMin = dvl.GetD("YMIN",1.);
    hp->mYMax = dvl.GetD("YMAX",-1.);
  }

  //--- remplissage de l'histo
  FitsBlockRW<r_8>::ReadColumnData(is,1,1,1,h->mData,nbin);
  if(h->mErr2) FitsBlockRW<r_8>::ReadColumnData(is,2,1,1,h->mErr2,nbin);
  if(hp) FitsBlockRW<r_8>::ReadColumnData(is,3,1,1,hp->mSumW,nbin);

  return;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////// Histo2D  ////////////////////////////////
///////////////////////////////////////////////////////////////////////////

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
int FitsHandler<Histo2D>::CheckReadability(FitsInOutFile& is)
{
  if (is.CurrentHDUType() == IMAGE_HDU ) return 0;
  string key = "SOPCLSNM"; 
  string clsnm = is.KeyValue(key);
  if( clsnm == "SOPHYA::Histo2D") return 2;
  return 0;
}

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
int FitsHandler<Histo2D>::CheckHandling(AnyDataObj & o) 
{  
 if (typeid(o) == typeid(Histo2D)) return 2;
 Histo2D * po = dynamic_cast< Histo2D * >(& o); 
 if (po != NULL) return 2;
 return 0;
}

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void FitsHandler<Histo2D>::Write(FitsInOutFile& os)
{
  if(dobj==NULL)   
    throw NullPtrError("FitsHandler<Histo2D>::Write() NULL dobj pointer ");

  //--- Le type d'objet et son pointeur
  Histo2D* h2 = dynamic_cast< Histo2D *> (dobj);

  //--- Les noms de colonnes
  int tbltyp = os.GetDef_TableType();
  vector<string> colnames, tform, tunit;
  // les valeurs du bin
  if(tbltyp==ASCII_TBL) tform.push_back("D15.8"); else tform.push_back("D");
  colnames.push_back("val");
  tunit.push_back("");
  // Les erreurs
  if(h2->mErr2) {
    if(tbltyp==ASCII_TBL) tform.push_back("D15.8"); else tform.push_back("D");
    colnames.push_back("e2");
    tunit.push_back("");
  }

  //--- On cree la table  
  string extname = os.NextExtensionName();
  os.CreateTable(os.GetDef_TableType(),extname,colnames,tform, tunit);

  // Ecriture des donnees des colonnes
  long n = h2->mNxy;
  if(n>0) {
    FitsBlockRW<r_8>::WriteColumnData(os,1,1,1,h2->mData,n);
    if(h2->mErr2) FitsBlockRW<r_8>::WriteColumnData(os,2,1,1,h2->mErr2,n);
  }

  // Ecriture des clefs fits
  MuTyV mtv;

  mtv = "SOPHYA::Histo2D";
  os.WriteKey("SOPCLSNM",mtv," SOPHYA class name");

  mtv = "Histo2D";
  os.WriteKey("CONTENT",mtv," name of SOPHYA object");

  mtv = h2->mNx;
  os.WriteKey("NBINX",mtv," number of bins in X");
  mtv = h2->mNy;
  os.WriteKey("NBINY",mtv," number of bins in Y");
  mtv = h2->mNxy;
  os.WriteKey("NBINXY",mtv," number of elements");

  mtv = h2->mXmin;
  os.WriteKey("XMIN",mtv," absc of beginning of 1srt bin in X");
  mtv = h2->mXmax;
  os.WriteKey("XMAX",mtv," absc of end of last bin in X");
  mtv = h2->mYmin;
  os.WriteKey("YMIN",mtv," absc of beginning of 1srt bin in Y");
  mtv = h2->mYmax;
  os.WriteKey("YMAX",mtv," absc of end of last bin in Y");

  mtv = h2->mWBinx;
  os.WriteKey("WBINX",mtv," bin width in X");
  mtv = h2->mWBiny;
  os.WriteKey("WBINY",mtv," bin width in Y");

  for(int i=0;i<3;i++) for(int j=0;j<3;j++) {
    char str[16]; sprintf(str,"OUT%1d%1d",i,j);
    mtv = h2->mOver[i][j];
    os.WriteKey(str,mtv," under/over X/Y");
  }

  mtv = h2->nHist;
  os.WriteKey("NHIST",mtv," entries weighted somme");

  mtv = h2->nEntries;
  os.WriteKey("NENTRIES",mtv," number of entries");

  int_4 haserr =(h2->mErr2) ? 1: 0;
  mtv = haserr;
  os.WriteKey("HASERR2",mtv," square errors associated");



  //-------------------------------------------------------------
  //------ Gestion des Histo1D de projx/y bandx/t slicex/y ------
  //-------------------------------------------------------------

  int_4 nrel = 0;
  if(h2->HProjX()) {nrel++; mtv=nrel; os.WriteKey("PROJX",mtv," relative HDU with HProjX");}
  if(h2->HProjY()) {nrel++; mtv=nrel; os.WriteKey("PROJY",mtv," relative HDU with HProjY");}
  if(h2->NSliX()>0) {
    mtv=h2->NSliX(); os.WriteKey("NSLICEX",mtv," number of SliX");
    nrel++;
    mtv=nrel; os.WriteKey("SLICEX",mtv," relative HDU with first SliX");
    nrel += h2->NSliX()-1;
  }
  if(h2->NSliY()>0) {
    mtv=h2->NSliY(); os.WriteKey("NSLICEY",mtv," number of SliY");
    nrel++;
    mtv=nrel; os.WriteKey("SLICEY",mtv," relative HDU with first SliY");
    nrel += h2->NSliY()-1;
  }
  if(h2->NBandX()>0) {
    mtv=h2->NBandX(); os.WriteKey("NBANDEX",mtv," number of BandX");
    nrel++;
    mtv=nrel; os.WriteKey("BANDEX",mtv," relative HDU with first BandX");
    nrel += h2->NBandX()-1;
    for(int i=0;i<h2->NBandX();i++) {
      char str[32]; r_8 vmin,vmax;
      h2->GetBandX(i,vmin,vmax);
      sprintf(str,"BXL%d",i);
      mtv = vmin; os.WriteKey(str,mtv," low Y limit for BandX");
      sprintf(str,"BXH%d",i);
      mtv = vmax; os.WriteKey(str,mtv," high Y limit for BandX");
    }
  }
  if(h2->NBandY()>0) {
    mtv=h2->NBandY(); os.WriteKey("NBANDEY",mtv," number of BandY");
    nrel++;
    mtv=nrel; os.WriteKey("BANDEY",mtv," relative HDU with first BandY");
    nrel += h2->NBandY()-1;
    for(int i=0;i<h2->NBandY();i++) {
      char str[32]; r_8 vmin,vmax;
      h2->GetBandY(i,vmin,vmax);
      sprintf(str,"BYL%d",i);
      mtv = vmin; os.WriteKey(str,mtv," low X limit for BandY");
      sprintf(str,"BYH%d",i);
      mtv = vmax; os.WriteKey(str,mtv," high X limit for BandY");
    }
  }
  mtv=nrel; os.WriteKey("NH1ASS",mtv," number of associated Histo1D");

  if(h2->HProjX()) {FitsHandler<Histo> fio(const_cast<Histo &>(*(h2->HProjX()))); fio.Write(os);}
  if(h2->HProjY()) {FitsHandler<Histo> fio(const_cast<Histo &>(*(h2->HProjY()))); fio.Write(os);}
  if(h2->NSliX()>0) for(int i=0;i<h2->NSliX();i++)
    {FitsHandler<Histo> fio(const_cast<Histo &>(*(h2->HSliX(i)))); fio.Write(os);}
  if(h2->NSliY()>0) for(int i=0;i<h2->NSliY();i++)
    {FitsHandler<Histo> fio(const_cast<Histo &>(*(h2->HSliY(i)))); fio.Write(os);}
  if(h2->NBandX()>0) for(int i=0;i<h2->NBandX();i++)
    {FitsHandler<Histo> fio(const_cast<Histo &>(*(h2->HBandX(i)))); fio.Write(os);}
  if(h2->NBandY()>0) for(int i=0;i<h2->NBandY();i++)
    {FitsHandler<Histo> fio(const_cast<Histo &>(*(h2->HBandY(i)))); fio.Write(os);}

  return;
}

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void FitsHandler<Histo2D>::Read(FitsInOutFile& is)
{
 int hdutyp = is.CurrentHDUType();
 if( (hdutyp != BINARY_TBL ) && (hdutyp != ASCII_TBL) )
    throw FitsIOException("FitsHandler<Histo2D>::Read() Not a binary or ascii table HDU");

  //--- Nb de lignes et de colonnes 
  vector<string> colnames; vector<int> coltypes; vector<LONGLONG> repcnt, width;
  is.GetColInfo(colnames,coltypes,repcnt,width);
  long ncol = colnames.size();
  if(ncol<=0)
    throw FitsIOException("FitsHandler<Histo2D>::Read() bad number of table columns");
  long nbrows = is.GetNbRows();
  if(nbrows<=0)
    throw FitsIOException("FitsHandler<Histo2D>::Read() number of rows is zero, no reading");

  //--- Lecture entete FITS
  DVList dvl; is.GetHeaderRecords(dvl,true,false);

  int_4 nbinx = dvl.GetI("NBINX",-1);
  int_4 nbiny = dvl.GetI("NBINY",-1);
  int_8 nbinxy = nbinx*nbiny;
  if(nbinx<=0 || nbiny<=0 || nbinxy!=nbrows)
    throw FitsIOException("FitsHandler<Histo2D>::Read() number of bins is zero or bad, no reading");

  r_8 xmin = dvl.GetD("XMIN",-1.);
  r_8 xmax = dvl.GetD("XMAX",1.);
  r_8 ymin = dvl.GetD("YMIN",-1.);
  r_8 ymax = dvl.GetD("YMAX",1.);

  //--- Creation de l'objet
  if(dobj == NULL) dobj = new Histo2D;
  dobj->CreateOrResize(xmin,xmax,nbinx,ymin,ymax,nbiny);
  int_4 haserr2 = dvl.GetI("HASERR2",0);
  if(ncol>1 && haserr2>0) dobj->Errors();

  //--- remplissage des variables d'entete
  dobj->nHist = dvl.GetD("NHIST",0.);
  dobj->nEntries = dvl.GetI("NENTRIES",0);

  for(int i=0;i<3;i++) for(int j=0;j<3;j++) {
    char str[16]; sprintf(str,"OUT%1d%1d",i,j);
    dobj->mOver[i][j] = dvl.GetD(str,0.);
  }

  //--- remplissage de l'histo
  FitsBlockRW<r_8>::ReadColumnData(is,1,1,1,dobj->mData,nbinxy);
  if(dobj->mErr2) FitsBlockRW<r_8>::ReadColumnData(is,2,1,1,dobj->mErr2,nbinxy);



  //-------------------------------------------------------------
  //------ Gestion des Histo1D de projx/y bandx/t slicex/y ------
  //-------------------------------------------------------------

  int_4 nh1ass = dvl.GetI("NH1ASS",-1);
  if(nh1ass<=0) return;
  int hducur = is.CurrentHDU();

  try {  // ProjX
    int_4 ipr = dvl.GetI("PROJX",-1);
    is.MoveAbsToHDU(hducur+ipr);
    dobj->SetProjX();
    Histo *h = dobj->HProjX();
    FitsHandler<Histo> fio(*h); fio.Read(is);
    if(h->NBins()!=dobj->NBinX())
      throw FitsIOException("unmatched bin number");
  } catch (...) {
    dobj->DelProjX();
    cout<<"FitsHandler<Histo2D>::Read: Error reading PROJX"<<endl;
  }

  try {  // ProjY
    int_4 ipr = dvl.GetI("PROJY",-1);
    is.MoveAbsToHDU(hducur+ipr);
    dobj->SetProjY();
    Histo *h = dobj->HProjY();
    FitsHandler<Histo> fio(*h); fio.Read(is);
    if(h->NBins()!=dobj->NBinY())
      throw FitsIOException("unmatched bin number");
  } catch (...) {
    dobj->DelProjY();
    cout<<"FitsHandler<Histo2D>::Read: Error reading PROJY"<<endl;
  }

  try {  // SliX
    int_4 nb = dvl.GetI("NSLICEX",-1);
    int_4 ipr = dvl.GetI("SLICEX",-1);
    dobj->SetSliX(nb);
    for(int i=0;i<nb;i++) {
      is.MoveAbsToHDU(hducur+ipr+i);
      Histo *h = dobj->HSliX(i);
      FitsHandler<Histo> fio(*h); fio.Read(is);
      if(h->NBins()!=dobj->NBinX())
        throw FitsIOException("unmatched bin number");
    }
  } catch (...) {
    dobj->DelSliX();
    cout<<"FitsHandler<Histo2D>::Read: Error reading SLICEX"<<endl;
  }

  try {  // SliY
    int_4 nb = dvl.GetI("NSLICEY",-1);
    int_4 ipr = dvl.GetI("SLICEY",-1);
    dobj->SetSliY(nb);
    for(int i=0;i<nb;i++) {
      is.MoveAbsToHDU(hducur+ipr+i);
      Histo *h = dobj->HSliY(i);
      FitsHandler<Histo> fio(*h); fio.Read(is);
      if(h->NBins()!=dobj->NBinY())
        throw FitsIOException("unmatched bin number");
    }
  } catch (...) {
    dobj->DelSliY();
    cout<<"FitsHandler<Histo2D>::Read: Error reading SLICEY"<<endl;
  }

  try {  // BandeX
    int_4 nb = dvl.GetI("NBANDEX",-1);
    int_4 ipr = dvl.GetI("BANDEX",-1);
    for(int i=0;i<nb;i++) {
      char str[32];
      sprintf(str,"BXL%d",i); r_8 vmin = dvl.GetD(str,0.);
      sprintf(str,"BXH%d",i); r_8 vmax = dvl.GetD(str,0.);
      dobj->SetBandX(vmin,vmax);
      is.MoveAbsToHDU(hducur+ipr+i);
      Histo *h = dobj->HBandX(i);
      FitsHandler<Histo> fio(*h); fio.Read(is);
      if(h->NBins()!=dobj->NBinX())
        throw FitsIOException("unmatched bin number");
    }
  } catch (...) {
    dobj->DelBandX();
    cout<<"FitsHandler<Histo2D>::Read: Error reading BANDEX"<<endl;
  }

  try {  // BandeY
    int_4 nb = dvl.GetI("NBANDEY",-1);
    int_4 ipr = dvl.GetI("BANDEY",-1);
    for(int i=0;i<nb;i++) {
      char str[32];
      sprintf(str,"BYL%d",i); r_8 vmin = dvl.GetD(str,0.);
      sprintf(str,"BYH%d",i); r_8 vmax = dvl.GetD(str,0.);
      dobj->SetBandY(vmin,vmax);
      is.MoveAbsToHDU(hducur+ipr+i);
      Histo *h = dobj->HBandY(i);
      FitsHandler<Histo> fio(*h); fio.Read(is);
      if(h->NBins()!=dobj->NBinY())
        throw FitsIOException("unmatched bin number");
    }
  } catch (...) {
    dobj->DelBandY();
    cout<<"FitsHandler<Histo2D>::Read: Error reading BANDEY"<<endl;
  }

  return;
}

} // FIN namespace SOPHYA 
