#include "machdefs.h"

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <typeinfo>

#include "datatable.h"
#include "fitsblkrw.h"
#include "fitshandler.h"
#include "swfitsdtable.h"

namespace SOPHYA {

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
int FitsHandler<BaseDataTable>::CheckReadability(FitsInOutFile& is)
{
  if (is.CurrentHDUType() == IMAGE_HDU ) return 0;
  string key = "SOPCLSNM"; 
  string clsnm = is.KeyValue(key);
  if ( (clsnm == "SOPHYA::DataTable") || (clsnm == "SOPHYA::SwFitsDataTable") )
    return 2;
  else return 1;
}

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void FitsHandler<BaseDataTable>::Write(FitsInOutFile& os)
{
  
  if (dobj == NULL)   
    throw NullPtrError("FitsHandler<DataTable>::Write() NULL dobj pointer ");

  DataTable* dt = dynamic_cast< DataTable *> (dobj);
  SwFitsDataTable* swfdt = dynamic_cast< SwFitsDataTable *> (dobj);

  int tbltyp = os.GetDef_TableType();
  if ( swfdt && (tbltyp == ASCII_TBL) )
    throw FitsIOException("FitsHandler<DataTable>::Write() ASCII_TBL not supported for SwFitsDataTable");

  // On ne reecrit pas la definition d'une SwFitsDataTable dans son fichier de swap
  if (swfdt && (swfdt->mSwF.FitsPtr()==os.FitsPtr()) && swfdt->NRows() )  return;

  string strcoltag;
  long strwidth = os.GetDef_StrColWidth(); 
  if (strwidth < 1) strwidth = 16;
  char buff[32];
  if (tbltyp == ASCII_TBL) 
    sprintf(buff, "A%ld",strwidth);
  else 
    sprintf(buff, "%ldA",strwidth);
  strcoltag = buff;

  vector<string> colnames, tform, tunit;
  vector<bool> colfgok;
  string str_tform;
  char buff_tform[32];
  for(sa_size_t k=0; k<dobj->NVar(); k++) { 
    bool fgoknm = true;
    bool fgoktun = true;
    size_t cvsz=dobj->GetColumnVecSize(k);
    BaseDataTable::FieldType ftyp = dobj->GetColumnType(k);
    if ((tbltyp == ASCII_TBL)&&(cvsz>1)) {
      cout << "FitsHandler<BaseDataTable>::Write()/Warning Column with vector content GetColumnVecSize(k)>1 not supported for ASCII_TBL" 
	   << endl;
      fgoknm = false;
      continue;
    }
    if ((cvsz>1)&&( (ftyp==BaseDataTable::StringField)||(ftyp==BaseDataTable::FMLStr16Field)||(ftyp==BaseDataTable::FMLStr64Field)) ) {
      cout << "FitsHandler<BaseDataTable>::Write()/Warning string/String16/Strin64 Colunm with vector content not supported" 
	   << endl;
      fgoknm = false;
      continue;

    }
    // Pour gerer les colonnes avec contenu de type vecteur, TFORM nJ nK nE nD ... ou n=GetColumnVecSize()
    sprintf(buff_tform,"%ld",(long)cvsz);
    str_tform=buff_tform;

    switch ( dobj->GetColumnType(k) ) {
    case BaseDataTable::IntegerField :
      if (tbltyp == ASCII_TBL) 	tform.push_back("I9");
      else  tform.push_back((str_tform+"J"));
      break;
    case BaseDataTable::LongField :
      if (tbltyp == ASCII_TBL) tform.push_back("I12");
      else tform.push_back((str_tform+"K"));
      break;
    case BaseDataTable::FloatField :
      if (tbltyp == ASCII_TBL) tform.push_back("E12.5");
      else tform.push_back((str_tform+"E"));
      break;
    case BaseDataTable::DoubleField :
    case BaseDataTable::DateTimeField :
      if (tbltyp == ASCII_TBL) tform.push_back("D15.8");
      else tform.push_back((str_tform+"D"));
      break;
    case BaseDataTable::ComplexField :
      if (tbltyp == ASCII_TBL) { 
	cout << "FitsHandler<BaseDataTable>::Write()/Warning ComplexField not supported for ASCII_TBL " 
	     << endl;
	fgoknm = false;
      }
      else   tform.push_back((str_tform+"C"));
      break;
    case BaseDataTable::DoubleComplexField :
      if (tbltyp == ASCII_TBL) {
	cout << "FitsHandler<BaseDataTable>::Write()/Warning DoubleComplexField not supported for ASCII_TBL " 
	     << endl;
	fgoknm = false;
      }
      else   tform.push_back((str_tform+"M"));
      break;
    case BaseDataTable::StringField :
      tform.push_back(strcoltag);
      break;
    case BaseDataTable::FMLStr16Field :
      if (tbltyp == ASCII_TBL) tform.push_back("A16");
      else tform.push_back("16A");
      break;
    case BaseDataTable::FMLStr64Field :
      if (tbltyp == ASCII_TBL) tform.push_back("A64");
      else tform.push_back("64A");
      break;
    default:
      throw IOExc("FitsHandler<BaseDataTable>::Write() unknown column type ");
      break;
    }
    if (fgoknm) {
      // Pour gerer les unites physiques associees aux colonnes 
      MuTyV mtv;
      bool sdone=false;
      Units un=dobj->GetUnits(k,sdone);
      if (dobj->GetColumnType(k) == BaseDataTable::DateTimeField) {
	tunit.push_back("Days (DateTimeField)");
      }
      else {
	if (sdone)  tunit.push_back(un.Name());
	else tunit.push_back("");
      }
      colnames.push_back(dobj->GetColumnName(k));
    }
    colfgok.push_back(fgoknm);
  }

  // On cree la table  
  string extname = os.NextExtensionName();
  os.CreateTable(os.GetDef_TableType(), extname, colnames, tform, tunit);

  // On n'ecrit pas les donnees d'une table SwFitsDataTable ds son fichier de swap 
  if (swfdt && (swfdt->mSwF.FitsPtr()==os.FitsPtr()) )  return;

  // Ecriture des donnees des colonnes
  for(sa_size_t l=0; l<dobj->NEntry(); l+=dobj->SegmentSize()) { 
    sa_size_t icol = 0;
    sa_size_t sszt = dobj->SegmentSize();
    sa_size_t iseg = l/sszt;
    if ((l+sszt) > dobj->NEntry()) sszt = dobj->NEntry()-l;
    for(sa_size_t k=0; k<dobj->NVar(); k++) {
      if (!colfgok[k])  continue;
      sa_size_t sk = dobj->mNames[k].ser;
      sa_size_t sz = sszt*dobj->GetColumnVecSize(k); 
      //DBG cout << "DBG-A k= " << k << " nom=" << dobj->mNames[k].nom << " ser=" << sk
      //DBG   << " type=" << dobj->GetColumType(k) << endl;
      switch ( dobj->GetColumnType(k) ) {
      case BaseDataTable::IntegerField :
	icol++;
	FitsBlockRW<int_4>::WriteColumnData(os, icol, l+1, 1, 
					    dobj->mIColsP[sk]->GetCstSegment(iseg), sz);
	break;
      case BaseDataTable::LongField :
	icol++;
	FitsBlockRW<int_8>::WriteColumnData(os, icol, l+1, 1, 
					    dobj->mLColsP[sk]->GetCstSegment(iseg), sz);
	break;
      case BaseDataTable::FloatField :
	icol++;
	FitsBlockRW<r_4>::WriteColumnData(os, icol, l+1, 1, 
					  dobj->mFColsP[sk]->GetCstSegment(iseg), sz);
	break;
      case BaseDataTable::DoubleField :
	icol++;
	FitsBlockRW<r_8>::WriteColumnData(os, icol, l+1, 1, 
					  dobj->mDColsP[sk]->GetCstSegment(iseg), sz);
	break;
      case BaseDataTable::ComplexField :
	if (tbltyp == BINARY_TBL) { 
	  icol++;
	  FitsBlockRW< complex<r_4> >::WriteColumnData(os, icol, l+1, 1, 
						       dobj->mYColsP[sk]->GetCstSegment(iseg), sz);
	}
	break;
      case BaseDataTable::DoubleComplexField :
	if (tbltyp == BINARY_TBL) { 
	  icol++;
	  FitsBlockRW< complex<r_8> >::WriteColumnData(os, icol, l+1, 1, 
						       dobj->mZColsP[sk]->GetCstSegment(iseg), sz);
	}
	break;
      case BaseDataTable::StringField :
	icol++;
	FitsBlockRW<std::string>::WriteColumnData(os, icol, l+1, 1, 
						  dobj->mSColsP[sk]->GetCstSegment(iseg), sz);
	break;
      case BaseDataTable::DateTimeField :
	icol++;
	FitsBlockRW<TimeStamp>::WriteColumnData(os, icol, l+1, 1, 
						dobj->mTColsP[sk]->GetCstSegment(iseg), sz);
	break;
      case BaseDataTable::FMLStr16Field :
	icol++;
	FitsBlockRW<String16>::WriteColumnData(os, icol, l+1, 1, 
						  dobj->mS16ColsP[sk]->GetCstSegment(iseg), sz);
      case BaseDataTable::FMLStr64Field :
	icol++;
	FitsBlockRW<String64>::WriteColumnData(os, icol, l+1, 1, 
						  dobj->mS64ColsP[sk]->GetCstSegment(iseg), sz);
	break;
      default:
	break;
      }
    }
  }

  // Ecriture de SegmentSize et autre elements de DVList  
  os.WriteHeaderRecords(dobj->Info());
  MuTyV mtv = (uint_8)dobj->SegmentSize();
  os.WriteKey("SEGMSIZE",mtv," SOPHYA::DataTable SegmentSize");
  mtv = "SOPHYA::DataTable";
  os.WriteKey("SOPCLSNM",mtv," Object class name "); 
  // Ecriture des unites de colonnes
  size_t kk=0;
  for(sa_size_t k=0; k<dobj->NVar(); k++) {
    if (!colfgok[k])  continue;
    MuTyV mtv;
    bool sdone=false;
    Units un=dobj->GetUnits(k,sdone);
    if (!sdone) continue;
    char keynb[32];
    kk++;  sprintf(keynb,"SOPUN%d",(int)kk);
    mtv = un.ToStringFullForm();
    os.WriteKey(keynb,mtv,"SOPHYA unit"); 
  }
}



DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void FitsHandler<BaseDataTable>::Read(FitsInOutFile& is)
{
 
  int hdutyp = is.CurrentHDUType();
  if ( (hdutyp != BINARY_TBL ) && (hdutyp != ASCII_TBL) )
    throw FitsIOException("FitsHandler<DataTable>::Read() Not a binary or ascii table HDU");

  // Determination de la taille de segment 
  sa_size_t segsz = is.GetNbRows() / 64; // Taille de segment par defaut 
  if (segsz > 2048) segsz = 2048;
  string key = "SEGMSIZE";
  bool knex = false;
  string ssegsz = is.KeyValue(key, knex);
  bool fgsss = false; 
  if (!knex && (ssegsz.length() > 0))  { segsz = atoi(ssegsz.c_str());  fgsss = true; }
  if (segsz < 16) segsz = 16;

  // Nb de lignes et de colonnes 
  vector<string> colnames;
  vector<int> coltypes;
  vector<LONGLONG> repcnt, width;
  vector<sa_size_t> colpos;
  is.GetColInfo(colnames, coltypes, repcnt, width);
  int_8 nbrows = is.GetNbRows();

  if (dobj == NULL) { // On cree la table si besoin 
    /*
      Reza, Sep06 : On ne cree pas automatiquement un SwFitsDataTable - 
      En effet, on ne peut garantir que l'objet FitsInOutFile is reste 
      valide pendant toute la duree de vie de l'objet SwFitsDataTable

    string key = "SOPCLSNM"; 
    string clsnm = is.KeyValue(key);
        if ( (clsnm == "SOPHYA::SwFitsDataTable") || (nbrows*colnames.size() < 10000000) )
      dobj = new DataTable(segsz);
    else { 
      dobj = new SwFitsDataTable(segsz);
      // dobj = new SwFitsDataTable(is, segsz, false);
      // Reza, Sep2006: Ce constructeur de SwFitsDataTable lit l'entete 
      // et initialise donc l'objet SwFitsDataTable  -> return
      // return;
    }
    */
    dobj = new DataTable(segsz);
  }
  else { 
    dobj->Clear();  // On efface la table sinon 
    if (fgsss)  dobj->mSegSz = segsz;
  }

  // Type reel de table
  DataTable* dt = dynamic_cast< DataTable *> (dobj);
  SwFitsDataTable* swfdt = dynamic_cast< SwFitsDataTable *> (dobj);

  vector<int_8> swpos;
  segsz = dobj->mSegSz;
  int_8 swp = 1;
  while (swp < nbrows) {
    swpos.push_back(swp); swp += segsz;
  }
  // Initialize the fits swap stream for SwFitsDataTable if necessary
  if (swfdt)  {
    if ( swfdt->FitsSwapFile().FitsPtr() != is.FitsPtr() ) {
      //      swfdt->FitsSwapFile().Close(); 
      //      swfdt->FitsSwapFile().Open(is.FileName().c_str(), FitsInOutFile::Fits_RO);
      //      swfdt->FitsSwapFile().MoveAbsToHDU(is.CurrentHDU());
      swfdt->FitsSwapFile().ShareFitsPtr(is); 
    }
  }

  // --ATTENTION-- repcnt (contenu=vecteur) pris en compte pour les colonnes autre que string 
  for(sa_size_t k=0; k<colnames.size(); k++) { 
    string ocolnm = colnames[k];
    if (dobj->CheckCorrectColName(colnames[k]) ) 
      cout << "FitsHandler<BaseDataTable>::Read()/Warning Column name changed for col[" 
	   << k << "] " << ocolnm << " -> " << colnames[k] << endl;

    switch ( coltypes[k] ) {
    case TBYTE :
    case TSHORT :
    case TUSHORT :
    case TINT :
    case TINT32BIT :
      if (swfdt) 
	swfdt->AddColRd(BaseDataTable::IntegerField, colnames[k], k+1, &swpos, repcnt[k]);
      else dobj->AddIntegerColumn(colnames[k], repcnt[k]);
      colpos.push_back(k+1);
      break;
    case TUINT:
      //  on ne peut pas garder TLONG, car meme valeur que TINT32BIT    case TLONG :
    case TULONG :
#ifdef TLONGLONG
    case TLONGLONG :
#endif 
      if (swfdt)       
	swfdt->AddColRd(BaseDataTable::LongField, colnames[k], k+1, &swpos, repcnt[k]);     
      else dobj->AddLongColumn(colnames[k], repcnt[k]);
      colpos.push_back(k+1);
      break;
    case TFLOAT :
      if (swfdt)       
	swfdt->AddColRd(BaseDataTable::FloatField, colnames[k], k+1, &swpos, repcnt[k]);     
      else dobj->AddFloatColumn(colnames[k], repcnt[k]);
      colpos.push_back(k+1);
      break;
    case TDOUBLE :
      if (swfdt)       
	swfdt->AddColRd(BaseDataTable::DoubleField, colnames[k], k+1, &swpos, repcnt[k]);     
      else dobj->AddDoubleColumn(colnames[k], repcnt[k]);
      colpos.push_back(k+1);
      break;
    case TCOMPLEX :
      if (swfdt)       
	swfdt->AddColRd(BaseDataTable::ComplexField, colnames[k], k+1, &swpos, repcnt[k]);     
      else dobj->AddComplexColumn(colnames[k], repcnt[k]);
      colpos.push_back(k+1);
      break;
    case TDBLCOMPLEX :
      if (swfdt)       
	swfdt->AddColRd(BaseDataTable::DoubleComplexField, colnames[k], k+1, &swpos, repcnt[k]);     
      else dobj->AddDoubleComplexColumn(colnames[k], repcnt[k]);
      colpos.push_back(k+1);
      break;
    case TSTRING :
      if (repcnt[k]<=16) {
	if (swfdt)       
	  swfdt->AddColRd(BaseDataTable::FMLStr16Field, colnames[k], k+1, &swpos, 1);     
	else dobj->AddString16Column(colnames[k], 1);
      }
      else if (repcnt[k]<=64) {
	if (swfdt)       
	  swfdt->AddColRd(BaseDataTable::FMLStr64Field, colnames[k], k+1, &swpos, 1);     
	else dobj->AddString64Column(colnames[k], 1);
      }
      else {
      if (swfdt)       
	swfdt->AddColRd(BaseDataTable::StringField, colnames[k], k+1, &swpos, repcnt[k]);     
      else dobj->AddStringColumn(colnames[k], repcnt[k]);
      }
      colpos.push_back(k+1);
      break;
    default:
      cout << "FitsHandler<BaseDataTable>::Read() NOT handled field type " 
	   << coltypes[k] << endl;
      break;
    }
  }

  // ------- Mise a jour des champs Nb d'entrees, nb segments ...
  dobj->mNEnt = is.GetNbRows();
  // Lecture DVList  (=entete FITS) 
  is.GetHeaderRecords(dobj->Info());
  // Lecture des unites de colonnes
  for(sa_size_t k=0; k<dobj->NVar(); k++) {
    char keynb[32];
    sprintf(keynb,"SOPUN%d",(int)colpos[k]);
    string key=keynb;
    bool nosk=false;
    string sunit=is.KeyValue(key,nosk);
    if (nosk) continue;
    Units un(sunit);
    dobj->SetUnits(k,un);
  }

  // On ne doit pas lire les donnees de la table pour un SwFitsDataTable
  if (swfdt) {
    swfdt->mNSeg = swpos.size();
    return;
  }

  while ((dobj->SegmentSize()*dobj->NbSegments()) < dobj->NEntry())
    dobj->Extend();
  //DBG  cout << " DBG2 -- dobj->mNSeg = " << dobj->NbSegments() << " SegSize=" 
  //DBG   << dobj->SegmentSize() << " NEntry=" << dobj->NEntry() << endl;

  // Lecture des donnees des colonnes
  for(sa_size_t l=0; l<dobj->NEntry(); l+=dobj->SegmentSize()) { 
    sa_size_t icol = 0;
    sa_size_t sszt = dobj->SegmentSize();
    sa_size_t iseg = l/sszt;
    if ((l+sszt) > dobj->NEntry()) sszt = dobj->NEntry()-l;
    for(sa_size_t k=0; k<dobj->NVar(); k++) { 
      sa_size_t sk = dobj->mNames[k].ser;
      sa_size_t sz = sszt*dobj->GetColumnVecSize(k); 
      icol = colpos[k];
      //      cout << " DBG-3 , k=" << k << " icol=" << icol << endl;
      switch ( dobj->GetColumnType(k) ) {
      case BaseDataTable::IntegerField :
	FitsBlockRW<int_4>::ReadColumnData(is, icol, l+1, 1, 
					   dobj->mIColsP[sk]->GetSegment(iseg), sz);
	break;
      case BaseDataTable::LongField :
	FitsBlockRW<int_8>::ReadColumnData(is, icol, l+1, 1, 
					   dobj->mLColsP[sk]->GetSegment(iseg), sz);
	break;
      case BaseDataTable::FloatField :
	FitsBlockRW<r_4>::ReadColumnData(is, icol, l+1, 1, 
					 dobj->mFColsP[sk]->GetSegment(iseg), sz);
	break;
      case BaseDataTable::DoubleField :
	FitsBlockRW<r_8>::ReadColumnData(is, icol, l+1, 1, 
					 dobj->mDColsP[sk]->GetSegment(iseg), sz);
	break;
      case BaseDataTable::ComplexField :
	FitsBlockRW< complex<r_4> >::ReadColumnData(is, icol, l+1, 1, 
						    dobj->mYColsP[sk]->GetSegment(iseg), sz);
	break;
      case BaseDataTable::DoubleComplexField :
	FitsBlockRW< complex<r_8> >::ReadColumnData(is, icol, l+1, 1, 
						    dobj->mZColsP[sk]->GetSegment(iseg), sz);
	break;
      case BaseDataTable::StringField :
	FitsBlockRW<std::string>::ReadColumnData(is, icol, l+1, 1, 
						 dobj->mSColsP[sk]->GetSegment(iseg), sz);
	break;
      case BaseDataTable::DateTimeField :
	FitsBlockRW<TimeStamp>::ReadColumnData(is, icol, l+1, 1, 
					       dobj->mTColsP[sk]->GetSegment(iseg), sz);
	break;
      case BaseDataTable::FMLStr16Field :
	FitsBlockRW<String16>::ReadColumnData(is, icol, l+1, 1, 
					      dobj->mS16ColsP[sk]->GetSegment(iseg), sz);
	break;
      case BaseDataTable::FMLStr64Field :
	FitsBlockRW<String64>::ReadColumnData(is, icol, l+1, 1, 
					      dobj->mS64ColsP[sk]->GetSegment(iseg), sz);
	break;
      default:
	break;
      }
    }
  }

}


} // FIN namespace SOPHYA 


