#include "machdefs.h"
#include "sopnamsp.h"

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <typeinfo>

#include "fitsmanager.h"
#include "fitshandler.h"

struct hand_list_el {
  FitsHandlerInterface * fhi;
  int glev;
  string desc;
};

typedef list<hand_list_el> HandlerList;

static HandlerList * hlistp = NULL;

static inline void ChkHLP() 
{
  if (hlistp == NULL) hlistp = new HandlerList;
}
/*!
  Generic handlers (ImageHDU, TableHDU) will have a higher global priority level
  than specific handlers. Use glev=0 for specific handlers and glev=1 for generic
  handlers.  
  \param fhi : handler object pointer (created by new) which will be kept in the 
  handler list
  \param glev : global priority level associated with the handler. Used when different
  handlers return the same value for CheckHandling() or CheckReadability().
  \param desc : classe name description associated with the handler
*/
int FitsManager::RegisterHandler(FitsHandlerInterface * fhi, int glev, string desc)
{
  ChkHLP(); 
  if (fhi == NULL)
    throw NullPtrError("FitsManager::RegisterHandler() fhi=NULL ");
  HandlerList::iterator it;
  for(it = hlistp->begin(); it != hlistp->end(); it++) {
    if (typeid(*((*it).fhi)) == typeid(*fhi)) 
      throw DuplicateIdExc("FitsManager::RegisterHandler() Already registered handler");
  }
  hand_list_el hle;
  hle.fhi = fhi;
  hle.glev = glev;
  hle.desc = desc;
  hlistp->push_back(hle);
  return hlistp->size();
}

int FitsManager::ListHandlers()
{
  ChkHLP(); 
  int kk=0;
  cout << "---- FitsManager::ListHandlers()  NbHandlers= " << hlistp->size()
       << endl;
  HandlerList::iterator it;
  for(it = hlistp->begin(); it != hlistp->end(); it++) {
    kk++;
    cout << kk << "- " << (*it).desc << " : " 
	 << typeid(*((*it).fhi)).name() << " glev= " <<(*it).glev << endl;
  }
  return hlistp->size();
}

/*!
  Return a NULL pointer if no handler is registered for object \b o .
  The returned pointer should not be used directly and should NOT
  be deleted. For read/write operations, the Clone() method should
  be called on the returned object.
*/
FitsHandlerInterface* FitsManager::FindHandler(AnyDataObj & o)
{
  ChkHLP(); 
  FitsHandlerInterface * fhi = NULL;
  HandlerList::iterator it;
  int hfg = 0;
  int bhfg = 0;
  //unused: int clev = 0;
  int blev = 0;
  for(it = hlistp->begin(); it != hlistp->end(); it++) {
    hfg = (*it).fhi->CheckHandling(o);
    if (hfg < 1) continue;
    if ( ( hfg > bhfg ) || ( (hfg == bhfg) && ((*it).glev > blev) ) ) {
      fhi = (*it).fhi;  bhfg = hfg;  blev = (*it).glev;
    }
  }
  return fhi ;
}

/*!
  Throws an exception ( NotFoundExc ) if the write operation can not be performed. 
*/
void FitsManager::Write(FitsInOutFile& os, AnyDataObj & o)
{
  FitsHandlerInterface * fhi = FindHandler(o);
  if (fhi == NULL) {
    string msg = "FitsManager::Write()/FindHandler() Handler not found for ";
    msg += typeid(o).name();
    throw NotFoundExc(msg);
  }
  FitsHandlerInterface * fhi2 = fhi->Clone();
  fhi2->SetDataObj(o);
  fhi2->Write(os);
  delete fhi2 ;
  return;
}

/*!
  Throws an exception ( NotFoundExc ) if the read operation can not be performed. 
*/
void FitsManager::Read(FitsInOutFile& is, AnyDataObj & o)
{
  FitsHandlerInterface * fhi = FindHandler(o);
  if (fhi == NULL) {
    string msg = "FitsManager::Read()/FindHandler() Handler not found for ";
    msg += typeid(o).name();
    throw NotFoundExc(msg);
  }
  FitsHandlerInterface * fhi2 = fhi->Clone();
  fhi2->SetDataObj(o);
  fhi2->Read(is);
  delete fhi2;
  return;
}

/*!
  Return a NULL pointer if no handler is found capable of reading
  the current HDU.
  The returned pointer should not be used directly and should NOT
  be deleted. For read/write operations, the Clone() method should
  be called on the returned object.
*/
FitsHandlerInterface * FitsManager::FindReader(FitsInOutFile& is)
{
  ChkHLP(); 
  FitsHandlerInterface * fhi = NULL;
  HandlerList::iterator it;
  int hfg = 0;
  int bhfg = 0;
  int blev = 0;
  for(it = hlistp->begin(); it != hlistp->end(); it++) {
    hfg = (*it).fhi->CheckReadability(is);
    if (hfg < 1) continue;
    if ( ( hfg > bhfg ) || ( (hfg == bhfg) && ((*it).glev > blev) ) ) {
      fhi = (*it).fhi;  bhfg = hfg;  blev = (*it).glev;
    }
  }
  return fhi;
}

