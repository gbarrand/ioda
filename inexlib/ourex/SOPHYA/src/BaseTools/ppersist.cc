#include "sopnamsp.h"
#include "machdefs.h"
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include "pexceptions.h"
#include "ppersist.h"
#include "anydataobj.h"
#include <iostream>
#include <iomanip>
#include <typeinfo>

// ------------------------- Historique ---------------------------
// Le code de ppersist a ete separe en deux en Decembre 2003
// a partir de la version CVS (1.26 - ppersist.cc) et 
// (1.19 - ppersist.h) .  
// la partie sur l'ecriture des donnees de base et leurs tableaux
// a ete mis dans les fichiers ppfbinstream.h .cc 
// (Classes PPFBinaryInputStream et PPFBinaryOutputStream
// -----------------------------------------------------------------



/*!
   \class SOPHYA::PIOPersist
   \ingroup BaseTools
   Handles (statically) the registration of PPF Sophya Persistence) handler classes. 
   \sa PPersist
*/


//++
// Class	PIOPersist
// Lib		Outils++
// include	ppersist.h
//
//	Root class for persistant files. Handles the registration of
//	persistant classes
//--

MD5_CONTEXT PIOPersist::ctx;
PIOPersist::ClassList * PIOPersist::ppclassList = NULL;   // $CHECK$ Reza 26/04/99
map<string, uint_8> * PIOPersist::ppclassNameList = NULL;
map<string, uint_8> * PIOPersist::dobjclassNameList = NULL;

//++
void
PIOPersist::Initialize()
//  Initialisation globale (objets statiques) $CHECK$ Reza 26/04/99
//--
{
ppclassList = new PIOPersist::ClassList;
ppclassNameList = new map<string, uint_8>;
dobjclassNameList = new map<string, uint_8>;
cout << " PIOPersist::Initialize() Starting Sophya Persistence management service " << endl;
}

/*!
  Register a new persistence handler (PPersist) class.
  The classId is usually a hash of the class< name, and
  ppclass_name is typeid(PPersistClass).name() .
  This method is called only through the PPersistRegistrar template
*/
void
PIOPersist::RegisterPPHandlerClass(uint_8 classId, string ppclass_name, ClassCreatorFunc f)
{
  if (ppclassList->size() && (ppclassList->find(classId) != ppclassList->end()) ) {
    cerr << "RegisterClass : Error, " << hex << classId << dec 
	 << " already registered." << endl;
    throw(DuplicateIdExc("PIOPersist::RegisterPPHandlerClass() Already registered (1)"));
  }
  if (ppclassNameList->size() && (ppclassNameList->find(ppclass_name) != ppclassNameList->end())) {
    cerr << "RegisterClass : Error (2) " <<  ppclass_name 
	 << " already registered." << endl;
      throw(DuplicateIdExc("PIOPersist::RegisterPPHandlerClass() Already registered(2)"));
    }
  
  (*ppclassList)[classId] = f;
  (*ppclassNameList)[ppclass_name]  = classId;
}

/*!
  Register a new DataObj class corresponding to a PPersist classId
  class_typename should be typeid(DataObject).name() 
*/
void
PIOPersist::RegisterDataObjClass(uint_8 classId, string class_name)
{
  if (ppclassList->find(classId) == ppclassList->end() ) {
    cerr << "PIOPersist::RegisterDataObjClass() Error (1) " 
	 << hex << classId << dec << " Not Found !" << endl;
    throw( NotFoundExc("PIOPersist::RegisterDataObjClass() Not found classId ") );
  }
  if (dobjclassNameList->size() && (dobjclassNameList->find(class_name) != dobjclassNameList->end())) {
    cerr << "PIOPersist::RegisterDataObjClass() Error (2)" << class_name 
	 << " already registered." << endl;
      throw(DuplicateIdExc("PIOPersist::RegisterDataObjClass() - Already registered"));
  }

  (*dobjclassNameList)[class_name]  = classId;
}

//	class_typename should be typeid(DataObject).name(), to be 
//	used by POutPersist::PutDataObject() methods.

//! Lists the registered PPersist handler classes and the corresponding class id.
void
PIOPersist::ListPPHandlers()
{
  cout << " PIOPersist::ListPPHandlers() - List of registered PPersist handler classes " << endl;
  map<string, uint_8>::iterator it;
  int k = 0;
  for (it = (*ppclassNameList).begin(); it != (*ppclassNameList).end(); it++) {
    cout << ++k << ":   " << (*it).first << "\n" << "          ClassId: Hex=  " 
	 << hex << (*it).second << "  (Dec= " << dec << (*it).second << ")" << endl;   
  }
}

