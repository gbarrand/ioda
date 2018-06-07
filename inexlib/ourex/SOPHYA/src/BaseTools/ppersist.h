// This may look like C code, but it is really -*- C++ -*-

#ifndef PPERSIST_H_SEEN
#define PPERSIST_H_SEEN

// Flat file persistance, similar to Java serialization
//
// E. Aubourg     CEA DAPNIA/SPP  1999
// R. Ansari      LAL IN2P3/CNRS  03/2000

// -------------------- Historique -----------------------
//  Separe en deux (CVS version 1.19 - ppersist.h)  
//  Classes PPFBinaryIn/OutputStream ds ppfbinstream.h
// -------------------------------------------------------


#include "machdefs.h"
#include "pexceptions.h"
#include "gnumd5.h"
#include "ppfbinstream.h"

#include <string>
#include <map>
#include <vector>
#include <typeinfo>


// Classe de base pour les objets qui peuvent devenir PPersist

namespace SOPHYA {
  
  class AnyDataObj;

  class PIOPersist;
  class PInPersist;
  class POutPersist;
  class PPersist;

//! Persistent (delegate or mixin) base class 
  class PPersist  {
  public:
    virtual           ~PPersist() {}

    void               Write(string const& fn) const;
    void               Read(string const& fn);

    virtual void       Write(POutPersist&) const;
    virtual void       Read(PInPersist& s);               // Reads the type tag and the object
    void               Write(POutPersist&, string const& tag) const; 
    void               ReadAtTag(PInPersist& s, string const& tag);

    virtual AnyDataObj* DataObj()=0;       // Retourne l'objet reel 
    virtual void       SetDataObj(AnyDataObj &)=0;     

    virtual uint_8     getMemOId() const ;      // Renvoie l'identificateur de l'objet  - par defaut=0
    // Ces deux methodes doivent etre redefinies si   getMemOId() renvoie non nul (>0)
    virtual void       ShareDataReference(PPersist &);       
    virtual PPersist*  CloneSharedReference();
                                                // doit etre surcharge pour renvoyer un mem-oid correct
  protected:  
    virtual void       ReadSelf(PInPersist&)=0;           
    virtual void       WriteSelf(POutPersist&) const=0;

    friend class       PInPersist;
    friend class       POutPersist;
  };



// Handles (statically) the registration of classes.

  class PIOPersist {
  public:
    typedef PPersist*            (*ClassCreatorFunc)();
  
    static void                  RegisterPPHandlerClass(uint_8 classId, string ppclass_name, ClassCreatorFunc f);
    static void                  RegisterDataObjClass(uint_8 classId, string class_name);

    static void			 ListPPHandlers();
    static void			 ListDataObjClasses();

    static ClassCreatorFunc      FindCreatorFunc(uint_8 classId);

    static string                getPPClassName(uint_8 classId);
    static uint_8                getPPClassId(string const & typ_name);
    static uint_8                getPPClassId(PPersist const & ppo);
    static bool                  checkPPClassId(PPersist const & ppo);

    static string                getDataObjClassName(uint_8 classId);
    static uint_8                getDataObjClassId(string const & typ_name);
    static uint_8                getDataObjClassId(AnyDataObj const & o);

    static uint_8 Hash(string const& typname);
    static MD5_CONTEXT ctx;
  
    static void Initialize();   // Pour initialiser classList


  private:

    typedef map<uint_8, ClassCreatorFunc, less<uint_8> > ClassList;  
    // Pas de createur appele pour objets statiques sur Linux - $CHECK$ Reza 26/04/99
    static ClassList *           ppclassList;      // PPersist class list  
    static map<string, uint_8> * ppclassNameList;  // PPersist classId = f(PPersistClassName)
    static map<string, uint_8> * dobjclassNameList;  // PPersist classId = f(DataObjClassName)

  };


// TBD : use hash tables instead of maps. Check hashtbl status in STL.

//! Input stream for PPersit objects.
  class PInPersist : public PPFBinaryInputStream, public PIOPersist  {
  public:
    PInPersist(RawInOutStream * is, bool ad, bool scan=false);
    PInPersist(string const& flnm, bool scan=true);
    virtual ~PInPersist();

    // A faire - Reza Dec 2003
    string     GetTagClassName(int itag);
    //   Object Reading 
    PPersist*  ReadObject();
    void       GetObject(AnyDataObj & o);
    void       GetObject(AnyDataObj & o, string  tagname);
    PPersist*  GetPPObject(AnyDataObj * po=NULL);

    //  Reza 03/2000
    //  Methodes qui pourraient etre protected, mais doivent etre utilisables par PPersist 
    void   ReadReference(PPersist & ppo);  // Fill the ppo object from the reference tag
    PPersist * ReadReference();            // Creates object from the reference tag
    void   KeepOId(uint_8 oid, PPersist & ppo);        // Keeps the ppo in the objList

  protected:

