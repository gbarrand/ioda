// This may look like C code, but it is really -*- C++ -*-
// Classe pour la gestion de persistance PPF des SegDataBlock 
//                         R. Ansari - Avril 2005
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA

#ifndef FIOSEGDATABLOCK_H
#define FIOSEGDATABLOCK_H

#include "machdefs.h"
#include "ppersist.h"
#include "segdatablock.h"
#include "ppftpointerio.h"
#include <typeinfo> 

/*!
   \class SOPHYA::FIO_SegDataBlock
   \ingroup BaseTools
   Class implementing PPF persistence (PPersist handler) for segmented data structures
   (class SegDataBlock).
   \sa PPersist
*/

namespace SOPHYA {

   
template <class T>
class FIO_SegDataBlock : public  PPersist  {
public:
  FIO_SegDataBlock()
  {
    dobj = NULL ;
    ownobj=true;    
  }

  //   FIO_SegDataBlock(string const & filename);
  FIO_SegDataBlock(SegDataBlock<T> & obj)
  {
    dobj = &obj;
    ownobj = false;
  }

  FIO_SegDataBlock(SegDataBlock<T> * obj)
  {
    if (obj == NULL)  
      throw ParmError("FIO_SegDataBlock<T>::FIO_SegDataBlock(* obj) obj=NULL (ppfwrapstlv.h)");
    dobj = obj;
    ownobj = false;
  }
  virtual ~FIO_SegDataBlock()
  {
    if (ownobj && dobj)    delete dobj;
  }
  virtual AnyDataObj* DataObj() { return dobj; }
  virtual void        SetDataObj(AnyDataObj & o)
  {
    SegDataBlock<T> * po = dynamic_cast< SegDataBlock<T> * >(&o);
    if (po == NULL) {
      char buff[160];
      sprintf(buff,"FIO_SegDataBlock<%s>::SetDataObj(%s) - Object type  error ! ",
	      typeid(T).name(), typeid(o).name());
      throw TypeMismatchExc(PExcLongMessage(buff));    
    }
    if (ownobj && dobj) delete dobj;
    dobj = po; ownobj = false;
  } 

  inline operator SegDataBlock<T>()  { return(*dobj); }

  // Les methodes suivantes implementent les fonctionalites necessaires
  // au  partage de reference ds les fichiers PPF 
  uint_8 getMemOId() const
  {
    return ( (dobj) ? dobj->DRefId() : 0 );
  }
  void ShareDataReference(PPersist & pp)
  {
    FIO_SegDataBlock<T> *ppo = dynamic_cast< FIO_SegDataBlock<T> * >(&pp);
    if (ppo == NULL) throw TypeMismatchExc("FIO_SegDataBlock<T>::ShareDataReference() - Type Mismatch Error");
    if (ppo->dobj) {
      if (dobj) dobj->Share(*(ppo->dobj));
      else { dobj = new SegDataBlock<T>(*(ppo->dobj)); ownobj = true; } 
    }
  }
  PPersist* CloneSharedReference()
  {
    FIO_SegDataBlock<T> * ppo = new FIO_SegDataBlock<T>;
    ppo->dobj = new SegDataBlock<T>(*(dobj));
    ppo->ownobj = true;
    return(ppo); 
  }

protected :
  virtual void       ReadSelf(PInPersist& is)
  {
    // On lit les 4 premiers uint_8
    // 0: Numero de version (16 bits, 0..15) + bits d'etat (48 bits 16..63)  
    // 1: SegmentSize  2: NbSegments  3: NbItems
    uint_8 itab[4];
    is.Get(itab, 4);
    if (dobj == NULL) {
      dobj = new SegDataBlock<T>(itab[1], itab[2], itab[3]);
      ownobj = true;
    }
    else dobj->SetSize(itab[1], itab[2], itab[3]);
    // lecture de l'unite si applicable 
    Units un;
    bool sdone=false;
    if (((itab[0]&0xFFFF)>1)&&(itab[0]&(1<<16)))  { is >> un;  sdone=true; }
    if (sdone) dobj->SetUnits(un);
    // On lit les donnees par paquet ...
    for(uint_8 k=0; k<itab[2]; k++) 
      PPF_TPointer_IO<T>::Read(is, dobj->GetSegment(k), itab[1]);
    // On lit la table des tags de positionnement ...
    vector<int_8> postags;
    is.GetPosTagTable(postags);
  }         
  virtual void       WriteSelf(POutPersist& os) const
  {
    if (dobj == NULL) 
      throw ParmError("FIO_SegDataBlock<T>::WriteSelf() dobj=NULL (fiosegdb.h)");
    // On ecrit les 4  uint_8
    // 0: Numero de version (16 bits 0..15) + bits d'etat (48 bits 16..63)  
    // 1: SegmentSize  2: NbSegments  3: NbItems
    //  Numero de version 2 en Novembre 2012, apres ajout Units mSRef->unit_p
    //      Bit 16 de itab[0] a 1 si  mSRef->unit_p non nul 
    uint_8 itab[4];
    itab[0] = 2;
    itab[1] = dobj->SegmentSize();
    itab[2] = dobj->NbSegments();
    itab[3] = dobj->NbItems();
    bool sdone;
    Units un=dobj->GetUnits(sdone);
    if (sdone)  itab[0] |= (1 << 16);
    os.Put(itab, 4);
    if (sdone)  os<<un;   // ecriture de l'unite si definie
    // On ecrit les donnees par paquets ...
    vector<int_8> postags;
    int_8 tag;
    for(uint_8 k=0; k<itab[2]; k++) {
      tag = os.WritePositionTag();
      PPF_TPointer_IO<T>::Write(os, dobj->GetSegment(k), itab[1]);
      postags.push_back(tag);
    }
    // On ecrit la table des tags de positionnement 
    // au cas ou on voudrait l'utiliser en mode swap
    os.PutPosTagTable(postags);
  }

  // Variables membres 
  SegDataBlock<T> * dobj;  // Le vecteur de la STL 
  bool ownobj;  // true -> objet cree par le wrapper 
};

/*! Writes the segmented data structure \b obj in the POutPersist stream \b os */
template <class T>
inline POutPersist& operator << (POutPersist& os, SegDataBlock<T> & obj)
{ FIO_SegDataBlock<T> fio(&obj);  fio.Write(os);  return(os); }
/*! Reads in and initializes the segmented data structure \b obj  
  from the PInPersist stream \b is */
template <class T>
inline PInPersist& operator >> (PInPersist& is, SegDataBlock<T> & obj)
{ FIO_SegDataBlock<T> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }

} // Fin du namespace

#endif