//! Lists the registered DataObj classes with the corresponding PPHandler name
void
PIOPersist::ListDataObjClasses()
{
  cout << " PIOPersist::ListDataObjClasses() : Registered DataObj class list " << endl;
  map<string, uint_8>::iterator it;
  int k = 0;
  for (it = (*dobjclassNameList).begin(); it != (*dobjclassNameList).end(); it++) {
    cout << ++k << "- "  << (*it).first << "  ->  " << getPPClassName((*it).second) << endl; 
  }
}

//! Returns the PPersist class creator function for the specified classId
PIOPersist::ClassCreatorFunc
PIOPersist::FindCreatorFunc(uint_8 classId)
{
  ClassList::iterator i = ppclassList->find(classId);
  if (i == ppclassList->end()) throw(NotFoundExc("PIOPersist::FindCreatorFunc() Not found classId"));
  return (*i).second;
}

//! Returns the PPersist class name for the specified classId
string 
PIOPersist::getPPClassName(uint_8 classId)
{
  map<string, uint_8>::iterator i;
  for (i= ppclassNameList->begin(); i != ppclassNameList->end(); i++)
    if ( (*i).second == classId ) return (*i).first;

  throw(NotFoundExc("PIOPersist::getPPClassName() Not found classId"));
}

//! Returns the classId for the specified PPersist class type name
uint_8
PIOPersist::getPPClassId(string const & typ_name)
{
  map<string, uint_8>::iterator i = ppclassNameList->find(typ_name);
  if (i == ppclassNameList->end()) 
    throw(NotFoundExc("PIOPersist::getPPClassId() Not found className"));
  return (*i).second;
}

//! Returns the classId for the specified PPersist class 
uint_8
PIOPersist::getPPClassId(PPersist const & ppo)
{
  string typ_name = typeid(ppo).name() ;
  return (getPPClassId(typ_name) );
}

//! Return true if the specified PPersist class is found in the registered class list
bool
PIOPersist::checkPPClassId(PPersist const & ppo)
{
  string typ_name = typeid(ppo).name() ;
  map<string, uint_8>::iterator i = ppclassNameList->find(typ_name);
  if (i == ppclassNameList->end())   return false;
  return true;
}

//! Returns the PPersist class name for the specified classId
string 
PIOPersist::getDataObjClassName(uint_8 classId)
{
  map<string, uint_8>::iterator i;
  for (i= dobjclassNameList->begin(); i != dobjclassNameList->end(); i++)
    if ( (*i).second == classId ) return (*i).first;

  throw(NotFoundExc("PIOPersist::getDataObjClassName() Not found classId"));
}

//! Returns the classId for the specified PPersist class type name
uint_8
PIOPersist::getDataObjClassId(string const & typ_name)
{
  map<string, uint_8>::iterator i = dobjclassNameList->find(typ_name);
  if (i == dobjclassNameList->end()) 
    throw(NotFoundExc("PIOPersist::getDataObjClassId() Not found className"));
  return (*i).second;
}

//! Returns the classId for the specified PPersist class 
uint_8
PIOPersist::getDataObjClassId(AnyDataObj const & o)
{
  string typ_name = typeid(o).name() ;
  return (getDataObjClassId(typ_name) );
}

//! \cond
static inline void bswap8_hash(void* p)
{
  uint_8 tmp = *(uint_8*)p;
  *(uint_8*)p = ((tmp >> (7*8)) & 0x000000FF) | 
                ((tmp >> (5*8)) & 0x0000FF00) |
                ((tmp >> (3*8)) & 0x00FF0000) |
                ((tmp >> (1*8)) & 0xFF000000) |
                ((tmp & 0xFF000000) << (1*8))  |
                ((tmp & 0x00FF0000) << (3*8))  |
                ((tmp & 0x0000FF00) << (5*8))  |
                ((tmp & 0x000000FF) << (7*8));
}

//! \endcond

uint_8 PIOPersist::Hash(string const& typname) { 
        md5_init(&ctx); 
        md5_write(&ctx, (unsigned char*) typname.c_str(), typname.size()); 
        md5_final(&ctx); 
	uint_8 hash1 =   *((uint_8*) ctx.buf);
	uint_8 hash2 =   *((uint_8*) (ctx.buf+8));
#if IS_BIG_ENDIAN
	bswap8_hash(&hash1);
	bswap8_hash(&hash2);
#endif
	
        return (hash1+hash2);
}