    // already read objects 
    typedef map<uint_8, PPersist * > ObjList;
    ObjList objList;
    friend class PPersist;
  };

//! Output stream for PPersit objects.
  class POutPersist : public PPFBinaryOutputStream, public PIOPersist  {
  public:
    POutPersist(RawInOutStream* os, bool ad, int endianness = PPS_NATIVE);
    POutPersist(string const& flnm, int endianness = PPS_NATIVE);
    virtual ~POutPersist();

    //    void   Put(PPersist const* x) {PutPPObject(x);}

    // Objet Write - Ecriture des objets
    void PutPPObject (PPersist const*); // Like doing Write(stream) on PPersist object

    void PutObject(AnyDataObj & o);   // Creates the corresponding PPersist Object and call Write()
    void PutObject(AnyDataObj & o, string tagname);


  protected:
    bool     serializeNullAndRepeat(PPersist const* x);
    uint_8   findObjectId(PPersist const* x, int_8 & pos);
    uint_8   assignObjectId(PPersist const* x);

    // objreftag contains the assigned POutStream Object Id and the stream position 
    // of the original written object
    typedef struct { uint_8 ppsoid; int_8 ppspos; } objreftag;
    // already serialized objects are kept in a map as a function of the Objects memory Id
    typedef map<uint_8, objreftag, less<uint_8> >  ObjList;
    ObjList objList;
    uint_8 pps_OId;    // PPS Object Id 
    int    wobj_level; // Niveau d'imbrication lors de l'ecriture d'objet
  };
  
//! \cond
// Le macro suivant permettent de simplifier la declaration 
// des operateurs >> << sur les POutPersist et  PInPersist
#define RAWPERSISTIO(_Type_,_xtyp_)                                             \
  inline POutPersist& operator << (POutPersist& c, _Type_ const& data)            \
  {                                                                               \
    c.Put##_xtyp_(data);                                                          \
    return c;                                                                     \
  }                                                                               \
                                                                                \
  inline PInPersist& operator >> (PInPersist& c, _Type_& data)                    \
  {                                                                               \
    c.Get##_xtyp_(data);                                                          \
    return c;                                                                     \
  }                                                                               

// On utilise le macro RAWPERSISTIO pour declarer POutPersist << et PInPersist >> 
// pour les types de base r_4 r_8 int_4 int_8 ...
  RAWPERSISTIO(int_8,I8);
  RAWPERSISTIO(uint_8,U8);
  RAWPERSISTIO(int_4,I4);
  RAWPERSISTIO(uint_4,U4);
  RAWPERSISTIO(int_2,I2);
  RAWPERSISTIO(uint_2,U2);
  RAWPERSISTIO(char,Byte);
  RAWPERSISTIO(r_4,R4);
  RAWPERSISTIO(r_8,R8);
  RAWPERSISTIO(complex<r_4>,Z4);
  RAWPERSISTIO(complex<r_8>,Z8);
  RAWPERSISTIO(string,Str);
#ifdef SO_LDBLE128
  RAWPERSISTIO(r_16,R16);
  RAWPERSISTIO(complex<r_16>,Z16);
#endif
  
#if 0
#define STRUCTPERSISTIO(_Type_, _field_, _size_)                                \
  inline POutPersist& operator << (POutPersist& c, _Type_ const& data)            \
  {                                                                               \
    c.PutBytes(&data._field_, _size_);                                            \
    return c;                                                                     \
  }                                                                               \
                                                                                \
  inline PInPersist& operator >> (PInPersist& c, _Type_& data)                    \
  {                                                                               \
    c.GetBytes(&data._field_, _size_);                                            \
    return c;                                                                     \
  }                                                                               

#endif

//! \endcond  

// --- Cela risque d'etre dangereux --- On le laisse au niveau des DataObj
//                           Reza 24/3/2000 
//   inline POutPersist& operator << (POutPersist& c, PPersist const& obj)
//     {
//       obj.Write(c);
//       return c;
//     }
  
//   inline PInPersist& operator >> (PInPersist& c, PPersist& obj)
//     {
//       obj.Read(c);
//       return c;
//     }

  // Utility class to
  //   - compute the class ID from a MD5 hash of the class name
  //   - register classes with PIOPersist, through PPRegister macro

//! template class for handling the PPersist registration mechanism.
  template <class T>
    class PPersistRegistrar {
    public:
      static PPersist* Create() {return new T();}
      static void Register(string id) { PIOPersist::RegisterPPHandlerClass(Hash(id), typeid(T).name(), Create); }
      static uint_8 Hash(string id) {
	return PIOPersist::Hash(id);
      }
    };
  
#define PPRegister(className) PPersistRegistrar< className >::Register( #className );
#define DObjRegister(ppclassName, className) PIOPersist::RegisterDataObjClass(PIOPersist::Hash(#ppclassName), typeid(className).name());

} // namespace SOPHYA

// La classe PPFNameTag facilite la manipulation des Nametag ds les fichiers
// PPersist - definie ds ppfnametag.h
#include "ppfnametag.h"


#endif