/*!
  Throws an exception ( NotFoundExc ) if the read operation can not be performed. 
  In case of success, the calling function is responsible for deleting 
  the returned FitsHandlerInterface object, when the correspondind data object
  is not needed any more.
*/
FitsHandlerInterface * FitsManager::Read(FitsInOutFile& is)
{
  FitsHandlerInterface * fhi = FindReader(is);
  if (fhi == NULL) {
    string msg = "FitsManager::Read()/FindReader() Reader/Handler not found  ";
    msg += is.FileName();
    char buff[64];
    sprintf(buff, " HDU= %d Type= %d", (int)(is.CurrentHDU()), 
	    (int)(is.CurrentHDUType()) );
    msg += buff;
    throw NotFoundExc(msg);
  }
  FitsHandlerInterface * fhi2 = fhi->Clone();
  fhi2->Read(is);
  return fhi2;
}

/*!
  \param filename : FITS file name to be scanned
  \param os : infomation will be sent to formatted stream os
  \param slev : scan level , bit 0 (1/3) print HDU keywords,
  bit 2 (2,3) try to read HDU data using the appropraite handler
  \param Rc : return number of scanned HDU's
 */
int FitsManager::ScanFile(string filename, ostream& os, int slev)
{
  FitsInOutFile is(filename, FitsInOutFile::Fits_RO);
  os << "=== FitsManager::ScanFile( " << filename << " ) NbHDUs= " 
     << is.NbHDUs() << endl;
  int rc = 0;
  for(int k=0; k<is.NbHDUs(); k++) {
    os << " ------ HDU No " << is.CurrentHDU() << " Type= " 
	 << is.CurrentHDUTypeStr() << endl;
    int hdutyp = is.CurrentHDUType();
    if (hdutyp == IMAGE_HDU) {
      LONGLONG naxes[5] = {0,0,0,0,0};
      int naxis=5;
      int imgtyp = is.GetImageHDUInfo(naxis, naxes);
      os << ">> IMAGE_HDU:  naxis= " << naxis << " : ";
      for(int i=0; i<naxis; i++) {
	if (i>0) os << " x " ;
	os << naxes[i];
      }
      os << endl;
    }
    else {
      vector<string> colnames;
      vector<int> coltypes;
      vector<LONGLONG> repcnt, width;
      int ncols = is.GetColInfo(colnames, coltypes, repcnt, width);
      if (hdutyp == BINARY_TBL) os << ">> BINARY_TBL :  NRows= " << is.GetNbRows();
      else os << ">> ASCII_TBL :  NRows= " << is.GetNbRows();
      os << " x NCols= " << ncols << endl;
      for(size_t kk=0; kk<colnames.size(); kk++) {
	os << "Col[" << kk+1 << "]  Name= " << colnames[kk] 
	     << " Type= " << FitsTypes::DataTypeToTypeString(coltypes[kk])
	     << " Repeat= " << repcnt[kk]
	     << " W= " << width[kk] << endl;
      }
    }
    // Fin the appropriate handler :
    ChkHLP(); 
    FitsHandlerInterface * fhi = NULL;
    HandlerList::iterator it;
    string hdesc;
    int hfg = 0;
    int bhfg = 0;
    int blev = 0;
    for(it = hlistp->begin(); it != hlistp->end(); it++) {
      hfg = (*it).fhi->CheckReadability(is);
      if (hfg < 1) continue;
      if ( ( hfg > bhfg ) || ( (hfg == bhfg) && ((*it).glev > blev) ) ) {
	fhi = (*it).fhi;  bhfg = hfg;  blev = (*it).glev; hdesc = (*it).desc; 
      }
    }
    if (fhi == NULL) 
      os << ">>> Warning : No handler found for this HDU ... " << endl;
    else 
      os << ">>> Reader/handler: " <<  hdesc << " : " 
	 << typeid(*fhi).name() << " HandLevel= " << blev << ", "  << bhfg << endl;
    if (fhi && (slev >= 2)) {
      os << ">>> Trying to read HDU data using the handler ..." << endl;
      FitsHandlerInterface* fhic = fhi->Clone();
      fhic->Read(is);
      os << " FitsHandler.Read() OK " << endl;
    }
    if ( (slev == 1) || (slev == 3) ) {
      os << ">>>> HDU keywords list :  " <<  endl;
      DVList dvl;
      is.GetHeaderRecords(dvl);
      os << dvl;
    }
    os << "               --------------------- " << endl;
    is.MoveToNextHDU();
    rc++;
  }
  os << "===================================================" << endl; 
  return rc;
}