//++
// Class	PPersist
// Lib		Outils++
// include	ppersist.h
//
//	Classe de base pour des objets persistants. Pour créer un objet
//	persistant :
//	- Hériter de PPersist.
//	- Implémenter "WriteSelf" et "ReadSelf", qui doivent écrire toutes les variables
//	  membres que l'on souhaite écrire, et les relire dans le même ordre.
//	  Pour écrire une référence à un objet : l'objet doit être un PPersist,
//	  et il suffit d'appeler "Write" sur cet objet, et "PPersistMgr::ReadObject".
//	  Si plusieurs objets font référence au même, pour éviter de l'écrire plusieurs
//	  fois, il faut que cet objet soit un PShPersist.
//	- Pour que le fichier soit portable, écrire et lire les variables membres en utilisant
//	  les fonctions PutXX/GetXX de PInPersist/POutPersist.
//
//	Attention: les méthodes à redéfinir sont WriteSelf et ReadSelf, mais il ne faut jamais
//	les appeler directement. Seuls Write et Read peuvent être appelées par l'utilisateur.
//--

/*!
   \class SOPHYA::PPersist
   \ingroup BaseTools
   Ancestor (base) class for SOPHYA persistence (PPF) handlers. 
   In order to have provide a PPF handler for an object :
   - Inherit from PPersist
   - Implement WriteSelf() and ReadSelf()

   \sa PInPersist POutPersist PIOPersist
*/

//! Writes the object into a new PPF file named \b fn 
void
PPersist::Write(string const& fn) const
{
  POutPersist of(fn);
  Write(of);
}

//! Reads the object from a PInPersist stream created from file \b fn 
void
PPersist::Read(string const& fn) 
{
  PInPersist inf(fn);
  inf.SkipToNextObject();
  Read(inf);
}

//! Writes the object to the output PPF stream \b s
void
PPersist::Write(POutPersist& s) const
{
   s.PutPPObject(this);
}


//! Reads the object from the input PPF stream \b s
void
PPersist::Read(PInPersist& s)
//
//	Relit l'objet dans le fichier ppersist. Il faut connaître a priori
//	le type de l'objet. Pour une relecture avec création automatique du bon
//	objet, utiliser PInPersist::ReadObject.
//  Il faut qu'on soit un objet ecrit 
//--
{
  // We should be the exact type  
  // Check tag value
  unsigned char ppstype,ppstag;
  s.GetTypeTag(ppstype);
  if ( (ppstype != PInPersist::PPS_OBJECT) && ( ppstype != PInPersist::PPS_REFERENCE ) ) {
  }
  if (ppstype == PInPersist::PPS_OBJECT) {
    // Check class id
    uint_8 classId;
    s.GetRawU8(classId);
    uint_8 oid,oid2;
    s.GetRawU8(oid);
    if (classId != PIOPersist::getPPClassId(*this) )
      throw FileFormatExc("PPersist::Read (): not the same object type");  
    ReadSelf(s);
    // Read the ENDOBJECT 
    s.GetRawUByte(ppstag);
    if (ppstag != PInPersist::PPS_ENDOBJECT)
      throw FileFormatExc("PPersist::Read() No PPS_ENDOBJECT tag");
    s.GetRawU8(oid2);
    if (oid2 != oid)
      throw FileFormatExc("PPersist::Read() Inconsistent PPS-OId at PPS_ENDOBJECT ");
    s.KeepOId(oid, *this);   // Object should be kept with its PPS_OId  (if oid > 0)
  }
  else if ( ppstype == PInPersist::PPS_REFERENCE ) 
    s.ReadReference(*this);

  else  throw FileFormatExc("PPersist::Read() : not an object in flow");

}

//! Writes a name tag (\b tag) and then the object to the output PPF stream \b s
void
PPersist::Write(POutPersist& s, string const& tag) const
{
  s.WriteNameTag(tag);
  s.PutPPObject(this);
}

//! Try to position at the name tag \b tag and then reads the object from the input stream \b s
void
PPersist::ReadAtTag(PInPersist& s, string const& tag)
{
  if (!s.GotoNameTag(tag))
    throw NotFoundExc("PPersist::ReadAtTag  tag not found");
  Read(s);
}

// Renvoie l'identificateur de l'objet  - par defaut=0
/*! Return the object identifier (default: return 0)
  (used for handling of PPF I/O for objects with reference sharing).
*/
uint_8 
PPersist::getMemOId() const
{
  return(0);
}

// Ces deux methodes doivent etre redefinies si   getMemOId() renvoie non nul (>0)
// ShareDataReference() et CloneSharedReference()
/*! 
  This method should be re-implemented if getMemOId() non zero value.
  (used for handling of PPF I/O for objects with reference sharing).
*/
void 
PPersist::ShareDataReference(PPersist & pcs)
{
  throw NotAvailableOperation("PPersist::ShareDataReference() - Unsupported operation !");
}

/*! 
  This method should be re-implemented if getMemOId() non zero value.
  (used for handling of PPF I/O for objects with reference sharing).
*/
PPersist *
PPersist::CloneSharedReference()
{
  throw NotAvailableOperation("PPersist::CloneSharedReference() - Unsupported operation !");
}

//++
//  virtual void PPersist::ReadSelf(PInPersist&)=0
//	Méthode virtuelle pure à redéfinir. Elle est appelée par Read
//	et PPersistMgr::ReadObject. Il faut relire les variables membres,
//	dans l'ordre où elles ont été écrites par WriteSelf.
//  virtual void PPersist::WriteSelf(POutPersist&) const=0
//	Méthode virtuelle pure à redéfinir. Elle est appelée par Write.
//	Il faut écrire les variables membres,
//	dans l'ordre où elles seront relues par ReadSelf.
//--



//++
// Class	PInPersist
// Lib		Outils++
// include	ppersist.h
//
//	Fichier d'objets persistants, en lecture.
//--

/*!
   \class SOPHYA::PInPersist
   \ingroup BaseTools
   This class implements the I/O read services necessary for SOPHYA
   persistence (PPF) by extending the services provided by PPFBinaryInputStream.
   \sa PPersist
*/

/*! Constructor from a RawInOutStream pointer
  \param is : pointer to RawInOutStream
  \param ad : if true, the RawInOutStream \b is is deleted by the destructor 
  \param scan : if true, try to read the name-tags table from the end of the input stream
*/ 
PInPersist::PInPersist(RawInOutStream * is, bool ad, bool scan)
  : PPFBinaryInputStream(is, ad, scan)
{
}

/*! Constructor 
  \param  flnm : input file name
  \param scan : if true, try to read the name-tags table from the end of the input stream
*/  
PInPersist::PInPersist(string const& flnm, bool scan)
  : PPFBinaryInputStream(flnm, scan)
{
}



PInPersist::~PInPersist()
{
  ObjList::iterator i;
  for(i=objList.begin(); i!= objList.end(); i++)  
    if ((*i).second)  delete (*i).second;
}



string
PInPersist::GetTagClassName(int itag)
{
  // A faire
//   if (itag<0 || itag >= (int)tags.size()) return "";
//   map<string, int_8>::iterator i = tags.begin();
//   for (int j=0; j<itag; j++) i++;
//   uint_8 cid = (*i).second;
//   return(GetClassName(cid));
  return("");
}

/*! Skip to next object in the streams, reads it and return 
  the corresponding PPersist handler 
*/
PPersist*
PInPersist::ReadObject()
{
  SkipToNextObject();
  return(GetPPObject());
}

/*! 
  Tries to read in from the stream into object \b o
*/
void 
PInPersist::GetObject(AnyDataObj & o)
{
  GetPPObject(&o);
  return;
}

/*! 
  Tries to move to \b nametag and then read in from the stream into object \b o
*/
void 
PInPersist::GetObject(AnyDataObj & o, string tagname)
{
  GotoNameTag(tagname);
  GetPPObject(&o);
  return;
}

/*! 
  Reading of objects - used by GetObject() and ReadObject()
*/
PPersist*
PInPersist::GetPPObject(AnyDataObj * po)
{
  // Get tag
  unsigned char ppstype;
  GetTypeTag(ppstype);
  if (ppstype != PPS_OBJECT && ppstype != PPS_REFERENCE && ppstype != PPS_NULL) {
    throw FileFormatExc("PInPersist::ReadObject : not an object in flow");
  }

  if (ppstype == PPS_NULL) {
    return NULL;
  } else if (ppstype == PPS_OBJECT) {
    // Get class id
    uint_8 classId;
    GetRawU8(classId);
    uint_8 oid,oid2;
    GetRawU8(oid);
    
    // Get factory method
    ClassCreatorFunc f = FindCreatorFunc(classId);
    if (!f) {
      throw NotFoundExc("PInPersist::ReadObject   class not registered");
    }
    
    // Create object
    PPersist* object = f();
    //  If a DataObject was specified , we assign it to the PPersistObject
    if (po != NULL) object->SetDataObj(*po);

    object->ReadSelf(*this);
    unsigned char ppstag;
    // Read the ENDOBJECT 
    GetRawUByte(ppstag);
    if (ppstag != PPS_ENDOBJECT)
      throw FileFormatExc("PInPersist::ReadObject No PPS_ENDOBJECT tag");
    GetRawU8(oid2);
    if (oid2 != oid)
      throw FileFormatExc("PInPersist::ReadObject Inconsistent PPS-OId at PPS_ENDOBJECT ");

    KeepOId(oid, *object);
    return object;
  } 
  else if (ppstype == PPS_REFERENCE) {
    PPersist* ppr = ReadReference();
    PPersist* pprc = ppr->CloneSharedReference();
    if (po) {  // We have to read it into an object 
      pprc->SetDataObj(*po);
      pprc->ShareDataReference(*ppr);  // On met a jour la reference de pprc
    }
    return pprc;
  }
  else throw FileFormatExc("PInPersist::ReadObject invalide Tag Type !");  
}



void
PInPersist::ReadReference(PPersist & ppo)
{
  PPersist * pr = ReadReference();
  ppo.ShareDataReference(*pr);
}


PPersist *
PInPersist::ReadReference()
{
  uint_8 oid;
  int_8 pos;
  GetRawU8(oid);
  GetRawI8(pos);
  //  cerr << " DBG - PInPersist::ReadReference-A "  << oid << " Pos= " << pos << endl; 
  map<uint_8, PPersist *>::iterator i = objList.find(oid);
  if (i != objList.end()) return (*i).second; 
  else  {  // We may have skeeped it !
    // Let's try to read it
    int_8 cpos;
    cpos = s->tellg();
    s->seekg(pos);
    PPersist* ppo = ReadObject();
    s->seekg(cpos);
    delete ppo;
    //    cerr << " DBG - PInPersist::ReadReference-B ... " << endl;  

    map<uint_8, PPersist *>::iterator i2 = objList.find(oid);
    if (i2 == objList.end()) 
      throw FileFormatExc("PInPersist::ReadReference()   Not found PPS_OId ");
    return (*i2).second; 
  }
}


void
PInPersist::KeepOId(uint_8 oid, PPersist & ppo)
{
  if ((oid&0x1) == 0)  return; // This is not an object which can be referenced
  //  cerr << " DBG - PInPersist::KeepOId() " << oid << endl;
  if ((objList.size() > 0) && (objList.find(oid) != objList.end()) ) {
    //  Ceci ne devrait arriver que si on lit dans le desordre (avec GotoNameTag)
    //  et pas avec une lecture sequentielle ...   Reza 03/2000
    //  cerr << "PInPersist::KeepOId()/Warning - already present PPS_ObjectId ! " << oid << endl;
    if (seqread) throw FileFormatExc("PInPersist::KeepOId() already present PPS_ObjectId ");
    PPersist *pp = (*objList.find(oid)).second;
    ppo.ShareDataReference(*pp);  // On met a jour la reference des donnees de ppo
  }
  else {
    PPersist * npp = ppo.CloneSharedReference(); 
    if (npp == NULL) throw PError("PInPersist::KeepOId() NULL returned by PPersist.Clone() ! ");
    objList[oid] = npp;
  }
  return;
}

//++
// Class	POutPersist
// Lib		Outils++
// include	ppersist.h
//
//	Fichier d'objets persistants, en écriture.
//--


/*!
   \class SOPHYA::POutPersist
   \ingroup BaseTools
   This class implements the I/O write services necessary for SOPHYA
   persistence (PPF) by extending the services provided by PPFBinaryOutputStream.
   \sa PPersist
*/

//++
//  POutPersist(string const& flnm, int endianness = PPersist::PPS_NATIVE)
//
//	Crée un nouveau fichier ppersist. Par défaut, il est petit=boutien
//	sur machines petit-boutiennes, et gros-boutien sur machines
//	gros-boutiennes. On peut explicitement spécifier PPersist::PPS_LITTLE_ENDIAN
//	ou PPersist::PPS_BIG_ENDIAN.
//--

/*! Constructor from a RawInOutStream pointer
  \param os : pointer to RawInOutStream
  \param ad : if true, the RawInOutStream \b os is deleted by the destructor 
  \param endianness : Endianness selector  PPS_NATIVE PPS_LITTLE_ENDIAN PPS_BIG_ENDIAN 
*/ 
POutPersist::POutPersist(RawInOutStream* os, bool ad, int endianness)
  : PPFBinaryOutputStream(os, ad, endianness)
{
  pps_OId = 0;
  wobj_level = 0;
}

/*! Constructor 
  \param flnm : output file name 
  \param endianness : Endianness selector  PPS_NATIVE PPS_LITTLE_ENDIAN PPS_BIG_ENDIAN 
*/ 
POutPersist::POutPersist(string const& flnm, int endianness)
  : PPFBinaryOutputStream(flnm, endianness)
{
  // PPS (POutPersist stream) Object Id initialisation
  pps_OId = 0;
  wobj_level = 0;
}

POutPersist::~POutPersist()
{
}


//! Writes the object \b o to the stream
void
POutPersist::PutObject(AnyDataObj & o)
{
  ClassCreatorFunc f = FindCreatorFunc(getDataObjClassId(o));
  if (!f) 
      throw NotFoundExc("POutPersist::PutObject()   class not registered");
  PPersist* ppo = f();
  ppo->SetDataObj(o);
  PutPPObject(ppo);
}

//! Writes the nametag \b tagname and then the object \b o to the stream
void
POutPersist::PutObject(AnyDataObj & o, string tagname)
{
  WriteNameTag(tagname);
  PutObject(o);
}


void
POutPersist::PutPPObject(PPersist const* obj) 
{
  if (serializeNullAndRepeat(obj)) return;  // NULL object or already written in stream

  //   We have to write the object 
  uint_8 oid = assignObjectId(obj);       // We assing a PPS Object Id 
  PutRawUByte(PPS_OBJECT);         // We write the Object Tag
  wobj_level++;  // Niveau d'imbrication d'ecriture d'objets
  PutRawU8(getPPClassId(*obj));    // Writing the PPersist ClassId
  PutRawU8(oid);                   // Write the PPS Object Id 
  obj->WriteSelf(*this);
  //  Comptage d'objets ecrits
  _nbobjs++;
  if (wobj_level > _maxnestlevel)  _maxnestlevel = wobj_level;
  if (wobj_level == 1) _nbtlobjs++;
  wobj_level--;  
  PutRawUByte(PPS_ENDOBJECT);      // We write the End-Of-Object Tag
  PutRawU8(oid);                   // and again its PPS Object Id 
}

bool
POutPersist::serializeNullAndRepeat(PPersist const* x)
{
  if (x == NULL) {
    PutRawUByte(PPS_NULL);
    return true;
  }

  int_8 pos;
  uint_8 id = findObjectId(x, pos);
  if (id > 0) {
    PutRawUByte(PPS_REFERENCE);
    PutRawU8(id);      // Writing the corresponding object Id
    PutRawI8(pos);     // The original object position 
    _nbrefs++;         // Compteur de nombre de reference ecrits
    return true;
  }
  
  return false;  // Object have to be written in stream ...
}

uint_8 
POutPersist::assignObjectId(PPersist const* x)
{
  pps_OId += 16;  // We keep the three first bytes for future usage
                  // Bit 1 non zero -> Object can be referenced 
  uint_8 id = pps_OId;
  uint_8 mid = x->getMemOId();
  if (mid > 0) {
    int_8 pos;
    if (findObjectId(x,pos) > 0)  
      throw PError("POutPersist::assignObjectId() Error - Already serialized object ! ");
    id += 1;  // Bit 1 non zero -> Object can be referenced 
    objreftag rt;
    rt.ppsoid = id;
    //    cout << " DBG-rt.ppspos = s->tellp(); " << endl;
    rt.ppspos = s->tellp();
    // cout << " DBG-rt.ppspos = s->tellp(); = " << rt.ppspos << endl;
    objList[mid] = rt;
  }
  return id;
}

uint_8 
POutPersist::findObjectId(PPersist const* x, int_8 & pos)
{
  pos = -1;
  uint_8 mid = x->getMemOId();
  if (mid == 0)   return(0);
  ObjList::iterator i = objList.find(mid);
  if (i == objList.end()) return 0;
  pos = (*i).second.ppspos;
  return (*i).second.ppsoid;
}


